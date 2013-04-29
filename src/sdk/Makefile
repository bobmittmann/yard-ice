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

PROG = skd-test

CFILES = sdk-test.c

LIBDIRS = libcm3 libstm32f libutil libthinkos libc\
		  libice-comm libtcpip libdrv libbitvec\
		  libhexdump libcrc libaltera libopcodes\

LIBS = cm3 stm32f util thinkos c ice-comm tcpip drv bitvec hexdump crc \
	altera opcodes 

CFLAGS = -g -O1

CDEFS = CONFIG_H

INCPATH = include 

include ../mk/stm32f207.mk

help:
	@echo 'Targets:'
	@echo
	@echo '  all        - Build sdk-test'
	@echo '  clean      - Remove most generated files'
	@echo
	@echo '  make V=0|1 [targets] 0 => quiet build (default), 1 => verbose build'
	@echo '  make O=dir [targets] Locate all output files in "dir"'
	@echo '  make D=0|1 [targets] 0 => release (default), 1 => debug'
	@echo

.PHONY: help

