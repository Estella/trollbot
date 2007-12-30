#ifdef CONFIG_H
#include "config.h"
#endif /* CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network.h"
#include "util.h"

#define network_new irc_LTX_network_new

struct network *network_new(char *label)
{
  struct network *ret;

  ret = tmalloc(sizeof(struct network));

  ret->label = (label != NULL) ? tstrdup(label) : NULL;

  ret->botnick       = NULL;
  ret->nick          = NULL;
  ret->altnick       = NULL;
  ret->realname      = NULL;
  ret->ident         = NULL;

  ret->vhost         = NULL;
  ret->shost         = NULL;
 
  /* Connection queuing */
  ret->connect_delay = 10; 
  ret->connect_try   = 10;
  ret->last_try      = 0;  
  
  return ret;
}
    

