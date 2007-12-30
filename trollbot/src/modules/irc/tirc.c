#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tirc.h"
#include "tconfig.h"
#include "util.h"

struct slist   *readfds;
struct slist   *writefds;

int num_read_fds  = 0;
int num_write_fds = 0;


struct slist *tmodule_get_writefds(void)
{
  if (num_write_fds > 0)
    return writefds;
  else
    return NULL;
}

struct slist *tmodule_get_readfds(void)
{
  if (num_read_fds > 0)
    return readfds;
  else
    return NULL;
}


int tmodule_init(void *init_data)
{
  struct tconfig_block *tcfg = init_data;

  if (tcfg != NULL)
  {
    printf("We got a tcfg struct, current value == %s\n",tcfg->value);
  }
  else
  {
    printf("no IRC module configuration detected. Exiting...\n");
    return 0;
  }

  printf("IRC: Loaded module\n");

  /* Initiate IRC connections */

  return 1;
}

