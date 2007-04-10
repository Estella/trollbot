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

void initiate_dcc_chat(struct network *net, struct trigger *trig, struct irc_data *data)
{
  int dcc_sock = -1;
  unsigned long ip = 0;
  unsigned int port = 0;
  int argc;
  struct sockaddr_in req_addr;
  struct dcc_session *dcc;
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

  if (connect(dcc_sock,
              (struct sockaddr *)&req_addr,
              sizeof(struct sockaddr))      == -1)
  {
    troll_debug(LOG_WARN,"Could not connect to dcc user at %d",port);
    return;
  }
    
  /* We are connected at this point, allocate a dcc session */
  dcc = new_dcc_session();

  dcc->sock   = dcc_sock;
  dcc->status = DCC_CONNECTED;
  
  /* So we know which userdb to read from */
  dcc->net    = net; 

  /* Print the welcome message */
  irc_printf(dcc->sock,"Welcome to Trollbot.");
  irc_printf(dcc->sock,"Enter your username to continue.");
 
  /* Insert it into the global DCC list */
  if (g_cfg->dccs == NULL)
  {
    g_cfg->dccs = dcc;
    return;
  }

  tmp = g_cfg->dccs;
  
  while (tmp->next != NULL)
    tmp = tmp->next;

  tmp->next = dcc;
  dcc->prev = tmp;

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

void parse_dcc_line(struct dcc_session *dcc, const char *buffer)
{
  SHA1_CTX context;
  unsigned char digest[20];
  struct user *user;

  switch (dcc->status)
  {
    case DCC_CONNECTED:
      user = dcc->net->users;

      while (user != NULL)
      {
        if (!strcmp(user->username,buffer))
        {
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
      if (dcc->user->passhash == NULL)
      {
        irc_printf(dcc->sock,"Login failed");
        shutdown(dcc->sock,SHUT_RDWR);
        dcc->sock = -1;
      }

      /* User entered password */
      SHA1Init(&context);
      SHA1Update(&context, (unsigned char *)buffer, strlen(buffer));
      SHA1Final(digest, &context);

      /* Pass is all good baby */
      if (!strcmp(dcc->user->passhash,digest))
        irc_printf(dcc->sock,"Authenticated!");
      else
        irc_printf(dcc->sock,"Incorrect password");

      break;
  }
  
  return;
}
