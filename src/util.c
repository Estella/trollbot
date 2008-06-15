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
#include "debug.h"

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

int slist_remove(struct slist *list, struct slist_node *node, void **data)
{
	struct slist_node *nodesearch = NULL;
	struct slist_node *oldnode    = NULL;
	struct slist_node *find       = NULL;
	int i = 0;

	if (list->size == 0)
		return -1;

	/* I wonder why I did this */
	if (node == NULL)
	{
		*data      = list->head->data;
		oldnode    = list->head;
		list->head = list->head->next;

		list->size--;

		if (list->size == 0)
			list->tail = NULL;

		return 0;
	}
	else
	{
		nodesearch = list->head;
		oldnode    = NULL;
		
		for (i=0; i<list->size; i++)
		{
			if (nodesearch == node)
				break;

			oldnode    = nodesearch;
			nodesearch = nodesearch->next;
		}
		
		if (nodesearch != NULL)
			slist_remove_next(list, oldnode, &data);

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

	/* Count the number of list entries */
	while (*sch)
	{
		if (*sch == ' ' || *sch == '\t')
		{
			list_size++;
			whole_size += chunk_size;
			chunk_size = 0;
		}

		chunk_size++;
		sch++;
	}

	/* Count the last one until NULL */
	list_size++;
	whole_size += chunk_size;

	/* bad */
	ret = (char **)tmalloc0((sizeof(char *) * list_size) + 1);

	sch = ptr;

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

		/* skip over space/tab */
		if (*sch != '\0')
			sch++;
	}

	ret[i] = NULL;

	return ret;
}


