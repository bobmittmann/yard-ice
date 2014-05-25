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
# Cross compiler configuration
#------------------------------------------------------------------------------ 

ifndef CROSS_COMPILE
  $(warning CROSS_COMPILE undefined!)
endif	# CROSS_COMPILE

#------------------------------------------------------------------------------ 
# Shared object file extensions
#------------------------------------------------------------------------------ 

#------------------------------------------------------------------------------ 
# Warning: This discovery method is very poor.
# The assumption is if your host is Msys or Windows shell and the
# CROSS_COMPILE is empty then most probably your tring to produce a DLL
#

ifeq ($(strip $(CROSS_COMPILE)),)
  # Cross compiling is set to an empty string (assuming host=target)
  ifeq ($(HOST),Windows)
    SOEXT := dll
  else 
    ifeq ($(HOST),Msys)
      SOEXT := dll
    else
      SOEXT := so
    endif
  endif
else
  # cross compiling (assuming *NIX shared objects)
  SOEXT := so
endif

#------------------------------------------------------------------------------ 
# gcc toolchain
#------------------------------------------------------------------------------ 
CC := $(strip $(CROSS_COMPILE))gcc
LD := $(strip $(CROSS_COMPILE))gcc
AS := $(strip $(CROSS_COMPILE))gcc
AR := $(strip $(CROSS_COMPILE))ar
NM := $(strip $(CROSS_COMPILE))nm
CXX := $(strip $(CROSS_COMPILE))g++
RANLIB := $(strip $(CROSS_COMPILE))ranlib
OBJCOPY := $(strip $(CROSS_COMPILE))objcopy
OBJDUMP := $(strip $(CROSS_COMPILE))objdump
STRIP := $(strip $(CROSS_COMPILE))strip

#------------------------------------------------------------------------------ 
# compiler flags
#------------------------------------------------------------------------------ 

ifneq ($(dbg_level),0) 
  CDEFS := $(CDEFS) DEBUG=$(dbg_level)
#  CFLAGS += -g
endif

#INCPATH	:= $(OUTDIR) $(abspath $(INCPATH))
#INCPATH += $(OUTDIR)
SFLAGS := $(OPTIONS) -Wall $(SFLAGS) $(addprefix -D,$(CDEFS))
CFLAGS := $(OPTIONS) -Wall $(CFLAGS) $(addprefix -D,$(CDEFS))
LDFLAGS := $(OPTIONS) $(LDFLAGS)

