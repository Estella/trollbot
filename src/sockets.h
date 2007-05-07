/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#ifndef __SOCKETS_H__
#define __SOCKETS_H__

void socket_set_blocking(int sock);
void socket_set_nonblocking(int sock);
void irc_loop(void);

#endif /* __SOCKETS_H__ */
