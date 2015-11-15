SUBDIRS 					= scenarii \
										static \
										bugs

DEFAULT_INCLUDES	=	-I$(srcdir)/../.. \
										-I$(srcdir)/../libeurocode \
										-I$(srcdir)/../libprojet \
										-I$(srcdir)/../libundo \
										-I$(srcdir)/../macro

TESTS							=	coverage

AM_LDFLAGS				= ../libundo/libundo.la ../libprojet/libprojet.la

coverage_SOURCES	=	coverage.cpp

EXTRA_PROGRAMS 		= $(TESTS)

CLEANFILES 				= *.xml coverage *.gcno *.gcda
