#ifndef __UTIL_H__
#define __UTIL_H__

enum tsocket_status
{
  STATUS_NOTINLOOP = 1,
  STATUS_RSOCK     = 2,
  STATUS_WSOCK     = 4,
  STATUS_WCONNECT  = 8,
  STATUS_WACCEPT   = 16,
  STATUS_IGNORE    = 32
};

struct tsocket
{
  int sock;

  int status;
};

struct slist_node
{
  void *data;

  struct slist_node *next;
};

struct slist
{
  int size;

  int (*match)(const void *key1, const void *key2);
  void (*destroy)(void *data);

  struct slist_node *head;
  struct slist_node *tail;
};

struct dlist_node
{
  void *data;

  struct dlist_node *prev;
  struct dlist_node *next;
};

struct dlist
{
  int size;

  int (*match)(const void *key1, const void *key2);
  void (*destroy)(void *data);

  struct dlist_node *head;
  struct dlist_node *tail;
};

void slist_init(struct slist **list, void (*destroy)(void *));
void slist_destroy(struct slist *list);
int slist_insert_next(struct slist *list, struct slist_node *node, void *data);
int slist_remove_next(struct slist *list, struct slist_node *node, void **data);


char *tstr_replace(char *subject, char *needle, char *replace);
int tstrncasecmp(const char *first, const char *second, int check_size);
int tstrcasecmp(const char *first, const char *second);
char *tstrdup(const char *ptr);
void tstrfreev(char *ptr[]);
char *tstrtrim(char *data);
void *tmalloc(size_t size);
void *tmalloc0(size_t size);
char *tcrealloc0(char *ptr, size_t size, unsigned int *bufsize);
char **tsrealloc0(char **ptr, size_t size, unsigned int *bufsize);

#endif /* __UTIL_H__ */
