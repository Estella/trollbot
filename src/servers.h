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

struct network {
  char *name;
  char *current_nick;
  char **nicklist;

  struct server *current_server;
  struct server *servers;
  struct network *next;
};

struct server {
  char *name;

  int port;
  int status;

  int sock;

  struct server *prev, *next;
};

#define __SERVERS_H__
#endif /* __SERVERS_H__ */
