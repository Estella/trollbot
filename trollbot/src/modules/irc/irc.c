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
#include "network.h"

#define BUFFER_SIZE 1024

/* aliases for the exported symbols */
#define tmodule_init	    irc_LTX_tmodule_init
#define tmodule_get_readfds irc_LTX_tmodule_get_readfds
#define testsock	    irc_LTX_testsock
#define readfds             irc_LTX_readfds
#define writefds            irc_LTX_writefds
#define tmodule_read_cb     irc_LTX_tmodule_read_cb
#define tmodule_write_cb    irc_LTX_tmodule_write_cb

struct slist   *readfds;
struct slist   *writefds;

void tmodule_read_cb(struct tsocket *tsock)
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

    irc_handle_line(tsock);

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
  return;
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
  struct tconfig_block *tcfg      = init_data;
  struct tconfig_block *ttmp      = NULL;
  struct network       *net       = NULL;
  struct hostent       *he        = NULL;
  int                   sock      = -1; 
  struct sockaddr_in    serv_addr;
  struct sockaddr_in    my_addr;

  if (tcfg == NULL)
  {
    printf("No configuration data for the IRC module found!\n");
    return 0;
  }

  while (tcfg != NULL)
  {
    if (!strcmp("network",tcfg->key))
    {
      /* New Network */
      net   = network_new(tcfg->value);

      ttmp  = tcfg->child;

      while (ttmp != NULL)
      {
        if (!strcmp("nick",tttmp->key))
        { 
          if (net->nick)
            free(net->nick);

          net->nick = tstrdup(ttmp->value);
        } 
        else if (!strcmp("altnick",ttmp->key))
        {
          if (net->altnick)
            free(net->altnick);
         
          net->altnick = tstrdup(ttmp->value);
        }
        else if (!strcmp("ident",ttmp->key))
        {
          if (net->ident)
            free(net->ident);

          net->ident = tstrdup(ttmp->value);
        }
        else if (!strcmp("realname",ttmp->key))
        {
          if (net->realname)
            free(net->realname);
      
          net->realname = tstrdup(ttmp->value);
        }
        else if (!strcmp("vhost",ttmp->key))
        {
          if (net->vhost)
            free(net->vhost);
      
          net->vhost = tstrdup(ttmp->value);
        }
    }
  }
      
  return 1;
}


