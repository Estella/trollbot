#!/bin/sh


# Prep
cat m4/tcl.m4 > acinclude.m4
cat m4/php.m4 >> acinclude.m4
cat m4/perl.m4 >> acinclude.m4
cat m4/python.m4 >> acinclude.m4
cat m4/libxml2.m4 >> acinclude.m4
cat m4/pgsql.m4 >> acinclude.m4

autoreconf
