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


PROG = thinkos

CFILES = boot.c ../monitor.c board.c

SDK = ../../sdk

LIBDIRS = $(SDK)/libstm32f $(SDK)/libcm3 $(SDK)/libutil \
		  $(SDK)/libcrc $(SDK)/libthinkos $(SDK)/libice-comm

LIBS = cm3 stm32f util crc thinkos ice-comm gcc 

INCPATH = $(SDK)/include 

CDEFS = CONFIG_H CM3_RAM_VECTORS NDEBUG

CFLAGS = -g -O2 -Wall -fomit-frame-pointer -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections 

MACH = stm32l151xb

JTAGTOOL_ADDR = 192.168.10.50

include ../../mk/stm32l1xx.mk
