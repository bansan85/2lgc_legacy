DEFAULT_INCLUDES	=	-I../.. \
										-I../lib

TESTS	=	coverage

coverage_SOURCES	=	coverage.cpp
coverage_LDADD		=	../lib/libeurocodes.la
coverage_CXXFLAGS	=	$(AM_CXXFLAGS) --coverage
coverage_CPPFLAGS	=	$(AM_CPPFLAGS) @XML_CPPFLAGS@

EXTRA_PROGRAMS = $(TESTS)
