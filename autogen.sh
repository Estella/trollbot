#!/bin/sh


# Prep
cat tcl.m4 > acinclude.m4
cat php.m4 >> acinclude.m4

aclocal
autoconf
automake
