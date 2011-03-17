DEFAULT_INCLUDES	=	-I../../.. \
				-I../1990 \
				-I.

lib_LTLIBRARIES = libcommon.la

libcommon_la_SOURCES =	common_text.c \
			common_erreurs.c \
			common_maths.c \
			common_projet.c

libcommon_ladir = $(includedir)/2lgc

libcommon_la_HEADERS =	common_text.h \
			common_erreurs.h \
			common_maths.h \
			common_projet.h

libcommon_la_LIBADD = ../1990/lib1990.la
