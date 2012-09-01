ACLOCAL_AMFLAGS = -I m4

SUBDIRS =	po \
			src \
			files \
			doc

#-Wconversion
CUSTOM_CFLAGS 	= @CFLAGS@ -march=native -O0 -g3 -ggdb3 -pipe -ftracer -fsched2-use-traces -Wl,-O0 -fno-inline -Wall -Wextra -Wfloat-equal -Wconversion -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DG_DISABLE_DEPRECATED -Wno-comment -Wswitch-enum -Wswitch-default -Wshadow -Werror -Wno-error=conversion -Wno-error=sign-conversion -Wformat=1 -Winit-self -fstrict-overflow -Wstrict-overflow=5 -DLOCALEDIR=\""@localedir@"\" -DDATADIR=\""@datadir@/2lgc"\" `pkg-config --cflags glib-2.0`
AM_CFLAGS 		= $(CUSTOM_CFLAGS) -Wdeclaration-after-statement
AM_CXXFLAGS		= $(CUSTOM_CFLAGS) -std=c++0x 
if ENABLE_GTK
AM_CFLAGS      += @GTK_CFLAGS@
AM_CXXFLAGS    += @GTK_CFLAGS@
endif

export AM_CFLAGS AM_CXXFLAGS

EXTRA_DIST = config.rpath
