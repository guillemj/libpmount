# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([libpmount], m4_esyscmd([./get-version]), [guillem@hadrons.org])
AC_CONFIG_SRCDIR([src/pmount.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 -Wall foreign nostdinc subdir-objects no-dist-gzip dist-xz])
AM_SILENT_RULES([yes])

LIBPMOUNT_ABI_MAJOR=0
LIBPMOUNT_ABI_MINOR=0
LIBPMOUNT_ABI_PATCH=16

LIBPMOUNT_ABI="$LIBPMOUNT_ABI_MAJOR:$LIBPMOUNT_ABI_MINOR:$LIBPMOUNT_ABI_PATCH"
AC_SUBST([LIBPMOUNT_ABI])

AC_ARG_ENABLE([loop],
  AS_HELP_STRING([--disable-loop], [disable loopback device support]),
  [], [enable_loop=yes]
)
AS_IF([test "x$enable_loop" = "xyes"],
  AC_DEFINE(USE_LOOP, [1], [Define to 1 to enable loopback device support.]))

AC_ARG_ENABLE([mtab],
  AS_HELP_STRING([--enable-mtab], [enable mtab support]),
  [], [enable_mtab=no]
)
AS_IF([test "x$enable_mtab" = "xyes"],
  AC_DEFINE(USE_MTAB, [1], [Define to 1 to enable mtab support.]))

# Check and store if we got user supplied variables
user_CFLAGS=${CFLAGS-unset}

# Checks for operating system services and capabilities.
AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE

# Checks for programs.
AM_PROG_AR
LT_INIT
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

# Set default compiler variables
if test "$user_CFLAGS" = unset && test "$GCC" = yes; then
  CFLAGS="$CFLAGS -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter"
fi

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  test/Makefile
])
AC_CONFIG_HEADERS([config.h])
AC_OUTPUT
