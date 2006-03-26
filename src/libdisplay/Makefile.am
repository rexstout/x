noinst_LIBRARIES = libdisplay.a libtimer.a

if HAVE_AQUA
  displayaqua = DisplayMacAqua.cpp DisplayMacAqua.h DesktopPicture.m

%.o: %.m
	$(OBJC) $(OBJCFLAGS) -c $<
endif

if HAVE_CYGWIN
  displaymswin = DisplayMSWin.cpp DisplayMSWin.h
endif

if HAVE_LIBFREETYPE
  textrendererft2 = TextRendererFT2.cpp TextRendererFT2.h
endif

if HAVE_LIBPANGOFT2
  textrendererpangoft2 = TextRendererPangoFT2.cpp TextRendererPangoFT2.h
endif

if HAVE_LIBX11
  displayx11 = DisplayX11.cpp DisplayX11.h vroot.h 
  timerx11   = TimerX11.h TimerX11.cpp
endif

EXTRA_libdisplay_a_SOURCES = DisplayMacAqua.cpp DisplayMacAqua.h DisplayMSWin.cpp DisplayMSWin.h TextRendererFT2.cpp TextRendererFT2.h TextRendererPangoFT2.cpp TextRendererPangoFT2.h DisplayX11.cpp DisplayX11.h vroot.h TimerX11.h TimerX11.cpp

AM_CPPFLAGS = @X_CFLAGS@ @FREETYPE_CFLAGS@ -I@top_srcdir@/src

if USE_AR
  libdisplay_a_AR = $(AR) cru
  libtimer_a_AR = $(AR) cru
else
  libdisplay_a_AR = $(CXX) @xplanet_ARFLAGS@
  libtimer_a_AR = $(CXX) @xplanet_ARFLAGS@
endif

libdisplay_a_SOURCES = \
	DisplayBase.cpp  	\
	DisplayBase.h   	\
	DisplayOutput.h		\
	DisplayOutput.cpp	\
	getDisplay.cpp 		\
	libdisplay.h		\
	TextRenderer.cpp	\
	TextRenderer.h		\
	getTextRenderer.cpp	\
	$(displayx11)		\
	$(displayaqua)		\
	$(displaymswin)		\
	$(textrendererft2)	\
	$(textrendererpangoft2)

libtimer_a_SOURCES = \
	getTimer.cpp		\
	libtimer.h		\
	Timer.h			\
	Timer.cpp		\
	$(timerx11)