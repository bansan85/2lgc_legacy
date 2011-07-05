DEFAULT_INCLUDES	=	-I../../.. \
				-I../common

lib_LTLIBRARIES = libEF.la
libEF_la_LIBADD =  -lcholmod -lspqr -llist
libEF_la_SOURCES =	EF_noeud.c \
			EF_appui.c \
			EF_rigidite.c
libEF_ladir = $(includedir)/2lgc
libEF_la_HEADERS =	EF_noeud.h \
			EF_appui.h \
			EF_rigidite.h
