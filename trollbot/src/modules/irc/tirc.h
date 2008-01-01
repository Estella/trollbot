#ifndef __TIRC_H__
#define __TIRC_H__

/* Defines for globally available functions */
#define tmodule_get_read_fds  irc_LTX_tmodule_get_read_fds
#define tmodule_get_write_fds irc_LTX_tmodule_get_write_fds
#define tmodule_init          irc_LTX_tmodule_init
#define connections_init      irc_LTX_connections_init

// connections.c
#define tmodule_read_cb       irc_LTX_tmodule_read_cb
#define tmodule_write_cb      irc_LTX_tmodule_write_cb

/* Defines for globally available vars */
#define readfds               irc_LTX_readfds
#define writefds              irc_LTX_writefds
#define num_read_fds          irc_LTX_num_read_fds
#define num_write_fds         irc_LTX_num_write_fds


struct slist   *readfds;
struct slist   *writefds;

int num_read_fds;
int num_write_fds;


#endif /* __TIRC_H__ */
