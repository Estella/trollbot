/* This file provides the module API expected by Trollbot */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "ics_server.h"

#include "tmod_ics.h"
#include "tconfig.h"
#include "tsocket.h"
#include "util.h"

#define tmodule_load          ics_LTX_tmodule_load
#define tmodule_unload        ics_LTX_tmodule_unload
#define tmodule_get_tsockets  ics_LTX_tmodule_get_tsockets
#define tsockets              ics_LTX_tsockets
#define ics_servers           ics_LTX_ics_servers


struct slist *tsockets    = NULL;
struct slist *ics_servers = NULL;

struct slist *tmodule_get_tsockets(void)
{
	return tsockets;
}

int tmodule_load(struct tconfig_block *tcfg)
{
	struct tconfig_block *myblk = tcfg;
	struct ics_server    *ics   = NULL;

	slist_init(&ics_servers, free_ics_server);

	while (myblk != NULL)
	{
		if (!strcmp(myblk->key,"ics_server"))
		{
			ics = ics_server_from_tconfig_block(myblk);

			slist_insert_next(ics_servers, NULL, (void *)ics);

			printf("We have an ICS server %s\n", ics->label);
		}

		myblk = myblk->next;
	}

	/* HACK */
	ics_server_connect(ics, NULL);

	/* FIXME: Need logging mechanism */
	printf("ICS Module Loaded.\n");
	return 1;
}

int tmodule_unload(void)
{
	/* Do memory cleanup for module */
	printf("Module Unloaded\n");

	return 1;
}
