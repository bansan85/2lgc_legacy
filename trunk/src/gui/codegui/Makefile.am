DEFAULT_INCLUDES	=	-I../../.. \
				-I../../lib/common \
				-I../../lib/1990 \
				-I../../lib/EF \
				-I../../lib/1992_1_1

if ENABLE_GTK
bin_PROGRAMS		= codegui
codegui_SOURCES		= main.c
codegui_CPPFLAGS	= @GTK_CFLAGS@
codegui_LDADD		=	@GTK_LIBS@ \
				../../lib/common/libcommon.la \
				../../lib/1990/lib1990.la \
				../../lib/1992_1_1/lib1992_1_1.la \
				../../lib/EF/libEF.la
endif
