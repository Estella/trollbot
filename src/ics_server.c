#include "config.h"

#include <stdio.h>
#include <stdlib.h>

/* I have to do this */
#ifdef HAVE_PYTHON
#include "python_embed.h"
#endif /* HAVE_PYTHON */

#include <unistd.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#include "main.h"
#include "ics_server.h"
#include "ics_proto.h"
#include "ics_trigger.h"
#include "ics_game.h"
#include "server.h"
#include "t_timer.h"
#include "sockets.h"
#include "debug.h"
#include "log_filter.h"
#include "log_entry.h"
#include "tconfig.h"
#include "tsocket.h"

#ifdef HAVE_TCL
#include "tcl_embed.h"
#endif /* HAVE_TCL */

#ifdef HAVE_PERL
#include "perl_embed.h"
#endif /* HAVE_PERL */

#ifdef HAVE_JS
#include "js_embed.h"
#endif /* HAVE_JS */


struct ics_server *ics_server_from_tconfig_block(struct tconfig_block *tcfg)
{
	struct ics_server    *ics = NULL;
	struct tconfig_block *tmp = NULL;
	struct server        *svr = NULL;
	int i;

	if ((tmp = tcfg) == NULL)
	{
		log_entry_printf(NULL,NULL,"X","ics_server_from_tconfig_block() called with NULL tconfig block");
		return NULL;
	}

	if (tstrcasecmp(tmp->key,"ics_server"))
	{
		log_entry_printf(NULL,NULL,"X","ics_server_from_tconfig_block() called with invalid block type");
		return NULL;
	}

	if (tmp->child == NULL)
	{
		log_entry_printf(NULL,NULL,"X","ics_server_from_tconfig_block() called with a ics_server with no fucking child");
		return NULL;
	}

	ics  = new_ics_server(tmp->value);
	tmp = tmp->child;

	while (tmp != NULL)
	{
		if (!tstrcasecmp(tmp->key,"server"))
		{
			if (ics->servers != NULL)
			{
				svr = ics->servers;

				while (svr->next != NULL)
					svr = svr->next;

				svr->next         = new_server(tmp->value);
				svr->next->prev   = svr;
				svr               = svr->next;
			}
			else
			{
				ics->servers  = new_server(tmp->value);
				svr          = ics->servers;
				svr->prev    = NULL;
				svr->next    = NULL;
			}
		} 
		else if (!tstrcasecmp(tmp->key,"username"))
		{
			if (ics->username == NULL)
			{
				ics->username = tstrdup(tmp->value);
			}
		}
		else if (!tstrcasecmp(tmp->key,"password"))
		{
			if (ics->password == NULL)
			{
				ics->password = tstrdup(tmp->value);
			}
		}
		else if (!tstrcasecmp(tmp->key,"vhost"))
		{
			if (ics->vhost == NULL)
			{
				ics->vhost = tstrdup(tmp->value);
			}
		}
#ifdef HAVE_TCL
		else if (!strcmp(tmp->key,"tclscript"))
		{
			if (ics->tcl_scripts_size == 0)
			{
				/* Allocate 10 slots (9 usable, 1 NULL) */
				ics->tcl_scripts = tmalloc0(sizeof(char *) * 10);


				ics->tcl_scripts_size = 10;
			}

			for (i=0;i<(ics->tcl_scripts_size-1);i++)
			{
				if (ics->tcl_scripts[i] == NULL)
				{
					ics->tcl_scripts[i] = tstrdup(tmp->value);
					break;
				}
			}

			if (ics->tcl_scripts[i] == NULL)
			{
				/* Need more slots */
				ics->tcl_scripts = tsrealloc0(ics->tcl_scripts,ics->tcl_scripts_size+10,&ics->tcl_scripts_size);

				ics->tcl_scripts[i] = tstrdup(tmp->value);
			}
		}
#endif /* HAVE_TCL */
		

		tmp = tmp->next;
	}

	return ics;
}

struct ics_server *ics_server_add(struct ics_server *ics_server, struct ics_server *add)
{
	struct ics_server *tmp = NULL;

	if ((tmp = ics_server) == NULL)
		return add;

	while (tmp->next != NULL) tmp = tmp->next;

	tmp->next = add;
	add->prev = tmp;

	return ics_server;
}

struct ics_server *ics_server_del(struct ics_server *ics_server, struct ics_server *del)
{
	struct ics_server *tmp = NULL;

	if ((tmp = ics_server) == NULL)
	{
		log_entry_printf(NULL,NULL,"X","ics_server_del() called with NULL server list");
		return NULL;
	}

	while (tmp != NULL)
	{
		if (tmp == del)
		{
			if (tmp->prev != NULL)
				tmp->prev->next = tmp->next;

			if (tmp->next != NULL)
				tmp->next->prev = tmp->prev;

			while (tmp == del && tmp->prev != NULL)
				tmp = tmp->prev;

			while (tmp == del && tmp->next != NULL)
				tmp = tmp->next;

			if (tmp == del)
				return NULL;
			else
				return tmp;

		}

		tmp = tmp->next;
	}

