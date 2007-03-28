VERSION = "0.0.1"

AC_INIT(src/main.c)
AM_INIT_AUTOMAKE(trollbot,0.0.1)

AC_CONFIG_HEADER(config.h)

AC_PROG_CC

dnl test C Compiler
AC_LANG_C

CMU_TCL

AM_CONDITIONAL(WITH_TCL, test "x${TCL_LIBS}" != "x")

AC_PROG_PHP

AM_CONDITIONAL(WITH_PHP, test "x${PHP_LDADD}" != "x")

AC_PROG_MAKE_SET

AC_HEADER_STDC

AC_SUBST(VERSION)

AC_OUTPUT(Makefile 
          src/Makefile)


