/*********************************************
 * TrollBot v1.0                             *
 *********************************************
 * TrollBot is an eggdrop-clone designed to  *
 * work with multiple networks and protocols *
 * in order to present a unified scriptable  *
 * event-based platform,                     *
 *********************************************
 * This software is PUBLIC DOMAIN. Feel free *
 * to use it for whatever use whatsoever.    *
 *********************************************
 * Originally written by poutine/DALnet      *
 *                       kicken/DALnet       *
 *                       comcor/DALnet       *
 *********************************************/
#include <stdio.h>
#include <time.h>

#include "util.h"

#include "http_server.h"
#include "http_request.h"
#include "http_proto.h"


struct http_request *http_request_add(struct http_request *http_request, struct http_request *add)
{
	struct http_request *tmp = NULL;

	if ((tmp = http_request) == NULL)
		return add;

	while (tmp->next != NULL) tmp = tmp->next;

	tmp->next = add;
	add->prev = tmp;

	return http_request;
}

struct http_request *http_request_del(struct http_request *http_request, struct http_request *del)
{
	struct http_request *tmp = NULL;

	if ((tmp = http_request) == NULL)
	{
		log_entry_printf(NULL,NULL,"X","http_request_del() called with NULL server list");
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

	log_entry_printf(NULL,NULL,"X","http_request_del() called with a server deletion that no entry existed for");

	return http_request;

}

struct http_request *new_http_request(void)
{
	struct http_request *req;

	req = tmalloc(sizeof(struct http_request));

	/* This has a new sock */
	req->sock = -1;
		
	req->host = NULL;
	req->port = 1;
	
	req->request_time = -1;

	req->return_buffer = NULL;

	req->headers = NULL;

	req->post = NULL;
	req->get = NULL;

	req->status = HTTP_NOTREADY;

	req->server = NULL;

	req->prev = NULL;
	req->next = NULL;

	req->response = NULL;

	return req;
}

void free_http_requests(struct http_request *reqs)
{
	struct http_request *sav = NULL;

	while (reqs != NULL)
	{
		sav = reqs->next;
		
		free_http_request(reqs);

		reqs = sav;
	}

	return;
}

void free_http_request(struct http_request *req)
{
	/* FIXME: Incomplete, laziness */
	free(req->host);
	free(req->response);

}

