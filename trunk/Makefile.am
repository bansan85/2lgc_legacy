ACLOCAL_AMFLAGS = -I m4

SUBDIRS =	po \
			src \
			files \
			doc

#-Wconversion
CUSTOM_CFLAGS 	= @CFLAGS@ -march=native -O0 -g3 -ggdb3 -pipe -ftracer -fsched2-use-traces -Wl,-O0 -fno-inline -Wall -Wextra -Wfloat-equal -Wconversion -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DG_DISABLE_DEPRECATED -Wno-comment -Wswitch-enum -Wswitch-default -Wshadow -Werror -Wno-error=conversion -Wno-error=sign-conversion -Wformat=1 -Winit-self -fstrict-overflow -Wstrict-overflow=5 `pkg-config --cflags glib-2.0`

if PLATFORM_WIN32
CUSTOM_CFLAGS	+= -DLOCALEDIR=\""./locale"\" -DDATADIR=\""./files"\"
else
CUSTOM_CFLAGS 	+= -DLOCALEDIR=\""@localedir@"\" -DDATADIR=\""@datadir@/2lgc"\"
endif

AM_CFLAGS 		= $(CUSTOM_CFLAGS) -Wdeclaration-after-statement
AM_CXXFLAGS		= $(CUSTOM_CFLAGS) -std=c++0x 
if ENABLE_GTK
AM_CFLAGS      += @GTK_CFLAGS@
AM_CXXFLAGS    += @GTK_CFLAGS@
endif

export AM_CFLAGS AM_CXXFLAGS

EXTRA_DIST = config.rpath

package_win :
if PLATFORM_WIN32
	mkdir -p codegui
	mkdir -p codegui/files
	mkdir -p codegui/files/ui
	cp $(prefix)/bin/{codegui.exe,libeurocodes-0.dll} codegui/
	cp /mingw/bin/lib{gcc_s_dw2-1,stdc++-6,cairo-2,fontconfig-1,freetype-6,xml2-2,iconv-2,pixman-1-0,png15-15,ffi-6,gdk-3-0,cairo-gobject-2,glib-2.0-0,intl-8,gobject-2.0-0,gdk_pixbuf-2.0-0,gio-2.0-0,gmodule-2.0-0,pango-1.0-0,pangocairo-1.0-0,pangoft2-1.0-0,harfbuzz-0,pangowin32-1.0-0,gtk-3-0,atk-1.0-0,M3d++-0,openblas,gfortran-3,quadmath-0}.dll codegui/
	cp files/ui/*.ui codegui/files/ui
	zip -r codegui_debug.zip codegui
	tar cvaf codegui_debug.tar.xz codegui
	strip codegui/*
	zip -r codegui.zip codegui
	tar cvaf codegui.tar.xz codegui
	rm -Rf codegui
endif
