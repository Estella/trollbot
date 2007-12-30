moduledir = ${prefix}/modules

INCLUDES        = -I$(top_builddir) -I$(top_srcdir)         \
                  -I$(top_builddir)/src -I$(top_srcdir)/src

module_LTLIBRARIES = crypto.la

crypto_la_SOURCES = tcrypto.c

crypto_la_LDFLAGS = -module
crypto_la_CFLAGS  = -Wall -Wsign-compare -W -Wshadow -Wno-unused-parameter -DLTC_SOURCE
