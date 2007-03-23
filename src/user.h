#ifndef __USER_H__
#define __USER_H__

struct channel_flags
{
  char *chan;
  char *flags;

  struct channel_flags *next;
  struct channel_flags *prev;
};
  
struct user
{
  char *username;
  char *nick;
  char *passhash;

  char *ident;
  char *realname;
  char *host;

  char *flags;
  struct channel_flags *chan_flags;

  struct user *next;
  struct user *prev;
};


struct user *new_user(char *username, char *nick, char *passhash, char *ident, char *realname, char *host, char *flags);
struct channel_flags *new_channel_flags(char *chan, char *flags);
void user_init(void);


#endif /* __USER_H__ */

