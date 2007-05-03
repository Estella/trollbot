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

char *egg_makearg(char *rest, char *mask);
int egg_matchwilds(const char *haystack, const char *needle);

void egg_putserv(struct network *net, const char *text, int option_next);
void egg_puthelp(struct network *net, const char *text, int option_next);
void egg_putquick(struct network *net, const char *text, int option_next);

int egg_matchattr(struct network *net, const char *handle, const char *flags, const char *channel);

int egg_passwdok(struct network *net, const char *handle, const char *pass);
int egg_countusers(struct network *net);

struct user *egg_finduser(struct network *net, const char *mask);
int egg_chhandle(struct network *net, const char *old, const char *new);

time_t egg_unixtime(void);

void egg_putdcc(int idx, const char *text);
void egg_dccbroadcast(struct network *net, const char *message);

int egg_bind(struct network *net, char *type, char *flags, char *mask, char *cmd, void (*handler)(struct network *, struct trigger *, struct irc_data *, struct dcc_session *, const char *));

char *egg_botnick(struct network *net);

#endif /* __EGGLIB_H__ */
