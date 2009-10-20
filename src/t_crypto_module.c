#include <stdio.h>
#include <stdlib.h>

#include <ltdl.h>

#include "debug.h"
#include "irc_network.h"
#include "util.h"
#include "t_crypto_module.h"

struct t_crypto_module *t_crypto_module_load(char *module_name)
{
	struct t_crypto_module *tcm = NULL;
	lt_dlhandle tmp;	

	/* Search modules directory */
	lt_dlsetsearchpath("modules");

	tmp = lt_dlopenext(module_name);

	if (tmp == NULL)
	{
		troll_debug(LOG_WARN,"Could not open module \"%s\"",module_name);
		return NULL;
	}

	troll_debug(LOG_DEBUG,"Loaded module \"%s\"",module_name);

	tcm = tmalloc(sizeof(struct t_crypto_module));

	tcm->handle = tmp;

	tcm->cipher_algorithm_exists = lt_dlsym(tcm->handle,"cipher_algorithm_exists");
	tcm->hash_algorithm_exists   = lt_dlsym(tcm->handle,"hash_algorithm_exists");
	tcm->create_hash             = lt_dlsym(tcm->handle,"create_hash");

	return tcm;
}
