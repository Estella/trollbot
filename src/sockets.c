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
    svr = net->servers;

    while (svr != NULL)
    {
      if ((net->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
      {
        fprintf(stderr, "Could not create socket for server %s in network %s\n",svr->host,net->label);
        svr = svr->next;
        continue;
      }

      if (net->vhost != NULL)
      {
        if ((vhost = gethostbyname(net->vhost)) == NULL)
        {
          troll_debug(LOG_WARN,"Could not resolve vhost (%s) using default",net->vhost);
          free(net->vhost);
          net->vhost = NULL;
        } 
        else
        {
          vhostip = tmalloc0(3*4+3+1);
          sprintf(vhostip,"%s",inet_ntoa(*((struct in_addr *)vhost->h_addr)));
        }
      }
  
      if ((he = gethostbyname(svr->host)) == NULL) 
      { 
        troll_debug(LOG_WARN,"Could not resolve %s in network %s\n",svr->host,net->label);       
        svr = svr->next;
        continue;
      }

      serv_addr.sin_family = AF_INET;     
      serv_addr.sin_port   = htons(svr->port);   
      serv_addr.sin_addr   = *((struct in_addr *)he->h_addr);
      memset(&(serv_addr.sin_zero), '\0', 8);

      if (net->vhost != NULL)
      {
        my_addr.sin_family = AF_INET;
        my_addr.sin_addr.s_addr = inet_addr(vhostip);
        free(vhostip);
        my_addr.sin_port = htons(0);
        memset(&(my_addr.sin_zero), '\0', 8);

        /* Bind IRC connection to vhost */
        if (bind(net->sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) 
        {
          troll_debug(LOG_WARN,"Could not use vhost: %s",net->vhost);
          free(net->vhost);
          net->vhost = NULL;
        }
      }

      if (connect(net->sock,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) == -1) 
      {
        troll_debug(LOG_WARN,"Could not connect to server %s on port %d for network %s\n",svr->host,svr->port,net->label);
        net->sock = -1;
        svr = svr->next;
        continue;
      }
 
      troll_debug(LOG_DEBUG,"Connected to %s port %d for network %s\n",svr->host,svr->port,net->label);
      net->cur_server = svr;
      net->status     = STATUS_CONNECTED;     
 
      break;
    } 
    
    if (net->sock == -1)
    {
      net->status     = STATUS_DISCONNECTED;
      troll_debug(LOG_ERROR,"Could not connect to any servers for network %s\n",net->label);
    }

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
			/* Remove dead connections */
			if (net->status == STATUS_DISCONNECTED){
				struct network *next=net->next;
				struct network *prev=net->prev;

				net->next=NULL;
				net->prev=NULL;
				free_networks(net);
				net=next;

				if (next != NULL){
					net->prev=prev;
					if (prev != NULL){
						net->prev->next = net;
					}
					else {
						g_cfg->networks = net;
					}
				}
				else if (prev == NULL){
					g_cfg->networks = NULL;
				}
				continue;
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
        numsocks = (net->sock > numsocks) ? net->sock : numsocks;
        FD_SET(net->sock,&socks);
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

      if (net->sock != -1)
      {
        if (FD_ISSET(net->sock,&socks))
        {
          if (net->status == STATUS_CONNECTED)
          {
            irc_printf(net->sock,"USER %s foo.com foo.com :%s",net->ident,net->realname);
            irc_printf(net->sock,"NICK %s",net->nick);
            net->status = STATUS_AUTHORIZED;
          } 

          if (!irc_in(net))
          {
            net->status = STATUS_DISCONNECTED;
          }
        }
      }
 
      net = net->next;
    }  
  }

  return;
}
