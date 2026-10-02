AM_CPPFLAGS = $(VMOD_CFLAGS)
AM_CFLAGS = -Wall -Wextra -Werror

vmod_LTLIBRARIES = libvmod_ip2proxy.la

libvmod_ip2proxy_la_LDFLAGS = $(VMOD_LDFLAGS)
libvmod_ip2proxy_la_LIBADD = -lIP2Proxy

libvmod_ip2proxy_la_SOURCES = \
	vmod_ip2proxy.c

nodist_libvmod_ip2proxy_la_SOURCES = \
	vcc_ip2proxy_if.c \
	vcc_ip2proxy_if.h

@BUILD_VMOD_IP2PROXY@

EXTRA_DIST = vmod_ip2proxy.vcc

DISTCLEANFILES = vmod_vcs_version.txt
