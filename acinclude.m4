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
      AC_PATH_PROGS(TCLSH, [tclsh8.5 tclsh8.4 tclsh8.3 tclsh8.2 tclsh8.1 tclsh8.0 tclsh], "unknown")
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
    else
      TCL_CPPFLAGS=""
      TCL_LIBS=""
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
  AC_ARG_WITH(php,        [  --with-php=4|5|6         configure with PHP4 PHP5 or PHP6 support],
        [ with_php="$withval" ] )

  dnl Default to check for PHP
  if test "${with_php}" = "yes" || test "${with_php}" = "6" || test "${with_php}" = "5" || test "${with_php}" = "4"; then
    AC_MSG_CHECKING([if there's a valid user specified php-config])

    if test -f "${with_php_config}/php-config"; then
      PHPCONFIG="${with_php_config}/php-config"
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
            PHP_CFLAGS=""
            PHP_LDADD=""
            AC_MSG_RESULT([Error, cannot use PHP5])
          fi
        fi

        if test "${with_php}" = "6"; then 
          AC_MSG_CHECKING([for LDFLAGS to use with PHP6])
        
          if test "x${with_php_ldflags}" != "x"; then
            PHP_LDADD="${with_php_ldflags} `${PHPCONFIG} --libs` `${PHPCONFIG} --ldflags` -lphp6 -Wl,--unresolved-symbols=ignore-all"
          else
            PHP_LDADD="`${PHPCONFIG} --libs` `${PHPCONFIG} --ldflags` -lphp6 -Wl,--unresolved-symbols=ignore-all"
          fi

          if test "x${with_php_lib}" != "x"; then
            PHP_LDADD="-L${with_php_lib} -Wl,-rpath=${with_php_lib} ${PHP_LDADD}"
          fi

          AC_MSG_RESULT([${PHP_LDADD}])

          AC_MSG_CHECKING([whether our test PHP6 program can link])
          
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
            PHP_CFLAGS=""
            PHP_LDADD=""
            AC_MSG_RESULT([Error, cannot use PHP6])
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
            PHP_CFLAGS=""
            PHP_LDADD=""
            AC_MSG_RESULT([Error, cannot use PHP4])
          fi

        fi
      else        
        AC_MSG_RESULT([Did not compile. PHP disabled])
      fi
  
      CPPFLAGS="${oldcppflags}"
    else
      PHP_CFLAGS=""
      PHP_LDADD=""
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

  if test "${with_perl}" = "yes"; then
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
        PERL_LDADD=""
        PERL_CFLAGS=""
        AC_MSG_RESULT([No])
      fi
    fi
  fi
] )
dnl http://autoconf-archive.cryp.to/ac_python_devel.html

AC_DEFUN([AC_PYTHON_DEVEL],[
  AC_ARG_WITH(python,        [  --with-python          configure with Python scripting support],
        [ with_python="$withval" ] )
	if test "${with_python}" == "yes"; then

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
	fi
])
# Configure paths for LIBXML2
# Mike Hommey 2004-06-19
# use CPPFLAGS instead of CFLAGS
# Toshio Kuratomi 2001-04-21
# Adapted from:
# Configure paths for GLIB
# Owen Taylor     97-11-3

