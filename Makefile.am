ACLOCAL_AMFLAGS = -I m4

SUBDIRS	=	po \
					files \
					src

if HAVE_DOXYGEN
SUBDIRS += docs
endif

AM_CPPFLAGS			= @XML_CPPFLAGS@

if ENABLE_GTK
AM_DISTCHECK_CONFIGURE_FLAGS	=	--with-gui=gtk3 --with-debug --with-doc
else
AM_DISTCHECK_CONFIGURE_FLAGS	=	--with-gui=none --with-debug --with-doc
endif

export AM_CPPFLAGS

EXTRA_DIST	= config.rpath  config.rpath \
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
	./configure --with-gui=gtk3 --with-debug --prefix=/usr/local CFLAGS="" CXXFLAGS=""
	$(MAKE) -j9 &> l_gcc_avec_gtk.log
	
	$(MAKE) clean
	./configure --with-gui=gtk3 --with-debug --prefix=/usr/local CFLAGS="-fsanitize=undefined" CXXFLAGS="-fsanitize=undefined"
	$(MAKE) -j9 &> l_gcc_undefined.log
	
	$(MAKE) clean
	./configure --with-gui=gtk3 --with-debug --prefix=/usr/local CFLAGS="-fsanitize=address" CXXFLAGS="-fsanitize=address"
	$(MAKE) -j9 &> l_gcc_address.log
	
	$(MAKE) clean
	./configure --with-gui=gtk3 --with-debug --prefix=/usr/local CFLAGS="" CXXFLAGS="" CC=clang CXX=clang++
	$(MAKE) -j9 &> l_clang.log
	
	$(MAKE) clean
	./configure --with-gui=gtk3 --with-debug --prefix=/usr/local CFLAGS="" CXXFLAGS=""
	sed -i 's/-DPIC/-DPIC -Wl,--as-needed/g' libtool
	../cov-analysis-*/bin/cov-build --dir cov-int make -j9 check
	tar czvf cov-2LGC.tgz cov-int
	
	$(MAKE) clean
	./configure --with-gui=gtk3 --with-debug --prefix=/usr/local CFLAGS="" CXXFLAGS=""
	MALLOC_CHECK_=3 MALLOC_PERTURB_=252 $(MAKE) -j9 &> l_gcc_avec_gtk.log
	
	$(MAKE) -C src/lib cppcheck
	../nsiqcppstyle/nsiqcppstyle . > coding_style.log

CLEANFILES =	test.xml 2lgc_code-*.tar.gz
