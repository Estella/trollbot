#!/bin/sh

./configure --with-tclsh=/usr/bin/tclsh8.4 --with-js=yes --with-perl --with-libxml2 --with-xmpp --with-http --with-ics --prefix=/home/ben/Projects/trollbot --with-python

for i in `find ./ -name Makefile`; do sed 's/O2/O0/g' $i > $i.new; mv $i.new $i; done;
