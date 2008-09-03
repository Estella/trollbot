/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#ifndef __IRC_H__
#define __IRC_H__

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

void irc_printf(int sock, const char *fmt, ...);
void irc_data_free(struct irc_data *data);
void parse_irc_line(struct network *net, const char *buffer);
struct irc_data *irc_data_new(void);
int irc_in(struct network *net);


#endif /* __IRC_H__ */
