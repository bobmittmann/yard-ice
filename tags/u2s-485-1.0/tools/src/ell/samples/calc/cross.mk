# File:    	cross.mk 
# Author:   Robinson Mittmann (bobmittmann@gmail.com)
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

#------------------------------------------------------------------------------ 
# parameters 
#------------------------------------------------------------------------------ 
ifdef V
  verbose = $(V)
else
  verbose = 0
endif

ifdef O
  OUTDIR = $(abspath $(O))
else
  OUTDIR = $(abspath ./)
endif

ifeq ($(verbose),0) 
  Q = @
  ACTION = @echo
else
  Q =
  ACTION = @\#
endif

#------------------------------------------------------------------------------ 
# gcc toolchain
#------------------------------------------------------------------------------ 
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump
STRIP = $(CROSS_COMPILE)strip

#------------------------------------------------------------------------------ 
# compiler flags
#------------------------------------------------------------------------------ 

INCPATH	:= $(SDKDIR)/include $(OUTDIR) $(INCPATH)
SFLAGS := $(SFLAGS) -Wall $(addprefix -D,$(CDEFS))
CFLAGS := $(CFLAGS) -O2 -g -Wall $(addprefix -D,$(CDEFS))
LDFLAGS	:= $(OPTIONS) $(LDFLAGS) -Wl,--sort-common $(addprefix -L,$(LIBPATH))

#------------------------------------------------------------------------------ 
# object files
#------------------------------------------------------------------------------ 
OFILES = $(addprefix $(OUTDIR)/, $(notdir $(CFILES:.c=.o) $(SFILES:.S=.o)))

#
# Extract the directory list from input file list
#
vpath %.c $(abspath $(sort $(dir $(CFILES))))

#------------------------------------------------------------------------------ 
# Program output files
#------------------------------------------------------------------------------ 
ifdef PROG
  PROG_OUT = $(OUTDIR)/$(PROG)
  PROG_ELF = $(OUTDIR)/$(PROG).elf
  OUT = $(PROG_OUT)
endif

#------------------------------------------------------------------------------ 
# library output files
#------------------------------------------------------------------------------ 
ifdef LIB_STATIC
  LIB_STATIC_OUT = $(OUTDIR)/lib$(LIB_STATIC).a
  OUT = $(LIB_STATIC_OUT)
endif

ifdef LIB_SHARED
  LIB_SHARED_OUT = $(OUTDIR)/lib$(LIB_SHARED).so
  OUT = $(LIB_SHARED_OUT)
endif

all: $(OUT)

clean:
	$(Q)rm -f $(OFILES) $(PROG_OUT) $(PROG_ELF) $(LIB_STATIC_OUT)

prog: $(PROG_OUT)

elf: $(PROG_ELF)

lib: $(LIB_STATIC_OUT)

#------------------------------------------------------------------------------ 
# App
#------------------------------------------------------------------------------ 

$(PROG_OUT): $(PROG_ELF)
	$(ACTION) "STRIP: $@"
	$(Q)$(STRIP) $< -o $@

$(PROG_ELF): $(OFILES) 
	$(ACTION) "LD: $@"
	$(Q)$(LD) $(LDFLAGS) $(OFILES) $(addprefix -l,$(LIBS)) -o $@ 

#------------------------------------------------------------------------------ 
# Lib
#------------------------------------------------------------------------------ 

$(LIB_STATIC_OUT): $(OFILES)
	$(ACTION) "AR: $@"
	$(Q)$(AR) $(ARFLAGS) $@ $(OFILES) $(LIBDEPS) 1> /dev/null

#------------------------------------------------------------------------------ 
# Build tree
#------------------------------------------------------------------------------ 

$(OUTDIR):
	$(ACTION) "Creating: $@"
	$(Q) mkdir $(OUTDIR)

.PHONY: all clean prog elf lib

.SUFFIXES:

compile = $(CC) $(addprefix -I,$(INCPATH)) $(DEFINES) $(CFLAGS)
assemble = $(AS) $(addprefix -I,$(INCPATH)) $(DEFINES) $(SFLAGS)

$(OUTDIR)/%.o : %.c 
	$(ACTION) "CC: $@"
	$(Q)$(compile) -o $@ -c $<

$(OUTDIR)/%.o : %.S 
	$(ACTION) "AS: $@"
	$(Q)$(assemble) -o $@ -c $<

#------------------------------------------------------------------------------ 
# generated source code files
#------------------------------------------------------------------------------ 

$(OUTDIR)/%.o : $(OUTDIR)/%.c
	$(ACTION) "CC: $@"
	$(Q)$(compile) -o $@ -c $<

$(OUTDIR)/%.o : $(OUTDIR)/%.S
	$(ACTION) "AS: $@"
	$(Q)$(assemble) -o $@ -c $<

%.sym: %.elf
	$(ACTION) "SYM: $@"
	$(Q)$(OBJDUMP) -t $< | sed '/^[0-9,a-f]\{8\} .[ ]*d[f]\?.*$$/d;/^SYMBOL.*$$/d;/^.*file format.*$$/d;/^$$/d' | sort > $@

%.lst: %.elf
	$(ACTION) "LST: $@"
	$(Q)$(OBJDUMP) -w -d -t -S -r -z $< | sed '/^[0-9,a-f]\{8\} .[ ]*d[f]\?.*$$/d' > $@

