dnl FIRST PASS AFTER STEALING THIS FROM CYRUS!
dnl USE AT YOUR OWN PERIL!
dnl I MEAN IT!
dnl
dnl tcl.m4: an autoconf Tcl locator
dnl $Id: tcl.m4,v 1.5 2005/03/05 00:36:14 dasenbro Exp $
dnl
dnl This is rob's Tcl macro, fixed by tjs.  It may need occasional tweaking,
dnl but until the next impediment to compilation, it's fill-in-the-blank,
dnl and it should be able to do reasonable things with user input.
dnl
dnl This will probably just work on Andrew systems, but given the variety
dnl and apparent creativity of people who compile Tcl elsewhere, I don't know
dnl what it will do.  I have yet to see an autoconf Tcl test that users were
dnl happy with.
dnl
dnl BUGS
dnl   The command-line arguments are overcomplicated.
dnl   There are doubtlessly others...

dnl To use this macro, just do CMU_TCL.  It outputs
dnl TCL_LIBS, TCL_CPPFLAGS, and TCL_DEFS and SUBSTs them.  
dnl If successful, these have stuff in them.  If not, they're empty.
dnl If not successful, with_tcl has the value "no".

AC_DEFUN([AX_PROG_TCL], [
# --- BEGIN CMU_TCL ---
dnl To link against Tcl, configure does several things to make my life
dnl "easier".
dnl
dnl * maybe ask the user where they think Tcl lives, and try to find it
dnl * maybe ask the user what "tclsh" is called this week (i.e., "tclsh8.0")
dnl * run tclsh, ask it for a path, then run that path through sed
dnl * sanity check its result (many installs are a little broken)
dnl * try to figure out where Tcl is based on this result
dnl * try to guess where the Tcl include files are
dnl
dnl Notes from previous incarnations:
dnl > XXX MUST CHECK FOR TCL BEFORE KERBEROS V4 XXX
dnl > This is because some genius at MIT named one of the Kerberos v4
dnl > library functions log().  This of course conflicts with the
dnl > logarithm function in the standard math library, used by Tcl.
dnl
dnl > Checking for Tcl first puts -lm before -lkrb on the library list.
dnl

dnl Check for some information from the user on what the world looks like
AC_ARG_WITH(tclconfig,[  --with-tclconfig=PATH   use tclConfig.sh from PATH
                          (configure gets Tcl configuration from here)],
        dnl trim tclConfig.sh off the end so we can add it back on later.
	TclLibBase=`echo ${withval} | sed s/tclConfig.sh\$//`)
AC_ARG_WITH(tcl,      [  --with-tcl=PATH         use Tcl from PATH],
	TclLibBase="${withval}/lib")
AC_ARG_WITH(tclsh,    [  --with-tclsh=TCLSH      use TCLSH as the tclsh program
                          (let configure find Tcl using this program)],
	TCLSH="${withval}")

if test "$TCLSH" = "no" -o "$with_tclconfig" = "no" ; then
  AC_MSG_WARN([Tcl disabled because tclsh or tclconfig specified as "no"])
  with_tcl=no
fi

if test "$with_tcl" != "no"; then
  if test \! -z "$with_tclconfig" -a \! -d "$with_tclconfig" ; then
    AC_MSG_ERROR([--with-tclconfig requires a directory argument.])
  fi

  if test \! -z "$TCLSH" -a \! -x "$TCLSH" ; then
    AC_MSG_ERROR([--with-tclsh must specify an executable file.])
  fi

  if test -z "$TclLibBase"; then # do we already know?
    # No? Run tclsh and ask it where it lives.

    # Do we know where a tclsh lives?
    if test -z "$TCLSH"; then
      # Try and find tclsh.  Any tclsh.
      # If a new version of tcl comes out and unfortunately adds another
      # filename, it should be safe to add it (to the front of the line --
      # somef vendors have older, badly installed tclshs that we want to avoid
      # if we can)
      AC_PATH_PROGS(TCLSH, [tclsh8.4 tclsh8.3 tclsh8.2 tclsh8.1 tclsh8.0 tclsh], "unknown")
    fi

    # Do we know where to get a tclsh?
    if test "${TCLSH}" != "unknown"; then
      AC_MSG_CHECKING([where Tcl says it lives])
      TclLibBase=`echo puts \\\$tcl_library | ${TCLSH} | sed -e 's,[^/]*$,,'`
      AC_MSG_RESULT($TclLibBase)
    fi
  fi

  if test -z "$TclLibBase" ; then
    AC_MSG_RESULT([can't find tclsh])
    AC_MSG_WARN([can't find Tcl installtion; use of Tcl disabled.])
    with_tcl=no
  else
    AC_MSG_CHECKING([for tclConfig.sh])
    # Check a list of places where the tclConfig.sh file might be.
    for tcldir in "${TclLibBase}" \
                  "${TclLibBase}/.." \
		  "${TclLibBase}"`echo ${TCLSH} | sed s/sh//` ; do
      if test -f "${tcldir}/tclConfig.sh"; then
        TclLibBase="${tcldir}"
        break
      fi
    done

    if test -z "${TclLibBase}" ; then
      AC_MSG_RESULT("unknown")
      AC_MSG_WARN([can't find Tcl configuration; use of Tcl disabled.])
      with_tcl=no
    else
      AC_MSG_RESULT(${TclLibBase}/)
    fi

    if test "${with_tcl}" != no ; then
      AC_MSG_CHECKING([Tcl configuration on what Tcl needs to compile])
      if test -f ${TclLibBase}/tclConfig.sh; then 
        AC_MSG_RESULT([ok])
        . ${TclLibBase}/tclConfig.sh
      else
        AC_MSG_RESULT(not found)
        with_tcl=no
      fi
    fi

    if test "${with_tcl}" != no ; then
      dnl Now, hunt for the Tcl include files, since we don't strictly
      dnl know where they are; some folks put them (properly) in the 
      dnl default include path, or maybe in /usr/local; the *BSD folks
      dnl put them in other places.
      AC_MSG_CHECKING([where Tcl includes are])
      for tclinclude in "${TCL_PREFIX}/include/tcl${TCL_VERSION}" \
                        "${TCL_PREFIX}/include/tcl" \
                        "${TCL_PREFIX}/include" ; do
        if test -r "${tclinclude}/tcl.h" ; then
          TCL_CPPFLAGS="-I${tclinclude}"
          break
        fi
      done
      if test -z "${TCL_CPPFLAGS}" ; then
        AC_MSG_WARN(can't find Tcl includes; use of Tcl disabled.)
        with_tcl=no
      fi
      AC_MSG_RESULT(${TCL_CPPFLAGS})
    fi
    
    # Finally, pick up the Tcl configuration if we haven't found an
    # excuse not to.
    if test "${with_tcl}" != no; then
      dnl TCL_LIBS="${TCL_LD_SEARCH_FLAGS} ${TCL_LIB_SPEC}"
      TCL_LIBS="${TCL_LIB_SPEC} ${TCL_LIBS}"
    fi

    if test "x${TCL_CPPFLAGS}" != "x" -a "x${TCL_LIBS}" != "x" ; then
      AC_DEFINE(HAVE_TCL, 1, [Ability to use TCL scripting])
    fi
  fi
fi

AC_SUBST(TCL_DEFS)
AC_SUBST(TCL_LIBS)
AC_SUBST(TCL_CPPFLAGS)

# --- END CMU_TCL ---
]) dnl AX_PROG_TCL
dnl PHP finder thingy
dnl by poutine

AC_DEFUN([AX_PROG_PHP],[
  AC_ARG_WITH(php-cflags, [  --with-php-cflags=FLAGS  CFLAGS to use for PHP],
        [ with_php_cflags="$withval" ])
  AC_ARG_WITH(php-ldflags,[  --with-php-ldflags=FLAGS LDFLAGS to use PHP],
        [ with_php_ldflags="$withval" ])
  AC_ARG_WITH(php-config, [  --with-php-config=PATH   use php-config from PATH],
        [ with_php_config="$withval" ] )             
  AC_ARG_WITH(php-lib,    [  --with-php-lib=PATH      PATH to libphp5.so or libphp4.so],
        [ with_php_lib="$withval" ] )
  AC_ARG_WITH(php,        [  --with-php=(4|5)         configure with PHP4 or 5 support],
        [ with_php="$withval" ] )

  dnl Default to check for PHP
  if test "x${with_php}" != "xno" ; then
    AC_MSG_CHECKING([if there's a valid user specified php-config])

    if test -f "$with_php/php-config"; then
      PHPCONFIG="${with_php}/php-config"
      AC_MSG_RESULT([Found at ${PHPCONFIG}])
    else
      AC_MSG_RESULT([Not specified, checking elsewhere])
      AC_PATH_PROG(PHPCONFIG,php-config)
    fi

    if test "x${PHPCONFIG}" != "x"; then
      AC_MSG_CHECKING([for CFLAGS to use with PHP])
    
      if test "x${with_php_cflags}" != "x"; then
        PHP_CFLAGS="${with_php_cflags} `${PHPCONFIG} --includes`"
      else
        PHP_CFLAGS="`${PHPCONFIG} --includes`"
      fi
   
      AC_MSG_RESULT([${PHP_CFLAGS}])

      AC_MSG_CHECKING([whether our test PHP program compiles])
  
      oldcppflags="${CPPFLAGS}"

      CPPFLAGS="${oldcppflags} ${PHP_CFLAGS}"

      AC_TRY_COMPILE([#include <main/php.h>
                      #include <main/SAPI.h>
                      #include <main/php_main.h>
                      #include <main/php_variables.h>
                      #include <main/php_ini.h>
                      #include <zend_ini.h>
                      ],[
                        zval *test;
                      ],[
                        php_compile="yes"
                      ],[
                        php_compile="no"
                      ])
 
      if test "${php_compile}" = "yes"; then
        AC_MSG_RESULT([Wonderbar])
        dnl yay we compiled, now to test linking

        if test "${with_php}" = "5" || test "x${with_php}" = "x"; then 
          AC_MSG_CHECKING([for LDFLAGS to use with PHP5])
        
          if test "x${with_php_ldflags}" != "x"; then
            PHP_LDADD="${with_php_ldflags} `${PHPCONFIG} --libs` `${PHPCONFIG} --ldflags` -lphp5 -Wl,--unresolved-symbols=ignore-all"
          else
            PHP_LDADD="`${PHPCONFIG} --libs` `${PHPCONFIG} --ldflags` -lphp5 -Wl,--unresolved-symbols=ignore-all"
          fi

          if test "x${with_php_lib}" != "x"; then
            PHP_LDADD="-L${with_php_lib} -Wl,-rpath=${with_php_lib} ${PHP_LDADD}"
          fi

          AC_MSG_RESULT([${PHP_LDADD}])

          AC_MSG_CHECKING([whether our test PHP5 program can link])
          
          oldlibs="${LIBS}"
          LIBS="${oldlibs} ${PHP_LDADD}"

          AC_TRY_LINK([#include <main/php.h>
                       #include <main/SAPI.h>
                       #include <main/php_main.h>
                       #include <main/php_variables.h>
                       #include <main/php_ini.h>
                       #include <zend_ini.h>
                       ],[
                         zval *test;
                       ],[
                         php_link="yes"
                       ],[
                         php_link="no"
                       ])

          LIBS="${oldlibs}"

          if test "${php_link}" = "yes"; then
            AC_MSG_RESULT([Wonderbar])
            AC_DEFINE([HAVE_PHP],1,[Ability to use PHP scripting])

            AC_SUBST(PHP_CFLAGS)
            AC_SUBST(PHP_LDADD)
          else
            AC_MSG_RESULT([Error, cannot use PHP5])
          fi
        fi

        if test "${with_php}" = "4" || test "${php_link}" = "no"; then
          AC_MSG_CHECKING([for LDFLAGS to use with PHP4])

          if test "x${with_php_ldflags}" != "x"; then
            PHP_LDADD="${with_php_ldflags} `${PHPCONFIG} --libs` `${PHPCONFIG} --ldflags` -lphp4 -Wl,--unresolved-symbols=ignore-all"
          else
            PHP_LDADD="`${PHPCONFIG} --libs` `${PHPCONFIG} --ldflags` -lphp4 -Wl,--unresolved-symbols=ignore-all"
          fi

          if test "x${with_php_lib}" != "x"; then
            PHP_LDADD="-L${with_php_lib} -Wl,-rpath=${with_php_lib} ${PHP_LDADD}"
          fi

          AC_MSG_RESULT([${PHP_LDADD}])

          AC_MSG_CHECKING([whether our test PHP4 program can link])

          oldlibs="${LIBS}"
          LIBS="${oldlibs} ${PHP_LDADD}"

          AC_TRY_LINK([#include <main/php.h>
                       #include <main/SAPI.h>
                       #include <main/php_main.h>
                       #include <main/php_variables.h>
                       #include <main/php_ini.h>
                       #include <zend_ini.h>
                       ],[
                         zval *test;
                       ],[
                         php_link="yes"
                       ],[
                         php_link="no"
                       ])

          LIBS="${oldlibs}"

          if test "${php_link}" = "yes"; then
            AC_MSG_RESULT([Wonderbar])
            AC_DEFINE([HAVE_PHP],1,[Ability to use PHP scripting])

            AC_SUBST(PHP_CFLAGS)
            AC_SUBST(PHP_LDADD)
          else
            AC_MSG_RESULT([Error, cannot use PHP4])
          fi

        fi
      else        
        AC_MSG_RESULT([Did not compile. PHP disabled])
      fi
  
      CPPFLAGS="${oldcppflags}"
    else
      AC_MSG_WARN([No php-config found or specified, PHP disabled.])
    fi
  else
    AC_MSG_WARN([PHP Support disabled])
  fi      
] )
dnl Perl finder thingy by poutine

AC_DEFUN([AX_PROG_PERL], [
  AC_ARG_WITH(perl,      [  --with-perl=PATH         use Perl from PATH],
        with_perl="$withval")

  if test "${with_perl}" != "no"; then
    AC_MSG_CHECKING([For perl binary])
    AC_PATH_PROG(PERL,perl)

    if test "x${PERL}" != "x"; then
      PERL_LDADD="`${PERL} -MExtUtils::Embed -e ldopts`"
      PERL_CFLAGS="`${PERL} -MExtUtils::Embed -e ccopts`"

      oldlibs="${LIBS}"
      oldcppflags="${CPPFLAGS}"
      CPPFLAGS="${oldcppflags} ${PERL_CFLAGS}"
      LIBS="${oldlibs} ${PERL_LDADD}"

      AC_MSG_CHECKING([whether we can link with perl])
      
      AC_TRY_LINK([#include <EXTERN.h>
                   #include <perl.h>
                  ],[
                   perl_alloc()
                  ],[
                   perl_link="yes"
                  ],[
                   perl_link="no"])

      LIBS="${oldlibs}"
      CPPFLAGS="${oldcppflags}"

      if test "x${perl_link}" = "xyes"; then
        AC_MSG_RESULT([Yes])
        AC_SUBST(PERL_LDADD)
        AC_SUBST(PERL_CFLAGS)
        AC_DEFINE([HAVE_PERL],1,[Ability to use Perl scripting])
      else
        AC_MSG_RESULT([No])
      fi
    fi
  fi
] )
dnl http://autoconf-archive.cryp.to/ac_python_devel.html

AC_DEFUN([AC_PYTHON_DEVEL],[
        #
        # Allow the use of a (user set) custom python version
        #
        AC_ARG_VAR([PYTHON_VERSION],[The installed Python
                version to use, for example '2.3'. This string
                will be appended to the Python interpreter
                canonical name.])

        AC_PATH_PROG([PYTHON],[python[$PYTHON_VERSION]])
        if test -z "$PYTHON"; then
           AC_MSG_ERROR([Cannot find python$PYTHON_VERSION in your system path])
           PYTHON_VERSION=""
        fi

        #
        # Check for a version of Python >= 2.1.0
        #
        AC_MSG_CHECKING([for a version of Python >= '2.1.0'])
        ac_supports_python_ver=`$PYTHON -c "import sys, string; \
                ver = string.split(sys.version)[[0]]; \
                print ver >= '2.1.0'"`
        if test "$ac_supports_python_ver" != "True"; then
                if test -z "$PYTHON_NOVERSIONCHECK"; then
                        AC_MSG_RESULT([no])
                        AC_MSG_FAILURE([
This version of the AC@&t@_PYTHON_DEVEL macro
doesn't work properly with versions of Python before
2.1.0. You may need to re-run configure, setting the
variables PYTHON_CPPFLAGS, PYTHON_LDFLAGS, PYTHON_SITE_PKG,
PYTHON_EXTRA_LIBS and PYTHON_EXTRA_LDFLAGS by hand.
Moreover, to disable this check, set PYTHON_NOVERSIONCHECK
to something else than an empty string.
])
                else
                        AC_MSG_RESULT([skip at user request])
                fi
        else
                AC_MSG_RESULT([yes])
        fi

        #
        # if the macro parameter ``version'' is set, honour it
        #
        if test -n "$1"; then
                AC_MSG_CHECKING([for a version of Python $1])
                ac_supports_python_ver=`$PYTHON -c "import sys, string; \
                        ver = string.split(sys.version)[[0]]; \
                        print ver $1"`
                if test "$ac_supports_python_ver" = "True"; then
                   AC_MSG_RESULT([yes])
                else
                        AC_MSG_RESULT([no])
                        AC_MSG_ERROR([this package requires Python $1.
If you have it installed, but it isn't the default Python
interpreter in your system path, please pass the PYTHON_VERSION
variable to configure. See ``configure --help'' for reference.
])
                        PYTHON_VERSION=""
                fi
        fi

        #
        # Check if you have distutils, else fail
        #
        AC_MSG_CHECKING([for the distutils Python package])
        ac_distutils_result=`$PYTHON -c "import distutils" 2>&1`
        if test -z "$ac_distutils_result"; then
                AC_MSG_RESULT([yes])
        else
                AC_MSG_RESULT([no])
                AC_MSG_ERROR([cannot import Python module "distutils".
Please check your Python installation. The error was:
$ac_distutils_result])
                PYTHON_VERSION=""
        fi

        #
        # Check for Python include path
        #
        AC_MSG_CHECKING([for Python include path])
        if test -z "$PYTHON_CPPFLAGS"; then
                python_path=`$PYTHON -c "import distutils.sysconfig; \
                        print distutils.sysconfig.get_python_inc();"`
                if test -n "${python_path}"; then
                        python_path="-I$python_path"
                fi
                PYTHON_CPPFLAGS=$python_path
        fi
        AC_MSG_RESULT([$PYTHON_CPPFLAGS])
        AC_SUBST([PYTHON_CPPFLAGS])

        #
        # Check for Python library path
        #
        AC_MSG_CHECKING([for Python library path])
        if test -z "$PYTHON_LDFLAGS"; then
                # (makes two attempts to ensure we've got a version number
                # from the interpreter)
                py_version=`$PYTHON -c "from distutils.sysconfig import *; \
                        from string import join; \
                        print join(get_config_vars('VERSION'))"`
                if test "$py_version" == "[None]"; then
                        if test -n "$PYTHON_VERSION"; then
                                py_version=$PYTHON_VERSION
                        else
                                py_version=`$PYTHON -c "import sys; \
                                        print sys.version[[:3]]"`
                        fi
                fi

                PYTHON_LDFLAGS=`$PYTHON -c "from distutils.sysconfig import *; \
                        from string import join; \
                        print '-L' + get_python_lib(0,1), \
                        '-lpython';"`$py_version
        fi
        AC_MSG_RESULT([$PYTHON_LDFLAGS])
        AC_SUBST([PYTHON_LDFLAGS])

        #
        # Check for site packages
        #
        AC_MSG_CHECKING([for Python site-packages path])
        if test -z "$PYTHON_SITE_PKG"; then
                PYTHON_SITE_PKG=`$PYTHON -c "import distutils.sysconfig; \
                        print distutils.sysconfig.get_python_lib(0,0);"`
        fi
        AC_MSG_RESULT([$PYTHON_SITE_PKG])
        AC_SUBST([PYTHON_SITE_PKG])

        #
        # libraries which must be linked in when embedding
        #
        AC_MSG_CHECKING(python extra libraries)
        if test -z "$PYTHON_EXTRA_LIBS"; then
           PYTHON_EXTRA_LIBS=`$PYTHON -c "import distutils.sysconfig; \
                conf = distutils.sysconfig.get_config_var; \
                print conf('LOCALMODLIBS'), conf('LIBS')"`
        fi
        AC_MSG_RESULT([$PYTHON_EXTRA_LIBS])
        AC_SUBST(PYTHON_EXTRA_LIBS)

        #
        # linking flags needed when embedding
        #
        AC_MSG_CHECKING(python extra linking flags)
        if test -z "$PYTHON_EXTRA_LDFLAGS"; then
                PYTHON_EXTRA_LDFLAGS=`$PYTHON -c "import distutils.sysconfig; \
                        conf = distutils.sysconfig.get_config_var; \
                        print conf('LINKFORSHARED')"`
        fi
        AC_MSG_RESULT([$PYTHON_EXTRA_LDFLAGS])
        AC_SUBST(PYTHON_EXTRA_LDFLAGS)

        #
        # final check to see if everything compiles alright
        #
        AC_MSG_CHECKING([consistency of all components of python development environment])
        AC_LANG_PUSH([C])
        # save current global flags
        LIBS="$ac_save_LIBS $PYTHON_LDFLAGS -Wl,--unresolved-symbols=ignore-all"
        CPPFLAGS="$ac_save_CPPFLAGS $PYTHON_CPPFLAGS"
        AC_TRY_LINK([
                #include <Python.h>
        ],[
                Py_Initialize();
        ],[pythonexists=yes],[pythonexists=no])

        AC_MSG_RESULT([$pythonexists])

        if test ! "$pythonexists" = "yes"; then
           AC_MSG_ERROR([
  Could not link test program to Python. Maybe the main Python library has been
  installed in some non-standard library path. If so, pass it to configure,
  via the LDFLAGS environment variable.
  Example: ./configure LDFLAGS="-L/usr/non-standard-path/python/lib"
  ============================================================================
   ERROR!
   You probably have to install the development version of the Python package
   for your distribution.  The exact name of this package varies among them.
  ============================================================================
           ])
          PYTHON_VERSION=""
        else
          PYTHON_CFLAGS="${PYTHON_CPPFLAGS}"
          PYTHON_LDADD="${PYTHON_LDFLAGS} ${PYTHON_EXTRA_LIBS} ${PYTHON_EXTRA_LDFLAGS}"
          AC_SUBST([PYTHON_CFLAGS])
          AC_SUBST([PYTHON_LDADD])

          AC_DEFINE([HAVE_PYTHON],1,[Ability to use Python scripting])
        fi
        AC_LANG_POP
        # turn back to default flags
        CPPFLAGS="$ac_save_CPPFLAGS"
        LIBS="$ac_save_LIBS"

        #
        # all done!
        #
])
