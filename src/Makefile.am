SUBDIRS = modules

bindir = ${prefix}

bin_PROGRAMS = trollbot

trollbot_SOURCES = channel.c config_engine.c dcc.c debug.c irc_proto.c die.c egg_lib.c irc.c log_entry.c log_filter.c main.c network.c server.c sockets.c tconfig.c t_crypto_module.c irc_trigger.c user.c util.c channel.h config_engine.h dcc.h debug.h irc_proto.h die.h egg_lib.h irc.h log_entry.h log_filter.h main.h network.h server.h sockets.h tconfig.h t_crypto_module.h irc_trigger.h user.h util.h t_timer.c t_timer.h troll_lib.c troll_lib.h tsocket.c tsocket.h tmodule.h tmodule.c

trollbot_CXXFLAGS = -O0 -g
trollbot_CFLAGS = -O0 -Wall -std=c99 -pedantic ${LTDLINCL}

trollbot_LDADD  = ${LIBLTDL} "-dlopen" modules/crypto/crypto.la "-dlopen" modules/tmod_ics/tmod_ics.la

trollbot_DEPENDENCIES = modules/crypto/crypto.la modules/tmod_ics/tmod_ics.la

if WITH_PGSQL
trollbot_LDADD += "-dlopen" modules/pgsql_log_filter/pgsql_log_filter.la
trollbot_DEPENDENCIES += modules/pgsql_log_filter/pgsql_log_filter.la
endif

if WITH_HTTP
trollbot_SOURCES += http_server.c http_server.h http_proto.c http_proto.h http_request.c http_request.h
endif

if WITH_ICS
trollbot_LDADD += "-dlopen" modules/tmod_ics/tmod_ics.la
trollbot_DEPENDENCIES += modules/tmod_ics/tmod_ics.la
endif

if WITH_XMPP
trollbot_SOURCES += xmpp_server.c xmpp_server.h xmpp_proto.c xmpp_proto.h xmpp_trigger.c xmpp_trigger.h
trollbot_CFLAGS  += ${XML_CPPFLAGS}
trollbot_LDADD   += ${XML_LIBS}
endif

if WITH_JS
trollbot_SOURCES += js_embed.c js_embed.h js_lib.c js_lib.h
trollbot_CFLAGS  += ${JS_CFLAGS}
trollbot_LDADD   += ${JS_LIBS}
endif


if WITH_TCL
trollbot_SOURCES += tcl_embed.c tcl_embed.h tcl_lib.c tcl_lib.h
trollbot_CFLAGS  += ${TCL_CPPFLAGS}
trollbot_LDADD   += ${TCL_LIBS}
endif

if WITH_PHP
trollbot_SOURCES += apachecompat.c apachecompat.h php_embed.c php_embed.h php_lib.c php_lib.h
trollbot_CFLAGS  += ${PHP_CFLAGS}
trollbot_LDADD   += ${PHP_LDADD}
endif

if WITH_PERL
trollbot_SOURCES += perl_embed.c perl_embed.h perl_lib.c perl_lib.h
trollbot_CFLAGS  += ${PERL_CFLAGS}
trollbot_LDADD   += ${PERL_LDADD}
endif

if WITH_PYTHON
trollbot_SOURCES += python_embed.c python_embed.h python_lib.c python_lib.h
trollbot_CFLAGS  += ${PYTHON_CFLAGS}
trollbot_LDADD   += ${PYTHON_LDADD}
endif
