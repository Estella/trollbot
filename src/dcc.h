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

enum {
  DCC_WAITING = 0,
  DCC_CONNECT,
  DCC_AUTH
};

struct dcc_session
{
  int id;

  int sock;

  char *user;

  int status;

  struct dcc_session *prev, *next;
};

#define __DCC_H__
#endif /* __DCC_H__ */
