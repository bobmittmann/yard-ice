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
  $(error MKDIR undefined!)
endif	

include $(MKDIR)/cross.mk

#------------------------------------------------------------------------------ 
# generated files
#------------------------------------------------------------------------------ 

#------------------------------------------------------------------------------ 
# update generated files list
#------------------------------------------------------------------------------ 

CFILES_OUT = $(addprefix $(OUTDIR)/, $(CFILES_GEN))
SFILES_OUT = $(addprefix $(OUTDIR)/, $(SFILES_GEN))
HFILES_OUT = $(addprefix $(OUTDIR)/, $(HFILES_GEN))

HFILES = $(HFILES_OUT)

override INCPATH += $(abspath .)

#------------------------------------------------------------------------------ 
# object files
#------------------------------------------------------------------------------ 
#OFILES = $(addprefix $(OUTDIR)/, $(notdir $(CFILES_GEN:.c=.o) $(SFILES_GEN:.S=.o) $(CFILES:.c=.o) $(SFILES:.S=.o)))

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
# library output files
#------------------------------------------------------------------------------ 
ifdef LIB_STATIC
  LIB_STATIC_OUT = $(OUTDIR)/lib$(LIB_STATIC).a
  LIB_STATIC_LST = $(OUTDIR)/lib$(LIB_STATIC).lst
endif

ifdef LIB_SHARED
  LIB_SHARED_OUT = $(OUTDIR)/lib$(LIB_SHARED).so
  LIB_SHARED_LST = $(OUTDIR)/lib$(LIB_SHARED).so.lst
endif

all: $(LIB_STATIC_LST) $(LIB_SHARED_LST)

clean:
	$(Q)rm -f $(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT) $(OFILES) $(LIB_STATIC_OUT) $(LIB_SHARED_OUT) $(LIB_SHARED_LST) $(LIB_STATIC_LST)

lib: $(LIB_STATIC_OUT)

lst: $(LIB_STATIC_LST)

gen: $(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT)
	
$(LIB_STATIC_OUT): $(LIBDEPS) $ $(DEPS) $(OFILES)
	$(ACTION) "AR: $@"
	$(Q)$(AR) $(ARFLAGS) $@ $(OFILES) $(LIBDEPS) 1> /dev/null

vars:
	@echo MAKELEVEL=$(MAKELEVEL)
	@echo ACTION=$(ACTION)
	@echo CROSS_COMPILE=$(CROSS_COMPILE)
	@echo OUTDIR=$(OUTDIR)
	@echo MKDIR=$(MKDIR)
	@echo TOOLSDIR=$(TOOLSDIR)
	@echo CFILES=$(CFILES)
	@echo CFILES_GEN=$(CFILES_GEN)
	@echo HFILES_GEN=$(HFILES_GEN)
	@echo OFILES=$(OFILES)
	@echo SFILES=$(SFILES)
	@echo ODIRS=$(ODIRS)
	@echo DDIRS=$(DDIRS)
	@echo CC=$(CC)
	@echo INCLUDES=$(INCLUDES)
	@echo CDEFS=$(CDEFS)
	@echo CFLAGS=$(CFLAGS)
	@echo LIB_STATIC_OUT=$(LIB_STATIC_OUT)
	@echo LIB_SHARED_OUT=$(LIB_SHARED_OUT)
	@echo VPATH=$(VPATH)

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

.PHONY: all clean lib lst

include $(MKDIR)/cc.mk

include $(DFILES)

