#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.61)
AC_INIT([pacmap], [0.0.1], [kai.selgrad@cs.fau.de])
AM_INIT_AUTOMAKE

# name one of your source files here
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES(Makefile src/Makefile doc/Makefile doc/doxyfile doc/doxygen_mainpage.h map/Makefile)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AX_STDCXX11_FLAG
CXXFLAGS="$CXXFLAGS $ac_cv_cxx_cxx11_spec"

# when using flex/bison:
# AC_PROG_LEX
# AC_PROG_YACC

# when building libraries
# AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
#AC_CHECK_LIB([png], [main])

AC_CHECK_LIB([poly2tri], [main])
AC_CHECK_LIB([expat], [main])

PKG_CHECK_MODULES(CGL, libcgl)
AC_SUBST([CGL_CFLAGS])
AC_SUBST([CGL_LIBS])


AC_OUTPUT
