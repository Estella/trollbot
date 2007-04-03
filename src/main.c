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

#include "main.h"
#include "sockets.h"
#include "config_engine.h"
#include "user.h"
#include "channel.h"
#include "irc.h"
#include "tconfig.h"

struct config *g_cfg;

int main(int argc, char *argv[])
{
  printf("#################################################\n");
  printf("# Trollbot, written by poutine/DALnet           #\n");
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
  
  irc_loop();
  
  die_nicely();

  return 0;
}
