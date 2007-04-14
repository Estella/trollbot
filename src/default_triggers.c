#include <time.h>

#include "main.h"
#include "default_triggers.h"

#include "dcc.h"
#include "irc.h"
#include "network.h"
#include "trigger.h"
#include "user.h"
#include "sha1.h"

void add_default_triggers(void)
{
  struct network *net;

  net = g_cfg->networks;

  while (net != NULL)
  {
    /* CTCP VERSION */
    if (net->trigs->msg == NULL)
    {
      net->trigs->msg       = new_trigger(NULL,TRIG_MSG,"\001VERSION\001",NULL,&return_ctcp_version);
      net->trigs->msg->prev = NULL;
      net->trigs->msg_head  = net->trigs->msg;
      net->trigs->msg_tail  = net->trigs->msg;
    }
    else
    {
      net->trigs->msg_tail->next = new_trigger(NULL,TRIG_MSG,"\001VERSION\001",NULL,&return_ctcp_version);
      net->trigs->msg            = net->trigs->msg_tail->next;
      net->trigs->msg->prev      = net->trigs->msg_tail;
      net->trigs->msg_tail       = net->trigs->msg;
    }

    /* CTCP PING */
    net->trigs->msg_tail->next = new_trigger(NULL,TRIG_MSG,"\001PING\001",NULL,&return_ctcp_ping);
    net->trigs->msg            = net->trigs->msg_tail->next;
    net->trigs->msg->prev      = net->trigs->msg_tail;
    net->trigs->msg_tail       = net->trigs->msg;

    /* CTCP TIME */
    net->trigs->msg_tail->next = new_trigger(NULL,TRIG_MSG,"\001TIME\001",NULL,&return_ctcp_time);
    net->trigs->msg            = net->trigs->msg_tail->next;
    net->trigs->msg->prev      = net->trigs->msg_tail;
    net->trigs->msg_tail       = net->trigs->msg;

    /* CTCP DCC CHAT */
    net->trigs->msg_tail->next = new_trigger(NULL,TRIG_MSG,"\001DCC CHAT",NULL,&initiate_dcc_chat);
    net->trigs->msg            = net->trigs->msg_tail->next;
    net->trigs->msg->prev      = net->trigs->msg_tail;
    net->trigs->msg_tail       = net->trigs->msg;

    /* MSG PASS - to change a user's pass [user] <pass> */
    net->trigs->msg_tail->next = new_trigger(NULL,TRIG_MSG,"pass",NULL,&new_user_pass);
    net->trigs->msg            = net->trigs->msg_tail->next;
    net->trigs->msg->prev      = net->trigs->msg_tail;
    net->trigs->msg_tail       = net->trigs->msg;

    /* MSG IDENT - to identify as a user ident [user] <pass> */
    net->trigs->msg_tail->next = new_trigger(NULL,TRIG_MSG,"ident",NULL,&check_user_pass);
    net->trigs->msg            = net->trigs->msg_tail->next;
    net->trigs->msg->prev      = net->trigs->msg_tail;
    net->trigs->msg_tail       = net->trigs->msg;

    /* MSG HELLO - to introduce permanent users to the bot */
    net->trigs->msg_tail->next = new_trigger(NULL,TRIG_MSG,"hello",NULL,&introduce_user);
    net->trigs->msg            = net->trigs->msg_tail->next;
    net->trigs->msg->prev      = net->trigs->msg_tail;
    net->trigs->msg_tail       = net->trigs->msg;



    /* To add incoming users to chan lists and to know when to use NAMES */
    if (net->trigs->join == NULL)
    {
      net->trigs->join       = new_trigger(NULL,TRIG_JOIN,"*",NULL,&new_join);
      net->trigs->join->prev = NULL;
      net->trigs->join_head  = net->trigs->join;
      net->trigs->join_tail  = net->trigs->join;
    }
    else
    {
      net->trigs->join_tail->next = new_trigger(NULL,TRIG_JOIN,"*",NULL,&new_join);
      net->trigs->join            = net->trigs->join_tail->next;
      net->trigs->join->prev      = net->trigs->join_tail;
      net->trigs->join_tail       = net->trigs->join;
    }

    /* To remove users from chan lists when they part */
    if (net->trigs->part == NULL)
    {
      net->trigs->part       = new_trigger(NULL,TRIG_PART,"*",NULL,&new_part);
      net->trigs->part->prev = NULL;
      net->trigs->part_head  = net->trigs->part;
      net->trigs->part_tail  = net->trigs->part;
    }
    else
    {
      net->trigs->part_tail->next = new_trigger(NULL,TRIG_PART,"*",NULL,&new_part);
      net->trigs->part            = net->trigs->part_tail->next;
      net->trigs->part->prev      = net->trigs->part_tail;
      net->trigs->part_tail       = net->trigs->part;
    }

    /* To remove users from chan lists when they quit */
    if (net->trigs->sign == NULL)
    {
      net->trigs->sign       = new_trigger(NULL,TRIG_SIGN,"*",NULL,&new_quit);
      net->trigs->sign->prev = NULL;
      net->trigs->sign_head  = net->trigs->sign;
      net->trigs->sign_tail  = net->trigs->sign;
    }
    else
    {
      net->trigs->sign_tail->next = new_trigger(NULL,TRIG_SIGN,"*",NULL,&new_quit);
      net->trigs->sign            = net->trigs->sign_tail->next;
      net->trigs->sign->prev      = net->trigs->sign_tail;
      net->trigs->sign_tail       = net->trigs->sign;
    }

    /* To remove users from chan lists when they are kicked */
    if (net->trigs->kick == NULL)
    {
      net->trigs->kick       = new_trigger(NULL,TRIG_KICK,"*",NULL,&new_kick);
      net->trigs->kick->prev = NULL;
      net->trigs->kick_head  = net->trigs->kick;
      net->trigs->kick_tail  = net->trigs->kick;
    }
    else
    {
      net->trigs->kick_tail->next = new_trigger(NULL,TRIG_KICK,"*",NULL,&new_kick);
      net->trigs->kick            = net->trigs->kick_tail->next;
      net->trigs->kick->prev      = net->trigs->kick_tail;
      net->trigs->kick_tail       = net->trigs->kick;
    }

    net = net->next;
  }
}

