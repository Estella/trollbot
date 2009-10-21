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
 *********************************************
 * This is the generic socket interface for  *
 * Trollbot. Eventually all protocols should *
 * use this to connect to wherever it is     *
 * that it is connecting to or for listening *
 *********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>

#include "tsocket.h"
#include "debug.h"
#include "util.h"

/* For g_cfg */
#include "trollbot.h"

static void tsocket_set_nonblocking(int sock)
{
  int opts;

  opts = fcntl(sock,F_GETFL);

  if (opts < 0)
  {
    troll_debug(LOG_ERROR,"Could not get socket options");
    return;
  }

  opts = (opts | O_NONBLOCK);

  if (fcntl(sock,F_SETFL,opts) < 0)
  {
    troll_debug(LOG_ERROR,"Could not set socket as nonblocking");
  }

  return;
}

ssize_t tsocket_printf(struct tsocket *tsock, const char *fmt, ...)
{
	va_list va;
	char buf[2048];
	char buf2[2059];

	memset(buf, 0, sizeof(buf));
	memset(buf2, 0, sizeof(buf2));

	va_start(va, fmt);

	/* C99 */
	vsnprintf(buf, sizeof(buf), fmt, va);
	va_end(va);

	snprintf(buf2,sizeof(buf2),"%s\n",buf);

	return send(tsock->sock,buf2,strlen(buf2),0);
}

int tsocket_close(struct tsocket *tsock)
{
	if (tsock->sock != -1)
	{
		shutdown(tsock->sock, SHUT_RDWR);
		tsock->status = TSOCK_UNINITIALIZED;
		tsock->sock = -1;
	}

	return 1;
}

