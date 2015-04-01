DEFAULT_INCLUDES				  =	-I$(srcdir) \
														-I$(srcdir)/../.. \
														-I$(srcdir)/../macro \
														-I$(srcdir)/../libundo

AUTOMAKE_OPTIONS				  =	subdir-objects

lib_LTLIBRARIES					  =	libeurocodes.la

libeurocodes_la_LIBADD	  =	@BLAS_LIBS@ -lgfortran @XML_LIBS@
if PLATFORM_LINUX
if ENABLE_DEBUG
libeurocodes_la_LIBADD   +=	-lgcov
endif
endif
if PLATFORM_WIN32
libeurocodes_la_LIBADD   += -lintl
endif
if PLATFORM_WIN64
libeurocodes_la_LIBADD   += -lintl
endif

libeurocodes_la_CXXFLAGS  = $(AM_CXXFLAGS)

if ENABLE_GTK
libeurocodes_la_CXXFLAGS += @GTKMM_CFLAGS@
libeurocodes_la_LIBADD   += @GTKMM_LIBS@
endif

libeurocodes_la_LDFLAGS		= $(AM_LDFLAGS) -release $(VERSION) -Wl,-O0 -no-undefined

libeurocodes_la_CPPFLAGS	=	@XML_CPPFLAGS@

libeurocodes_ladir 				=	$(includedir)/2lgc

libeurocodes_la_SOURCES 	=	CAction.cpp \
														CBarreInfoEF.cpp \
														CCalculs.cpp \
														CModele.cpp \
														NbCalcul.cpp \
														NbUser.cpp \
														CParamEC.cpp \
														CPonderations.cpp \
														CPreferences.cpp \
														CProjet.cpp \
														IActionGroupe.cpp \
														IGui.cpp \
														INb.cpp \
														IParametres.cpp

if ENABLE_GTK
libeurocodes_la_SOURCES  +=	gtk-ressources.cpp \
														GWindowMain.cpp
endif

libeurocodes_la_HEADERS 	=	$(wildcard *.hpp)

$(libeurocodes_la_SOURCES) : uml gtk-ressources.xml

uml :
	$(MAKE) -C ../../files/uml

CLEANFILES = *.gcno *.gcda *.hpp gtk-ressources.cpp gtk-ressources.xml ui/*.ui.h

if ENABLE_GTK
UI_FILES = $(wildcard ui/*.ui)
UI_H_FILES = $(UI_FILES:.ui=.ui.h)
UI_H_PRINT_FILES = $(UI_FILES:.ui=.ui.h.p)

gtk-ressources.cpp : $(UI_FILES) gtk-ressources.xml
	glib-compile-resources --target=gtk-ressources.cpp gtk-ressources.xml --c-name _2lgc --generate-source --manual-register
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

endif

cppcheck :
	cppcheck --inconclusive --enable=all $(DEFAULT_INCLUDES) $(libeurocodes_la_SOURCES) &> ../../cppcheck.log
	@sed 's/^.*scanf without field.*$$//g' -i ../../cppcheck.log