dnl AM_PATH_XML2([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for XML, and define XML_CPPFLAGS and XML_LIBS
dnl
AC_DEFUN([AM_PATH_XML2],[ 
AC_ARG_WITH(xml-prefix,
            [  --with-xml-prefix=PFX   Prefix where libxml is installed (optional)],
            xml_config_prefix="$withval", xml_config_prefix="")
AC_ARG_WITH(xml-exec-prefix,
            [  --with-xml-exec-prefix=PFX Exec prefix where libxml is installed (optional)],
            xml_config_exec_prefix="$withval", xml_config_exec_prefix="")
AC_ARG_ENABLE(xmltest,
              [  --disable-xmltest       Do not try to compile and run a test LIBXML program],,
              enable_xmltest=yes)

  if test x$xml_config_exec_prefix != x ; then
     xml_config_args="$xml_config_args"
     if test x${XML2_CONFIG+set} != xset ; then
        XML2_CONFIG=$xml_config_exec_prefix/bin/xml2-config
     fi
  fi
  if test x$xml_config_prefix != x ; then
     xml_config_args="$xml_config_args --prefix=$xml_config_prefix"
     if test x${XML2_CONFIG+set} != xset ; then
        XML2_CONFIG=$xml_config_prefix/bin/xml2-config
     fi
  fi

  AC_PATH_PROG(XML2_CONFIG, xml2-config, no)
  min_xml_version=ifelse([$1], ,2.0.0,[$1])
  AC_MSG_CHECKING(for libxml - version >= $min_xml_version)
  no_xml=""
  if test "$XML2_CONFIG" = "no" ; then
    no_xml=yes
  else
    XML_CPPFLAGS=`$XML2_CONFIG $xml_config_args --cflags`
    XML_LIBS=`$XML2_CONFIG $xml_config_args --libs`
    xml_config_major_version=`$XML2_CONFIG $xml_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    xml_config_minor_version=`$XML2_CONFIG $xml_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    xml_config_micro_version=`$XML2_CONFIG $xml_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_xmltest" = "xyes" ; then
      ac_save_CPPFLAGS="$CPPFLAGS"
      ac_save_LIBS="$LIBS"
      CPPFLAGS="$CPPFLAGS $XML_CPPFLAGS"
      LIBS="$XML_LIBS $LIBS"
dnl
dnl Now check if the installed libxml is sufficiently new.
dnl (Also sanity checks the results of xml2-config to some extent)
dnl
      rm -f conf.xmltest
      AC_TRY_RUN([
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libxml/xmlversion.h>

int 
main()
{
  int xml_major_version, xml_minor_version, xml_micro_version;
  int major, minor, micro;
  char *tmp_version;

  system("touch conf.xmltest");

  /* Capture xml2-config output via autoconf/configure variables */
  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = (char *)strdup("$min_xml_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string from xml2-config\n", "$min_xml_version");
     exit(1);
   }
   free(tmp_version);

   /* Capture the version information from the header files */
   tmp_version = (char *)strdup(LIBXML_DOTTED_VERSION);
   if (sscanf(tmp_version, "%d.%d.%d", &xml_major_version, &xml_minor_version, &xml_micro_version) != 3) {
     printf("%s, bad version string from libxml includes\n", "LIBXML_DOTTED_VERSION");
     exit(1);
   }
   free(tmp_version);

 /* Compare xml2-config output to the libxml headers */
  if ((xml_major_version != $xml_config_major_version) ||
      (xml_minor_version != $xml_config_minor_version) ||
      (xml_micro_version != $xml_config_micro_version))
    {
      printf("*** libxml header files (version %d.%d.%d) do not match\n",
         xml_major_version, xml_minor_version, xml_micro_version);
      printf("*** xml2-config (version %d.%d.%d)\n",
         $xml_config_major_version, $xml_config_minor_version, $xml_config_micro_version);
      return 1;
    } 
/* Compare the headers to the library to make sure we match */
  /* Less than ideal -- doesn't provide us with return value feedback, 
   * only exits if there's a serious mismatch between header and library.
   */
    LIBXML_TEST_VERSION;

    /* Test that the library is greater than our minimum version */
    if ((xml_major_version > major) ||
        ((xml_major_version == major) && (xml_minor_version > minor)) ||
        ((xml_major_version == major) && (xml_minor_version == minor) &&
        (xml_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of libxml (%d.%d.%d) was found.\n",
               xml_major_version, xml_minor_version, xml_micro_version);
        printf("*** You need a version of libxml newer than %d.%d.%d. The latest version of\n",
           major, minor, micro);
        printf("*** libxml is always available from ftp://ftp.xmlsoft.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the xml2-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of LIBXML, but you can also set the XML2_CONFIG environment to point to the\n");
        printf("*** correct copy of xml2-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
    }
  return 1;
}
],, no_xml=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CPPFLAGS="$ac_save_CPPFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi

  if test "x$no_xml" = x ; then
     AC_MSG_RESULT(yes (version $xml_config_major_version.$xml_config_minor_version.$xml_config_micro_version))
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$XML2_CONFIG" = "no" ; then
       echo "*** The xml2-config script installed by LIBXML could not be found"
       echo "*** If libxml was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the XML2_CONFIG environment variable to the"
       echo "*** full path to xml2-config."
     else
       if test -f conf.xmltest ; then
        :
       else
          echo "*** Could not run libxml test program, checking why..."
          CPPFLAGS="$CPPFLAGS $XML_CPPFLAGS"
          LIBS="$LIBS $XML_LIBS"
          AC_TRY_LINK([
#include <libxml/xmlversion.h>
#include <stdio.h>
],      [ LIBXML_TEST_VERSION; return 0;],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding LIBXML or finding the wrong"
          echo "*** version of LIBXML. If it is not finding LIBXML, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
          echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means LIBXML was incorrectly installed"
          echo "*** or that you have moved LIBXML since it was installed. In the latter case, you"
          echo "*** may want to edit the xml2-config script: $XML2_CONFIG" ])
          CPPFLAGS="$ac_save_CPPFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi

     XML_CPPFLAGS=""
     XML_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(XML_CPPFLAGS)
  AC_SUBST(XML_LIBS)
  rm -f conf.xmltest
])
