DEFAULT_INCLUDES	=	-I$(srcdir)/../../.. \
										-I$(srcdir)/../../libeurocode \
										-I$(srcdir)/../../libprojet \
										-I$(srcdir)/../../libundo \
										-I$(srcdir)/../../macro

TESTS	=	action1 \
				action2 \
				undomanager1 \
				undomanager2 \
				undomanager3 \
				undomanager4 \
				undomanager5

# pour md5sumfile.cpp
#LIBS += -lssl -lcrypto

AM_LDFLAGS			= ../../libundo/libundo.la ../../libprojet/libprojet.la

action1_SOURCES	=	action1.cpp

action2_SOURCES	=	action2.cpp

undomanager1_SOURCES	=	undomanager1.cpp

undomanager2_SOURCES	=	undomanager2.cpp

undomanager3_SOURCES	=	undomanager3.cpp

undomanager4_SOURCES	=	undomanager4.cpp

undomanager5_SOURCES	=	undomanager5.cpp

EXTRA_PROGRAMS = $(TESTS)

CLEANFILES = *.xml undomanager1 undomanager2 undomanager3 undomanager4 undomanager5 action1 action2 *.gcno *.gcda
