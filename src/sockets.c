/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
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

void irc_loop(void)
{
  struct hostent *he, *vhost;
  struct sockaddr_in serv_addr, my_addr;
  fd_set socks;
  fd_set writefds;
  struct timeval timeout;
  struct network *net;
  struct server  *svr;
  struct dcc_session *dcc;
  int numsocks = 0;
  socklen_t lon      = 0;
  int valopt   = 0;
  char *vhostip = NULL;

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

  while (g_cfg->networks != NULL)
  {
    FD_ZERO(&socks);
    FD_ZERO(&writefds);

    net = g_cfg->networks;

    /* Set a timeout of 1 second */
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

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

    select(numsocks+1, &socks, &writefds, NULL, &timeout);

    net = g_cfg->networks;

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

      while (dcc != NULL)
      {
        /* Remove dead sessions */
        if (dcc->sock == -1){
					struct dcc_session *tmp = dcc->next;
					dcc_list_del(&net->dccs, dcc);
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
              dcc_list_del(&net->dccs,dcc);
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
              dcc_list_del(&net->dccs,dcc);
              dcc = tmp;
              continue;
            }
            else
            {    
              if (valopt != 0) 
              { 
                struct dcc_session *tmp = dcc->next;
                troll_debug(LOG_ERROR,"Non-blocking connect() failed.");
                dcc_list_del(&net->dccs,dcc);
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
