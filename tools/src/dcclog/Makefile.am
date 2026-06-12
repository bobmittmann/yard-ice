## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" 

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = dcclog

dcclog__SOURCES =  \
	arm_elf.c \
	dcc.c \
	dcclog.c \
	file.c \
	net.c

dcclog_SOURCES = \
	arm_elf.c\
	hexdump.c\
	dcclog.c\
	net.c\
	file.c\
	dcc.c

dcclog_CFLAGS = \
	-O2 -Wall

dcclog_LDADD =  \
	-lpthread 

