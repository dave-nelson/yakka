INCLUDES  = -I$(top_srcdir)

test_ldadd = $(YAKKA_LIBS) $(top_builddir)/yakka/libyakka.la

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(YAKKA_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

noinst_PROGRAMS = test

test_SOURCES = \
	main.c
test_LDFLAGS = 
test_LDADD = $(test_ldadd)

check: $(noinst_PROGRAMS)
	teststatus=0; 						\
	progfailed=""; 						\
	for p in $(noinst_PROGRAMS); do 			\
		./$$p; 						\
		status=$$?; 					\
		if test $$status != 0; then 			\
			teststatus=$$status; 			\
			progfailed="$$progfailed $$p"; 		\
		fi; 						\
	done;							\
	if test $$teststatus != 0; then				\
		echo "Programs failed:$$progfailed";		\
	fi;							\
	exit $$teststatus;
# END
