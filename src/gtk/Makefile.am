DEFAULT_INCLUDES	 =	-I$(builddir) \
											-I$(srcdir)/../.. \
											-I$(srcdir)/../libprojet \
											-I$(srcdir)/../libsouvenir \
											-I$(srcdir)/../macro

bin_PROGRAMS			 =	codegui
codegui_SOURCES		 =	main.cpp
codegui_CPPFLAGS	 =	@XML_CPPFLAGS@ @GTKMM_CFLAGS@
codegui_LDADD			 =	-lcholmod \
											-lamd \
											-lcolamd \
											-lumfpack \
											-lgfortran \
											@GTKMM_LIBS@ \
											@BLAS_LIBS@ \
											@XML_LIBS@ \
											../libprojet/libprojet.la \
											../libsouvenir/libsouvenir.la

if ENABLE_GTK
codegui_SOURCES   +=	gtk-ressources.cpp \
											GWindowMain.cpp
endif

$(codegui_SOURCES) : gtk-ressources.xml

if ENABLE_GTK
UI_FILES = $(wildcard ui/*.ui)
UI_H_FILES = $(UI_FILES:.ui=.ui.h)
UI_H_PRINT_FILES = $(UI_FILES:.ui=.ui.h.p)

gtk-ressources.cpp : $(UI_FILES) gtk-ressources.xml
	glib-compile-resources --target=gtk-ressources.cpp gtk-ressources.xml --c-name _2lgc --generate-source --manual-register
	sed -i "s/NULL/nullptr/g" gtk-ressources.cpp
#	glib-compile-resources --target=gtk-ressources.h --sourcedir=../../files/gtk gtk-ressources.xml --c-name _2lgc --generate-header --manual-register

gtk-ressources.xml : $(UI_H_FILES)
	@echo '<?xml version="1.0" encoding="UTF-8"?><gresources><gresource prefix="/org/2lgc/codegui">' > gtk-ressources.xml
	$(MAKE) uifiles
	@echo '</gresource></gresources>' >> gtk-ressources.xml

uifiles : $(UI_H_PRINT_FILES)

%.ui.h : %.ui
	intltool-extract --quiet --type=gettext/glade $^
	sed -i 's/N_(/gettext (/g' $@

%.ui.h.p : %.ui
	@echo "<file compressed=\"true\">$^</file>" >> gtk-ressources.xml

else
gtk-ressources.xml :
endif

if PLATFORM_LINUX
if ENABLE_DEBUG
codegui_LDADD    +=	-lgcov
endif
endif

codegui_LDFLAGS		=   $(AM_LDFLAGS) -Wl,-O0 -no-undefined

if !ENABLE_DEBUG
if !PLATFORM_LINUX
codegui_LDFLAGS	 +=  -mwindows
endif
endif #ENABLE_DEBUG

CLEANFILES	      =	*.gcno *.gcda *.hpp gtk-ressources.cpp gtk-ressources.xml ui/*.ui.h *.stamp

EXTRA_DIST				= gtk-ressources.h
