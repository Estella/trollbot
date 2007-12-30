SUBDIRS = modules

bindir = ${prefix}

bin_PROGRAMS = trollbot

trollbot_SOURCES = debug.c debug.h loop.c loop.h tconfig.c tconfig.h tmodule.c tmodule.h trollbot.c trollbot.h util.c util.h

trollbot_LDADD = $(LIBLTDL) "-dlopen" modules/foo/foo.la "-dlopen" modules/irc/irc.la

trollbot_DEPENDENCIES = modules/foo/foo.la modules/irc/irc.la

trollbot_INCLUDES = $(INCLTDL)
