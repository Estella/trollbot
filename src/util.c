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

#include "util.h"

/* 
	 ?  matches any single character
   *  matches 0 or more characters of any type
   %  matches 0 or more non-space characters (can be used to match a single
      word)
   ~  matches 1 or more space characters (can be used for whitespace between
      words)
	 \  makes the next character literal

   returns 1 if no match, 0 if matched
*/
int matchwilds(const char *haystack, const char *needle)
{
	int escaped = 0;

	if (needle == NULL || haystack == NULL)
		return 1;

	while (*needle)
	{
		if (*haystack == '\0')
		{
			/* If *needle is '*', and *(needle+1) = '\0', this should return 0 for success */
			if ((*needle == '*') && *(needle+1) == '\0' && escaped == 0)
				return 0;
			/* Hit end of haystack but not the ned of needle, so match fails. */
			return 1;
		}

		if (*needle == '\\')
		{
			escaped = 1;
			needle++;
		}

		if (*needle == '?' && escaped == 0)
		{
			/* Any character matches, just move on. */
			needle++;
			haystack++;
		}
		else if (*needle == '*' && escaped == 0)
		{
			/* Match characters til end of haystack, or until *(needle+1) */
			while (*haystack != '\0' && *haystack != *(needle+1))
			{
				haystack++;
			}
			needle++;
		}
		else if (*needle == '%' && escaped == 0)
		{
			while (*haystack != '\0' && !isspace(*haystack) && *haystack != *(needle+1))
			{
				haystack++;
			}
			needle++;
		}
		else if (*needle == '~' && escaped == 0)
		{
			if (isspace(*haystack))
			{
				haystack++;
				while (*haystack != '\0' && isspace(*haystack))
				{
					haystack++;
				}
				needle++;
			}
			else 
			{
				/* Must match at least one space. */
				return 1;
			}
		}
		else if (*needle != *haystack)
		{
			return 1;
		}
		else 
		{
			/* Two characters match.  Next */
			needle++;
			haystack++;
	
			escaped = 0;
		}
	}

	if (*haystack == '\0')
	{
		/* Hit end of haystack and end of needle, so match succeeded */
		return 0;
	}
	else 
	{
		/* Hit end of needled, but not end of haystack, match fails. */
		return 1;
	}
}

/* Singly linked lists interface */
void slist_init(struct slist **list, void (*destroy)(void *))
{
	struct slist *new_list;

	new_list = tmalloc(sizeof(struct slist));

	new_list->size    = 0;
	new_list->head    = NULL;
	new_list->tail    = NULL;
	new_list->destroy = destroy;  

	*list = new_list;
}

void slist_destroy(struct slist *list)
{
	void *data;

	if (list == NULL)
		return;

	while (list->size > 0)
	{
		if (slist_remove_next(list,NULL,&data) == 0)
		{
			if (list->destroy != NULL)
				list->destroy(data);
		}
	}
}

int slist_insert_next(struct slist *list, struct slist_node *node, void *data)
{
	struct slist_node *newnode;

	newnode = tmalloc(sizeof(struct slist_node));

	newnode->data = data;

	if (node == NULL)
	{
		if (list->size == 0)
			list->tail = newnode;

		newnode->next = list->head;
		list->head    = newnode;
	}
	else
	{
		if (newnode->next == NULL)
			list->tail = newnode;

		newnode->next = node->next;
		node->next    = newnode;
	}

	list->size++;

	return 0;    
}

/* 
 * We have two options here, search by node ptr or data ptr
 * removes from the slist, calls delete function on it if it
 * exists. if not, sets data to it.
 */
