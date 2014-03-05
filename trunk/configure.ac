#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(2lgc_code, 0.0.0.2, devel@2lgc.org, 2lgc_code, http://2lgc.org)
AM_INIT_AUTOMAKE([-Wno-portability])
AC_CONFIG_SRCDIR([src/lib/1992_1_1/1992_1_1_materiaux.h])
AC_CONFIG_HEADERS([config.h])
AM_GNU_GETTEXT([external])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AM_PROG_CC_C_O
AC_PROG_F77
AC_PROG_LIBTOOL
LIBXML_REQUIRED_VERSION=2.8.0
AC_SUBST(XML_CPPFLAGS)
AC_SUBST(XML_LIBS)
AM_PATH_XML2($LIBXML_REQUIRED_VERSION, :, AC_MSG_ERROR(Test for libXML failed.))
GLIB_REQUIRED_VERSION=2.38.0
AM_PATH_GLIB_2_0($GLIB_REQUIRED_VERSION, :, AC_MSG_ERROR(Test for GLIB failed.))
AC_ARG_WITH(debug,  [  --with-debug           build with debug support], [withdebug=true], [withdebug=false])
AC_MSG_RESULT([XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
with-debug $withdebug
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX])
AM_CONDITIONAL(ENABLE_DEBUG, test x$withdebug = xtrue)
if test x$withdebug = xtrue; then
AC_DEFINE(ENABLE_DEBUG, 1, [Défini si les options de débug doivent être activés])
fi
AC_ARG_WITH(gtk,  [  --with-gtk           build with gtk support], [withgtk=true], [withgtk=false])
AC_MSG_RESULT([XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
with-gtk $withgtk
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX])
AM_CONDITIONAL(ENABLE_GTK, test x$withgtk = xtrue)
if test x$withgtk = xtrue; then
GTK_REQUIRED_VERSION=3.6.0
AM_PATH_GTK_3_0($GTK_REQUIRED_VERSION, :, AC_MSG_ERROR(Test for GTK+ failed.))
AC_CHECK_LIB([M3d++], [main], [], [AC_MSG_ERROR([M3d++ ( http://m3dlib.tuxfamily.org ) est necessaire])])
AC_DEFINE(ENABLE_GTK, 1, [Défini si GTK doit être supporté])
fi
AM_GNU_GETTEXT([external])
ALL_LINGUAS="fr"
PKG_PROG_PKG_CONFIG

# Détermination de la version de Windows que l'on est en train de compiler
AC_MSG_CHECKING([for some Win platform])
case "$host" in
  *-*64-mingw*|*-*64-cygwin*)
    platform_win=Win64
    ;;
  *-*-mingw*|*-*-cygwin*)
    platform_win=Win32
    ;;
  *)
    platform_win=no
    ;;
esac
AC_MSG_RESULT([$platform_win])
AM_CONDITIONAL(PLATFORM_WIN32, test "$platform_win" = "Win32")
AM_CONDITIONAL(PLATFORM_WIN64, test "$platform_win" = "Win64")
AM_CONDITIONAL(PLATFORM_LINUX, test "$platform_win" = "no")

# Vérification de la possibilité de compiler la documentation via doxygen
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
    then AC_MSG_WARN([Doxygen introuvable - documentation non compilable])
fi
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])

# Checks for libraries.
AC_CHECK_LIB(m, sqrt)
AC_CHECK_LIB([colamd], [colamd_recommended], [], [AC_MSG_ERROR([colamd ( www.cise.ufl.edu/research/sparse/colamd ) est necessaire])])
AC_CHECK_LIB([suitesparseconfig], [SuiteSparse_time], [LIBS="-lsuitesparseconfig $LIBS"], [AC_CHECK_LIB([ufconfig], [UFfree], [LIBS="-lufconfig $LIBS"], [AC_MSG_ERROR([suitesparseconfig ( www.cise.ufl.edu/research/sparse ) est necessaire])])])
AC_CHECK_LIB([amd], [amd_malloc], [], [AC_MSG_ERROR([amd ( http://www.cise.ufl.edu/research/sparse/amd ) est necessaire])])
LIBS="-lgfortran $LIBS"
sinclude(acx_blas.m4)
ACX_BLAS
if test x"$BLAS_LIBS" = x; then
    AC_MSG_ERROR([Cannot find blas libraries])
fi
AC_CHECK_LIB([cholmod], [cholmod_start], [], [AC_MSG_ERROR([cholmod ( http://www.cise.ufl.edu/research/sparse/cholmod ) est necessaire])])
AC_CHECK_LIB([umfpack], [main], [], [AC_MSG_ERROR([umfpack ( www.cise.ufl.edu/research/sparse/umfpack ) est necessaire])])

# Checks for header files.
AC_CHECK_HEADERS([libintl.h locale.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([localeconv memset modf pow setlocale sqrt strchr strstr])

AC_CONFIG_FILES([Makefile po/Makefile.in
                 docs/Makefile
                 files/Makefile
                 src/Makefile
                 src/gui/Makefile
                 src/gui/codegui/Makefile
                 src/lib/Makefile
                 docs/Doxyfile
                 src/test/Makefile])
AC_OUTPUT
