#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <time.h>

struct user;
struct channel;
struct tconfig_block;
struct network;
struct trigger;
struct dcc_session;
struct irc_data;

/* Used with struct slist, no prev, next */
struct chan_egg_var
{
	char *key;
	char *value;
};

struct channel_ban
{
  /*
   * >> :swiftco.wa.us.dal.net 367 poutine #php *!*@152.92.113.* global-r.hub.dal.net 1190168062
   * >> :swiftco.wa.us.dal.net 368 poutine #php :End of Channel Ban List
   */
	char *chan;

	char *mask;

	char *who;
	
	time_t time;

	struct channel_ban *prev;
	struct channel_ban *next;
};


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

	struct slist *egg_vars;

  struct channel_user *user_list;

	char *chanmode;
	char *topic;

	struct channel_ban  *banlist;

  /* Unhandled blocks get stuck here */
  struct tconfig_block *tcfg;

  struct channel *prev;
  struct channel *next;
};

struct chan_egg_var *new_chan_egg_var(void);
void free_chan_egg_var(void *chan_egg_var_ptr);

struct channel_ban *channel_ban_del(struct channel_ban *bans, struct channel_ban *del);
struct channel_ban *channel_ban_add(struct channel_ban *bans, struct channel_ban *add);
void channel_ban_free(struct channel_ban *ban);
struct channel_ban *channel_ban_new(void);


struct tconfig_block *chans_to_tconfig(struct channel *chans);
/* WTF is this */
void channel_list_add(struct channel **orig, struct channel *new);
void channel_user_add(struct channel_user **orig, struct channel_user *new);
void channel_user_del(struct channel_user **orig, const char *nick);
struct channel_user *new_channel_user(const char *nick, int jointime, struct user *urec);
void free_channels(struct channel *chans);
void free_channel(void *chanptr);
struct channel *new_channel(const char *chan);
void join_channels(struct network *net);
void channel_list_populate(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void chans_save(struct network *net);
struct channel *new_chan_from_tconfig_block(struct tconfig_block *tcfg);
void chan_init(void);


#endif /* __CHANNEL_H__ */
