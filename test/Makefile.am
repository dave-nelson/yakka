SUBDIRS = ootest

INCLUDES  = -I$(top_srcdir)

test_ldadd = $(YAKKA_LIBS) 				\
	$(top_builddir)/yakka/libyakka-0.la		\
	$(top_builddir)/test/ootest/libootest.la

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(YAKKA_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

test_programs = 		\
	test_error		\
	test_object		\
	test_interface		\
	test_method		\
	test_weak_ref		\
	test_members

noinst_PROGRAMS = $(test_programs)

test_error_SOURCES = test_error.c
test_error_LDADD = $(test_ldadd)

test_object_SOURCES = test_object.c
test_object_LDADD = $(test_ldadd)

test_interface_SOURCES = test_interface.c
test_interface_LDADD = $(test_ldadd)

test_method_SOURCES = test_method.c
test_method_LDADD = $(test_ldadd)

test_weak_ref_SOURCES = test_weak_ref.c
test_weak_ref_LDADD = $(test_ldadd)

test_members_SOURCES = test_members.c
test_members_LDADD = $(test_ldadd)

check: $(test_programs)
	teststatus=0; 						\
	progfailed=""; 						\
	for p in $(test_programs); do				\
		echo "----------";				\
		echo "Test suite: $$p"; echo "";		\
		./$$p; 						\
		status=$$?; 					\
		if test $$status != 0; then 			\
			teststatus=$$status; 			\
			progfailed="$$progfailed $$p"; 		\
		fi; 						\
	done;							\
	echo "----------";					\
	if test $$teststatus != 0; then				\
		echo "Programs failed:$$progfailed";		\
	fi;							\
	exit $$teststatus;
# END
