#ifndef __UTIL_H__
#define __UTIL_H__

char *tstrdup(const char *ptr);
void tstrfreev(char *ptr[]);
char *tstrtrim(char *data);
void *tmalloc(size_t size);
void *tmalloc0(size_t size);
char *tcrealloc0(char *ptr, size_t size, unsigned int *bufsize);
char **tsrealloc0(char **ptr, size_t size, unsigned int *bufsize);

#endif /* __UTIL_H__ */
