SUBDIRS = modules

bindir = ${prefix}

bin_PROGRAMS = trollbot

trollbot_SOURCES = config_engine.c debug.c die.c log_entry.c log_filter.c main.c server.c sockets.c tconfig.c t_crypto_module.c util.c config_engine.h debug.h die.h log_entry.h log_filter.h main.h server.h sockets.h tconfig.h t_crypto_module.h util.h t_timer.c t_timer.h tsocket.c tsocket.h tmodule.h tmodule.c tmessage.c tmessage.h

trollbot_CXXFLAGS = -O0 -g
trollbot_CFLAGS = -O0 -Wall -std=c99 -pedantic ${LTDLINCL}

trollbot_LDADD  = ${LIBLTDL} "-dlopen" modules/crypto/crypto.la

trollbot_DEPENDENCIES = modules/crypto/crypto.la

if WITH_PGSQL
trollbot_LDADD += "-dlopen" modules/pgsql_log_filter/pgsql_log_filter.la
trollbot_DEPENDENCIES += modules/pgsql_log_filter/pgsql_log_filter.la
endif

if WITH_HTTP
trollbot_SOURCES += http_server.c http_server.h http_proto.c http_proto.h http_request.c http_request.h
endif

if WITH_ICS
trollbot_LDADD += "-dlopen" modules/ics/ics.la
trollbot_DEPENDENCIES += modules/ics/ics.la
endif

if WITH_IRC
trollbot_LDADD += "-dlopen" modules/irc/irc.la
trollbot_DEPENDENCIES += modules/irc/irc.la
endif

if WITH_JS
trollbot_LDADD += "-dlopen" modules/js/js.la
trollbot_DEPENDENCIES += modules/js/js.la
endif

