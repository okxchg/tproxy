#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([tproxy], [0.1], [oliver.kindernay@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign 1.9.6])

AC_CONFIG_SRCDIR([src/tproxy.c])
# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AM_PATH_CHECK()
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])

#PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([netinet/in.h stdlib.h string.h strings.h sys/socket.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([socket strerror vsyslog strcasecmp])

AC_OUTPUT
