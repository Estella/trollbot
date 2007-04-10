#include "main.h"
#include "network.h"
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

  ret->tindex = NULL;
  ret->prev   = NULL;
  ret->next   = NULL;

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
  struct network *net;

  net  = g_cfg->networks;

  while (net != NULL)
  {
    if (net->userfile != NULL)
      g_cfg->tcfg = file_to_tconfig(g_cfg->tcfg,net->userfile);

    net = net->next;
  }

  return;
}

