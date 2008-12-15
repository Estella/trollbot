#include "main.h"
#include "channel.h"

#include "log_entry.h"
#include "tconfig.h"
#include "irc.h"
#include "util.h"
#include "network.h"
#include "user.h"

/* Adds a channel's user (not trollbot user) to the channel's user list */
struct channel_user *channel_user_add(struct channel_user *cusers, struct channel_user *add)
{
	struct channel_user *tmp = NULL;

	if ((tmp = cusers) == NULL)
		return add;

	while (tmp->next != NULL) tmp = tmp->next;

	tmp->next = add;
	add->prev = tmp;

	return cusers;
}

/* Removes a channel's user (not trollbot user) from the channel's user list */
struct channel_user *channel_user_del(struct channel_user *cusers, struct channel_user *del)
{
	struct channel_user *tmp = NULL;

	if ((tmp = cusers) == NULL)
	{
		log_entry_printf(NULL,NULL,"T","channel_user_del() called with NULL channel user list");
		return NULL;
	}

	while (tmp != NULL)
	{
		if (tmp == del)
		{
			if (tmp->prev != NULL)
				tmp->prev->next = tmp->next;

			if (tmp->next != NULL)
				tmp->next->prev = tmp->prev;

			while (tmp == del && tmp->prev != NULL)
				tmp = tmp->prev;

			while (tmp == del && tmp->next != NULL)
				tmp = tmp->next;

			if (tmp == del)
				return NULL;
			else
				return tmp;

		}

		tmp = tmp->next;
	}

	log_entry_printf(NULL,NULL,"T","channel_user_del() called with a channel user deletion that no entry existed for");

	return cusers;
}

/* Frees the entire user list */
void channel_users_free(struct channel_user *cusers)
{
	struct channel_user *walk;
	struct channel_user *tmp;

	walk = cusers;

	while (walk != NULL)
	{
		tmp = walk->next;
		channel_user_free(walk);
		walk = tmp;
	}
}

/* Frees a single channel user */
void channel_user_free(struct channel_user *cuser)
{
	free(cuser->nick);
	free(cuser->ident);

	free(cuser->modes);

  free(cuser->host);

	free(cuser->realname);
	free(cuser);
}

/* These are generic key/values for eggdrop compatibility and script extension */
struct chan_egg_var *new_chan_egg_var(void)
{
	struct chan_egg_var *ret = tmalloc(sizeof(struct chan_egg_var));

	ret->key   = NULL;
	ret->value = NULL;

	return ret;
}

void free_chan_egg_var(void *chan_egg_var_ptr)
{
	/* Ridiculous names, I know */
	struct chan_egg_var *tmp = chan_egg_var_ptr;

	free(tmp->key);
	free(tmp->value);

	free(tmp);
}

struct channel_ban *channel_ban_del(struct channel_ban *bans, struct channel_ban *del)
{
	struct channel_ban *tmp = NULL;

	if ((tmp = bans) == NULL)
	{
		log_entry_printf(NULL,NULL,"T","channel_ban_del() called with NULL banlist");
		return NULL;
	}

	while (tmp != NULL)
	{
		if (tmp == del)
		{
			if (tmp->prev != NULL)
				tmp->prev->next = tmp->next;

			if (tmp->next != NULL)
				tmp->next->prev = tmp->prev;

			while (tmp == del && tmp->prev != NULL)
				tmp = tmp->prev;

			while (tmp == del && tmp->next != NULL)
				tmp = tmp->next;

			if (tmp == del)
				return NULL;
			else
				return tmp;

		}

		tmp = tmp->next;
	}

	log_entry_printf(NULL,NULL,"T","channel_ban_del() called with a ban deletion that no entry existed for");

	return bans;
}

struct channel_ban *channel_ban_add(struct channel_ban *bans, struct channel_ban *add)
{
	struct channel_ban *tmp = NULL;

	if ((tmp = bans) == NULL)
		return add;

	while (tmp->next != NULL) tmp = tmp->next;

	tmp->next = add;
	add->prev = tmp;

	return bans;
}

void channel_bans_free(struct channel_ban *bans)
{
	struct channel_ban *tmp = NULL;
	struct channel_ban *old = NULL;

	tmp = bans;

	while (tmp->prev != NULL) tmp = tmp->prev;

	while (tmp != NULL)
	{
		old = tmp;
		tmp = tmp->next;

		channel_ban_free(old);
	}
}

void channel_ban_free(struct channel_ban *ban)
{
	if (ban == NULL)
	{
		log_entry_printf(NULL,NULL,"T","channel_ban_free() called with NULL ban");
		return;
	}

	free(ban->chan);
	free(ban->mask);
	free(ban->who);
	free(ban->comment);
	free(ban);
}

