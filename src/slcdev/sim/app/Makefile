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

PROG = slcdev-app

CFILES = slcdev-app.c io.c isink.c slcdev-drv.c \
		 serdrv.c slcdev-db.c flashfs.c shell.c\
		 slcdev-sim.c slcdev-cfg.c\
		 slcdev-str.c slcdev-lib.c profclk.c js.c slcdev-log.c\
		 simlnk-usr.c simrpc-file.c simrpc-sys.c simrpc-sim.c

SDK = ../../../sdk

LIBDIRS = $(SDK)/libstm32f $(SDK)/libcm3 $(SDK)/libc $(SDK)/libutil \
		  $(SDK)/libcrc $(SDK)/libshell $(SDK)/libmicrojs

LIBS = gcc stm32f cm3 util c crc shell microjs

INCPATH = $(SDK)/include ../include ../../include

CDEFS = CONFIG_H NDEBUG
CFLAGS = -g -O2 -Wall -fomit-frame-pointer -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections 

VERSION_DATE = "2015-09-28"
VERSION_MAJOR = "0"
VERSION_MINOR = "6"

LOAD_ADDR = 0x08004000
JTAGTOOL_ADDR = 192.168.10.50

MACH = stm32l151xc
THINKAPP = 1
LDSCRIPT = slcdev-sim.ld
include ../../../mk/stm32l1xx.mk

