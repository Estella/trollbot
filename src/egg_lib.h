#ifndef __EGGLIB_H__
#define __EGGLIB_H__

#include <stdio.h>
#include <string.h>


char *egg_makearg(char *rest, char *mask);
int egg_matchwilds(const char *haystack, const char *needle);

#endif /* __EGGLIB_H__ */
