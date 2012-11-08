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
# ld script
#------------------------------------------------------------------------------ 

ifndef LDSCRIPT
  $(error LDSCRIPT undefined!)
endif

#------------------------------------------------------------------------------ 
# cross compiling 
#------------------------------------------------------------------------------ 
ifndef MKDIR
  $(error MKDIR undefined!)
endif	

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
ODIRS = $(abspath $(sort $(dir $(OFILES))))

#------------------------------------------------------------------------------ 
# dependency files
#------------------------------------------------------------------------------ 
DFILES = $(addprefix $(DEPDIR)/, $(notdir $(CFILES_OUT:.c=.d) \
			$(SFILES_OUT:.S=.d)) $(CFILES:.c=.d) $(SFILES:.S=.d))
DDIRS = $(abspath $(sort $(dir $(DFILES))))

#------------------------------------------------------------------------------ 
# path variables
#------------------------------------------------------------------------------ 
LIBPATH := $(addprefix $(OUTDIR)/, $(notdir $(LIBDIRS))) $(LIBPATH)
INCPATH	:= $(INCPATH) $(abspath .)

#export INCPATH	LIBPATH

#------------------------------------------------------------------------------ 
# program output files
#------------------------------------------------------------------------------ 
ifdef PROG
  PROG_BIN = $(OUTDIR)/$(PROG).bin
  PROG_MAP = $(OUTDIR)/$(PROG).map
  PROG_ELF = $(OUTDIR)/$(PROG).elf
  PROG_SYM = $(OUTDIR)/$(PROG).sym
  PROG_LST = $(OUTDIR)/$(PROG).lst
  PROG_TAG = $(OUTDIR)/$(PROG).tag
endif

all: $(PROG_BIN) $(PROG_SYM) $(PROG_LST)

clean: libs-clean
	$(Q)rm -f $(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT) $(OFILES) $(PROG_BIN) $(PROG_ELF) $(PROG_LST) $(PROG_MAP)

prog: $(PROG_BIN)

elf: $(PROG_ELF)

bin: $(PROG_BIN)

sym: $(PROG_SYM)

lst: $(PROG_LST)

dirs: $(ODIRS)
	$(warning ODIRS=$(ODIRS))
	$(warning DDIRS=$(DDIRS))
	$(warning OFILES=$(OFILES))

$(PROG_ELF): libs-all $(LIBDIRS) $(OFILES) $(OBJ_EXTRA)
	$(ACTION) "LD: $@"
	$(Q)$(LD) $(LDFLAGS) $(OFILES) $(OBJ_EXTRA) -Wl,--print-map -Wl,--cref \
	-Wl,--sort-common -Wl,--start-group $(addprefix -l,$(LIBS)) \
    -Wl,--end-group $(addprefix -L,$(LIBPATH)) -lgcc -o $@ > $(PROG_MAP)

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

libs-all: $(DDIRS)
	$(ECHO) - LIBS START ------------------
	$(Q)for d in $(LIBDIRS); do\
		if [ -f ./$$d/Makefile ]; then \
			OUT=$(OUTDIR)/`basename $$d`;\
			if [ ! -d $$OUT ]; then\
				mkdir $$OUT;\
			fi;\
			(cd ./$$d && $(MAKE) O=$$OUT $(FLAGS_TO_PASS) all) || exit 1;\
		fi;\
	done;
	$(ECHO) - LIBS END --------------------

libs-clean: $(DDIRS)
	$(Q)for d in $(LIBDIRS); do \
		if [ -f ./$$d/Makefile ]; then \
			OUT=$(OUTDIR)/`basename $$d`;\
			if [ ! -d $$OUT ]; then\
				mkdir $$OUT;\
			fi;\
			(cd ./$$d && $(MAKE) O=$$OUT $(FLAGS_TO_PASS) clean) || exit 1;\
		fi;\
	done;

#------------------------------------------------------------------------------ 
# Build tree
#------------------------------------------------------------------------------ 

$(ODIRS):
	$(ACTION) "Creating: $@"
	$(Q) mkdir $@

$(DDIRS):
	$(ACTION) "Creating: $@"
	$(Q) mkdir $@

$(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT): | $(ODIRS)

$(DDIRS): | $(ODIRS) $(CFILES_OUT) $(HFILES_OUT)

$(DFILES): | $(DDIRS) 

.PHONY: all clean prog elf bin lst libs-all libs-clean vars libs-vars 

include $(MKDIR)/cc.mk

ifdef VERSION_MAJOR
  include $(MKDIR)/version.mk
endif

include $(MKDIR)/jtag.mk

#include $(DFILES)

