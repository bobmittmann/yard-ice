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
ifeq (Windows,$(HOST))
  HFILES_OUT = $(addprefix $(OUTDIR)\, $(HFILES_GEN))
  CFILES_OUT = $(addprefix $(OUTDIR)\, $(CFILES_GEN))
  SFILES_OUT = $(addprefix $(OUTDIR)\, $(SFILES_GEN))
else
  HFILES_OUT = $(addprefix $(OUTDIR)/, $(HFILES_GEN))
  CFILES_OUT = $(addprefix $(OUTDIR)/, $(CFILES_GEN))
  SFILES_OUT = $(addprefix $(OUTDIR)/, $(SFILES_GEN))
endif

#------------------------------------------------------------------------------ 
# object files
#------------------------------------------------------------------------------ 
OFILES = $(addprefix $(OUTDIR)/,\
		   $(CFILES_GEN:.c=.o) \
		   $(SFILES_GEN:.S=.o) \
		   $(CFILES:.c=.o) \
		   $(SFILES:.S=.o))

#ifeq (Windows,$(HOST))
#  ODIRS = $(subst /,\,$(sort $(dir $(OFILES))))
#else
  ODIRS = $(sort $(dir $(OFILES)))
#endif

#------------------------------------------------------------------------------ 
# dependency files
#------------------------------------------------------------------------------ 

DFILES = $(OFILES:.o=.d) $(OFILES_OUT:.c=.d)

#------------------------------------------------------------------------------ 
# Installation directory
#------------------------------------------------------------------------------ 

INSTALLDIR = $(abspath .)

ifndef LIBDIR
  LIBDIR := $(OUTDIR)
endif

#------------------------------------------------------------------------------ 
# path variables
#------------------------------------------------------------------------------ 
override INCPATH += $(abspath .)

ifeq ($(HOST),Cygwin)
  INCPATH_WIN := $(subst \,\\,$(foreach h,$(INCPATH),$(shell cygpath -w $h)))
  OFILES_WIN := $(subst \,\\,$(foreach o,$(OFILES),$(shell cygpath -w $o)))
endif

#------------------------------------------------------------------------------ 
# library output files
#------------------------------------------------------------------------------ 
ifdef LIB_STATIC
ifeq (Windows,$(HOST))
  LIB_STATIC_OUT = $(LIBDIR)\lib$(LIB_STATIC).a
  LIB_STATIC_LST = $(LIBDIR)\lib$(LIB_STATIC).lst
else
  LIB_STATIC_OUT = $(LIBDIR)/lib$(LIB_STATIC).a
  LIB_STATIC_LST = $(LIBDIR)/lib$(LIB_STATIC).lst
endif
  LIB_OUT = $(LIB_STATIC_OUT)
  LIB_LST = $(LIB_STATIC_LST)
endif

ifdef LIB_SHARED
  LIB_SHARED_LST = $(LIBDIR)/lib$(LIB_SHARED).lst
  ifeq ($(SOEXT),dll)
    LIB_SHARED_OUT = $(LIBDIR)/$(LIB_SHARED).$(SOEXT)
    LDFLAGS = -Wl,--dll
  else
    LIB_SHARED_OUT = $(LIBDIR)/lib$(LIB_SHARED).$(SOEXT)
    LDFLAGS = -Wl,-soname,$(LIB_SHARED).so
  endif
  LIB_OUT += $(LIB_SHARED_OUT)
  LIB_LST += $(LIB_SHARED_LST)
endif

DEPDIRS_ALL:= $(DEPDIRS:%=%-all)

DEPDIRS_CLEAN := $(DEPDIRS:%=%-clean)

LFILES := $(LIB_STATIC_OUT) $(LIB_SHARED_OUT) $(LIB_SHARED_LST) \
		  $(LIB_STATIC_LST) 

ifeq (Windows,$(HOST))
  CLEAN_OFILES := $(strip $(subst /,\,$(OFILES)))
  CLEAN_DFILES := $(strip $(subst /,\,$(DFILES)))
  CLEAN_LFILES := $(strip $(subst /,\,$(LFILES)))
  OUTDIR := $(subst /,\,$(OUTDIR))
  INSTALLDIR := $(subst /,\,$(INSTALLDIR))
  LIB_OUT_WIN := $(subst /,\,$(LIB_OUT))
