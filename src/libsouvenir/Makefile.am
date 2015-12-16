DEFAULT_INCLUDES		 = -I$(srcdir) \
											 -I$(srcdir)/../.. \
											 -I$(srcdir)/../macro \
											 @XML_CPPFLAGS@

AUTOMAKE_OPTIONS		 = subdir-objects

lib_LTLIBRARIES			 = libsouvenir.la

libsouvenir_la_LIBADD		 = @XML_LIBS@
if PLATFORM_LINUX
if ENABLE_DEBUG
libsouvenir_la_LIBADD   += -lgcov
endif
endif
if PLATFORM_WIN32
libsouvenir_la_LIBADD   += -lintl
endif
if PLATFORM_WIN64
libsouvenir_la_LIBADD   += -lintl
endif

libsouvenir_la_LDFLAGS	 = $(AM_LDFLAGS) -release $(VERSION) -Wl,-O0 -no-undefined

libsouvenir_ladir 			 = $(includedir)/2lgc

libsouvenir_la_SOURCES 	 = IObserveur.cpp \
											 ISujet.cpp \
											 UndoManager.cpp \
											 POCO/UndoData.cpp

libsouvenir_la_HEADERS   = \
	EEvent.hpp \
	EUndoEtat.hpp \
	IObserveur.hpp \
	ISujet.hpp \
	POCO/UndoData.hpp \
	UndoManager.hpp

%.hpp :
	$(MAKE) -C $(top_builddir)/files/uml souvenir-uml

CLEANFILES = *.gcno POCO/*.gcno *.gcda $(libsouvenir_la_HEADERS)

checkcode :
	../../../deheader-1.2/deheader > deheader.log
	sed -i "s/^.*\"config\.h\".*$//g" deheader.log

cppcheck :
	cppcheck --inconclusive --enable=all $(DEFAULT_INCLUDES) $(libsouvenir_la_SOURCES) &> ../../cppcheck-souvenir.log
	@sed 's/^.*scanf without field.*$$//g' -i ../../cppcheck-souvenir.log
