#ifdef CONFIG_H
#include "config.h"
#endif /* CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "util.h"
#include "tconfig.h"

/* static sets the initial unchangeable structure */
struct tconfig_block *file_to_tconfig(const char *filename)
{
  FILE   *cfile;
  char   *fbuf   = NULL;
  char   *eip    = NULL;
  struct tconfig_block *block    = NULL;
  struct tconfig_block *head     = NULL;
  struct tconfig_block *search   = NULL;
  struct tconfig_block *tmp      = NULL;
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
    return NULL;
  }

  fbuf = tmalloc0(BUFFER_SIZE);

  /* Read the entire file into memory */
  for(i=0;(count = fread(&fbuf[i],1,1024,cfile)) == 1024;i+=1024)
  {
    if ((fbuf = realloc(fbuf,i+1024+1024)) == NULL)
    {
      troll_debug(LOG_FATAL,"Could not allocate memory for config file, barfing.\n");
      exit(EXIT_FAILURE);
    }
  }
  
  /* If NOT the end-of-file, we must have had an error of some sort, barf and die */
  if (!feof(cfile))
  {
    troll_debug(LOG_ERROR,"An error occurred while reading the config file");
    return NULL;
  }
 
  /* Terminate it with a NULL */
  fbuf[i+count] = '\0';

  fclose(cfile);

  block = tmalloc(sizeof(struct tconfig_block));

  block->parent = NULL;
  block->child  = NULL;
  block->prev   = NULL;
  block->next   = NULL;

  block->key    = NULL;
  block->value  = NULL;

  head = block;
    
  eip = fbuf;

  while(*eip)
  {
    switch (*eip)
    {
      case '{':
        if (block->child != NULL)
        {
          block = block->child;
          break;
        }
        block->child = tmalloc(sizeof(struct tconfig_block));

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
        if (*(eip+1) != '*' && *(eip+1) != '/')
        {
          fprintf(stderr,"Stray / on line: %d\n",line);
          exit(EXIT_FAILURE);
        }

        /* If comment takes C form */
        if (*(eip+1) == '*')
        {
          /* Skip over / and * */
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
            return head;
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
                return head;

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
            return head;                
        }

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
          return head;


        break;
      case '\n':
        line++;
          
        break;
      case '\t':
        break;
      case ' ':
        break;
      default:
        tmp        = tmalloc(sizeof(struct tconfig_block));

        /* Do a wasteful scan so I don't have to deal with realloc */
        if (*eip == '"')
        {
          eip++; /* Skip the first " */
          for (i=0;*(eip+i) != '\0' && *(eip+i) != '\n' && *(eip+i) != '\r' &&
                   *(eip+i) != '"';i++);

          if (*(eip+i) != '"')
          {
            fprintf(stderr, "Unmatched \" on line %d\n",line);
            exit(EXIT_FAILURE);
          }
        }
        else
        { 
          for (i=0;*(eip+i) != '\0' && *(eip+i) != ' ' && *(eip+i) != '\t';i++);
            
          if (*(eip+i) != ' ' && *(eip+i) != '\t')
          {
            fprintf(stderr, "Unmatched key on line %d\n",line);
            exit(EXIT_FAILURE);
          }
        }

        size = i;
  
        tmp->key = tmalloc0(size+1);

        for (i=0;i<size;i++)
        {
          *(tmp->key+i) = *(eip++);
        }
   
        /* Skip over trailing " if exists, this could be a problem if key is next to value */
        if (*eip == '"')
          eip++;

        /* Skip over whitespace, print error if EOF or there's a newline reached before text */
        while(*eip == '\t' || *eip == ' ' || *eip == '\r' || *eip == '\n' || *eip == '\0')
        {
          if (*eip == '\0')
          {
            fprintf(stderr, "Reached end of file looking for %s's value on line %d\n",block->key,line);
            exit(EXIT_FAILURE);
          }

          if (*eip == '\r' || *eip == '\n')
          {
            fprintf(stderr, "Reached end of line looking for %s's value on line %d\n",block->key,line);
            exit(EXIT_FAILURE);
          }
    
          eip++;
        }

        /* Now to scan the value */
        /* Do a wasteful scan so I don't have to deal with realloc */
        if (*eip == '"')
        {
          eip++; /* Skip the first " */
          for (i=0;*(eip+i) != '\0' && *(eip+i) != '\n' && *(eip+i) != '\r' &&
                   *(eip+i) != '"';i++);

          if (*(eip+i) != '"')
          {
            fprintf(stderr, "Unmatched \" on line %d\n",line);
            exit(EXIT_FAILURE);
          }
        }
        else
        {
          for (i=0;*(eip+i) != '\0' && *(eip+i) != ' ' && *(eip+i) != '\t' &&
                   *(eip+i) != '\r' && *(eip+i) != '\n';i++);

          if (*(eip+i) != ' ' && *(eip+i) != '\t' && *(eip+i) != '\r' && *(eip+i) != '\n')
          {
            fprintf(stderr, "Unmatched key on line %d\n",line);
            exit(EXIT_FAILURE);
          }
        }

        size = i;

        tmp->value = tmalloc0(size+1);

        for (i=0;i<size;i++)
        {
          *(tmp->value+i) = *(eip++);
        }

        tmp->value[size] = '\0';
 
        /* Rewind list */
        search = block;

        while (search->prev != NULL)
          search = search->prev;

        while (search != NULL)
        {
          if (search->key != NULL && search->value != NULL)
          {
            if (!strcmp(search->key,tmp->key) && !strcmp(search->value,tmp->value))
            {
              /* Same block and key, if previous block was a block with children,
               * jump into that block, elsewise, put in a dupe 
               */
              free(tmp->key);
              free(tmp->value);
              free(tmp);
              tmp = NULL;
              block = search;
              break;
            } 
          }

          block  = search;

          search = search->next;
        }

        if (tmp != NULL)
        {
          if (block->key == NULL && block->value == NULL)
          {
            block->key   = tmp->key;
            block->value = tmp->value;
 
            /* Keeping above memory */
            free(tmp);            
          } 
          else
          {
            /* Make a new block */
            block->next = tmp;
            tmp->prev   = block;
            tmp->next   = NULL;
            tmp->parent = NULL;
            tmp->child  = NULL;
            block       = block->next;
          }
        }

        break;          
    }   

    eip++; /* While loop increase */
  }

  return head;
}

