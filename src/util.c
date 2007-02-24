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
#include "main.h"

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