else
  CLEAN_OFILES := $(strip $(OFILES))
  CLEAN_DFILES := $(strip $(DFILES))
  CLEAN_LFILES := $(strip $(LFILES))
endif

#------------------------------------------------------------------------------ 
# Make scripts debug
#------------------------------------------------------------------------------ 

$(call trace1,<lib.mk> ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~)
$(call trace1,HOST = '$(HOST)')
$(call trace1,DIRMODE = '$(DIRMODE)')
$(call trace1,SHELL = '$(SHELL)')
$(call trace2,OUTDIR = '$(OUTDIR)')
$(call trace2,SRCDIR = '$(SRCDIR)')
$(call trace3,CFILES = '$(CFILES)')
$(call trace3,OFILES = '$(OFILES)')
$(call trace3,ODIRS = '$(ODIRS)')
$(call trace3,VERSION_H = '$(VERSION_H)')
#$(info OS = '$(OS)')
#$(info HOST = '$(HOST)')
#$(info DIRMODE = '$(DIRMODE)')
#$(info SHELL = '$(SHELL)')
#$(info SRCDIR = '$(SRCDIR)')
#$(info OUTDIR = '$(OUTDIR)')
#$(info CFILES = '$(CFILES)')
#$(info OFILES = '$(OFILES)')
#$(info LIB_OUT = '$(LIB_OUT)')
#$(info ODIRS = '$(ODIRS)')
#$(info MAKEFILE_LIST = '$(MAKEFILE_LIST)')
#$(info SET = '$(shell set)')
#$(info LIB_STATIC = '$(LIB_STATIC)')
#$(info LIB_STATIC_OUT = '$(LIB_STATIC_OUT)')
#$(info CFILES_OUT = '$(CFILES_OUT)')
#$(info MSYSTEM = '$(MSYSTEM)')
#$(info MSYSCON = '$(MSYSCON)')
#$(info MAKE_MODE = '$(MAKE_MODE)')
#$(info INCPATH = '$(INCPATH)')
#$(info INCPATH_WIN = '$(INCPATH_WIN)')
#$(info abspath = '$(abspath .)')
#$(info realpath = '$(realpath .)')
#$(info CFLAGS = '$(CFLAGS)')
#$(info HOME = '$(HOME)')
#$(info HOMEPATH = '$(HOMEPATH)')
$(call trace1,~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ </lib.mk>)

all: $(LIB_OUT)

clean: deps-clean
	$(Q)$(RMALL) $(CLEAN_LFILES)
	$(Q)$(RMALL) $(CLEAN_DFILES)
	$(Q)$(RMALL) $(CLEAN_OFILES)

install: $(LIB_OUT)
ifeq (Windows,$(HOST))
	$(Q)$(CP) $(LIB_OUT_WIN) $(INSTALLDIR)
else
	$(Q)$(CP) $(LIB_OUT) $(INSTALLDIR)
endif


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

$(LIB_STATIC_OUT): $(OFILES) 
	$(ACTION) "AR: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(AR) $(ARFLAGS) $(subst \,\\,$(shell cygpath -w $@)) $(OFILES_WIN) > $(DEVNULL)
else
  ifeq ($(HOST),Windows)
	$(foreach f,$(OFILES),$(shell $(AR) r $@ $(f)))
  else
	$(Q)$(AR) $(ARFLAGS) $@ $(OFILES) 1> $(DEVNULL)
  endif
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
	$(Q)if not exist $(subst /,\,$@) $(MKDIR) $(subst /,\,$@)
else
	-$(Q)$(MKDIR) $@ 
endif

$(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT): | $(ODIRS)

$(DFILES): | $(ODIRS)

$(OFILES): | $(ODIRS)

#------------------------------------------------------------------------------ 
# Compilation
#------------------------------------------------------------------------------ 

include $(SCRPTDIR)/cc.mk

#------------------------------------------------------------------------------ 
# Automatic dependencies
#------------------------------------------------------------------------------ 

#
# FIXME: automatic dependencies are NOT included in Cygwin.
# The dependencie files must have the paths converted
# to cygwin (unix) style to be of any use!
#
ifneq ($(HOST),Cygwin)
-include $(DFILES)
endif

