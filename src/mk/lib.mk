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

ifndef CONFIG_MK
 $(error Please include "config.mk" in your Makefile)
endif

THISDIR := $(dir $(lastword $(MAKEFILE_LIST)))

ifndef SCRPTDIR
  SCRPTDIR := $(abspath $(THISDIR))
  BASEDIR := $(abspath $(THISDIR)/..)
endif	

#------------------------------------------------------------------------------ 
# cross compiling 
#------------------------------------------------------------------------------ 

ifndef CROSS_COMPILE
  # default to the host compiler by tricking the make to assign a 
  # empty string to the CROSS_COMPILE variable
  empty =
  CROSS_COMPILE = $(empty)
endif	

ifndef CFLAGS
  CFLAGS := -g -O1
endif

include $(SCRPTDIR)/cross.mk

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
#INCPATH := $(abspath $(INCPATH))

ifeq ($(HOST),Cygwin)
  INCPATH_WIN := $(subst \,\\,$(foreach h,$(INCPATH),$(shell cygpath -w $h)))
  OFILES_WIN := $(subst \,\\,$(foreach o,$(OFILES),$(shell cygpath -w $o)))
endif

#------------------------------------------------------------------------------ 
# library output files
#------------------------------------------------------------------------------ 
ifdef LIB_STATIC
  LIB_STATIC_OUT = $(OUTDIR)/lib$(LIB_STATIC).a
  LIB_STATIC_LST = $(OUTDIR)/lib$(LIB_STATIC).lst
  LIB_OUT := $(LIB_STATIC_OUT)
  LIB_LST := $(LIB_STATIC_LST)
endif

ifdef LIB_SHARED
  LIB_SHARED_LST = $(OUTDIR)/lib$(LIB_SHARED).lst
  ifeq ($(SOEXT),dll)
    LIB_SHARED_OUT = $(OUTDIR)/$(LIB_SHARED).$(SOEXT)
    LDFLAGS = -Wl,--dll
  else
    LIB_SHARED_OUT = $(OUTDIR)/lib$(LIB_SHARED).$(SOEXT)
    LDFLAGS = -Wl,-soname,$(LIB_SHARED).so
  endif
  LIB_OUT += $(LIB_SHARED_OUT)
  LIB_LST += $(LIB_SHARED_LST)
endif

DEPDIRS_ALL:= $(DEPDIRS:%=%-all)

DEPDIRS_CLEAN := $(DEPDIRS:%=%-clean)

CLEAN_FILES := $(OFILES) $(DFILES) $(LIB_STATIC_OUT) $(LIB_SHARED_OUT) $(LIB_SHARED_LST) $(LIB_STATIC_LST)

ifeq (Windows,$(HOST))
  CLEAN_FILES := $(subst /,\,$(CLEAN_FILES))
endif

#$(info ~~~~~~~~~~~~~~~~~~~~~~~~~~)
#$(info MAKEFILE_LIST = '$(MAKEFILE_LIST)')
#$(info SRCDIR = '$(firstword $(SRCDIR))')
#$(info ODIRS = '$(ODIRS)')
#$(info OS = '$(OS)')
#$(info OSTYPE = '$(OSTYPE)')
#$(info HOST = '$(HOST)')
#$(info DIRMODE = '$(DIRMODE)')
#$(info MSYSTEM = '$(MSYSTEM)')
#$(info MSYSCON = '$(MSYSCON)')
#$(info MAKE_MODE = '$(MAKE_MODE)')
#$(info INCPATH = '$(INCPATH)')
#$(info INCPATH_WIN = '$(INCPATH_WIN)')
#$(info LIBDIRS = '$(LIBDIRS)')
#$(info abspath = '$(abspath .)')
#$(info realpath = '$(realpath .)')
#$(info CFLAGS = '$(CFLAGS)')
#$(info HOME = '$(HOME)')
#$(info HOMEPATH = '$(HOMEPATH)')
#$(info CLEAN_FILES = '$(CLEAN_FILES)')
#$(info ~~~~~~~~~~~~~~~~~~~~~~~~~~)

