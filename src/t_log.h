#ifndef __T_TIMERS__
#define __T_TIMERS__

#include <time.h>

struct network;

struct log_filter
{
	struct network *net;

	char *flags;

	void (*handler)(struct network *, struct log_filter *);

	struct log_filter *prev;
	struct log_filter *next;
};

void t_timers_check(struct t_timer *timers, time_t timestamp);
struct t_timer *t_timer_add(struct t_timer *timers, struct t_timer *add);
struct t_timer *t_timer_remove(struct t_timer *timers, struct t_timer *del);
void t_timers_free(struct t_timer *timers);
void t_timer_free(struct t_timer *timer);
struct t_timer *t_timer_new(void);

#endif /* __T_TIMERS__ */
