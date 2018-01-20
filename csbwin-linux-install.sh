#!/bin/sh
PRAEFIX="/usr/local"
NAME="CSBwin"
GAMES="games"
CSBCFG="config.linux"
SCRIPT="csbwin.sh"
SHRDIR="$PRAEFIX/share"
GAMDIR="$SHRDIR/$GAMES"
CSBBAS="$GAMDIR/$NAME"
BINDIR="$PRAEFIX/$GAMES"
PROG="$BINDIR/$NAME"
DOCDIR="$SHRDIR/doc"
GAMDOC="$DOCDIR/$GAMES"
CSBDOC="$GAMDOC/$NAME"
CSBDOCS="COPYING README.linux AUTHORS ChangeLog CSBWIN_LINUX_LIESMICH.TXT CSBWIN_LINUX_README.TXT"

for DIR in "$BINDIR" "$GAMDIR" "$CSBBAS" "$GAMDOC" "$CSBDOC"
do
 if [ ! -d "$DIR" ]
 then
  mkdir -vp "$DIR"
  chown $GAMES:$GAMES "$DIR"
  chmod 2775 "$DIR"
 fi
done

if [ -f "./src/$NAME" ]
then
 cp -av "./src/$NAME" ./$NAME
fi

if [ -f "$NAME" ]
then
 chown $GAMES:$GAMES "$NAME"
 chmod 775 "$NAME"
 cp -av "$NAME" "$PROG"
else
 echo "********** « $NAME »:"
 echo "********** Das Programm fehlt! Executable is missing!" 
fi

if [ -f "$SCRIPT" ]
then
 chown $GAMES:$GAMES "$SCRIPT"
 chmod 775 "$SCRIPT"
 cp -av "$SCRIPT" "$BINDIR/$SCRIPT"
else
 echo "********** « $SCRIPT »:"
 echo "********** Das Programm fehlt! Executable is missing!" 
fi

if [ -f "$CSBCFG" ]
then
 chown $GAMES:$GAMES "$CSBCFG"
 chmod 664  "$CSBCFG"
 cp -av "$CSBCFG" "$CSBBAS/$CSBCFG"
else
 echo "********** « $CSBCFG »:"
 echo "********** Konfigurationsdatei fehlt! Configuration file missing!"
fi

for DATEI in $CSBDOCS
do
 if [ -f "$DATEI" ]
 then
  chown $GAMES:$GAMES "$DATEI"
  chmod 664  "$DATEI"
  cp -av "$DATEI" "$CSBDOC/$DATEI"
 else
  echo "********** « $DATEI »:"
  echo "********** Datei fehlt! File missing!"
 fi
done

for OBJ in "$PRAEFIX" "$SHRDIR" "$GAMDIR" "$CSBBAS" "$CSBBAS/$CSBCFG" "$BINDIR" "$PROG" "$DOCDIR" "$GAMDOC" "$CSBDOC"
do
 if [ ! -e "$OBJ" ]
 then 
  echo "„$OBJ“ konnte NICHT erfolgreich angelegt werden!"
  echo "\"$OBJ\" NOT created successfully!"
  echo "« root »?"
 fi
done
