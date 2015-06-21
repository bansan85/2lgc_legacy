gettextize -f --no-changelog && sed -i 's#po/Makefile.in po/Makefile.in#po/Makefile.in#g' configure.ac && sed -i 's/config.rpath  config.rpath/config.rpath/g' Makefile.am && cd po && patch < ../diff-po-Makevars.template.patch && cd .. && mv po/Makevars.template po/Makevars && rm -f po/POTFILES.in && find src/libeurocode -name "*.cpp" > po/POTFILES.in && find src/libundo -maxdepth 1 -name "*.cpp" >> po/POTFILES.in && autoheader && aclocal -I m4 && libtoolize && automake --add-missing --copy && autoconf
if [ "$CC" = "clang" ]; then
  ./configure --with-gui=none --with-debug --prefix=/usr/local CFLAGS="" CXXFLAGS="-fno-use-cxa-atexit" && sed -i 's/-DPIC/-DPIC -Wl,--as-needed/g' libtool && find . -name "Makefile" -exec sed -i "s#-I/usr#-isystem/usr#g" {} \; && \
  scan-build make && scan-build make check && scan-build make distcheck || exit 1
else
  ./configure --with-gui=none --with-debug --prefix=/usr/local CFLAGS="" CXXFLAGS="" && sed -i 's/-DPIC/-DPIC -Wl,--as-needed/g' libtool && find . -name "Makefile" -exec sed -i "s#-I/usr#-isystem/usr#g" {} \;
  cppcheck --inconclusive --enable=all src -I src/libprojet -I src/macro -I src/libeurocode /I src/libundo
  make && make check && make distcheck || exit 1
fi
