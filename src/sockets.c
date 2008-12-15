/******************************
 * Trollbot                   *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#include "main.h"
#include "sockets.h"

#include "dcc.h"
#include "irc.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "t_timer.h"
#include "tsocket.h"

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

void socket_set_blocking(int sock)
{
	int opts;

	opts = fcntl(sock,F_GETFL);

	if (opts < 0)
	{
		troll_debug(LOG_ERROR,"Could not get socket options");
		return;
	}

	opts = (opts & O_NONBLOCK);

	if (fcntl(sock,F_SETFL,opts) < 0)
	{
		troll_debug(LOG_ERROR,"Could not set socket as blocking");
	}

	return;
}

void socket_set_nonblocking(int sock)
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

/*  Program Loop? Split up more also */
void irc_loop(void)
{
	fd_set socks;
	fd_set writefds;
	struct timeval timeout;
	struct network *net;
#ifdef HAVE_ICS
	struct ics_server *ics;
#endif /* HAVE_ICS */
#ifdef HAVE_XMPP
	struct xmpp_server *xs;
#endif /* HAVE_XMPP */
	struct dcc_session *dcc;

#ifdef HAVE_HTTP
	struct http_server *http;
#endif /* HAVE_HTTP */

	int numsocks = 0;
	socklen_t lon      = 0;
	int valopt   = 0;
	time_t last = 0;
	struct slist_node *node;
	struct tsocket *tsock;


	/* Rather then all these conditional piece of shits
	 * I made everything generic around these tsocket
	 * thingamajigs, let's introduce this gradually
	 */
	
#ifdef HAVE_ICS
	ics = g_cfg->ics_servers;

	/* Connect to one server for each network, or mark network unconnectable */
	while (ics != NULL)
	{
		/* Shouldn't be done here */
		ics->cur_server = ics->ics_servers;

		ics->last_try = time(NULL);
		ics_server_connect(ics);

		ics = ics->next;
	}
#endif /* HAVE_ICS */

#ifdef HAVE_XMPP
	xs = g_cfg->xmpp_servers;

	/* Connect to one server for each network, or mark network unconnectable */
	while (xs != NULL)
	{
		/* Shouldn't be done here */
		xs->cur_server = xs->servers;

		xs->last_try = time(NULL);
		xs->status   = XMPP_INPROGRESS;
		xmpp_server_connect(xs);

		xs = xs->next;
	}
#endif /* HAVE_XMPP */
#ifdef HAVE_HTTP
	http = g_cfg->http_servers;

	/* Connect to one server for each network, or mark network unconnectable */
	while (http != NULL)
	{
		http->status = HTTP_UNINITIALIZED;
		http_server_listen(http);

		http = http->next;
	}
#endif /* HAVE_HTTP */

	net = g_cfg->networks;

	/* Connect to one server for each network, or mark network unconnectable */
	while (net != NULL)
	{
		/* Shouldn't be done here */
		net->cur_server = net->servers;

		net->last_try = time(NULL);
		net->status   = NET_INPROGRESS;
		network_connect(net);

		net = net->next;
	}

	/* We want this to never interrupt unless told to
	 * There are limbo eggdrop bots which connect to no
	 * network, but operate as relays.
	 */
	while (1)
	{
		FD_ZERO(&socks);
		FD_ZERO(&writefds);

		timeout.tv_sec  = 1;
		timeout.tv_usec = 0;

#ifdef HAVE_XMPP
		xs = g_cfg->xmpp_servers;

		while (xs != NULL)
		{
			if (xs->status == XMPP_DISCONNECTED)
			{
				if (xs->never_give_up == 1)
				{
					if (xs->last_try + xs->connect_delay <= time(NULL))
					{
						xs->status = XMPP_INPROGRESS;
						/* Try a non-blocking connect to the next server */
						xs->last_try = time(NULL);

						xmpp_server_connect(xs);
					}
				}
			}

			if (xs->status == XMPP_NONBLOCKCONNECT || xs->status == XMPP_WAITINGCONNECT)
			{
				numsocks = (xs->sock > numsocks) ? xs->sock : numsocks;

				FD_SET(xs->sock,&writefds);

				/* Now in a FD set */
				xs->status = XMPP_WAITINGCONNECT;
			}

			/* Add each xmpp_server to the read fd set */
			if (xs->sock != -1)
			{
				if (xs->status >= XMPP_NOTREADY)
				{
					if (xs->status == XMPP_NOTREADY)
					{
						xmpp_ball_start_rolling(xs);
						xs->status = XMPP_CONNECTED;
					}

					numsocks = (xs->sock > numsocks) ? xs->sock : numsocks;
					FD_SET(xs->sock,&socks);
				}
			}

			xs = xs->next;
		}

#endif /* HAVE_XMPP */

		net = g_cfg->networks;

		while (net != NULL)
		{
			if (net->status == NET_DISCONNECTED)
			{
				if (net->never_give_up == 1)
				{
					if (net->last_try + net->connect_delay <= time(NULL))
					{
						net->status = NET_INPROGRESS;
						/* Try a non-blocking connect to the next server */
						net->last_try = time(NULL);

						network_connect(net);
					}
				}	
			}

			if (net->status == NET_NONBLOCKCONNECT || net->status == NET_WAITINGCONNECT)
			{
				numsocks = (net->sock > numsocks) ? net->sock : numsocks;

				FD_SET(net->sock,&writefds);

				/* Now in a FD set */
				net->status = NET_WAITINGCONNECT;
			}

			/* If a DCC listener exists, add it to the fd set */
			if (net->dcc_listener != -1)
			{
				FD_SET(net->dcc_listener,&socks);
				numsocks = (net->dcc_listener > numsocks) ? net->dcc_listener : numsocks;
			}

			dcc = net->dccs;

			while (dcc != NULL)
			{
				/* Set all active connections into the read fd set */
				if (dcc->sock != -1 && dcc->status >= DCC_NOTREADY)
				{
					numsocks = (dcc->sock > numsocks) ? dcc->sock : numsocks;
					FD_SET(dcc->sock,&socks);

					/* If it was previously waiting on being in a fd set, set it as connected */
					if (dcc->status == DCC_NOTREADY)
						dcc->status = DCC_CONNECTED;

				}

				/* Waiting on non-blocking connect() call */
				if (dcc->status == DCC_NONBLOCKCONNECT)
				{
					numsocks = (dcc->sock > numsocks) ? dcc->sock : numsocks;

					FD_SET(dcc->sock,&writefds);

					/* Now in a FD set */
					dcc->status = DCC_WAITINGCONNECT;
				}

				dcc = dcc->next;
			}

			/* Add each network to the read fd set */
			if (net->sock != -1)
			{
				if (net->status >= NET_NOTREADY)
				{
					if (net->status == NET_NOTREADY)
						net->status = NET_CONNECTED;

					numsocks = (net->sock > numsocks) ? net->sock : numsocks;
					FD_SET(net->sock,&socks);
				}
			}

			net = net->next;
		}

#ifdef HAVE_HTTP
		http = g_cfg->http_servers;
		
		while (http != NULL)
		{
			if (http->sock == -2)
				continue;

      if (http->sock == -1)
      {
				http_init_listener(http);	
			}

			FD_SET(http->sock,&socks);
			numsocks = (http->sock > numsocks) ? http->sock : numsocks;
			
			http = http->next;
		}
#endif /* HAVE_HTTP */
		/* Check the generic list of tsockets */
		if (g_cfg->tsockets != NULL)
		{
			node = g_cfg->tsockets->head;

			while (node != NULL)
			{
				tsock = node->data;

				if (tsock != NULL)
				{
					switch (tsock->status)
					{
						case TSOCK_CONNECTING:
							FD_SET(tsock->sock, &writefds);
							numsocks = (tsock->sock > numsocks) ? tsock->sock : numsocks;
							break;
						/* Check reads/writes and call their respective callbacks */
						case TSOCK_INFDSET:
						case TSOCK_NOTINFDSET:
							FD_SET(tsock->sock, &socks);
							tsock->status = TSOCK_INFDSET;
							numsocks = (tsock->sock > numsocks) ? tsock->sock : numsocks;
							break;
					}
				}	

				node = node->next;
			}

		}

		select(numsocks+1, &socks, &writefds, NULL, NULL);

		/* Check the generic list of tsockets */
		if (g_cfg->tsockets != NULL)
		{
			node = g_cfg->tsockets->head;

			while (node != NULL)
			{
				tsock = node->data;

				if (tsock != NULL)
				{
					switch (tsock->status)
					{
						case TSOCK_CONNECTING:
							/* Socket is in a non-blocking connect, do some hacks to figure
							 * out if it succeeded or not.
							 */
							if (FD_ISSET(tsock->sock, &writefds))
								tsocket_check_nonblocking_connect(tsock);
							break;
						/* Check reads/writes and call their respective callbacks */
						case TSOCK_INFDSET:
							if (FD_ISSET(tsock->sock, &socks))
								if (tsock->tsocket_read_cb != NULL)
									tsock->tsocket_read_cb(tsock);
							if (FD_ISSET(tsock->sock, &writefds))
								if (tsock->tsocket_write_cb != NULL)
									tsock->tsocket_write_cb(tsock);
							break;
					}
				}	

				node = node->next;
			}

		}

#ifdef HAVE_XMPP
		xs = g_cfg->xmpp_servers;

		while (xs != NULL)
		{
			if (xs->status == XMPP_WAITINGCONNECT)
			{
				if (FD_ISSET(xs->sock, &writefds))
				{
					/* Socket is set as writeable */
					lon = sizeof(int); 

					/* Get the current socket options for the non-blocking socket */
					if (getsockopt(xs->sock, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) 
					{ 
						xs->sock   = -1;
						xs->status = XMPP_DISCONNECTED;
						troll_debug(LOG_ERROR,"Could not get socket options for xmpp_server sock");
					}
					else
					{    
						if (valopt != 0) 
						{ 
							xs->sock   = -1;
							xs->status = XMPP_DISCONNECTED;
							troll_debug(LOG_ERROR,"Non-blocking connect() to xmpp_server failed.");
						}
						else
						{
							/* Socket connect succeeded */
							troll_debug(LOG_DEBUG,"Non-blocking connect() to xmpp_server succeeded");

							/* Set connection as blocking again */
							/* socket_set_blocking(dcc->sock); */


							xs->status = XMPP_NOTREADY;
						}
					}
				}
			}

			if (xs->sock != -1 && xs->status >= XMPP_CONNECTED)
			{
				if (FD_ISSET(xs->sock,&socks))
				{
					if (!xmpp_in(xs))
					{
						xs->status = XMPP_DISCONNECTED;
					}
				}
			}

			xs = xs->next;
		}
#endif /* HAVE_XMPP */

		net = g_cfg->networks;

#ifdef HAVE_HTTP

		http = g_cfg->http_servers;
		
		while (http != NULL)
		{
      /* If a DCC listener exists, add it to the fd set */
      if (http->sock > 0)
      {
        if (FD_ISSET(http->sock,&socks))
				{
					new_http_connection(http);
				}
      }
			
			http = http->next;
		}


#endif /* HAVE_HTTP */


		while (net != NULL)
		{
			if (net->dcc_listener > 0)
			{
				if (FD_ISSET(net->dcc_listener,&socks))
				{
					/* Accept a new connection that's waiting */
					new_dcc_connection(net);
				}
			}

			dcc = net->dccs;

			if (net->status == NET_WAITINGCONNECT)
			{
				if (FD_ISSET(net->sock, &writefds))
				{
					/* Socket is set as writeable */
					lon = sizeof(int); 

					/* Get the current socket options for the non-blocking socket */
					if (getsockopt(net->sock, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) 
					{ 
						net->sock   = -1;
						net->status = NET_DISCONNECTED;
						troll_debug(LOG_ERROR,"Could not get socket options for server sock");
					}
					else
					{    
						if (valopt != 0) 
						{ 
							net->sock   = -1;
							net->status = NET_DISCONNECTED;
							troll_debug(LOG_ERROR,"Non-blocking connect() to server failed.");
						}
						else
						{
							/* Socket connect succeeded */
							troll_debug(LOG_DEBUG,"Non-blocking connect() to server succeeded");

							/* Set connection as blocking again */
							/* socket_set_blocking(dcc->sock); */


							net->status = NET_NOTREADY;
						}
					}
				}
			}

			if (net->status >= NET_CONNECTED && net->timers != NULL)
			{
				if (last <= time(NULL))
				{
					net->timers = t_timers_check(net->timers,time(NULL));
					last = time(NULL);
				}
			}


			while (dcc != NULL)
			{
				/* Remove dead sessions */
				if (dcc->sock == -1){
					struct dcc_session *tmp = dcc->next;
					net->dccs = dcc_list_del(net->dccs,dcc);
					free_dcc_session(dcc);
					dcc = tmp;
					continue;
				}

				/* Read all active DCC socks */
				if (dcc->sock != -1 && dcc->status > DCC_NOTREADY)
				{
					if (FD_ISSET(dcc->sock,&socks))
					{
						if (!dcc_in(dcc))
						{
							struct dcc_session *tmp = dcc->next;
							net->dccs = dcc_list_del(net->dccs,dcc);
							free_dcc_session(dcc);
							dcc = tmp;
							continue;
						}
					}
				}


				if (dcc->status == DCC_WAITINGCONNECT)
				{
					if (FD_ISSET(dcc->sock,&writefds))
					{
						/* Socket is set as writeable */
						lon = sizeof(int); 

						/* Get the current socket options for the non-blocking socket */
						if (getsockopt(dcc->sock, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) 
						{ 
							struct dcc_session *tmp = dcc->next;
							troll_debug(LOG_ERROR,"Could not get socket options for DCC sock");
							net->dccs = dcc_list_del(net->dccs, dcc);
							free_dcc_session(dcc);
							dcc = tmp;
							continue;
						}
						else
						{    
							if (valopt != 0) 
							{ 
								struct dcc_session *tmp = dcc->next;
								troll_debug(LOG_ERROR,"Non-blocking connect() failed.");
								net->dccs = dcc_list_del(net->dccs, dcc);
								free_dcc_session(dcc);
								dcc = tmp;
								continue;
							}
							else
							{
								/* Socket connect succeeded */
								troll_debug(LOG_DEBUG,"Non-blocking connect() succeeded");

								/* Set connection as blocking again */
								/* socket_set_blocking(dcc->sock); */

								irc_printf(dcc->sock,"Welcome to Trollbot.");
								irc_printf(dcc->sock,"Enter your username to continue.");

								dcc->status = DCC_NOTREADY;
							}
						}
					}
				}
				dcc = dcc->next;
			}

			if (net->sock != -1 && net->status >= NET_CONNECTED)
			{
				if (FD_ISSET(net->sock,&socks))
				{
					if (net->status == NET_CONNECTED)
					{
						irc_printf(net->sock,"USER %s foo.com foo.com :%s",net->ident,net->realname);
						irc_printf(net->sock,"NICK %s",net->nick);
						net->status = NET_AUTHORIZED;
					} 

					if (!irc_in(net))
					{
						net->status = NET_DISCONNECTED;
					}
				}
			}

			net = net->next;
		}  
	}

	return;
}
