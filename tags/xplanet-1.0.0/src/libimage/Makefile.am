noinst_LIBRARIES = libimage.a

if HAVE_LIBGIF
  gif = gif.c
endif

if HAVE_LIBJPEG
  jpeg = jpeg.c
endif

if HAVE_LIBPNG
  png = png.c
endif

if HAVE_LIBPNM
  pnm = pnm.c
endif

if HAVE_LIBTIFF
  tiff = tiff.c
endif

EXTRA_libimage_a_SOURCES = gif.c jpeg.c png.c pnm.c tiff.c

libimage_a_SOURCES = bmp.c Image.cpp Image.h $(gif) $(jpeg) $(png) $(pnm) $(tiff)
