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


PROG = simple

CFILES = simple.c

LIBDIRS = ../../sdk/libcm3 cmx-irq

LIBS = cmx cm3

INCPATH = ../../sdk/include 

CFLAGS = -g -O1

export MKDIR LDDIR TOOLSDIR

ifndef MACH 
MACH = stm32f100
endif

ARCH = cm3
CPU = cortex-m3
MACH = stm32f100x6
CDEFS += STM32F100 "HCLK_HZ=22579200" 

OPTIONS	= -mcpu=$(CPU) -mthumb -mthumb-interwork 
CROSS_COMPILE = arm-none-eabi-

LDFLAGS = -nostdlib -T simple.ld

include ../../mk/prog.mk


