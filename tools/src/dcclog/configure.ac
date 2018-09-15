dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT([dcclog], [0.4])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

AC_CANONICAL_HOST
dnl Special check for windows
dnl case "$host_os" in
dnl 	*mingw*)
dnl 		AC_CHECK_LIB(ws2_32, WSAStartup, , AC_MSG_ERROR(Winsock is needed for a Win32 build.))
dnl 		;;
dnl esac

AC_CHECK_HEADER([winsock2.h],
	[AC_DEFINE([HAVE_LIBWS2_32], [1], [Define if libws2_32 exists.])
	LIBS="-lws2_32 $LIBS"], [], [#include <windows.h>])

AC_OUTPUT([Makefile])

