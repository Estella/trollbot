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

/* aliases for the exported symbols */
#define tmodule_init	    foo_LTX_tmodule_init
#define tmodule_get_readfds foo_LTX_tmodule_get_readfds
#define testsock	    foo_LTX_testsock
#define readfds             foo_LTX_readfds
#define writefds            foo_LTX_writefds
#define tmodule_read_cb     foo_LTX_tmodule_read_cb
#define tmodule_write_cb    foo_LTX_tmodule_write_cb


struct tsocket *testsock;
struct slist   *readfds;
struct slist   *writefds;


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

struct slist *tmodule_get_writefds(void)
{
  return writefds;
}

struct slist *tmodule_get_readfds(void)
{
  return readfds;
}


/* an exported function */
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


