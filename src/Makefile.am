bin_PROGRAMS = trollbot
trollbot_SOURCES = channels.c tconfig.c dcc.c debug.c die.c handlers.c  \
                   irc.c main.c mem.c sendq.c servers.c sockets.c       \
                   triggers.c users.c util.c default_handlers.c         \
                   tcl_lib.c tcl.c scripts.c php_embed.c sha1.c 

trollbot_CFLAGS  = ${TCL_CFLAGS} ${PHP_CFLAGS} -O0

trollbot_LDADD   = ${TCL_LDFLAGS} ${PHP_LDADD}
