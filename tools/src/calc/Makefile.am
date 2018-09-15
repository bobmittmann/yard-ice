## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -DPACKAGE_DATA_DIR=\""$(datadir)"\" 

AM_CFLAGS = -O1 -Wall -g

bin_PROGRAMS = calc

calc_SOURCES = calc.c calc_lex.c calc_dbg.c calc_parse.c 
nodist_calc_SOURCES = calc_ll.c calc_ll.h
CLEANFILES = calc_ll.c calc_ll.h

calc_ll.h: calc.gr Makefile
	../ell/ell -e -h $<

calc_ll.c: calc.gr Makefile
	../ell/ell -e -c $<
