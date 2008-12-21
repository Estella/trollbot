#ifndef __ICS_SERVER_H__
#define __ICS_SERVER_H__

#include "config.h"

struct server;
struct tconfig_block;
struct ics_game;
struct tsocket;

#include <time.h>

enum ics_status {
  ICS_DISCONNECTED = 0,
	ICS_INPROGRESS,
  ICS_NONBLOCKCONNECT, /* A connect() call has been made, it is not in any fd set */
  ICS_WAITINGCONNECT,  /* A connect() call has been made, it's now in a fd set    */
  ICS_NOTREADY,        /* Socket has been accept()ed but not added to FD set      */
  ICS_CONNECTED,
	ICS_AUTHORIZED,
	ICS_IDLE
};


/* All lists are at head */
struct ics_server
{
  char *label;

	/* WTF is this */
	/* I'm using some of the same stuff from the irc code
   * it should all be portable, or made to be portable
   * Interfaces need to be separated.
   */
	/* Copy over on rehash */
  struct server *cur_server;

	/* This should be used for inner "mirror" sites */
  struct server *servers;

	char *username;
	char *password;

	/* Copy over on rehash */
  struct tsocket *tsock;

	struct ics_trigger_table *ics_trigger_table;

  char *vhost;
  char *shost; /* Server given host */

	/* Copy over on rehash */
  int status;

	/* This is what FICS recognizes me as */
	char *my_name; 

	/* This settings makes the bot cycle forever through the server list until
 	 * it successfully connects to one.
 	 */ 
	int never_give_up;

	/* Stupid thing I use for the on connect trigger */
	int connected;

  /* Time in seconds to wait before trying to reconnect */
  int connect_delay;

  /* if (connect_try--) if (last_try + connect_delay <= time(NULL)) connect() */
  time_t last_try;

	struct t_timer *timers;

  /* Unhandled blocks go here */
  struct tconfig_block *tcfg;

	struct ics_game *game;

  struct ics_server *prev;
  struct ics_server *next;
};


struct ics_server *ics_server_from_tconfig_block(struct tconfig_block *tcfg);

struct ics_server *ics_server_add(struct ics_server *servers, struct ics_server *add);
struct ics_server *ics_server_del(struct ics_server *servers, struct ics_server *del);

void ics_server_connect(struct ics_server *ics, struct tsocket *tsock);
void free_ics_servers(struct ics_server *ics_servers);
void free_ics_server(void *ics_ptr);

struct ics_server *new_ics_server(char *label);

#endif /* __ICS_SERVER_H__ */
