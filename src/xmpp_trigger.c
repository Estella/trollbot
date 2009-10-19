#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "trollbot.h"

#include "xmpp_server.h"
#include "xmpp_proto.h"
#include "xmpp_trigger.h"
#include "log_entry.h"

#include "server.h"

struct xmpp_trigger *xmpp_trigger_add(struct xmpp_trigger *xmpp_triggers, struct xmpp_trigger *add)
{
	struct xmpp_trigger *tmp = NULL;

	if ((tmp = xmpp_triggers) == NULL)
		return add;

	while (tmp->next != NULL) tmp = tmp->next;

	tmp->next = add;
	add->prev = tmp;

	return xmpp_triggers;
}

struct xmpp_trigger *xmpp_trigger_del(struct xmpp_trigger *xmpp_triggers, struct xmpp_trigger *del)
{
	struct xmpp_trigger *tmp = NULL;

	if ((tmp = xmpp_triggers) == NULL)
	{
		log_entry_printf(NULL,NULL,"X","xmpp_trigger_del() called with NULL trigger list");
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

	log_entry_printf(NULL,NULL,"X","xmpp_trigger_del() called with a trigger deletion that no entry existed for");

	return xmpp_triggers;
}

struct xmpp_trigger *new_xmpp_trigger(void)
{
	struct xmpp_trigger *ret;

	ret = tmalloc(sizeof(struct xmpp_trigger));

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
int xmpp_trigger_match(struct xmpp_server *xs, struct xmpp_data *data)
{
	int trig_count = 0;

	/* XML Shiat */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node      = NULL;
	xmlNodePtr splnker   = NULL;

	root_node = xmlDocGetRootElement(data->xml_packet);

	/* If it's not a stream packet, I don't know what to do with it */
	if ((xmlStrcmp(root_node->name, (const xmlChar *)"stream")))
		return 0;


	/* Go to the child node pointer */	
	node = root_node->xmlChildrenNode;

	while (node != NULL)
	{
		if (!xmlStrcmp(node->name, (const xmlChar *)"error"))
		{
			/* Errors should have their type in a child node */
			printf("Got an error!!!!\n");

			if ((splnker = node->xmlChildrenNode) == NULL)
			{
				printf("Error had no body\n");
				return trig_count;
			}

			/* Find the trigger */
			/* Call the handler */
			printf("Error type: %s\n",splnker->name);
		} /* error */

		node = node->next;
	}

	/* This is going to be fucking ugly */
	/* Figure out what type of triggers it could possibly match */
	/* ERROR */




	/* Let's handle the error one first shall we? */

	/* MESSAGE */

	return trig_count;
}

struct xmpp_trigger_table *new_xmpp_trigger_table(void)
{
	struct xmpp_trigger_table *ret;

	ret = tmalloc(sizeof(struct xmpp_trigger_table));

	ret->msg      = NULL;
	ret->error    = NULL;

	return ret;
}

void free_xmpp_triggers(struct xmpp_trigger *xmpp_triggers)
{
	struct xmpp_trigger *tmp = xmpp_triggers;
	struct xmpp_trigger *old = NULL;

	if (tmp == NULL)
	{
		log_entry_printf(NULL,NULL,"X","free_xmpp_triggers() called with NULL trigger list");
		return;
	}

	while (tmp != NULL)
	{
		old = tmp->next;

		free_xmpp_trigger(tmp);

		tmp = old;
	}

	return;
}	

void free_xmpp_trigger(struct xmpp_trigger *xmpp_trigger)
{
	if (xmpp_trigger == NULL)
		return;

	free(xmpp_trigger->mask);
	free(xmpp_trigger->command);

	free(xmpp_trigger);
}


void free_xmpp_trigger_table(struct xmpp_trigger_table *xtt)
{
	free_xmpp_triggers(xtt->error);
	free_xmpp_triggers(xtt->msg);
	free(xtt);
}

