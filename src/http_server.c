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
	struct sockaddr_in my_addr;
	struct hostent *he;
	int yes=1;

	char *hostip       = NULL;

	if ((http == NULL) || http->host == NULL)
	{
		troll_debug(LOG_ERROR, "http_server_listen() called with NULL http or NULL host.");
		return;
	}

	if ((http->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		troll_debug(LOG_WARN,"Could not create socket to server for HTTP server %s",http->label);
		return;
	}

	socket_set_nonblocking(http->sock);

	if (setsockopt(http->sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		troll_debug(LOG_ERROR,"Could not set socket options for HTTP server %s",http->label);
		return;
	}

	if ((he = gethostbyname(http->host)) == NULL)
		troll_debug(LOG_WARN,"Could not resolve HTTP host (%s) using default",http->host);
	else
	{
		hostip = tmalloc0(3*4+3+1);
		sprintf(hostip,"%s",inet_ntoa(*((struct in_addr *)he->h_addr_list[0])));
	}

  my_addr.sin_family = AF_INET;

  my_addr.sin_addr.s_addr = inet_addr(hostip);

  if (http->port == -1)
  {
    http->port = 4964;
  }

  my_addr.sin_port = htons(http->port);

  memset(&(my_addr.sin_zero), '\0', 8);

  if (bind(http->sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
  {
    troll_debug(LOG_ERROR,"Could not bind to HTTP socket");
    free(hostip);
    return;
  }

  if (listen(http->sock, HTTP_MAX) == -1)
  {
    troll_debug(LOG_ERROR,"Could not listen on HTTP socket");
    free(hostip);
    return;
  }

  troll_debug(LOG_DEBUG,"Listening on %s port %d\n",hostip,http->port);
  free(hostip);

	http->status = HTTP_LISTENING;

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
	ret->status        = HTTP_UNINITIALIZED;

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
