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
# but WITHLST ANY WARRANTY; withlst even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You can receive a copy of the GNU Lesser General Public License from 
# http://www.gnu.org/

THISDIR:= $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

ifndef BASEDIR
  BASEDIR := $(realpath $(THISDIR)/..)
endif

ifndef TOOLSDIR 
  TOOLSDIR = $(BASEDIR)/../tools
endif	

CPU = cortex-m3
OPTIONS	= -mcpu=$(CPU) -mthumb -mthumb-interwork -fpic
LDSCRIPT = cm3-pic.ld 
INCPATH	:= $(INCPATH) $(realpath .) $(realpath $(BASEDIR)/sdk/include) 

override SFLAGS :=  -Wall $(OPTIONS)
override CFLAGS :=  -Wall $(OPTIONS) -Os -g
override LDFLAGS := $(OPTIONS) $(LDFLAGS) -nostdlib -T $(LDSCRIPT)
override CROSS_COMPILE = arm-none-eabi-

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)gcc
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy

OUTDIR = $(realpath .)
OFILES = $(addprefix $(OUTDIR)/, $(CFILES:.c=.o) $(SFILES:.S=.o))

CODELIB_ELF = $(CODELIB).elf
CODELIB_LST = $(CODELIB).lst
CODELIB_BIN = $(CODELIB).bin
CODELIB_C = $(CODELIB).c
CODELIB_H = $(CODELIB).h

all: $(CODELIB_C) $(CODELIB_LST)

clean: 
	@rm -f $(OFILES) $(CODELIB_ELF) $(CODELIB_LST) $(CODELIB_BIN) \
		$(CODELIB_C) $(CODELIB_H) 

install: $(CODELIB_C) $(CODELIB_LST)
	cp $(CODELIB_C) ../slc-dev

elf: $(CODELIB_ELF)

lst: $(CODELIB_LST)

.PHONY: all clean elf lst bin

$(info --------------------------)
$(info OFILES = '$(OFILES)')
$(info --------------------------)

$(CODELIB_ELF): $(OFILES)
	$(LD) $(LDFLAGS) $(OFILES) -lgcc -o $@

$(CODELIB_LST): $(CODELIB_ELF)

$(CODELIB_BIN): $(CODELIB_ELF)

$(CODELIB_C): $(CODELIB_BIN)

.SUFFIXES:

%.o : %.c 
	$(CC) $(addprefix -I,$(INCPATH)) $(DEFINES) $(CFLAGS) -o $@ -c $<

%.o : %.S 
	$(AS) $(addprefix -I,$(INCPATH)) $(DEFINES) $(SFLAGS) -o $@ -c $<

%.lst: %.elf
	$(OBJDUMP) -w -d -t -S -r -z $< > $@
#	$(OBJDUMP) -t -s -j .text $< > $@

%.bin: %.elf
	$(OBJCOPY) -j .init -j .text -j .data --output-target binary $< $@
#	$(OBJCOPY) -j .init -j .text -j .ARM.extab -j .ARM.exidx -j .data \
#					  --output-target binary $< $@

%.c: %.bin
	$(TOOLSDIR)/bin2hex.py -n $(CODELIB) $< > $@ 

