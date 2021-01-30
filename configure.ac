#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.70])
AC_INIT([abstouch-nux], [1.5.0], [acedrons@yahoo.co.jp])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/input.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([X11], [XWarpPointer])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([fcntl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove memset select strstr strtol])

AC_CONFIG_FILES([
    Makefile src/Makefile
    man/Makefile examples/Makefile
    config/Makefile completion/Makefile
    doc/Makefile])
AC_OUTPUT