all: $(LIB_OUT)

clean: deps-clean
	$(Q)$(RMALL) $(CLEAN_FILES)

lib: $(LIB_OUT)

lst: $(LIB_LST)

gen: $(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT)
	
deps-all: $(DEPDIRS_ALL)

deps-clean: $(DEPDIRS_CLEAN)

#------------------------------------------------------------------------------ 
# Code::Blocks targets
#------------------------------------------------------------------------------ 

Debug: 
	$(Q)$(MAKE) D=1 all

Release: 
	$(Q)$(MAKE) D=0 all

cleanDebug: 
	$(Q)$(MAKE) D=1 clean

cleanRelease: 
	$(Q)$(MAKE) D=0 clean

#------------------------------------------------------------------------------ 
# Library targets
#------------------------------------------------------------------------------ 

#$(LIB_STATIC_OUT): $(DEPDIRS_ALL) $(OFILES)
$(LIB_STATIC_OUT): $(OFILES)
	$(ACTION) "AR: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(AR) $(ARFLAGS) $(subst \,\\,$(shell cygpath -w $@)) $(OFILES_WIN) > $(DEVNULL)
else
	$(Q)$(AR) $(ARFLAGS) $@ $(OFILES) 1> $(DEVNULL)
endif

$(LIB_SHARED_OUT): $(DEPDIRS_ALL) $(OFILES)
	$(ACTION) "LD: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(LD) $(LDFLAGS) -shared $(OFILES_WIN) $(OBJ_EXTRA) \
	$(addprefix -l,$(LIBS)) $(addprefix -L,$(LIBPATH_WIN)) -o $@
else
	$(Q)$(LD) $(LDFLAGS) -shared $(OFILES) $(OBJ_EXTRA) \
	$(addprefix -l,$(LIBS)) $(addprefix -L,$(LIBPATH)) -o $@
endif

#------------------------------------------------------------------------------ 
# Library dependencies targets
#------------------------------------------------------------------------------ 

$(DEPDIRS_ALL):
	$(ACTION) "Building : $@"
	$(Q)$(MAKE) -C $(@:%-all=%) O=$(OUTDIR)/$(notdir $(@:%-all=%)) $(FLAGS_TO_PASS) all

$(DEPDIRS_CLEAN):
	$(ACTION) "Cleaning : $@"
	$(Q)$(MAKE) -C $(@:%-clean=%) O=$(OUTDIR)/$(notdir $(@:%-clean=%)) $(FLAGS_TO_PASS) clean

%.lst: %.a
	$(ACTION) "LST: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(OBJDUMP) -w -t -d -S $(subst \,\\,$(shell cygpath -w $<)) > $@
else
	$(Q)$(OBJDUMP) -w -t -d -S $< > $@
endif

.PHONY: all clean lib lst deps-all deps-clean
.PHONY: Debug Release cleanDebug cleanRelease
.PHONY: $(DEPDIRS_BUILD) $(DEPDIRS_CLEAN)

#------------------------------------------------------------------------------ 
# Build tree
#------------------------------------------------------------------------------ 

$(ODIRS):
	$(ACTION) "Creating outdir: $@"
ifeq ($(HOST),Windows)
	$(Q)$(MKDIR) $(subst /,\,$@) 
else
	-$(Q)$(MKDIR) $@ 
endif

$(DDIRS):
	$(ACTION) "Creating depdir: $@"
ifeq ($(HOST),Windows)
	$(Q)$(MKDIR) $(subst /,\,$@) 
else
	-$(Q)$(MKDIR) $@ 
endif

$(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT): | $(ODIRS)

$(DDIRS) : | $(ODIRS) $(CFILES_OUT)

$(DFILES): | $(DDIRS)

include $(SCRPTDIR)/cc.mk

#
# FIXME: automatic dependencies are NOT included in Cygwin.
# The dependencie files must have the paths converted
# to cygwin (unix) style to be of any use!
#
ifneq ($(HOST),Cygwin)
-include $(DFILES)
endif