int tsocket_check_nonblocking_connect(struct tsocket *tsock)
{
	socklen_t lon  = 0;
	int valopt     = 0;

	if (tsock->status == TSOCK_CONNECTING)
	{
		/* Socket is set as writeable */
		lon = sizeof(int);

		/* Get the current socket options for the non-blocking socket */
		if (getsockopt(tsock->sock, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0)
		{
			tsock->sock   = -1;
			tsock->status = TSOCK_UNINITIALIZED;
			troll_debug(LOG_ERROR,"Could not get socket options for tsocket");
		}
		else
		{
			if (valopt != 0)
			{
				tsock->sock   = -1;
				tsock->status = TSOCK_UNINITIALIZED;
				troll_debug(LOG_ERROR,"Non-blocking connect() to tsocket failed.");
			}
			else
			{
				/* Socket connect succeeded */
				troll_debug(LOG_DEBUG,"Non-blocking connect() to tsocket succeeded");
	
				if (tsock->tsocket_connect_cb != NULL)
					tsock->tsocket_connect_cb(tsock);

				tsock->status = TSOCK_NOTINFDSET;
				return 1;
			}
		}
	}

	return 0;
}

/* The current behavior of this function is to listen on this hostname if it resolves, if not INADDR_ANY
 * Which probably should be changed.
 */
int tsocket_listen(struct tsocket *tsock, const char *hostname, int port)
{
	struct hostent *he;
	char hostip[16];
	struct sockaddr_in my_addr;
	int yes=1;
	int use_hostname = 0;

	memset(hostip, 0, sizeof(hostip));

	/* If a hostname is provided, go ahead and resolve it, get the ip for later binding */
	if (hostname != NULL)
	{
		if ((he = gethostbyname(hostname)) == NULL)
		{
			troll_debug(LOG_WARN,"Could not resolve host (%s) for tsocket listening", hostname);
			use_hostname = 0;
		}
		else
		{
			sprintf(hostip,"%s",inet_ntoa(*((struct in_addr *)he->h_addr_list[0])));

			use_hostname = 1;
		}
	}

	if ((tsock->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		troll_debug(LOG_ERROR,"Could not create socket for tsocket listener");
		return 0;
	}

	/* Get rid of socket in use errors */
	if (setsockopt(tsock->sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		troll_debug(LOG_ERROR,"Could not set socket options");
		return 0;
	}

	my_addr.sin_family      = AF_INET;

	/* If we successfully resolved the hostname, go ahead and use that */
	if (use_hostname == 1)
	{
		my_addr.sin_addr.s_addr = inet_addr(hostip);
	}
	else
	{
		my_addr.sin_addr.s_addr = INADDR_ANY;
	}

	my_addr.sin_port = htons(port);

	memset(&(my_addr.sin_zero), '\0', 8);

	if (bind(tsock->sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
	{
		troll_debug(LOG_ERROR,"Could not bind to tsocket");
		return 0;
	}

	if (listen(tsock->sock, g_cfg->max_listeners) == -1)
	{
		troll_debug(LOG_ERROR,"Could not listen on tsocket");
		return 0;
	}

	tsock->status = TSOCK_LISTENER;

	return 1;
}

int tsocket_connect(struct tsocket *tsock, const char *from_hostname, const char *to_hostname, int port)
{
	char   hostip[16];
	struct sockaddr_in my_addr;
	struct sockaddr_in serv_addr;
	struct hostent *he;

	/* Create the socket
	 * Just working with TCP/IP now
	 */
  if ((tsock->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    troll_debug(LOG_WARN,"tsocket_connect() could not create a socket");
    return 0;
  }

	tsocket_set_nonblocking(tsock->sock);

  /* If a hostname is provided, go ahead and resolve it, get the ip for later binding */
  if (from_hostname != NULL)
  {
    if ((he = gethostbyname(from_hostname)) == NULL)
    {
      troll_debug(LOG_WARN,"tsocket_connect() could not resolve %s for binding", from_hostname);
			return 0;
    }
    else
		{
			/* They provided a hostname to use when connecting out.
			 * make sure it's used or don't try connecting.
			 */
		  memset(hostip, 0, sizeof(hostip));

			sprintf(hostip,"%s",inet_ntoa(*((struct in_addr *)he->h_addr_list[0])));

			my_addr.sin_family      = AF_INET;

			my_addr.sin_addr.s_addr = inet_addr(hostip);

			/* I don't care about the from_port */	
			my_addr.sin_port = htons(0);

			memset(&(my_addr.sin_zero), '\0', 8);

			if (bind(tsock->sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
			{
				troll_debug(LOG_ERROR,"tsocket_connect() could not bind hostname %s to connecting socket", from_hostname);
				return 0;
			}
		}
	}

	/* Now we need to resolve and do a bunch of stuff for the server side
	 * if a from_hostname was specified, it will be bound to the socket at
	 * this point, but does not need messed with further.
	 */
	if ((he = gethostbyname(to_hostname)) == NULL)
	{
		troll_debug(LOG_WARN,"tsocket_connect() could not resolve %s for connection", to_hostname);
		return 0;
	}

	/* Set the server side information */
	memset(hostip, 0, sizeof(hostip));
	sprintf(hostip,"%s",inet_ntoa(*((struct in_addr *)he->h_addr_list[0])));

	serv_addr.sin_family      = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(hostip);
	serv_addr.sin_port = htons(port);

	memset(&(serv_addr.sin_zero), '\0', 8);

	if (connect(tsock->sock,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) == -1)
	{
		if (errno == EINPROGRESS)
			troll_debug(LOG_DEBUG,"tsocket_connect() initiated a non-blocking connection to %s:%d", to_hostname, port);
		else
		{
			troll_debug(LOG_WARN,"tsocket_connect() instantly failed connecting to %s:%d", to_hostname, port);
			/* should probably set it as ignore here */
			return 0;
		}
	}
	else
	{
		troll_debug(LOG_DEBUG,"tsocket_connect() instantly connected to %s:%d", to_hostname, port);

		/* Connected right away
		 * make sure it gets put into the FD_SET the next select run
		 */
		tsock->status     = TSOCK_NOTINFDSET;

		if (tsock->tsocket_connect_cb != NULL)
			tsock->tsocket_connect_cb(tsock);

		return 1;
	}

	tsock->last_attempt = time(NULL);

	/* They're in a non-blocking connect, check for write status in the select loop */
	tsock->status = TSOCK_CONNECTING;

	return 1;
}

struct tsocket *tsocket_new(void)
{
	struct tsocket *tsocket = tmalloc(sizeof(struct tsocket));

	tsocket->status      = TSOCK_UNINITIALIZED;
	tsocket->save_status = TSOCK_UNINITIALIZED;


	tsocket->sock   = -1;
	tsocket->name   = NULL;
	tsocket->data   = NULL;

	tsocket->tsocket_read_cb       = NULL;
	tsocket->tsocket_write_cb      = NULL;
	tsocket->tsocket_connect_cb    = NULL;
	tsocket->tsocket_disconnect_cb = NULL;

	/* Connection Queueing */
	tsocket->last_attempt       = 0;
	tsocket->retry_limit        = 0;
	tsocket->retry_amount       = 0;
	tsocket->retry_wait         = 0;
	
	return tsocket;
}

void tsockets_free(struct slist *tsockets)
{
	struct slist_node *node = NULL;
	struct slist_node *tmp  = NULL;

	if (tsockets == NULL)
		return;
	
	node = tsockets->head;

	while (node != NULL)
	{
		tmp = node;

		node = node->next;

		tsocket_free(tmp->data);
		free(tmp);
	}

	free(tsockets);
}

void tsocket_free(void *tsocket)
{
	struct tsocket *tsock = tsocket;

	free(tsock->name);
	free(tsock);
}

