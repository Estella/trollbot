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
  DCC_NONBLOCKCONNECT, /* A connect() call has been made, it is not in any fd set */
  DCC_WAITINGCONNECT,  /* A connect() call has been made, it's now in a fd set    */
  DCC_NOTREADY,        /* Socket has been accept()ed but not added to FD set      */
  DCC_CONNECTED,
  DCC_HAS_USERNAME,
  DCC_AUTHENTICATED
};

struct dcc_session
{
  int id; /* IDX is per network */

  int sock;

  struct user *user;

  int status;

  struct network *net;

  struct dcc_session *prev;
  struct dcc_session *next;
};

void dcc_print_motd(struct dcc_session *dcc);
struct dcc_session *dcc_list_del(struct dcc_session *dccs, struct dcc_session *del);
struct dcc_session *dcc_list_add(struct dcc_session *dccs, struct dcc_session *add) ;
void dcc_init_listener(struct network *net);
void new_dcc_connection(struct network *net);
struct dcc_session *new_dcc_session(void);
void free_dcc_sessions(struct dcc_session *dccs);
void free_dcc_session(struct dcc_session *dcc);
void reverse_dcc_chat(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void initiate_dcc_chat(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void dcc_help_menu(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void dcc_add_chan(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void dcc_del_chan(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void dcc_command_handler(struct dcc_session *dcc, const char *command);
void dcc_partyline_handler(struct dcc_session *dcc, const char *message);
int dcc_in(struct dcc_session *dcc);
void parse_dcc_line(struct dcc_session *dcc, const char *buffer);
void dcc_who(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void dcc_tbinds(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void dcc_chattr(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);

#define __DCC_H__
#endif /* __DCC_H__ */
