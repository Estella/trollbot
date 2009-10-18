#include "config.h"

#include <sys/types.h>

#ifdef HAVE_PYTHON
#include "python_embed.h"
#endif /* HAVE_PYTHON */

#include "main.h"
#include "config_engine.h"

#include "network.h"
#include "server.h"
#include "dcc.h"
#include "channel.h"
#include "user.h"
#include "irc_trigger.h"
#include "t_crypto_module.h"
#include "log_filter.h"
#include "log_entry.h"
#include "debug.h"

#ifdef HAVE_HTTP
#include "http_server.h"
#include "http_proto.h"
#include "http_request.h"
#endif /* HAVE_HTTP */

#ifdef HAVE_ICS
#include "ics_server.h"
#include "ics_proto.h"
#include "ics_trigger.h"
#endif /* HAVE_ICS */

#ifdef HAVE_XMPP
#include "xmpp_server.h"
#include "xmpp_proto.h"
#include "xmpp_trigger.h"
#endif /* HAVE_XMPP */

#ifdef HAVE_TCL
#include "tcl_embed.h"
#endif /* HAVE_TCL */

#ifdef HAVE_PHP
#include "php_embed.h"
#endif /* HAVE_PHP */

#ifdef HAVE_JS
#include "js_embed.h"
#endif /* HAVE_JS */

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

	log_entry_printf(NULL,NULL,"o","Loading %s as conf file.",filename);
	tcfg      = file_to_tconfig(filename);
	log_entry_printf(NULL,NULL,"o","Loaded %s.",filename);

	log_entry_printf(NULL,NULL,"o","Loading defaults.conf");
	defaults  = file_to_tconfig("defaults.conf");
	log_entry_printf(NULL,NULL,"o","Loaded defaults.conf");


	if (tcfg == NULL)
	{
		log_entry_printf(NULL,NULL,"o","Did you remember to rename trollbot.conf.dist to trollbot.conf and edit it?");
		exit(EXIT_FAILURE);
	}

	if (defaults != NULL)
	{
		tconfig_merge(defaults,tcfg);
		free_tconfig(defaults);
		log_entry_printf(NULL,NULL,"o","Merged %s with defaults.conf",filename);
	}

	g_cfg = config_engine_load(tcfg);

#ifdef HAVE_PHP
	log_entry_printf(NULL,NULL,"o","Loading PHP scripts contained in config file.");
	php_load_scripts_from_config(g_cfg);
	log_entry_printf(NULL,NULL,"o","Loaded PHP scripts.");
#endif /* HAVE_PHP */

#ifdef HAVE_PYTHON
	log_entry_printf(NULL,NULL,"o","Loading Python scripts contained in config file.");
	python_load_scripts_from_config(g_cfg);
	log_entry_printf(NULL,NULL,"o","Loaded Python scripts.");
#endif /* HAVE_PYTHON */

#ifdef HAVE_JS
	log_entry_printf(NULL,NULL,"o","Loading Javascript scripts contained in config file.");
	js_load_scripts_from_config(g_cfg);
	log_entry_printf(NULL,NULL,"o","Loaded Javascript scripts.");
#endif /* HAVE_JS */

#ifdef HAVE_TCL
	log_entry_printf(NULL,NULL,"o","Loading TCL scripts contained in config file.");
	tcl_load_scripts_from_config(g_cfg);
	log_entry_printf(NULL,NULL,"o","Loaded TCL scripts.");
#endif /* HAVE_TCL */

#ifdef HAVE_ICS
#ifdef HAVE_TCL
	log_entry_printf(NULL,NULL,"I","Loading TCL scripts contained in config file for ICS.");
	ics_tcl_load_scripts_from_config(g_cfg);
	log_entry_printf(NULL,NULL,"o","Loaded ICS TCL scripts.");
#endif /* HAVE_TCL */
#endif /* HAVE_ICS */

	/* keep a copy in the global config */
	g_cfg->tcfg = tcfg;

	if (g_cfg->crypto_name != NULL)
	{
		log_entry_printf(NULL,NULL,"o","Loading crypto module.");
		g_cfg->crypto = t_crypto_module_load(g_cfg->crypto_name);
		log_entry_printf(NULL,NULL,"o","Loaded crypto module successfully.");
	}

	return 0;
}

struct config *config_engine_load(struct tconfig_block *tcfg)
{
	struct config *cfg;
	struct tconfig_block *topmost = tcfg;
	struct tconfig_block *search  = tcfg;
	struct network *net;
	struct server *svr;
	struct channel *chan;
	struct log_filter *filter;

	int i;

	cfg = tmalloc(sizeof(struct config));

	cfg->networks     = NULL;
	cfg->fork         = 0;
	cfg->forked       = 0;
	cfg->debug_level  = 0;
	cfg->hash_type    = NULL;
	cfg->dccs         = NULL;
	cfg->filters      = NULL;

	cfg->dcc_motd     = NULL;