	log_entry_printf(NULL,NULL,"X","ics_server_del() called with a server deletion that no entry existed for");

	return ics_server;

}

void ics_server_connect(struct ics_server *ics, struct tsocket *tsock)
{
	struct tsocket *mytsock;
	struct server  *srv;

	if (tsock == NULL)
		mytsock = tsocket_new();
	else
		mytsock = tsock;

	/* Assign it in the ics_server */
	mytsock->data = ics;
	ics->tsock    = mytsock;

	mytsock->name = tstrdup(ics->label);

	/* in ics_proto.c */
	mytsock->tsocket_read_cb       = ics_in;
	mytsock->tsocket_write_cb      = NULL;
	mytsock->tsocket_connect_cb    = ics_ball_start_rolling;
	mytsock->tsocket_disconnect_cb = ics_disconnected; 
	
	/* Find a suitable network server */
	srv = ics->servers;

	while (srv != NULL)
	{
		if (srv->host != NULL)
			if (tsocket_connect(mytsock, NULL, srv->host, srv->port))
				break;

		srv = srv->next;
	}

	if (srv == NULL)
	{
		troll_debug(LOG_WARN, "Could not connect to any servers for ICS server %s", ics->label);
		tsocket_free(tsock);
		return;
	}

	if (tsock == NULL)
	{
		/* Insert it into the global check list */
		if (g_cfg->tsockets == NULL)
			slist_init(&g_cfg->tsockets, tsocket_free);

		slist_insert_next(g_cfg->tsockets, NULL, (void *)mytsock);
	}

	return;
}

void free_ics_servers(struct ics_server *ics_server)
{
	struct ics_server *xtmp=NULL;
	struct ics_server *xold=NULL;

	if (ics_server == NULL)
		return;

	xtmp = ics_server;

	while (xtmp->prev != NULL)
		xtmp = xtmp->prev;

	while (xtmp != NULL)
	{
		xold = xtmp->next;

		free_ics_server(xtmp);

		xtmp = xold;
	}

	return;
}

void free_ics_server(void *ics_ptr)
{
	struct ics_server *ics = ics_ptr;
	free(ics->label);
	free(ics->vhost);
	free(ics->shost); 
	free(ics->username);
	free(ics->password);
	free(ics->my_name);

	free_ics_game(ics->game);
	tsocket_free(ics->tsock);

	free_servers(ics->servers);
	t_timers_free(ics->timers);
	free_ics_trigger_table(ics->ics_trigger_table);

	/* FIXME: free fucking JS shit, PHP shit, etc */

#ifdef HAVE_TCL
	tstrfreev(ics->tcl_scripts);
#endif /* HAVE_TCL */

#ifdef HAVE_JS
	tstrfreev(ics->js_scripts);
	/* Cleanup Spidermonkey */
	if (ics->cx != NULL)
	{
		JS_DestroyContext(ics->cx);
	}
#endif /* HAVE_JS */

#ifdef HAVE_PYTHON
	tstrfreev(ics->py_scripts);
#endif /* HAVE_PYTHON*/

	free(ics);

	return;
}


struct ics_server *new_ics_server(char *label)
{
	struct ics_server *ret;

	ret = tmalloc(sizeof(struct ics_server));

	if (label != NULL)
		ret->label = tstrdup(label);
	else
		ret->label = NULL;

	ret->prev          = NULL;
	ret->next          = NULL;

	ret->servers   = NULL; 

	ret->cur_server    = NULL;

	ret->tsock         = NULL;
	ret->status        = 0;

	ret->timers        = NULL;

	ret->vhost         = NULL;
	ret->shost         = NULL;

	ret->game          = NULL;
	ret->my_name       = NULL;

	ret->never_give_up = -1;
	ret->connect_delay = -1;

	ret->connected     = 0;

	ret->last_try      = 0;

	ret->tcfg          = NULL;

	ret->username      = NULL;
	ret->password      = NULL;

	ret->never_give_up = -1;
	/* This is the queueing BS */
	ret->connect_delay = -1; 
	ret->last_try      = 0;  

	ret->filters       = NULL;

	/* Ridiculously long */
	ret->ics_trigger_table = new_ics_trigger_table();


#ifdef HAVE_TCL
	ret->tcl_scripts      = NULL;
	ret->tcl_scripts_size = 0;
	ics_init_tcl(ret);
#endif /* HAVE_TCL */

#ifdef HAVE_PERL
	/* net_init_perl(ret); Need Jabber func */
#endif /* HAVE_PERL */

#ifdef HAVE_PYTHON
	ret->py_scripts = NULL;
	ret->py_scripts_size = 0;
#endif /* HAVE_PYTHON */

#ifdef HAVE_JS
	ret->cx     = NULL;
	ret->global = NULL;
	/*net_init_js(ret); */

	ret->plain_cx     = NULL;
	ret->plain_global = NULL;

	ret->js_scripts   = NULL;
	ret->js_scripts_size = 0;
#endif /* HAVE_JS */

	return ret;
}
