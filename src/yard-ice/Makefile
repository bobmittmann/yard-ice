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

CFILES = yard-ice.c board.c debugger.c ice_drv.c\
		 mod_mem.c fmt_freq.c mem_hexdump.c targets.c\
		 i2c-master.c mod_ice.c shell.c lookup.c monitor.c\
		 console.c 

LIBDIRS = ../sdk/libcm3 ../sdk/libstm32f ../sdk/libutil ../sdk/libthinkos\
		  ../sdk/libc ../sdk/libice-comm ../sdk/libtcpip ../sdk/libstm32drv\
		  ../sdk/libbitvec ../sdk/libhexdump ../sdk/libcrc ../sdk/libaltera\
		  ../sdk/libtrace ../sdk/libtty ../sdk/libxmodem\
		  ../sdk/libshell ../sdk/libconsole ../sdk/libopcodes\
		  arch jtag script cmd nand jtag3drv\
		  armice cm3ice targets comm tftp gdb vcom telnet

LIBS = cm3 stm32f util thinkos c ice-comm tcpip stm32drv bitvec hexdump \
	altera opcodes trace arch jtag script cmd nand jtag3drv armice cm3ice\
	targets comm tftp shell console tty gdb vcom telnet crc xmodem

VERSION_DATE = "2015-09-17"
VERSION_MAJOR = "0"
VERSION_MINOR = "22"

CDEFS = CONFIG_H
CFLAGS = -g -O2 -Wall -fomit-frame-pointer -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections 

INCPATH = ../sdk/include ./include

include ../mk/stm32f207.mk

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

