#!/bin/sh
if [ y"$1" = y ]
then
 echo "Benutzung/usage:"
 echo "$0 CSBWIN-VERSION"
 echo "Installation VORHER durchführen!"
 echo "Install before calling this!"
else

 TMP=/var/tmp
 SRCZIP="$TMP/CSBwin_$1_Source.zip"
 BINZIP="$TMP/CSBwin_$1_Linux_$HOSTTYPE.zip"
 
 if [ h"$HOSTTYPE" = h ]
 then
  HOSTTYPE=i386
 fi

 ./clean.sh

 for KOPIEN in "CSBwin_11.067-11.068-12.0_Linux-Aenderungen" "CSBwin_12.100v0"
 do
  mv -v "$KOPIEN" "$TMP/$KOPIEN"
 done
 
 pushd ..
 zip -y9rz "$SRCZIP" CSBwin < CSBwin/srczipname.txt
 popd
 
 if [ -f CSBwin ]
 then
  CSBwin=CSBwin
 elif [ -f src/CSBwin ]
 then
  CSBwin=src/CSBwin 
 elif [ -f /usr/local/games/CSBwin ]
 then
  CSBwin=/usr/local/games/CSBwin
 else
  echo "KEIN PROGRAMM CSBwin gefunden!"
  echo "NO CSBwin executable FOUND!"
 fi
  
 zip -yj9rz "$BINZIP" $CSBwin ./config.linux ./csbwin*.sh COPYING README.linux AUTHORS ChangeLog CSBWIN_LINUX*.TXT < $HOSTTYPE.binzipname.txt

 unzip -tv "$SRCZIP"
 unzip -tv "$BINZIP"

 mv -v "$SRCZIP" .
 mv -v "$BINZIP" .

 for KOPIEN in "CSBwin_11.067-11.068-12.0_Linux-Aenderungen" "CSBwin_12.100v0"
 do
  mv -v "/var/tmp/$KOPIEN" .
 done
 
 ls -l CSBwin*.zip

fi
