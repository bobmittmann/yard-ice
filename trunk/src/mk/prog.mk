#
# prog.mk 
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
# ld scripts
#------------------------------------------------------------------------------ 
ifndef LDDIR
  $(error LDDIR undefined!)
endif	

#------------------------------------------------------------------------------ 
# cross compiling 
#------------------------------------------------------------------------------ 
ifndef MKDIR
  $(error MKDIR undefined!)
endif	

#------------------------------------------------------------------------------ 
# cross compiling rules 
#------------------------------------------------------------------------------ 
include $(MKDIR)/cross.mk

#------------------------------------------------------------------------------ 
# generated files
#------------------------------------------------------------------------------ 
ifdef VERSION_MAJOR
  VERSION_H = $(OUTDIR)/version.h
else
  VERSION_H =
endif

#------------------------------------------------------------------------------ 
# generated source files
#------------------------------------------------------------------------------ 
HFILES_OUT = $(VERSION_H) $(addprefix $(OUTDIR)/, $(HFILES_GEN))
CFILES_OUT = $(addprefix $(OUTDIR)/, $(CFILES_GEN))
SFILES_OUT = $(addprefix $(OUTDIR)/, $(SFILES_GEN))

#------------------------------------------------------------------------------ 
# object files
#------------------------------------------------------------------------------ 
OFILES = $(addprefix $(OUTDIR)/, $(notdir $(CFILES_OUT:.c=.o) \
			$(SFILES_OUT:.S=.o)) $(CFILES:.c=.o) $(SFILES:.S=.o))
#ODIRS = $(abspath $(sort $(dir $(OFILES))))
ODIRS = $(sort $(dir $(OFILES)))

#------------------------------------------------------------------------------ 
# dependency files
#------------------------------------------------------------------------------ 
DFILES = $(addprefix $(DEPDIR)/, $(notdir $(CFILES_OUT:.c=.d) \
			$(SFILES_OUT:.S=.d)) $(CFILES:.c=.d) $(SFILES:.S=.d))
#DDIRS = $(abspath $(sort $(dir $(DFILES))))
DDIRS = $(sort $(dir $(DFILES)))

#------------------------------------------------------------------------------ 
# path variables
#------------------------------------------------------------------------------ 
LIBPATH := $(addprefix $(OUTDIR)/, $(notdir $(LIBDIRS))) $(LDDIR) $(LIBPATH)
INCPATH	:= $(INCPATH) $(abspath .)

#export INCPATH	LIBPATH

#------------------------------------------------------------------------------ 
# program output files
#------------------------------------------------------------------------------ 
ifdef PROG
  PROG_BIN := $(OUTDIR)/$(PROG).bin
  PROG_MAP := $(OUTDIR)/$(PROG).map
  PROG_ELF := $(OUTDIR)/$(PROG).elf
  PROG_SYM := $(OUTDIR)/$(PROG).sym
  PROG_LST := $(OUTDIR)/$(PROG).lst
  PROG_TAG := $(OUTDIR)/$(PROG).tag
endif

FLAGS_TO_PASS := $(FLAGS_TO_PASS) 'D=$(dbg_level)' 'V=$(verbose)' \
				 'MACH=$(MACH)'\
				 'CPU=$(CPU)'\
				 'CC=$(CC)'\
				 'LD=$(LD)'\
				 'AS=$(AS)'\
				 'AR=$(AR)'\
				 'OBJCOPY=$(OBJCOPY)'\
				 'OBJDUMP=$(OBJDUMP)'\
				 'STRIP=$(STRIP)'\
				 'CFLAGS=$(CFLAGS)'\
				 'SFLAGS=$(SFLAGS)'\
				 'LDFLAGS=$(LDFLAGS)'\
				 'INCPATH=$(INCPATH)'\
				 'LIBPATH=$(LIBPATH)'

LIBDIRS_ALL := $(LIBDIRS:%=%-all)

LIBDIRS_CLEAN := $(LIBDIRS:%=%-clean)

all: $(PROG_BIN) $(PROG_SYM) $(PROG_LST)

clean: $(LIBDIRS_CLEAN)
	$(Q)rm -f $(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT) $(OFILES) $(PROG_BIN) $(PROG_ELF) $(PROG_LST) $(PROG_SYM) $(PROG_MAP)

prog: $(PROG_BIN)

elf: $(PROG_ELF)

map: $(PROG_MAP)

bin: $(PROG_BIN)

sym: $(PROG_SYM)

lst: $(PROG_LST)

libs-all: $(LIBDIRS_ALL)

libs-clean: $(LIBDIRS_CLEAN)

bin_path:
	@echo $(PROG_BIN)

elf_path:
	@echo $(PROG_ELF)

$(LIBDIRS_ALL):
	$(ACTION) "Building : $@"
	$(Q)OUT=$(OUTDIR)/$(notdir $(@:%-all=%));\
	$(MAKE) -C $(@:%-all=%) O=$$OUT $(FLAGS_TO_PASS) all

$(LIBDIRS_CLEAN):
	$(ACTION) "Cleaning : $@"
	$(Q)OUT=$(OUTDIR)/$(notdir $(@:%-clean=%));\
	$(MAKE) -C $(@:%-clean=%) O=$$OUT $(FLAGS_TO_PASS) clean

.PHONY: all clean prog elf map bin lst libs-all libs-clean vars libs-vars 

.PHONY: $(LIBDIRS_ALL) $(LIBDIRS_CLEAN)

$(PROG_ELF) $(PROG_MAP): $(LIBDIRS_ALL) $(OFILES) $(OBJ_EXTRA)
	$(ACTION) "LD: $(PROG_ELF)"
	$(Q)$(LD) $(LDFLAGS) $(OFILES) $(OBJ_EXTRA) -Wl,--print-map \
	-Wl,--cref -Wl,--sort-common \
	-Wl,--start-group $(addprefix -l,$(LIBS)) -Wl,--end-group \
	$(addprefix -L,$(LIBPATH)) -lgcc -o $(PROG_ELF) > $(PROG_MAP)

%.sym: %.elf
	$(ACTION) "SYM: $@"
	$(Q)$(OBJDUMP) -t $< | sed '/^[0-9,a-f]\{8\} .[ ]*d[f]\?.*$$/d;/^SYMBOL.*$$/d;/^.*file format.*$$/d;/^$$/d' | sort > $@

%.lst: %.elf
	$(ACTION) "LST: $@"
	$(Q)$(OBJDUMP) -w -d -t -S -r -z $< | sed '/^[0-9,a-f]\{8\} .[ ]*d[f]\?.*$$/d' > $@

%.bin: %.elf
	$(ACTION) "BIN: $@"
	$(Q)$(OBJCOPY) -j .init -j .text -j .data --output-target binary $< $@


#------------------------------------------------------------------------------ 
# Build tree
#------------------------------------------------------------------------------ 

$(ODIRS):
	$(ACTION) "Creating: $@"
	$(Q) mkdir -p $@

$(DDIRS):
	$(ACTION) "Creating: $@"
	$(Q) mkdir -p $@

$(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT): | $(ODIRS)

$(DDIRS): | $(ODIRS) $(CFILES_OUT) $(HFILES_OUT)

$(DFILES): | $(DDIRS) 

include $(MKDIR)/cc.mk

ifdef VERSION_MAJOR
  include $(MKDIR)/version.mk
endif

include $(MKDIR)/jtag.mk

-include $(DFILES)

