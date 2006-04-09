/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine and     *
 * and kicken from DALnet     *               
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
#include "servers.h"
#include "tconfig.h"

struct tconfig_blocks *master_blocks = NULL;

static void allocate_master_blocks(const char *type,
                                   const char *identifier,
                                   const char **data)
{
  master_blocks = tmalloc(sizeof(struct tconfig_blocks));

  master_blocks->prev   = master_blocks->next  = NULL;
  master_blocks->parent = master_blocks->child = NULL;
 
  master_blocks->type = type;
  master_blocks->data = identifier;

  master_blocks->identifier = data;    
}

static void free_master_blocks(void)
{
  struct tconfig_blocks *tmp_parent   = NULL,
                        *tmp_ptr      = NULL,
                        *local_blocks = master_blocks;
   
  /* go to parent */
  while (local_blocks->parent != NULL)
    local_blocks = local_blocks->parent;

  /* Go to beginning */
  while (local_blocks->prev != NULL)
    local_blocks = local_blocks->prev;
  
  /* Go to child */
  while (local_blocks->child != NULL)
    local_blocks = local_blocks->child;

  while (local_blocks)
  {
    tmp_parent = local_blocks->parent;

    while (local_blocks)
    {
      tstrfreev(local_blocks->data);
      free(local_blocks->identifier);
      free(local_blocks->type);

      tmp_ptr = local_blocks;   

      local_blocks = local_blocks->next;

      free(tmp_ptr);
    }

    local_blocks = tmp_parent;

  }
 
  return;
}

int tconfig_load(void)
{
  FILE *fp;
  char buffer[BUFFER_SIZE];  

  if ((fp = fopen("./trollbot.conf","r")) == NULL)
  {
    printf("Could not open trollbot.conf\n");
    exit(1); /* Do we need to die cleanly here? */
  }

  memset(buffer,0,sizeof(buffer));

  while (fgets(buffer,sizeof(buffer),fp) != NULL)
  {
    /* If this is true, the line was longer than BUFFER_SIZE */
    if (buffer[strlen(buffer)-1] != '\n' && buffer[strlen(buffer)-1] != '\r')
    {
      printf("Unhandled exception in %s near line %d FIXME\n",__FILE__,__LINE__);
      exit(1);
    }
    
    tconfig_parse_line(buffer);
 
    memset(buffer,0,sizeof(buffer));
  }

  free_master_blocks();
  return tconfig_check_validity();  
}

void tconfig_parse_line(const char *buffer)
{
  static char *lastword       = NULL;
  static int   block_ref      = 0;
  static struct tconfig_block *curblk = NULL;
  
  char *word = NULL;
  char *ptr  = buffer;

  int i = 0;

  /* Skip initial whitespace */
  while (*ptr)
  {
    if (*ptr == ' ' || *ptr == '\t')
      ptr++;
    else
      break;
  }

  if (*ptr == ';')
    return; /* This line is a comment */

  /* Get the next word */
  word = tmalloc0(strlen(buffer) + 1);
  
  while (*ptr != ' ' && *ptr != '\t' && *ptr != '\r' && *ptr != '\n')
  {
    word[i] = *ptr;
  }    
    
  if (lastword == NULL)
  {
    /* We haven't encountered a block yet, check for "network",
     * "channels", "nicks", and "options" so far
     */
    if (!strcmp(word,"network"))
    {
     
    } else if (!strcmp(word,"channels"))
    {
      /* start a channels block */
    } else if (!strcmp(word,"nicks"))
    {
      /* start a nicks block */
    } else if (!strcmp(word,"options"))
    {
      /* start an options block */
    }
  }

   
  /* block_ref should never be over 2 */
  if (lastword != NULL)
  {
    if (!strcmp(lastword,"network"))
    {
      /* This next word should be the identifier, block_ref should be 0 */
    } else if (!strcmp(lastword,"nicks"))
    {
      /* if block_ref == 0 then it's global, if block_ref == 1 then it's for a network  */
    } else if (!strcmp(lastword,"channels"))
      /* if block_ref == 0 then it's global, if block_ref == 1 then it's for a channel */ 
    }else {
      /* Unhandled exception */
    }
  }

  return;  
 
}

int tconfig_check_validity(void)
{
  return 1;
}

