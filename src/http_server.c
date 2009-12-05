/*********************************************
 * TrollBot v1.0                             *
 *********************************************
 * TrollBot is an eggdrop-clone designed to  *
 * work with multiple networks and protocols *
 * in order to present a unified scriptable  *
 * event-based platform,                     *
 *********************************************
 * This software is PUBLIC DOMAIN. Feel free *
 * to use it for whatever use whatsoever.    *
 *********************************************
 * Originally written by poutine/DALnet      *
 *                       kicken/DALnet       *
 *                       comcor/DALnet       *
 *********************************************/
#include "config.h"

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

#include "trollbot.h"

#include "tsocket.h"
#include "http_server.h"
#include "http_request.h"
#include "http_proto.h"


#include "server.h"
#include "t_timer.h"
#include "sockets.h"
#include "debug.h"
#include "log_filter.h"
#include "log_entry.h"
#include "tconfig.h"

#ifdef HAVE_TCL
#include "tcl_embed.h"
#endif /* HAVE_TCL */

#ifdef HAVE_PERL
#include "perl_embed.h"
#endif /* HAVE_PERL */

#ifdef HAVE_JS
#include "js_embed.h"
#endif /* HAVE_JS */

/* In function for tsockets */
int http_server_accept(struct tsocket *tsock)
{
	struct tsocket *ntsock;

	ntsock       = tsocket_accept(tsock);
	/* Unsafe ? */
	ntsock->data = tsock->data;

	log_entry_printf(NULL,NULL,"H","http_server_accept: New HTTP server connection.");

	/* Give it a read callback */
	ntsock->tsocket_read_cb = http_in;

	/* Insert it into global list */
	if (g_cfg->tsockets == NULL)                                     
		slist_init(&g_cfg->tsockets, tsocket_free);              

	slist_insert_next(g_cfg->tsockets, NULL, (void *)ntsock);

	return ntsock->sock;
}

struct http_server *http_server_from_tconfig_block(struct tconfig_block *tcfg)
{
	struct http_server    *http = NULL;
	struct tconfig_block   *tmp = NULL;

	if ((tmp = tcfg) == NULL)
	{
		log_entry_printf(NULL,NULL,"X","http_server_from_tconfig_block() called with NULL tconfig block");
		return NULL;
	}

	if (tstrcasecmp(tmp->key,"http_server"))
	{
		log_entry_printf(NULL,NULL,"X","http_server_from_tconfig_block() called with invalid block type");
		return NULL;
	}

	if (tmp->child == NULL)
	{
		log_entry_printf(NULL,NULL,"X","http_server_from_tconfig_block() called with a http_server with no fucking child");
		return NULL;
	}

	http  = new_http_server(tmp->value);
	tmp = tmp->child;

	while (tmp != NULL)
	{
		if (!tstrcasecmp(tmp->key,"username"))
		{
			if (http->username == NULL)
			{
				http->username = tstrdup(tmp->value);
			}
		}
		else if (!tstrcasecmp(tmp->key,"password"))
		{
			if (http->password == NULL)
			{
				http->password = tstrdup(tmp->value);
			}
		}
		else if (!tstrcasecmp(tmp->key,"web_root"))
		{
			if (http->web_root == NULL)
			{
				http->web_root = tstrdup(tmp->value);
			}
		}
		else if (!tstrcasecmp(tmp->key,"host"))
		{
			if (http->host == NULL)
			{
				http->host = tstrdup(tmp->value);
			}
		}
		else if (!tstrcasecmp(tmp->key,"port"))
		{
			if (http->port == -1)
			{
				http->port = atoi(tmp->value);
			}
		}

		tmp = tmp->next;
	}

	return http;
}

struct http_server *http_server_add(struct http_server *http_server, struct http_server *add)
{
	struct http_server *tmp = NULL;

	if ((tmp = http_server) == NULL)
		return add;

	while (tmp->next != NULL) tmp = tmp->next;

	tmp->next = add;
	add->prev = tmp;

	return http_server;
}

struct http_server *http_server_del(struct http_server *http_server, struct http_server *del)
{
	struct http_server *tmp = NULL;

	if ((tmp = http_server) == NULL)
	{
		log_entry_printf(NULL,NULL,"X","http_server_del() called with NULL server list");
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

	log_entry_printf(NULL,NULL,"X","http_server_del() called with a server deletion that no entry existed for");

	return http_server;

}

void http_server_listen(struct http_server *http)
{
	struct tsocket *tsock;

	tsock       = tsocket_new();
	http->tsock = tsock;

	tsocket_listen(tsock, http->host, http->port);

	tsock->data = http;
	tsock->tsocket_read_cb = http_server_accept;
	tsock->tsocket_write_cb = http_server_accept;


	/* Insert it into the global watchlist */
	if (g_cfg->tsockets == NULL)                                     
		slist_init(&g_cfg->tsockets, tsocket_free);              

	slist_insert_next(g_cfg->tsockets, NULL, (void *)tsock);    

	return;
}

void free_http_http_server(struct http_server *http_server)
{
	struct http_server *xtmp=NULL;
	struct http_server *xold=NULL;

	if (http_server == NULL)
		return;

	xtmp = http_server;

	while (xtmp->prev != NULL)
		xtmp = xtmp->prev;

	while (xtmp != NULL)
	{
		xold = xtmp->next;

		free_http_server(xtmp);

		xtmp = xold;
	}

	return;
}

void free_http_server(struct http_server *http)
{
	free(http->label);
	free(http->host);
	free(http->web_root);
	free(http->username);
	free(http->password);
	tsocket_free(http->tsock);

	free_http_requests(http->requests);
	t_timers_free(http->timers);
/*	free_http_trigger_table(http->http_trigger_table); */

	/* FIXME: free fucking JS shit, PHP shit, etc */

#ifdef HAVE_TCL
	tstrfreev(http->tcl_scripts);
#endif /* HAVE_TCL */

#ifdef HAVE_JS
	tstrfreev(http->js_scripts);
	/* Cleanup Spidermonkey */
	if (http->cx != NULL)
	{
		JS_DestroyContext(http->cx);
	}
#endif /* HAVE_JS */

#ifdef HAVE_PYTHON
	tstrfreev(http->py_scripts);
#endif /* HAVE_PYTHON*/

	free(http);

	return;
}


struct http_server *new_http_server(char *label)
{
	struct http_server *ret;

	ret = tmalloc(sizeof(struct http_server));

	if (label != NULL)
		ret->label = tstrdup(label);
	else
		ret->label = NULL;

	ret->prev          = NULL;
	ret->next          = NULL;

	ret->sock          = -1;
	ret->tsock         = NULL;

	ret->timers        = NULL;

	ret->host          = NULL;
	ret->port          = -1;

	ret->tcfg          = NULL;

	ret->username      = NULL;
	ret->password      = NULL;

	ret->web_root      = NULL;

	ret->filters       = NULL;

	ret->requests      = NULL;

	/* Ridiculously long */
/*	ret->http_trigger_table = new_http_trigger_table(); */


#ifdef HAVE_TCL
	ret->tcl_scripts      = NULL;
	ret->tcl_scripts_size = 0;
	/* net_init_tcl(ret); Need Jabber func */
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
