noinst_LIBRARIES = libephemeris.a

AM_CPPFLAGS = -I@top_srcdir@/src

if USE_AR
  libephemeris_a_AR = $(AR) cru
else
  libephemeris_a_AR = $(CXX) @xplanet_ARFLAGS@
endif

libephemeris_a_SOURCES = \
			Ephemeris.cpp		\
			Ephemeris.h		\
			EphemerisHigh.cpp	\
			EphemerisHigh.h		\
			EphemerisLow.cpp	\
			EphemerisLow.h		\
			jpl_int.h		\
			jpleph.cpp		\
			jpleph.h		\
			pluto.cpp
