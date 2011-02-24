DEFAULT_INCLUDES	=	-I../.. \
				-I../lib/1990 \
				-I../cmd/common

TESTS = 1990__1

1990__1_SOURCES =	1990__1.c \
			../cmd/common/cmd_text.c \
			../cmd/common/erreurs.c

EXTRA_PROGRAMS = 1990__1

1990__1_LDADD = ../lib/1990/lib1990.la

datadir		=       @datadir@
localedir       =       $(datadir)/locale
DEFS		=       -DLOCALEDIR=\"$(localedir)\" @DEFS@

