/* This file defines an interface for trollbot to use
 * to deal with sockets and other messaging.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* for symbol resolution */
#define tmodule_get_readfds  irc_LTX_tmodule_get_readfds
#define tmodule_get_writefds irc_LTX_tmodule_get_writefds

