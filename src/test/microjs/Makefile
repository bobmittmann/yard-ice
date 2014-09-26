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

PROG = microjs

#CFILES_GEN = device_db.c
CFILES = microjs.c board.c flashfs.c profclk.c shell.c test_lib.c

SDK = ../../sdk

LIBDIRS = $(SDK)/libstm32f $(SDK)/libcm3 $(SDK)/libc $(SDK)/libutil \
		  $(SDK)/libice-comm $(SDK)/libxmodem \
		  $(SDK)/libcrc $(SDK)/libtty $(SDK)/libshell $(SDK)/libmicrojs

LIBS = gcc stm32f cm3 util c ice-comm xmodem crc tty shell microjs
#thinkos 

INCPATH = $(SDK)/include 

CDEFS = CONFIG_H
CFLAGS = -g -Os -fomit-frame-pointer

MACH = stm32l151xb

JTAGTOOL_ADDR = 192.168.10.50

include ../../mk/stm32l1xx.mk

$(OUTDIR)/device_db.c: device_db.js
	$(ACTION) "BIN2HEX: $@"
	$(Q)../../../tools/bin2hex.py -n device_db_js $< > $@ 

