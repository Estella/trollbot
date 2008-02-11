#include "main.h"
#include "channel.h"

#include "tconfig.h"
#include "irc.h"
#include "util.h"
#include "network.h"
#include "user.h"

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

void chan_init(void)
{
  struct network *net;

  net  = g_cfg->networks;

  while (net != NULL)
  {
    if (net->chanfile != NULL)  
    {
      /* FIXME */
      /* g_cfg->tcfg = file_to_tconfig(net->chanfile); */
    }

    net = net->next;
  }

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
