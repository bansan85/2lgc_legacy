DEFAULT_INCLUDES	=	-I../../.. \
				-I../common \
				-I../1990 \
				-I../1992_1_1 \
				-I.

lib_LTLIBRARIES		=	libEF.la
libEF_la_LIBADD		=	-lcholmod -llist -lspqr
libEF_la_SOURCES	=	EF_noeud.c \
				EF_appui.c \
				EF_rigidite.c \
				EF_calculs.c
libEF_ladir		=	$(includedir)/2lgc
libEF_la_HEADERS	=	EF_noeud.h \
				EF_appui.h \
				EF_rigidite.h \
				EF_calculs.h