static void tconfig_r_depth_first_write(int depth, FILE *out, struct tconfig_block *tmp)
{
  int i;

  if (tmp == NULL)
    return;

  if (tmp->child != NULL)
  {
    fputc('\n',out);

    for(i=0;i<(depth*2);i++)
      fputc(' ',out);

    if (strchr(tmp->key,' ') || strchr(tmp->key,'\t'))
      fputc('"',out);

    fprintf(out,"%s",tmp->key);

    if (strchr(tmp->key,' ') || strchr(tmp->key,'\t'))
      fputc('"',out);

    fputc('\t',out);

    if (strchr(tmp->value,' ') || strchr(tmp->value,'\t'))
      fputc('"',out);

    fprintf(out,"%s",tmp->value);

    if (strchr(tmp->value,' ') || strchr(tmp->value,'\t'))
      fputc('"',out);

    fputc('\n',out);

    for(i=0;i<(depth*2);i++)
      fputc(' ',out);

    fprintf(out,"{\n");

    tmp = tmp->child;
    depth++;
  }
  else if (tmp->next != NULL)
  {
    for(i=0;i<(depth*2);i++)
      fputc(' ',out);

    if (strchr(tmp->key,' ') || strchr(tmp->key,'\t'))
      fputc('"',out);

    fprintf(out,"%s",tmp->key);

    if (strchr(tmp->key,' ') || strchr(tmp->key,'\t'))
      fputc('"',out);

    fputc('\t',out);

    if (strchr(tmp->value,' ') || strchr(tmp->value,'\t'))
      fputc('"',out);

    fprintf(out,"%s",tmp->value);

    if (strchr(tmp->value,' ') || strchr(tmp->value,'\t'))
      fputc('"',out);

    fputc('\n',out);

    tmp = tmp->next;
  }
  else
  {
    for(i=0;i<(depth*2);i++)
      fputc(' ',out);

    if (strchr(tmp->key,' ') || strchr(tmp->key,'\t'))
      fputc('"',out);

    fprintf(out,"%s",tmp->key);

    if (strchr(tmp->key,' ') || strchr(tmp->key,'\t'))
      fputc('"',out);

    fputc('\t',out);

    if (strchr(tmp->value,' ') || strchr(tmp->value,'\t'))
      fputc('"',out);

    fprintf(out,"%s",tmp->value);

    if (strchr(tmp->value,' ') || strchr(tmp->value,'\t'))
      fputc('"',out);

    fputc('\n',out);

    if (depth > 0)
    {
      while (tmp)
      {
        if (tmp->next == NULL)
        {
          if (depth == 0)
            return;

          while (tmp->parent == NULL)
          {
            tmp = tmp->prev;
          }
 
          tmp = tmp->parent;
          depth--;

          for(i=0;i<(depth*2);i++)
            fputc(' ',out);
          
          fprintf(out,"}\n\n");

        } 
        else
        {
          tmp = tmp->next;
          break;
        }
      }
    } 
    else
      return;         
  }

  tconfig_r_depth_first_write(depth,out,tmp);
}


