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
#include "tconfig.h"

void parse_config(const char *filename)
{
  FILE   *cfile;
  char   *fbuf   = NULL;
  char   *eip    = NULL;
  char   *fptr   = NULL;
  struct tconfig_block *block = NULL;

  size_t  size   = 0;
  size_t  i      = 0;
  size_t  count  = 0;

  /* For debug purposes */
  int line       = 1;
  
  /* No memory allocated at this point, we don't need 
   * to die nicely
   */
  if ((cfile = fopen(filename,"r")) == NULL)
  {
    fprintf(stderr,"Could not open configuration file: %s\n",filename);
    exit(EXIT_FAILURE);
  }

  /* Allocate initial memory for fbuf */
  if ((fbuf = malloc(1024)) == NULL)
  {
    fprintf(stderr, "Could not allocate memory, barfing.\n");
    exit(EXIT_FAILURE);
  }

  /* Read the entire file into memory */
  for(i=0;(count = fread(&fbuf[i],1,1024,cfile)) == 1024;i+=1024)
  {
    if ((fbuf = realloc(fbuf,i+1024+1024)) == NULL)
    {
      fprintf(stderr,"Could not allocate memory for config file, barfing.\n");
      exit(EXIT_FAILURE);
    }
  }
  
  /* If NOT the end-of-file, we must have had an error of some sort, barf and die */
  if (!feof(cfile))
  {
    fprintf(stderr, "An error occurred while reading the config file\n");
    exit(EXIT_FAILURE);
  }
 
  /* Terminate it with a NULL */
  fbuf[i+count] = '\0';

  close(cfile);

  /* We now have the config file in memory, now to parse it */
  if ((block = malloc(sizeof(struct tconfig_block))) == NULL)
  {
    fprintf(stderr, "Could not allocate memory for tconfig block\n");
    exit(EXIT_FAILURE);
  }

  block->parent = NULL;
  block->child  = NULL;
  block->prev   = NULL;
  block->next   = NULL;

  block->key    = NULL;
  block->value  = NULL;
  
  eip = fbuf;

  while(*eip)
  {
    switch (*eip)
    {
      case '{':
        if ((block->child = malloc(sizeof(struct tconfig_block))) == NULL)
        {
          fprintf(stderr, "Could not allocate memory for tconfig block\n");
          exit(EXIT_FAILURE);
        } 

        block->child->parent = block;
        block                = block->child;
        block->child         = NULL;
        block->prev          = NULL;
        block->next          = NULL;
        block->key           = NULL;
        block->value         = NULL;

        break;
      
      /* The very left element in tree should have the parent */
      case '}':
        /* Rewind the list the very left */
        while (block->prev != NULL)
          block = block->prev;    
   
        if (block->parent == NULL)
        {
          fprintf(stderr, "Mismatched } around line: %d\n",line);
          exit(EXIT_FAILURE);
        }
  
        block              = block->parent;
        break;

      /* Ugly comment parsing routines */
      case '/':
        if (*(eip+1) != '*' || *(eip+1) != '/')
        {
          fprintf(stderr,"Stray / on line: %d\n",line);
          exit(EXIT_FAILURE);
        }

        /* If comment takes C form */
        if (*(eip+1) == '*')
        {
          /* Skip over /* */
          eip+=2;

          /* Go until terminating -> */
          while (*eip != '\0') 
          {
            if (*(eip+1) == '\0')
            {
              fprintf(stderr,"Reached end of file while looking for */\n");
              exit(EXIT_FAILURE);
            }

            if (*eip == '*' && *(eip+1) == '/')
            {
              /* loop increases eip, so only add 1 */
              eip++;
              break; /* Found end of comment */
            } 

            if (*eip == '\r' && *(eip+1) == '\n')
            {
              eip++;
              line++;
            }
            else if (*eip == '\n')
            {
              line++;
            }
            else if (*eip == '\r')
            {
              line++;
            }

            eip++;
          }

          if (eip == '\0')
          {
            return;
          }   
        }
        else /* // comment, go until \r\n, \n, or \r */
        {
          eip+=2; /* Skip over // */
         
          while (*eip != '\0')
          {
            if (*eip == '\r')
            {
              /* This just means the file is over, not an error */
              if (*(eip+1) == '\0')
                return;

              if (*(eip+1) != '\n')
              {
                eip++;
                line++;
                break;
              }
              else
              {
                line++;
                break;
              }
            }
            else if (*eip == '\n')
            {
              line++;
              break;
            }

            eip++;
          }
         
          if (*eip == '\0')
            return;                

          break;
        case '\r':
          if (*(eip+1) == '\n')
          {
            eip++;
            line++;
          }
          else
            line++;

          if (eip == '\0')
            return;


          break;
        case '\n':
          line++;
          
          break;
        case '\t':
          break;
        case ' ':
          break;
        default:
          if (block->key == NULL)
          {
            /* Do a wasteful scan so I don't have to deal with realloc */
            if (*eip != '"')
            {
              for (i=0;*(eip+i+1) != '\0' && *(eip+i+1) != '\n' && *(eip+i+1) != '\r' &&
                       *(eip+i+1) != '"';i++);
            }
            else
            { 
              for (i=0;*(eip+i) != '\0' && *(eip+i) != '"';i++);
              eip++;
            }

            if (*(eip+i) == '\0')
            {
              fprintf(stderr, "Reached end of file while looking for value on line %d\n",line);
              exit(EXIT_FAILURE);
            }

            size = i;
  
            /* No I'm not off by 1, in the case of a " " string, we're over by 1 */
            if ((block->key = malloc(size)) == NULL)
            {
              fprintf(stderr,"Could not allocate memory for config key\n");
              exit(EXIT_FAILURE);
            }

            for (i=0;*(eip+i) != ' ' && *(eip+i) != '\t';i++)
            {
              *(block->key+i) = *(eip+i);
            }
        
          
    }   

    eip++; /* While loop increase */
  }
}

