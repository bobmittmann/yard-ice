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

LIBDIRS = libcm3 libstm32f libutil libthinkos libc libice-comm libtcpip libdrv\
		  libbitvec libhexdump

LIBS = stm32f cm3 util thinkos c ice-comm tcpip drv bitvec hexdump

CFLAGS = -g -O2 

JTAGTOOL_ADDR = 192.168.0.80

VERSION_MAJOR = "0"
VERSION_MINOR = "1"

include mk/stm32f207.mk

