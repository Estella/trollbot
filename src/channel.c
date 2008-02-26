#include "main.h"
#include "channel.h"

#include "log_entry.h"
#include "tconfig.h"
#include "irc.h"
#include "util.h"
#include "network.h"
#include "user.h"

struct tconfig_block *chans_to_tconfig(struct channel *chans)
{
	struct channel       *tmp  = NULL;
	struct tconfig_block *tcfg = NULL;
	struct tconfig_block *tpar = NULL;
	char                 *tstr = NULL;
	int                   size = 0;

	tmp = chans;
	
	while (tmp != NULL)
	{
		if (tcfg == NULL)
			tcfg = tconfig_block_new();
		else
		{
			tcfg->next       = tconfig_block_new();
			tcfg->next->prev = tcfg;
			tcfg             = tcfg->next;
		}

		/* Should work out some memory logic to not have to do this with keys */
		tcfg->key   = tstrdup("channel");
		tcfg->value = tstrdup(tmp->name);

		/* Create child, save parent pointer */
		tcfg->child  = tconfig_block_new();
		tpar         = tcfg;
		tcfg         = tcfg->child;
		tcfg->parent = tpar;


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
  if (chan != NULL)
    ret->name = tstrdup(chan);
	else
	{
		troll_debug(LOG_ERROR, "Tried making a new channel without a channel name\n");
		free(ret);
		return NULL;
	}

	ret->topic = NULL;

	/* Eggdrop Vars */
	ret->flood_chan_count = -1;
	ret->flood_chan_sec   = -1;

	ret->flood_deop_count = -1;
	ret->flood_deop_sec   = -1;

	ret->flood_kick_count = -1;
	ret->flood_kick_sec   = -1;

	ret->flood_join_count = -1;
	ret->flood_join_sec   = -1;

	ret->flood_ctcp_count = -1;
	ret->flood_ctcp_sec   = -1;

	ret->flood_nick_count = -1;
	ret->flood_nick_sec   = -1;

	ret->aop_delay_count  = -1;
	ret->aop_delay_sec    = -1;

	ret->idle_kick        = -1;
	
	ret->chanmode         = NULL;

	ret->stopnethack_mode = -1;
	
	ret->revenge_mode     = -1;
	ret->ban_time         = -1;

	ret->exempt_time      = -1;
	ret->invite_time      = -1;

	ret->autoop           = -1;
	ret->bitch            = -1;

	ret->autovoice        = -1;
	ret->cycle            = -1;

	ret->dontkickops      = -1;
	ret->dynamicexempts   = -1;

	ret->enforcebans      = -1;
	ret->greet            = -1;

	ret->dynamicinvites   = -1;
	ret->dynamicbans      = -1;
	ret->enforcebans      = -1;
	ret->revenge          = -1;
	ret->userbans         = -1;
	ret->userinvites      = -1;
	ret->autohalfop       = -1;
	ret->nodesynch        = -1;
	ret->protectops       = -1;
	ret->revengebot       = -1;
	ret->seen             = -1;
	ret->statuslog        = -1;
	ret->userexempts      = -1;
	ret->protecthalfops   = -1;
	ret->statuslog        = -1;
	ret->secret           = -1;
	ret->shared           = -1;

  ret->tcfg = NULL;
  ret->user_list = NULL;

  ret->prev = NULL;
  ret->next = NULL;

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
    if ((numbytes += strlen(tmpchan->name)) > BUFFER_SIZE-3)
      return;

    strcat(joinstr,tmpchan->name);
    strcat(joinstr,",");
    
    tmpchan = tmpchan->next;
  } 

  joinstr[strlen(joinstr)-1] = '\0';
 
  irc_printf(net->sock,"JOIN %s\n",joinstr);
  free(joinstr);
  return;
}

void channel_list_populate(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	struct channel *chan       = NULL;
	struct channel_user *cuser = NULL;
	int i                      = 0;
	char *ptr                  = NULL;

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
	for(i=0;data->rest[i] != NULL;i++)
	{
		/* try to find user first */
		ptr = data->rest[i];

		while (*ptr == '@' || *ptr == '+')
			ptr++;

		if (chan->user_list == NULL)
		{
			chan->user_list = new_channel_user(ptr,time(NULL),NULL);
			cuser           = chan->user_list;
			cuser->prev     = NULL;
		}
		else
		{
			cuser = chan->user_list;

			/* Check for an existing user first */
			while (cuser->prev != NULL) cuser = cuser->prev;

			while (cuser != NULL)
			{
				if (!tstrcasecmp(cuser->nick,ptr))
				{
					/* FIXME: Just update modes if needed and continue */
					break;
				}
				
				cuser = cuser->next;
			}
		
			if (cuser != NULL)
				continue;

			cuser = chan->user_list;

			while (cuser->next != NULL) cuser = cuser->next;
	
			cuser->next = new_channel_user(ptr,time(NULL),NULL);

			cuser->next->prev = cuser;
			cuser->next->next = NULL;			
		}
		
		if (!strncmp(data->rest[i],"@",1))
		{
			/* CHECK MODES */
		}	

	}
}

struct channel *new_chan_from_tconfig_block(struct tconfig_block *tcfg)
{
	struct channel       *chan  = NULL;
	struct tconfig_block *child = NULL;

	if (tcfg == NULL)
	{
		troll_debug(LOG_ERROR, "new_chan_from_tconfig_block() called with NULL argument");
		return NULL;
	}

	if (!strcmp(tcfg->key,"channel"))
	{
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

	log_entry_sprintf(net, NULL, "c", "Saving channel file...");

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
