ACLOCAL_AMFLAGS = -I m4

SUBDIRS	=	po \
					files \
					src

if HAVE_DOXYGEN
SUBDIRS += docs
endif

AM_CPPFLAGS			= @XML_CPPFLAGS@

AM_DISTCHECK_CONFIGURE_FLAGS	=	--with-debug --with-doc

if PLATFORM_LINUX
if ENABLE_DEBUG
CUSTOM_CXXFLAGS	= --coverage -fprofile-arcs -ftest-coverage
AM_LDFLAGS			= -fprofile-arcs -ftest-coverage
else
CUSTOM_CXXFLAGS	=
endif
else
CUSTOM_CXXFLAGS	=
endif

if PLATFORM_WIN32
CUSTOM_CXXFLAGS	+= @CXXFLAGS@ -march=i686 -mms-bitfields
else
if PLATFORM_WIN64
CUSTOM_CXXFLAGS	+= @CXXFLAGS@ -march=x86-64 -mms-bitfields
else
CUSTOM_CXXFLAGS	+= @CXXFLAGS@
endif
endif

if ENABLE_DEBUG
CUSTOM_CXXFLAGS	+= -O0 -g3 -ggdb3 -fno-omit-frame-pointer -pipe
else
CUSTOM_CXXFLAGS	+= -O2 -fomit-frame-pointer -pipe
endif


CUSTOM_CXXFLAGS	+= -Wall -Wextra -Winline -Wpacked -Wmissing-format-attribute -Wstrict-aliasing=2 -Wmissing-declarations -Wuninitialized -Wfloat-equal -Wno-comment -Wswitch-enum -Wswitch-default -Wshadow -Wformat=2 -Wformat-nonliteral -Wformat-security -Wswitch-enum -Winit-self -fstrict-overflow -Wstrict-overflow=1 -Wno-unused-parameter -Wno-error=deprecated-declarations -Wconversion -Wold-style-cast -Wno-overloaded-virtual -Wwrite-strings -Wcast-qual -Wcast-align -Wredundant-decls -Wlogical-op -Wpointer-arith -Weffc++ -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DG_DISABLE_DEPRECATED -fmax-errors=1

if PLATFORM_WIN32
CUSTOM_CXXFLAGS	+= -DLOCALEDIR=\""./locale"\" -DDATADIR=\""./files"\"
else
if PLATFORM_WIN64
CUSTOM_CXXFLAGS	+= -DLOCALEDIR=\""./locale"\" -DDATADIR=\""./files"\"
else
CUSTOM_CXXFLAGS += -DLOCALEDIR=\""@localedir@"\" -DDATADIR=\""@datadir@/2lgc"\" #-fsanitize=address
endif
endif

AM_CXXFLAGS 		= $(CUSTOM_CXXFLAGS) -Wno-error -D__STRICT_ANSI__

export AM_CXXFLAGS AM_CPPFLAGS

EXTRA_DIST	= config.rpath 	config.rpath \
							config.h

package_win :
if PLATFORM_WIN32

if ENABLE_DEBUG
PREFIX = ../comp32debug/libs
else
PREFIX = ../comp32/libs
endif

#Après PLATFORM_WIN32 on teste PLATFORM_WIN64
else

if PLATFORM_WIN64
if ENABLE_DEBUG
PREFIX = ../comp64debug/libs
else
PREFIX = ../comp64/libs
endif

#Après PLATFORM_WIN64 on échoue
else
	echo "package_win est réservé pour la compilation pour Windows."
	@false
endif
endif

	rm -Rf codegui
	cp -R $(PREFIX) codegui
	cp $(prefix)/bin/{libeurocodes-0.dll,codegui.exe} codegui/
if ENABLE_GTK
	cp $(prefix)/bin/libeurocodes-gtk-0.dll codegui/
endif
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

checkcode :
	@(cd files/uml && $(MAKE) clean && $(MAKE) $@)
	
	$(MAKE) clean
	./configure --with-debug --prefix=/usr/local CFLAGS="" CXXFLAGS=""
	sed -i 's/-DPIC/-DPIC -Wl,--as-needed/g' libtool
	$(MAKE) -j9 &> l_gcc_sans_gtk.log
	
	$(MAKE) clean
	./configure --with-gtk --with-debug --prefix=/usr/local CFLAGS="" CXXFLAGS=""
	sed -i 's/-DPIC/-DPIC -Wl,--as-needed/g' libtool
	$(MAKE) -j9 &> l_gcc_avec_gtk.log
	
	$(MAKE) clean
	./configure --with-gtk --with-debug --prefix=/usr/local CFLAGS="" CXXFLAGS="" CC=clang CXX=clang++
	$(MAKE) -j9 &> l_clang.log
	
	$(MAKE) clean
	scan-build make -j9 &> l_clang_scan.log
	
	$(MAKE) clean
	./configure --with-gtk --with-debug --prefix=/usr/local CFLAGS="" CXXFLAGS=""
	sed -i 's/-DPIC/-DPIC -Wl,--as-needed/g' libtool
	../cov-analysis-*/bin/cov-build --dir cov-int make -j9
	tar czvf cov-2LGC.tgz cov-int
	
	$(MAKE) -C src/lib cppcheck
	../nsiqcppstyle/nsiqcppstyle . > coding_style.log

CLEANFILES =	test.xml 2lgc_code-*.tar.gz
