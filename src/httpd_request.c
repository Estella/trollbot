#include <stdio.h>
#include <time.h>

#include "util.h"

#include "httpd_server.h"
#include "httpd_request.h"



void new_httpd_request(struct httpd_server *httpd)
{
	struct httpd_request *req;

	req = tmalloc(sizeof(httpd_request));

	req->sock = -1;
		
	req->host = NULL;
	req->port = 1;
	
	req->request_time = -1;

	req->return_buffer = NULL;

	req->headers = NULL;

	req->post = NULL;
	req->get = NULL;

	req->httpd = NULL;

	req->prev = NULL;
	req->next = NULL;

	return req;
}

