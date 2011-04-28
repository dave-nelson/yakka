## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(YAKKA_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

lib_LTLIBRARIES = libyakka.la


libyakka_la_SOURCES = \
	lib.c

libyakka_la_LDFLAGS = 

libyakka_la_LIBADD = $(YAKKA_LIBS)

include_HEADERS = \
	yakka.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libyakka-0.1.pc

EXTRA_DIST = \
	libyakka-0.1.pc.in
