bin_PROGRAMS = trollbot
trollbot_SOURCES = main.c tconfig.c network.c server.c config_engine.c sockets.c irc.c util.c debug.c channel.c

trollbot_CFLAGS  = ${TCL_CFLAGS} ${PHP_CFLAGS} -O0 -pedantic -std=c89 -Wall

trollbot_LDADD   = ${TCL_LDFLAGS} ${PHP_LDADD}
