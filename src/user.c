#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "config_engine.h"
#include "tconfig.h"
#include "util.h"
#include "user.h"

struct user *new_user(char *nick, char *passhash, char *ident, char *realname, char *host, char *flags)
{
  struct user *ret;

  ret = tmalloc(sizeof(struct user));
  
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
  struct tconfig_block *top;
  struct tconfig_block *tnet;
  struct tconfig_block *tnick;
  struct network *net;

  tcfg = file_to_tconfig("userdb");

  if (tcfg == NULL)
    return;

  /* Should be here already, check won't hurt */
  while (tcfg->parent != NULL || tcfg->prev != NULL)
    tcfg = (tcfg->parent == NULL) ? tcfg->prev : tcfg->parent;

  top = tcfg;
  
  do
  {
    if (!strcmp(top->key,"network"))
    {
      net = g_cfg->network_head;

      do
      {
        if (!strcmp(net->label,top->value))
          break;
      } while ((net = net->next) != NULL);

      if (net == NULL)
        continue;

      /* Not a known network, ignore it */
      if (strcmp(net->label,top->value))
        continue;

      if (top->child == NULL)
        continue;

      tnet = top->child;

      /* Scan the users now */
      do
      {
        if (!strcmp(tnet->key,"user"))
        {
          
          tnick = tnet;

          do
          {
            printf("Found nick: %s for network %s\n",tnick->value,top->value);
          } while ((tnick = tnick->next) != NULL);
        }
      } while ((tnet = tnet->next) != NULL);
  
    }
  } while ((top = top->next) != NULL);
      
  exit(EXIT_FAILURE);

  return;
}

