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
#include "network.h"
#include "trigger.h"
#include "trig_table.h"
#include "irc.h"

struct config *g_cfg;

static void dummy_handler(struct network *net, struct irc_data *data)
{
  irc_printf(net->sock,"PRIVMSG %s :Moo\n",data->c_params[0]);
}

int main(int argc, char *argv[])
{
  struct network *net;

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

  printf("# %-45s #\n","Checking userdb");
  
  user_init();

  /* Trigger testing */
  net = g_cfg->network_head;

  while (net != NULL)
  {
    if (!strcmp(net->label,"DALnet"))
    {
      net->trigs->pub = new_trigger(NULL,"moo",NULL,dummy_handler);
    }
    net = net->next;
  }

      
  printf("#################################################\n");
 
  printf("# %-45s #\n","Entering IRC loop");
  
  irc_loop();

  printf("#################################################\n");
  
  return 0;
}
