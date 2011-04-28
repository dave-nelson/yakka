SUBDIRS = yakka \
	  test

ACLOCAL_AMFLAGS = -I m4

yakkadocdir = ${prefix}/doc/yakka
yakkadoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

EXTRA_DIST = $(yakkadoc_DATA)

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(yakkadocdir)
