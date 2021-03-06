VERSION = "snap20080930"

AC_SUBST(VERSION)

dnl set the default prefix to be /home/user/trollbot

AC_INIT(src/main.c)
AM_INIT_AUTOMAKE(trollbot,snap20080930)

AC_CONFIG_HEADER(config.h)

LT_CONFIG_LTDL_DIR([libltdl])

LT_INIT
LTDL_INIT

AC_LIBTOOL_DLOPEN
AC_LIBLTDL_CONVENIENCE
AC_PROG_LIBTOOL

dnl wtf is up with that naming
AC_SUBST(LTDLINCL)
AC_SUBST(LIBLTDL)

AC_SUBST(LIBCSOAP_LDFLAGS)
AC_SUBST(LIBCSOAP_CFLAGS)
AC_SUBST(LIBCSOAP_LIBS)

AM_MAINTAINER_MODE

AC_PROG_CC
AM_PROG_CC_C_O

dnl test C Compiler
AC_LANG_C

AC_C_CONST

AC_STRUCT_TM

AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([sys/time.h])

dnl set HAVE_WORKING_FORK if system supports forking
AC_FUNC_FORK

AC_TYPE_PID_T


dnl check if libxml2 exists
dnl needed for jabber/xmpp

dnl temporarily disabled
dnl AC_ARG_WITH(http, [  --with-http=[yes/no]  Whether to include webserver support],
dnl	[ AC_DEFINE([HAVE_HTTP],1,[Whether to use http support])
dnl		with_http="yes"])

dnl temporarily disabled
dnl AC_ARG_WITH(pgsql, [  --with-pgsql=[yes/no]  Whether to include PostgreSQL support],
dnl	[ AC_DEFINE([HAVE_PGSQL],1,[Whether to use PostgreSQL support])
dnl		with_pgsql="yes"])

AC_ARG_WITH(irc, [  --with-irc=[yes/no]  Whether to include IRC support],
	[ AC_DEFINE([HAVE_IRC],1,[Whether to use IRC support])
		with_irc="yes" ])

AC_ARG_WITH(ics, [  --with-ics=[yes/no]  Whether to include ICS support],
	[ AC_DEFINE([HAVE_ICS],1,[Whether to use ICS support])
		with_ics="yes" ])

dnl temporarily disabled
dnl AC_ARG_WITH(xmpp, [  --with-xmpp=[yes/no]  Whether to include XMPP support],
dnl	[ with_xmpp="$withval" ])

dnl if test "${with_pgsql}" = "yes"; then
	dnl check if postgresql exists
dnl	POSTGRESQL_CHECKS

dnl	AC_SUBST(PQINCPATH)
dnl	AC_SUBST(PQLIB)
dnl fi

dnl if test "${with_xmpp}" = "yes"; then
dnl	AM_PATH_XML2(2.6.13)

dnl	AC_SUBST(XML_LIBS)
dnl	AC_SUBST(XML_CPPFLAGS)
dnl fi

dnl if test "x${XML_LIBS}" != "x"; then
dnl	AC_DEFINE([HAVE_LIBXML],1,[Ability to use libxml2])
dnl	AC_DEFINE([HAVE_XMPP],1,[Ability to use XMPP/Jabber])
dnlfi

dnl check if python exists
AC_PYTHON_DEVEL

AC_SUBST(PYTHON_LDADD)
AC_SUBST(PYTHON_CFLAGS)

if test "x${PYTHON_LDADD}" != "x"; then
	AC_DEFINE([HAVE_PYTHON],1,[Ability to use Python scripting])
fi

PKG_PROG_PKG_CONFIG


dnl test if javascript exists
AC_ARG_WITH(js, [  --with-js=[yes/no]  Whether to use JS scripting support],
	[ with_js="$withval" ])

if test "${with_js}" != "no"; then
	PKG_CHECK_MODULES(JS, xulrunner-js, has_js="yes", has_js="no")
fi

AC_SUBST(JS_LIBS)
AC_SUBST(JS_CFLAGS)

