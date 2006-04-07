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

static void allocate_master_blocks(void)
{
}

static void free_master_blocks(void)
{
}

/* small rewrite */
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

  allocate_master_blocks();

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
  
  char *ptr = buffer;

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

