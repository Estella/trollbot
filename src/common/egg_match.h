#ifndef __EGG_MATCH_H__
#define __EGG_MATCH_H__



#define QUOTE '\\' /* quoting character (overrides wildcards) */
#define WILDS '*'  /* matches 0 or more characters (including spaces) */
#define WILDP '%'  /* matches 0 or more non-space characters */
#define WILDQ '?'  /* matches ecactly one character */
#define WILDT '~'  /* matches 1 or more spaces */

#define NOMATCH 0
#define MATCH (match+sofar)
#define PERMATCH (match+saved+sofar)

int _wild_match_per(register unsigned char *m, register unsigned char *n);
int _wild_match(register unsigned char *m, register unsigned char *n);


#endif /* __EGG_MATCH_H__ */
