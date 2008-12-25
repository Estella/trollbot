#include "config.h"

#include "main.h"
#include "config_engine.h"

#include "server.h"
#include "t_crypto_module.h"
#include "log_filter.h"
#include "log_entry.h"
#include "debug.h"
#include "tmodule.h"


/* The configuration process works in 2 steps
 * first it loads the file's data in tree form
 * and checks for syntax, then it walks through
 * handlers to be able to work with config files
 * in an overly complex and bloated way.
 */
int config_engine_init(char *filename)
{
	struct tconfig_block *tcfg;
	struct tconfig_block *defaults;

	log_entry_printf("o","Loading %s as conf file.",filename);
	tcfg      = file_to_tconfig(filename);
	log_entry_printf("o","Loaded %s.",filename);

	log_entry_printf("o","Loading defaults.conf");
	defaults  = file_to_tconfig("defaults.conf");
	log_entry_printf("o","Loaded defaults.conf");

	if (tcfg == NULL)
	{
		log_entry_printf("o","Did you remember to rename trollbot.conf.dist to trollbot.conf and edit it?");
		exit(EXIT_FAILURE);
	}

	if (defaults != NULL)
	{
		tconfig_merge(defaults,tcfg);
		free_tconfig(defaults);
		log_entry_printf("o","Merged %s with defaults.conf",filename);
	}

	g_cfg = config_engine_load(tcfg);

	/* keep a copy in the global config */
	g_cfg->tcfg = tcfg;

	return 0;
}

struct config *config_engine_load(struct tconfig_block *tcfg)
{
	struct config *cfg;
	struct tconfig_block *topmost = tcfg;
	struct tconfig_block *search  = tcfg;
	struct log_filter *filter;
	struct tmodule *tmodule;

	int i;

	cfg = tmalloc(sizeof(struct config));

	cfg->fork         = 0;
	cfg->forked       = 0;
	cfg->debug_level  = 0;
	cfg->hash_type    = NULL;
	cfg->filters      = NULL;

	cfg->dcc_motd     = NULL;

	cfg->tmodules     = NULL;
	cfg->tsockets     = NULL;

	cfg->crypto_name  = NULL;
	cfg->crypto       = NULL;

	while (topmost != NULL)
	{
		/* Only two global keywords exist right now, 
		 * network and owner 
		 */
		if (!strcmp(topmost->key,"global"))
		{
			if (strcmp(topmost->value,"settings"))
			{
				topmost = topmost->next;
				continue;
			}

			search = topmost->child;

			while (search != NULL)
			{
				if (!strcmp(search->key,"fork"))
				{
					cfg->fork = atoi(search->value);
				}
				else if (!strcmp(search->key,"console_log_flags"))
				{
					if (cfg->filters == NULL)
					{
						cfg->filters = log_filter_new();

						cfg->filters->flags   = tstrdup(search->value);
						cfg->filters->handler = console_log_filter_handler;
					}
					else
					{
						filter = log_filter_new();

						filter->flags   = tstrdup(search->value);
						filter->handler = console_log_filter_handler;

						cfg->filters = log_filter_add(cfg->filters, filter);
					}
				}
				else if (!strcmp(search->key,"debuglevel"))
				{
					cfg->debug_level = atoi(search->value);
				}
				else if (!strcmp(search->key,"dcc_motd"))
				{
					if (cfg->dcc_motd == NULL)
						cfg->dcc_motd = tstrdup(search->value);
				}
				else if (!strcmp(search->key,"hash_type"))
				{
					if (cfg->hash_type == NULL){
						cfg->hash_type = tstrdup(search->value);
					}
				}
				else if (!strcmp(search->key,"crypto_module"))
				{
					if (cfg->crypto_name == NULL)
					{
						cfg->crypto_name = tstrdup(search->value);
					}
				}
				search = search->next;
			}
		}
		else if (!strcmp(topmost->key,"module"))
		{
			if (cfg->tmodules == NULL)
				slist_init(&cfg->tmodules, tmodule_free);

			tmodule = tmodule_from_tconfig(topmost, tcfg);
	
			if (tmodule != NULL)
				slist_insert_next(cfg->tmodules, NULL, tmodule);
		}

		topmost = topmost->next;
	}

	return cfg;
}


