#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "util.h"
#include "network.h"

struct network *new_network(char *label)
{
  struct network *ret;

  ret = tmalloc(sizeof(struct network));

  if (label != NULL)
    ret->label = tstrdup(label);
  else
    ret->label = NULL;

  ret->prev         = NULL;
  ret->next         = NULL;
  ret->server_head  = NULL;
  ret->server_list  = NULL;
  ret->server_tail  = NULL; 
  ret->channel_list = NULL;
  ret->channel_head = NULL;
  ret->channel_tail = NULL;
  ret->cur_server   = NULL;
  ret->sock         = -1;
  ret->status       = 0;

  ret->nick         = NULL;
  ret->altnick      = NULL;
  ret->realname     = NULL;
  ret->ident        = NULL;

  return ret;
}
    

