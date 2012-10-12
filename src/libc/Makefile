# File:		Makefile
# Author:   Robinson Mittmann (bobmittmann@gmail.com)
# Target:
# Comment:  
# Copyright(C) 2012 Bob Mittmann. All Rights Reserved.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 

LIB_STATIC = c

INET_CFILES = inet_addr.c inet_aton.c inet_ntoa.c inet_ntop.c 

CFILES = atoi.c environ.c fclose.c fflush.c fgetc.c\
		 fgets.c fprintf.c fputc.c fputs.c fread.c fwrite.c\
		 getchar.c ctype.c \
		 int2dec.c uint2dec.c ll2dec.c ull2dec.c uint2hex.c ull2hex.c\
		 memcmp.c memcpy.c memset.c ffs.c\
		 printf.c putchar.c puts.c vsnprintf.c sprintf.c snprintf.c strcat.c strchr.c \
		 strcmp.c strcpy.c strlen.c strtol.c strtoul.c vfprintf.c\
		 rand.c stdin.c stdout.c stderr.c strtok.c \
		 $(addprefix inet/,$(INET_CFILES)) \

include ../mk/lib.mk

