bindir = ${prefix}
bin_PROGRAMS = trollbot
trollbot_SOURCES = main.c tconfig.c network.c server.c config_engine.c sockets.c irc.c util.c debug.c channel.c trigger.c sha1.c \
		   user.c egg_lib.c die.c default_triggers.c dcc.c

trollbot_CFLAGS = -O0 -Wall -std=c99 -pedantic
trollbot_LDADD  = 

if WITH_JS

trollbot_SOURCES += js_embed.c js_lib.c
trollbot_CFLAGS  += ${JS_CFLAGS}
trollbot_LDADD   += ${JS_LDADD}

endif


if WITH_TCL

trollbot_SOURCES += tcl_embed.c tcl_lib.c
trollbot_CFLAGS  += ${TCL_CPPFLAGS}
trollbot_LDADD   += ${TCL_LIBS}

endif

if WITH_PHP

trollbot_SOURCES += apachecompat.c php_embed.c php_lib.c
trollbot_CFLAGS  += ${PHP_CFLAGS}
trollbot_LDADD   += ${PHP_LDADD}

endif

if WITH_PERL

trollbot_SOURCES += perl_embed.c perl_lib.c
trollbot_CFLAGS  += ${PERL_CFLAGS}
trollbot_LDADD   += ${PERL_LDADD}

endif

if WITH_PYTHON

trollbot_SOURCES += python_embed.c python_lib.c
trollbot_CFLAGS  += ${PYTHON_CFLAGS}
trollbot_LDADD   += ${PYTHON_LDADD}

endif
