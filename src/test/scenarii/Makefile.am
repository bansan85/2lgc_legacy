DEFAULT_INCLUDES	=	-I$(srcdir)/../../.. \
										-I$(srcdir)/../../lib

TESTS	=	action1 \
				undomanager1 \
				undomanager2

action1_SOURCES	=	action1.cpp
action1_LDADD		=	../../lib/libeurocodes.la

undomanager1_SOURCES	=	undomanager1.cpp
undomanager1_LDADD		=	../../lib/libeurocodes.la

undomanager2_SOURCES	=	undomanager2.cpp
undomanager2_LDADD		=	../../lib/libeurocodes.la

EXTRA_PROGRAMS = $(TESTS)

CLEANFILES = *.xml undomanager1 undomanager2 action1 *.gcno *.gcda
