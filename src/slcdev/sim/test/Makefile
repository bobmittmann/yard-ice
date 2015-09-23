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

PROG = slc-dev

#CFILES_GEN = device_db.c
CFILES = ../boot/board.c ../boot/simlnk.c ../boot/simrpc_mem.c \
		 ../boot/simrpc_sys.c ../boot/boot.c \
		 ../app/flashfs.c ../app/io.c ../app/isink.c ../app/js.c \
		 ../app/profclk.c ../app/shell.c ../app/slcdev-cfg.c \
		 ../app/slcdev-db.c ../app/slcdev-drv.c ../app/slcdev-lib.c \
		 ../app/slcdev-log.c ../app/slcdev-sim.c ../app/slcdev-str.c\
		 slcdev-app.c simlnk-usr.c simrpc-file.c

SDK = ../../../sdk

LIBDIRS = $(SDK)/libstm32f $(SDK)/libcm3 $(SDK)/libc $(SDK)/libutil \
		  $(SDK)/libthinkos $(SDK)/libice-comm  $(SDK)/libcrc \
		  $(SDK)/libmicrojs $(SDK)/libshell

LIBS = gcc stm32f cm3 util c thinkos ice-comm crc microjs shell

INCPATH = $(SDK)/include ../include ../app ../../include

CDEFS = CONFIG_H NDEBUG
CFLAGS = -g -Os -Wall -fomit-frame-pointer -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections 

MACH = stm32l151xc

JTAGTOOL_ADDR = 192.168.10.50

include ../../../mk/stm32l1xx.mk
