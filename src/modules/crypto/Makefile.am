libdir = ${prefix}/modules

INCLUDES        = -I$(top_builddir) -I$(top_srcdir)         \
                  -I$(top_builddir)/src -I$(top_srcdir)/src

lib_LTLIBRARIES = crypto.la

crypto_la_SOURCES = sha512.c tomcrypt_argchk.h tomcrypt_cfg.h \
										tomcrypt_cipher.h tomcrypt_custom.h tomcrypt.h \
										tomcrypt_hash.h tomcrypt_mac.h tomcrypt_macros.h \
										tomcrypt_math.h tomcrypt_misc.h tomcrypt_pkcs.h tomcrypt_pk.h \
										tomcrypt_prng.h crypto.c util.c util.h blowfish.c md5.c

crypto_la_LDFLAGS = -module -no-undefined -avoid-version
