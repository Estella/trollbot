#ifndef __UTIL_H__
#define __UTIL_H__

/* Utility Macros */
#define LIST_ATTACH(x,y) if (x == NULL) \
                         {\
                           x = y; \
                           x->prev = NULL; \
                           x->next = NULL;  \
                         }\
                         else \
                           while (x->next != NULL) x = x->next; \
                           x->next = y; \
                           x->next->prev = x; \
                           x->next->next = NULL; \
                           while (x->prev != NULL) x = x->prev; \
                         }

int tstrncasecmp(const char *first, const char *second, int check_size);
int tstrcasecmp(const char *first, const char *second);
char *tstrdup(const char *ptr);
void tstrfreev(char *ptr[]);
char *tstrtrim(char *data);
void *tmalloc(size_t size);
void *tmalloc0(size_t size);
char *tstrarrayserialize(char **ptr);
char *tcrealloc0(char *ptr, size_t size, unsigned int *bufsize);
char **tsrealloc0(char **ptr, size_t size, unsigned int *bufsize);

#endif /* __UTIL_H__ */
