DEFAULT_INCLUDES	=	-I../../.. \
										-I../../lib

TESTS	=	action1 \
				undomanager1

action1_SOURCES	=	action1.cpp
action1_LDADD		=	../../lib/libeurocodes.la

undomanager1_SOURCES	=	undomanager1.cpp
undomanager1_LDADD		=	../../lib/libeurocodes.la

EXTRA_PROGRAMS = $(TESTS)

CLEANFILES = *.xml
