#ifndef __SERVER_H__
#define __SERVER_H__

#include "tconfig.h"

struct server
{
  char *host;
  int port;

  struct tconfig_block *tindex;

  struct server *prev;
  struct server *next;
};

struct server *new_server(char *hostport);
#endif /* __SERVER_H__ */

