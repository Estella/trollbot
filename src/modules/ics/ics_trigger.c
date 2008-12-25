#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "main.h"

#include "ics_server.h"
#include "ics_proto.h"
#include "ics_trigger.h"
#include "log_entry.h"

#include "server.h"

/* Adds an ICS trigger to the trigger list */
struct ics_trigger *ics_trigger_add(struct ics_trigger *ics_triggers, struct ics_trigger *add)
{
	struct ics_trigger *tmp = NULL;

	if ((tmp = ics_triggers) == NULL)
		return add;

	while (tmp->next != NULL) tmp = tmp->next;

	tmp->next = add;
	add->prev = tmp;

	return ics_triggers;
}

/* Removes an ICS trigger from the trigger list */
struct ics_trigger *ics_trigger_del(struct ics_trigger *ics_triggers, struct ics_trigger *del)
{
	struct ics_trigger *tmp = NULL;

	if ((tmp = ics_triggers) == NULL)
	{
		printf("ics_trigger_del() called with NULL trigger list");
		return NULL;
	}

	while (tmp != NULL)
	{
		if (tmp == del)
		{
			if (tmp->prev != NULL)
				tmp->prev->next = tmp->next;

			if (tmp->next != NULL)
				tmp->next->prev = tmp->prev;

			while (tmp == del && tmp->prev != NULL)
				tmp = tmp->prev;

			while (tmp == del && tmp->next != NULL)
				tmp = tmp->next;

			if (tmp == del)
				return NULL;
			else
				return tmp;

		}

		tmp = tmp->next;
	}

	printf("ics_trigger_del() called with a trigger deletion that no entry existed for");

	return ics_triggers;
}

/* constructor */
struct ics_trigger *new_ics_trigger(void)
{
	struct ics_trigger *ret;

	ret = tmalloc(sizeof(struct ics_trigger));

	ret->type     = -1;
	ret->mask     = NULL;

	ret->command  = NULL;

	ret->handler  = NULL;

	ret->usecount = 0;


	ret->prev     = NULL;
	ret->next     = NULL;

	return ret;
}

/* This is the heart of the scripting system */
int ics_trigger_match(struct ics_server *xs, struct ics_data *data)
{
	int trig_count = 0;
	struct ics_trigger *trig = xs->ics_trigger_table->msg;

	while (trig != NULL)
	{
		/* MSG Bind handler */
		if (!matchwilds(data->txt_packet, trig->mask))
		{
			if (trig->handler != NULL)
			{
				trig->handler(xs, trig, data);
				trig_count++;
			}
		}

		if (!matchwilds(data->txt_packet, "Notification:*has arrived."))
		{
			/* Check notifications for whatever is in the * */	
		}

		trig = trig->next;
	}

	return trig_count;
}

struct ics_trigger_table *new_ics_trigger_table(void)
{
	struct ics_trigger_table *ret;

	ret = tmalloc(sizeof(struct ics_trigger_table));

	ret->msg      = NULL;
	ret->notify   = NULL;
	ret->error    = NULL;
	ret->connect  = NULL;
	ret->game     = NULL;
	ret->move     = NULL;
	ret->endgame  = NULL;
	ret->tell     = NULL;

	return ret;
}

void free_ics_triggers(struct ics_trigger *ics_triggers)
{
	struct ics_trigger *tmp = ics_triggers;
	struct ics_trigger *old = NULL;

	if (tmp == NULL)
	{
		printf("free_ics_triggers() called with NULL trigger list");
		return;
	}

	while (tmp != NULL)
	{
		old = tmp->next;

		free_ics_trigger(tmp);

		tmp = old;
	}

	return;
}	

void free_ics_trigger(struct ics_trigger *ics_trigger)
{
	if (ics_trigger == NULL)
		return;

	free(ics_trigger->mask);
	free(ics_trigger->command);

	free(ics_trigger);
}


void free_ics_trigger_table(struct ics_trigger_table *xtt)
{
	free_ics_triggers(xtt->error);
	free_ics_triggers(xtt->notify);
	free_ics_triggers(xtt->msg);
	free_ics_triggers(xtt->tell);
	free_ics_triggers(xtt->connect);
	free_ics_triggers(xtt->move);
	free_ics_triggers(xtt->game);
	free_ics_triggers(xtt->endgame);
	free(xtt);
}

