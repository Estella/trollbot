#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "network.h"

struct chan_user
{
  char *nick;
  
  int jointime;

  /* Actual user record */
  struct user *urec;

  struct chan_userlist *prev;
  struct chan_userlist *next;
};

struct channel
{
  char *name;
  int status;

  struct chan_user *user_list;

  struct tconfig_block *tindex;

  struct channel *prev;
  struct channel *next;
};

struct chan_user *new_chan_user(const char *nick, int jointime, struct user *urec);
struct channel *new_channel(const char *chan);
void join_channels(struct network *net);
void chan_init(void);

#endif /* __CHANNEL_H__ */
