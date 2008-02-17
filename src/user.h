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
	char *hash_type;

  char *ident;
  char *realname;
  char *host;  /* regular hostname/ip */
  char *uhost; /* nick!ident@hostname/ip */

  char *flags;

  struct channel_flags *chan_flags;

  /* Unhandled blocks go here */
  struct tconfig_block *tcfg;

  struct user *next;
  struct user *prev;
};


struct tconfig_block *users_to_tconfig(struct user *users);
void user_list_add(struct user **orig, struct user *new);
void free_users(struct user *users);
struct user *new_user(char *username, char *nick, char *passhash, char *ident, char *realname, char *host, char *flags);
struct channel_flags *new_channel_flags(char *chan, char *flags);
struct user *new_user_from_tconfig_block(struct tconfig_block *tcfg);
void user_init(void);


#endif /* __USER_H__ */

