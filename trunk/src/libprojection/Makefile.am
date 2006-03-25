noinst_LIBRARIES = libprojection.a

AM_CPPFLAGS = -I@top_srcdir@/src

if USE_AR
  libprojection_a_AR = $(AR) cru
else
  libprojection_a_AR = $(CXX) @xplanet_ARFLAGS@
endif

libprojection_a_SOURCES = 	\
		ProjectionAncient.cpp		\
		ProjectionAncient.h		\
		ProjectionAzimuthal.cpp		\
		ProjectionAzimuthal.h		\
		ProjectionBase.cpp		\
		ProjectionBase.h		\
		ProjectionHemisphere.cpp	\
		ProjectionHemisphere.h		\
		ProjectionLambert.cpp		\
		ProjectionLambert.h		\
		ProjectionMercator.cpp		\
		ProjectionMercator.h		\
		ProjectionMollweide.cpp		\
		ProjectionMollweide.h		\
		ProjectionOrthographic.cpp	\
		ProjectionOrthographic.h	\
		ProjectionPeters.cpp		\
		ProjectionPeters.h		\
		ProjectionRectangular.cpp	\
		ProjectionRectangular.h		\
		getProjection.cpp		\
		libprojection.h