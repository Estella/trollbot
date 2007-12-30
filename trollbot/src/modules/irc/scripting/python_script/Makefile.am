moduledir = ${prefix}/modules

INCLUDES        = -I$(top_builddir) -I$(top_srcdir)         \
                  -I$(top_builddir)/src -I$(top_srcdir)/src

module_LTLIBRARIES = python_script.la

python_script_la_SOURCES = python_embed.c python_embed.h python_lib.c python_lib.h

python_script_la_LDFLAGS = -no-undefined -module -avoid-version ${PYTHON_LDADD}
python_script_la_CFLAGS = ${PYTHON_CFLAGS}
