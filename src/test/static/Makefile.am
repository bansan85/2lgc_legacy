DEFAULT_INCLUDES	=	-I$(srcdir)/../../.. \
										-I$(srcdir)/../../lib \
										-I$(srcdir)/../../libundo \
										-I$(srcdir)/../../macro

TESTS	=	projet

AM_LDFLAGS      = ../../lib/libeurocodes.la ../../libundo/libundo.la

projet_SOURCES	=	projet.cpp

EXTRA_PROGRAMS	= $(TESTS)
EXTRA_DIST			= projet.cpp

CLEANFILES = *.xml projet *.gcno *.gcda
