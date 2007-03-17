#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "server.h"

enum
{
  STATUS_IGNORED      = -1,
  STATUS_DISCONNECTED =  0,
  STATUS_CONNECTED    =  1,
  STATUS_AUTHORIZED,
  STATUS_IDLE
};

struct network
{
  char *label;

  struct server *cur_server;
  struct server *server_head;
  struct server *server_list;
  struct server *server_tail;

  struct channel *channel_list;
  struct channel *channel_head;
  struct channel *channel_tail;
   
  int sock;
  
  char *botnick;
  char *ident;
  char *realname;

  int status;
  
  struct network *prev;
  struct network *next;
};

struct network *new_network(char *label);

#endif /* __NETWORK_H__ */
