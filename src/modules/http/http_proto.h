#ifndef __HTTP_PROTO_H__
#define __HTTP_PROTO_H__


struct http_server;


struct http_data
{
	char *txt_packet;
	char **tokens;
};


void http_init_listener(struct http_server *http);
void new_http_connection(struct http_server *http);
void parse_http_line(struct http_server *http, char *buffer);
int http_in(struct http_server *http);

#endif /* __HTTP_PROTO_H__ */
