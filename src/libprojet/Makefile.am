DEFAULT_INCLUDES		  =	-I$(srcdir) \
												-I$(srcdir)/../.. \
												-I$(srcdir)/../macro \
												-I$(srcdir)/../libeurocode \
												-I$(srcdir)/../libundo

AUTOMAKE_OPTIONS		  =	subdir-objects

lib_LTLIBRARIES			  =	libprojet.la

libprojet_la_LIBADD	 	=	@XML_LIBS@ \
                        ../libundo/libundo.la \
												../libeurocode/libeurocode.la
if PLATFORM_LINUX
if ENABLE_DEBUG
libprojet_la_LIBADD	 +=	-lgcov
endif
endif
if PLATFORM_WIN32
libprojet_la_LIBADD	 += -lintl
endif
if PLATFORM_WIN64
libprojet_la_LIBADD	 += -lintl
endif

libprojet_la_CXXFLAGS = $(AM_CXXFLAGS)

libprojet_la_LDFLAGS	= $(AM_LDFLAGS) -release $(VERSION) -Wl,-O0 -no-undefined

libprojet_la_CPPFLAGS	=	@XML_CPPFLAGS@

libprojet_ladir 			=	$(includedir)/2lgc

libprojet_la_SOURCES 	=	CAction.cpp \
												CBarreInfoEF.cpp \
												CCalculs.cpp \
												CModele.cpp \
												NbCalcul.cpp \
												NbUser.cpp \
												CPonderations.cpp \
												CPreferences.cpp \
												CProjet.cpp \
												IActionGroupe.cpp \
												INb.cpp \
												IParametres.cpp

libprojet_la_HEADERS 	=	$(wildcard *.hpp)

$(libprojet_la_SOURCES) : uml

uml :
	$(MAKE) -C ../../files/uml

CLEANFILES = *.gcno *.gcda *.hpp

cppcheck :
	cppcheck --inconclusive --enable=all $(DEFAULT_INCLUDES) $(libprojet_la_SOURCES) &> ../../cppcheck.log
	@sed 's/^.*scanf without field.*$$//g' -i ../../cppcheck.log
