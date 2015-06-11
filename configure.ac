#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(2lgc_code, 0.0.1, devel@2lgc.org, 2lgc_code, http://2lgc.org)
AM_INIT_AUTOMAKE([-Wno-portability])
AC_CONFIG_SRCDIR([src/lib/CProjet.cpp])
AC_CONFIG_HEADERS([config.h])
AM_GNU_GETTEXT([external])
AC_CONFIG_MACRO_DIR([m4])
AH_BOTTOM([
#include "src/macro/common.h"
])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AM_PROG_CC_C_O
AC_PROG_F77
AC_PROG_LIBTOOL

sinclude(acinclude.m4)

AC_LANG(C++)

for NEW_FLAG in \
    -fmax-errors=10 \
    -fstack-protector-all \
    -fstrict-overflow \
    -Wall \
    -Wcast-align \
    -Wcast-qual \
    -Wconditionally-supported \
    -Wconversion \
    -Wextra \
    -Wfloat-equal \
    -Wformat=2 \
    -Wformat-nonliteral \
    -Wformat-security \
    -Winit-self \
    -Winline \
    -Wlogical-op \
    -Wmissing-format-attribute \
    -Wno-comment \
    -Wno-error=deprecated-declarations \
    -Wno-overloaded-virtual \
    -Wno-unused-parameter \
    -Wold-style-cast \
    -Wpacked \
    -Wpedantic \
    -Wpointer-arith \
    -Wredundant-decls \
    -Wshadow \
    -Wsign-conversion \
    -Wstrict-aliasing=1 \
    -Wstrict-overflow=5 \
    -Wswitch-default \
    -Wswitch-enum \
    -Wtrampolines \
    -Wundef \
    -Wuninitialized \
    -Wunused-parameter \
    -Wuseless-cast \
    -Wwrite-strings \
    -Wzero-as-null-pointer-constant \
    -fabi-version=6 \
    -Wabi \
    -Wctor-dtor-privacy \
    -Wc++11-compat \
    -Weffc++ \
    -Wstrict-null-sentinel \
    -Woverloaded-virtual
do
  AC_MSG_CHECKING([if $CXX accepts $NEW_FLAG])
  OLD_CFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS $NEW_FLAG -Werror"
  AC_COMPILE_IFELSE([AC_LANG_SOURCE(
    [void foo(void); void foo(void) { }])], [
    AM_CXXFLAGS="$AM_CXXFLAGS $NEW_FLAG"
    AC_MSG_RESULT([yes])
  ], [
    AC_MSG_RESULT([no])
  ])
  CXXFLAGS="$OLD_CFLAGS"
done

#Pour clang
old_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -Werror -Wno-unused-macros"
for NEW_FLAG in \
    -Weverything \
    -Wno-c++98-compat \
    -Wno-c++98-compat-pedantic \
    -Wno-reserved-id-macro \
    -Wno-documentation-unknown-command \
    -Wno-documentation \
    -Wno-padded \
    -Wno-unreachable-code-break \
    -Wno-covered-switch-default \
    -Wno-exit-time-destructors \
    -Wno-global-constructors
do
  AC_MSG_CHECKING([if $CXX accepts $NEW_FLAG])
  OLD_CFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS $NEW_FLAG -Werror"
  AC_COMPILE_IFELSE([AC_LANG_SOURCE(
    [void foo(void); void foo(void) { }])], [
    AM_CXXFLAGS="$AM_CXXFLAGS $NEW_FLAG"
    AC_MSG_RESULT([yes])
  ], [
    AC_MSG_RESULT([no])
  ])
  CXXFLAGS="$OLD_CFLAGS"
done
CXXFLAGS="$old_CXXFLAGS"

#Vérification que l'utilisation de c++11 est possible
AC_MSG_CHECKING([if $CXX accepts -std=c++11])
OLD_CFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -std=c++11 -Werror"
AC_COMPILE_IFELSE([AC_LANG_SOURCE(
  [void foo(void); void foo(void) { }])], [
  AM_CXXFLAGS="$AM_CXXFLAGS -std=c++11"
  AC_MSG_RESULT([yes])
], [
  AC_MSG_RESULT([no])
  AC_MSG_CHECKING([if $CXX accepts -std=c++0x])
  CXXFLAGS="$OLD_CFLAGS -std=c++0x -Werror"
  AC_COMPILE_IFELSE([AC_LANG_SOURCE(
    [void foo(void); void foo(void) { }])], [
    AM_CXXFLAGS="$AM_CXXFLAGS -std=c++0x"
    AC_MSG_RESULT([yes])
  ], [
    AC_MSG_RESULT([no])
  ])
])
CXXFLAGS="$OLD_CFLAGS"

AC_MSG_CHECKING(de la récense de gcc...)
AC_PREPROC_IFELSE([AC_LANG_SOURCE([[#include <list>
                                    
                                    int main ()
                                    {
                                      std::list <int> test;
                                      
                                       for (int action : test) {}
                                      
                                      return 0;
                                    }
                                   ]])],
                  [AC_MSG_RESULT(ok)],
                  [AC_MSG_ERROR(g++ en version 4.6 minimum est nécessaire.)])

