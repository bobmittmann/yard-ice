## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -DPACKAGE_DATA_DIR=\""$(datadir)"\" 

AM_CFLAGS = -Os -Wall

bin_PROGRAMS = microjs

microjs_SOURCES = microjs_lex.c microjs_lib.c microjs_str.c microjs_sym.c microjs_dbg.c microjs_sdt.c microjs_vm.c microjs_ll.c microjs.c 

#nodist_microjs_SOURCES = microjs_ll.c microjs_ll.h
#CLEANFILES = microjs_ll.c microjs_ll.h const_str.h

microjs_ll.h: microjs.gr Makefile
	../ell/ell -e -d -h $<

microjs_ll.c: microjs.gr Makefile
	../ell/ell -e -d -c $<

const_str.h: const_str.txt Makefile
	./mkstr.py -n const $< > $@
