#include "main.h"
#include "channel.h"

#include "log_entry.h"
#include "tconfig.h"
#include "irc.h"
#include "util.h"
#include "network.h"
#include "user.h"

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
	free(ban);
}

struct channel_ban *channel_ban_new(void)
{
	struct channel_ban *ret = tmalloc(sizeof(struct channel_ban));

	ret->chan = NULL;
	ret->mask = NULL;
	ret->who  = NULL;

	ret->time = 0;

	ret->prev = NULL;
	ret->next = NULL;

	return ret;
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

		/*if (tmp->nick != NULL)
			{
			*//* Nick */
		/*tcfg->key        = tstrdup("nick");
			tcfg->value      = tstrdup(tmp->nick);
			tcfg->next       = tconfig_block_new();
			tcfg->next->prev = tcfg;
			tcfg             = tcfg->next;
			}

			if (tmp->flags != NULL)
			{*/
		/* flags */
		/*tcfg->key   = tstrdup("flags");
			tcfg->value = tstrdup(tmp->flags);
			}*/


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

void channel_user_del(struct channel_user **orig, const char *nick)
{
	struct channel_user *tmp = *orig;

	while (tmp != NULL)
	{
		if (!tstrcasecmp(tmp->nick,nick))
		{
			/* This is the one that needs removed */
			free(tmp->nick);
			free(tmp->uhost);
			free(tmp->ident);
			free(tmp->modes);

			if (tmp->prev != NULL)
				tmp->prev->next = tmp->next;

			if (tmp->next != NULL)
				tmp->next->prev = tmp->prev;

			free(tmp);

			return;
		}

		tmp = tmp->next;
	}

	if (tmp == NULL)
	{
		troll_debug(LOG_ERROR, "channel_user_del() called with non-existing nickname\n");
		return;
	}
}

void channel_user_add(struct channel_user **orig, struct channel_user *new)
{
	struct channel_user *tmp;      

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

struct channel_user *new_channel_user(const char *nick, int jointime, struct user *urec)
{
	struct channel_user *ret;

	ret = tmalloc(sizeof(struct channel_user));

	ret->nick     = (nick != NULL)     ? tstrdup(nick)     : NULL;

	ret->jointime = jointime;
	ret->urec     = urec;
	ret->uhost    = NULL;
	ret->ident    = NULL;
	ret->modes    = NULL;


	ret->prev     = NULL;
	ret->next     = NULL;

	return ret;
}

void free_channel(void *chanptr)
{
	struct channel      *ctmp   = chanptr;
	struct channel_user *cusers = NULL;
	struct channel_user *cutmp  = NULL;

	if (ctmp == NULL)
		return;

	free(ctmp->name);

	/* Free egg vars */
	slist_destroy(ctmp->egg_vars);

	cusers = ctmp->user_list;

	/* Next for slist? */
	if (cusers != NULL)
		while (cusers->prev != NULL) cusers = cusers->prev;

	while (cusers != NULL)
	{
		cutmp = cusers->next;

		free(cusers->nick);
		free(cusers->uhost);
		free(cusers->ident);
		free(cusers);

		cusers = cutmp;
	}

	free(ctmp);

	return;
}

void free_channels(struct channel *chans)
{
	struct channel_user *cusers=NULL;
	struct channel_user *cusertmp=NULL;
	struct channel   *chantmp=NULL;

	if (chans == NULL)
		return;

	while (chans->prev != NULL)
		chans = chans->prev;

	while (chans != NULL)
	{
		free(chans->name);

		slist_destroy(chans->egg_vars); 

		cusers  = chans->user_list;

		if (cusers != NULL)
		{
			while (cusers->prev != NULL)
				cusers = cusers->prev;

			while (cusers != NULL)
			{
				free(cusers->nick);
				free(cusers->uhost);
				free(cusers->ident);
				cusertmp = cusers;
				cusers = cusers->next;
				free(cusertmp);
			}
		}

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

	/* Hopefully all IRCDs support #chan1,#chan2,#chan3, verify with RFC */
	irc_printf(net->sock,"JOIN %s\n",joinstr);

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

		modes = tmalloc0(strlen(ptr + 1)); /* Guaranteed Maximum size */

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

		/* Create the userlist if it doesn't exist already, if it does, just continue */
		if (chan->user_list == NULL)
		{
			chan->user_list = new_channel_user(ptr,time(NULL),NULL);
			cuser           = chan->user_list;
			cuser->prev     = NULL;
		}
		else
		{
			/* Each channel has its own userlist, they are linked 
			 * afterwards to actual user entries if they match a
			 * preset mask.
			 */
			cuser = chan->user_list;

			/* Check for an existing user first */
			while (cuser->prev != NULL) cuser = cuser->prev;

			while (cuser != NULL)
			{
				if (!tstrcasecmp(cuser->nick,ptr))
				{
					if (cuser->modes != NULL)
						free(cuser->modes);

					cuser->modes = (strlen(modes) >= 1) ? tstrdup(modes) : NULL;

					free(modes);
					modes = NULL;

					break;
				}

				cuser = cuser->next;
			}

			if (cuser != NULL)
				continue;

			cuser = chan->user_list;

			while (cuser->next != NULL) cuser = cuser->next;

			/* at tail */
			cuser->next = new_channel_user(ptr,time(NULL),NULL);

			cuser->next->prev = cuser;

			cuser = cuser->next;
		}

		if (cuser->modes != NULL)
			free(cuser->modes);

		cuser->modes = (strlen(modes) >= 1) ? tstrdup(modes) : NULL;

		free(modes);
		modes = NULL;

	}
}

struct channel *new_chan_from_tconfig_block(struct tconfig_block *tcfg)
{
	struct channel       *chan  = NULL;
	struct tconfig_block *child = NULL;
	struct chan_egg_var  *cev   = NULL;
	struct slist         *list  = NULL;

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
