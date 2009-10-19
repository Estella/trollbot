/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "trollbot.h"
#include "xmpp_proto.h"
#include "xmpp_server.h"
#include "xmpp_trigger.h"
#include "log_entry.h"

void xmpp_ball_start_rolling(struct xmpp_server *xs)
{
	/* Fuck it */
	xmpp_printf(xs,"<stream:stream xmlns:stream=\"http://etherx.jabber.org/streams\" xmlns=\"jabber:client\" to=\"%s\" version=\"1.0\">\n", xs->label);
#ifdef CLOWNS
	xmlDocPtr  doc       = NULL;/* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node      = NULL;
	xmlNodePtr node1     = NULL;/* node pointers */
	xmlNsPtr   ns_stream = NULL;
	xmlNsPtr   ns_jabber = NULL;

	/* Well commented because libxml2 is a mindfuck */

	/* Starts the document with xml 1.0 */
	doc = xmlNewDoc(BAD_CAST "1.0");

	/* Create the root node */
	root_node = xmlNewNode(NULL, BAD_CAST "stream");

	/* Create the stream namespace */
	ns_stream = xmlNewNs(root_node,
			/* Hardcode OK */
			BAD_CAST "http://etherx.jabber.org/streams",
			/* Hardcode OK */
			BAD_CAST "stream");

	/* Create the jabber namespace */
	ns_jabber = xmlNewNs(root_node,
			/* Hardcode OK */
			BAD_CAST "jabber:client",
			NULL);

	/* Make the node I just created the first node in the document */
	xmlDocSetRootElement(doc, root_node);

	/* Name of the server, should probably go off sslserver or server tag in conf */
	xmlNewProp(root_node, BAD_CAST "to", BAD_CAST xs->label);

	/* I need to meet the requirements of RFC 3920 for this */
	xmlNewProp(root_node, BAD_CAST "version", BAD_CAST "1.0");

	/* Sends XML Packet */
	xmpp_xml_send(xs, doc);

	/* We don't need it anymore */
	xmlFreeDoc(doc);

	return;
#endif /* CLOWNS */
}

/* This prints an XML document to a XMPP server */
void xmpp_xml_send(struct xmpp_server *xs, xmlDocPtr doc)
{
	xmlChar    *xmlbuf;
	int        buffer_size;

	xmlDocDumpFormatMemory(doc, &xmlbuf, &buffer_size, 0);

	printf("Sending XML Packet: %s\n",xmlbuf);
	send(xs->sock,xmlbuf,buffer_size, 0);
	send(xs->sock,"\n",1, 0);

	xmlFree(xmlbuf);
	return;
}

/* This is different from irc_printf() (should be sock_printf())
 * in the way that it has an extra level of abstraction in the
 * case of SSL sockets.
 */
void xmpp_printf(struct xmpp_server *xs, const char *fmt, ...)
{
	va_list va;
	char buf[2048];
	char buf2[2059];

	memset(buf, 0, sizeof(buf));
	memset(buf2, 0, sizeof(buf2));

	va_start(va, fmt);

	/* C99 */
	vsnprintf(buf, sizeof(buf), fmt, va);
	va_end(va);

	snprintf(buf2,sizeof(buf2),"%s\n",buf);

	printf("Sent: %s\n",buf2);
	send(xs->sock,buf2,strlen(buf2),0);
}

/* Constructor */
struct xmpp_data *xmpp_data_new(void)
{
	struct xmpp_data *local;

	local = tmalloc(sizeof(struct xmpp_data));

	local->txt_packet = NULL;
	local->xml_packet = NULL;

	return local;
}

/* Destructor */
void xmpp_data_free(struct xmpp_data *data)
{
	xmlFreeDoc(data->xml_packet);

	free(data->txt_packet);

	free(data);
}

