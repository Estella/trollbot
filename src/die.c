/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/
#include "main.h"
#include "die.h"
#include "tconfig.h"
#include "config_engine.h"
#include "dcc.h"
#include "network.h"
#include "config.h"

/* Needs more cowbell */
void die_nicely(int ret)
{
  if (g_cfg != NULL)
  {
    free_networks(g_cfg->networks);

    if (g_cfg->tcfg != NULL)
      free_tconfig(g_cfg->tcfg);

    if (g_cfg->dccs != NULL)
      free_dcc_sessions(g_cfg->dccs);

#ifdef HAVE_JS
    JS_DestroyRuntime(g_cfg->js_rt);
#endif

    free(g_cfg);
  }
    
  exit(ret);
}
