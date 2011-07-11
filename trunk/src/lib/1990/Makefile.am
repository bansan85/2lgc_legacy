DEFAULT_INCLUDES	=	-I../../.. \
				-I../common \
				-I../EF

lib_LTLIBRARIES = lib1990.la

if ENABLE_GTK
lib1990_la_LIBADD = @GTK_LIBS@ -llist -lcholmod
lib1990_la_CPPFLAGS = @GTK_CFLAGS@
else
lib1990_la_LIBADD = -llist -lcholmod
endif

lib1990_la_SOURCES =	1990_duree.c \
			1990_coef_psi.c \
			1990_combinaisons.c \
			1990_actions.c \
			1990_groupes.c \
			1990_ponderations.c \
			1990_gtk_groupes.c

lib1990_ladir = $(includedir)/2lgc
lib1990_la_HEADERS =	1990_duree.h \
			1990_coef_psi.h \
			1990_combinaisons.h \
			1990_actions.h \
			1990_groupes.h \
			1990_ponderations.h \
			1990_gtk_groupes.h
