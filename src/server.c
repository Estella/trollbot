/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************
 * Last Audit: 2007-04-30     *
 ******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "util.h"

void server_list_add(struct server **orig, struct server *new)
{
	struct server *tmp;

	if (*orig == NULL)
	{
		*orig = new;
		new->prev = NULL;
		new->next = NULL;
	}
	else
	{
		tmp = *orig;

		while (tmp->next != NULL)
			tmp = tmp->next;

		tmp->next       = new;
		tmp->next->prev = tmp;
		tmp->next->next = NULL;
	}
}

struct server *new_server(char *hostport)
{
	struct server *ret;
	char *ctmp;
	char *host;

	ret = tmalloc(sizeof(struct server));

	if (hostport != NULL)
	{
		if ((ctmp = strchr(hostport,':')) != NULL)
		{
			ctmp++;

			ret->port = atoi(ctmp);

			/* off by one to leave room for '\0' */
			host = tmalloc0((strlen(hostport) - strlen(ctmp)));

			strncpy(host,hostport,strlen(hostport) - strlen(ctmp) - 1);
			ret->host = host;
		} 
		else
		{
			ret->host = tstrdup(hostport);
			ret->port = 6667;
		}
	}
	else
	{
		ret->host = NULL;
		ret->port = 6667;
	}

	ret->tcfg = NULL;

	ret->prev = NULL;
	ret->next = NULL;

	return ret;

}

void free_servers(struct server *servers){
	struct server *nextServer=NULL;
	while (servers != NULL){
		nextServer = servers->next;

		free(servers->host);
		free(servers);
		servers = nextServer;
	}
}
