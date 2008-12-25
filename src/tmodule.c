/* Trollbot's module interface */
#ifdef CONFIG_H
#include "config.h"
#endif /* CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ltdl.h>

#include "debug.h"
#include "util.h"
#include "tconfig.h"
#include "tmodule.h"

/* This function will parse a tconfig "module" block, and attempt
 * to load the module
 */
struct tmodule *tmodule_from_tconfig(struct tconfig_block *tcfg, struct tconfig_block *global_cfg)
{
	lt_dlhandle ltmodule   = NULL;
	struct tmodule *module = NULL;
	int (*tmodule_load)(struct tconfig_block *, struct tconfig_block *);

	if (strcmp(tcfg->key, "module"))
		return NULL;

	/* Search modules directory */
	lt_dlsetsearchpath("modules");

	ltmodule = lt_dlopenext(tcfg->value);

	if (ltmodule == NULL)
	{
		troll_debug(LOG_WARN, "Could not open module \"%s\" (Reason: %s)", tcfg->value, lt_dlerror());
		return NULL;
	}

	troll_debug(LOG_DEBUG, "Loaded module \"%s\"", tcfg->value);

	if ((tmodule_load = lt_dlsym(ltmodule, "tmodule_load")) == NULL)
	{
		troll_debug(LOG_WARN, "Could not find symbol tmodule_load in  module \"%s\"", tcfg->value);
		lt_dlclose(ltmodule);
	} 
	else
	{
		/* We praise our magnificient overlords */
		if (!(*tmodule_load)(tcfg->child, global_cfg))
		{
			troll_debug(LOG_WARN, "Init function for module \"%s\" failed", tcfg->value);
			lt_dlclose(ltmodule);
			return NULL;
		}
	}

	module = tmalloc(sizeof(struct tmodule));

	module->name                    = tstrdup(tcfg->value);
	module->handle                  = ltmodule;

	/* Loading mechanism */
	module->tmodule_load            = lt_dlsym(ltmodule, "tmodule_load"); 
	module->tmodule_unload          = lt_dlsym(ltmodule, "tmodule_unload");

	/* Socket system */
	module->tmodule_get_tsockets    = lt_dlsym(ltmodule, "tmodule_get_tsockets");

	/* Messaging system */
	module->tmodule_get_messages    = lt_dlsym(ltmodule, "tmodule_get_messages");
	module->tmodule_handle_messages = lt_dlsym(ltmodule, "tmodule_handle_messages");

	return module;
}

void tmodule_free(void *data)
{
	struct tmodule *module = data;

	free(module->name);
	free(module);
}  
