#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "util.h"

struct server *new_server(char *hostport)
{
  struct server *ret;
  char *ctmp;
  char *host;

  ret = tmalloc(sizeof(struct server));

  if (hostport != NULL)
  {
    if ((ctmp = strchr(hostport,':')) != NULL)
    {
      ctmp++;

      ret->port = atoi(ctmp);

      /* off by one to leave room for '\0' */
      host = tmalloc0((strlen(hostport) - strlen(ctmp)));

      strncpy(host,hostport,strlen(hostport) - strlen(ctmp) - 1);
      ret->host = host;
    } 
    else
    {
      ret->host = tstrdup(hostport);
      ret->port = 6667;
    }
  }
  else
  {
    ret->host = NULL;
    ret->port = 6667;
  }

  ret->prev = NULL;
  ret->next = NULL;

  return ret;

}
