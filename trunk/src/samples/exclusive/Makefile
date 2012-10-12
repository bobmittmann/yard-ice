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

MACH = stm32f207

VERSION_MAJOR = 0

PROG = exclusive

CFILES = exclusive.c 

LIBDIRS = ../../stm32f ../../libcm3 ../../libutil ../../libc

LIBS = stm32f cm3 util c

CFLAGS = -g -Os
CDEFS = "CM3_SYSTICK_CLK_HZ=15000000"

JTAGTOOL_ADDR = 192.168.1.30

include ../../mk/cortex-m3.mk

