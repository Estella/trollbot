#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

#ifdef UNIX
#include <unistd.h>
#endif /* UNIX */

#include "config.h"

#include "main.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "sockets.h"
#include "dcc.h"
#include "default_triggers.h"

struct config *g_cfg = NULL;

int main(int argc, char *argv[])
{
  pid_t pid;

  printf("#################################################\n");
  printf("# Trollbot v1.0.0, written by                   #\n");
  printf("#################################################\n");
	printf("# poutine/DALnet - Lead Developer               #\n");
  printf("# kicken/DALnet  - Javascript Support and Core  #\n");
	printf("# comcor/DALnet  - Python Support               #\n");
  printf("#################################################\n");  

  if (argc > 2)
  {
    printf("# %-45s #\n","Proper syntax: ./trollbot [config file]");
    printf("#################################################\n");
    return EXIT_FAILURE;
  }

  printf("# %-45s #\n","Reading configuration");
  printf("#################################################\n");
  printf("Configuration Parser Output:\n");
  
  if (argc == 2)
    config_engine_init(argv[1]);
  else
    config_engine_init("trollbot.conf");

  printf("#################################################\n");
  printf("# %-45s #\n","Configuration file looks good");
  printf("# %-45s #\n","Checking user databases");
  printf("#################################################\n");
  printf("userdb Parser Output:\n");

  user_init();

  printf("#################################################\n");
  printf("# %-45s #\n","User databases loaded");
  printf("# %-45s #\n","Checking channel databases");
  printf("#################################################\n");
  printf("chandb Parser Output:\n");

  chan_init();

  printf("#################################################\n");
  printf("# %-45s #\n","Channel databases loaded");
  printf("# %-45s #\n","Entering IRC loop");
  printf("#################################################\n");
  printf("IRC Debug Output:\n");

  add_default_triggers();


#ifdef HAVE_WORKING_FORK
  if (g_cfg->fork == 1)
  {
    g_cfg->forked = 1;

    pid = fork();

    if (pid != 0)
    {
      printf("Forked with pid: %d\n",pid);
      return 0;
    }
  }
#endif /* HAVE_WORKING_FORK */
    
  irc_loop();
  
  die_nicely(EXIT_SUCCESS);

  return 0;
}
