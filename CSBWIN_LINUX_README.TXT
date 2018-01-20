1) Build            : ./autogen.sh
2) Install (AS ROOT): ./csbwin-linux-install.sh
3) Run (NOT AS ROOT): csbwin.sh $GAME

Prerequisite for installation:

Have a group and user "games".
If not, files and directory will belong to "root".
If yes, users in "games" should be able to install game packages without becoming "root" first.

Game packages are to be unzipped into: "/usr/local/share/CSBwin/$GAMENAME",
and MUST have a file "dungeon.dat" there.

If not, you must modify the start script.

For more instructions see the other files.
