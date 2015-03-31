DEFAULT_INCLUDES	=	-I$(srcdir)/../lib \
										-I$(srcdir)/../.. \
										-I$(srcdir)/../libundo \
										-I$(srcdir)/../macro

bin_PROGRAMS			=	codegui
codegui_SOURCES		=	main.cpp
codegui_CPPFLAGS	=	@XML_CPPFLAGS@
codegui_LDADD			=	-lcholmod -lamd -lcolamd -lumfpack @BLAS_LIBS@ -lgfortran @XML_LIBS@ ../lib/libeurocodes.la ../libundo/libundo.la

if PLATFORM_LINUX
if ENABLE_DEBUG
codegui_LDADD    +=	-lgcov
endif
endif

codegui_LDFLAGS		=   $(AM_LDFLAGS) -Wl,-O0 -no-undefined

if !ENABLE_DEBUG
if !PLATFORM_LINUX
codegui_LDFLAGS	 +=  -mwindows
endif
endif #ENABLE_DEBUG

CLEANFILES	      =	*.gcno *.gcda
