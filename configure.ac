dnl Process this file with autoconf to produce a configure script.

dnl Process this file with autoconf
AC_INIT([CSBwin_for_Linux],12.100)
AC_PREREQ(2.57)
AC_CONFIG_SRCDIR(src/Code222ea.cpp)
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([1.4 foreign dist-xz no-dist-gzip])

dnl AC_PREREQ(2.57)
dnl AC_INIT(src/Code222ea.cpp)
dnl AM_INIT_AUTOMAKE([CSBwin-for-Linux],12.100)
dnl AM_CONFIG_HEADER(config.h)




dnl Linux will require the following parameters to compile
dnl correctly when used with gcc/g++.
C_COMPILER_QUIRCS="-D_LINUX -fsigned-char"
CXX_COMPILER_QUIRCS="-D_LINUX -fpermissive"
#-fms-extensions



## DEBUG/RELEASE selector
AC_ARG_ENABLE( release, [ --enable-release Turn off debugging [default=disable]],, enable_release="no")
if test "$enable_release" = "yes" ;then
	CFLAGS="$CFLAGS -D_NDEBUG -DG_DISABLE_ASSERT -O2 $C_COMPILER_QUIRCS"
	CXXFLAGS="$CXXFLAGS -D_NDEBUG -DG_DISABLE_ASSERT -O2 $C_COMPILER_QUIRCS $CXX_COMPILER_QUIRCS"
else
	CFLAGS="$CFLAGS -g -Wall $C_COMPILER_QUIRCS"
	CXXFLAGS="$CXXFLAGS -g -Wall $C_COMPILER_QUIRCS $CXX_COMPILER_QUIRCS"
fi




## Compile with electric fence.
AC_ARG_ENABLE( efence,
[ --enable-efence Compile with electric fence [default=disable]],, enable_efence="no")
if test "$enable_efence" = "yes" ;then
	LINK_EFENCE="-lefence"
else
	LINK_EFENCE=""
fi
AC_SUBST(LINK_EFENCE)






AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL
AC_PROG_INSTALL
AC_HEADER_STDC







#AC_PATH_PROG(GLIB_CONFIG, glib-config, AC_ERROR("I need glib"))
PKG_CHECK_MODULES(GLIB, glib-2.0 >= 1.2.0 )


## Compile with old GTK for menus.
AC_ARG_ENABLE( oldgtk, [ --enable-oldgtk Use gtk+ v1.2 for menus. Note that gtk v2.0 is NOT supported. [default=disable]],, enable_oldgtk="no")
if test "$enable_oldgtk" = "yes" ;then
	PKG_CHECK_MODULES(GTK_CONFIG, gtk+ >= 1.2 )
	CFLAGS="$CFLAGS -DUSE_OLD_GTK"
	CXXFLAGS="$CXXFLAGS -DUSE_OLD_GTK"
fi

## Compile with paragui for menus

#AM_PATH_SDL(1.2.0,
#	CFLAGS="$CFLAGS $SDL_CFLAGS"; LIBS="$LIBS $SDL_LIBS",
#	AC_MSG_ERROR(SDL is not installed))





AC_PATH_PROG(SDL_CONFIG, sdl-config, AC_ERROR("I am in great need of SDL"))
#PKG_CHECK_MODULES(SDL_CONFIG, sdl >= 1.2.4 )

AC_ARG_ENABLE( esound, [ --enable-esound Enable sound support by esd (esound). [default=enable]],, enable_esound="yes")
if test "$enable_esound" = "yes" ;then
	PKG_CHECK_MODULES(ESD_CONFIG,esound >= 0.2 )
	CFLAGS="$CFLAGS -DSOUND__ESOUND"
	CXXFLAGS="$CXXFLAGS -DSOUND__ESOUND"
else
	AC_WARN(CSB will be built without SOUND support!)
fi

# PKG_CHECK_MODULES(CSL_CONFIG,csl >= 0.2 )





## Dynamic windowsize or fixed.
AC_ARG_ENABLE( dynwinsize,
[ --enable-dynwinsize Turn on dynamic winsize \(default and requires libHermes\). [default=enable]],, enable_dynwinsize="yes")
if test "$enable_dynwinsize" = "yes" ;then
	AC_CHECK_LIB(Hermes
		,Hermes_Init
		,HAVE_LIBHERMES=yes
		CFLAGS="$CFLAGS -D_DYN_WINSIZE"
		CXXFLAGS="$CXXFLAGS -D_DYN_WINSIZE"
		LINK_HERMES="-lHermes"
		,LINK_HERMES=""
		AC_WARN(No Hermes...)
		AC_ERROR(Hermes is needed to build with dynamic windowsize. Use \"--disable-dynwinsize\" to disable the need for Hermes.)
	)
else
	LINK_HERMES=""
fi
AC_SUBST(LINK_HERMES)


##AM_CONFIG_HEADER(config.h)
##AC_CONFIG_HEADERS([config.h:config.h.in])
AC_CONFIG_FILES([Makefile \
src/Makefile \
data/Makefile \
stamp.h])
##AC_OUTPUT([Makefile \
##src/Makefile \
##stamp.h])
AC_OUTPUT
