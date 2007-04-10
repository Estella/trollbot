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

  ret->prev   = NULL;
  ret->next   = NULL;

  return ret;
}

struct channel *new_channel(const char *chan)
{
  struct channel *ret;

  ret = tmalloc(sizeof(struct channel));
  
  if (chan != NULL)
    ret->name = tstrdup(chan);

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

  if ((tmpchan = net->channel_head) == NULL)
    return;

  /* "JOIN " */
  numbytes += 5;

  do
  {
    if ((numbytes += strlen(tmpchan->name)) > BUFFER_SIZE-3)
      return;

    strcat(joinstr,tmpchan->name);
    strcat(joinstr,",");
    
  } while ((tmpchan = tmpchan->next) != NULL);

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
      g_cfg->tcfg = file_to_tconfig(g_cfg->tcfg,net->chanfile);
 
    net = net->next;
  }

  return;
}
