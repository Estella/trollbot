libdir = ${prefix}/modules

INCLUDES        = -I$(top_builddir) -I$(top_srcdir)         \
                  -I$(top_builddir)/src -I$(top_srcdir)/src

lib_LTLIBRARIES = irc.la

irc_la_SOURCES = tirc.c tconfig.c tconfig.h debug.c debug.h util.c util.h

irc_la_LDFLAGS = -module -no-undefined -avoid-version

