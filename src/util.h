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

/* single linked list interface */
int matchwilds(const char *haystack, const char *needle);
void slist_init(struct slist **list, void (*destroy)(void *));
void slist_destroy(struct slist *list);
int slist_insert_next(struct slist *list, struct slist_node *node, void *data);
int slist_remove_next(struct slist *list, struct slist_node *node, void **data);
void *slist_remove(struct slist *list, struct slist_node *node, void *data);

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
int tstrcount(char **ptr);
char **tssv_split(char *ptr);

#endif /* __UTIL_H__ */
