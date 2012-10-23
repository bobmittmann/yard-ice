#
# Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
# 
# This file is part of the YARD-ICE.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3.0 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You can receive a copy of the GNU Lesser General Public License from 
# http://www.gnu.org/

#
# File:   Makefile
# Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

PROG = yard-ice

CFILES = yard-ice.c

LIBDIRS = sdk/libcm3 sdk/libstm32f sdk/libutil sdk/libthinkos sdk/libc \
		  sdk/libice-comm sdk/libtcpip sdk/libdrv sdk/libbitvec \
		  sdk/libhexdump sdk/libcrc sdk/libaltera sdk/libopcodes \
		  yard-ice/arch yard-ice/jtag

LIBS = stm32f cm3 util thinkos c ice-comm tcpip drv bitvec hexdump crc 

CFLAGS = -g -O2 

JTAGTOOL_ADDR = 192.168.0.80

VERSION_MAJOR = "0"
VERSION_MINOR = "1"

CDEFS = CONFIG_H

INCPATH = sdk/include yard-ice/include

include mk/stm32f207.mk

help:
	@echo 'Targets:'
	@echo
	@echo '  all        - Build YARD-ICE'
	@echo '  clean      - Remove most generated files'
	@echo '  jtagload   - Build YARD-ICE and try to load it into target'
	@echo
	@echo '  make V=0|1 [targets] 0 => quiet build (default), 1 => verbose build'
	@echo '  make O=dir [targets] Locate all output files in "dir"'
	@echo '  make D=0|1 [targets] 0 => release (default), 1 => debug'
	@echo

.PHONY: help

