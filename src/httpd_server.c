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

#include "main.h"
#include "httpd_server.h"
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


struct httpd_server *httpd_server_from_tconfig_block(struct tconfig_block *tcfg)
{
	struct httpd_server    *httpd = NULL;
	struct tconfig_block   *tmp = NULL;

	if ((tmp = tcfg) == NULL)
	{
		log_entry_printf(NULL,NULL,"X","httpd_server_from_tconfig_block() called with NULL tconfig block");
		return NULL;
	}

	if (tstrcasecmp(tmp->key,"httpd_server"))
	{
		log_entry_printf(NULL,NULL,"X","httpd_server_from_tconfig_block() called with invalid block type");
		return NULL;
	}

	if (tmp->child == NULL)
	{
		log_entry_printf(NULL,NULL,"X","httpd_server_from_tconfig_block() called with a httpd_server with no fucking child");
		return NULL;
	}

	httpd  = new_httpd_server(tmp->value);
	tmp = tmp->child;

	while (tmp != NULL)
	{
		if (!tstrcasecmp(tmp->key,"username"))
		{
			if (httpd->username == NULL)
			{
				httpd->username = tstrdup(tmp->value);
			}
		}
		else if (!tstrcasecmp(tmp->key,"password"))
		{
			if (httpd->password == NULL)
			{
				httpd->password = tstrdup(tmp->value);
			}
		}
		else if (!tstrcasecmp(tmp->key,"web_root"))
		{
			if (httpd->web_root == NULL)
			{
				httpd->web_root = tstrdup(tmp->value);
			}
		}
		else if (!tstrcasecmp(tmp->key,"host"))
		{
			if (httpd->host == NULL)
			{
				httpd->host = tstrdup(tmp->value);
			}
		}
		else if (!tstrcasecmp(tmp->key,"port"))
		{
			if (httpd->port == -1)
			{
				httpd->port = atoi(tmp->value);
			}
		}



		tmp = tmp->next;
	}

	return httpd;
}

struct httpd_server *httpd_server_add(struct httpd_server *httpd_server, struct httpd_server *add)
{
	struct httpd_server *tmp = NULL;

	if ((tmp = httpd_server) == NULL)
		return add;

	while (tmp->next != NULL) tmp = tmp->next;

	tmp->next = add;
	add->prev = tmp;

	return httpd_server;
}

struct httpd_server *httpd_server_del(struct httpd_server *httpd_server, struct httpd_server *del)
{
	struct httpd_server *tmp = NULL;

	if ((tmp = httpd_server) == NULL)
	{
		log_entry_printf(NULL,NULL,"X","httpd_server_del() called with NULL server list");
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

	log_entry_printf(NULL,NULL,"X","httpd_server_del() called with a server deletion that no entry existed for");

	return httpd_server;

}

void httpd_server_listen(struct httpd_server *httpd)
{
	struct sockaddr_in my_addr;
	struct hostent *he;
	int yes=1;

	char *hostip       = NULL;

	if ((httpd == NULL) || httpd->host == NULL)
	{
		troll_debug(LOG_ERROR, "httpd_server_listen() called with NULL httpd or NULL host.");
		return;
	}

	if ((httpd->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		troll_debug(LOG_WARN,"Could not create socket to server for HTTPD server %s",httpd->label);
		return;
	}

	socket_set_nonblocking(httpd->sock);

	if (setsockopt(httpd->sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		troll_debug(LOG_ERROR,"Could not set socket options for HTTPD server %s",httpd->label);
		return;
	}

	if ((he = gethostbyname(httpd->host)) == NULL)
		troll_debug(LOG_WARN,"Could not resolve HTTPD host (%s) using default",httpd->host);
	else
	{
		hostip = tmalloc0(3*4+3+1);
		sprintf(hostip,"%s",inet_ntoa(*((struct in_addr *)he->h_addr)));
	}

  my_addr.sin_family = AF_INET;

  my_addr.sin_addr.s_addr = inet_addr(hostip);

  if (httpd->port == -1)
  {
    httpd->port = 4964;
  }

  my_addr.sin_port = htons(httpd->port);

  memset(&(my_addr.sin_zero), '\0', 8);

  if (bind(httpd->sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
  {
    troll_debug(LOG_ERROR,"Could not bind to HTTPD socket");
    free(hostip);
    return;
  }

  if (listen(httpd->sock, HTTPD_MAX) == -1)
  {
    troll_debug(LOG_ERROR,"Could not listen on HTTPD socket");
    free(hostip);
    return;
  }

  troll_debug(LOG_DEBUG,"Listening on %s port %d\n",hostip,httpd->port);
  free(hostip);

	httpd->status = HTTPD_LISTENING;

	return;
}

void free_httpd_httpd_server(struct httpd_server *httpd_server)
{
	struct httpd_server *xtmp=NULL;
	struct httpd_server *xold=NULL;

	if (httpd_server == NULL)
		return;

	xtmp = httpd_server;

	while (xtmp->prev != NULL)
		xtmp = xtmp->prev;

	while (xtmp != NULL)
	{
		xold = xtmp->next;

		free_httpd_server(xtmp);

		xtmp = xold;
	}

	return;
}

void free_httpd_server(struct httpd_server *httpd)
{
	free(httpd->label);
	free(httpd->host);
	free(httpd->web_root);
	free(httpd->username);
	free(httpd->password);

	t_timers_free(httpd->timers);
/*	free_httpd_trigger_table(httpd->httpd_trigger_table); */

	/* FIXME: free fucking JS shit, PHP shit, etc */

#ifdef HAVE_TCL
	tstrfreev(httpd->tcl_scripts);
#endif /* HAVE_TCL */

#ifdef HAVE_JS
	tstrfreev(httpd->js_scripts);
	/* Cleanup Spidermonkey */
	if (httpd->cx != NULL)
	{
		JS_DestroyContext(httpd->cx);
	}
#endif /* HAVE_JS */

#ifdef HAVE_PYTHON
	tstrfreev(httpd->py_scripts);
#endif /* HAVE_PYTHON*/

	free(httpd);

	return;
}


struct httpd_server *new_httpd_server(char *label)
{
	struct httpd_server *ret;

	ret = tmalloc(sizeof(struct httpd_server));

	if (label != NULL)
		ret->label = tstrdup(label);
	else
		ret->label = NULL;

	ret->prev          = NULL;
	ret->next          = NULL;

	ret->sock          = -1;
	ret->status        = HTTPD_UNINITIALIZED;

	ret->timers        = NULL;

	ret->host          = NULL;
	ret->port          = -1;

	ret->tcfg          = NULL;

	ret->username      = NULL;
	ret->password      = NULL;

	ret->web_root      = NULL;

	ret->filters       = NULL;

	/* Ridiculously long */
/*	ret->httpd_trigger_table = new_httpd_trigger_table(); */


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
