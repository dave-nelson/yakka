
INCLUDES  = -I$(top_srcdir)

test_ldadd = $(YAKKA_LIBS) 				\
	$(top_builddir)/yakka/libyakka-0.la

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(YAKKA_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

noinst_LTLIBRARIES = libootest.la

libootest_la_SOURCES = 		\
	Alpha.h			\
	Alpha-protected.h	\
	Alpha.c			\
	Beta.h			\
	Beta.c			\
	Gamma.h			\
	Gamma-protected.h	\
	Gamma.c			\
	Delta.h			\
	Delta-protected.h	\
	Delta.c

libootest_la_LIBADD = $(YAKKA_LIBS)			\
	$(top_builddir)/yakka/libyakka-0.la

