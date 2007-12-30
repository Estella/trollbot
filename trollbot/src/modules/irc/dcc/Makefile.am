moduledir = ${prefix}/modules

INCLUDES        = -I$(top_builddir) -I$(top_srcdir)         \
                  -I$(top_builddir)/src -I$(top_srcdir)/src

module_LTLIBRARIES = dcc.la

dcc_la_SOURCES = dcc.c dcc.h
dcc_la_LDFLAGS = -no-undefined -module -avoid-version
