moduledir = ${prefix}/modules

INCLUDES        = -I$(top_builddir) -I$(top_srcdir)         \
                  -I$(top_builddir)/src -I$(top_srcdir)/src

module_LTLIBRARIES = perl_script.la

perl_script_la_SOURCES = perl_embed.c perl_embed.h perl_lib.c perl_lib.h

perl_script_la_LDFLAGS = -no-undefined -module -avoid-version ${PERL_LDADD}
perl_script_la_CFLAGS = ${PERL_CFLAGS}
