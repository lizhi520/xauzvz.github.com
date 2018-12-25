#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([xaudiopro], [1.0], [luolongzhi@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([libxautil/llz_clock.c])
AC_CONFIG_HEADERS([config.h])

LT_INIT

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([memory.h stdint.h stdlib.h string.h strings.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset mkdir pow sqrt])

AC_OUTPUT([Makefile libxautil/Makefile libxafilter/Makefile libxacodec/Makefile xatools/Makefile])