/* This function gets an unparsed line from XMPP, and makes it into the xmpp_data struct */
void parse_xmpp_line(struct xmpp_server *xs, const char *buffer)
{
	struct xmpp_data *data    = NULL;
	struct auth_type *at      = NULL;
	xmlNodePtr        node    = NULL;
	xmlNodePtr        node2   = NULL;
	xmlNodePtr        splnker = NULL;

	data = xmpp_data_new();

	printf("Incoming Packet: %s\n",buffer);
	data->txt_packet = tstrdup(buffer);

	data->xml_packet = xmlReadMemory(buffer, strlen(buffer), "noname.xml", NULL, 0);

	if (data->xml_packet == NULL)
	{
		log_entry_printf(NULL,NULL,"X","Failed to parse XML packet from XMPP");
		return;
	}

	/* Parse out the packet a bit for the one time shit */
	node = xmlDocGetRootElement(data->xml_packet);

	/* Go to the child node pointer */
	if ((node = node->xmlChildrenNode) == NULL)
	{
		log_entry_printf(NULL,NULL,"X","Root element of packet had no children");
		xmpp_data_free(data);
		return;
	}

	/* For all the ugly one time stuff that I'm not going to create a bind type for */
	if (!xmlStrcmp(node->name, (const xmlChar *)"features"))
	{
		/*  features
		 *    starttls/
		 *    mechanisms
		 *      mechanism - DIGEST-MD5
		 *      mechanism - PLAIN
		 *    /mechanisms
		 *  /features
		 */

		if ((node2 = node->xmlChildrenNode) == NULL)
		{
			log_entry_printf(NULL,NULL,"X","features node of XMPP packet had NULL children");
			xmpp_data_free(data);
			return;
		}

		while (node2 != NULL)
		{
			if (!xmlStrcmp(node2->name, (const xmlChar *)"starttls"))
			{
				log_entry_printf(NULL,NULL,"X","Server claims: TLS Supported");
			}
			else if (!xmlStrcmp(node2->name, (const xmlChar *)"register"))
			{
				log_entry_printf(NULL,NULL,"X","Server is requiring us to register");
			}
			else if (!xmlStrcmp(node2->name,(const xmlChar *)"mechanisms"))
			{
				if ((splnker = node2->xmlChildrenNode) == NULL)
				{
					log_entry_printf(NULL,NULL,"X","mechanisms of XMPP packet had NULL children");
					xmpp_data_free(data);
					return;
				}

				while (splnker != NULL)
				{
					if (!xmlStrcmp(splnker->name,(const xmlChar *)"mechanism"))
					{
						at       = new_auth_type();
						at->name = xmlNodeListGetString(data->xml_packet, splnker->xmlChildrenNode, 1);

						log_entry_printf(NULL,NULL,"X","Server accepts algorithm type: %s",at->name);
						/* Add it to the server's list */
						xs->auth_types_remote = auth_type_add(xs->auth_types_remote, at);

						/* Try to get the local algorithm name */
						at->algo_name = auth_type_get_algo_info(at->name);
					}

					splnker = splnker->next;
				}

				log_entry_printf(NULL,NULL,"X","Server Mechanisms complete");
			}

			node2 = node2->next;
		}

		xmpp_printf(xs, "<starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>");
	}

	xmpp_trigger_match(xs, data);

	xmpp_data_free(data);
}

/* Needs to be adapted for the possibility of SSL */
/* This is broken
 * Problem: no newlines, or EOL indicator
 * Solutions: Wait until it validates?
 * will work on anything but the beginning and end packet
 * which contain <stream:stream> and </stream:stream>
 * could skip first packet but it won't guarantee that
 * it will get everything in the first packet.
 * different status perhaps.
 */
int xmpp_in(struct xmpp_server *xs)
{

	/* This method is deprecated for XMPP */
	/* Big Ol FIXME */
	return 0;
#ifdef CLOWNS
	static char         *buffer  = NULL;
	static size_t       size     = BUFFER_SIZE;
	int                 recved   = 0;
	char                *line    = NULL;
	const char          *ptr     = NULL;
	char                *optr    = NULL;
	char                *bufcopy = NULL;


	if (buffer == NULL)
	{
		buffer = tmalloc0(BUFFER_SIZE + 1);
		recved = recv(xs->sock,buffer,BUFFER_SIZE-1,0);
	} else {
		/* There was a fragment left over */
		buffer = tcrealloc0(buffer,
				strlen(buffer) + BUFFER_SIZE + 1,
				&size);

		recved = recv(xs->sock,&buffer[strlen(buffer)],BUFFER_SIZE-1,0);

	}


	switch (recved)
	{
		case -1:
			free(buffer);
			buffer = NULL;
			return 1;
		case 0:
			/* Try parsing what's left of the buffer if it has any contents *NON-VALIDATED* */
			if (buffer != NULL)
				parse_xmpp_line(xs,buffer);

			xs->sock = -1;
			free(buffer);
			buffer = NULL;
			return 0;
	}

	/* Check if validates here */
	line = tmalloc0(strlen(buffer)+1);

	optr = line;

	for(ptr = buffer;*ptr != '\0';ptr++)
	{
		*optr = *ptr;
		optr++;
	}

	/* IF VALIDATES */
	if (xs->status == XMPP_CONNECTED)
	{

		parse_xmpp_line(xs,line);
	}

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

	return 1;
#endif /* CLOWNS */
}
