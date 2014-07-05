DEFAULT_INCLUDES	=	-I../../.. \
										-I../../lib

bin_PROGRAMS			=	codegui
codegui_SOURCES		=	main.cpp
codegui_CXXFLAGS	=	$(AM_CXXFLAGS)
codegui_CPPFLAGS	=	@XML_CPPFLAGS@
codegui_LDADD			=	-lcholmod -lamd -lcolamd -lumfpack @BLAS_LIBS@ -lgfortran @XML_LIBS@ ../../lib/libeurocodes.la

if ENABLE_GTK
codegui_CXXFLAGS	+=	@GTK_CFLAGS@
codegui_LDADD			+=	@GTK_LIBS@ ../../lib/libeurocodes-gtk.la
endif
codegui_LDFLAGS		=   $(AM_LDFLAGS) -no-undefined

if !ENABLE_DEBUG
if !PLATFORM_LINUX
codegui_LDFLAGS		+=  -mwindows
endif
endif #ENABLE_DEBUG