struct channel_ban *channel_ban_new(void)
{
	struct channel_ban *ret = tmalloc(sizeof(struct channel_ban));

	ret->chan = NULL;
	ret->mask = NULL;
	ret->who  = NULL;

	ret->comment = NULL;

	ret->time        = 0;
	ret->expire_time = 0;
	ret->lifetime    = 0;

	ret->type        = CHANNEL_BAN_UNKNOWN;
	ret->is_sticky   = 0;
	ret->is_perm     = 0;

	ret->prev = NULL;
	ret->next = NULL;

	return ret;
}

struct channel_ban *channel_channel_ban_find(struct channel *chan, const char *find)
{
	struct channel_ban *cban;

	if (chan->banlist == NULL)
		return NULL;

	cban = chan->banlist;

	while (cban != NULL)
	{
		if (!tstrcasecmp(cban->mask, find))
			return cban;

		cban = cban->next;
	}
	
	return NULL;
}

struct tconfig_block *chans_to_tconfig(struct channel *chans)
{
	struct channel       *tmp  = NULL;
	struct tconfig_block *tcfg = NULL;
	struct tconfig_block *tpar = NULL;
	struct tconfig_block *add  = NULL;
	struct slist_node    *node = NULL;
	struct chan_egg_var  *cev  = NULL;

	tmp = chans;

	while (tmp != NULL)
	{
		if (tcfg == NULL)
			tcfg = tconfig_block_new();
		else
		{
			while (tcfg->next != NULL) tcfg = tcfg->next;
			tcfg->next       = tconfig_block_new();
			tcfg->next->prev = tcfg;
			tcfg             = tcfg->next;
		}

		/* Should work out some memory logic to not have to do this with keys */
		tcfg->key   = tstrdup("channel");
		tcfg->value = tstrdup(tmp->name);

		/* Ensure at least one value exists */
		if (tmp->egg_vars == NULL)
		{
			tmp = tmp->next;
			continue;
		}

		/* save parent pointer */
		tpar         = tcfg;

		tcfg         = NULL;

		if (tmp->egg_vars != NULL)
		{
			/* write out all the egg vars */
			node = tmp->egg_vars->head;

			while (node != NULL)
			{
				if (tcfg == NULL)
				{
					tcfg = tconfig_block_new();
					tcfg->parent        = tpar;
					tcfg->parent->child = tcfg;
					add                 = tcfg;
				}
				else
				{
					/* Should operate as if */
					while (tcfg->next != NULL) tcfg = tcfg->next;

					tcfg->next       = tconfig_block_new();
					tcfg->next->prev = tcfg;
					tcfg             = tcfg->next;
					add              = tcfg;
				}

				cev = node->data;

				add->key         = tstrdup(cev->key);
				add->value       = tstrdup(cev->value);

				node = node->next;
			}
		}

		tcfg = tpar;

		tmp  = tmp->next;
	}

	while (tcfg->prev != NULL)
		tcfg = tcfg->prev;

	/* To be returned by caller */
	return tcfg;
}

void channel_list_add(struct channel **orig, struct channel *new)
{
	struct channel *tmp;

	if (*orig == NULL)
	{
		*orig = new;
		new->prev = NULL;
		new->next = NULL;
	}
	else
	{
		tmp = *orig;

		while (tmp->next != NULL)
			tmp = tmp->next;

		tmp->next       = new;
		tmp->next->prev = tmp;
		tmp->next->next = NULL;
	}
}

struct channel_user *new_channel_user(void)
{
	struct channel_user *ret;

	ret = tmalloc(sizeof(struct channel_user));

	ret->nick     = NULL;

	ret->jointime = 0;
	ret->urec     = NULL;
	ret->host     = NULL;
	ret->ident    = NULL;
	ret->realname = NULL;
	ret->modes    = NULL;

	ret->prev     = NULL;
	ret->next     = NULL;

	return ret;
}

void free_channel_user(void *ptr)
{
	struct channel_user *tmp = ptr;

	if (tmp == NULL)
		return;

	free(tmp->nick);
	free(tmp->ident);
	free(tmp->host);
	free(tmp->realname);

	free(tmp->modes);
	
	free(tmp);
}


struct channel_user *channel_channel_user_find(struct channel *chan, const char *find)
{
	struct channel_user *cuser;

	/* write out all the egg vars */
	cuser = chan->user_list;

	while (cuser != NULL)
	{
		if (!tstrcasecmp(cuser->nick, find))
			return cuser;

		cuser = cuser->next;
	}
	
	return NULL;
}

void free_channel(void *chanptr)
{
	struct channel      *ctmp   = chanptr;

	if (ctmp == NULL)
		return;

	free(ctmp->name);

	/* Free egg vars */
	slist_destroy(ctmp->egg_vars);

	channel_users_free(ctmp->user_list);

	free(ctmp);

	return;
}

