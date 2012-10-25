#
# arm-codelet.mk 
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

BASEDIR := $(realpath $(MKDIR)/..)

TOOLSDIR = $(BASEDIR)/../tools

export MKDIR TOOLSDIR

CPU = arm7tdmi

OPTIONS	= -mcpu=$(CPU) -fpic

CROSS_COMPILE = arm-none-eabi-

LDSCRIPT = $(BASEDIR)/ld/arm-pic.ld 

include $(MKDIR)/prog.mk

