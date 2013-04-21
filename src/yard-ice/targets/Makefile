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

LIB_STATIC = targets

AT91X40_CFILES = at91x40.c a7e10x2.c jtagtool3.c 

AT91SAM7_CFILES = at91sam7.c at91sam7s32.c at91sam7s64.c at91sam7s128.c \
		 at91sam7s256.c at91sam7x256.c

AT91SAM9_CFILES = at91sam9.c at91sam9260.c

DAVINCI_CFILES = davinci.c dm365.c dm365-nand.c vpm-dm36x.c

STELLARIS_CFILES = stellaris.c lm3s6965.c

STM32F_CFILES = stm32f.c stm32f10x.c stm32f2xx.c stm32f205.c stm32f207.c

CFILES = target_db.c dummy.c \
		$(addprefix at91x40/,$(AT91X40_CFILES)) \
		$(addprefix at91sam7/,$(AT91SAM7_CFILES)) \
		$(addprefix at91sam9/,$(AT91SAM9_CFILES)) \
		$(addprefix davinci/,$(DAVINCI_CFILES)) \
		$(addprefix stellaris/,$(STELLARIS_CFILES)) \
		$(addprefix stm32f/,$(STM32F_CFILES))

include ../../mk/lib.mk

