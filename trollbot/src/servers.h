/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/
#ifndef __SERVERS_H__

struct servers {
  char *name;

  int port;
  int status;

  int sock;

  struct servers *prev, *next;
};

#define __SERVERS_H__
#endif /* __SERVERS_H__ */
