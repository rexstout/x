noinst_LIBRARIES = libephemeris.a

AM_CPPFLAGS = -I@top_srcdir@/src

if USE_AR
  libephemeris_a_AR = $(AR) cru
else
  libephemeris_a_AR = $(CXX) @xplanet_ARFLAGS@
endif

EXTRA_libephemeris_a_SOURCES = driver.cpp
libephemeris_a_SOURCES = ephemeris.cpp pluto.cpp
