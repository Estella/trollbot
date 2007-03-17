#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "network.h"

struct channel
{
  char *name;
  int status;

  struct channel *prev;
  struct channel *next;
};

struct channel *new_channel(const char *chan);
void join_channels(struct network *net);

#endif /* __CHANNEL_H__ */
