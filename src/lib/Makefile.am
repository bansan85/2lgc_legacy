DEFAULT_INCLUDES					=		-I$(srcdir) \
															-I$(srcdir)/../.. \
															-I$(srcdir)/../macro \
															-I$(srcdir)/../libundo

AUTOMAKE_OPTIONS					= 	subdir-objects

lib_LTLIBRARIES						= 	libeurocodes.la

libeurocodes_la_LIBADD		=		@BLAS_LIBS@ -lgfortran @XML_LIBS@
if PLATFORM_LINUX
if ENABLE_DEBUG
libeurocodes_la_LIBADD    +=	-lgcov
endif
endif
if PLATFORM_WIN32
libeurocodes_la_LIBADD    +=  -lintl
endif
if PLATFORM_WIN64
libeurocodes_la_LIBADD    +=  -lintl
endif

if ENABLE_GTK
libeurocodes_la_CXXFLAGS =   @GTKMM_CFLAGS@
libeurocodes_la_LIBADD   +=  @GTKMM_LIBS@
endif

libeurocodes_la_LDFLAGS		= 	$(AM_LDFLAGS) -release $(VERSION) -Wl,-O0 -no-undefined

libeurocodes_la_CPPFLAGS	=		@XML_CPPFLAGS@
if ENABLE_GTK
libeurocodes_la_CPPFLAGS	+=	@GTK_CFLAGS@
endif

libeurocodes_ladir 				= 	$(includedir)/2lgc

libeurocodes_la_SOURCES 	=		CAction.cpp \
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
															INb.cpp \
															IParametres.cpp \
															../macro/SString.cpp

libeurocodes_la_HEADERS 	=		$(wildcard *.hpp *.hh)

$(libeurocodes_la_SOURCES) : 	uml

uml :
	$(MAKE) -C ../../files/uml

CLEANFILES = *.gcno *.gcda *.hpp gtk/ressources.cpp gtk/ressources.hpp

gtk/ressources.hpp: ../../files/ui/ressources.xml gtk/ressources.cpp
if ENABLE_GTK
	glib-compile-resources --target=$@ --sourcedir=../../files/ui ../../files/ui/ressources.xml --c-name _2lgc --generate-header --manual-register
else
	touch gtk/ressources.hpp
endif

gtk/ressources.cpp: ../../files/ui/ressources.xml $(wildcard ../../files/ui/*.ui)
if ENABLE_GTK
	$(MAKE) -C ../../files/ui
	glib-compile-resources --target=$@ --sourcedir=../../files/ui ../../files/ui/ressources.xml --c-name _2lgc --generate-source --manual-register
else
	touch gtk/ressources.cpp
endif

cppcheck :
	cppcheck --inconclusive --enable=all $(DEFAULT_INCLUDES) $(libeurocodes_la_SOURCES) &> ../../cppcheck.log
	@sed 's/^.*scanf without field.*$$//g' -i ../../cppcheck.log
