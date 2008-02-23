#ifndef __MAIN_H__
#define __MAIN_H__

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "debug.h"
#include "util.h"
#include "tconfig.h"
#include "config_engine.h"
#include "die.h"

#define BUFFER_SIZE 1024

extern struct config *g_cfg;

#endif /* __MAIN_H__ */
