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

MKCODELET = $(TOOLSDIR)/mkcodelet.py

export MKDIR TOOLSDIR

CPU = arm7tdmi

OPTIONS	= -mcpu=$(CPU) -fpic

CROSS_COMPILE = arm-none-eabi-

LDSCRIPT = $(BASEDIR)/ld/arm-pic.ld 

include $(MKDIR)/cross.mk

#------------------------------------------------------------------------------ 
# update generated files list
#------------------------------------------------------------------------------ 

CFILES_OUT = $(addprefix $(OUTDIR)/, $(CFILES_GEN))
SFILES_OUT = $(addprefix $(OUTDIR)/, $(SFILES_GEN))

INCPATH	:= $(INCPATH) $(abspath .)

#------------------------------------------------------------------------------ 
# object files
#------------------------------------------------------------------------------ 
OFILES = $(addprefix $(OUTDIR)/, $(notdir $(CFILES:.c=.o) $(SFILES:.S=.o)))
ODIRS = $(abspath $(sort $(dir $(OFILES))))

#------------------------------------------------------------------------------ 
# dependency files
#------------------------------------------------------------------------------ 
DFILES = $(addprefix $(DEPDIR)/, $(notdir $(CFILES:.c=.d) $(SFILES:.S=.d)))
DDIRS = $(abspath $(sort $(dir $(DFILES))))

LIBPATH := $(addprefix $(OUTDIR)/, $(notdir $(LIBDIRS))) $(LIBPATH)

PROG_BIN = $(OUTDIR)/$(PROG).bin
PROG_ELF = $(OUTDIR)/$(PROG).elf
PROG_SYM = $(OUTDIR)/$(PROG).sym
PROG_OUT = $(OUTDIR)/$(PROG).out
PROG_C = $(OUTDIR)/$(PROG).c

all: $(PROG_OUT) $(PROG_C)

clean: 
	$(Q)rm -f $(OFILES) $(PROG_ELF) $(PROG_OUT) $(PROG_SYM)

prog: $(PROG_BIN)

elf: $(PROG_ELF)

sym: $(PROG_SYM)

lst: $(PROG_OUT)

codelet: $(PROG_C)

$(PROG_ELF): $(LIBDIRS) $(OFILES)
	$(ACTION) "LD: $@"
	$(Q)$(LD) $(LDFLAGS) $(OFILES) $(addprefix -l,$(LIBS)) $(addprefix -L,$(LIBPATH)) -lgcc -o $@

vars: libs-vars
	@echo MAKELEVEL=$(MAKELEVEL)
	@echo ACTION=$(ACTION)
	@echo MACH=$(MACH)
	@echo CPU=$(CPU)
	@echo CROSS_COMPILE=$(CROSS_COMPILE)
	@echo OUTDIR=$(OUTDIR)
	@echo MKDIR=$(MKDIR)
	@echo TOOLSDIR=$(TOOLSDIR)
	@echo CFILES=$(CFILES)
	@echo OFILES=$(OFILES)
	@echo SFILES=$(SFILES)
	@echo DFILES=$(DFILES)
	@echo CC=$(CC)
	@echo INCLUDES=$(INCLUDES)
	@echo CFLAGS=$(CFLAGS)
	@echo PROG_ELF=$(PROG_ELF)
	@echo PROG_OUT=$(PROG_OUT)
	@echo LIBPATH=$(LIBPATH)

#------------------------------------------------------------------------------ 
# Build tree
#------------------------------------------------------------------------------ 

$(ODIRS):
	$(ACTION) "Creating: $@"
	$(Q) mkdir $@

$(DDIRS):
	$(ACTION) "Creating: $@"
	$(Q) mkdir $@

$(DDIRS) : | $(ODIRS) 

$(DFILES): | $(DDIRS) 

.PHONY: all clean prog elf bin lst 

include $(MKDIR)/cc.mk

%.out: %.elf
	$(ACTION) "DUMP: $@"
	$(Q)$(OBJDUMP) -t -s -j .text $< > $@

%.c: %.out
	$(ACTION) "CODELET: $@"
	$(Q)$(MKCODELET) -o $@ $< 

include $(DFILES)


