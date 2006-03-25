noinst_LIBRARIES = libannotate.a

AM_CPPFLAGS = @FREETYPE_CFLAGS@ -I@top_srcdir@/src

if USE_AR
  libannotate_a_AR = $(AR) cru
else
  libannotate_a_AR = $(CXX) @xplanet_ARFLAGS@
endif

libannotate_a_SOURCES = 	\
	Annotation.h 		\
	Annotation.cpp 		\
	addArcs.cpp		\
	addMarkers.cpp		\
	addSatellites.cpp	\
	arrangeMarkers.cpp	\
	drawArc.h		\
	drawArc.cpp		\
	drawCircle.h		\
	drawCircle.cpp		\
	Icon.h 			\
	Icon.cpp 		\
	LineSegment.h 		\
	LineSegment.cpp 	\
	Symbol.h 		\
	Symbol.cpp 		\
	Text.h 			\
	Text.cpp 		\
	libannotate.h
