bin_PROGRAMS = trollbot
trollbot_SOURCES = main.c tconfig.c network.c server.c config_engine.c sockets.c irc.c util.c debug.c channel.c trigger.c sha1.c \
		   user.c trig_table.c

trollbot_CFLAGS = -O0 -Wall
trollbot_LDADD  = 

if WITH_TCL

trollbot_SOURCES += tcl_embed.c tcl_lib.c
trollbot_CFLAGS  += ${TCL_CPPFLAGS}
trollbot_LDADD   += ${TCL_LIBS}

endif
