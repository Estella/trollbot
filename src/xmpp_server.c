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
#include "xmpp_server.h"
#include "xmpp_proto.h"
#include "xmpp_trigger.h"
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

/* returns an algorithm name for the crypto module if it exists */
char *auth_type_get_algo_info(char *name)
{
	if (!tstrcasecmp(name,"DIGEST-MD5"))
		return tstrdup("md5");

	if (!tstrcasecmp(name,"PLAIN"))
		return tstrdup("plain");

	return NULL;
}

struct auth_type *auth_type_add(struct auth_type *auth_types, struct auth_type *add)
{
	struct auth_type *tmp = NULL;

	if ((tmp = auth_types) == NULL)
		return add;

	while (tmp->next != NULL) tmp = tmp->next;

	tmp->next = add;
	add->prev = tmp;

	return auth_types;
}

struct auth_type *auth_type_del(struct auth_type *auth_types, struct auth_type *del)
{
	struct auth_type *tmp = NULL;

	if ((tmp = auth_types) == NULL)
	{
		log_entry_printf(NULL,NULL,"X","auth_type_del() called with NULL auth_type list");
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

	log_entry_printf(NULL,NULL,"X","auth_type_del() called with a auth_type deletion that no entry existed for");

	return auth_types;

}

void free_auth_types(struct auth_type *auth_types)
{
	struct auth_type *xtmp=NULL;
	struct auth_type *xold=NULL;

	if (auth_types == NULL)
		return;

	xtmp = auth_types;

	while (xtmp->prev != NULL)
		xtmp = xtmp->prev;

	while (xtmp != NULL)
	{
		xold = xtmp->next;

		free_auth_type(xtmp);

		xtmp = xold;
	}

	return;
}

void free_auth_type(struct auth_type *at)
{
	free(at->name);
	free(at->algo_name);
	free(at);
}

struct auth_type *new_auth_type(void)
{
	struct auth_type *ret;

	ret = tmalloc(sizeof(struct auth_type));

	ret->name					 = NULL;
	ret->algo_name     = NULL;

	ret->prev          = NULL;
	ret->next          = NULL;

	return ret;
}

struct xmpp_server *xmpp_server_from_tconfig_block(struct tconfig_block *tcfg)
{
	struct xmpp_server   *xs  = NULL;
	struct tconfig_block *tmp = NULL;
	struct server        *svr = NULL;

	if ((tmp = tcfg) == NULL)
	{
		log_entry_printf(NULL,NULL,"X","xmpp_server_from_tconfig_block() called with NULL tconfig block");
		return NULL;
	}

	if (tstrcasecmp(tmp->key,"xmpp_server"))
	{
		log_entry_printf(NULL,NULL,"X","xmpp_server_from_tconfig_block() called with invalid block type");
		return NULL;
	}

	if (tmp->child == NULL)
	{
		log_entry_printf(NULL,NULL,"X","xmpp_server_from_tconfig_block() called with a xmpp_server with no fucking child");
		return NULL;
	}

	xs  = new_xmpp_server(tmp->value);
	tmp = tmp->child;

	while (tmp != NULL)
	{
		if (!tstrcasecmp(tmp->key,"server"))
		{
			if (xs->servers != NULL)
			{
				svr = xs->servers;

				while (svr->next != NULL)
					svr = svr->next;

				svr->next         = new_server(tmp->value);
				svr->next->prev   = svr;
				svr               = svr->next;
			}
			else
			{
				xs->servers  = new_server(tmp->value);
				svr          = xs->servers;
				svr->prev    = NULL;
				svr->next    = NULL;
			}
		}

		tmp = tmp->next;
	}

	return xs;
}

struct xmpp_server *xmpp_server_add(struct xmpp_server *servers, struct xmpp_server *add)
{
	struct xmpp_server *tmp = NULL;

	if ((tmp = servers) == NULL)
		return add;

	while (tmp->next != NULL) tmp = tmp->next;

	tmp->next = add;
	add->prev = tmp;

	return servers;
}

struct xmpp_server *xmpp_server_del(struct xmpp_server *servers, struct xmpp_server *del)
{
	struct xmpp_server *tmp = NULL;

	if ((tmp = servers) == NULL)
	{
		log_entry_printf(NULL,NULL,"X","xmpp_server_del() called with NULL server list");
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

	log_entry_printf(NULL,NULL,"X","xmpp_server_del() called with a server deletion that no entry existed for");

	return servers;

}

void xmpp_server_connect(struct xmpp_server *xs)
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in my_addr;
	struct hostent *he;

	char *vhostip      = NULL;
	struct server *svr = NULL;