# libxml2
LIBXML_REQUIRED_VERSION=2.7.0
AC_SUBST(XML_CPPFLAGS)
AC_SUBST(XML_LIBS)
AM_PATH_XML2($LIBXML_REQUIRED_VERSION, :, AC_MSG_ERROR(Test for libXML failed.))

# debug
AC_ARG_WITH(debug,
            AS_HELP_STRING([--with-debug],
                           [build with debug support.]))
if test "$with_debug" != "yes"; then
  withdebug=false
else
  withdebug=true
fi
AC_MSG_RESULT([XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
with-debug $withdebug
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX])
AM_CONDITIONAL(ENABLE_DEBUG, test x$withdebug = xtrue)
if test x$withdebug = xtrue; then
AC_DEFINE(ENABLE_DEBUG, 1, [Défini si les options de débug doivent être activés])
fi

# Détermination de la version de Windows que l'on est en train de compiler
AC_MSG_CHECKING([for some platform])
case "$host" in
  *-*64-mingw*|*-*64-cygwin*)
    platform_win=Win64
    ;;
  *-*-mingw*|*-*-cygwin*)
    platform_win=Win32
    ;;
  *-*-linux-androideabi|*-*-linux-gnueabihf)
    platform_win=Android
    ;;
  *)
    platform_win=Linux
    ;;
esac
AC_MSG_RESULT([$platform_win])
AM_CONDITIONAL(PLATFORM_WIN32, test "$platform_win" = "Win32")
AM_CONDITIONAL(PLATFORM_WIN64, test "$platform_win" = "Win64")
AM_CONDITIONAL(PLATFORM_ANDROID, test "$platform_win" = "Android")
AM_CONDITIONAL(PLATFORM_LINUX, test "$platform_win" = "Linux")

if test "$platform_win" == "Linux" && "$withdebug" == "true"; then
  old_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS="$CPPFLAGS -Werror"
  CC_CHECK_CFLAGS_APPEND([-fprofile-arcs -ftest-coverage])
  CPPFLAGS="$old_CPPFLAGS"
fi

if test "$platform_win" == "Win32"; then
  AM_CXXFLAGS="$AM_CXXFLAGS -march=i686 -mms-bitfields"
fi
if test "$platform_win" == "Win64"; then
  AM_CXXFLAGS="$AM_CXXFLAGS -march=i686 -mms-bitfields"
fi
if test "$withdebug" == "true"; then
  AM_CXXFLAGS="$AM_CXXFLAGS -O0 -g3 -ggdb3 -fno-omit-frame-pointer -pipe"
else
  AM_CXXFLAGS="$AM_CXXFLAGS -O2 -fomit-frame-pointer -pipe"
fi
AM_CXXFLAGS="$AM_CXXFLAGS -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DG_DISABLE_DEPRECATED"
if test "$platform_win" == "Win32"; then
  AM_CXXFLAGS="$AM_CXXFLAGS -DLOCALEDIR=\\\"./locale\\\" -DDATADIR=\\\"./files\\\""
else
  if test "$platform_win" == "Win64"; then
    AM_CXXFLAGS="$AM_CXXFLAGS -DLOCALEDIR=\\\"./locale\\\" -DDATADIR=\\\"./files\\\""
  else
    AM_CXXFLAGS="$AM_CXXFLAGS -DLOCALEDIR=\\\""${localedir}"\\\" -DDATADIR=\\\""${datadir}"/2lgc\\\""
  fi
fi
AM_CXXFLAGS="$AM_CXXFLAGS -Wno-error -D__STRICT_ANSI__"

# GUI
AC_ARG_WITH(gui,
            AS_HELP_STRING([--with-gui=@<:@none/gtk3/win/metro@:>@],
                           [build with GUI support]))
if test "$with_gui" == "gtk3"; then
  if test "x$platform_win" == "xWin32" -o "x$platform_win" == "xWin64" -o "x$platform_win" == "xLinux"; then
    withgui=gtk3
    GLIB_REQUIRED_VERSION=2.38.0
    PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED_VERSION)
    GTK_REQUIRED_VERSION=3.6.0
    PKG_CHECK_MODULES(GTK, gtk+-3.0 >= $GTK_REQUIRED_VERSION)
    PKG_CHECK_MODULES(GLIBMM, glibmm-2.4 >= $GLIB_REQUIRED_VERSION)
    PKG_CHECK_MODULES(GTKMM, gtkmm-3.0 >= $GTK_REQUIRED_VERSION)
#    AC_CHECK_LIB([M3d++], [main], [], [AC_MSG_ERROR([M3d++ ( http://m3dlib.tuxfamily.org ) est necessaire])])
    AC_DEFINE(ENABLE_GTK, 1, [Défini si l’interface GTK doit être supportée.])
  else
    AC_MSG_ERROR([L’interface graphique sélectionnée via --with-gui n’est pas compatible.])
  fi
