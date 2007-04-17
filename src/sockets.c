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

#include "main.h"
#include "sockets.h"

#include "dcc.h"
#include "irc.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"

void irc_loop(void)
{
  struct hostent *he, *vhost;
  struct sockaddr_in serv_addr, my_addr;
  fd_set socks;
  struct network *net;
  struct server  *svr;
  struct dcc_session *dcc;
  int numsocks = 0;
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
        fprintf(stderr, "Could not connect to server %s on port %d for network %s\n",svr->host,svr->port,net->label);
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
      troll_debug(LOG_ERROR,"Could not connect to any servers for network %s\n",net->label);
    }

    net = net->next;
  }

  while (1)
  {
    FD_ZERO(&socks);

    net = g_cfg->networks;

    while (net != NULL)
    {
      if (net->sock != -1)
      {
        numsocks = (net->sock > numsocks) ? net->sock : numsocks;
        FD_SET(net->sock,&socks);
      }
 
      net = net->next;
    }

    dcc = g_cfg->dccs;
 
    while (dcc != NULL)
    {
      if (dcc->sock != -1)
      {
        numsocks = (dcc->sock > numsocks) ? dcc->sock : numsocks;
        FD_SET(dcc->sock,&socks);
      }

      dcc = dcc->next;
    }

    if (g_cfg->dcc_listener != -1)
    {
      FD_SET(g_cfg->dcc_listener,&socks);
      numsocks = (g_cfg->dcc_listener > numsocks) ? g_cfg->dcc_listener : numsocks;
    }

    select(numsocks+1, &socks, NULL, NULL, NULL);

    net = g_cfg->networks;

    while (net != NULL)
    {
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

          irc_in(net);
        }
      }
 
      net = net->next;
    } 
 
    dcc = g_cfg->dccs;
   
    while (dcc != NULL)
    {
      if (dcc->sock != -1)
      {
        if (FD_ISSET(dcc->sock,&socks))
        {
          dcc_in(dcc);
        }
      }
      
      dcc = dcc->next;
    }

    if (g_cfg->dcc_listener != -1)
    {
      new_dcc_connection(g_cfg->dcc_listener);
    } 
  }

  return;
}