	if ((xs->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		troll_debug(LOG_WARN,"Could not create socket to server for XMPP server %s",xs->label);
		return;
	}

	socket_set_nonblocking(xs->sock);

	if (xs->vhost != NULL)
	{
		if ((he = gethostbyname(xs->vhost)) == NULL)
			troll_debug(LOG_WARN,"Could not resolve vhost (%s) using default",xs->vhost);
		else
		{
			vhostip = tmalloc0(3*4+3+1);
			sprintf(vhostip,"%s",inet_ntoa(*((struct in_addr *)he->h_addr_list[0])));
		}
	}

	svr = xs->cur_server;

	if (svr == NULL)
	{
		troll_debug(LOG_WARN,"NULL current server for XMPP server %s",xs->label);

		xs->status = XMPP_DISCONNECTED;
		xs->sock   = -1;
		return;
	}

	/* Move on to the next one on the list, if exhausted, start over */
	if (svr->next == NULL)
	{
		while (svr->prev != NULL)
			svr = svr->prev;
	}
	else
		svr = svr->next;

	xs->cur_server = svr;

	if ((he = gethostbyname(svr->host)) == NULL)
	{
		troll_debug(LOG_WARN,"Could not resolve %s in XMPP server %s\n",svr->host,xs->label);
		xs->status = XMPP_DISCONNECTED;
		xs->sock   = -1;
		return;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(svr->port);
	serv_addr.sin_addr   = *((struct in_addr *)he->h_addr_list[0]);
	memset(&(serv_addr.sin_zero), '\0', 8);

	if (vhostip != NULL)
	{
		my_addr.sin_family = AF_INET;
		my_addr.sin_addr.s_addr = inet_addr(vhostip);
		free(vhostip);
		my_addr.sin_port = htons(0);
		memset(&(my_addr.sin_zero), '\0', 8);

		/* Bind IRC connection to vhost */
		if (bind(xs->sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
			troll_debug(LOG_WARN,"Could not use vhost: %s",xs->vhost);
	}

	if (connect(xs->sock,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) == -1)
	{
		if (errno == EINPROGRESS)
			troll_debug(LOG_DEBUG,"Non-blocking connect(%s) in progress", svr->host);
		else
		{
			troll_debug(LOG_WARN,"Could not connect to server %s at %d",svr->host,svr->port);
			xs->last_try = time(NULL);
			xs->status   = XMPP_DISCONNECTED;
			return;
		}

	}
	else
	{
		troll_debug(LOG_DEBUG,"Connected instantly to server %s at %d",svr->host,svr->port);
		/* Connected right away */
		xs->status     = XMPP_NOTREADY;
		return;
	}


	xs->status = XMPP_NONBLOCKCONNECT;

	return;
}

void free_xmpp_servers(struct xmpp_server *servers)
{
	struct xmpp_server *xtmp=NULL;
	struct xmpp_server *xold=NULL;

	if (servers == NULL)
		return;

	xtmp = servers;

	while (xtmp->prev != NULL)
		xtmp = xtmp->prev;

	while (xtmp != NULL)
	{
		xold = xtmp->next;

		free_xmpp_server(xtmp);

		xtmp = xold;
	}

	return;
}

void free_xmpp_server(struct xmpp_server *xs)
{
	free(xs->label);
	free(xs->vhost);
	free(xs->shost); 

	free_servers(xs->servers);
	t_timers_free(xs->timers);
	free_xmpp_trigger_table(xs->xmpp_trigger_table);

	/* FIXME: free fucking JS shit, PHP shit, etc */

#ifdef HAVE_TCL
	tstrfreev(xs->tcl_scripts);
#endif /* HAVE_TCL */

#ifdef HAVE_JS
	tstrfreev(xs->js_scripts);
	/* Cleanup Spidermonkey */
	if (xs->cx != NULL)
	{
		JS_DestroyContext(xs->cx);
	}
#endif /* HAVE_JS */

#ifdef HAVE_PYTHON
	tstrfreev(xs->py_scripts);
#endif /* HAVE_PYTHON*/

	free(xs);

	return;
}


struct xmpp_server *new_xmpp_server(char *label)
{
	struct xmpp_server *ret;

	ret = tmalloc(sizeof(struct xmpp_server));

	if (label != NULL)
		ret->label = tstrdup(label);
	else
		ret->label = NULL;

	ret->prev          = NULL;
	ret->next          = NULL;

	ret->servers       = NULL; 

	ret->cur_server    = NULL;

	ret->sock          = -1;
	ret->status        = 0;

	ret->timers        = NULL;

	ret->vhost         = NULL;
	ret->shost         = NULL;

	ret->never_give_up = -1;
	ret->connect_delay = -1;

	ret->last_try      = 0;

	ret->tcfg          = NULL;

	ret->never_give_up = -1;
	/* This is the queueing BS */
	ret->connect_delay = -1; 
	ret->last_try      = 0;  

	ret->filters       = NULL;

	/* Ridiculously long */
	ret->xmpp_trigger_table = new_xmpp_trigger_table();


	ret->auth_types_remote = NULL;
	ret->auth_types_local  = NULL;

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


