# src/Makefile.am
# Is read by configure

EXTRA_DIST = CSBwin.bmp

pkgdata_DATA = $(EXTRA_DIST)

noinst_LTLIBRARIES = libCSBwin.la libCSBplatform.la

bin_PROGRAMS = CSBwin
#Chaos_strikes_back_for_linux
##some_name_LDADD=$(top_builddir)/src/gtksdl-0.2/libgtksdl.la

#
# FIXME: Platform specific files should go here... but I don't know if anything but linux really use this.
#
# Platform=Linux
PLATFORM_SPECIFIC_BIN = CSBlinux.cpp
PLATFORM_SPECIFIC_LIB = LinScreen.cpp LinCSBUI.cpp
#Platform = Macintosh
#PLATFORM_SPECIFIC = MacScreen.cpp
#Platform = Windows
#PLATFORM_SPECIFIC = WinScreen.cpp CSBUI.cpp


AM_CPPFLAGS = \
	-I. \
	@GTK_CONFIG_CFLAGS@ \
	`$(SDL_CONFIG) --cflags` \
	@ESD_CONFIG_CFLAGS@ \
	@GLIB_CFLAGS@ \
	-Wall -Wextra -Wno-switch -Wno-parentheses -Wno-unused

LIBS = \
	@LINK_EFENCE@ \
	@LINK_HERMES@ \
	@GTK_CONFIG_LIBS@ \
	`$(SDL_CONFIG) --libs` \
	@ESD_CONFIG_LIBS@ \
	@GLIB_LIBS@

CSBwin_headers = \
	CSB.h \
	Data.h \
	Dispatch.h \
	Objects.h \
	stdafx.h \
	UI.h \
	resource.h \
	CSBTypes.h \
	LinuxFileName.inl \
	Recording.h \
	Prefix.h \
	sndhead.h
#	Transition.h

AM_CXXFLAGS = -ffast-math -finline-functions-called-once -fno-inline-small-functions
AM_CFLAGS = -ffast-math -finline-functions-called-once -fno-inline-small-functions

libCSBwin_la_CXXFLAGS = $(AM_CXXFLAGS) -ffunction-sections -fdata-sections
libCSBwin_la_CFLAGS = $(AM_CFLAGS) -ffunction-sections -fdata-sections
libCSBwin_la_SOURCES = \
	$(CSBwin_headers) \
	CSBUI.cpp \
	Statistics.cpp \
	AsciiDump.cpp \
	Bitmaps.cpp \
	Attack.cpp \
	CSBCode.cpp \
	Chaos.cpp \
	Character.cpp \
	Code11f52.cpp \
	Code13ea4.cpp \
	Code17818.cpp \
	Code1f9e6.cpp \
	Code222ea.cpp \
	Code390e.cpp \
	Code51a4.cpp \
	Codea59a.cpp \
	Graphics.cpp \
	Hint.cpp \
	Magic.cpp \
	Menu.cpp \
	Monster.cpp \
	Mouse.cpp \
	NewBugs.cpp \
	OldBugs.cpp \
	SaveGame.cpp \
	SmartDiscard.cpp \
	Sound.cpp \
	Stdafx.cpp \
	Timer.cpp \
	VBL.cpp \
	Viewport.cpp \
	data.cpp \
	system.cpp \
	utility.cpp \
	DSA.cpp \
	RC4.cpp \
	md5C.cpp \
	MoveObject.cpp \
	Recording.cpp \
	Translation.cpp

libCSBplatform_la_CXXFLAGS = $(AM_CXXFLAGS) -ffunction-sections -fdata-sections
libCSBplatform_la_CFLAGS = $(AM_CFLAGS) -ffunction-sections -fdata-sections
libCSBplatform_la_SOURCES = \
	$(PLATFORM_SPECIFIC_LIB)

CSBwin_CPPFLAGS = $(AM_CPPFLAGS) -DCSB_ICON=\"$(DESTDIR)/$(pkgdatadir)/CSBwin.bmp\"
CSBwin_SOURCES = \
	$(PLATFORM_SPECIFIC_BIN)

CSBwin_LDFLAGS = $(AM_LDFLAGS) -Wl,--gc-sections
CSBwin_LDADD = libCSBplatform.la libCSBwin.la

#	Swoosh.c
#	Transition.c

