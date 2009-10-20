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
#ifndef __XMPP_TRIGGER_H__
#define __XMPP_TRIGGER_H__

struct xmpp_server;
struct xmpp_data;

enum xmpp_trigger_types
{
	XMPP_TRIG_MSG,
	XMPP_TRIG_ERROR
};

struct xmpp_trigger_table
{
  /* All point to head */
	struct xmpp_trigger *msg;
	struct xmpp_trigger *error;
};

struct xmpp_trigger
{
  int type;
 
  /* What triggers are matched against */
  char *mask;  

  void (*handler)(struct xmpp_server *, struct xmpp_trigger *, struct xmpp_data *);

  char *command;

  int usecount;

  struct xmpp_trigger *prev;
  struct xmpp_trigger *next;  
};

/* This really needs wrapped in a generic interface, how about
 * the slist/dlist shit in util.c?
 */
struct xmpp_trigger *xmpp_trigger_add(struct xmpp_trigger *xmpp_triggers, struct xmpp_trigger *add);
struct xmpp_trigger *xmpp_trigger_del(struct xmpp_trigger *xmpp_triggers, struct xmpp_trigger *del);

struct xmpp_trigger *new_xmpp_trigger(void);

/* Returns: number of triggers matched */
int xmpp_trigger_match(struct xmpp_server *xs, struct xmpp_data *data);

struct xmpp_trigger_table *new_xmpp_trigger_table(void);

void free_xmpp_triggers(struct xmpp_trigger *xmpp_triggers);
void free_xmpp_trigger(struct xmpp_trigger *xmpp_trigger);

void free_xmpp_trigger_table(struct xmpp_trigger_table *xtt);

#endif /* __XMPP_TRIGGER_H__ */
