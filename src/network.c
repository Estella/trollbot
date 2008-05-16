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
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "trigger.h"
#include "dcc.h"
#include "t_timer.h"
#include "sockets.h"
#include "debug.h"
#include "log_filter.h"

#ifdef HAVE_TCL
#include "tcl_embed.h"
#endif /* HAVE_TCL */

#ifdef HAVE_PERL
#include "perl_embed.h"
#endif /* HAVE_PERL */

#ifdef HAVE_JS
#include "js_embed.h"
#endif /* HAVE_JS */

/* The plan for this function is to do a test of commonly needed
 * server capabilities, like maximum message length, etc for use
 * by scripting to determine how to split up stuff and whatnot.
 */
void network_capabilities_test(struct network *net)
{
	return;
}

struct network_capabilities *network_capabilities_new(void)
{
	struct network_capabilities *ret;

	ret->nicklen    = -1;
  ret->chanellen  = -1;
  ret->kicklen    = -1;
  ret->topiclen   = -1;
  ret->awaylen    = -1;
  ret->maxtargets = -1;

  ret->chantypes  = NULL;
  ret->statusmsg  = NULL;
  ret->prefix     = NULL;
  ret->modes      = NULL;

  ret->modes_with_args = NULL;

	return ret;
}

void network_capabilities_free(struct network_capabilities *caps)
{
	free(caps->chantypes);
	free(caps->statusmsg);
	free(caps->prefix);
	free(caps->modes);
	free(caps->modes_with_args);

	free(caps);
}

struct channel *network_channel_find(struct network *net, const char *name)
{
	struct channel *chan = net->chans;

	while (chan != NULL)
	{
		if (!tstrcasecmp(chan->name,name))
		{
			break;
		}
		chan = chan->next;
	}

	return chan;
}

void network_connect(struct network *net)
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in my_addr;
	struct hostent *he;

	char *vhostip      = NULL;
	struct server *svr = NULL;

	if ((net->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		troll_debug(LOG_WARN,"Could not create socket to server for network %s",net->label);
		return;
	}

	socket_set_nonblocking(net->sock);

	if (net->vhost != NULL)
	{
		if ((he = gethostbyname(net->vhost)) == NULL)
			troll_debug(LOG_WARN,"Could not resolve vhost (%s) using default",net->vhost);
		else
		{
			vhostip = tmalloc0(3*4+3+1);
			sprintf(vhostip,"%s",inet_ntoa(*((struct in_addr *)he->h_addr)));
		}
	}

	svr = net->cur_server;

	if (svr == NULL)
	{
		troll_debug(LOG_WARN, "NULL current server for network %s",net->label);

		net->status = NET_DISCONNECTED;
		net->sock   = -1;
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

	net->cur_server = svr;

	if ((he = gethostbyname(svr->host)) == NULL)
	{
		troll_debug(LOG_WARN,"Could not resolve %s in network %s\n",svr->host,net->label);
		net->status = NET_DISCONNECTED;
		net->sock   = -1;
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
		if (bind(net->sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
			troll_debug(LOG_WARN,"Could not use vhost: %s",net->vhost);
	}

	if (connect(net->sock,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) == -1)
	{
		if (errno == EINPROGRESS)
			troll_debug(LOG_DEBUG,"Non-blocking connect(%s) in progress", svr->host);
		else
		{
			troll_debug(LOG_WARN,"Could not connect to server %s at %d",svr->host,svr->port);
			net->last_try = time(NULL);
			net->status   = NET_DISCONNECTED;
			return;
		}

	}
	else
	{
		troll_debug(LOG_DEBUG,"Connected instantly to server %s at %d",svr->host,svr->port);
		/* Connected right away */
		net->status     = NET_NOTREADY;
		return;
	}


	net->status = NET_NONBLOCKCONNECT;

	return;
}

void free_networks(struct network *net)
{
	struct network *ntmp=NULL;

	if (net == NULL)
		return;

	while (net->prev != NULL)
		net = net->prev;

	while (net != NULL)
	{
		if (net->label){  free(net->label); }
		if (net->botnick){  free(net->botnick); }
		if (net->nick){  free(net->nick); }
		if (net->altnick){  free(net->altnick); }
		if (net->ident){  free(net->ident); }
		if (net->realname){  free(net->realname); }
		if (net->vhost){  free(net->vhost); }
		if (net->shost){  free(net->shost); }
		if (net->userfile){ free(net->userfile);  }
		if (net->chanfile){ free(net->chanfile);  }

		/* hey kicken, free(NULL) is defined and perfectly fine behavior 
		 * No need to check pointers beforehand. If it crashes, it's because
		 * it's not set to NULL in the new_network() 	
		 */
		free(net->dcc_motd);

		free_trigger_table(net->trigs);

		free_users(net->users);

		free_channels(net->chans);

		free_servers(net->servers);

		t_timers_free(net->timers);

		/* FIXME: free fucking JS shit, PHP shit, etc */

		free_dcc_sessions(net->dccs);

#ifdef HAVE_TCL
		tstrfreev(net->tcl_scripts);
#endif /* HAVE_TCL */

#ifdef HAVE_JS
		tstrfreev(net->js_scripts);
		/* Cleanup Spidermonkey */
		if (net->cx != NULL){
			JS_DestroyContext(net->cx);
		}
#endif /* HAVE_JS */

#ifdef HAVE_PYTHON
		tstrfreev(net->py_scripts);
#endif /* HAVE_PYTHON*/

		ntmp = net;
		net  = net->next;
		free(ntmp);
	}

	return;
}


struct network *new_network(char *label)
{
	struct network *ret;

	ret = tmalloc(sizeof(struct network));

	if (label != NULL)
		ret->label = tstrdup(label);
	else
		ret->label = NULL;

	ret->prev          = NULL;
	ret->next          = NULL;

	ret->servers       = NULL; 

	ret->chans         = NULL;

	ret->cur_server    = NULL;

	ret->sock          = -1;
	ret->status        = 0;

	ret->botnick       = NULL;
	ret->nick          = NULL;
	ret->altnick       = NULL;
	ret->realname      = NULL;
	ret->ident         = NULL;

	ret->dcc_motd      = NULL;

	ret->users         = NULL;

	ret->timers        = NULL;
	ret->trigs         = new_trig_table();

	ret->vhost         = NULL;
	ret->shost         = NULL;

	ret->userfile      = NULL;
	ret->chanfile      = NULL;

	ret->never_give_up = -1;
	ret->connect_delay = -1;

	ret->last_try      = 0;

	ret->dccs          = NULL;

	ret->dcc_listener  = -1;
	ret->dcc_port      = -1;

	ret->tcfg          = NULL;

	ret->never_give_up = -1;
	/* This is the queueing BS */
	ret->connect_delay = -1; 
	ret->last_try      = 0;  

	ret->handlen       = 32;

	ret->filters       = NULL;
	ret->caps          = NULL;
#ifdef HAVE_TCL
	ret->tcl_scripts      = NULL;
	ret->tcl_scripts_size = 0;
	net_init_tcl(ret);
#endif /* HAVE_TCL */

#ifdef HAVE_PERL
	net_init_perl(ret);
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


