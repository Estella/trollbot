/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

#include "channels.h"
#include "servers.h"

/* I use a linked list instead of a dynamic pointer array because
 * there will be extra data associated with them eventually
 */
void add_channel(const char *channel)
{
  struct channels *tmp = NULL;

  if (glob_chan == NULL)
  {
    glob_chan                  = tmalloc(sizeof(struct channels));
    glob_chan->prev            = NULL;
    glob_chan_head             = glob_chan;
    glob_chan_tail             = glob_chan;
    tmp                        = glob_chan;
  } else {
    glob_chan_tail->next       = tmalloc(sizeof(struct channels));
    glob_chan_tail->next->prev = glob_chan_tail;
    glob_chan_tail             = glob_chan_tail->next;
    tmp                        = glob_chan_tail;
  }

  tmp->name = tstrdup(channel);

  tmp->users      = NULL;
  tmp->users_head = NULL;
  tmp->users_tail = NULL;
  tmp->status     = CHAN_ABSENT;

  tmp->next = NULL;

  return;
}

void join_channels(void)
{
  struct channels *tmp = glob_chan_head;

  if (tmp == NULL)
    return;

  while (tmp != NULL)
  {
    irc_printf(glob_server_head->sock,"JOIN %s",tmp->name);
    tmp = tmp->next;
  }
}

void print_channels(void)
{
  struct channels *tmp = glob_chan_head;

  if (tmp == NULL)
    return;

  while (tmp != NULL)
  {
    troll_debug(LOG_DEBUG,"Channel: %s\n",tmp->name);
  
    tmp = tmp->next;
  }

  return;
}

void free_channels(void)
{
  struct channels *chans = glob_chan_head;
  struct channels *tmp   = NULL;

  if (chans == NULL)
    return;

  while (chans != NULL)
  {
    free(chans->name);
    tmp = chans;

    chans = chans->next;

    free(tmp);
  }

  return;
}

