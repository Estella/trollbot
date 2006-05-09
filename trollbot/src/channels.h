/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/
#ifndef __CHANNELS_H__

#include "users.h"

enum
{
  CHAN_ABSENT = 0,
  CHAN_PRESENT,
  CHAN_BANNED,
  CHAN_INVITEONLY,
  CHAN_NEEDKEY
};

struct channels {
  char *name;

  int members;

  struct user_record *users,
                     *users_head,
                     *users_tail;

  int status;

  struct channels *prev, *next;
};

#define __CHANNELS_H__
#endif /* __CHANNELS_H__ */
