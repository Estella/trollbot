#ifndef __ICS_TRIGGER_H__
#define __ICS_TRIGGER_H__

struct ics_server;
struct ics_data;

enum ics_trigger_types
{
	ICS_TRIG_MSG,
	ICS_TRIG_NOTIFY,
	ICS_TRIG_CONNECT,
	ICS_TRIG_GAME,
	ICS_TRIG_MOVE,
	ICS_TRIG_TELL,
	ICS_TRIG_ENDGAME,
	ICS_TRIG_ERROR
};

struct ics_trigger_table
{
  /* All point to head */
	struct ics_trigger *msg;
	struct ics_trigger *notify;
	struct ics_trigger *error;
	struct ics_trigger *connect;
	struct ics_trigger *game;
	struct ics_trigger *move;
	struct ics_trigger *endgame;
	struct ics_trigger *tell;
};

struct ics_trigger
{
  int type;
 
  /* What triggers are matched against */
  char *mask;  

  void (*handler)(struct ics_server *, struct ics_trigger *, struct ics_data *);

  char *command;

  int usecount;

  struct ics_trigger *prev;
  struct ics_trigger *next;  
};

/* This really needs wrapped in a generic interface, how about
 * the slist/dlist shit in util.c?
 */
struct ics_trigger *ics_trigger_add(struct ics_trigger *ics_triggers, struct ics_trigger *add);
struct ics_trigger *ics_trigger_del(struct ics_trigger *ics_triggers, struct ics_trigger *del);

struct ics_trigger *new_ics_trigger(void);

/* Returns: number of triggers matched */
int ics_trigger_match(struct ics_server *xs, struct ics_data *data);

struct ics_trigger_table *new_ics_trigger_table(void);

void free_ics_triggers(struct ics_trigger *ics_triggers);
void free_ics_trigger(struct ics_trigger *ics_trigger);

void free_ics_trigger_table(struct ics_trigger_table *xtt);

#endif /* __ICS_TRIGGER_H__ */
