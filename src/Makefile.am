bin_PROGRAMS = trollbot
trollbot_SOURCES = main.c tconfig.c network.c server.c config_engine.c sockets.c irc.c util.c debug.c channel.c trigger.c sha1.c \
		   user.c trig_table.c

trollbot_CFLAGS  = ${TCL_CFLAGS} ${PHP_CFLAGS} -O0 -pedantic -Wall

trollbot_LDADD   = ${TCL_LDFLAGS} ${PHP_LDADD}
