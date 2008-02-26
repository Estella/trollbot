#include <time.h>

#include "config.h"

#include "main.h"
#include "default_triggers.h"

#include "dcc.h"
#include "irc.h"
#include "network.h"
#include "channel.h"
#include "trigger.h"
#include "user.h"
#include "util.h"
#include "egg_lib.h"
#include "log_filter.h"
#include "log_entry.h"

#ifdef HAVE_JS
#include "js_embed.h"
#endif /* HAVE_JS */

static void channel_set_topic(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
static void dcc_saveall(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
static void dcc_dump(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
static void do_join_channels(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
static void rehash_bot(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);

void add_default_triggers(void)
{
  struct network *net;

  net = g_cfg->networks;

  while (net != NULL)
  {
    /* These should be in the CTCP bind, but MSG for now */
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"\001VERSION",NULL,&return_ctcp_version));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"\001CHAT",NULL,&reverse_dcc_chat));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"\001JOIN",NULL,&do_join_channels));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"\001PING",NULL,&return_ctcp_ping));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"\001TIME",NULL,&return_ctcp_time));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"\001DCC CHAT",NULL,&initiate_dcc_chat));
 
    /* BIND MSG */
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"pass",NULL,&new_user_pass));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"ident",NULL,&check_user_pass));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"hello",NULL,&introduce_user));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"goodbye",NULL,&disconnect_bot));

    /* BIND JOIN */
    trigger_list_add(&net->trigs->join,new_trigger(NULL,TRIG_JOIN,"*",NULL,&new_join));

    /* BIND PART */
    trigger_list_add(&net->trigs->part,new_trigger(NULL,TRIG_PART,"*",NULL,&new_part));

    /* BIND SIGN */
    trigger_list_add(&net->trigs->sign,new_trigger(NULL,TRIG_SIGN,"*",NULL,&new_quit));
     

    /* BIND KICK */
    trigger_list_add(&net->trigs->kick,new_trigger(NULL,TRIG_KICK,"*",NULL,&new_kick));
    
    /* BIND DCC */
    trigger_list_add(&net->trigs->dcc,new_trigger(NULL,TRIG_DCC,".help",NULL,&dcc_help_menu));
    trigger_list_add(&net->trigs->dcc,new_trigger(NULL,TRIG_DCC,".+chan",NULL,&dcc_add_chan));
    trigger_list_add(&net->trigs->dcc,new_trigger(NULL,TRIG_DCC,".-chan",NULL,&dcc_del_chan));
    trigger_list_add(&net->trigs->dcc,new_trigger(NULL,TRIG_DCC,".tbinds",NULL,&dcc_tbinds));
    trigger_list_add(&net->trigs->dcc,new_trigger(NULL,TRIG_DCC,".who",NULL,&dcc_who));
		trigger_list_add(&net->trigs->dcc,new_trigger(NULL,TRIG_DCC,".dump",NULL,&dcc_dump));
		trigger_list_add(&net->trigs->dcc,new_trigger(NULL,TRIG_DCC,".saveall",NULL,&dcc_saveall));
		trigger_list_add(&net->trigs->dcc,new_trigger(NULL,TRIG_DCC,".rehash",NULL,&rehash_bot));

#ifdef HAVE_JS
		trigger_list_add(&net->trigs->dcc,new_trigger(NULL,TRIG_DCC,".loadjavascript",NULL,&dcc_javascript_load));
#endif /* HAVE_JS */

#ifdef HAVE_TCL
    /* OWNER only 
    trigger_list_add(&net->trigs->dcc,new_trigger("n",TRIG_DCC,".tcl",NULL,&dcc_tcl));*/
#endif /* HAVE_TCL */

#ifdef HAVE_PHP
    /* OWNER only
    trigger_list_add(&net->trigs->dcc,new_trigger("n",TRIG_DCC,".php",NULL,&dcc_php));*/
