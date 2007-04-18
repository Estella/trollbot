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
#include "irc.h"
#include "dcc.h"
#include "user.h"
#include "sha1.h"
#include "network.h"
#include "egg_lib.h"
#include "trigger.h"

void init_dcc_listener(struct network *net)
{
  char *dcchostip;
  char *dcchost;
  struct sockaddr_in dccaddr;
  struct hostent *he;
  int yes=1;

  if (net->shost == NULL)
  {
    if (net->vhost == NULL)
    {
      troll_debug(LOG_WARN,"Neither a valid vhost, nor a valid server host exists for a DCC connection");
      return;
    }
  }

  dcchost   = (net->shost != NULL) ? net->shost : net->vhost;
  dcchostip = NULL;

  if ((he = gethostbyname(dcchost)) == NULL)
  {
    troll_debug(LOG_WARN,"Could not resolve host (%s) for DCC listening",dcchost);
    return;
  }

  dcchostip = tmalloc0(3*4+3+1);
  sprintf(dcchostip,"%s",inet_ntoa(*((struct in_addr *)he->h_addr)));
 
  if ((net->dcc_listener = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
  {
    troll_debug(LOG_ERROR,"Could not create socket for DCC listener");
    return;
  }

  if (setsockopt(net->dcc_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
  {
    troll_debug(LOG_ERROR,"Could not set socket options");
    return;
  }

  dccaddr.sin_family = AF_INET;

  dccaddr.sin_addr.s_addr = inet_addr(dcchostip);
  free(dcchostip);

  if (g_cfg->dcc_port == -1)
    dccaddr.sin_port = htons(4928);
  else
    dccaddr.sin_port = htons(g_cfg->dcc_port);

  memset(&(dccaddr.sin_zero), '\0', 8);

  if (bind(g_cfg->dcc_listener, (struct sockaddr *)&dccaddr, sizeof(dccaddr)) == -1) 
  {
    troll_debug(LOG_ERROR,"Could not bind to DCC socket");
    return;
  }

  if (listen(g_cfg->dcc_listener, DCC_MAX) == -1) 
  {
    troll_debug(LOG_ERROR,"Could not listen on DCC socket");
    return;
  }

  return;
}

/* This creates a new connection from a listening socket */
void new_dcc_connection(int listensock)
{
  struct network *net;
  struct dcc_session *newdcc;
  struct dcc_session *tmpdcc;
  struct sockaddr_in client_addr;
  socklen_t sin_size;

  sin_size = sizeof(struct sockaddr_in);
 
  newdcc = new_dcc_session();

  if ((newdcc->sock = accept(listensock,(struct sockaddr *)&client_addr,&sin_size)) == -1)
  {
    troll_debug(LOG_WARN,"Could not accept DCC connection");
    return;
  }

  newdcc->status = DCC_GETNETWORK;
  
  if (g_cfg->dccs == NULL)
    g_cfg->dccs = newdcc;
  else
  {
    tmpdcc = g_cfg->dccs;

    while (tmpdcc->next != NULL)
      tmpdcc = tmpdcc->next;

    tmpdcc->next = newdcc;
    newdcc->prev = tmpdcc;
  }

  irc_printf(newdcc->sock,"Please type the name of the network you are connecting from");
  irc_printf(newdcc->sock,"Available choices are:");
  
  net = g_cfg->networks;
   
  while (net != NULL)
  {
    irc_printf(newdcc->sock,"  * %s",net->label);
    net = net->next;
  }
  
  return; 
}

struct dcc_session *new_dcc_session(void)
{
  struct dcc_session *ret;

  ret = tmalloc(sizeof(struct dcc_session));

  /* Needs proper ID */
  ret->id   = -1;
  ret->sock = -1;
  ret->user = NULL;
  ret->status = DCC_WAITING;

  ret->prev = NULL;
  ret->next = NULL;
  return ret;
}

void free_dcc_sessions(struct dcc_session *dccs)
{
  while (dccs != NULL)
  {
    free(dccs);
    dccs = dccs->next;
  }
}

void reverse_dcc_chat(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  /* This function should listen(), then pass the socket off to the irc_loop */
}

void initiate_dcc_chat(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  int dcc_sock = -1;
  unsigned long ip = 0;
  unsigned int port = 0;
  int argc;
  int highest_idx = 0;
  struct sockaddr_in req_addr;
  struct dcc_session *newdcc;
  struct dcc_session *tmp;

  /* Make sure we have at least the required args */
  if (data->rest == NULL)
    return;

  for(argc=0;argc<5;argc++)
  {
    if (data->rest[argc] == NULL)
      return;
  }
  
  ip = strtoul(data->rest[3],(char **)'\0',10);
  port = atoi(data->rest[4]);

  if ((dcc_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    troll_debug(LOG_WARN,"Could not create DCC socket");
    return;
  }

  req_addr.sin_family      = AF_INET;
  req_addr.sin_addr.s_addr = htonl(ip);
  req_addr.sin_port        = htons(port);

  memset(&(req_addr.sin_zero), '\0', 8);

  if (connect(dcc_sock,(struct sockaddr *)&req_addr,sizeof(struct sockaddr)) == -1)
  {
    troll_debug(LOG_WARN,"Could not connect to dcc user at %d",port);
    return;
  }
    
  /* We are connected at this point, allocate a dcc session */
  newdcc = new_dcc_session();

  newdcc->sock   = dcc_sock;
  newdcc->status = DCC_CONNECTED;
  
  /* So we know which userdb to read from */
  newdcc->net    = net; 

  /* Print the welcome message */
  irc_printf(newdcc->sock,"Welcome to Trollbot.");
  irc_printf(newdcc->sock,"Enter your username to continue.");
 
  /* Insert it into the global DCC list */
  if (g_cfg->dccs == NULL)
  {
    newdcc->id     = 1;
    g_cfg->dccs = newdcc;
    return;
  }

  tmp = g_cfg->dccs;

  while (tmp->prev != NULL)
    tmp = tmp->prev;
  
  highest_idx = tmp->id;

  while (tmp->next != NULL)
  {
    highest_idx = (tmp->id > highest_idx) ? tmp->id : highest_idx;
    tmp = tmp->next;
  }

  /* give the largest id + 1 to the new dcc session */
  newdcc->id = ++highest_idx;

  tmp->next = newdcc;
  newdcc->prev = tmp;

  return;
}

/* Weak */
void show_dcc_menu(struct dcc_session *dcc)
{
  irc_printf(dcc->sock,"TrollBot v1.0 DCC MENU\n"
                       "----------------------\n\n"
                       " +chan -chan +user -user\n"
                       " chattr rehash tcl\n");
                      
  return;
                                                          
}

int dcc_in(struct dcc_session *dcc)
{
  static char         *buffer  = NULL;
  static size_t       size     = BUFFER_SIZE;
  int                 recved   = 0;
  char                *line    = NULL;
  const char          *ptr     = NULL;
  char                *optr    = NULL;
  char                *bufcopy = NULL;

  if (buffer == NULL)
  {
    buffer = tmalloc0(BUFFER_SIZE + 1);
    recved = recv(dcc->sock,buffer,BUFFER_SIZE-1,0);
  } else {
    /* There was a fragment left over */
    buffer = tcrealloc0(buffer,
                        strlen(buffer) + BUFFER_SIZE + 1,
                        &size);

    recved = recv(dcc->sock,&buffer[strlen(buffer)],BUFFER_SIZE-1,0);

  }

  switch (recved)
  {
    case -1:
      free(buffer);
      buffer = NULL;
      return 1;
    case 0:
      dcc->sock = -1;
      return 0;
  }

  while (strchr(buffer,'\n') != NULL)
  { /* Complete IRC line */
    line = tmalloc0(strlen(buffer)+1);

    optr = line;

    for(ptr = buffer;*ptr != '\n' && *ptr != '\r';ptr++)
    {
      *optr = *ptr;
      optr++;
    }

    /* This should deal with ircds which output \r only, \r\n, or \n */
    while (*ptr == '\r' || *ptr == '\n')
      ptr++;

    parse_dcc_line(dcc,line);

    free(line);

    if (strlen(ptr) == 0)
    {
      free(buffer);
      buffer = NULL;
      break;
    }

    bufcopy = tstrdup(ptr);

    free(buffer);

    size   = strlen(bufcopy) + 1;

    buffer = bufcopy;
  }
  
  return 1;
}

void dcc_command_handler(struct dcc_session *dcc, const char *command)
{
  struct trigger *trig;

  trig = dcc->net->trigs->dcc_head;

  while (trig != NULL)
  {
    if (!strncmp(trig->mask,command,strlen(trig->mask)))
    {
      if (trig->handler != NULL)
      {
        trig->handler(dcc->net,trig,NULL,dcc,command);
        return;
      }
    }

    trig = trig->next;
  }

  dcc_partyline_handler(dcc,command);
  return;
}

void dcc_partyline_handler(struct dcc_session *dcc, const char *message)
{
  struct dcc_session *tmp;

  if ((tmp = g_cfg->dccs) == NULL)
    return;

  while (tmp != NULL)
  {
    irc_printf(tmp->sock,"<%s> %s",dcc->user->username,message);

    tmp = tmp->next;
  }
 
  return;
}


void parse_dcc_line(struct dcc_session *dcc, const char *buffer)
{
  struct user *user;
  struct network *net;

  switch (dcc->status)
  {
    case DCC_GETNETWORK:
      net = g_cfg->networks;

      while (net != NULL)
      {
        if (!strcmp(net->label,buffer))
        {
          irc_printf(dcc->sock,"Switching to network %s",net->label);
          dcc->net = net;
          dcc->status = DCC_CONNECTED;
          irc_printf(dcc->sock,"Enter your username to continue.");
          break;
        }
       
        net = net->next;
      }

      break;
    case DCC_CONNECTED:
      user = dcc->net->users;

      while (user != NULL)
      {
        if (!strcmp(user->username,buffer))
        {
          if (!egg_matchattr(dcc->net,user->username,"p",NULL))
          {
            irc_printf(dcc->sock,"You do not have the flags to access DCC.");
            shutdown(dcc->sock,SHUT_RDWR);
            dcc->sock = -1;
            return;
          }
          dcc->user = user;
          irc_printf(dcc->sock,"Please enter your password.");
          dcc->status = DCC_HAS_USERNAME;
          break;
        }
        
        user = user->next;
      }

      if (user == NULL)
      {
        irc_printf(dcc->sock,"Incorrect username.");
        shutdown(dcc->sock,SHUT_RDWR);
        dcc->sock = -1;
      }
 
      break;
    case DCC_HAS_USERNAME:
      /* No password */
      if (dcc->user->passhash == NULL)
      {
        irc_printf(dcc->sock,"Login failed");
        shutdown(dcc->sock,SHUT_RDWR);
        dcc->sock = -1;
      }

      if (egg_passwdok(dcc->net,dcc->user->username,buffer))
      {
        irc_printf(dcc->sock,"Type .help for help.");
        dcc->status = DCC_AUTHENTICATED;
      }
      else
      {
        irc_printf(dcc->sock,"Incorrect password");
        shutdown(dcc->sock,SHUT_RDWR);
        dcc->sock = -1;
      }
      
      break;
    case DCC_AUTHENTICATED:
      dcc_command_handler(dcc, buffer);
      break;
  }
  
  return;
}
