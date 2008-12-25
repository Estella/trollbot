#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "t_timer.h"
#include "debug.h"
#include "util.h"

struct t_timer *t_timers_check(struct t_timer *timers, time_t timestamp)
{
	struct t_timer *tmp = NULL;
	struct t_timer *ret = NULL;

	tmp = timers;
	ret = timers;

	while (tmp != NULL)
	{
		if (tmp->time_trig <= timestamp)
		{
			if (tmp->handler != NULL)
				tmp->handler(tmp->net, tmp);

			 ret = t_timer_remove(ret, tmp);
		}

		tmp = tmp->next;
	}

	return ret;	
}

struct t_timer *t_timer_add(struct t_timer *timers, struct t_timer *add)
{
	struct t_timer *ret = NULL;

	ret = timers;

	if (ret == NULL)
	{
		ret = add;
		return ret;
	}

	while (ret->next != NULL) ret = ret->next;

	ret->next = add;
	add->prev = ret;

	return timers;
}

struct t_timer *t_timer_remove(struct t_timer *timers, struct t_timer *del)
{
	struct t_timer *ret = NULL;

	ret = timers;

	while (ret != del)
		ret = ret->next;

	if (ret == NULL)
	{
		troll_debug(LOG_ERROR, "t_timer_remove() called with timer that didn't exist in the list");
		return timers;
	}

	if (ret == timers)
	{
		timers = timers->next;
		t_timer_free(ret);
		return timers; /* Will be NULL if no other timers exist */
	}	

	if (ret->next != NULL)
		ret->next->prev = ret->prev;

	if (ret->prev != NULL)
		ret->prev->next = ret->next;

	return ret;
}

void t_timers_free(struct t_timer *timers)
{
	struct t_timer *tmp = NULL;

	while (timers != NULL)
	{
		tmp = timers;

		timers = timers->next;
		t_timer_free(tmp);
	}

	return;
}

void t_timer_free(struct t_timer *timer)
{
	free(timer->command);
	free(timer);
}

struct t_timer *t_timer_new(void)
{
	struct t_timer *ret = NULL;

	ret = tmalloc(sizeof (struct t_timer));

	ret->timer_id  = -1;

	ret->time_set  = 0;
	ret->time_trig = 0;

	ret->extra     = NULL; /* Transport only, do not attempt to free */

	ret->command   = NULL;
	ret->handler   = NULL;
	ret->net       = NULL;

	ret->prev      = NULL;
	ret->next      = NULL;

	return ret;
}
