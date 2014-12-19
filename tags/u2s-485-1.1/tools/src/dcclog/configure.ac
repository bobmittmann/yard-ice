dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(dcclog, 0.1)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

AC_CANONICAL_HOST
dnl Special check for windows
case "$host_os" in
	*mingw*)
		AC_CHECK_LIB(wsock32, main, , AC_MSG_ERROR(Winsock is needed for a Win32 build.))
		;;
esac


AC_OUTPUT([
Makefile
])
