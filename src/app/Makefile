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

THINKOSDIR = ../../thinkos
SDKDIR = $(THINKOSDIR)/sdk
ARCHDIR = $(THINKOSDIR)/arch/stm32

VERSION_DATE = "2020-12-08"
VERSION_MAJOR = "0"
VERSION_MINOR = "44"

PROG = yard-ice-$(VERSION_MAJOR).$(VERSION_MINOR)

CFILES = ../yard-ice.c ../io.c ../debugger.c ../ice_drv.c\
	 ../mod_mem.c ../fmt_freq.c ../mem_hexdump.c ../targets.c\
	 ../i2c-master.c ../mod_ice.c ../shell.c ../lookup.c ../console.c 

LIBDIRS = $(ARCHDIR)/libstm32f $(SDKDIR)/libutil \
	  $(SDKDIR)/libice-comm $(SDKDIR)/libc $(SDKDIR)/libtcpip \
	  $(ARCHDIR)/libstm32drv $(SDKDIR)/libbitvec $(SDKDIR)/libhexdump \
	  $(SDKDIR)/libcrc $(SDKDIR)/libtrace $(SDKDIR)/libtty \
	  $(SDKDIR)/libxmodem $(SDKDIR)/libshell $(SDKDIR)/libthinkos-usr \
	  ../libopcodes ../libaltera\
	  ../arch ../jtag ../script ../cmd ../nand ../jtag3drv\
	  ../armice ../cm3ice ../targets ../tftp ../gdb ../vcom ../telnet \
	  ../comm 

LIBS = stm32f util ice-comm c tcpip stm32drv bitvec hexdump \
       altera opcodes trace arch jtag script cmd nand jtag3drv armice cm3ice\
       targets tftp shell thinkos-usr tty gdb vcom telnet crc xmodem comm gcc

CDEFS = CONFIG_H
CFLAGS = -g -Os 

INCPATH = $(SDKDIR)/include ../include

MACH = stm32f207xe
LDCODE := 0x08020000
LDDATA := 0x20002000
include $(ARCHDIR)/mk/stm32-app.mk

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

