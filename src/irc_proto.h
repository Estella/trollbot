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
#ifndef __IRC_PROTO_H__
#define __IRC_PROTO_H__

struct irc_hostmask
{
	char *nick;
	char *ident;
	char *host;
	char *s_ip;
	
	long long_ip;
};

struct network;
struct channel;
struct channel_user;
struct irc_ban;
struct trigger;
struct irc_data;
struct dcc_session;

void add_default_triggers(void);

void irc_ban(struct network *net, struct channel *channel, struct irc_ban *ban);
void irc_kick_ban(struct network *net, struct channel *channel, struct channel_user *cuser, struct irc_ban *ban);
struct irc_hostmask *irc_hostmask_parse(char *mask);
void irc_hostmask_free(struct irc_hostmask *hm);
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
