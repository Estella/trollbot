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

int egg_deluser(struct network *net, char *username);
void egg_putlog(struct network *net, const char *text);
int egg_unstick(struct network *net, char *ban, char *channel);
int egg_stick(struct network *net, char *ban, char *channel);
int egg_adduser(struct network *net, char *username, char *hostmask);
void egg_die(struct network *net, const char *reason);

char *egg_unbind(struct network *net, char *type, char *flags, char *mask, char *command);

int egg_botisvoice(struct network *net, const char *nickname, const char *channel);
int egg_botisop(struct network *net, const char *channel);
int egg_botonchan(struct network *net, const char *nickname, const char *channel);

int egg_isban(struct network *net, char *ban, char *channel);
int egg_ispermban(struct network *net, char *ban, char *channel);
int egg_isbansticky(struct network *net,  char *ban, char *channel);
int egg_isbansticky(struct network *net,  char *ban, char *channel);

int egg_utimer(struct network *net, int seconds, char *command, void (*handler)(struct network *, struct t_timer *));
int egg_timer(struct network *net, int minutes, char *command, void (*handler)(struct network *, struct t_timer *));


int egg_ischanban(struct network *net, const char *ban, const char *channel);
int egg_isop(struct network *net, const char *nickname, const char *channel);
int egg_isvoice(struct network *net, const char *nickname, const char *channel);

char *egg_getchanmode(struct network *net, const char *channel);


/* These put data to the server */
void egg_putserv(struct network *net, const char *text, int option_next);
void egg_puthelp(struct network *net, const char *text, int option_next);
void egg_putquick(struct network *net, const char *text, int option_next);

/* Helluva function */
char *egg_chattr(struct network *net, const char *handle, const char *changes, const char *channel);

/* Matches a users flags, and optionally channel flags, uses glob|chan notation */
int egg_matchattr(struct network *net, const char *handle, const char *flags, const char *channel);

/* Checks SHA-1 hash, should be configurable */
int egg_passwdok(struct network *net, const char *handle, const char *pass);

/* Counts the users for a network */
int egg_countusers(struct network *net);

/* Finds a user based on their hostmask */
struct user *egg_finduser(struct network *net, const char *mask);

/* Finds if the user is valid */
int egg_validuser(struct network *net, const char *handle);

/* Changes a users handle (username) */
int egg_chhandle(struct network *net, const char *old, const char *new);

/* Returns time as a UNIX timestamp */
time_t egg_unixtime(void);

/* puts a message to dcc idx based on network */
void egg_putdcc(struct network *net, int idx, const char *text);

/* Sends a message to all DCC users on a specific net */
void egg_dccbroadcast(struct network *net, const char *message);

/* Returns a user for a dcc ID */
struct user *egg_idx2hand(struct network *net, int idx);

/* Returns 0 on error, an idx if successful */
int egg_hand2idx(struct network *net, const char *handle);

int egg_valididx(struct network *net, int idx);

/* Binds an event handler to an event */
char **egg_bind(struct network *net, char *type, char *flags, char *mask, char *cmd, void (*handler)(struct network *, struct trigger *, struct irc_data *, struct dcc_session *, const char *));

/* Returns the bot's nickname */
char *egg_botnick(struct network *net);

char *egg_botname(struct network *net);

char *egg_version(void);

int egg_getting_users(struct network *net);
int egg_isbotnick(struct network *net, char *nick);

/**
 * Eggdrop Compatible onchan
 * @param net A network struct where this is to be checked.
 * @param nickname The nickname to try and find.
 * @param channel Optional channel name to check in
 * @return 1 if user found on optional channel, 1 if user found without optional channel, 0 if user not found on optional channel or 0 if not found without optional channel
 */
int egg_onchan(struct network *net, char *nickname, char *channel);

void egg_rehash(void);
void egg_save(struct network *net);
char *egg_makepasswd(const char *pass, const char *hash_type);
void egg_savechannels(struct network *net);
char *egg_channels(struct network *net);
char *egg_encpass(const char *pass);

char *egg_topic(struct network *net, char *chan);

#endif /* __EGGLIB_H__ */
