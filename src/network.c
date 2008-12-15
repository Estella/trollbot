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
#include "irc_trigger.h"
#include "dcc.h"
#include "t_timer.h"
#include "sockets.h"
#include "debug.h"
#include "log_filter.h"
#include "egg_lib.h"
#include "troll_lib.h"
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

void irc_ball_start_rolling(struct tsocket *tsock)
{
	struct network *net = tsock->data;

	tsocket_printf(net->tsock,"USER %s foo.com foo.com :%s",net->ident,net->realname);
	tsocket_printf(net->tsock,"NICK %s",net->nick);

	return;
}

struct user *network_user_find_by_hostmask(struct network *net, const char *hostmask)
{
	struct user *tmpusr;
	
	tmpusr = net->users;

	while (tmpusr != NULL)
	{
		if (!troll_matchwilds(tmpusr->uhost, hostmask))
			return tmpusr;

		tmpusr = tmpusr->next;
	}

	return NULL;
}

struct user *network_user_find_by_username(struct network *net, const char *username)
{
	struct user *tmpusr;

	tmpusr = net->users;

	while (tmpusr != NULL)
	{
		if (!tstrcasecmp(tmpusr->username, username))
			return tmpusr;
	
		tmpusr = tmpusr->next;
	}

	return NULL;
}

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
	struct tsocket *tsock;
	struct server  *srv;

	tsock = tsocket_new();

	/* Assign it in the irc_server */
	tsock->data = net;
	net->tsock  = tsock;

	tsock->name = tstrdup(net->label);

	/* in irc_proto.c */
	tsock->tsocket_read_cb    = irc_in;
	tsock->tsocket_write_cb   = NULL;
	tsock->tsocket_connect_cb = irc_ball_start_rolling;

	/* Find a suitable network server */
	srv = net->servers;

	while (srv != NULL)
	{
		if (srv->host != NULL)
			if (tsocket_connect(tsock, srv->vhost, srv->host, srv->port))
				break;

		srv = srv->next;
	}

	if (srv == NULL)
	{
		troll_debug(LOG_WARN, "Could not connect to any servers for IRC network %s", net->label);
		tsocket_free(tsock);
		return;
	}

	/* Insert it into the global check list */
	if (g_cfg->tsockets == NULL)
		slist_init(&g_cfg->tsockets, tsocket_free);

	slist_insert_next(g_cfg->tsockets, NULL, (void *)tsock);

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
		free(net->label);
		free(net->botnick);
		free(net->nick);
		free(net->altnick);
		free(net->ident);
		free(net->realname);
		free(net->vhost);
		free(net->shost);
		free(net->userfile);
		free(net->chanfile);
		tsocket_free(net->tsock);

		/* hey kicken, free(NULL) is defined and perfectly fine behavior 
		 * No need to check pointers beforehand. If it crashes, it's because
		 * it's not set to NULL in the new_network() 	
		 */
		free(net->dcc_motd);
		free(net->default_flags);

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

	ret->tsock         = NULL;
	ret->status        = 0;

	ret->botnick       = NULL;
	ret->nick          = NULL;
	ret->altnick       = NULL;
	ret->default_flags = NULL;
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


