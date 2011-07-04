AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(YAKKA_CFLAGS)

AM_CFLAGS =\
	 -Wall

lib_LTLIBRARIES = libyakka-0.la

libyakka_0_la_SOURCES =		\
	Error.c			\
	MethodList.c		\
	Object.c		\
	Runtime.c		\
	WeakRef.c

libyakka_0_la_LDFLAGS = 

libyakka_0_la_LIBADD = $(YAKKA_LIBS)

yakkaincludedir = $(includedir)/yakka-0/yakka
yakkainclude_HEADERS = 		\
	Yakka.h			\
	Error.h			\
	Error-protected.h	\
	Interface.h		\
	MethodList.h		\
	Object.h		\
	Object-protected.h	\
	Runtime.h		\
	WeakRef.h		\
	WeakRef-protected.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libyakka-0.pc

EXTRA_DIST = \
	libyakka-0.pc.in
