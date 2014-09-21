SUBDIRS = scenarii \
					static \
					bugs

DEFAULT_INCLUDES	=	-I$(srcdir)/../.. \
										-I$(srcdir)/../lib

TESTS	=	coverage

coverage_SOURCES	=	coverage.cpp
coverage_LDADD		=	../lib/libeurocodes.la

EXTRA_PROGRAMS = $(TESTS)

CLEANFILES = *.xml coverage *.gcno *.gcda
