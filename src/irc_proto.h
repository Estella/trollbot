#ifndef __IRC_PROTO_H__
#define __IRC_PROTO_H__

struct network;
struct trigger;
struct irc_data;
struct dcc_session;

void add_default_triggers(void);

void new_join(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void new_part(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void new_quit(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void new_kick(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void new_user_pass(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void check_user_pass(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void introduce_user(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void return_ctcp_ping(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void return_ctcp_time(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void return_ctcp_version(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void disconnect_bot(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);

#endif /* __IRC_PROTO_H__ */
