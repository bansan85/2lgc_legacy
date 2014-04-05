DEFAULT_INCLUDES	=	-I../../.. \
						-I../../lib/common \
						-I../../lib/1990 \
						-I../../lib/EF \
						-I../../lib/1992_1_1 \
						-I../../lib/1993_1_1 \
						-I../../lib/gtk

if ENABLE_GTK
bin_PROGRAMS		=	codegui
codegui_SOURCES		=	main.c
codegui_CPPFLAGS	=	@GTK_CFLAGS@
codegui_LDADD		=	-lcholmod -lamd -lcolamd -lumfpack @BLAS_LIBS@ -lgfortran @XML_LIBS@ @GTK_LIBS@ \
						../../lib/libeurocodes.la \
						../../lib/libeurocodes-gtk.la
codegui_LDFLAGS		=   $(AM_LDFLAGS) -no-undefined
if !ENABLE_DEBUG
if !PLATFORM_LINUX
codegui_LDFLAGS		+=  -mwindows
endif
endif #ENABLE_DEBUG


endif #ENABLE_GTK
