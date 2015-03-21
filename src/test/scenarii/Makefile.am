DEFAULT_INCLUDES	=	-I$(srcdir)/../../.. \
										-I$(srcdir)/../../lib \
										-I$(srcdir)/../../libundo \
										-I$(srcdir)/../../macro

TESTS	=	action1 \
				action2 \
				undomanager1 \
				undomanager2 \
				undomanager3

action1_SOURCES	=	action1.cpp
action1_LDADD		=	../../lib/libeurocodes.la ../../libundo/libundo.la

action2_SOURCES	=	action2.cpp
action2_LDADD		=	../../lib/libeurocodes.la ../../libundo/libundo.la

undomanager1_SOURCES	=	undomanager1.cpp
undomanager1_LDADD		=	../../lib/libeurocodes.la ../../libundo/libundo.la

undomanager2_SOURCES	=	undomanager2.cpp
undomanager2_LDADD		=	../../lib/libeurocodes.la ../../libundo/libundo.la

undomanager3_SOURCES	=	undomanager3.cpp
undomanager3_LDADD		=	../../lib/libeurocodes.la ../../libundo/libundo.la

EXTRA_PROGRAMS = $(TESTS)

CLEANFILES = *.xml undomanager1 undomanager2 action1 action2 *.gcno *.gcda
