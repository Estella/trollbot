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

void server_list_add(struct server **orig, struct server *new);
void free_servers(struct server *servers);
struct server *new_server(char *hostport);
void free_server(struct server *server);

#endif /* __SERVER_H__ */

