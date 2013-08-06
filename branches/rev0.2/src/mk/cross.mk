#
# cross.mk 
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

#------------------------------------------------------------------------------ 
# default output directories
#------------------------------------------------------------------------------ 
RELEASE_DIR = release
DEBUG_DIR = debug

#------------------------------------------------------------------------------ 
# parameters 
#------------------------------------------------------------------------------ 
ifdef V
  verbose = $(V)
else
  verbose = 0
endif

ifdef D
  dbg_level = $(D)
else
  dbg_level = 0
endif

ifdef O
  OUTDIR = $(abspath $(O))
else
  ifneq ($(dbg_level),0)
    OUTDIR = $(abspath $(DEBUG_DIR))
  else
    OUTDIR = $(abspath $(RELEASE_DIR))
  endif
endif

ifeq ($(verbose),0) 
  Q = @
  ACTION = @echo
  ECHO = @\#
else
  Q =
  ACTION = @\#
  ECHO = @echo
endif

DEPDIR=$(OUTDIR)/.deps

export OUTDIR DEPDIR Q ACTION

#------------------------------------------------------------------------------ 
# Cross compiler configuration
#------------------------------------------------------------------------------ 

ifndef CROSS_COMPILE
  $(warning CROSS_COMPILE undefined!)
endif	# CROSS_COMPILE

export CROSS_COMPILE

#------------------------------------------------------------------------------ 
# gcc toolchain
#------------------------------------------------------------------------------ 
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
NM = $(CROSS_COMPILE)nm
CXX = $(CROSS_COMPILE)g++
RANLIB = $(CROSS_COMPILE)ranlib
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
STRIP = $(CROSS_COMPILE)strip

#------------------------------------------------------------------------------ 
# compiler flags
#------------------------------------------------------------------------------ 

ifneq ($(dbg_level),0) 
  CDEFS := $(CDEFS) DEBUG=$(dbg_level)
  CFLAGS += -g
endif

INCPATH	:= $(OUTDIR) $(abspath $(INCPATH))
SFLAGS := $(OPTIONS) -Wall $(SFLAGS) $(addprefix -D,$(CDEFS))
CFLAGS := $(OPTIONS) -Wall $(CFLAGS) $(addprefix -D,$(CDEFS))
LDFLAGS := $(OPTIONS) $(LDFLAGS) -nostdlib -T $(LDSCRIPT)

