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


PROG = bacnet_test

CFILES = bacnet_test.c console.c lattice.c ice40lp384.c device.c
#bip.c

SDK = ../../../sdk
TOOLS = ../../../../tools

LIBDIRS = $(SDK)/libstm32f $(SDK)/libcm3 $(SDK)/libutil \
		  $(SDK)/libthinkos $(SDK)/libc $(SDK)/libice-comm \
		  $(SDK)/libdrv $(SDK)/libcdc-acm $(SDK)/libtty $(SDK)/libcrc \
		  $(SDK)/libshell $(SDK)/libbacnet 
		  
# $(SDK)/libtcpip

LIBS = cm3 stm32f util thinkos c ice-comm crc drv cdc-acm tty \
	   shell  bacnet gcc
#tcpip

INCPATH = $(SDK)/include ../bacnet-stack/include ../bacnet-stack/demo/object

CDEFS = CONFIG_H
CFLAGS = -g -O2 -Wall
#CFLAGS = -g -O2 -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused

JTAGTOOL_ADDR = 192.168.10.50
MACH = stm32f407
include ../../../mk/stm32f4xx.mk

