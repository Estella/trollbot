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

	char *console; /* console flags */

  char *flags;

  struct channel_flags *chan_flags;

  /* Unhandled blocks go here */
  struct tconfig_block *tcfg;

  struct user *next;
  struct user *prev;
};


struct tconfig_block *users_to_tconfig(struct user *users);
struct user *user_list_add(struct user *users, struct user *add);
struct user *user_list_del(struct user *users, struct user *del);
void free_users(struct user *users);
struct user *new_user(char *username, char *nick, char *passhash, char *ident, char *realname, char *host, char *flags);
struct channel_flags *new_channel_flags(char *chan, char *flags);
struct user *new_user_from_tconfig_block(struct tconfig_block *tcfg);
void user_init(void);
void users_save(struct network *net);


#endif /* __USER_H__ */

