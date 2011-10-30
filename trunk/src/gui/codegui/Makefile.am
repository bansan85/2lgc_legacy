DEFAULT_INCLUDES	=	-I../../.. \
						-I../../lib/common \
						-I../../lib/1990 \
						-I../../lib/EF \
						-I../../lib/1992_1_1

if ENABLE_GTK
bin_PROGRAMS		=	codegui
codegui_SOURCES		=	main.cc
codegui_CPPFLAGS	=	@GTK_CFLAGS@
codegui_LDADD		=	-llist -lcholmod @BLAS_LIBS@ -lcolamd -lumfpack -lblas -lgfortran @GTK_LIBS@ \
						../../lib/libeurocodes.la
endif
