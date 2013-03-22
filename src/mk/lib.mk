#
# lib.mk 
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
# cross compiling 
#------------------------------------------------------------------------------ 
ifndef MKDIR
  $(warning MKDIR undefined!)
  MKDIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
endif	

include $(MKDIR)/cross.mk

#------------------------------------------------------------------------------ 
# generated source files
#------------------------------------------------------------------------------ 
HFILES_OUT = $(addprefix $(OUTDIR)/, $(HFILES_GEN))
CFILES_OUT = $(addprefix $(OUTDIR)/, $(CFILES_GEN))
SFILES_OUT = $(addprefix $(OUTDIR)/, $(SFILES_GEN))

#------------------------------------------------------------------------------ 
# object files
#------------------------------------------------------------------------------ 
OFILES = $(addprefix $(OUTDIR)/, $(CFILES_GEN:.c=.o) \
			$(SFILES_GEN:.S=.o) $(CFILES:.c=.o) $(SFILES:.S=.o))
#ODIRS = $(abspath $(sort $(dir $(OFILES))))
ODIRS = $(sort $(dir $(OFILES)))

#------------------------------------------------------------------------------ 
# dependency files
#------------------------------------------------------------------------------ 
DFILES = $(addprefix $(DEPDIR)/, $(CFILES:.c=.d) $(SFILES:.S=.d))
#DDIRS = $(abspath $(sort $(dir $(DFILES))))
DDIRS = $(sort $(dir $(DFILES)))

#------------------------------------------------------------------------------ 
# path variables
#------------------------------------------------------------------------------ 
override INCPATH += $(abspath .)

#------------------------------------------------------------------------------ 
# library output files
#------------------------------------------------------------------------------ 
ifdef LIB_STATIC
  LIB_STATIC_OUT = $(OUTDIR)/lib$(LIB_STATIC).a
  LIB_STATIC_LST = $(OUTDIR)/lib$(LIB_STATIC).lst
endif

DEPDIRS_ALL:= $(DEPDIRS:%=%-all)

DEPDIRS_CLEAN := $(DEPDIRS:%=%-clean)

all: $(LIB_STATIC_LST)

clean: deps-clean
	$(Q)rm -f $(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT) $(OFILES) $(LIB_STATIC_OUT) $(LIB_SHARED_OUT) $(LIB_SHARED_LST) $(LIB_STATIC_LST)

lib: $(LIB_STATIC_OUT)

lst: $(LIB_STATIC_LST)

gen: $(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT)
	
$(LIB_STATIC_OUT): $(DEPDIRS_ALL) $(OFILES)
	$(ACTION) "AR: $@"
	$(Q)$(AR) $(ARFLAGS) $@ $(OFILES) 1> /dev/null

deps-all: $(DEPDIRS_ALL)

deps-clean: $(DEPDIRS_CLEAN)

$(DEPDIRS_ALL):
	$(ACTION) "Building : $@"
	$(Q)OUT=$(OUTDIR)/$(notdir $(@:%-all=%));\
	$(MAKE) -C $(@:%-all=%) O=$$OUT $(FLAGS_TO_PASS) all

$(DEPDIRS_CLEAN):
	$(ACTION) "Cleaning : $@"
	$(Q)OUT=$(OUTDIR)/$(notdir $(@:%-clean=%));\
	$(MAKE) -C $(@:%-clean=%) O=$$OUT $(FLAGS_TO_PASS) clean

%.lst: %.a
	$(ACTION) "LST: $@"
	$(Q)$(OBJDUMP) -w -t -d -S $< > $@


.PHONY: all clean lib lst deps-all deps-clean

.PHONY: $(DEPDIRS_BUILD) $(DEPDIRS_CLEAN)

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

$(DDIRS) : | $(ODIRS) $(CFILES_OUT)

$(DFILES): | $(DDIRS)

include $(MKDIR)/cc.mk

-include $(DFILES)

