#ifndef __TSOCKET_H__
#define __TSOCKET_H__

#include <ltdl.h>

#include "util.h"

enum tsocket_status
{
  TSOCK_IGNORE = 0,    /* Nothing happens with this sock    */
	TSOCK_DISCONNECTED,  /* This socket was just disconnected */
	TSOCK_UNINITIALIZED, /* This socket has not been setup    */
	TSOCK_NOTINFDSET,    /* This socket is not in a FD SET    */
	TSOCK_INFDSET,       /* The socket is now in a FD SET     */
	TSOCK_LISTENER,      /* The socket is now listening       */
  TSOCK_CONNECTING     /* The socket is now connecting      */
};

struct tsocket
{
  char *name;
 
	int sock;

  int status;

	/* Extra data */
	void *data;

	/* Callback API */
	int (*tsocket_read_cb)(struct tsocket *);
	int (*tsocket_write_cb)(struct tsocket *);
	int (*tsocket_connect_cb)(struct tsocket *);
	int (*tsocket_disconnect_cb)(struct tsocket *);

	/* Connection queuing */
	time_t last_attempt;
	int    retry_limit;
	int    retry_amount;
	int    retry_wait;

};

/* TCP API */
ssize_t tsocket_printf(struct tsocket *tsock, const char *fmt, ...);
int tsocket_check_nonblocking_connect(struct tsocket *tsock);
int tsocket_close(struct tsocket *tsock);
int tsocket_listen(struct tsocket *tsock, const char *hostname, int port);
int tsocket_connect(struct tsocket *tsock, const char *from_hostname, const char *to_hostname, int port);

struct tsocket *tsocket_new(void);
void tsockets_free(struct slist *tsockets);
void tsocket_free(void *tsockets);  

#endif /* __TSOCKET_H__ */
