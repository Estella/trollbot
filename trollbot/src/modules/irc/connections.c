#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif /* HAVE_STDARG_H */

/* According to earlier standards */
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif /* HAVE_SYS_TIME_H */

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "util.h"

#define BUFFER_SIZE 1024

void tmodule_read_cb(struct tsocket *sock)
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
    recved = recv(sock->sock,buffer,BUFFER_SIZE-1,0);
  } else {
    /* There was a fragment left over */
    buffer = tcrealloc0(buffer,
                        strlen(buffer) + BUFFER_SIZE + 1,
                        &size);

    recved = recv(sock->sock,&buffer[strlen(buffer)],BUFFER_SIZE-1,0);

  }


  switch (recved)
  {
    case -1:
      free(buffer);
      buffer = NULL;
      return;
    case 0:
      /* sock->sock     = -1;
      sock->status  |= STATUS_IGNORE;*/
      return;
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

    printf("%s\n",line);

    free(line);

    if (strlen(ptr) == 0)
    {
      free(buffer);
      buffer = NULL;
      break;
    }

    bufcopy = tstrdup(ptr);

    if (buffer != NULL)
      free(buffer);

    size   = strlen(bufcopy) + 1;

    buffer = bufcopy;
  }

  return;
}

void tmodule_write_cb(struct tsocket *sock)
{
  static has_wrote_headers = 0;

  if (has_wrote_headers == 0)
  {
    has_wrote_headers = 1;
 
    send(sock->sock, "GET C:/\n",9,0);
    printf("Wrote to get header\n");
  }
  /* sock is writeable */
}

void connections_init(struct tconfig_block *tcfg)
{
  int sock = -1;
  struct hostent *he;
  struct sockaddr_in serv_addr;
  struct sockaddr_in my_addr;
  struct tconfig_block *toplevel;
  struct tsocket *tsock = NULL;

  toplevel = tcfg;

  while (tcfg != NULL)
  {
    if (!strcmp(tcfg->name,"network"))
    {
      /* Found a network, initiate a tsocket, find a suitable server,
       * then try a non-blocking connection.
       */
      tcfg = tcfg->child;
      
      while (tcfg != NULL)
      {
        if (!strcmp(tcfg->key,"server"))
        {
          /* found a server */
          if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
          {
            printf("Error getting socket for network %s\n",tcfg->value);
            break;
          }

          if ((he = gethostbyname(tcfg->value)) == NULL)
          {
            printf("Could not resolve %s\n",tcfg->value);
            break;
          }
          
          serv_addr.sin_family = AF_INET;
          /* Fixme */
          serv_addr.sin_port   = htons(6667);  
          serv_addr.sin_addr   = *((struct in_addr *)he->h_addr);
          memset(&(serv_addr.sin_zero), '\0', 8);

          if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1)
          {
            printf("Could not connect to %s\n",tcfg->value);
            break;
          }

          tsock = tmalloc0(sizeof(struct tsocket));
 
          tsock->sock   = sock;
          tsock->status = 0;

          tsock->status |= STATUS_NOTINLOOP;

          if (writefds != NULL)
            slist_init(&writefds,NULL);

          slist_insert_next(writefds, NULL, tsock);

          if (readfds != NULL)
            slist_init(&readfds,NULL);

          slist_insert_next(readfds, NULL, tsock);

          tcfg = NULL; /* To bypass the nex */
        }

        tcfg = tcfg->next;
        
  

/* an exported function 
int tmodule_init(void *init_data) 
{
  int sock = -1;
  struct hostent *he;
  struct sockaddr_in serv_addr;

  printf("This is the test module \"foo\". If you are reading this, the test passed\n");

  printf("Connecting to www.google.com port 80\n");

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    printf("Error getting socket\n");
    return 0;
  }

  if ((he = gethostbyname("www.intpos.com")) == NULL)
  {
    printf("Could not resolve\n");
    return 0;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port   = htons(80);
  serv_addr.sin_addr   = *((struct in_addr *)he->h_addr);
  memset(&(serv_addr.sin_zero), '\0', 8);


  if (connect(sock,(struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1)
  {
    printf("Could not connect\n");
    return 0;
  }

  printf("Connected to google with socket# %d\n",sock);

  testsock = tmalloc0(sizeof(struct tsocket));

  testsock->sock   = sock;
  testsock->status = 0;

  testsock->status |= STATUS_NOTINLOOP;

  slist_init(&writefds,NULL);
  slist_insert_next(writefds, NULL, testsock);

  slist_init(&readfds,NULL);

  slist_insert_next(readfds, NULL, testsock);

  return 1;
}

*/
