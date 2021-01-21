#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.70])
AC_INIT([abstouch-nux], [1.4.0], [acedrons@yahoo.co.jp])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/input.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
# FIXME: Replace 'main' with a function in '-lX11':
AC_CHECK_LIB([X11], [XWarpPointer])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([fcntl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memmove memset select strtol])

AC_CONFIG_FILES([
    Makefile src/Makefile
    man/Makefile examples/Makefile
    config/Makefile completion/Makefile
    ])
AC_OUTPUT
