## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" 

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = dcclog

dcclog_SOURCES = \
	arm_elf.c\
	hexdump.c\
	dcclog.c

dcclog_CFLAGS = \
	-O2 -Wall

dcclog_LDADD =  \
	-lpthread 

