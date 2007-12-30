VERSION = "1.0.0"

AC_SUBST(VERSION)

dnl set the default prefix to be /home/user/trollbot

AC_INIT(src/trollbot.c)
AM_INIT_AUTOMAKE(trollbot,1.0.0)

AC_CONFIG_HEADER(config.h)

AC_CONFIG_SUBDIRS(libltdl)

AC_LIBTOOL_DLOPEN
AC_LIBLTDL_CONVENIENCE
AC_PROG_LIBTOOL

AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)

AM_MAINTAINER_MODE

AC_PROG_CC

dnl test C Compiler
AC_LANG_C

AC_C_CONST

AC_STRUCT_TM

AC_CHECK_HEADERS([ctype.h])
AC_CHECK_HEADERS([sys/select.h])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([sys/time.h])

dnl set HAVE_WORKING_FORK if system supports forking
AC_FUNC_FORK

AC_TYPE_PID_T

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

dnl AC_PYTHON_DEVEL

AM_CONDITIONAL(WITH_PYTHON, test "x${PYTHON_LDADD}" != "x")

AC_PROG_MAKE_SET

AC_HEADER_STDC

dnl Set the prefix to /home/trollbot if none exists
if test $prefix = NONE; then
    prefix=${HOME}/trollbot
fi

 	
dnl Xsed="sed -e s/^X//"
dnl LTLIBOBJS=`echo X"$LIBOBJS"|\
dnl           [$Xsed -e "s,\.[^.]* ,.lo ,g;s,\.[^.]*$,.lo,"]`
dnl AC_SUBST(LTLIBOBJS)

AC_OUTPUT(Makefile 
          src/Makefile
          doc_dist/Makefile
          scripts_dist/Makefile
          conf_dist/Makefile
          text_dist/Makefile
          db_dist/Makefile
          src/modules/Makefile
          src/modules/foo/Makefile
          src/modules/irc/Makefile)
dnl          src/modules/dcc/Makefile
dnl          src/modules/scripting/Makefile
dnl          src/modules/scripting/tcl/Makefile
dnl          src/modules/scripting/php/Makefile
dnl          src/modules/scripting/perl/Makefile
dnl          src/modules/scripting/python/Makefile)



echo ""
echo "  ********** Trollbot Configuration **********  "

echo ""
echo "  *** IRC *** "
if test "x${TCL_LIBS}" != "x"; then
  echo "  TCL    Support: Yes"             
else
  echo "  TCL    Support: No"
fi

if test "x${PHP_LDADD}" != "x"; then
  echo "  PHP    Support: Yes"
else
  echo "  PHP    Support: No"
fi

if test "x${PERL_LDADD}" != "x"; then
  echo "  Perl   Support: Yes"
else
  echo "  Perl   Support: No"
fi

if test "x${PYTHON_LDADD}" != "x"; then
  echo "  Python Support: Yes"
else
  echo "  Python Support: No"
fi

if test "x${TMIRC_LDADD}" != "x"; then
  echo "  Trollbot mIRC Scripting Support (experimental): Yes"
else
  echo "  Trollbot mIRC Scripting Support (experimental): No"
fi

echo ""

echo ""
echo "  Files will be installed to: $prefix "
echo "  Type 'make' then 'make install' to continue"
echo ""
