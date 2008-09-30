#ifndef __HTTPD_REQUEST_H__
#define __HTTPD_REQUEST_H__

#include <time.h>

struct httpd_server;

struct httpd_request
{
	int sock;
		
	char *host;
	int port;
	
	time_t request_time;

	char *return_buffer;

	char **headers;

	char **post;
	char **get;

	struct httpd_server *httpd;

	struct httpd_request *prev;
	struct httpd_request *next;
};

void new_httpd_request(struct httpd_server *httpd);

#endif /* __HTTPD_REQUEST_H__ */
