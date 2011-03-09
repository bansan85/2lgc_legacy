DEFAULT_INCLUDES	=	-I../../.. \
				-I../../lib/common

bin_PROGRAMS=codegui
codegui_SOURCES =	main.c
codegui_CPPFLAGS	= @GTK_CFLAGS@
codegui_LDADD		= @GTK_LIBS@ ../../lib/common/libcommon.la 

# codegui_LDADD = ../../..b/libhello.la

datadir		=       @datadir@
localedir       =       $(datadir)/locale
DEFS		=       -DLOCALEDIR=\"$(localedir)\" @DEFS@

