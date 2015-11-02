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

LIB_STATIC = arch

ARM_CFILES = arm7.c arm9.c arm-ram.c arm-pn.c

CM3_CFILES = cm3.c cm3-ram.c 

TI_CFILES = icepick.c

CFILES = $(addprefix arm/,$(ARM_CFILES)) \
		 $(addprefix cm3/,$(CM3_CFILES)) \
		 $(addprefix ti/,$(TI_CFILES)) \

include ../../mk/lib.mk

