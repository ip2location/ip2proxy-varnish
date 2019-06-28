AM_CPPFLAGS = @VMOD_INCLUDES@
AM_CFLAGS = -Wall -Wextra -Werror

vmoddir = @VMOD_DIR@
vmod_LTLIBRARIES = libvmod_ip2proxy.la

libvmod_ip2proxy_la_LDFLAGS = -module -export-dynamic -avoid-version -shared \
	-lIP2Proxy

libvmod_ip2proxy_la_SOURCES = \
	vcc_if.c \
	vcc_if.h \
	vmod_ip2proxy.c

vcc_if.c vcc_if.h: @VMODTOOL@ $(top_srcdir)/src/vmod_ip2proxy.vcc
	@VMODTOOL@ $(top_srcdir)/src/vmod_ip2proxy.vcc

EXTRA_DIST = vmod_ip2proxy.vcc $(VMOD_TESTS)

CLEANFILES = $(builddir)/vcc_if.c $(builddir)/vcc_if.h
