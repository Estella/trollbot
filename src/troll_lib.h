#ifndef __TROLL_LIB_H__
#define __TROLL_LIB_H__


struct network;
struct trigger;
struct irc_data;
struct dcc_session;

int troll_matchwilds(const char *haystack, const char *needle);

char *troll_makearg(const char *rest, const char *mask);
void troll_user_host_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void troll_parse_who(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void troll_nick_in_use_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void troll_trig_channel_redirect(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void troll_undernet_hack(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void troll_trig_update_mode(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void troll_trig_update_nick(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void troll_trig_update_topic(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);

#endif /* __TROLL_LIB_H__ */
