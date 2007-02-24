/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

/* This file needs merged into util.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

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
    die_nicely();
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
    die_nicely();
  }

  if ((*bufsize) < size)
  {
    /* (*bufsize)-1 == strlen(new) + 1 - 1 */
    memset(&new[(*bufsize/sizeof(*ptr))-1],0,size-(*bufsize));
  }

  *bufsize = size;

  return new;
}
