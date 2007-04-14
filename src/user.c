#include "main.h"
#include "network.h"
#include "user.h"

void free_users(struct user *users)
{
  struct user *utmp;
  struct channel_flags *cftmp;

  if (users == NULL)
    return;

  while (users->prev != NULL)
    users = users->prev;

  while (users != NULL)
  {
    free(users->username);
    free(users->nick);
    free(users->ident);
    free(users->host);
    free(users->realname);
    free(users->passhash);
    free(users->flags); 

    if (users->chan_flags != NULL)
    {
      while (users->chan_flags->prev != NULL)
        users->chan_flags = users->chan_flags->prev;

      while (users->chan_flags != NULL)
      {
        free(users->chan_flags->chan);
        free(users->chan_flags->flags);

        cftmp = users->chan_flags; 
        
        users->chan_flags = users->chan_flags->next;
         
        free(cftmp);
      }
    }

    utmp  = users;
    users = users->next;

    free(utmp);
  }
}


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

  ret->chan_flags = NULL;

  ret->extra  = NULL;
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

