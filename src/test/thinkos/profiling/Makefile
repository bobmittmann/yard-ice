# File:     Makefile
# Author:   Robinson Mittmann (bobmittmann@gmail.com)
# Target:
# Comment:
# Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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

PROG = prof_test

CFILES = prof_test.c ../common/stdio.c

LIBDIRS = ../../../libstm32f ../../../libcm3 ../../../libutil \
		  ../../../libthinkos ../../../libc ../../../libice-comm

LIBS = stm32f cm3 util thinkos c ice-comm

INCPATH = ../../../include

CFLAGS = -g -O2 

#include ../../../../mk/stm32f207.mk
MACH = stm32f407
include ../../../../mk/stm32f4xx.mk

