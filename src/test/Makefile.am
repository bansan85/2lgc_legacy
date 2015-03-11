SUBDIRS = scenarii \
					static \
					bugs

DEFAULT_INCLUDES	=	-I$(srcdir)/../.. \
										-I$(srcdir)/../lib \
										-I$(srcdir)/../libundo \
										-I$(srcdir)/../macro

TESTS	=	coverage

coverage_SOURCES	=	coverage.cpp
coverage_LDADD		=	../lib/libeurocodes.la ../libundo/libundo.la

EXTRA_PROGRAMS = $(TESTS)

CLEANFILES = *.xml coverage *.gcno *.gcda
