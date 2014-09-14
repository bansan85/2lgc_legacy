DEFAULT_INCLUDES	=	-I$(srcdir)/../../.. \
										-I$(srcdir)/../../lib

TESTS	=	projet

projet_SOURCES	=	projet.cpp
projet_LDADD		=	../../lib/libeurocodes.la

EXTRA_PROGRAMS	= $(TESTS)
EXTRA_DIST			= projet.cpp

CLEANFILES = *.xml projet *.gcno *.gcda
