SUBDIRS 					= scenarii \
										static \
										bugs

DEFAULT_INCLUDES	=	-I$(srcdir)/../.. \
										-I$(srcdir)/../lib \
										-I$(srcdir)/../libundo \
										-I$(srcdir)/../macro

TESTS							=	coverage

AM_LDFLAGS				= ../lib/libeurocodes.la ../libundo/libundo.la

coverage_SOURCES	=	coverage.cpp

EXTRA_PROGRAMS 		= $(TESTS)

CLEANFILES 				= *.xml coverage *.gcno *.gcda
