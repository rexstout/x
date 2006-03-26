AC_DEFUN(AC_FIND_CSPICE,
[

AC_ARG_WITH(cspice,AC_HELP_STRING([--with-cspice],[Use JPL's SPICE toolkit (NO)]))

have_cspice='no'
if test "$with_cspice" = yes; then
  have_cspice_h='no'
  AC_CHECK_HEADER(SpiceUsr.h,have_cspice_h='yes',have_cspice_h='no')
  if test "$have_cspice_h" = 'yes'; then
    have_cspice_lib='no'
    AC_CHECK_LIB(cspice,furnsh_c,have_cspice_lib='yes',have_cspice_lib='no',-lm)
    if test "$have_cspice_lib" = 'yes'; then
      CSPICE_LIBS="-lcspice"
      AC_DEFINE(HAVE_CSPICE,,Define if you have CSPICE library)
      AC_SUBST(CSPICE_LIBS)
      have_cspice='yes'
    fi
  fi

  if test "$have_cspice" = 'no'; then
    AC_MSG_WARN(*** Xplanet will be built without SPICE support ***)
  fi
fi

AM_CONDITIONAL(HAVE_CSPICE, test "$have_cspice" = 'yes')
])

AC_DEFUN(AC_FIND_FREETYPE,
[

AC_ARG_WITH(freetype,AC_HELP_STRING([--with-freetype],[Enable Freetype support for TrueType fonts (YES)]))

have_freetype='no'
if test "$with_freetype" != 'no'; then
  AC_PATH_PROG(FREETYPE_CONFIG, freetype-config, no)
  if test "$FREETYPE_CONFIG" = no; then
    AC_MSG_WARN(*** Xplanet will be built without freetype support ***)
  else
    FREETYPE_CFLAGS="`$FREETYPE_CONFIG --cflags` -I`$FREETYPE_CONFIG --prefix`/include"
    FREETYPE_LIBS=`$FREETYPE_CONFIG --libs`
    AC_SUBST(FREETYPE_CFLAGS)
    AC_SUBST(FREETYPE_LIBS)
    AC_DEFINE(HAVE_LIBFREETYPE,,Define if you have freetype)
    have_freetype='yes'
  fi
fi

AM_CONDITIONAL(HAVE_LIBFREETYPE, test "$have_freetype" = 'yes')
])

AC_DEFUN(AC_FIND_PANGO,
[
AC_ARG_WITH(pango,AC_HELP_STRING([--with-pango],[Enable Pango (YES)]))

have_pangoft2='no'
if test "$with_pango" != 'no'; then
   AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
   if test "$PKG_CONFIG" = no; then
      AC_MSG_WARN(*** Xplanet will be built without pango support ***)
   else
      PKG_CHECK_MODULES([PANGOFT2], pangoft2 >= 1.2.0, have_pangoft2='yes', have_pangoft2='no')
      FREETYPE_CFLAGS="$FREETYPE_CFLAGS $PANGOFT2_CFLAGS"
      FREETYPE_LIBS="$FREETYPE_LIBS $PANGOFT2_LIBS"
      AC_SUBST(FREETYPE_CFLAGS)
      AC_SUBST(FREETYPE_LIBS)
      if test "$have_pangoft2" = 'yes'; then
        AC_DEFINE(HAVE_LIBPANGOFT2,,Define if you have pango with freetype 2)
      fi
   fi
fi

AM_CONDITIONAL(HAVE_LIBPANGOFT2, test "$have_pangoft2" = 'yes')
])

dnl Autoconf stuff to check for graphics libraries is adapted from 
dnl imagemagick's configure.in

AC_DEFUN(AC_FIND_GRAPHICS_LIBS,

[

GRAPHICS_LIBS=""

#
# Check for GIF
#
AC_ARG_WITH(gif,AC_HELP_STRING([--with-gif],[Enable GIF support (YES)]))

have_gif='no'
if test "$with_gif" != 'no'; then
    have_gif_header='no'
    AC_CHECK_HEADER(gif_lib.h,have_gif_header='yes',have_gif_header='no')
    if test "$have_gif_header" != 'no'; then
        have_libgif='no'
        AC_CHECK_LIB(gif, DGifOpenFileName,have_libgif='yes',have_libgif='no',$X_LIBS)

	if test "$have_libgif" != 'no'; then
            GRAPHICS_LIBS="$GRAPHICS_LIBS -lgif $X_LIBS"
	    AC_DEFINE(HAVE_LIBGIF,,Define if you have GIF library)
	    have_gif='yes'
	else
            AC_CHECK_LIB(ungif, DGifOpenFileName,have_libgif='yes',have_libgif='no',$X_LIBS)
            if test "$have_libgif" != 'no'; then
                GRAPHICS_LIBS="$GRAPHICS_LIBS -lungif"
                AC_DEFINE(HAVE_LIBGIF,,Define if you have GIF library)
	        have_gif='yes'
	    fi
	fi

    fi
    if test "$have_gif" != 'yes'; then
	AC_MSG_WARN(*** Native GIF support will not be included ***)
    fi
fi

#
# Check for JPEG
#
AC_ARG_WITH(jpeg,AC_HELP_STRING([--with-jpeg],[Enable JPEG support (YES)]))

have_jpeg='no'
if test "$with_jpeg" != 'no'; then
    have_jpeg_header='no'
    AC_CHECK_HEADER(jpeglib.h,have_jpeg_header='yes',have_jpeg_header='no')
    if test "$have_jpeg_header" != 'no'; then
	have_libjpeg='no'
        AC_CHECK_LIB(jpeg,jpeg_read_header,have_libjpeg='yes',have_libjpeg='no',)
	if test "$have_libjpeg" != 'no'; then
	    GRAPHICS_LIBS="$GRAPHICS_LIBS -ljpeg"
	    AC_DEFINE(HAVE_LIBJPEG,,Define if you have JPEG library)
            have_jpeg='yes'
  	fi
    fi
    if test "$have_jpeg" != 'yes'; then
	AC_MSG_WARN(*** Native JPEG support will not be included ***)
    fi
fi

#
# Check for PNG
#
AC_ARG_WITH(png,AC_HELP_STRING([--with-png],[Enable PNG support (YES)]))

have_png='no'
if test "$with_png" != 'no'; then
    have_png_header='no'
    AC_CHECK_HEADER(png.h,have_png_header='yes',have_png_header='no')
    if test "$have_png_header" != 'no'; then
	have_libpng='no'
	AC_CHECK_LIB(png,png_create_read_struct,have_libpng='yes',have_libpng='no',-lm -lz)
	if test "$have_libpng" != 'no'; then
	    GRAPHICS_LIBS="$GRAPHICS_LIBS -lpng -lm -lz"
	    AC_DEFINE(HAVE_LIBPNG,,Define if you have PNG library)
            have_png='yes'
  	fi
    fi
    if test "$have_png" != 'yes'; then
	AC_MSG_WARN(*** Native PNG support will not be included ***)
    fi
fi

#
# Check for PNM
#
AC_ARG_WITH(pnm,AC_HELP_STRING([--with-pnm],[Enable PNM support (YES)]))

have_pnm='no'
if test "$with_pnm" != 'no'; then
    have_pnm_header='no'
    AC_CHECK_HEADER(pnm.h,have_pnm_header='yes',have_pnm_header='no')
    if test "$have_pnm_header" != 'no'; then
	have_libpnm='no'
        AC_CHECK_LIB(netpbm,pnm_init,have_libpnm='yes',have_libpnm='no',)
	if test "$have_libpnm" != 'no'; then
	    GRAPHICS_LIBS="$GRAPHICS_LIBS -lnetpbm"
	    AC_DEFINE(HAVE_LIBPNM,,Define if you have PNM library)
            have_pnm='yes'
	else
            AC_CHECK_LIB(pnm,pnm_init,have_libpnm='yes',have_libpnm='no',)
	      if test "$have_libpnm" != 'no'; then
	         GRAPHICS_LIBS="$GRAPHICS_LIBS -lpnm -lpbm"
	         AC_DEFINE(HAVE_LIBPNM,,Define if you have PNM library)
	         have_pnm='yes'
	      fi
  	fi
    fi
    if test "$have_pnm" != 'yes'; then
	AC_MSG_WARN(*** Native PNM support will not be included ***)
    fi
fi

#
# Check for TIFF
#
AC_ARG_WITH(tiff,AC_HELP_STRING([--with-tiff],[Enable TIFF support (YES)]))

have_tiff='no'
if test "$with_tiff" != 'no'; then
    have_tiff_header='no'
    AC_CHECK_HEADER(tiff.h,have_tiff_header='yes',have_tiff_header='no')
    AC_CHECK_HEADER(tiffio.h,have_tiff_header='yes',have_tiff_header='no')
    if test "$have_tiff_header" != 'no'; then
	have_libtiff='no'
        AC_CHECK_LIB(tiff,_TIFFmalloc,have_libtiff='yes',have_libtiff='no',-lm)
	if test "$have_libtiff" != 'no'; then
	    GRAPHICS_LIBS="$GRAPHICS_LIBS -ltiff -lm"
	    AC_DEFINE(HAVE_LIBTIFF,,Define if you have TIFF library)
            have_tiff='yes'
  	fi
    fi
    if test "$have_tiff" != 'yes'; then
	AC_MSG_WARN(*** Native TIFF support will not be included ***)
    fi
fi

AM_CONDITIONAL(HAVE_LIBGIF, test "$have_gif" = 'yes')
AM_CONDITIONAL(HAVE_LIBJPEG, test "$have_jpeg" = 'yes')
AM_CONDITIONAL(HAVE_LIBPNG, test "$have_png" = 'yes')
AM_CONDITIONAL(HAVE_LIBPNM, test "$have_pnm" = 'yes')
AM_CONDITIONAL(HAVE_LIBTIFF, test "$have_tiff" = 'yes')

AC_SUBST(GRAPHICS_LIBS)

])

#
# Check for X11
#

AC_DEFUN(AC_FIND_X11,
[

have_libx11='no'
if test "$with_x" != 'no'; then

dnl Locate X include files and libraries
   AC_PATH_XTRA
   NEW_LIBS="$X_LIBS -lX11"

   AC_CHECK_LIB(X11, XOpenDisplay, have_libx11='yes',have_libx11='no',$X_LIBS)
   if test "$have_libx11" != 'no'; then
     AC_DEFINE(HAVE_LIBX11,,Define if you have X11 libraries)
     X_LIBS="$NEW_LIBS"
     CPPFLAGS="$X_CFLAGS $CPPFLAGS"
     LIBS="$X_LIBS $LIBS"
   else
     AC_MSG_WARN(*** Xplanet will be built without X11 support ***)
   fi
fi

AM_CONDITIONAL(HAVE_LIBX11, test "$have_libx11" = 'yes')

])

AC_DEFUN(AC_FIND_XSS,
[
dnl Check for XScreenSaver
AC_ARG_WITH(xscreensaver,AC_HELP_STRING([--with-xscreensaver],[compile with X screensaver extension (YES)]))

have_xss='no'
if test "$have_libx11" = 'yes' ; then
if test "$with_xscreensaver" != 'no' ; then	
    AC_CHECK_HEADERS([X11/Xlib.h])
    AC_CHECK_HEADERS([X11/extensions/scrnsaver.h], [have_xss=yes], [],
[#if HAVE_X11_XLIB_H
#include <X11/Xlib.h>
#endif
])
    if test "$have_xss" = "yes"; then
        AC_CHECK_LIB(Xext, XScreenSaverRegister,[XSS_LIBS="-lXext"],[have_xss=no],[-lX11 -lm])
	if test "$have_xss" = "no"; then
	  AC_CHECK_LIB(Xss, XScreenSaverRegister,[have_xss=yes; XSS_LIBS="-lXss -lXext"],[have_xss=no],[$X_LIBS -lX11 -lXext -lm])
	fi
	if test "$have_xss" = "yes"; then
	   AC_DEFINE(HAVE_XSS,,Define if you have X screensaver extension)
	   AC_SUBST(XSS_LIBS)
	fi
    fi
fi
fi
])

AC_DEFUN(AC_USE_MACAQUA,
[
AC_ARG_WITH(aqua,AC_HELP_STRING([--with-aqua],[For Mac OS X Aqua (NO)]))

if test "$with_aqua" = yes; then
  AC_DEFINE(HAVE_AQUA,,Define for Mac OS X)
  AQUA_LIBS="-framework Carbon -framework Cocoa -bind_at_load"
  AC_SUBST(AQUA_LIBS)

  OBJC="gcc"
  OBJCFLAGS="-Wno-import"
  AC_SUBST(OBJC)
  AC_SUBST(OBJCFLAGS)
fi

AM_CONDITIONAL(HAVE_AQUA, test "$with_aqua" = 'yes')
])
