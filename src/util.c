/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "main.h"

/* case insensitive string compare */
int tstrncasecmp(const char *first, const char *second, int check_size)
{
  while (*first != '\0' && check_size > 0) 
  {
    if (*second == '\0')
      return 1;

    if (tolower(*first) != tolower(*second))
    {
      /* Need better return values FIXME */
      return 1;
    }

    first++;
    second++;
    check_size--;
  }

  if (*first != '\0' || *second != '\0')
    return 1;

  return 0;
}

int tstrcasecmp(const char *first, const char *second)
{
  while (*first != '\0')
  {
    if (*second == '\0')
      return 1;

    if (tolower(*first) != tolower(*second))
    {
      /* Need better return values FIXME */
      return 1;
    }

    first++;
    second++;
  }

  if (*first != '\0' || *second != '\0')
    return 1;

  return 0;
}


/* because some implementations don't have strdup()? */
char *tstrdup(const char *ptr)
{
  char *local = NULL;

  if (ptr == NULL)
    local = NULL;
  else 
  {
    local = tmalloc0(strlen(ptr) + 1);

    strcpy(local,ptr);
  }

  return local;
}

void tstrfreev(char *ptr[])
{
  int i = 0;

  if ((char **)ptr == NULL)
    return;

  while (ptr[i] != NULL)
  {
    free(ptr[i]);
    i++;
  }

  free((char **)ptr);

  return;
}


char *tstrtrim(char *data)
{
  char *end = &data[strlen(data)-1];

  while (*data == ' ' || *data == '\t' || *data == '\r' || *data == '\n') data++;
  while (*end == ' ' || *data == '\t' || *data == '\r' || *data == '\n'){ *end = '\0'; end--; }

  return data;

}

void *tmalloc(size_t size)
{
  void *ret;

  if ((ret = malloc(size)) == NULL)
  {
    troll_debug(LOG_FATAL,"Allocation failed");
  }

  return ret;
}

void *tmalloc0(size_t size)
{
  void *ret;

  ret = tmalloc(size);

  memset(ret,0,size);

  return ret;
}


char *tcrealloc0(char *ptr, size_t size, unsigned int *bufsize)
{
  char *new = NULL;

  if ((new = realloc(ptr,size)) == NULL)
  {
    free(ptr);
    exit(EXIT_FAILURE);
  }

  if ((*bufsize) < size)
  {
    /* (*bufsize)-1 == strlen(new) + 1 - 1 */
    memset(&new[(*bufsize/sizeof(*ptr))-1],0,size-(*bufsize));
  }

  *bufsize = size;

  return new;
}

char **tsrealloc0(char **ptr, size_t size, unsigned int *bufsize)
{
  char **new = NULL;

  if ((new = realloc(ptr,size)) == NULL)
  {
    tstrfreev(ptr);
    exit(EXIT_FAILURE);
  }

  if ((*bufsize) < size)
  {
    /* (*bufsize)-1 == strlen(new) + 1 - 1 */
    memset(&new[(*bufsize/sizeof(*ptr))-1],0,size-(*bufsize));
  }

  *bufsize = size;

  return new;
}

