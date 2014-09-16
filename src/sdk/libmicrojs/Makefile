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

include ../../mk/config.mk

LIB_STATIC = microjs

CFILES = microjs_dbg.c microjs_str.c microjs_lib.c microjs_vm.c \
		 microjs_lex.c microjs_sym.c microjs_ll.c microjs_sdt.c \
		 microjs_json.c

INCPATH = ../include

include ../../mk/lib.mk

ifeq ($(HOST),Linux)
	ELL = $(TOOLSDIR)/linux/ell
else
ifeq ($(HOST),Cygwin)
	ELL = $(TOOLSDIR)/mingw/ell
else
ifeq ($(HOST),Msys)
	ELL = $(TOOLSDIR)/mingw/ell
else
ifeq ($(HOST),Windows)
	ELL = $(TOOLSDIR)/mingw/ell
else
 $(error Unsuported HOST)
endif
endif
endif
endif

ll: microjs_ll.c microjs_ll.h

microjs_ll.h: microjs.gr
	$(ELL) -e -h $<

microjs_ll.c: microjs.gr
	$(ELL) -e -c $<
	
