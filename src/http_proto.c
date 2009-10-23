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
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <unistd.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>


#include "http_proto.h"
#include "http_server.h"
#include "http_request.h"

#include "trollbot.h"
#include "debug.h"
#include "util.h"

/* Constructor */
struct http_data *http_data_new(void)
{
	struct http_data *local;

	local = tmalloc(sizeof(struct http_data));

	local->txt_packet = NULL;

	return local;
}

/* Destructor */
void http_data_free(struct http_data *data)
{
	free(data->txt_packet);
	tstrfreev(data->tokens);

	free(data);
}

void http_init_listener(struct http_server *http)
{
	char *httphostip;
	struct sockaddr_in httpaddr;
	struct hostent *he;
	int yes=1;

	if (http->host == NULL)
	{
		troll_debug(LOG_WARN,"You need to specify a host for a HTTP server");
		http->sock = -2;
		return;
	}

	httphostip = NULL;

	if ((he = gethostbyname(http->host)) == NULL)
	{
		troll_debug(LOG_WARN,"Could not resolve host (%s) for HTTP listening",http->host);
		http->sock = -2;
		return;
	}

	httphostip = tmalloc0(3*4+3+1);
	sprintf(httphostip,"%s",inet_ntoa(*((struct in_addr *)he->h_addr_list[0])));

	if ((http->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		troll_debug(LOG_ERROR,"Could not create socket for DCC listener");
		http->sock = -2;
		return;
	}

	if (setsockopt(http->sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
	{
		troll_debug(LOG_ERROR,"Could not set socket options");
		http->sock = -2;
		return;
	}

	httpaddr.sin_family = AF_INET;

	httpaddr.sin_addr.s_addr = inet_addr(httphostip);

	if (http->port == -1)
	{
		http->port = 84191;
	}

  httpaddr.sin_port = htons(http->port);

  memset(&(httpaddr.sin_zero), '\0', 8);

  if (bind(http->sock, (struct sockaddr *)&httpaddr, sizeof(httpaddr)) == -1)
  {
    troll_debug(LOG_ERROR,"Could not bind to HTTP socket");
    free(httphostip);
		http->sock = -2;
    return;
  }

  if (listen(http->sock, HTTP_MAX) == -1)
  {
    troll_debug(LOG_ERROR,"Could not listen on HTTP socket");
    free(httphostip);
		http->sock = -2;
    return;
  }

  troll_debug(LOG_DEBUG,"Listening on %s port %d\n",httphostip,http->port);
  free(httphostip);

  return;
}


/* This creates a new connection from a listening socket */
void new_http_connection(struct http_server *http)
{
	struct http_request *req;
	struct sockaddr_in client_addr;
	socklen_t sin_size = 0;
	int sock = 0;

	if ((sock = accept(http->sock,(struct sockaddr *)&client_addr,&sin_size)) == -1)
	{
		return;
	}

	sin_size = sizeof(struct sockaddr_in);

	req = new_http_request();

	req->sock   = sock;
	req->status = HTTP_NOTREADY;

	req->server = http;

	printf("Accepted new HTTP connection\n");

	http->requests = http_request_add(http->requests, req);

	return; 
}

/* This function gets an unparsed line from ICS, and makes it into the http_data struct */
struct http_data *parse_http_line(struct http_server *http, char *buffer)
{
	struct http_data *data    = NULL;

	data = http_data_new();

	data->txt_packet = tstrdup(buffer);
	data->tokens     = tssv_split(buffer);

	troll_debug(LOG_DEBUG, "%s\n",data->txt_packet);

	return data;
/*	http_data_free(data); */
}

int http_in(struct http_server *http)
{
	static char         *buffer  = NULL;
	static size_t       size     = BUFFER_SIZE;
	int                 recved   = 0;
	char                *line    = NULL;
	const char          *ptr     = NULL;
	char                *optr    = NULL;
	char                *bufcopy = NULL;


	/* The previous line never existed, or it was completed and
	 * set to NULL upon completion.
	 */
	if (buffer == NULL)
	{
		/* Start with a new zeroed buffer */
		buffer = tmalloc0(BUFFER_SIZE + 1);
		recved = recv(http->sock,buffer,BUFFER_SIZE-1,0);
	} else {
		/* There was a fragment left over, create a larger buffer */
		buffer = tcrealloc0(buffer,
				strlen(buffer) + BUFFER_SIZE + 1,
				&size);

		recved = recv(http->sock,&buffer[strlen(buffer)],BUFFER_SIZE-1,0);

	}

	/* On Errors, or socket close */
	switch (recved)
	{
		case -1:
			free(buffer);
			buffer = NULL;
			return 1;
		case 0:
			http->sock = -1;
			free(buffer);
			buffer = NULL;
			return 0;
	}

	while (strchr(buffer,'\n') != NULL)
	{ /* Complete ICS line */
		line = tmalloc0(strlen(buffer)+1);

		optr = line;

		for(ptr = buffer;*ptr != '\n' && *ptr != '\r';ptr++)
		{
			*optr = *ptr;
			optr++;
		}

		/* This should deal with httpds which output \r only, \r\n, or \n */
		while (*ptr == '\r' || *ptr == '\n')
			ptr++;

		/* Pass the single line for more processing */
		parse_http_line(http,line);

		free(line);
		line = NULL;

		if (strlen(ptr) == 0)
		{
			free(buffer);
			buffer = NULL;
			break;
		}

		bufcopy = tstrdup(ptr);

		free(buffer);

		size   = strlen(bufcopy) + 1;

		buffer = bufcopy;
	}

	return 1;

}
