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
#include "ics_server.h"
#include "ics_proto.h"
#include "ics_trigger.h"
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

struct ics_server *ics_server_from_tconfig_block(struct tconfig_block *tcfg)
{
	struct ics_server    *ics = NULL;
	struct tconfig_block *tmp = NULL;
	struct server        *svr = NULL;

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
			if (ics->ics_servers != NULL)
			{
				svr = ics->ics_servers;

				while (svr->next != NULL)
					svr = svr->next;

				svr->next         = new_server(tmp->value);
				svr->next->prev   = svr;
				svr               = svr->next;
			}
			else
			{
				ics->ics_servers  = new_server(tmp->value);
				svr          = ics->ics_servers;
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

void ics_server_connect(struct ics_server *ics)
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in my_addr;
	struct hostent *he;

	char *vhostip      = NULL;
	struct server *svr = NULL;

	if ((ics->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		troll_debug(LOG_WARN,"Could not create socket to server for ICS server %s",ics->label);
		return;
	}

	socket_set_nonblocking(ics->sock);

	if (ics->vhost != NULL)
	{
		if ((he = gethostbyname(ics->vhost)) == NULL)
			troll_debug(LOG_WARN,"Could not resolve vhost (%s) using default",ics->vhost);
		else
		{
			vhostip = tmalloc0(3*4+3+1);
			sprintf(vhostip,"%s",inet_ntoa(*((struct in_addr *)he->h_addr)));
		}
	}

	svr = ics->cur_server;

	if (svr == NULL)
	{
		troll_debug(LOG_WARN,"NULL current server for ICS server %s",ics->label);

		ics->status = ICS_DISCONNECTED;
		ics->sock   = -1;
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

	ics->cur_server = svr;

	if ((he = gethostbyname(svr->host)) == NULL)
	{
		troll_debug(LOG_WARN,"Could not resolve %s in ICS server %s\n",svr->host,ics->label);
		ics->status = ICS_DISCONNECTED;
		ics->sock   = -1;
		return;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(svr->port);
	serv_addr.sin_addr   = *((struct in_addr *)he->h_addr);
	memset(&(serv_addr.sin_zero), '\0', 8);

	if (vhostip != NULL)
	{
		my_addr.sin_family = AF_INET;
		my_addr.sin_addr.s_addr = inet_addr(vhostip);
		free(vhostip);
		my_addr.sin_port = htons(0);
		memset(&(my_addr.sin_zero), '\0', 8);

		/* Bind IRC connection to vhost */
		if (bind(ics->sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
			troll_debug(LOG_WARN,"Could not use vhost: %s",ics->vhost);
	}

	if (connect(ics->sock,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) == -1)
	{
		if (errno == EINPROGRESS)
			troll_debug(LOG_DEBUG,"Non-blocking connect(%s) in progress", svr->host);
		else
		{
			troll_debug(LOG_WARN,"Could not connect to server %s at %d",svr->host,svr->port);
			ics->last_try = time(NULL);
			ics->status   = ICS_DISCONNECTED;
			return;
		}

	}
	else
	{
		troll_debug(LOG_DEBUG,"Connected instantly to server %s at %d",svr->host,svr->port);
		/* Connected right away */
		ics->status     = ICS_NOTREADY;
		ics->connected  = 0;
		return;
	}


	ics->status = ICS_NONBLOCKCONNECT;

	return;
}

void free_ics_ics_server(struct ics_server *ics_server)
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

void free_ics_server(struct ics_server *ics)
{
	free(ics->label);
	free(ics->vhost);
	free(ics->shost); 
	free(ics->username);
	free(ics->password);
	free(ics->my_name);

	free_servers(ics->ics_servers);
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

	ret->ics_servers      = NULL; 

	ret->cur_server    = NULL;

	ret->sock          = -1;
	ret->status        = 0;

	ret->timers        = NULL;

	ret->vhost         = NULL;
	ret->shost         = NULL;

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