void free_channels(struct channel *chans)
{
	struct channel   *chantmp=NULL;

	if (chans == NULL)
		return;

	while (chans->prev != NULL)
		chans = chans->prev;

	while (chans != NULL)
	{
		free(chans->name);

		slist_destroy(chans->egg_vars); 

		channel_users_free(chans->user_list);

		free(chans->chanmode);
		free(chans->topic);

		chantmp = chans;
		chans   = chans->next;
		free(chantmp);    
	}

	return;
}


struct channel *new_channel(const char *chan)
{
	struct channel *ret;

	ret = tmalloc(sizeof(struct channel));

	/* Chan should never be NULL but it should be checked nontheless */
	/* Why not? */
	if (chan != NULL)
		ret->name = tstrdup(chan);
	else
	{
		troll_debug(LOG_ERROR, "Tried making a new channel without a channel name\n");
		free(ret);
		return NULL;
	}

	ret->topic     = NULL;

	ret->egg_vars  = NULL;

	ret->chanmode  = NULL;
	ret->topic     = NULL;
	ret->banlist   = NULL;

	ret->tcfg      = NULL;
	ret->user_list = NULL;

	ret->prev      = NULL;
	ret->next      = NULL;

	return ret;
}

void join_channels(struct network *net)
{
	char *joinstr = NULL;
	struct channel *tmpchan;
	int numbytes = 0;

	joinstr = tmalloc0(BUFFER_SIZE);

	if ((tmpchan = net->chans) == NULL)
		return;

	/* "JOIN " */
	numbytes += 5;

	while (tmpchan != NULL)
	{
		/* 3 ? */
		if ((numbytes += strlen(tmpchan->name)) > BUFFER_SIZE-3)
			return;

		/* Why not just sprintf/snprintf? */
		strcat(joinstr,tmpchan->name);
		strcat(joinstr,",");

		tmpchan = tmpchan->next;
	} 

	/* Get rid of the trailing space */
	joinstr[strlen(joinstr)-1] = '\0';

	/* Hopefully all IRCDs support #chan1,#chan2,#chan3, TODO: verify with RFC */
	tsocket_printf(net->tsock,"JOIN %s\n",joinstr);

	/* Toss it like a used condom */
	free(joinstr);

	return;
}

void channel_list_populate(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	struct channel *chan       = NULL;
	struct channel_user *cuser = NULL;
	int i                      = 0;
	int j                      = 0;
	char *ptr                  = NULL;
	char *modes                = NULL;

	/* First check to see if channel exists, if not, create a record. */
	chan = net->chans;

	while (chan->prev != NULL) chan = chan->prev;

	while (chan != NULL)
	{
		if (!tstrcasecmp(chan->name,data->c_params[2]))
			break; /* Found a channel record */

		chan = chan->next;
	}

	/* this is a new channel */
	if (chan == NULL)
	{
		chan = net->chans;
		while (chan->next != NULL) chan = chan->next;

		chan->next       = new_channel(data->c_params[2]);
		chan->next->prev = chan;
	}

	/* 353 toodle @ #java :toodle */
	/* For each user listed after the colon */
	for(i=0;data->rest[i] != NULL;i++)
	{
		/* try to find user first */
		ptr = data->rest[i];

		modes = tmalloc0(strlen(ptr) + 1); /* Guaranteed Maximum size */

		/* All modes under RFC? FIXME: Verify with IRC RFC */
		/* Get all modes, nasty code */
		for (j = 0;*ptr == '@' || *ptr == '+';j++)
		{
			switch (*ptr)
			{
				case '@':
					modes[j] = 'o';
					break;
				case '+':
					modes[j] = 'v';
					break;
				default:
					log_entry_printf(NULL,NULL,"o","Unhandled symbol in channel_list_populate: %c",*ptr);
			}

			ptr++;
		}

		cuser = channel_channel_user_find(chan,ptr);

		if (cuser == NULL)
		{
			cuser = new_channel_user();
			/* nick is how they're found, a sort of primary key, so do not set it when updating */
			cuser->nick = tstrdup(ptr);

			chan->user_list = channel_user_add(chan->user_list, cuser);
		}

		/* In case they're an old user */
		free(cuser->modes);
	
		if ((modes == NULL) || strlen(modes) <= 0)
			cuser->modes = NULL;
		else
			cuser->modes = tstrdup(modes);	

		free(modes);
	}
}

struct channel *new_chan_from_tconfig_block(struct tconfig_block *tcfg)
{
	struct channel       *chan  = NULL;
	struct tconfig_block *child = NULL;
	struct chan_egg_var  *cev   = NULL;

	if (tcfg == NULL)
	{
		troll_debug(LOG_ERROR, "new_chan_from_tconfig_block() called with NULL argument");
		return NULL;
	}

