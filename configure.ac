# configure.ac Copyright Sinodun 2018

AC_PREREQ(2.61)
AC_INIT(dnsperf, 2.1.0.0, jim@sinodun.com)
AC_CONFIG_SRCDIR([./dnsperf.c])
AC_CONFIG_HEADER([config.h])

AC_CONFIG_MACRO_DIR([.])

AM_INIT_AUTOMAKE([foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC_99
AC_PROG_RANLIB

AC_C_INLINE

AC_CHECK_LIB(socket, socket)
AC_CHECK_LIB(nsl, inet_ntoa)

AC_PATH_PROG(ac_cv_isc_config, [isc-config.sh], "no")
if test "$ac_cv_isc_config" = "no"; then
  AC_MSG_ERROR(BIND 9 libraries must be installed)
fi
AC_SUBST(DNSLIBS, "`$ac_cv_isc_config --libs dns bind9`")
AC_SUBST(DNSCFLAGS, "`$ac_cv_isc_config --cflags dns bind9`")

AX_PTHREAD
AX_CHECK_OPENSSL([], [AC_MSG_ERROR(["OpenSSL libraries not found"])])

AC_CONFIG_FILES(Makefile)

AC_OUTPUT
