moduledir = ${prefix}/modules

INCLUDES        = -I$(top_builddir) -I$(top_srcdir)         \
                  -I$(top_builddir)/src -I$(top_srcdir)/src

module_LTLIBRARIES = php_script.la

php_script_la_SOURCES = apachecompat.c apachecompat.h php_embed.c php_embed.h php_lib.c php_lib.h

php_script_la_LDFLAGS = -no-undefined -module -avoid-version ${PHP_LDADD}
php_script_la_CFLAGS = ${PHP_CFLAGS}
