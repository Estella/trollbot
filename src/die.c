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
#include "config_engine.h"
#include "tconfig.h"

#include "die.h"

void die_nicely(int ret)
{
  if (g_cfg != NULL)
  {
    if (g_cfg->tcfg != NULL)
      free_tconfig(g_cfg->tcfg);

  }
    
  exit(ret);
}
