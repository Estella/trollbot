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
#include "sha1.h"
#include "util.h"

static void do_join_channels(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);

void add_default_triggers(void)
{
  struct network *net;

  net = g_cfg->networks;

  while (net != NULL)
  {
    /* These should be in the CTCP bind, but MSG for now */
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"\001VERSION\001",NULL,&return_ctcp_version));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"\001CHAT\001",NULL,&reverse_dcc_chat));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"\001JOIN\001",NULL,&do_join_channels));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"\001PING\001",NULL,&return_ctcp_ping));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"\001TIME\001",NULL,&return_ctcp_time));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"\001DCC CHAT",NULL,&initiate_dcc_chat));
 
    /* BIND MSG */
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"pass",NULL,&new_user_pass));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"ident",NULL,&check_user_pass));
    trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,"hello",NULL,&introduce_user));

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
    /* trigger_list_add(&net->trigs->dcc,new_trigger(NULL,TRIG_DCC,".rehash",NULL,&dcc_rehash)); */

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

    net = net->next;
  }
}

static void do_join_channels(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  join_channels(net);
}

void new_join(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  /* Here we need to create a channel_user in the current channel struct */

}

void new_part(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  /* Here we need to free a channel_user from the current channel struct */
}

void new_quit(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  /* Here we need to remove the channel_user from every channel struct */
}

void new_kick(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  /* if bot was kicked, try a rejoin immediately */
  if (!strcmp(data->c_params[1],net->nick))
    irc_printf(net->sock,"JOIN %s",data->c_params[0]);

}

void new_user_pass(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  struct user *user;
  SHA1_CTX context;

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
    return user->passhash;

  SHA1Init(&context);
  SHA1Update(&context, (unsigned char *)data->rest[1], strlen(data->rest[1]));

  user->passhash = tmalloc0(21);
 
  SHA1Final(user->passhash, &context);

  irc_printf(net->sock,"PRIVMSG %s :Your password has been set as '%s'",data->prefix->nick,data->rest[1]);

  return;
}

void check_user_pass(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
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

    if (!strcmp(user,data->prefix->nick))
    {
      irc_printf(net->sock,"PRIVMSG %s :I'm sorry hal, another user already exists by that nick",data->prefix->nick);
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
