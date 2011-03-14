DEFAULT_INCLUDES	=	-I../../.. \
				-I../../lib/common \
				-I../../lib/1990

if ENABLE_GTK
bin_PROGRAMS		= codegui
codegui_SOURCES		= main.c
codegui_CPPFLAGS	= @GTK_CFLAGS@
codegui_LDADD		= @GTK_LIBS@ ../../lib/common/libcommon.la ../../lib/1990/lib1990.la
endif

datadir		=       @datadir@
localedir       =       $(datadir)/locale
DEFS		=       -DLOCALEDIR=\"$(localedir)\" @DEFS@

