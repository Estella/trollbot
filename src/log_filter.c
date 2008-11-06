#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Check portability */
#include <time.h>

#include "log_filter.h"
#include "log_entry.h"
#include "debug.h"
#include "network.h"
#include "util.h"


/* This goes through all setup log filters, and sends the log output to the handler
 * of the matching filter
 */
void log_filters_check(struct log_filter *filters, struct log_entry *entry)
{
	struct log_filter *tmp = NULL;
	int i;

	tmp = filters;

	while (tmp != NULL)
	{
		if (tmp->flags == NULL || entry->flags == NULL)
		{
			tmp = tmp->next;
			continue;
		}

		for (i=0;entry->flags[i] != '\0';i++)
		{
			if (strchr(tmp->flags,entry->flags[i]) == NULL)
			{
				tmp = tmp->next;
				break;
			}
		}

		/* Didn't find the flag */
		if (entry->flags[i] != '\0')
			continue;

		if (tmp->handler != NULL)
			tmp->handler(tmp->net, tmp, entry);

		tmp = tmp->next;
	}

	return;	
}

struct log_filter *log_filter_add(struct log_filter *filters, struct log_filter *add)
{
	struct log_filter *ret = NULL;

	ret = filters;

	if (ret == NULL)
	{
		ret = add;
		return ret;
	}

	while (ret->next != NULL) ret = ret->next;

	ret->next = add;
	add->prev = ret;

	return filters;
}

/* This doesn't work */
struct log_filter *log_filter_remove(struct log_filter *filters, struct log_filter *del)
{
	struct log_filter *ret = NULL;

	ret = filters;

	while (ret != del)
		ret = ret->next;

	if (ret == NULL)
	{
		troll_debug(LOG_ERROR, "log_filter_remove() called with filter that didn't exist in the list");
		return filters;
	}

	if (ret == filters)
	{
		ret = ret->next;
		log_filter_free(filters);
		return ret; /* Will be NULL if no other filters exist */
	}	

	if (ret->next != NULL)
		ret->next->prev = ret->prev;

	if (ret->prev != NULL)
		ret->prev->next = ret->next;

	return ret;
}

void log_filters_free(struct log_filter *filters)
{
	struct log_filter *tmp = NULL;

	while (filters != NULL)
	{
		tmp = filters;

		filters = filters->next;
		log_filter_free(tmp);
	}

	return;
}

void log_filter_free(struct log_filter *filter)
{
	free(filter->flags);
	free(filter);
}

struct log_filter *log_filter_new(void)
{
	struct log_filter *ret = NULL;

	ret = tmalloc(sizeof (struct log_filter));

	ret->net     = NULL;
	ret->chan    = NULL;
	ret->flags   = NULL;
	ret->handler = NULL;

	ret->prev      = NULL;
	ret->next      = NULL;

	return ret;
}