#endif /* HAVE_PHP */


    /* BIND RAW */
    trigger_list_add(&net->trigs->raw,new_trigger(NULL,TRIG_RAW,"353",NULL,&channel_list_populate));
		trigger_list_add(&net->trigs->raw,new_trigger(NULL,TRIG_RAW,"332",NULL,&channel_set_topic));

    net = net->next;
  }
}

static void channel_set_topic(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	struct channel *ctmp = NULL;
	/* Should check irc_data to see whether we have this info */
	
	ctmp = net->chans;

	while (ctmp != NULL)
	{
		if (!tstrcasecmp(ctmp->name,data->c_params[1]))
			break;

		ctmp = ctmp->next;
	}

	if (ctmp == NULL)
	{
		troll_debug(LOG_ERROR, "channel_set_topic() called when I'm not even in that channel");
		return;
	}

	if (ctmp->topic != NULL)
		free(ctmp->topic);

	ctmp->topic = tstrdup(data->rest_str);

	return;
}

static void dcc_saveall(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	egg_save(NULL);
}

static void dcc_dump(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	irc_printf(net->sock, "%s", egg_makearg(dccbuf,trig->mask));
}

static void rehash_bot(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  egg_rehash();
}

static void do_join_channels(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  join_channels(net);
}

void new_join(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	struct channel *chan          = net->chans;
	struct channel_user *cuser    = NULL;

	log_entry_sprintf(net,"j","%s joined %s.",data->prefix->nick,data->rest_str);
	
	/* HACKERY */
	if (chan != NULL)
		while(chan->prev != NULL) chan = chan->prev;

	while (chan != NULL)
	{
		if (!tstrcasecmp(data->rest_str,chan->name))
		{
			if (chan->user_list == NULL)
			{
				/* Add a new channel user with a jointime of now. */
				chan->user_list = new_channel_user(data->prefix->nick, time(NULL), NULL);
				cuser           = chan->user_list;
				cuser->prev     = NULL;
			}
			else
			{
				cuser = chan->user_list;
			
				while(cuser->next != NULL)
					cuser = cuser->next;

				cuser->next       = new_channel_user(data->prefix->nick, time(NULL), NULL);
				cuser->next->prev = cuser;
	
				cuser             = cuser->next;
			}
				

			/*cuser->nick  = tstrdup(data->prefix->nick); --Done in new_channel_user */
			cuser->uhost = tstrdup(data->prefix->host); 
			cuser->ident = tstrdup(data->prefix->user);
			/* FIXME: Need check for whether they're a recognized user in the bot */
			/* FIXME: Check if nick is bot, if so do WHO, and MODE */
				
			cuser->next = NULL;
				
			return;
		
		}
	
		chan = chan->next;
	}

	/* FIXME: If chan is NULL, might be a freenode-like redirection. Handle it. */
	if (chan == NULL)
	{
		troll_debug(LOG_ERROR,"Yeah, that place in the code that was supposed to never be reached, umm we got here\n");
		return;
	}

	return;

}

void new_part(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  struct channel *chan       = net->chans;

	log_entry_sprintf(net,"j","%s parted %s.",data->prefix->nick,data->c_params[0]);

  while (chan != NULL)
  {
    if (!tstrcasecmp(data->rest_str,chan->name))
    {
      if (chan->user_list == NULL)
			{
				troll_debug(LOG_ERROR, "Got a part message for %s, but no user exists on the channel by that name",data->prefix->nick);
				return;
			}

			/* Found the bugger */
			channel_user_del(&chan->user_list, data->prefix->nick);

      return;
    }

    chan = chan->next;
  }

  /* FIXME: If chan is NULL, might be a freenode-like redirection. Handle it. */
  if (chan == NULL)
  {
    troll_debug(LOG_ERROR,"Yeah, that place in the code that was supposed to never be reached, umm we got here\n");
    return;
  }

  return;
}

void new_quit(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  struct channel *chan       = net->chans;

	log_entry_sprintf(net,"j","%s quit irc",data->prefix->nick);


  while (chan != NULL)
  {
		if (egg_onchan(net, data->prefix->nick, chan->name)){
			/*channel_list_del(&chan->user_list, data->prefix->nick);*/
    }

    chan = chan->next;
  }


  return;
}

