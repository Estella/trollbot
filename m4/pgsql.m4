# Checks for libpq and libpq-fe.h, sets variables if found.
AC_DEFUN(POSTGRESQL_CHECKS, [
	AC_SUBST(PQINCPATH)
	AC_SUBST(PQLIB)

echo Checking if PostgreSQL is available

dnl Check for PQLIB first.
AC_CHECK_LIB(pq,PQconnectdb,FOUND_PQLIB=1)
if test $FOUND_PQLIB ; then
	PQLIB="-lpq"
else
	echo PostgreSQL library not found in default library path.
	exit 0
fi

dnl Check for PQINCPATH
if test -d /usr/include/pgsql ; then
	pg_inc_dir="/usr/include/pgsql"
elif test -d /usr/include/postgresql ; then
	pg_inc_dir="/usr/include/postgresql"
elif test -d /usr/local/psql/include ; then
	pg_inc_dir="/usr/local/psql/include"
elif test -d /opt/psql/include ; then
	pg_inc_dir="/opt/psql/include"
else
	echo PostgreSQL includes directory not found!
	PQLIB=""
	exit 0
fi

dnl Check for libpq-fe.h
AC_CHECK_HEADERS("$pg_inc_dir/libpq-fe.h")
if test $HAVE_HEADER-LIBPQ-FE.H ; then
	PQINCPATH="-I$pg_inc_dir"
else
	echo PostgreSQL header libpq-fe.h not found!
	PQLIB=""
	exit 0
fi

echo PostgreSQL configuration successful.
])
