#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "config_engine.h"
#include "tconfig.h"
#include "util.h"
#include "user.h"

struct user *new_user(char *username, char *nick, char *passhash, char *ident, char *realname, char *host, char *flags)
{
  struct user *ret;

  ret = tmalloc(sizeof(struct user));

  ret->username = (username != NULL) ? tstrdup(username) : NULL;  
  ret->nick     = (nick != NULL)     ? tstrdup(nick)     : NULL;
  ret->passhash = (passhash != NULL) ? tstrdup(passhash) : NULL;
  ret->ident    = (ident != NULL)    ? tstrdup(ident)    : NULL;
  ret->realname = (realname != NULL) ? tstrdup(realname) : NULL;
  ret->host     = (host != NULL)     ? tstrdup(host)     : NULL;
  ret->flags    = (flags != NULL)    ? tstrdup(flags)    : NULL;

  ret->prev = NULL;
  ret->next = NULL;

  return ret;
}

struct channel_flags *new_channel_flags(char *chan, char *flags)
{
  struct channel_flags *ret;

  ret = tmalloc(sizeof(struct channel_flags));

  ret->chan  = (chan != NULL)  ? tstrdup(chan)  : NULL;
  ret->flags = (flags != NULL) ? tstrdup(flags) : NULL;

  ret->prev = NULL;
  ret->next = NULL;

  return ret;
}


void user_init(void)
{
  struct tconfig_block *tcfg;
  struct tconfig_block *tnet;
  struct tconfig_block *tuser;
  struct tconfig_block *tparams;
  struct user *user;
  struct network *net;

  tcfg = file_to_tconfig("userdb");

  if (tcfg == NULL)
    return;

  /* Should be here already, check won't hurt */
  while (tcfg->parent != NULL || tcfg->prev != NULL)
    tcfg = (tcfg->parent == NULL) ? tcfg->prev : tcfg->parent;

  tnet = tcfg;
  
  do
  {
    if (!strcmp(tnet->key,"network"))
    {
      net = g_cfg->network_head;

      do
      {
        if (!strcmp(net->label,tnet->value))
          break;
      } while ((net = net->next) != NULL);

      if (net == NULL)
        continue;

      if (tnet->child == NULL)
        continue;

      tuser = tnet->child;

      /* Scan the users now */
      do
      {
        if (!strcmp(tuser->key,"user"))
        {
          if (net->users_head == NULL)
          {
            net->users_head  = new_user(tuser->key,NULL,NULL,NULL,NULL,NULL,NULL);
            net->users       = net->users_head;
            net->users_tail  = net->users_head;
            net->users->prev = NULL;
            net->users->next = NULL;
          } 
          else
          {
            net->users_tail->next = new_user(tuser->key,NULL,NULL,NULL,NULL,NULL,NULL);
            net->users            = net->users_tail->next;
            net->users->prev      = net->users_tail;
            net->users->next      = NULL;
            net->users_tail       = net->users;
          }
         
          user = net->users;

          tparams = tuser->child;

          do
          {
            if (!strcmp(tparams->key,"nick"))
            {
              if (user->nick != NULL)
                free(user->nick);
     
              user->nick = tstrdup(tparams->key);
            }
            else if (!strcmp(tparams->key,"passhash"))
            {
              if (user->passhash != NULL)
                free(user->passhash);

              user->passhash = tstrdup(tparams->key);
            }
            else if (!strcmp(tparams->key,"flags"))
            {
              if (user->flags != NULL)
                free(user->flags);

              user->flags = tstrdup(tparams->key);
            }
          } while ((tparams = tparams->next) != NULL);
        }
      } while ((tuser = tuser->next) != NULL);  
    }
  } while ((tnet = tnet->next) != NULL);

  /* Free up all tconfig related stuff */
  /* free_tconfig(tcfg); */
      
  return;
}

