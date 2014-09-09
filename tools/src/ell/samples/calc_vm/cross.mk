# File:    	prog.mk 
# Author:   Robinson Mittmann (bobmittmann@gmail.com)
# Target:
# Comment:  
# Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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
CFLAGS := $(CFLAGS) -Wall $(addprefix -D,$(CDEFS))
LDFLAGS	:= $(OPTIONS) $(LDFLAGS) -Wl,--print-map -Wl,--cref -Wl,--sort-common $(addprefix -L,$(LIBPATH))

#------------------------------------------------------------------------------ 
# update generated files list
#------------------------------------------------------------------------------ 

HFILES_OUT = $(addprefix $(OUTDIR)/, $(HFILES_GEN))
CFILES_OUT = $(addprefix $(OUTDIR)/, $(CFILES_GEN))
SFILES_OUT = $(addprefix $(OUTDIR)/, $(SFILES_GEN))

#------------------------------------------------------------------------------ 
# object files
#------------------------------------------------------------------------------ 
OFILES = $(addprefix $(OUTDIR)/, $(notdir $(CFILES_GEN:.c=.o) $(SFILES_GEN:.S=.o) $(CFILES:.c=.o) $(SFILES:.S=.o)))

#
# Extract the directory list from input file list
#
vpath %.c $(abspath $(sort $(dir $(CFILES))))

#------------------------------------------------------------------------------ 
# Program output files
#------------------------------------------------------------------------------ 
PROG_OUT = $(OUTDIR)/$(PROG)
PROG_MAP = $(OUTDIR)/$(PROG).map
PROG_ELF = $(OUTDIR)/$(PROG).elf
PROG_LST = $(OUTDIR)/$(PROG).lst

all: $(PROG_OUT) $(PROG_LST)

clean:
	$(Q)rm -f $(CFILES_OUT) $(SFILES_OUT) $(OFILES) $(PROG_OUT) $(PROG_ELF) $(PROG_LST) $(PROG_MAP)

prog: $(PROG_OUT)

elf: $(PROG_ELF)

lst: $(PROG_LST)

$(PROG_OUT): $(PROG_ELF)
	$(ACTION) "STRIP: $@"
	$(Q)$(STRIP) $< -o $@

$(PROG_ELF): $(OFILES) 
	$(ACTION) "LD: $@"
	$(Q)$(LD) $(LDFLAGS) $(OFILES) $(addprefix -l,$(LIBS)) -o $@ > $(PROG_MAP)
#------------------------------------------------------------------------------ 
# Build tree
#------------------------------------------------------------------------------ 

$(OUTDIR):
	$(ACTION) "Creating: $@"
	$(Q) mkdir $(OUTDIR)

$(HFILES_OUT): | $(OUTDIR)

$(CFILES_OUT) $(SFILES_OUT): | $(HFILES_OUT) $(OUTDIR)

$(OFILES): $(CFILES_OUT) $(SFILES_OUT) 

.PHONY: all clean prog elf lst 

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
# Automatically generated source code files
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

%.bin: %.elf
	$(ACTION) "BIN: $@"
	$(Q)$(OBJCOPY) --output-target binary $< $@

$(OUTDIR)/%_svc.c: %.x $(OUTDIR)/%.h 
	$(ACTION) "RPCGEN: $@"
	$(Q)rm -f $@ 
	$(Q)rpcgen -M -C -m -o $@ $<

$(OUTDIR)/%_xdr.c: %.x
	$(ACTION) "RPCGEN: $@"
	$(Q)rm -f $@ 
	$(Q)rpcgen -M -C -c -o $@ $<

$(OUTDIR)/%_clnt.c: %.x
	$(ACTION) "RPCGEN: $@"
	$(Q)rm -f $@ 
	$(Q)rpcgen -M -C -l -o $@ $<

$(OUTDIR)/%.h: %.x
	$(ACTION) "RPCGEN: $@"
	$(Q)rm -f $@ 
	$(Q)rpcgen -M -C -h -o $@ $<
