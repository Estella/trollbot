libdir = ${prefix}/modules

INCLUDES        = -I$(top_builddir) -I$(top_srcdir)         \
                  -I$(top_builddir)/src -I$(top_srcdir)/src

lib_LTLIBRARIES = pgsql_log_filter.la

pgsql_log_filter_la_SOURCES = pgsql_log_filter.c pgsql_log_filter.h

pgsql_log_filter_la_LDFLAGS = -module -no-undefined -avoid-version
