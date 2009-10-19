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
#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <time.h>

enum http_request_status {
  HTTP_DISCONNECTED = 0,
	HTTP_INPROGRESS,
  HTTP_NONBLOCKCONNECT, /* An accept() call has been made, it is not in any fd set */
  HTTP_WAITINGCONNECT,  /* An accept() call has been made, it's now in a fd set    */
  HTTP_NOTREADY,        /* Socket has been accept()ed but not added to FD set      */
  HTTP_CONNECTED,
	HTTP_AUTHORIZED,
	HTTP_IDLE
};


struct http_server;

struct http_request
{
	int sock;
		
	int status;

	char *host;
	int port;
	
	time_t request_time;

	char *return_buffer;

	char **headers;

	char **post;
	char **get;

	char *response;

	struct http_server *server;

	struct http_request *prev;
	struct http_request *next;
};

struct http_request *http_request_add(struct http_request *http_request, struct http_request *add);
struct http_request *http_request_del(struct http_request *http_request, struct http_request *del);


struct http_request *new_http_request(void);

#endif /* __HTTP_REQUEST_H__ */