/* write a tconfig block back out */
void tconfig_to_file(struct tconfig_block *tcfg, char *filename)
{
  FILE *out;
  struct tconfig_block *tmp = tcfg;

  if (tmp == NULL)
  {
    troll_debug(LOG_WARN,"tconfig_to_file() passed a NULL tconfig block.");
    return;
  }

  if ((out = fopen(filename,"w")) == NULL)
  {
    troll_debug(LOG_ERROR,"Could not open %s for writing.",filename);
    return;
  }
 
  tconfig_r_depth_first_write(0,out,tmp);


  fclose(out);
}

char *tconfig_get_subparam(struct tconfig_block *tcfg, const char *search)
{
  tcfg = tcfg->child;

  while (tcfg)
  {
    if (!strcmp(tcfg->key,search))
      return tcfg->value;

    tcfg = tcfg->next;
  }

  return NULL;
}

void tconfig_foreach_width(struct tconfig_block *tcfg, int (*cback)(struct tconfig_block *,void *), void *extra)
{
  while (tcfg)
  {
    if ((*cback)(tcfg,extra))
      return;
    tcfg = tcfg->next;
  }

  return;
}

void tconfig_foreach_child(struct tconfig_block *tcfg, int (*cback)(struct tconfig_block *))
{
  while (tcfg)
  {
    if (!(*cback)(tcfg))
      return;
    tcfg = tcfg->child;
  }

  return;
}

void tconfig_foreach_depth_first(struct tconfig_block *tcfg, int (*cback)(struct tconfig_block *, int, void *), void *extra)
{
  int depth = 0;

  while (tcfg != NULL)
  {
    if (tcfg->child != NULL)
    {
      if (!(*cback)(tcfg,depth,extra))
        return;

      depth++;
     
      tcfg = tcfg->child;

      continue;
    }
 
    if (tcfg->next != NULL)
    {
      if (!(*cback)(tcfg,depth,extra))
        return;

      tcfg = tcfg->next;
      
      continue;
    }

    if (!(*cback)(tcfg,depth,extra))
      return;

     while (tcfg->next == NULL)
     {
       while (tcfg->parent == NULL)
         tcfg = tcfg->prev;
     
       tcfg = tcfg->parent;

       depth--;

       if (depth == 0)
         break;
     }

    
     tcfg = tcfg->next;

  }

  return;    
}

void free_tconfig(struct tconfig_block *tcfg)
{
  struct tconfig_block *oldptr;
  int depth = 0;

  while (tcfg != NULL)
  {
    if (tcfg->child != NULL)
    {
      depth++;

      tcfg = tcfg->child;

      continue;
    }

    if (tcfg->next != NULL)
    {
      tcfg = tcfg->next;

      continue;
    }

    while ((tcfg) && tcfg->next == NULL)
    {    
      while ((tcfg) && tcfg->parent == NULL)
      {
        free(tcfg->key);
        free(tcfg->value);
        
        oldptr = tcfg;

        tcfg = tcfg->prev;

        free(oldptr);
      }

      if (tcfg == NULL)
        return;

      free(tcfg->key);
      free(tcfg->value);

      tcfg = tcfg->parent;
       
      free(tcfg->child);

      depth--;
    }

    tcfg = tcfg->next;

  }

  return;
}

/* Isolate a tconfig block and its children */
struct tconfig_block *tconfig_isolate(struct tconfig_block *tcfg)
{
  if (tcfg->next != NULL)
  {
    tcfg->next->prev = tcfg->prev;
  }
  
  if (tcfg->parent != NULL)
  {
    tcfg->parent->child = NULL;
  }

  if (tcfg->prev != NULL)
  {
    tcfg->prev->next = tcfg->next;
  }

  tcfg->prev   = NULL;
  tcfg->next   = NULL;
  tcfg->parent = NULL;
 
  return tcfg;
}

