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
ARCHDIR = $(THINKOSDIR)/arch/stm32

VERSION_DATE = "2020-12-08"
VERSION_MAJOR = "0"
VERSION_MINOR = "32"

PROG = thinkos-$(VERSION_MAJOR).$(VERSION_MINOR)

CFILES = boot.c monitor.c otg_xflash_pic.c

LIBDIRS = $(ARCHDIR)/libstm32f $(SDKDIR)/libutil\
		  $(SDKDIR)/libice-comm \
		  $(SDKDIR)/libthinkos-krn $(SDKDIR)/libthinkos-mon
		  
# $(SDKDIR)/libice-comm ice-comm 

LIBS = thinkos-krn thinkos-mon stm32f util ice-comm gcc 

INCPATH = ../include

CDEFS = CONFIG_H 

CFLAGS = -Os 

RAM_VECTORS = 1
MACH = stm32f207xe
KRN_CODE_SIZE = 16384
KRN_DATA_SIZE = 4096
include $(ARCHDIR)/mk/stm32-krn.mk

