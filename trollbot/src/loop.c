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

#include <ltdl.h>

/* According to POSIX.1-2001 */
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif /* HAVE_SYS_SELECT_H */

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

#include "tconfig.h"
#include "tmodule.h"

#include "loop.h"


/* This is the main program loop
 * It may seem a bit ugly, but it has to be in order to be portable.
 */
void trollbot_loop(struct tconfig_block *tcfg, struct slist *modules)
{
  fd_set read_fds;
  fd_set write_fds;
  struct timeval timeout;
  struct slist *messages;
  struct slist *sockets;
  struct slist_node *node;
  struct slist_node *s_node; /* Sockets node */
  struct tmodule *module;
  struct tsocket *sock;

  int highest_fd = 0;
  int running    = 1;

  while (running)
  {
/*
    messages = tmodule_get_messages();
    tmodule_send_messages(messages);
*/

    /* Deal with sockets */
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

    highest_fd = 0;

    /* Set a timeout of 1 second */
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    node = modules->head;

    while (node != NULL)
    {
      module = node->data;

      if (module != NULL)
      {
        if (module->tmodule_get_writefds != NULL)
        {
          sockets = (*module->tmodule_get_writefds)();

          if (sockets != NULL)
          {
            s_node = sockets->head;

            while (s_node != NULL)
            {
              sock = s_node->data;

              if ((sock->status & STATUS_IGNORE) != STATUS_IGNORE)
              {
                if (highest_fd < sock->sock)
                  highest_fd = sock->sock;

                FD_SET(sock->sock,&write_fds);

		/* This is in case a descriptor is added between the FD_SET and FD_ISSET */
                sock->status &= ~STATUS_NOTINLOOP;
              }

              s_node = s_node->next;
            }
          }
        }

        if (module->tmodule_get_readfds != NULL)
        {
          sockets = (*module->tmodule_get_readfds)();

          if (sockets != NULL)
          {
            s_node = sockets->head;

            while (s_node != NULL)
            {
              sock = s_node->data;

              if ((sock->status & STATUS_IGNORE) != STATUS_IGNORE)
              {
                if (highest_fd < sock->sock)
                  highest_fd = sock->sock;

                FD_SET(sock->sock,&read_fds);

		/* This is in case a descriptor is added between the FD_SET and FD_ISSET */
                sock->status &= ~STATUS_NOTINLOOP;
              }

              s_node = s_node->next;
            }
          }
        }
      }

      node = node->next;
    }

    /* select() */
    select(highest_fd+1, &read_fds, &write_fds, NULL, &timeout);

    node = modules->head;

    while (node != NULL)
    {
      module = node->data;

      if (module != NULL)
      {
        if (module->tmodule_get_readfds != NULL)
        {
          sockets = (*module->tmodule_get_readfds)();

          if (sockets != NULL)
          {
            s_node = sockets->head;

            while (s_node != NULL)
            {
              sock = s_node->data;

              /* If the sockets aren't marked ignore, call their 
               * associated callback functions, also ignore socks
               * added to list, but not in fd set
               */
              if ((sock->status & STATUS_IGNORE) != STATUS_IGNORE &&
                  (sock->status & STATUS_NOTINLOOP) != STATUS_NOTINLOOP)
              {
                /* If we have read data */
                if (FD_ISSET(sock->sock,&read_fds))
                {
                  if (module->tmodule_read_cb != NULL)
                  {
                    (*module->tmodule_read_cb)(sock);
                  }
                }

                if (FD_ISSET(sock->sock,&write_fds))
                {
                  if (module->tmodule_write_cb != NULL)
                  {
                    (*module->tmodule_write_cb)(sock);
                  }
                }
              }

              s_node = s_node->next;
            }
          }
        }
      }

      node = node->next;
    }

    /* Check if exit signalled */
  }

}

