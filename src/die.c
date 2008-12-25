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
#include "config.h"
#include "util.h"

void die_nicely(int ret)
{
	struct tmodule *tmodule;
	struct tmodule *tmod_tmp;

	if (g_cfg != NULL)
	{
		/* Go through each module and unload it */
		
	
		free_tconfig(g_cfg->tcfg);
		free(g_cfg);
	}

	exit(ret);
}