	cfg->tsockets     = NULL;

#ifdef HAVE_HTTP
	cfg->http_servers = NULL;
#endif /* HAVE_HTTP */

#ifdef HAVE_ICS
	cfg->ics_servers = NULL;
#endif /* HAVE_ICS */
#ifdef HAVE_XMPP
	cfg->xmpp_servers = NULL;
#endif /* HAVE_XMPP */

	cfg->crypto_name  = NULL;
	cfg->crypto       = NULL;
#ifdef HAVE_PYTHON
	cfg->py_main         = NULL;
	cfg->py_main_dict    = NULL;
	cfg->py_scripts      = NULL;
	cfg->py_scripts_size = 0;
#endif /* HAVE_PYTHON */

#ifdef HAVE_PHP
	cfg->php_scripts      = NULL;
	cfg->php_scripts_size = 0;
#endif /* HAVE_PHP */

#ifdef HAVE_JS
	cfg->js_rt           = NULL;
#endif /* HAVE_JS */

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
						cfg->filters->net     = NULL;
						cfg->filters->handler = console_log_filter_handler;
					}
					else
					{
						filter = log_filter_new();

						filter->flags   = tstrdup(search->value);
						filter->net     = NULL;
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
#ifdef HAVE_HTTP
		else if (!strcmp(topmost->key,"http_server"))
		{
			/* Much simpler this way, isn't it? */
			cfg->http_servers = http_server_add(cfg->http_servers,http_server_from_tconfig_block(topmost));
		}
#endif /* HAVE_HTTP */
#ifdef HAVE_ICS
		else if (!strcmp(topmost->key,"ics_server"))
		{
			/* Much simpler this way, isn't it? */
			cfg->ics_servers = ics_server_add(cfg->ics_servers,ics_server_from_tconfig_block(topmost));
		}
#endif /* HAVE_ICS */

#ifdef HAVE_XMPP
		else if (!strcmp(topmost->key,"xmpp_server"))
		{
			/* Much simpler this way, isn't it? */
			cfg->xmpp_servers = xmpp_server_add(cfg->xmpp_servers,xmpp_server_from_tconfig_block(topmost));
		}
#endif /* HAVE_XMPP */
		else if (!strcmp(topmost->key,"network"))
		{
			net = cfg->networks;

			if (net != NULL)
			{
				while (net->next != NULL)
					net = net->next;

				net->next       = new_network(topmost->value);
				net->next->prev = net;
				net             = net->next;
			}
			else
			{
				cfg->networks = new_network(topmost->value);

				net           = cfg->networks;
				net->next     = NULL;
				net->prev     = NULL;
			}

			net->tcfg = topmost;

			search = topmost->child;

			while (search != NULL)
			{
				if (!strcmp(search->key,"nick"))
				{
					/* Use the first value (for stacking configs) */
					if (net->nick == NULL)
					{
						net->nick = tstrdup(search->value);

						if (net->botnick != NULL)
							free(net->botnick);

						net->botnick = tstrdup(search->value);
					}
				}
				else if (!strcmp(search->key,"never-give-up"))
				{
					/* Use the first value only (Initially set to -1) */
					if (net->never_give_up == -1)
						net->never_give_up = atoi(search->value);
				}
				else if (!strcmp(search->key,"server-cycle-wait"))
				{
					/* Only use the first value */
					if (net->connect_delay == -1)
						net->connect_delay = atoi(search->value);
				}
				else if (!strcmp(search->key,"default-flags"))
				{
					if (net->default_flags == NULL)
						net->default_flags = tstrdup(search->value);
				}		
				else if (!strcmp(search->key,"console"))
				{
					if (cfg->filters == NULL)
					{
						cfg->filters = log_filter_new();

						cfg->filters->flags   = tstrdup(search->value);
						cfg->filters->net     = net;
						cfg->filters->handler = dcc_log_filter_handler;
					}
					else
					{
						filter = log_filter_new();

						filter->flags   = tstrdup(search->value);
						filter->net     = net;
						filter->handler = dcc_log_filter_handler;

						cfg->filters = log_filter_add(cfg->filters, filter);
					}

				}
				else if (!strcmp(search->key,"dcc_motd"))
				{
					if (net->dcc_motd == NULL)
						net->dcc_motd = tstrdup(search->value);
				}		
				else if (!strcmp(search->key,"dcc_port"))
				{
					if (net->dcc_port == -1)
						net->dcc_port = atoi(search->value);
				}
				else if (!strcmp(search->key,"altnick"))
				{
					/* Use first value only */
					if (net->altnick == NULL)
						net->altnick = tstrdup(search->value);
				}
				else if (!strcmp(search->key,"handlen"))
				{
					/* TODO: Proper checking */
					net->handlen = atoi(search->value);
				}
#ifdef HAVE_JS
				else if (!strcmp(search->key,"javascript"))
				{
					if (net->js_scripts_size == 0)
					{
						/* Allocate 10 slots (9 usable, 1 NULL) */
						net->js_scripts = tmalloc0(sizeof(char *) * 10);


						net->js_scripts_size = 10;
					}

					for (i=0;i<(net->js_scripts_size-1);i++)
					{
						if (net->js_scripts[i] == NULL)
						{
							net->js_scripts[i] = tstrdup(search->value);
							break;
						}
					}

					if (net->js_scripts[i] == NULL)
					{
						/* Need more slots */
						net->js_scripts = tsrealloc0(net->js_scripts,net->js_scripts_size+10,&net->js_scripts_size);

						net->js_scripts[i] = tstrdup(search->value);
					}
				}
#endif /* HAVE_JS */
#ifdef HAVE_TCL
				else if (!strcmp(search->key,"tclscript"))
				{
					if (net->tcl_scripts_size == 0)
					{
						/* Allocate 10 slots (9 usable, 1 NULL) */
						net->tcl_scripts = tmalloc0(sizeof(char *) * 10);


						net->tcl_scripts_size = 10;
					}

					for (i=0;i<(net->tcl_scripts_size-1);i++)
					{
						if (net->tcl_scripts[i] == NULL)
						{
							net->tcl_scripts[i] = tstrdup(search->value);
							break;
						}
					}

					if (net->tcl_scripts[i] == NULL)
					{
						/* Need more slots */
						net->tcl_scripts = tsrealloc0(net->tcl_scripts,net->tcl_scripts_size+10,&net->tcl_scripts_size);

						net->tcl_scripts[i] = tstrdup(search->value);
					}
				}
#endif /* HAVE_TCL */
#ifdef HAVE_PHP
				else if (!strcmp(search->key,"phpscript"))
				{
					if (cfg->php_scripts_size == 0)
					{
						/* Allocate 10 slots (9 usable, 1 NULL) */
						cfg->php_scripts = tmalloc0(sizeof(char *) * 10);


						cfg->php_scripts_size = 10;
					}

					for (i=0;i<(cfg->php_scripts_size-1);i++)
					{
						if (cfg->php_scripts[i] == NULL)
						{
							cfg->php_scripts[i] = tstrdup(search->value);
							break;
						}
					}

					if (cfg->php_scripts[i] == NULL)
					{
						/* Need more slots */
						cfg->php_scripts = tsrealloc0(cfg->php_scripts,cfg->php_scripts_size+10,&cfg->php_scripts_size);

						cfg->php_scripts[i] = tstrdup(search->value);
					}
				}
#endif /* HAVE_PHP */
#ifdef HAVE_PYTHON
				else if (!strcmp(search->key,"pythonscript") || !strcmp(search->key,"pyscript"))
				{
					if (net->py_scripts_size == 0)
					{
						/* Allocate 10 slots (9 usable, 1 NULL) */
						net->py_scripts = tmalloc0(sizeof(char *) * 10);


						net->py_scripts_size = 10;
					}

					for (i=0;i<(net->py_scripts_size-1);i++)
					{
						if (net->py_scripts[i] == NULL)
						{
							net->py_scripts[i] = tstrdup(search->value);
							break;
						}
					}

					if (net->py_scripts[i] == NULL)
					{
						/* Need more slots */
						net->py_scripts = tsrealloc0(net->py_scripts,net->py_scripts_size+10,&net->py_scripts_size);

						net->py_scripts[i] = tstrdup(search->value);
					}

					cfg_init_python(cfg);

					net_init_python(cfg,net);
				}
#endif /* HAVE_PYTHON */
				else if (!strcmp(search->key,"userfile"))
				{
					if (net->userfile == NULL)
						net->userfile = tstrdup(search->value);
				}
				else if (!strcmp(search->key,"chanfile"))
				{
					if (net->chanfile == NULL)
						net->chanfile = tstrdup(search->value);
				}
				else if (!strcmp(search->key,"realname"))
				{
					if (net->realname == NULL)
						net->realname = tstrdup(search->value);
				}
				else if (!strcmp(search->key,"ident"))
				{
					if (net->ident == NULL)
						net->ident = tstrdup(search->value);
				}
				else if (!strcmp(search->key,"vhost"))
				{
					if (net->vhost == NULL)
						net->vhost = tstrdup(search->value);
				}
				else if (!strcmp(search->key,"server"))
				{
					if (net->servers != NULL)
					{
						svr = net->servers;

						while (svr->next != NULL)
							svr = svr->next;

						svr->next         = new_server(search->value);
						svr->next->prev   = svr;
						svr               = svr->next;
					}
					else
					{
						net->servers = new_server(search->value);
						svr          = net->servers;
						svr->prev    = NULL;
						svr->next    = NULL;
					}
				}
				else if (!strcmp(search->key,"channel"))
				{
					chan = new_chan_from_tconfig_block(search);
				
					if (chan != NULL)	
						channel_list_add(&net->chans,chan);
				}

				search = search->next;
			}
		}

		topmost = topmost->next;
	}

	return cfg;
}


