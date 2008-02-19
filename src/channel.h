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
	char *topic;
  int status;

	/* Eggdrop Vars */
  int flood_chan_count;
  int flood_chan_sec;
  int flood_deop_count;
  int flood_deop_sec;
  int flood_kick_count;
  int flood_kick_sec;
  int flood_join_count;
  int flood_join_sec;
  int flood_ctcp_count;
  int flood_ctcp_sec;
  int flood_nick_count;
  int flood_nick_sec;
  int aop_delay_count;
  int aop_delay_sec;
  int idle_kick;
  char *chanmode;
  int stopnethack_mode;
  int revenge_mode;
  int ban_time;
  int exempt_time;
  int invite_time;
  int autoop;
  int bitch;
  int autovoice;
  int cycle;
  int dontkickops;
  int dynamicexempts;
  int greet;
  int dynamicinvites;
  int dynamicbans;
  int enforcebans;
  int revenge;
  int userbans;
  int userinvites;
  int autohalfop;
  int nodesynch;
  int protectops;
  int revengebot;
  int seen;
  int userexempts;
  int protecthalfops;
  int statuslog;
  int secret;
  int shared;

  struct channel_user *user_list;

  /* Unhandled blocks get stuck here */
  struct tconfig_block *tcfg;

  struct channel *prev;
  struct channel *next;
};

struct tconfig_block *chans_to_tconfig(struct channel *chans);
/* WTF is this */
void channel_list_add(struct channel **orig, struct channel *new);
void channel_user_add(struct channel_user **orig, struct channel_user *new);
void channel_user_del(struct channel_user **orig, const char *nick);
struct channel_user *new_channel_user(const char *nick, int jointime, struct user *urec);
void free_channels(struct channel *chans);
struct channel *new_channel(const char *chan);
void join_channels(struct network *net);
void channel_list_populate(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void chans_save(struct network *net);
struct channel *new_chan_from_tconfig_block(struct tconfig_block *tcfg);
void chan_init(void);


#endif /* __CHANNEL_H__ */
