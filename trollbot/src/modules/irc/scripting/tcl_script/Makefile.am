moduledir = ${prefix}/modules

INCLUDES        = -I$(top_builddir) -I$(top_srcdir)         \
                  -I$(top_builddir)/src -I$(top_srcdir)/src

module_LTLIBRARIES = tcl_script.la

tcl_script_la_SOURCES = tcl_embed.c tcl_embed.h tcl_lib.c tcl_lib.h

tcl_script_la_LDFLAGS = -no-undefined -module -avoid-version ${TCL_LDADD}
tcl_script_la_CFLAGS = ${TCL_CFLAGS}
