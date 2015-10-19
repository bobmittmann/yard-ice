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


PROG = xftest

CFILES = xftest.c board.c ../xflash.c ../usb-xflash/stm32-usb_fs.c \
		 ../usb-xflash/stm32f1xx-flash.c

LIBDIRS = ../../../sdk/libstm32f ../../../sdk/libcm3 ../../../sdk/libutil \
		  ../../../sdk/libthinkos ../../../sdk/libc ../../../sdk/libice-comm \
		  ../../../sdk/libcdc-acm 

LIBS = stm32f cm3 util thinkos c ice-comm cdc-acm 

INCPATH = ../../../sdk/include ../include

CDEFS = CONFIG_H NDEBUG
#CM3_RAM_VECTORS
CFLAGS = -g -Os -Wall -ffunction-sections -fdata-sections
LDFLAGS = -Xlinker --gc-sections 

JTAGTOOL_ADDR = 192.168.10.50
#JTAG_TARGET = stm32

LDSCRIPT = stm32f103_ram.ld 
MACH = stm32f103x6
include ../../../mk/stm32f103.mk