/* Modifies dst, but never the actual dst pointer */
void tconfig_merge(struct tconfig_block *src, struct tconfig_block *dst)
{
  struct tconfig_block *tmp;
  int depth      = 0;

  /* We want to start at the initial positions and traverse down from 
   * there, copying the keys and values and subkeys maintaining current
   * order if it already exists.
   */
  if (src == NULL || dst == NULL)
    return;

  while (src != NULL)
  {      
    if (src->child != NULL)
    {
      /* Do wildcards recursively, I only figure it will
       * get a few levels in depth
       */
      if ((src->value != NULL) && !strcmp(src->value,"*"))
      {
        /* The trick is to loop through the current depth
         * matching the key to the wildcard's key, if so,
         * and call this function after going to child of each
         * making them both in the toplevel during the merge
         */
        tmp = dst;

        while (tmp->prev != NULL) tmp = tmp->prev;
        
        while (tmp != NULL)
        {
          if (tmp->key != NULL)
          {
            if (!strcmp(src->key,tmp->key))
            {
              if (tmp->child == NULL)
              {
                tmp->child         = tmalloc(sizeof(struct tconfig_block));
                tmp->child->parent = tmp;
                tmp                = tmp->child;
                tmp->key           = NULL;
                tmp->value         = NULL;
                tmp->prev          = NULL;
                tmp->next          = NULL;
                tmp->child         = NULL;
                tmp                = tmp->parent; /* Return back to the parent */
              } 

              tconfig_merge(src->child,tmp->child);

            }

          }
 
          tmp = tmp->next;
        } 

        src = src->next;
        continue;
      }

      tmp = dst;

      /* Make sure it's at the beginning */
      while (tmp->prev != NULL) tmp = tmp->prev;

      while (tmp != NULL)
      {
        if (tmp->key != NULL)
        {
          if (!strcmp(src->key,tmp->key) && !strcmp(src->value,tmp->value))
          {
            /* It's a match, merely attach to it */
            dst = tmp->child;
            break;
          }
        }

        tmp = tmp->next;
      }

      if (tmp == NULL)
      {
        /* No matches, create they key/value parent node */
        while (dst->next != NULL && dst->key != NULL) dst = dst->next;
        
        if (dst->key != NULL)
        {
          dst->next = tmalloc(sizeof(struct tconfig_block));
        
          dst->next->prev    = dst; 
          dst->next->next    = NULL;
          dst                = dst->next;
        }
       
        dst->key           = tstrdup(src->key);
        dst->value         = tstrdup(src->value);
        dst->parent        = NULL;

        dst->child         = tmalloc(sizeof(struct tconfig_block));
        dst->child->parent = dst;
        dst                = dst->child;
        
        dst->key           = NULL;
        dst->value         = NULL;
        dst->child         = NULL;
        dst->prev          = NULL;
        dst->next          = NULL;
      } 

      depth++;

      src = src->child;
      continue;
    }

    tmp = dst;

    /* Make sure it's at the beginning */
    while (tmp->prev != NULL) tmp = tmp->prev;

    while (tmp != NULL)
    {
      if (tmp->key != NULL)
      {
        /* No Duplicate keys on merge unless prepended with @ */
        if (!strcmp(src->key,tmp->key) && src->key[0] != '@')
        {
          /* Ignore dupes */
          break;
        }
      }

      tmp = tmp->next;
    }

    if (tmp == NULL)
    {
      /* go to the end of the list */
      while (dst->next != NULL && dst->key != NULL) dst = dst->next;

      if (dst->key != NULL)
      {
        dst->next       = tmalloc(sizeof(struct tconfig_block));
        dst->next->prev = dst;
        dst             = dst->next;
        dst->next       = NULL;
        dst->parent     = NULL;
        dst->child      = NULL;
      }

      if (src->key[0] == '@')
        dst->key      = tstrdup(&src->key[1]);
      else
        dst->key      = tstrdup(src->key);

      dst->value      = tstrdup(src->value);
      dst->next       = NULL;
    }

    while (src->next == NULL)
    {
      if (depth == 0)
        return;

      while (src->prev != NULL) src = src->prev;
      
      /* Do the same to dst */
      while (dst->prev != NULL) dst = dst->prev;      

      src = src->parent;
      dst = dst->parent;

      depth--; 
    }
     
    src = src->next;
  }

  
}



static int tconfig_replace(struct tconfig_block *tcfg, int depth, void *extra)
{
  char *replace = NULL;

  printf("%s - %s\n",tcfg->key,tcfg->value);

  if (tcfg->parent != NULL)
    replace = tcfg->parent->value;

  tstr_replace(tcfg->value, "%pkey%", replace);
  tstr_replace(tcfg->value, "%pvalue", replace);  
  
  return 1;
}

/* This function goes through the entire tconfig tree
 * replacing %pkey% and %pvalue% with their respective
 * values, this seems kind of ugly.
 */
void tconfig_final_pass(struct tconfig_block *tcfg)
{
/*  while (tcfg->prev != NULL  && tcfg->parent != NULL)
    tcfg = (tcfg->parent != NULL) ? tcfg->parent : tcfg->prev;

  tconfig_foreach_depth_first(tcfg, &tconfig_replace, NULL);*/
}
