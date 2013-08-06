# File:		Makefile
# Module: 
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


PROG = con_test

CFILES = con_test.c

LIBDIRS = ../../../sdk/libstm32f ../../../sdk/libcm3 ../../../sdk/libutil \
	  ../../../sdk/libc 

LIBS = stm32f cm3 util c

INCPATH = ../../../sdk/include 

CFLAGS = -g -O1

MACH = stm32f303xb
HSE_HZ = 8000000

include ../../../mk/stm32f3xx.mk

