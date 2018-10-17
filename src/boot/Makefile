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

THINKOSDIR = ../../thinkos
SDKDIR = $(THINKOSDIR)/sdk

VERSION_DATE = "2018-10-17"
VERSION_MAJOR = "0"
VERSION_MINOR = "27"

PROG = thinkos-$(VERSION_MAJOR).$(VERSION_MINOR)

CFILES = boot.c monitor.c otg_xflash_pic.c

LIBDIRS = $(SDKDIR)/libstm32f $(SDKDIR)/libcm3 $(SDKDIR)/libutil\
		  $(SDKDIR)/libthinkos 
		  
# $(SDKDIR)/libice-comm ice-comm 

LIBS = cm3 stm32f util thinkos gcc 

INCPATH = $(SDKDIR)/include ../include

CDEFS = CONFIG_H CM3_RAM_VECTORS

CFLAGS = -g -Os -Wall -fno-builtin -ffreestanding -fomit-frame-pointer
OPTIONS = -mno-unaligned-access 

MACH = stm32f207
include $(THINKOSDIR)/mk/stm32.mk
