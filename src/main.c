#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>

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
  printf("# Trollbot v1.0, written by poutine/DALnet      #\n");
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

  tconfig_to_file(g_cfg->tcfg,"out.txt");
    
  irc_loop();
  
  die_nicely(EXIT_SUCCESS);

  return 0;
}