void new_join(struct network *net, struct trigger *trig, struct irc_data *data)
{
}

void new_part(struct network *net, struct trigger *trig, struct irc_data *data)
{
}

void new_quit(struct network *net, struct trigger *trig, struct irc_data *data)
{
}

void new_kick(struct network *net, struct trigger *trig, struct irc_data *data)
{
  /* Auto Rejoin */
  if (!strcmp(data->c_params[1],net->nick))
    irc_printf(net->sock,"JOIN %s",data->c_params[0]);
}

void new_user_pass(struct network *net, struct trigger *trig, struct irc_data *data)
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

  SHA1Init(&context);
  SHA1Update(&context, (unsigned char *)data->rest[1], strlen(data->rest[1]));

  user->passhash = tmalloc0(21);
 
  SHA1Final(user->passhash, &context);

}

void check_user_pass(struct network *net, struct trigger *trig, struct irc_data *data)
{
}

void introduce_user(struct network *net, struct trigger *trig, struct irc_data *data)
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

    while (user->next != NULL)   
      user = user->next;

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
  
  irc_printf(net->sock,"PRIVMSG %s :Welcome to trollbot, your username is '%s'",data->prefix->nick,data->prefix->nick);
  irc_printf(net->sock,"PRIVMSG %s :Type '/msg %s pass <your new password>' to continue",data->prefix->nick,net->nick);
}

void return_ctcp_ping(struct network *net, struct trigger *trig, struct irc_data *data)
{
  irc_printf(net->sock,"NOTICE %s :\001PING %d\001",data->prefix->nick,time(NULL));
  return;
}

void return_ctcp_time(struct network *net, struct trigger *trig, struct irc_data *data)
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

void return_ctcp_version(struct network *net, struct trigger *trig, struct irc_data *data)
{
  irc_printf(net->sock,"NOTICE %s :\001VERSION Trollbot v1.0 by poutine\001",data->prefix->nick);

  return;
}
