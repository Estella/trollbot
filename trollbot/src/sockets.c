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

#include "dcc.h"
#include "servers.h"
#include "main.h"
#include "tconfig.h"

void irc_loop(void)
{
  int sock      = 0,
      connected = 0;
  struct hostent *he, *vhost;
  struct sockaddr_in serv_addr, my_addr;
  char tmpip[200];
  fd_set socks;
  struct dcc_session *dccs;

  if (config->vhost != NULL)
  {
    if ((vhost = gethostbyname(config->vhost)) == NULL)
    {
      troll_debug(LOG_WARN,"Could not resolve: %s",config->vhost);
      free(config->vhost);
      config->vhost = NULL;
    }
  }

  if ((he = gethostbyname(glob_server_head->name)) == NULL) 
  { 
    troll_debug(LOG_WARN,"Could not resolve %s",glob_server_head->name);
    return;
  }

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
  {
    troll_debug(LOG_WARN,"Could not create socket");
    return;
  }  

  serv_addr.sin_family = AF_INET;     
  serv_addr.sin_port = htons(glob_server_head->port);   
  serv_addr.sin_addr = *((struct in_addr *)he->h_addr);
  memset(&(serv_addr.sin_zero), '\0', 8);

  if (config->vhost != NULL)
  {
   sprintf(tmpip,"%s\0",inet_ntoa(*((struct in_addr *)vhost->h_addr)));
   my_addr.sin_family = AF_INET;
   my_addr.sin_addr.s_addr = inet_addr(tmpip);
   my_addr.sin_port = htons(0);
   memset(&(my_addr.sin_zero), '\0', 8);

    /* Bind IRC connection to vhost */
    if (bind(sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) 
    {
      troll_debug(LOG_WARN,"Could not use vhost: %s",config->vhost);

      free(config->vhost);
      config->vhost == NULL;
    }
  }

  if (connect(sock, 
              (struct sockaddr *)&serv_addr,
              sizeof(struct sockaddr))      == -1) 
  {
    troll_debug(LOG_WARN,"Could not connect to server");
    return;
  }

  troll_debug(LOG_DEBUG,"Connected to server");
  connected = 1;

  glob_server_head->sock = sock;

  while (connected)
  {
    FD_ZERO(&socks);
    FD_SET(sock,&socks);

    dccs = glob_dcc_head;
    
    /* Go through DCC connections */
    while (dccs != NULL)
    {
      if (dccs->status > DCC_WAITING)
        FD_SET(dccs->sock,&socks);
 
      dccs = dccs->next;
    }

    connected = select(sock+1, &socks, NULL, NULL, NULL);
 
    if (!connected)
      break;

    if (FD_ISSET(sock,&socks))
    {
      if (!irc_in(sock))
        return;
    }

    /* Check DCC connections */
    dccs = glob_dcc_head;

    while (dccs != NULL)
    {
      if (dccs->status > DCC_WAITING)
        if (FD_ISSET(dccs->sock,&socks))
          dcc_in(dccs);
     
      dccs = dccs->next;
    }
  }

  die_nicely();
  return;
}
