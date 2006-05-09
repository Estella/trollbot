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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "main.h"

#include "servers.h"
#include "irc.h"

int dcc_connect(struct irc_data *data, const char *line)
{
  int dcc_sock = 0;
  unsigned long ip = 0;
  unsigned int port = 0;
  struct sockaddr_in req_addr;
  struct dcc_session *dccs;

  ip = strtoul(data->rest[3],(char **)'\0',10);
  port = atoi(data->rest[4]);

  /* error checking */

  if ((dcc_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    troll_debug(LOG_WARN,"Could not create dcc socket");
    return;
  }

  req_addr.sin_family      = AF_INET;
  req_addr.sin_addr.s_addr = htonl(ip);
  req_addr.sin_port        = htons(port);

  memset(&(req_addr.sin_zero), '\0', 8);

  if (connect(dcc_sock,
              (struct sockaddr *)&req_addr,
              sizeof(struct sockaddr))      == -1)
  {
    troll_debug(LOG_WARN,"Could not connect to dcc user at %d",port);
    return;
  }
    
  dccs = glob_dcc_head;

  if (dccs == NULL)
  {
    glob_dcc_head = malloc(sizeof(struct dcc_session));
    glob_dcc_head->prev = NULL; 
    dccs = glob_dcc_head; 
    dccs->id = 0;
  } else {
    glob_dcc_tail->next = malloc(sizeof(struct dcc_session));
    glob_dcc_tail->next->prev = glob_dcc_tail;
    glob_dcc_tail->next->id   = glob_dcc_tail->id + 1;
    glob_dcc_tail = glob_dcc_tail->next;
    dccs = glob_dcc_tail;
  }

  dccs->sock   = dcc_sock;
  dccs->user   = tstrdup(data->prefix->nick);
  dccs->status = DCC_CONNECT;

  dccs->next = NULL;

  irc_printf(dcc_sock,"You've been connected\n");  
  irc_printf(glob_server_head->sock,"PRIVMSG #tcl :got dcc request from %s "
                                    "IP(long): %s Port: %s",data->prefix->nick,data->rest[3],data->rest[4]);
  return 1;
}

int dcc_in(struct dcc_session *dcc)
{
  
  return 1;
}
