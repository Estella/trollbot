#include "main.h"
#include "channel.h"

#include "tconfig.h"
#include "irc.h"
#include "util.h"
#include "network.h"
#include "user.h"

struct chan_user *new_chan_user(const char *nick, int jointime, struct user *urec)
{
  struct chan_user *ret;

  ret = tmalloc(sizeof(struct chan_user));

  ret->nick     = (nick != NULL)     ? tstrdup(nick)     : NULL;

  ret->jointime = jointime;
  ret->urec     = urec;
 
  ret->prev     = NULL;
  ret->next     = NULL;

  return ret;
}

void free_channels(struct channel *chans)
{
  struct chan_user *cusers;
  struct chan_user *cusertmp;
  struct channel   *chantmp;

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
