#!/bin/sh
PRAEFIX="/usr/local"
NAME="CSBwin"
SPIEL="$1"
CSBCFG="$HOME/.$NAME"
SPIELSTAENDE="$CSBCFG/$SPIEL"
CSBBAS="$PRAEFIX/share/games/$NAME"
SPIELVZ="$CSBBAS/$SPIEL"
DUNGEON="$SPIELVZ/dungeon.dat"
PROG="$PRAEFIX/games/$NAME"
if [ "x$SPIEL" = "x" ]
then
 echo "*** Kein Spielverzeichnis angegeben! *** No game directory provided! ***"
 echo "Installiere Deine Spiele in Unterverzeichnissen:"
 echo "Put your games into subdirectories:"
 echo "                 „$CSBBAS/\$DIR“." 
 echo "Aufruf/run:      „$0 \$DIR“."
elif [ ! -x "$PROG" ]
then
 echo "Nicht vorhanden oder ausführbar / not executable or not existing at all:"
 echo "„$PROG“"
elif [ -f "$DUNGEON" ]
then
 if [ ! -d $SPIELSTAENDE ]
 then
  mkdir -p "$SPIELSTAENDE"
 fi
 if [ -d $SPIELSTAENDE ]
 then
 "$PROG" --gamsav "$SPIELSTAENDE" --directory "$CSBBAS" --module "$SPIELVZ" --dungeon "$DUNGEON"
 else
  echo "Konnte kein Spielstandsverzeichnis „$SPIELSTAENDE“ anlegen! Rechteproblem in „$HOME”?"
  echo "Creating the saved game directory \"$SPIELSTAENDE\" has failed! User rights issue in \"$HOME\"?"
 fi
else
 echo "Datei nicht gefunden / File not found:"
 echo "„$DUNGEON“"
 echo "Groß-/Kleinschreibung prüfen. / Name in capital letters?"
fi
