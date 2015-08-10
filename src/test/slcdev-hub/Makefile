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


PROG = slcdev-hub

CFILES = slcdev-hub.c console.c lattice.c pktbuf.c sndbuf.c \
		 rs485lnk.c net.c \
		 xflash.c otg_xflash_pic.c uart_xflash_pic.c ice40lp384.c

SDK = ../../sdk
TOOLS = ../../../tools

LIBDIRS = $(SDK)/libstm32f $(SDK)/libcm3 $(SDK)/libutil \
		  $(SDK)/libthinkos $(SDK)/libc $(SDK)/libice-comm \
		  $(SDK)/libstm32drv $(SDK)/libcdc-acm $(SDK)/libtty \
		  $(SDK)/libxmodem $(SDK)/libshell $(SDK)/libcrc \
		  $(SDK)/libtrace $(SDK)/libhexdump

LIBS = gcc stm32f cm3 util thinkos c ice-comm stm32drv cdc-acm \
	   tty crc xmodem shell trace hexdump

INCPATH = $(SDK)/include 

JTAGTOOL_ADDR = 192.168.10.50

CDEFS = CONFIG_H
CFLAGS = -g -O2

MACH = stm32f407
include ../../mk/stm32f4xx.mk

ice40lp384.c: release/rs485hub_bitmap.bin
	$(TOOLS)/bin2hex.py -n ice40lp384_bin $< > $@ 
