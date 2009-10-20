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
#ifndef __TRIGGER_H__
#define __TRIGGER_H__

struct network;
struct irc_data;

/* HANDLER IMP IN TCL:
                case TRIG_PUB:
                case TRIG_PUBM:
                case TRIG_MSG:
                case TRIG_MSGM:
                case TRIG_TOPC:
                case TRIG_RAW:
                case TRIG_JOIN:
                case TRIG_PART:
*/
enum
{
	TRIG_PUB,
	TRIG_PUBM, 
	TRIG_MSG,  
	TRIG_MSGM, 
	TRIG_PART,
	TRIG_NOTC,
	TRIG_JOIN,
	TRIG_SIGN,
	TRIG_CTCP,
	TRIG_KICK,
	TRIG_DCC,
	TRIG_TOPC,
	TRIG_RAW
};

struct trig_table
{
	/* All point to head */
	struct trigger *pub;
	struct trigger *pubm;
	struct trigger *msg;
	struct trigger *msgm;
	struct trigger *part;
	struct trigger *join;
	struct trigger *sign;
	struct trigger *ctcp;
	struct trigger *kick;
	struct trigger *notc;
	struct trigger *dcc;
	struct trigger *raw;
	struct trigger *topc;
};

struct trigger
{
	char *glob_flags;
	char *chan_flags;

	int type;

	/* What triggers are matched against */
	char *mask;  

	void (*handler)(struct network *, struct trigger *, struct irc_data *, struct dcc_session *, const char *);

	char *command;

	int usecount;

	struct trigger *prev;
	struct trigger *next;  
};

struct trigger *trigger_list_del(struct trigger *triggers, struct trigger *del);
void trigger_list_add(struct trigger **orig, struct trigger *new);
struct trigger *new_trigger(char *flags, int type, char *mask, char *command, void (*handler)(struct network *, struct trigger *, struct irc_data *, struct dcc_session *, const char *));
void trigger_match(struct network *net, struct irc_data *data);
struct trig_table *new_trig_table(void);
void free_trigger(struct trigger *trig);
void free_trigger_table(struct trig_table *table);
void free_trigger_list(struct trigger *list);

#endif /* __TRIGGER_H__ */
