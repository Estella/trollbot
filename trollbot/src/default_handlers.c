#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "servers.h"
#include "main.h"
#include "irc.h"
#include "handlers.h"
#include "channels.h"

static int ping_handler(struct irc_data *data, const char *line)
{
  /* Return PING :<servername> */
  irc_printf(glob_server_head->sock,
             "PING :%s",
             data->rest_str);

  printf("Returned ping\n");
  return 0;
}

static int ctcp_ping_handler(struct irc_data *data, const char *line)
{
  if (data->rest[1] != NULL)
  {
    irc_printf(glob_server_head->sock,"NOTICE %s :\001PING %s\001",data->prefix->nick,data->rest[1]);
  }

  return 0;
}

static int ctcp_version_handler(struct irc_data *data, const char *line)
{
  irc_printf(glob_server_head->sock,
             "NOTICE %s :\001VERSION Trollbot v0.0.1 (poutine)\001",
             data->prefix->nick);
  return 0;
}

static int die_handler(struct irc_data *data, const char *line)
{
  irc_printf(glob_server_head->sock,"QUIT :Trollbot v0.0.1");

  die_nicely();
  return 0;
}

static int join_channels_handler(struct irc_data *data, const char *line)
{
  printf("Joining channels\n");

  join_channels();

  return 0;
}

void add_default_handlers(void)
{
  /* For DCC initiation handling */
  /* Reference:
   * void add_handler(const char *name,
   *                  int (*startup_func)(void),
   *                  int (*exec_func)(struct irc_data *,const char *),
   *                  int (*shutdown_func)(void),
   *                  int persistant)
   *
   *
   * void add_trigger(int type,
   *                  const char *trigger,
   *                  const char *handler_name,
   *                  const char *flags,
   *                  const char *exec)
   */

  add_handler("dcc_connect",
              NULL,
              dcc_connect,
              NULL,
              1);

  add_trigger(CTCP,
              "\001DCC CHAT",
              "dcc_connect",
              NULL,
              "foo");
 
  add_handler("server_ping",
              NULL,
              ping_handler,
              NULL,
              1);

  add_trigger(RAW,
              "PING",
              "server_ping",
              NULL,
              "foo");

  add_handler("ctcp-ping",
              NULL,
              ctcp_ping_handler,
              NULL,
              1);

  add_trigger(CTCP,
              "\001PING",
              "ctcp-ping",
              NULL,
              "Foo");

  add_handler("ctcp-version",
              NULL,
              ctcp_version_handler,
              NULL,
              1);

  add_trigger(CTCP,
              "\001VERSION\001",
              "ctcp-version",
              NULL,
              "foo");

  add_handler("join_channels",
              NULL,
              join_channels_handler,
              NULL,
              1);

  add_trigger(RAW,
              "005",
              "join_channels",
              NULL,
              "foo");

#ifdef HAVE_TCL
  add_handler("tcl-handle",
              tcl_startup,
              tcl_handler,
              tcl_shutdown,
              1);
#endif /* HAVE_TCL */

#ifdef HAVE_PHP
  add_handler("php-handle",
              php_startup,
              php_handle,
              php_shutdown,
              1);
#endif /* HAVE_PHP */

  return;
}
