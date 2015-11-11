DEFAULT_INCLUDES			  =	-I$(srcdir) \
													-I$(srcdir)/../.. \
													-I$(srcdir)/../macro \
													-I$(srcdir)/../libprojet \
													-I$(srcdir)/../libundo

AUTOMAKE_OPTIONS			  =	subdir-objects

lib_LTLIBRARIES				  =	libeurocode.la

libeurocode_la_LIBADD		=	@XML_LIBS@ \
                          ../libundo/libundo.la
if PLATFORM_LINUX
if ENABLE_DEBUG
libeurocode_la_LIBADD	 +=	-lgcov
endif
endif
if PLATFORM_WIN32
libeurocode_la_LIBADD	 += -lintl
endif
if PLATFORM_WIN64
libeurocode_la_LIBADD	 += -lintl
endif

libeurocode_la_CXXFLAGS	= $(AM_CXXFLAGS)

libeurocode_la_LDFLAGS	= $(AM_LDFLAGS) -release $(VERSION) -Wl,-O0 -no-undefined

libeurocode_la_CPPFLAGS	=	@XML_CPPFLAGS@

libeurocode_ladir 			=	$(includedir)/2lgc

libeurocode_la_SOURCES	=	CParamEC.cpp

libeurocode_la_HEADERS	=	$(libeurocode_la_SOURCES:.cpp=.hpp)

$(libeurocode_la_SOURCES) : uml

uml :
	$(MAKE) -C $(top_builddir)/files/uml eurocode-uml

CLEANFILES = *.gcno *.gcda *.hpp

cppcheck :
	cppcheck --inconclusive --enable=all $(DEFAULT_INCLUDES) $(libeurocode_la_SOURCES) &> ../../cppcheck.log
	@sed 's/^.*scanf without field.*$$//g' -i ../../cppcheck.log
