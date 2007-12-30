libdir = ${prefix}/modules

INCLUDES        = -I$(top_builddir) -I$(top_srcdir)         \
                  -I$(top_builddir)/src -I$(top_srcdir)/src

lib_LTLIBRARIES = foo.la

foo_la_SOURCES = foo.c util.c util.h debug.c debug.h

foo_la_LDFLAGS = -module -no-undefined -avoid-version

