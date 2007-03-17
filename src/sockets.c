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
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "main.h"
#include "network.h"
#include "server.h"
#include "sockets.h"
#include "config_engine.h"
#include "irc.h"

void irc_loop(void)
{
  struct hostent *he;
  struct sockaddr_in serv_addr;
  fd_set socks;
  struct network *net;
  struct server  *svr;
  int numsocks = 0;

  net = g_cfg->network_head;

  /* Connect to one server for each network, or mark network unconnectable */
  do
  {
    svr = net->server_head;

    do
    {
      if (svr == NULL)
        break;

      if ((he = gethostbyname(svr->host)) == NULL) 
      { 
        fprintf(stderr,"Could not resolve %s in network %s\n",svr->host,net->label);       
        continue;
      }

      if ((net->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
      {
        fprintf(stderr, "Could not create socket for server %s in network %s\n",svr->host,net->label);
        continue;
      }  

      serv_addr.sin_family = AF_INET;     
      serv_addr.sin_port   = htons(svr->port);   
      serv_addr.sin_addr   = *((struct in_addr *)he->h_addr);
      memset(&(serv_addr.sin_zero), '\0', 8);

      if (connect(net->sock,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) == -1) 
      {
        fprintf(stderr, "Could not connect to server %s on port %d for network %s\n",svr->host,svr->port,net->label);
        net->sock = -1;
        continue;
      }
 
      printf("Connected to %s port %d for network %s\n",svr->host,svr->port,net->label);
      net->cur_server = svr;
      net->status     = STATUS_CONNECTED;     
 
      break;
    } while ((svr = svr->next) != NULL);
    
    if (net->sock == -1)
    {
      fprintf(stderr, "Could not connect to any servers for network %s\n",net->label);
    }

  } while ((net = net->next) != NULL);

  while (1)
  {
    FD_ZERO(&socks);

    net = g_cfg->network_head;

    do
    {
      if (net->sock != -1)
      {
        numsocks = (net->sock > numsocks) ? net->sock : numsocks;
        FD_SET(net->sock,&socks);
      }
    } while ((net = net->next) != NULL);

    select(numsocks+1, &socks, NULL, NULL, NULL);

    net = g_cfg->network_head;

    do
    {
      if (net->sock != -1)
      {
        if (FD_ISSET(net->sock,&socks))
        {
          if (net->status == STATUS_CONNECTED)
          {
            irc_printf(net->sock,"USER %s foo.com foo.com :%s",net->ident,net->realname);
            irc_printf(net->sock,"NICK %s",net->botnick);
            net->status = STATUS_AUTHORIZED;
          } 

          irc_in(net);
        }
      }
    } while ((net = net->next) != NULL);
  }

  return;
}
