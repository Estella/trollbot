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
#ifndef __EGGLIB_H__
#define __EGGLIB_H__

#include "config.h"

#ifdef HAVE_TIME_H
#include <time.h>
#endif /* HAVE_TIME_H */

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif /* HAVE_SYS_TIME_H */

enum stripcodes_flags
{
	STRIPCODES_NONE      = 0,
	STRIPCODES_BOLD      = 1,
	STRIPCODES_COLOR     = 2,
	STRIPCODES_REVERSE   = 4,
	STRIPCODES_UNDERLINE = 8,
	STRIPCODES_ANSI      = 16,
	STRIPCODES_BELL      = 32
};

struct network;
struct irc_data;
struct trigger;
struct dcc_session;
struct t_timer;

/* Stirpcodes */
char *egg_stripcodes(const char *flags, const char *text);


/*** Output Commands ***/
void egg_putserv(struct network *net, const char *text, int option_next);
void egg_puthelp(struct network *net, const char *text, int option_next);
void egg_putquick(struct network *net, const char *text, int option_next);
int egg_passwdok(struct network *net, const char *handle, const char *pass);
void egg_putlog(struct network *net, const char *text);

/*** User Record Manipulation Commands ***/
int egg_countusers(struct network *net);
int egg_validuser(struct network *net, const char *handle);
struct user *egg_finduser(struct network *net, const char *mask);
int egg_killchanban(struct network *net, char *channel, char *ban);
void egg_newchanban(struct network *net, const char *channel, const char *ban, const char *who, const char *comment, int lifetime, char *options);
void egg_newban(struct network *net, char *mask, char *creator, char *comment, int lifetime, char *options);


/* Rename in case using c++ compiler */
int egg_chhandle(struct network *net, const char *old, const char *new);
char *egg_chattr(struct network *net, const char *handle, const char *changes, const char *channel);
int egg_matchattr(struct network *net, const char *handle, const char *flags, const char *channel);
int egg_adduser(struct network *net, char *username, char *hostmask);
int egg_deluser(struct network *net, char *username);

/* These are in the eggdrop documentation under this heading, but they seem to be channel commands */
int egg_unstick(struct network *net, char *ban, char *channel);
int egg_stick(struct network *net, char *ban, char *channel);
int egg_isban(struct network *net, char *ban, char *channel);
int egg_ispermban(struct network *net, char *ban, char *channel);
int egg_isbansticky(struct network *net,  char *ban, char *channel);
int egg_getting_users(struct network *net);

/*** Miscellaneous commands ***/
char *egg_md5(char *string);

/*** Channel Commands ***/
void egg_savechannels(struct network *net);
void egg_loadchannels(struct network *net);
char *egg_channels(struct network *net);
int egg_isbotnick(struct network *net, char *nick); /* Another miscategorization ? */
int egg_botisop(struct network *net, const char *channel);
int egg_isop(struct network *net, const char *nickname, const char *channel);
int egg_isvoice(struct network *net, const char *nickname, const char *channel);
int egg_botisvoice(struct network *net, const char *nickname, const char *channel);
int egg_botonchan(struct network *net, const char *nickname, const char *channel);
int egg_onchan(struct network *net, const char *nickname, const char *channel);
char *egg_topic(struct network *net, char *chan);
int egg_ischanban(struct network *net, const char *ban, const char *channel);
char *egg_getchanmode(struct network *net, const char *channel);

/*** DCC Commands ****/
void egg_putdcc(struct network *net, int idx, const char *text);
int egg_hand2idx(struct network *net, const char *handle);
void egg_dccbroadcast(struct network *net, const char *message);
struct user *egg_idx2hand(struct network *net, int idx);
int egg_valididx(struct network *net, int idx);

/*** Notes Module ***/
/*** Assoc Module ***/
/*** Compress Module ***/
/*** Filesys Module ***/
/*** Miscellaneous Commands ***/
void egg_rehash(void);
void egg_save(struct network *net);
char *egg_makepasswd(const char *pass, const char *hash_type);
char *egg_encpass(const char *pass);
char *egg_unbind(struct network *net, char *type, char *flags, char *mask, char *command);
void egg_die(struct network *net, const char *reason);
int egg_utimer(struct network *net, int seconds, char *command, void (*handler)(struct network *, struct t_timer *));
int egg_timer(struct network *net, int minutes, char *command, void (*handler)(struct network *, struct t_timer *));
char **egg_bind(struct network *net, char *type, char *flags, char *mask, char *cmd, void (*handler)(struct network *, struct trigger *, struct irc_data *, struct dcc_session *, const char *));
time_t egg_unixtime(void);
char **egg_binds(struct network *net, char *mask);


/* Unorganized */
char **egg_chanbans(struct network *net, const char *channel);
char **egg_banlist(struct network *net, const char *channel);




/*** THESE SHOULD BE GLOBAL VARIABLES, NOT QUITE SURE WTF I WAS THINKING */
char *egg_botnick(struct network *net);
char *egg_botname(struct network *net);
char *egg_version(void);

#endif /* __EGGLIB_H__ */
