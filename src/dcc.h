/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#ifndef __DCC_H__

/* Forward declaration */
struct user;
struct irc_data;
struct trigger;
struct network;

#define DCC_MAX 512 /* DCC max connections */

enum dcc_status {
  DCC_WAITING = 0,
  DCC_GETNETWORK, /* For anonymous dcc/telnet */
  DCC_CONNECTED,
  DCC_HAS_USERNAME,
  DCC_AUTHENTICATED
};

struct dcc_session
{
  int id;

  int sock;

  struct user *user;

  int status;

  struct network *net;

  struct dcc_session *prev;
  struct dcc_session *next;
};

void dcc_init_listener(struct network *net);
void new_dcc_connection(struct network *net);
struct dcc_session *new_dcc_session(void);
void free_dcc_sessions(struct dcc_session *dccs);
void reverse_dcc_chat(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void initiate_dcc_chat(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void show_dcc_menu(struct dcc_session *dcc);
void dcc_command_handler(struct dcc_session *dcc, const char *command);
void dcc_partyline_handler(struct dcc_session *dcc, const char *message);
int dcc_in(struct dcc_session *dcc);
void parse_dcc_line(struct dcc_session *dcc, const char *buffer);

#define __DCC_H__
#endif /* __DCC_H__ */
