#ifndef __BAN_H__
#define __BAN_H__

#include <time.h>

struct ban
{
	char *mask;
	char *comment;
	time_t expiration;
	time_t created;
	time_t last_time;

	char *creator;
};

struct ban *ban_new(void);
void ban_free(void *data);


#endif /* __BAN_H__ */
