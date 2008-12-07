#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tsocket.h"
#include "util.h"

struct tsocket *tsocket_new(void)
{
	struct tsocket = tmalloc(sizeof(struct tsocket));

	tsocket->status = TSOCK_UNINITIALIZED;

	tsocket->sock   = -1;
	tsocket->name   = NULL;
	
	return tsocket;
}

void tsockets_free(struct slist *tsockets)
{
	struct slist_node *node = NULL;
	struct slist_node *tmp  = NULL;

	if (tsockets == NULL)
		return;
	
	node = tsockets->head;

	while (node != NULL)
	{
		tmp = node;

		node = node->next;

		tsocket_free(tmp->data);
		free(tmp);
	}

	free(tsockets);
}

void tsocket_free(struct tsocket *tsocket)
{
	free(tsocket->name);
	free(tsocket);
}

