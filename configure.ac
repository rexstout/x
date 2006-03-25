# Process this file with autoconf to produce a configure script.
AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(xplanet, 1.0.0)
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

# my functions
AC_FIND_X11
AC_FIND_XSS
AC_FIND_GRAPHICS_LIBS
AC_FIND_FREETYPE

AC_USE_MACAQUA

separator="/"
use_ar='yes'

AC_CANONICAL_HOST
case "$host" in
     sparc-sun-*)
       if test "x$GXX" != "xyes"; then
# This is for Sun Workshop compiler
         use_ar='no'                                                          
	 xplanet_ARFLAGS="-xar -o"
	 xplanet_LDFLAGS="-xildoff"
       fi
     ;;
     mips-sgi-irix*)
# this is for the MIPSpro compilers                                                       
       if test "x$GXX" != "xyes"; then
         use_ar='no'                                                          
         xplanet_ARFLAGS="-ar -WR,-s,-u,-v -o"
       fi
     ;;
     *-cygwin*)
       AC_DEFINE(HAVE_CYGWIN,,Define if compiling under cygwin)
       have_cygwin='yes'
       xplanet_LDFLAGS="-static"
       separator="\\\\\\\\"
     ;;
     *-apple-darwin*)
     ;;
     i686-pc-linux-gnu)
     ;;
esac

AM_CONDITIONAL(HAVE_CYGWIN, test "$have_cygwin" = "yes")
AM_CONDITIONAL(USE_AR, test $use_ar = 'yes')
AC_SUBST(xplanet_ARFLAGS)
AC_SUBST(xplanet_LDFLAGS)

AC_SUBST(separator)

AC_ARG_WITH(map-extension, [  --with-map-extension=EXTENSION
                          use EXTENSION as default map extension [jpg]], mapext="$withval", mapext="jpg")
AC_SUBST(mapext)


AC_CHECK_FUNCS(unsetenv)
AC_CHECK_FUNCS(timegm)

AC_OUTPUT([xpDefines.h	\
	   Makefile	\
           src/Makefile \
           src/libannotate/Makefile \
           src/libdisplay/Makefile \
	   src/libimage/Makefile   \
	   src/libmultiple/Makefile   \
	   src/libplanet/Makefile  \
	   src/libplanet/libephemeris/Makefile \
	   src/libplanet/libmoons/Makefile \
	   src/libprojection/Makefile	   \
	   src/libsgp4sdp4/Makefile])
