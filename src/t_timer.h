#ifndef __T_TIMERS__
#define __T_TIMERS__

#include <time.h>

struct network;

struct t_timer
{
	/* Unique identification that needs rolled over 
 	 * when exhausted 
 	 */
	int timer_id;

	/* When was this timer set */
	time_t time_set;
	/* When will the timer trigger */
	time_t time_trig;

	char *command;

	void *extra;

	struct network *net;

	void (*handler)(struct network *, struct t_timer *);

	struct t_timer *prev;
	struct t_timer *next;
};

struct t_timer *t_timers_check(struct t_timer *timers, time_t timestamp);
struct t_timer *t_timer_add(struct t_timer *timers, struct t_timer *add);
struct t_timer *t_timer_remove(struct t_timer *timers, struct t_timer *del);
void t_timers_free(struct t_timer *timers);
void t_timer_free(struct t_timer *timer);
struct t_timer *t_timer_new(void);

#endif /* __T_TIMERS__ */
