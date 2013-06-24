ACLOCAL_AMFLAGS = -I m4

SUBDIRS =	po \
			src \
			files \
			doc

#-Wconversion
if PLATFORM_WIN32
CUSTOM_CFLAGS 	= @CFLAGS@ -march=i686
else
CUSTOM_CFLAGS 	= @CFLAGS@ -march=native
endif

if ENABLE_DEBUG
CUSTOM_CFLAGS	+= -O0 -g3 -ggdb3 -fno-omit-frame-pointer -pipe
else
CUSTOM_CFLAGS	+= -O2 -fomit-frame-pointer -pipe
endif


CUSTOM_CFLAGS 	+= -ftracer -fsched2-use-traces -Wl,-O0 -fno-inline -Wall -Wextra -Wuninitialized -Wmaybe-uninitialized -Wfloat-equal -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DG_DISABLE_DEPRECATED -Wno-comment -Wswitch-enum -Wswitch-default -Wshadow -Werror -Wno-error=conversion -Wno-error=sign-conversion -Wformat=1 -Winit-self -fstrict-overflow -Wstrict-overflow=1 -Wno-unused-parameter -ftrack-macro-expansion=0

if PLATFORM_WIN32
CUSTOM_CFLAGS	+= -DLOCALEDIR=\""./locale"\" -DDATADIR=\""./files"\"
else
CUSTOM_CFLAGS 	+= -DLOCALEDIR=\""@localedir@"\" -DDATADIR=\""@datadir@/2lgc"\"
endif

AM_CFLAGS 		= $(CUSTOM_CFLAGS) -Wdeclaration-after-statement -std=gnu11
AM_CXXFLAGS		= $(CUSTOM_CFLAGS) -Wold-style-cast -std=gnu++11
if ENABLE_GTK
AM_CFLAGS      += @GTK_CFLAGS@
AM_CXXFLAGS    += @GTK_CFLAGS@
endif

export AM_CFLAGS AM_CXXFLAGS

EXTRA_DIST = config.rpath

package_win :
if PLATFORM_WIN32

if ENABLE_DEBUG
PREFIX = ../comp/libs32debug
else
PREFIX = ../comp/libs32
endif

#Après PLATFORM_WIN32 on teste PLATFORM_WIN64
else

if PLATFORM_WIN64
if ENABLE_DEBUG
PREFIX = ../comp/libs64debug
else
PREFIX = ../comp/libs64
endif

#Après PLATFORM_WIN64 on échoue
else
	echo "package_win est réservé pour la compilation pour Windows."
	@false
endif
endif

	rm -Rf codegui
	cp -R $(PREFIX) codegui
	cp $(prefix)/bin/{codegui.exe,libeurocodes-0.dll} codegui/
	cp files/xml/*.xml codegui/files
	cp files/formules/*.svg codegui/files
	cp files/csv/*.csv codegui/files
if PLATFORM_WIN32

if ENABLE_DEBUG
	zip -r codegui_debug_win32.zip codegui
	tar cvaf codegui_debug_win32.tar.xz codegui
else
	strip -s codegui/{*.exe,*.dll} codegui/lib/gdk-pixbuf-2.0/2.10.0/loaders/*.dll
	zip -r codegui_win32.zip codegui
	tar cvaf codegui_win32.tar.xz codegui
endif

else
if PLATFORM_WIN64

if ENABLE_DEBUG
	zip -r codegui_debug_win64.zip codegui
	tar cvaf codegui_debug_win64.tar.xz codegui
else
	strip -s codegui/{*.exe,*.dll} codegui/lib/gdk-pixbuf-2.0/2.10.0/loaders/*.dll
	zip -r codegui_win64.zip codegui
	tar cvaf codegui_win64.tar.xz codegui
endif

#Après PLATFORM_WIN64 on échoue
else
	@false
endif

endif
	rm -Rf codegui
