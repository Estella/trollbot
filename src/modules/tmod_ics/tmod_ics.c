/* This file provides the module API expected by Trollbot */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tconfig.h"
#include "tsocket.h"
#include "util.h"

#define tmodule_init          tmod_ics_LTX_tmodule_init
#define tmodule_unload        tmod_ics_LTX_tmodule_unload
#define tmodule_get_tsockets  tmod_ics_LTX_tmodule_get_tsockets

int tmodule_init(struct tconfig *)
{
	
}

int tmodule_unload(void)
{
	
}


