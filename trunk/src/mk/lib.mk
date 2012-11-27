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
ODIRS = $(abspath $(sort $(dir $(OFILES))))

#------------------------------------------------------------------------------ 
# dependency files
#------------------------------------------------------------------------------ 
DFILES = $(addprefix $(DEPDIR)/, $(CFILES:.c=.d) $(SFILES:.S=.d))
DDIRS = $(abspath $(sort $(dir $(DFILES))))

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

ifdef LIB_SHARED
  LIB_SHARED_OUT = $(OUTDIR)/lib$(LIB_SHARED).so
  LIB_SHARED_LST = $(OUTDIR)/lib$(LIB_SHARED).so.lst
endif

all: $(LIB_STATIC_LST) $(LIB_SHARED_LST)

clean: deps-clean
	$(Q)rm -f $(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT) $(OFILES) $(LIB_STATIC_OUT) $(LIB_SHARED_OUT) $(LIB_SHARED_LST) $(LIB_STATIC_LST)

lib: $(LIB_STATIC_OUT)

lst: $(LIB_STATIC_LST)

gen: $(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT)
	
$(LIB_STATIC_OUT): deps-all $(DEPDIRS) $(LIBDEPS) $(OFILES)
	$(ACTION) "AR: $@"
	$(Q)$(AR) $(ARFLAGS) $@ $(OFILES) $(LIBDEPS) 1> /dev/null

deps-all: $(DDIRS)
	$(ECHO) - DEPS START ------------------
	$(Q)for d in $(DEPDIRS); do\
		if [ -f ./$$d/Makefile ]; then \
			OUT=$(OUTDIR)/`basename $$d`;\
			if [ ! -d $$OUT ]; then\
				mkdir $$OUT;\
			fi;\
			(cd ./$$d && $(MAKE) O=$$OUT $(FLAGS_TO_PASS) all) || exit 1;\
		fi;\
	done;
	$(ECHO) - DEPS END --------------------

deps-clean: $(DDIRS) 
	$(Q)for d in $(DEPDIRS); do \
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

