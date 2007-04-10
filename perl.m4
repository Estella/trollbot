dnl Perl finder thingy by poutine

AC_DEFUN([AX_PROG_PERL], [

  AC_MSG_CHECKING([For perl binary])
  AC_PATH_PROG(PERL,perl)

  if test "x${PERL}" != "x"; then
    PERL_LDADD="`${PERL} -MExtUtils::Embed -e ldopts`"
    PERL_CFLAGS="`${PERL} -MExtUtils::Embed -e ccopts`"
    AC_SUBST(PERL_LDADD)
    AC_SUBST(PERL_CFLAGS)
    AC_DEFINE([HAVE_PERL],1,[Ability to use Perl scripting])
  fi
] )
