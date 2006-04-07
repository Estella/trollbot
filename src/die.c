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

#include "channels.h"
#include "servers.h"
#include "users.h"
#include "tconfig.h"
#include "main.h"


void die_nicely(void)
{
  global_config_free();
  /*free_servers();
  free_channels();*/
  exit(1);

}



