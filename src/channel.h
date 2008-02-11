#ifndef __CHANNEL_H__
#define __CHANNEL_H__

struct user;
struct channel;
struct tconfig_block;
struct network;
struct trigger;
struct dcc_session;
struct irc_data;


struct channel_user
{
  char *nick;
	char *ident;

	char *modes;

  char *uhost;
  
  int jointime;

  /* Actual user record */
  struct user *urec;

  struct channel_user *prev;
  struct channel_user *next;
};

struct channel
{
  char *name;
  int status;

  struct channel_user *user_list;

  /* Unhandled blocks get stuck here */
  struct tconfig_block *tcfg;

  struct channel *prev;
  struct channel *next;
};

/* WTF is this */
void channel_list_add(struct channel **orig, struct channel *new);
void channel_user_add(struct channel_user **orig, struct channel_user *new);
void channel_user_del(struct channel_user **orig, const char *nick);
struct channel_user *new_channel_user(const char *nick, int jointime, struct user *urec);
void free_channels(struct channel *chans);
struct channel *new_channel(const char *chan);
void join_channels(struct network *net);
void chan_init(void);
void channel_list_populate(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);


#endif /* __CHANNEL_H__ */