void new_kick(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  /* if bot was kicked, try a rejoin immediately */
	/* Verified eggdrop behavior, there is no setting for this [acidtoken] */
  if (!strcmp(data->c_params[1],net->nick))
    irc_printf(net->sock,"JOIN %s",data->c_params[0]);

	log_entry_sprintf(net,"k","%s Got kicked from <FILL ME IN>",data->prefix->nick);
}

void new_user_pass(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  struct user *user;

  /* Wrong amount of args */  
  if (data->rest[1] == NULL)
    return;

  user = net->users;

  while (user != NULL)
  {
    if (!strcmp(data->prefix->nick,user->nick))
      break;

    user = user->next;
  }

  if (user == NULL)
  {
    irc_printf(net->sock,"PRIVMSG %s :I don't know you",data->prefix->nick);
    return;
  }

  if (user->passhash != NULL)
    return;

	user->passhash = egg_makepasswd(data->rest[1],g_cfg->hash_type);

  irc_printf(net->sock,"PRIVMSG %s :Your password has been set as '%s'",data->prefix->nick,data->rest[1]);

	users_save(net);
  return;
}

void check_user_pass(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
}

void disconnect_bot(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
		egg_save(NULL);
		irc_printf(net->sock,"QUIT :Trollbot v1.0");
    /*die_nicely(0); */
}

void introduce_user(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  struct user *user;

  if (net->users == NULL)
  {
    net->users = new_user(data->prefix->nick,  /* username */
                          data->prefix->nick,  /* nickname */
                          NULL,                /* passhash */
                          data->prefix->user,  /* ident    */
                          NULL,                /* realname */
                          data->prefix->host,  /* hostname */
                          "p");                /* flags    */

    user       = net->users;

    user->prev = NULL;
    user->next = NULL;
  }
  else
  {
    user       = net->users;

    while (user->next != NULL && strcmp(user->username,data->prefix->nick))  
      user = user->next;

    if (!strcmp(user->nick,data->prefix->nick))
    {
      irc_printf(net->sock,"PRIVMSG %s :Another user already exists by that nick",data->prefix->nick);
      return;
    }

    user->next       = new_user(data->prefix->nick,  /* username */
                                data->prefix->nick,  /* nickname */
                                NULL,                /* passhash */
                                data->prefix->user,  /* ident    */
                                NULL,                /* realname */
                                data->prefix->host,  /* hostname */
                                "p");                /* flags    */

    user->next->prev = user;
    user             = user->next;


    user->next       = NULL;
  }

	user->hash_type = tstrdup(g_cfg->hash_type);
  user->uhost = tmalloc0(strlen(data->prefix->user) + strlen(data->prefix->nick) + strlen(data->prefix->host) + 2 + 1);

  sprintf(user->uhost,"%s!%s@%s",data->prefix->user,data->prefix->nick,data->prefix->host);
  
  irc_printf(net->sock,"PRIVMSG %s :Welcome to trollbot, your username is '%s'",data->prefix->nick,data->prefix->nick);
  irc_printf(net->sock,"PRIVMSG %s :Type '/msg %s pass <your new password>' to continue",data->prefix->nick,net->nick);
}

void return_ctcp_ping(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  irc_printf(net->sock,"NOTICE %s :\001PING %d\001",data->prefix->nick,time(NULL));
  return;
}

void return_ctcp_time(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  char tdate[100];
  struct tm *now;
  time_t ntime;

  ntime = time(NULL);

  memset(tdate,0,sizeof(tdate));

  now = localtime(&ntime);

  strftime(tdate,100,"%x %X",now);

  irc_printf(net->sock,"NOTICE %s :\001TIME %s\001",data->prefix->nick,tdate);
}

void return_ctcp_version(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  irc_printf(net->sock,"NOTICE %s :\001VERSION Trollbot v1.0.0 by poutine\001",data->prefix->nick);

  return;
}
