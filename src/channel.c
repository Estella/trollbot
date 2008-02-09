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
  
  if (chan != NULL)
    ret->name = tstrdup(chan);

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
  /* 353 toodle @ #java :toodle */
  
}
