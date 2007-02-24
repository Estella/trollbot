/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#ifndef __MAIN_H_

#include "channels.h"
#include "irc.h"
#include "dcc.h"

#include "../config.h"

#ifdef HAVE_TCL
#include <tcl.h>
#endif /* HAVE_TCL */

#ifdef HAVE_PHP
#include <TSRM.h>
#endif /* HAVE_PHP */


/* Defines */
enum {
  LOG_NONE  = 0,
  LOG_FATAL,
  LOG_WARN,
  LOG_ALL,
  LOG_DEBUG
};

#define BUFFER_SIZE 2048

/* Tranversing Macros */
#define REWIND_LIST(x) \
    while (x->prev != NULL) x = x->prev;

#define FASTFORWARD_LIST(x) \
    while (x->next != NULL) x = x->next;

/* Global Variables */
#ifdef HAVE_TCL
extern Tcl_Interp *tcl_interpreter;
#endif /* HAVE_TCL */

extern struct glob_config *config;

extern struct users    *glob_users,
                       *glob_users_head,
                       *glob_users_tail;

extern struct channels *glob_chan,
                       *glob_chan_head,
                       *glob_chan_tail;
 
extern struct servers  *glob_server,
                       *glob_server_head,
                       *glob_server_tail;

extern struct dcc_session *glob_dcc,
                          *glob_dcc_head,
                          *glob_dcc_tail;

extern struct triggers *trig_pub,
                       *trig_msg,
                       *trig_notc,
                       *trig_quit,
                       *trig_part,
                       *trig_ctcp,
                       *trig_join,
                       *trig_mode,
                       *trig_pubm,
                       *trig_msgm,
                       *trig_raw;

extern struct scripts  *tcl_scripts_head,
                       *tcl_scripts_tail,
                       *tcl_scripts,
                       *php_scripts_head,
                       *php_scripts_tail,
                       *php_scripts;

extern struct handlers *handlers,
                       *handlers_head,
                       *handlers_tail;

/* Function Prototypes */

/* config.c */
extern void parse_config_line(char *);
extern void set_sendq(const char *);
extern void set_nick(const char *);
extern void set_altnick(const char *);
extern void set_fork(int);
extern void set_debug(int);
extern void set_vhost(const char *);
extern void glob_config_new(void);
extern void glob_config_free(void);
extern int  glob_config_check(void);

/* mem.c */
extern void *tmalloc(size_t);
extern void *tmalloc0(size_t);
extern char *tcrealloc0(char *ptr, size_t size, unsigned int *bufsize);
extern char **tsrealloc0(char **ptr, size_t size, unsigned int *bufsize);

/* debug.c */
extern void troll_debug(int, const char *, ...);

/* util.c */
extern char *tstrdup(const char *ptr);
extern void  tstrfreev(char *ptr[]);
extern char *tstrtrim(char *ptr);


/* die.c */
extern void die_nicely(void);

/* channels.c */
extern void add_channel(const char *);
extern void join_channels(void);
extern void print_channels(void);
extern void free_channels(void);

/* dcc.c */
extern int dcc_in(struct dcc_session *);
extern int dcc_connect(struct irc_data *, const char *);

/* servers.c */
extern void add_server(const char *);
extern void print_servers(void);
extern void free_servers(void);

/* sockets.c */
extern void irc_loop(void);

/* irc.c */
void irc_printf(int sock, const char *fmt, ...);
struct irc_data *irc_data_new(void);
void irc_data_free(struct irc_data *);
void parse_irc_buffer(const char *);
int  irc_in(int);


/* triggers.c */
void add_trigger(int,
                 const char *,
                 const char *,
                 const char *,
                 const char *);

int match_triggers(struct irc_data *);

/* handlers.c */
int handler_call(struct irc_data *,struct triggers *);
struct handlers *handler_lookup(const char *);
void add_handler(const char *,
                 int (*startup_func)(void),
                 int (*exec_func)(struct irc_data *,const char *),
                 int (*shutdown_func)(void),
                 int);

/* default_handlers.c */
void add_default_handlers(void);

/* scripts.c */
void add_php_script(const char *);
void add_tcl_script(const char *);

/* users.c */
int load_userdb(void);

#ifdef HAVE_TCL
/* tcl.c */
int tcl_startup(void);
int tcl_handler(struct irc_data *data, const char *line);
int tcl_shutdown(void);
/* tcl_lib.c */
int tcl_putserv(ClientData clientData,
                Tcl_Interp *interp,
                int objc,
                Tcl_Obj *const objv[]);

int tcl_bind(ClientData clientData,
             Tcl_Interp *interp,
             int objc,
             Tcl_Obj *const objv[]);
#endif /* HAVE_TCL */

#ifdef HAVE_PHP
/* php_embed.c */
int php_embed_init(int, char **);
void php_embed_shutdown(TSRMLS_D);
int php_startup(void);
int php_handle(struct irc_data *, const char *);
int php_shutdown(void);
#endif /* HAVE_PHP */

#define __MAIN_H_

#endif /* __MAIN_H */
