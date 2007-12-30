#include "main.h"
#include "network.h"
#include "user.h"

void user_list_del(struct user **orig, struct user *old)
{
  struct user *tmp;

  if (*orig == NULL || old == NULL)
    return;

  if (*orig == old)
  {
    /* Original pointer is modified */
    if (old->next != NULL)
      *orig = old->next;
    else
      *orig = NULL;

    old->prev = NULL;
    old->next = NULL;

    free_users(old);
  } else {
    /* original pointer is unmodified */
    tmp = old;

    while (tmp->prev != NULL) tmp = tmp->prev;

    while (tmp != NULL)
    {
      if (tmp == old)
      {
        /* match */
        if (tmp->prev != NULL)
          tmp->prev->next = tmp->next;

        if (tmp->next != NULL)
          tmp->next->prev = tmp->prev;

        tmp->prev = NULL;
        tmp->next = NULL;
        
        free_users(tmp);

        return;
      }

      tmp = tmp->next;
    }
  }

  return;
}


void user_list_add(struct user **orig, struct user *new)
{
  struct user *tmp;

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
    free(users->uhost);
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

  ret->uhost = NULL;
  
  ret->chan_flags = NULL;

  ret->tcfg   = NULL;
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
    {
      /* g_cfg->tcfg = file_to_tconfig(g_cfg->tcfg,net->userfile); FIXME */
    }

    net = net->next;
  }

  return;
}

