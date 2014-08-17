SUBDIRS = scenarii \
					static

DEFAULT_INCLUDES	=	-I../.. \
										-I../lib

TESTS	=	coverage



coverage_SOURCES	=	coverage.cpp
coverage_LDADD		=	../lib/libeurocodes.la

EXTRA_PROGRAMS = $(TESTS)

CLEANFILES = *.xml
