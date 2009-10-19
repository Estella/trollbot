/*********************************************
 * TrollBot v1.0                             *
 *********************************************
 * TrollBot is an eggdrop-clone designed to  *
 * work with multiple networks and protocols *
 * in order to present a unified scriptable  *
 * event-based platform,                     *
 *********************************************
 * This software is PUBLIC DOMAIN. Feel free *
 * to use it for whatever use whatsoever.    *
 *********************************************
 * Originally written by poutine/DALnet      *
 *                       kicken/DALnet       *
 *                       comcor/DALnet       *
 *********************************************/
#ifndef __IRC_H__
#define __IRC_H__

#include "config.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

struct network;

/* <servername> | <nick> [ '!' <user> ] [ '@' <host> ] */
struct irc_prefix {
  char *servername;

  char *nick;
  char *user;
  char *host;
};

struct irc_data {
  char *line;

  struct irc_prefix *prefix;

  char *command;

  char **c_params;
  char *c_params_str;

  char **rest;
  char *rest_str;
};

ssize_t irc_printf(int sock, const char *fmt, ...);
void irc_data_free(struct irc_data *data);
void parse_irc_line(struct network *net, const char *buffer);
struct irc_data *irc_data_new(void);
int irc_in(struct network *net);


#endif /* __IRC_H__ */
