#
# Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
# 
# This file is part of the YARD-ICE.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3.0 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You can receive a copy of the GNU Lesser General Public License from 
# http://www.gnu.org/

#
# File:   Makefile
# Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

include ../../mk/config.mk

LIB_STATIC = c

INET_CFILES = inet_addr.c inet_aton.c inet_ntoa.c inet_ntop.c 

CFILES = atoi.c environ.c fclose.c fflush.c fgetc.c\
		 fgets.c fprintf.c fputc.c fputs.c fread.c fwrite.c\
		 fopen.c fscanf.c vfscanf.c sscanf.c vsscanf.c file.c \
		 getchar.c putchar.c ctype.c div.c\
		 memcmp.c memcpy.c memset.c memmove.c ffs.c\
		 printf.c puts.c vsnprintf.c sprintf.c snprintf.c\
		 strcasecmp.c strcat.c strchr.c strstr.c\
		 strcmp.c strncmp.c strcpy.c strncpy.c strlen.c\
		 strtol.c strtoul.c vfprintf.c malloc.c calloc.c errno.c \
		 strerror.c strtod.c strtoull.c assert.c \
		 rand.c stdin.c stdout.c stderr.c strtok.c null.c abort.c\
		 time.c $(addprefix inet/,$(INET_CFILES))


include ../../mk/lib.mk

