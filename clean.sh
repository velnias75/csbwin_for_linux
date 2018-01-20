#!/bin/sh
cd src
make maintainer-clean
cd ..
for LOESCHE in CSBwin missing install-sh depcomp config.sub config.guess config.status config.log libgtk.txt *~ src/*~ aclocal.m4 configure stamp.h.in stamp.h ltmain.sh Makefile.in Makefile libtool autom4te.cache/* .deps/*.Po src/.deps/*.Po src/log.log CSBwin_*.zip
do
 if [ -f "$LOESCHE" ]
 then
  rm -v "$LOESCHE"
 fi
done
