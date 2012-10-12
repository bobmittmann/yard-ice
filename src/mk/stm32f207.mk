#
# stm32f207.mk 
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

MKDIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

SDKDIR := $(realpath $(MKDIR)/..)

TOOLSDIR = $(SDKDIR)/../tools

export SDKDIR MKDIR TOOLSDIR

MACH = stm32f207
ARCH = cm3
CPU = cortex-m3
CDEFS += STM32F207 "CM3_SYSTICK_CLK_HZ=15000000" 
OPTIONS	= -mcpu=$(CPU) -mthumb -mthumb-interwork 
CROSS_COMPILE = arm-none-eabi-

include $(MKDIR)/prog.mk

