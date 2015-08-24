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

CFILES = slcdev-hub.c io.c console.c\
		 ../mstp/mstp_crc.c ../mstp/mstp_lnk.c pktbuf.c net.c \
		 simlnk.c simrpc.c webserver.c 

SDK = ../../../sdk

LIBS = gcc stm32f cm3 util console c stm32drv \
	   tty crc xmodem shell trace hexdump tcpip httpd

LIBPATH = ../sdklibs/Debug

INCPATH = $(SDK)/include ../include ../../include

JTAGTOOL_ADDR = 192.168.10.50

CDEFS = CONFIG_H NDEBUG
CFLAGS = -g -Os -Wall -fomit-frame-pointer -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections 

MACH = stm32f427
THINKAPP = 1
LDSCRIPT = slcdev-hub.ld
LOAD_ADDR = 0x08010000
include ../../../mk/stm32f4xx.mk

