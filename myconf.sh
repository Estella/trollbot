#!/bin/sh

./configure --with-tclsh=/usr/local/bin/tclsh8.4 --with-js=yes --with-perl --with-php=5 --with-libxml2 --with-xmpp --with-http --with-ics --prefix=/home/poutine/trollbot

for i in `find ./ -name Makefile`; do sed 's/O2/O0/g' $i > $i.new; mv $i.new $i; done;
