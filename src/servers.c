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
#include "servers.h"


struct network *network_new(void){
  struct network *n;
  n = tmalloc0(sizeof(*n));

  return n;
}

struct network *network_free(struct network *network){
  struct network *network_next = network->next;

  network->current_nick = NULL;
  if (network->nicklist){
    tstrfreev(network->nicklist);
    network->nicklist=NULL;
  }

  if (network->name) free(network->name);
  /* TODO: Add code for freeing server list, when server list exists. */

  free(network);
  return network_next;
}


/*
void add_server(const char *server)
{
  struct servers *tmp       = NULL;
  int            port       = -1;
  const char     *ptr       = server;
  int            i          = 0;
  char           *localserv = NULL;

  /* First off, let's see if this is a server:port or just a
   * server
   * /
  if ((ptr = strchr(server,':')) != NULL)
  {
    ptr++;
    port = atoi(ptr);

    /* reset the pointer * /
    ptr = server;

    localserv = tmalloc0(sizeof(char) * strlen(server) + 1);

    for (i=0;ptr[i] != ':';i++)
      localserv[i] = ptr[i];
  }

  if (glob_server == NULL)
  {
    glob_server                  = tmalloc(sizeof(struct servers));
    glob_server->prev            = NULL;
    glob_server_head             = glob_server;
    glob_server_tail             = glob_server;
    tmp                          = glob_server;
  } else {
    glob_server_tail->next       = tmalloc(sizeof(struct servers));
    glob_server_tail->next->prev = glob_server_tail;
    glob_server_tail             = glob_server_tail->next;
    tmp                          = glob_server_tail;
  }

  /* server only * /
  if (port == -1)
  {
    tmp->name = tstrdup(server);
    tmp->port = 6667;
  } else { /* server:port * /
    tmp->name = localserv;
    tmp->port = port;
  }

  tmp->status = -1;
  tmp->sock   = -1;

  tmp->next   = NULL;

  return;
}

void print_servers(void)
{
  struct servers *tmp = glob_server_head;

  if (tmp == NULL)
    return;

  while (tmp != NULL)
  {
   troll_debug(LOG_DEBUG,"Server: %s %d\n",tmp->name,tmp->port);

    tmp = tmp->next;
  }

  return;
}

void free_servers(void)
{
  struct servers *servers = glob_server_head;
  struct servers *tmp     = NULL;

  if (servers == NULL)
    return;

  while (servers != NULL)
  {
    free(servers->name);

    tmp = servers;

    servers = servers->next;

    free(tmp);
  }

  return;
}

*/
