/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#ifndef __DCC_H__

enum dcc_status {
  DCC_WAITING = 0,
  DCC_CONNECTED,
  DCC_HAVE_USERNAME,
  DCC_AUTHENTICATED
};

struct dcc_session
{
  int id;

  int sock;

  struct user *user;

  int status;

  struct dcc_session *prev;
  struct dcc_session *next;
};

#define __DCC_H__
#endif /* __DCC_H__ */
