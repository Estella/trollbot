libdir = ${prefix}/modules

INCLUDES        = -I$(top_builddir) -I$(top_srcdir)         \
                  -I$(top_builddir)/src -I$(top_srcdir)/src

lib_LTLIBRARIES = tmod_ics.la

tmod_ics_la_SOURCES = ics_game.c ics_game.h ics_lib.c ics_lib.h ics_proto.c ics_proto.h ics_server.c ics_server.h ics_trigger.c ics_trigger.h

tmod_ics_la_LDFLAGS = -module -no-undefined -avoid-version
