# Makefile.am Copyright 2018 Sinodun

bin_PROGRAMS = dnsperf resperf

noinst_LIBRARIES = libperf.a
libperf_a_SOURCES = datafile.c dns.c log.c net.c opt.c os.c

AM_CFLAGS = @DNSCFLAGS@ @PTHREAD_CFLAGS@

dnsperf_SOURCES = dnsperf.c
dnsperf_LDADD = libperf.a @DNSLIBS@ @PTHREAD_LIBS@

resperf_SOURCES = resperf.c
resperf_LDADD = libperf.a @DNSLIBS@ @PTHREAD_LIBS@

dist_man_MANS = dnsperf.1 resperf.1
