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

enum
{
  PUB = 0x0,
  PUBM,
  MSG,
  MSGM,
  KICK,
  MODE,
  NICK,
  QUIT,
  JOIN,
  PART,
  RAW,
  CTCP,
  CTCR,
  NOTC
};

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

  int bind_hint;
};


#define __IRC_H__
#endif /* __IRC_H__ */
