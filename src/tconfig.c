#include "main.h"

/* static sets the initial unchangeable structure */
struct tconfig_block *file_to_tconfig(struct tconfig_block *old, const char *filename)
{
  FILE   *cfile;
  char   *fbuf   = NULL;
  char   *eip    = NULL;
  struct tconfig_block *block  = NULL;
  struct tconfig_block *head   = NULL;
  struct tconfig_block *search = NULL;
  struct tconfig_block *tmp    = NULL;
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
    return old;
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
    fprintf(stderr, "An error occurred while reading the config file\n");
    return old;
  }
 
  /* Terminate it with a NULL */
  fbuf[i+count] = '\0';

  fclose(cfile);

  if (old == NULL)
  {
    block = tmalloc(sizeof(struct tconfig_block));

    block->parent = NULL;
    block->child  = NULL;
    block->prev   = NULL;
    block->next   = NULL;

    block->key    = NULL;
    block->value  = NULL;

    head = block;
  } else {
    block = old;

    /* Go to top left node */
    while (block->parent != NULL || block->prev != NULL)
      block = (block->parent != NULL) ? block->parent : block->next;

    head = block;
  }
    
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
 
        /* Now that we've got all the values, we check for dupes
         * to link more than one tconfig file. It might make more
         */
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
               * jump into that block, elsewise, put in a dupe */
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

void tconfig_foreach_length(struct tconfig_block *tcfg, int (*cback)(struct tconfig_block *))
{
  while (tcfg)
  {
    if ((*cback)(tcfg))
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

void tconfig_foreach_depth_first(struct tconfig_block *tcfg, int (*cback)(struct tconfig_block *, int))
{
  int depth = 0;

  while (tcfg != NULL)
  {
    if (tcfg->child != NULL)
    {
      if (!(*cback)(tcfg,depth))
        return;

      depth++;
     
      tcfg = tcfg->child;

      continue;
    }
 
    if (tcfg->next != NULL)
    {
      if (!(*cback)(tcfg,depth))
        return;

      tcfg = tcfg->next;
      
      continue;
    }

    if (!(*cback)(tcfg,depth))
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

