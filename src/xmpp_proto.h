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
#ifndef __XMPP_PROTO_H__
#define __XMPP_PROTO_H__

#include <libxml/parser.h>
#include <libxml/tree.h>

/*
      jid             = [ node "@" ] domain [ "/" resource ]
      domain          = fqdn / address-literal
      fqdn            = (sub-domain 1*("." sub-domain))
      sub-domain      = (internationalized domain label)
      address-literal = IPv4address / IPv6address
*/
struct tjid
{
	/* Spec says each of these cannot be over 1023 bytes in length */
	/* Maximum size all together is 3071 bytes */
	/* Some are derived values */
	char *jid;
	char *domain; /* Required */ /* Can be IP, should be FQDN */
	char *fqdn;
	char *sub_domain;
	char *address_literal;
	char *node; /* secondary identifier placed before the domain identifier,
							 * separated by the '@' character
               */
	/*
	 * tertiary identifier placed after the domain identifier and separated
	 * from the latter by the '/' character
   */
	char *resource;
};

struct xmpp_data
{
	xmlDocPtr xml_packet;

	char *txt_packet;
};

void xmpp_ball_start_rolling(struct xmpp_server *xs);
void xmpp_xml_send(struct xmpp_server *xs, xmlDocPtr doc);
void xmpp_printf(struct xmpp_server *xs, const char *fmt, ...);
struct xmpp_data *xmpp_data_new(void);
void xmpp_data_free(struct xmpp_data *data);
void parse_xmpp_line(struct xmpp_server *xs, const char *buffer);
int xmpp_in(struct xmpp_server *xs);

#endif /* __IRC_H__ */
