#ifndef __USER_H__
#define __USER_H__

struct tconfig_block;

struct channel_flags
{
  char *chan;
  char *flags;

  struct tconfig_block *tindex;

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

  /* Unhandled blocks go here */
  struct tconfig_block *tcfg;

  struct user *next;
  struct user *prev;
};


void free_users(struct user *users);
struct user *new_user(char *username, char *nick, char *passhash, char *ident, char *realname, char *host, char *flags);
struct channel_flags *new_channel_flags(char *chan, char *flags);
void user_init(void);


#endif /* __USER_H__ */

