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


PROG = simrpc-test

BOOT_CFILES = ../boot/boot.c ../boot/monitor.c ../boot/board.c \
			  ../boot/otg_xflash_pic.c ../fpga/ice40lp384.c ../fpga/lattice.c

HUB_CFILES = ../test/io.c ../test/simlnk.c ../test/simrpc.c

CFILES = $(BOOT_CFILES) $(HUB_CFILES) simlnk_test.c

SDK = ../../../sdk

LIBDIRS = $(SDK)/libstm32f $(SDK)/libcm3 $(SDK)/libice-comm $(SDK)/libutil \
		  $(SDK)/libthinkos $(SDK)/libconsole $(SDK)/libstm32drv \
		  $(SDK)/libtrace

LIBS = gcc stm32f cm3 ice-comm util thinkos console c stm32drv trace 

INCPATH = $(SDK)/include ../include ../../include ../test/include 

JTAGTOOL_ADDR = 192.168.10.50

CDEFS = CONFIG_H CM3_RAM_VECTORS TEST
CFLAGS = -g -Os -Wall -fomit-frame-pointer -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections 

MACH = stm32f427
LOAD_ADDR = 0x08010000
include ../../../mk/stm32f4xx.mk

