#ifndef __EGGLIB_H__
#define __EGGLIB_H__

char *egg_makearg(char *rest, char *mask);
int egg_matchwilds(const char *haystack, const char *needle);

void egg_putserv(struct network *net, const char *text, int option_next);
void egg_puthelp(struct network *net, const char *text, int option_next);
void egg_putquick(struct network *net, const char *text, int option_next);

int egg_matchattr(struct network *net, const char *handle, const char *flags, const char *channel);

int egg_passwdok(struct network *net, const char *handle, const char *pass);
int egg_countusers(struct network *net);

void egg_putdcc(int idx, const char *text);

#endif /* __EGGLIB_H__ */
