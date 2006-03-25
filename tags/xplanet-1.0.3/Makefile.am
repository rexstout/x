SUBDIRS = src

man_MANS = xplanet.1

nobase_dist_data_DATA = \
	xplanet/rgb.txt			\
	xplanet/arcs/constellations	\
	xplanet/config/default		\
	xplanet/config/earth_markers	\
	xplanet/config/moon_orbit	\
	xplanet/config/overlay_clouds	\
	xplanet/ephemeris/README	\
	xplanet/fonts/README		\
	xplanet/fonts/FreeMonoBold.ttf	\
	xplanet/images/README		\
	xplanet/images/earth.jpg	\
	xplanet/images/night.jpg	\
	xplanet/images/hubble.png       \
	xplanet/images/shuttle.png      \
	xplanet/images/sublunar.png	\
	xplanet/images/iss.png          \
	xplanet/images/smile.png        \
	xplanet/images/subsolar.png	\
	xplanet/markers/brightStars	\
	xplanet/markers/earth		\
	xplanet/markers/mars		\
	xplanet/markers/moon		\
	xplanet/origin/README		\
	xplanet/origin/cassini		\
	xplanet/origin/galileo		\
	xplanet/satellites/iss		\
	xplanet/satellites/iss.tle	\
	xplanet/stars/BSC

sources = \
	scripts/convertCassini.perl.in	\
	scripts/convertCassini2.perl.in	

derived = \
	scripts/convertCassini.perl	\
	scripts/convertCassini2.perl

nobase_pkgdata_SCRIPTS = $(derived)

EXTRA_DIST = README.config xplanet.1 $(sources)
