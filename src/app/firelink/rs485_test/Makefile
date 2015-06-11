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


PROG = io_test

CFILES = io_test.c 

SDK = ../../../sdk

LIBS = cm3 stm32f stm32drv console util c driver 

INCPATH = $(SDK)/include ../include

LIBPATH = ../sdklibs/Debug ../driver/Debug

CDEFS =

CFLAGS = -g3 -Og -Wall

MACH = stm32f429
THINKOS = 1
include ../../../mk/stm32f4xx.mk

