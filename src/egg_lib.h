#ifndef __EGGLIB_H__
#define __EGGLIB_H__

#include "config.h"

#ifdef HAVE_TIME_H
#include <time.h>
#endif /* HAVE_TIME_H */

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif /* HAVE_SYS_TIME_H */

struct network;
struct irc_data;
struct trigger;
struct dcc_session;

/* This is a band-aid function to make an arg to match against FIXME */
char *egg_makearg(char *rest, char *mask);

/* This is an eggdrop compliant pattern matching recursive function */
int egg_matchwilds(const char *haystack, const char *needle);

/* These put data to the server */
void egg_putserv(struct network *net, const char *text, int option_next);
void egg_puthelp(struct network *net, const char *text, int option_next);
void egg_putquick(struct network *net, const char *text, int option_next);

/* Matches a users flags, and optionally channel flags, uses glob|chan notation */
int egg_matchattr(struct network *net, const char *handle, const char *flags, const char *channel);

/* Checks SHA-1 hash, should be configurable */
int egg_passwdok(struct network *net, const char *handle, const char *pass);

/* Counts the users for a network */
int egg_countusers(struct network *net);

/* Finds a user based on their hostmask */
struct user *egg_finduser(struct network *net, const char *mask);

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

/* Binds an event handler to an event */
char **egg_bind(struct network *net, char *type, char *flags, char *mask, char *cmd, void (*handler)(struct network *, struct trigger *, struct irc_data *, struct dcc_session *, const char *));

/* Returns the bot's nickname */
char *egg_botnick(struct network *net);

char *egg_botname(struct network *net);

char *egg_version(void);


/**
 * Eggdrop Compatible onchan
 * @param net A network struct where this is to be checked.
 * @param nickname The nickname to try and find.
 * @param channel Optional channel name to check in
 * @return 1 if user found on optional channel, 1 if user found without optional channel, 0 if user not found on optional channel or 0 if not found without optional channel
 */
int egg_onchan(struct network *net, char *nickname, char *channel);

#endif /* __EGGLIB_H__ */
