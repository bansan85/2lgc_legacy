DEFAULT_INCLUDES	=	-I../../.. \
				-I../1990 \
				-I.

lib_LTLIBRARIES = libcommon.la

libcommon_la_SOURCES =	common_text.c \
			common_projet.c \
			common_tooltip.c

libcommon_ladir = $(includedir)/2lgc

libcommon_la_HEADERS =	common_text.h \
			common_erreurs.h \
			common_maths.h \
			common_projet.h \
			common_tooltip.h

libcommon_la_LIBADD = ../1990/lib1990.la @XML_LIBS@
libcommon_la_CPPFLAGS=@XML_CPPFLAGS@

if ENABLE_GTK
libcommon_la_LIBADD += @GTK_LIBS@
libcommon_la_CPPFLAGS += @GTK_CFLAGS@
endif