	if (!strcmp(tcfg->key,"channel"))
	{
		if (tcfg->child == NULL)
			return NULL;

		/* New Chan Record */
		chan = new_channel(tcfg->value); 

		child = tcfg->child;

		while (child != NULL)
		{
			/* Do something here
				 if (!strcmp(child->key,"flags"))
				 {
				 if (user->flags == NULL)
				 user->flags = tstrdup(child->value);
				 }*/

			/* at least make egg_var nodes */
			cev = new_chan_egg_var();

			cev->key   = tstrdup(child->key);
			cev->value = tstrdup(child->value);

			if (chan->egg_vars == NULL)
				slist_init(&chan->egg_vars, free_chan_egg_var);

			/* Making a NULL first node? */
			slist_insert_next(chan->egg_vars, NULL, (void *)cev);

			child = child->next;
		}
	}

	return chan;
}

/* Saves chans with optional network */
void chans_save(struct network *net)
{
	struct tconfig_block *tcfg   = NULL;
	char                 *tmpstr = NULL;
	struct network       *nettmp = NULL;

	log_entry_printf(net, NULL, "c", "Saving channel file...");

	if (net != NULL)
	{
		tcfg = chans_to_tconfig(net->chans);

		if (net->chanfile == NULL)
		{
			tmpstr = tmalloc0(strlen("./db/chandb.") + strlen(net->label) + 1);
			sprintf(tmpstr, "./db/chandb.%s",nettmp->label);

			tconfig_to_file(tcfg, tmpstr);

			free(tmpstr);
		}
		else
			tconfig_to_file(tcfg, net->chanfile);

		free_tconfig(tcfg);
		return;
	}

	nettmp = g_cfg->networks;

  if (nettmp == NULL)
  {
    free_tconfig(tcfg);
    return;
  }

	while (nettmp->prev != NULL) nettmp = nettmp->prev;

	while (nettmp != NULL)
	{
		tcfg = chans_to_tconfig(nettmp->chans);

		if (nettmp->chanfile == NULL)
		{
			tmpstr = tmalloc0(strlen("./db/chandb.") + strlen(nettmp->label) + 1);
			sprintf(tmpstr, "./db/chandb.%s",nettmp->label);

			tconfig_to_file(tcfg, tmpstr);

			free(tmpstr);
		}
		else
			tconfig_to_file(tcfg, nettmp->chanfile);

		free_tconfig(tcfg);

		nettmp = nettmp->next;
	}

	return;
}

void chan_init(void)
{
	struct network       *net      = NULL;
	struct tconfig_block *chantcfg = NULL;
	struct tconfig_block *defaults = NULL;
	struct tconfig_block *tmp      = NULL;
	struct channel       *chan     = NULL;
	struct channel       *tmpchan  = NULL;

	net  = g_cfg->networks;

	while (net != NULL)
	{
		if (net->chanfile != NULL)
		{
			/* The idea is to read the chanfile, parse the
			 * returned data into the internal format, attach
			 * location of tcfg entry, and when saved, the
			 * bot will write out this tcfg. New and deleted
			 * users will have to be mirrored in the tcfg.
			 * forget that last part.
			 */
			chantcfg = file_to_tconfig(net->chanfile);
			defaults = file_to_tconfig("db/chandb.defaults");

			if (defaults != NULL)
			{
				tconfig_merge(defaults,chantcfg);
				free_tconfig(defaults);
			}

			tmp = chantcfg;

			while (tmp != NULL)
			{
				if (tmp->key == NULL)
					break;
				if (!strcmp(tmp->key,"channel"))
				{
					chan = new_chan_from_tconfig_block(tmp);

					if (chan != NULL)
					{
						chan->tcfg = tmp;

						/* See if it already exists first */
						tmpchan = net->chans;

						while (tmpchan != NULL)
						{
							if (!tstrcasecmp(tmpchan->name,chan->name))
								break;

							tmpchan = tmpchan->next;
						}

						if (tmpchan != NULL)
						{
							free_channels(chan);
							tmp = tmp->next;								
							continue;
						}

						tmpchan = net->chans;

						/* link it into the networks shit */
						if ((tmpchan = net->chans) == NULL)
						{
							net->chans    = chan;
							tmpchan       = chan;

							tmpchan->prev = NULL;
						}
						else
						{	
							while (tmpchan->next != NULL)
								tmpchan       = tmpchan->next;

							tmpchan->next = chan;
							chan->prev    = tmpchan;	
						}
					}
				}

				tmp = tmp->next;
			}

			/* Why the child? */
			tconfig_merge(chantcfg, net->tcfg->child);
			free_tconfig(chantcfg);
		}


		net = net->next;
	}

	return;
}
