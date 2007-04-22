#ifndef __SERVER_H__
#define __SERVER_H__

struct tconfig_block;

struct server
{
  char *host;
  int port;

  /* Unhandled blocks go here */
  struct tconfig_block *tcfg;

  struct server *prev;
  struct server *next;
};

struct server *new_server(char *hostport);
#endif /* __SERVER_H__ */

