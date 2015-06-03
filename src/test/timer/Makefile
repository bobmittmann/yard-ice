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


PROG = timer-test

CFILES = timer-test.c 


LIBDIRS = ../../sdk/libstm32f ../../sdk/libcm3 ../../sdk/libutil \
		  ../../sdk/libthinkos ../../sdk/libice-comm

LIBS = stm32f cm3 util thinkos ice-comm 

INCPATH = ../../sdk/include 

JTAGTOOL_ADDR = 192.168.10.50

CFLAGS = -g -O1

MACH = stm32f407
include ../../mk/stm32f4xx.mk

