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

struct config *g_cfg;

int main(int argc, char *argv[])
{
  printf("#################################################\n");
  printf("# Trollbot, written by poutine/DALnet           #\n");
  printf("#################################################\n");  

  if (argc > 2)
  {
    printf("# %-45s #\n","Proper Syntax: ./trollbot [config file]");
    printf("#################################################\n");
    return EXIT_FAILURE;
  }
  
  if (argc == 2)
    config_engine_init(argv[1]);
  else
    config_engine_init("trollbot.conf");

  printf("# %-45s #\n","Configuration File looks good");
  
  printf("#################################################\n");
 
  printf("# %-45s #\n","Entering IRC loop");
  
  irc_loop();

  printf("#################################################\n");
  
  return 0;
}
