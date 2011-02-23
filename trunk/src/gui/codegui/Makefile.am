DEFAULT_INCLUDES	=	-I../../.. \
				-I../../cmd/common

bin_PROGRAMS=codegui
codegui_SOURCES =	main.c \
			../../cmd/common/cmd_text.c

# codegui_LDADD = ../../..b/libhello.la

datadir		=       @datadir@
localedir       =       $(datadir)/locale
DEFS		=       -DLOCALEDIR=\"$(localedir)\" @DEFS@

