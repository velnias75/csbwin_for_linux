#!/bin/sh
# Run this to generate all the initial makefiles, etc.

# You cann add/delete swithes here which
# "macros/autogen.sh" and "configure" understand:
PARAM="--disable-dynwinsize --enable-release --enable-oldgtk"

# Name of the package, not really used anywhere...
PKG_NAME="CSBwin for Linux"

# Look if some important files are in the right place
# before we begin:
srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

(test -f $srcdir/configure.in \
  && test -d $srcdir/src/ \
  && test -f $srcdir/src/Code11f52.cpp) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level CSBwin directory."
    exit 1
}

touch stamp.h.in

# Let "macros/autogen.sh" not yet run "./configure",
# because "src/Makefile.in" needs to be copied/linked
# into the current directory before:
NOCONFIGURE="true"
export NOCONFIGURE

# Call "macros/autogen.sh" without "configure":
. $srcdir/macros/autogen.sh $PARAM $*

# Link "Makefile.in" from "src" to "here":
ln -sf ./src/Makefile.in ./Makefile.in

# Now run "./configure" with appropriate parameters:
./configure $PARAM $*

# Change the current directory to "src"...
cd src
# ... and call "make" from there to build "src/CSBwin"
make

echo "--==##==--"
if  [ -f CSBwin ]
then
 echo "If only warnings, and no fatal error occured,"
 echo "copy \"CSBwin\" to \"/usr/local/games\" or \"/usr/local/bin\" now,"
 echo "or wherever you keep your self-compiled programmes."
 echo "Start it with \"--savegame </path/to/your/saved_games>\""
 echo "and \"--root-path </path/to/game_files>\" (where \"dungeon.dat\" is.)"
 echo "Example:"
 echo "/usr/local/games/CSBwin --savegame ~/.csbwin --root-path /usr/local/share/games/csbwin/dungeonmaster"
else
 echo "Please check the error messages above,"
 echo "\"CSBwin\" has not been built correctly."
fi
