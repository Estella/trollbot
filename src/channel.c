#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "irc.h"
#include "main.h"
#include "channel.h"
#include "util.h"

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

