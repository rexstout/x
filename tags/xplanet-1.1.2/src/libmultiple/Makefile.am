noinst_LIBRARIES = libmultiple.a

AM_CPPFLAGS = @FREETYPE_CFLAGS@ -I@top_srcdir@/src

if USE_AR
  libmultiple_a_AR = $(AR) cru
else
  libmultiple_a_AR = $(CXX) @xplanet_ARFLAGS@
endif

libmultiple_a_SOURCES = 	\
	libmultiple.h 		\
	addOrbits.cpp 		\
	drawSphere.cpp		\
	drawEllipsoid.cpp	\
	drawRings.cpp 		\
	drawStars.cpp 		\
	drawSun.cpp