if test "x${JS_LIBS}" != "x"; then
	AC_DEFINE([HAVE_JS],1,[Ability to use Javascript scripting])
fi

AM_CONDITIONAL(WITH_PGSQL, test "x${PQLIB}" != "x")

AM_CONDITIONAL(WITH_JS, test "x${JS_LIBS}" != "x")

AM_CONDITIONAL(WITH_IRC, test "x${with_irc}" != "x")

AM_CONDITIONAL(WITH_ICS, test "x${with_ics}" != "x")

AM_CONDITIONAL(WITH_HTTP, test "x${with_http}" != "x")

dnl test if TCL exists
AX_PROG_TCL

dnl set the WITH_TCL for automake's use
AM_CONDITIONAL(WITH_TCL, test "x${TCL_LIBS}" != "x")

dnl test if PHP exists
AX_PROG_PHP


dnl supply automake a WITH_PHP conditional
AM_CONDITIONAL(WITH_PHP, test "x${PHP_LDADD}" != "x")

AX_PROG_PERL

AM_CONDITIONAL(WITH_PERL, test "x${PERL_LDADD}" != "x")
AM_CONDITIONAL(WITH_PYTHON, test "x${PYTHON_LDADD}" != "x")
AM_CONDITIONAL(WITH_XMPP, test "x${XML_LIBS}" != "x")

AC_PROG_MAKE_SET

AC_HEADER_STDC

dnl Set the prefix to /home/trollbot if none exists
if test $prefix = NONE; then
    prefix=${HOME}/trollbot
fi

AC_OUTPUT(Makefile 
          src/Makefile
					src/modules/Makefile
					src/modules/crypto/Makefile
					src/modules/pgsql_log_filter/Makefile
					src/modules/ics/Makefile
					src/modules/irc/Makefile
					src/modules/js/Makefile
          doc_dist/Makefile
          scripts_dist/Makefile
					log_dist/Makefile
					modules_dist/Makefile
					text_dist/Makefile
          conf_dist/Makefile
					db_dist/Makefile
					webroot_dist/Makefile)



echo ""
echo "  ********** Trollbot Configuration **********  "
echo ""


echo ""
echo "  -=Protocol Support=-"
echo ""

if test "${with_irc}" = "yes"; then
	echo "  IRC        Support: Yes"
else
	echo "  IRS        Support: No"
fi

if test "${with_xmpp}" = "yes"; then
	echo "  XMPP       Support: Yes"
else
	echo "  XMPP       Support: No"
fi

if test "${with_ics}" = "yes"; then
	echo "  ICS        Support: Yes"
else
	echo "  ICS        Support: No"
fi

if test "${with_http}" = "yes"; then
 	echo "  HTTP       Support: Yes"
 else
		echo "  HTTP       Support: No"
fi


echo ""
echo "  -=Scripting Support=-"
echo ""

if test "x${TCL_LIBS}" != "x"; then
  echo "  TCL        Support: Yes"             
else
  echo "  TCL        Support: No"
fi

if test "x${JS_LIBS}" != "x"; then
  echo "  Javascript Support: Yes"
else
  echo "  Javascript Support: No"
fi

echo ""
echo "  -=Experimental Support=-"
echo ""

if test "x${PHP_LDADD}" != "x"; then
  echo "  PHP    Support: Yes"
else
  echo "  PHP    Support: No"
fi

if test "x${PYTHON_LDADD}" != "x"; then
  echo "  Python Support: Yes"
else
  echo "  Python Support: No"
fi

if test "x${PERL_LDADD}" != "x"; then
  echo "  Perl   Support: Yes"
else
  echo "  Perl   Support: No"
fi

if test "x${PQLIB}" != "x"; then
  echo "  Pgsql  Support: Yes"
else
  echo "  Pgsql  Support: No"
fi


echo ""
echo "  Files will be installed to: $prefix "
echo "  Type 'make' then 'make install' to continue"
echo ""