void *slist_remove(struct slist *list, struct slist_node *node, void *data)
{
	struct slist_node *snode = NULL; /* Search node */
	struct slist_node *lnode = NULL; /* Last Node   */

	snode = list->head;

	while (snode != NULL)
	{
		if (node != NULL)
		{
			if (snode == node)
				break;
		}

		if (data != NULL)
		{
			if (data == snode->data)
				break;
		}

		lnode  = snode;
		snode  = snode->next;
	}

	if (snode != NULL)
	{
		if (lnode == NULL)
		{
			/* If it's head */
			list->head = list->head->next;
		}

		if (snode->next == NULL)
		{
			/* If it's tail */
			list->tail = lnode;
		}

		if (lnode != NULL)
		{
			/* If it's not head */
			lnode->next = snode->next;
		}

		/* Call the free function on the node's data */
		if (list->destroy != NULL)
		{
			list->destroy(snode->data);
			return NULL;
		}
		else
		{
			return snode->data;
		}
		
		free(snode);
	}

	return 0;
}

int slist_remove_next(struct slist *list, struct slist_node *node, void **data)
{
	struct slist_node *oldnode;

	if (list->size == 0)
		return -1;

	if (node == NULL)
	{
		*data      = list->head->data;
		oldnode    = list->head;
		list->head = list->head->next;

		if (list->size == 1)
			list->tail = NULL;
	}
	else
	{
		if (node->next == NULL)
			return -1;

		*data = node->next->data;

		oldnode = node->next;

		node->next = node->next->next;

		if (node->next == NULL)
			list->tail = node;
	}

	free(oldnode);

	list->size--;

	return 0;

}

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
		printf("Allocation failed");
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

char *tstrarrayserialize(char **ptr)
{
	char *ret   = NULL;
	char *esp   = NULL;
	char **hold = NULL;
	size_t alloc_size = 0;

	hold = ptr;

	while ((esp = *hold++))
	{
		alloc_size += strlen(esp) + 1;		
	}

	ret = tmalloc0(alloc_size + 1);

	hold = ptr;

	while ((esp = *hold++))
	{
		strcpy(&ret[strlen(ret)],esp);
	}

	return ret;	
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

int tstrcount(char **ptr)
{
	int count;

	if (ptr == NULL)
		return -1;

	for(count=0;ptr[count] != NULL;++count);

	return count;
}

char **tssv_split(char *ptr)
{
	char   **ret      = NULL;
	size_t whole_size = 0;
	size_t chunk_size = 0;
	size_t list_size  = 0;
	char   *sch       = NULL;
	char   *old       = NULL;
	int    i          = 0;

	/* greedy count */
	sch = ptr;

	/* skip all leading whitespace */
	while ((*sch != '\0') && (*sch == ' ' || *sch == '\t'))
		sch++;

	/* return null if it was all whitespace */
	if (*sch == '\0')
		return (char **)NULL;

	/* Count the number of list entries */
	while (*sch)
	{
		/* We found whitespace, since the above block succeeded to get here
		 * we clearly have at least 1 as chunk_size and list_size
		 */
		if (*sch == ' ' || *sch == '\t')
		{
			list_size++;
			whole_size += chunk_size;
			chunk_size = 0;

			/* Skip over all whitespace in between */
			while ((*sch != '\0') && (*sch == ' ' || *sch == '\t'))
				sch++;
		}
		else
		{
			chunk_size++;
			sch++;
		}
	}

	/* Count the last one until NULL */
	list_size++;
	whole_size += chunk_size;

	/* bad -- why? */
	ret = (char **)tmalloc0((sizeof(char *) * list_size) + 1);

	sch = ptr;

	/* This will never hit NULL, since the first one didn't */
	while ((*sch != '\0') && (*sch == ' ' || *sch == '\t'))
		sch++;

	for(i=0;i<list_size;i++)
	{
		old = sch;
		chunk_size = 0;

		while ((*sch != '\0') && *sch != '\t' && *sch != ' ')
		{
			chunk_size++;
			sch++;
		}

		ret[i] = tmalloc0(chunk_size + 1);

		strncpy(ret[i],old,chunk_size);

		while ((*sch != '\0') && (*sch == ' ' || *sch == '\t'))
		{
			sch++;
		}
	}

	ret[i] = NULL;

	return ret;
}


