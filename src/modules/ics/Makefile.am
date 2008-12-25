libdir = ${prefix}/modules

INCLUDES        = -I$(top_builddir) -I$(top_srcdir)         \
                  -I$(top_builddir)/src -I$(top_srcdir)/src

lib_LTLIBRARIES = ics.la

ics_la_SOURCES = ics_game.c ics_game.h ics_lib.c ics_lib.h ics_proto.c ics_proto.h ics_server.c ics_server.h ics_trigger.c ics_trigger.h debug.c debug.h tmod_ics.c tmod_ics.h ../../util.c ../../util.h ../../tsocket.c ../../tsocket.h ../../t_timer.c ../../t_timer.h ../../server.c ../../server.h ../../tmessage.c ../../tmessage.h

ics_la_CFLAGS = ${JS_CFLAGS} ${PYTHON_CFLAGS} ${TCL_CPPFLAGS} ${PHP_CFLAGS} ${LTDLINCL}
ics_la_LDFLAGS = -module -no-undefined -avoid-version ${JS_LIBS} ${PYTHON_LDADD} ${TCL_LIBS} ${PHP_LDADD} ${LIBLTDL}
