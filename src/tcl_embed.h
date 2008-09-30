#ifndef __TCL_EMBED_H__
#define __TCL_EMBED_H__

struct network;
struct trigger;
struct irc_data;
struct config;
struct t_timer;

#ifdef HAVE_ICS
struct ics_server;
struct ics_trigger;
struct ics_data;
#endif /* HAVE_ICS */

void dcc_tcl(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void tcl_load_scripts_from_config(struct config *cfg);
void net_init_tcl(struct network *net);
void net_tcl_init_commands(struct network *net);
void tcl_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void t_timer_tcl_handler(struct network *net, struct t_timer *timer);

#ifdef HAVE_ICS
void ics_tcl_load_scripts_from_config(struct config *cfg);
void ics_init_tcl(struct ics_server *ics);
void ics_tcl_init_commands(struct ics_server *ics);
void ics_tcl_handler(struct ics_server *ics, struct ics_trigger *trig, struct ics_data *data);
#endif /* HAVE_ICS */

#endif /* __TCL_EMBED_H__ */
