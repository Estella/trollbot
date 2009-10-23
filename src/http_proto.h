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
struct http_data *parse_http_line(struct http_server *http, char *buffer);
int http_in(struct http_server *http);

#endif /* __HTTP_PROTO_H__ */
