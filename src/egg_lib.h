#ifndef __EGGLIB_H__
#define __EGGLIB_H__

char *egg_makearg(char *rest, char *mask);
int egg_matchwilds(const char *haystack, const char *needle);

void egg_putserv(struct network *net, const char *text, int option_next);
void egg_puthelp(struct network *net, const char *text, int option_next);
void egg_putquick(struct network *net, const char *text, int option_next);


#endif /* __EGGLIB_H__ */
