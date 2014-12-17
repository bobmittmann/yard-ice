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

MKCODELET = $(TOOLSDIR)/mkcodelet.py

OPTIONS = -mcpu=arm7tdmi -fpic
LDSCRIPT = $(BASEDIR)/ld/arm-pic.ld 
INCPATH	:= $(INCPATH) $(abspath .)

override SFLAGS :=  -Wall $(OPTIONS)
override CFLAGS :=  -Wall $(OPTIONS) 
override LDFLAGS := $(OPTIONS) $(LDFLAGS) -nostdlib -T $(LDSCRIPT)
override CROSS_COMPILE = arm-none-eabi-

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)gcc
OBJDUMP = $(CROSS_COMPILE)objdump

OFILES = $(CFILES:.c=.o) $(SFILES:.S=.o)

CODELIB_ELF = $(CODELIB).elf
CODELIB_LST = $(CODELIB).lst
CODELIB_C = $(CODELIB).c
CODELIB_H = $(CODELIB).h

all: $(CODELIB_C)
	cp $(CODELIB_C) ..

clean: 
	$(Q)rm -f $(OFILES) $(CODELIB_ELF) $(CODELIB_LST) $(CODELIB_C) $(CODELIB_H) 

elf: $(CODELIB_ELF)

lst: $(CODELIB_LST)

.PHONY: all clean elf lst

$(CODELIB_ELF): $(OFILES)
	$(LD) $(LDFLAGS) $(OFILES) -lgcc -o $@

$(CODELIB_LST): $(CODELIB_ELF)

$(CODELIB_C): $(CODELIB_LST)

.SUFFIXES:

%.o : %.c 
	$(CC) $(addprefix -I,$(INCPATH)) $(DEFINES) $(CFLAGS) -o $@ -c $<

%.o : %.S 
	$(AS) $(addprefix -I,$(INCPATH)) $(DEFINES) $(SFLAGS) -o $@ -c $<

%.lst: %.elf
	$(OBJDUMP) -t -s -j .text $< > $@

%.c: %.lst
	$(MKCODELET) -o $@ $< 

