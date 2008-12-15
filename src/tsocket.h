#ifndef __TSOCKET_H__
#define __TSOCKET_H__

#include <ltdl.h>

#include "util.h"

enum tsocket_status
{
  TSOCK_IGNORE = 0,  /* Nothing happens with this sock */
	TSOCK_UNITIALIZED, /* This socket has not been setup */
	TSOCK_NOTINFDSET,  /* This socket is not in a FD SET */
	TSOCK_INFDSET,     /* The socket is now in a FD SET  */
	TSOCK_LISTENER,    /* The socket is now listening    */
  TSOCK_CONNECTING,  /* The socket is now connecting   */
  TSOCK_CONNECTED,   /* The socket is now connected    */
  TSOCK_ACTIVE       /* The socket is active for r/w   */
};

struct tsocket
{
  char *name;
 
	int sock;

	/* Callback API */
	void (*tsocket_read_cb)(struct tsocket *);
	void (*tsocket_write_cb)(struct tsocket *);

  int status;
};

/* TCP API */
int tsocket_close(struct tsocket *tsock);
int tsocket_listen(struct tsocket *tsock, const char *hostname, int port);
int tsocket_connect(struct tsocket *tsock, const char *hostname, int port);

struct tsocket *tsocket_new(void);
void tsockets_free(struct slist *tsockets);
void tsocket_free(struct tsocket *tsockets);  

#endif /* __TSOCKET_H__ */