else
  if test "x$with_gui" == "xwin"; then
    if test "x$platform_win" == "xWin32" -o "x$platform_win" == "xWin64"; then
      withgui=win
      AC_DEFINE(ENABLE_WIN, 1, [Défini si l’interface Windows doit être supportée.])
    else
      AC_MSG_ERROR([L’interface graphique sélectionnée via --with-gui n’est pas compatible.])
    fi
  else
    if test "x$with_gui" == "xmetro"; then :
      if test "x$platform_win" == "xWin32" -o "x$platform_win" == "xWin64"; then :
        withgui=metro
        AC_DEFINE(ENABLE_METRO, 1, [Défini si l’interface METRO doit être supportée.])
      else
        AC_MSG_ERROR([L’interface graphique sélectionnée via --with-gui n’est pas compatible.])
      fi
    else
      withgui=none
    fi
  fi
fi
AC_MSG_RESULT([XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
with-gui $withgui
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX])

AM_CONDITIONAL(ENABLE_GTK, test x$withgui = xgtk3)
AM_CONDITIONAL(ENABLE_WIN, test x$withgui = xwin)
AM_CONDITIONAL(ENABLE_METRO, test x$withgui = xmetro)

AC_SUBST([AM_CXXFLAGS])

AC_CHECK_LIB([colamd], [colamd_recommended], [], [AC_MSG_ERROR([colamd ( www.cise.ufl.edu/research/sparse/colamd ) est necessaire])])
#AC_CHECK_LIB([suitesparseconfig], [SuiteSparse_time], [LIBS="-lsuitesparseconfig $LIBS"], [AC_CHECK_LIB([ufconfig], [UFfree], [LIBS="-lufconfig $LIBS"], [AC_MSG_ERROR([suitesparseconfig ( www.cise.ufl.edu/research/sparse ) est necessaire])])])
AC_CHECK_LIB([amd], [amd_malloc], [], [AC_MSG_ERROR([amd ( http://www.cise.ufl.edu/research/sparse/amd ) est necessaire])])
LIBS="-lgfortran $LIBS"
ACX_BLAS
if test x"$BLAS_LIBS" = x; then
    AC_MSG_ERROR([Cannot find blas libraries])
fi
AC_CHECK_LIB([cholmod], [cholmod_start], [], [AC_MSG_ERROR([cholmod ( http://www.cise.ufl.edu/research/sparse/cholmod ) est necessaire])])
AC_CHECK_LIB([umfpack], [main], [], [AC_MSG_ERROR([umfpack ( www.cise.ufl.edu/research/sparse/umfpack ) est necessaire])])


AM_GNU_GETTEXT([external])
ALL_LINGUAS="fr"
PKG_PROG_PKG_CONFIG

# Vérification de la possibilité de compiler la documentation via doxygen
AC_ARG_WITH(doc,
            AS_HELP_STRING([--with-doc],
                           [build with doc support]))
if test "$with_doc" != "yes"; then
  withdoc=false
else
  withdoc=true
fi
AC_MSG_RESULT([XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
with-doc $withdoc
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX])
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
  then AC_MSG_WARN([Doxygen introuvable - documentation non compilable])
fi
AM_CONDITIONAL(HAVE_DOXYGEN, test x$withdoc = xtrue && test -n "$DOXYGEN")

dnl Check for valgrind
TESTS_ENVIRONMENT=""
ac_enable_valgrind=no
AC_ARG_ENABLE(valgrind-testing,
     [  --enable-valgrind-testing     enable running of tests inside Valgrind],     [ ac_enable_valgrind=yes ], [ ac_enable_valgrind=no] )

if test "x${ac_enable_valgrind}" = xyes ; then
  if test "x${enable_shared}" = xyes ; then
    TESTS_ENVIRONMENT="libtool --mode=execute "
  fi

  AC_CHECK_PROG(HAVE_VALGRIND, valgrind, yes, no)
  if test "x$HAVE_VALGRIND" = xyes ; then
    TESTS_ENVIRONMENT="$TESTS_ENVIRONMENT valgrind --show-reachable=yes --leak-check=full --show-leak-kinds=all --trace-children=yes"
    AC_SUBST(TESTS_ENVIRONMENT)
  fi
fi

# Checks for libraries.
AC_CHECK_LIB(m, sqrt)

# Checks for header files.
AC_CHECK_HEADERS([libintl.h locale.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([localeconv memset modf pow setlocale sqrt strchr strstr])

AC_CONFIG_FILES([Makefile po/Makefile.in
                 docs/Makefile
                 files/Makefile
                 files/desktop/Makefile
                 files/ui/Makefile
                 files/uml/Makefile
                 src/Makefile
                 src/gtk/Makefile
                 src/gtk/ui/Makefile
                 src/lib/Makefile
                 src/libundo/Makefile
                 docs/Doxyfile
                 src/test/Makefile
                 src/test/bugs/Makefile
                 src/test/static/Makefile
                 src/test/scenarii/Makefile])
AC_OUTPUT
