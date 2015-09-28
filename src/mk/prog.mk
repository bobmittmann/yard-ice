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
# generated files
#------------------------------------------------------------------------------ 
ifdef VERSION_MAJOR
  ifeq (Windows,$(HOST))
    VERSION_H := $(OUTDIR)\version.h
  else
  	VERSION_H = $(OUTDIR)/version.h
  endif
else
  VERSION_H =
endif

#------------------------------------------------------------------------------ 
# generated source files
#------------------------------------------------------------------------------ 
ifeq (Windows,$(HOST))
  HFILES_OUT = $(VERSION_H) $(addprefix $(OUTDIR)\, $(HFILES_GEN))
else
  HFILES_OUT = $(VERSION_H) $(addprefix $(OUTDIR)/, $(HFILES_GEN))
endif

CFILES_OUT = $(addprefix $(OUTDIR)/, $(CFILES_GEN))
SFILES_OUT = $(addprefix $(OUTDIR)/, $(SFILES_GEN))

#------------------------------------------------------------------------------ 
# object files
#------------------------------------------------------------------------------ 
OFILES = $(addprefix $(OUTDIR)/,\
		   $(notdir $(CFILES_OUT:.c=.o) $(SFILES_OUT:.S=.o))\
		   $(subst ../,,$(CFILES:.c=.o))\
		   $(subst ../,,$(SFILES:.S=.o)))

#ifeq (Windows,$(HOST))
# ODIRS = $(sort $(subst /,\,$(dir $(OFILES))))
#else
  ODIRS = $(sort $(dir $(OFILES)))
#endif

#------------------------------------------------------------------------------ 
# dependency files
#------------------------------------------------------------------------------ 

DFILES = $(OFILES:.o=.d)

#------------------------------------------------------------------------------ 
# library dircetories 
#------------------------------------------------------------------------------ 
LIBDIRS := $(abspath $(LIBDIRS))

#------------------------------------------------------------------------------ 
# Installation directory
#------------------------------------------------------------------------------ 

INSTALLDIR = $(abspath .)

#------------------------------------------------------------------------------ 
# library output directories 
#------------------------------------------------------------------------------ 

ifeq (Windows,$(HOST))
  LIB_OUTDIR := $(subst /,\,$(OUTDIR))
  LIB_INSTALLDIR := $(subst /,\,$(OUTDIR))
else
  LIB_OUTDIR = $(OUTDIR)
  LIB_INSTALLDIR = $(OUTDIR)
endif

#------------------------------------------------------------------------------ 
# path variables
#------------------------------------------------------------------------------ 

LIBPATH := $(LIB_OUTDIR) $(LDDIR) $(abspath $(LIBPATH))
INCPATH	:= $(abspath $(INCPATH)) $(abspath .) $(OUTDIR)

#------------------------------------------------------------------------------ 
# program output files
#------------------------------------------------------------------------------ 
ifdef PROG
  ifeq ($(strip $(CROSS_COMPILE)),)
    ifneq ($(HOST),Linux)
    	PROG_BIN := $(OUTDIR)/$(PROG).exe
    else
    	PROG_BIN := $(OUTDIR)/$(PROG)
    endif
  else
    PROG_BIN := $(OUTDIR)/$(PROG).bin
  endif
  PROG_SREC := $(OUTDIR)/$(PROG).srec
  PROG_MAP := $(OUTDIR)/$(PROG).map
  PROG_ELF := $(OUTDIR)/$(PROG).elf
  PROG_SYM := $(OUTDIR)/$(PROG).sym
  PROG_LST := $(OUTDIR)/$(PROG).lst
  PROG_TAG := $(OUTDIR)/$(PROG).tag
endif

ifeq ($(HOST),Cygwin)
  INCPATH_WIN := $(subst \,\\,$(foreach h,$(INCPATH),$(shell cygpath -w $h)))
  OFILES_WIN := $(subst \,\\,$(foreach o,$(OFILES),$(shell cygpath -w $o)))
  LIBPATH_WIN := $(subst \,\\,$(foreach l,$(LIBPATH),$(shell cygpath -w $l)))
  ifdef PROG_BIN
    PROG_BIN_WIN := $(subst \,\\,$(shell cygpath -w $(PROG_BIN)))
  endif
  ifdef PROG_ELF
    PROG_ELF_WIN := $(subst \,\\,$(shell cygpath -w $(PROG_ELF)))
  endif
  ifdef PROG_LST
    PROG_LST_WIN := $(subst \,\\,$(shell cygpath -w $(PROG_LST)))
  endif
  ifdef PROG_SYM
    PROG_SYM_WIN := $(subst \,\\,$(shell cygpath -w $(PROG_SYM)))
  endif
endif

GFILES := $(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT) 

PFILES := $(PROG_BIN) $(PROG_SREC) $(PROG_ELF) $(PROG_LST) \
		  $(PROG_SYM) $(PROG_MAP)

ifeq (Windows,$(HOST))
  CLEAN_OFILES := $(strip $(subst /,\,$(OFILES)))
  CLEAN_DFILES := $(strip $(subst /,\,$(DFILES)))
  CLEAN_GFILES := $(strip $(subst /,\,$(GFILES)))
  CLEAN_PFILES := $(strip $(subst /,\,$(PFILES)))
  LIB_INSTALLDIR := $(subst /,\,$(OUTDIR))
  LIB_OUTDIR := $(subst /,\,$(OUTDIR))
  INSTALLDIR := $(subst /,\,$(INSTALLDIR))
else
  CLEAN_OFILES := $(strip $(OFILES))
  CLEAN_DFILES := $(strip $(DFILES))
  CLEAN_GFILES := $(strip $(GFILES))
  CLEAN_PFILES := $(strip $(PFILES))
  LIB_OUTDIR = $(OUTDIR)
  LIB_INSTALLDIR = $(OUTDIR)
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
				 'LIBPATH=$(LIBPATH)'\
				 'LIBDIR=$(LIB_OUTDIR)'\
				 'INSTALLDIR=$(LIB_INSTALLDIR)'

LIBDIRS_ALL := $(LIBDIRS:%=%-all)

LIBDIRS_CLEAN := $(LIBDIRS:%=%-clean)

LIBDIRS_INSTALL := $(LIBDIRS:%=%-install)

#------------------------------------------------------------------------------ 
# Make scripts debug
#------------------------------------------------------------------------------ 

$(call trace1,<prog.mk> ------------------------------------------------------)
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
#$(info OSTYPE = '$(OSTYPE)')
#$(info MACHTYPE = '$(MACHTYPE)')
#$(info LIBDIRS_ALL = '$(LIBDIRS_ALL)')
#$(info SET = '$(shell set)')
#$(info LDDIR = '$(LDDIR)')
#$(info BASEDIR = '$(BASEDIR)')
#$(info LIB_OUTDIR = '$(LIB_OUTDIR)')
#$(info LIB_INSTALLDIR = '$(LIB_INSTALLDIR)')
#$(info DFILES = '$(DFILES)')
#$(info CC = '$(CC)')
#$(info INCPATH = '$(INCPATH)')
#$(info LIBDIRS = '$(LIBDIRS)')
#$(info INCPATH = '$(INCPATH)')
#$(info LIBPATH = '$(LIBPATH)')
#$(info abspath = '$(abspath .)')
#$(info realpath = '$(realpath .)')
#$(info MAKE_MODE = '$(MAKE_MODE)')
#$(info CFLAGS = '$(CFLAGS)')
#$(info $(shell set))
$(call trace1,----------------------------------------------------- </prog.mk>)

all: $(LIBDIRS_ALL) $(PROG_BIN) $(PROG_SYM) $(PROG_LST)

clean:: libs-clean
ifneq "$(strip $(CLEAN_OFILES))" ""
	$(Q)$(RMALL) $(CLEAN_OFILES)
endif
ifneq "$(strip $(CLEAN_DFILES))" ""
	$(Q)$(RMALL) $(CLEAN_DFILES)
endif
ifneq "$(strip $(CLEAN_GFILES))" ""
	$(Q)$(RMALL) $(CLEAN_GFILES)
endif
ifneq "$(strip $(CLEAN_PFILES))" ""
	$(Q)$(RMALL) $(CLEAN_PFILES)
endif

prog: $(PROG_BIN)

elf: $(PROG_ELF)

map: $(PROG_MAP)

bin: $(PROG_BIN)

srec: $(PROG_SREC)

sym: $(PROG_SYM)

lst: $(PROG_LST)

libs-all: $(LIBDIRS_ALL)

libs-clean: $(LIBDIRS_CLEAN)

libs-install: $(LIBDIRS_INSTALL)

#------------------------------------------------------------------------------ 
# Helpers to print the binary full path
#------------------------------------------------------------------------------ 

bin_path:
	@$(ECHO) $(PROG_BIN)

elf_path:
	@$(ECHO) $(PROG_ELF)

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

.PHONY: all clean prog elf map bin lst libs-all libs-clean bin_path elf_path 
.PHONY: Debug Release cleanDebug cleanRelease
.PHONY: $(LIBDIRS_ALL) $(LIBDIRS_CLEAN) $(LIBDIRS_INSTALL)

#------------------------------------------------------------------------------ 
# Library dependencies targets
#------------------------------------------------------------------------------ 

$(LIBDIRS_ALL):
	$(ACTION) "Building : $@"
ifeq (Windows,$(HOST))
	$(Q)$(MAKE) -C $(@:%-all=%) OUTDIR=$(LIB_OUTDIR)\$(notdir $(@:%-all=%)) $(FLAGS_TO_PASS) all
else
	$(Q)$(MAKE) -C $(@:%-all=%) OUTDIR=$(LIB_OUTDIR)/$(notdir $(@:%-all=%)) $(FLAGS_TO_PASS) all
endif

$(LIBDIRS_CLEAN):
	$(ACTION) "Cleaning : $@"
ifeq (Windows,$(HOST))
	$(Q)$(MAKE) -C $(@:%-clean=%) OUTDIR=$(LIB_OUTDIR)\$(notdir $(@:%-clean=%)) $(FLAGS_TO_PASS) clean
else
	$(Q)$(MAKE) -C $(@:%-clean=%) OUTDIR=$(LIB_OUTDIR)/$(notdir $(@:%-clean=%)) $(FLAGS_TO_PASS) clean
endif

$(LIBDIRS_INSTALL):
	$(ACTION) "Installing : $@"
ifeq (Windows,$(HOST))
	$(Q)$(MAKE) -C $(@:%-install=%) OUTDIR=$(LIB_OUTDIR)\$(notdir $(@:%-install=%)) $(FLAGS_TO_PASS) install
else
	$(Q)$(MAKE) -C $(@:%-install=%) OUTDIR=$(LIB_OUTDIR)/$(notdir $(@:%-install=%)) $(FLAGS_TO_PASS) install
endif

#------------------------------------------------------------------------------ 
# Program targets
#------------------------------------------------------------------------------ 

$(PROG_ELF) $(PROG_MAP): $(LIBDIRS_ALL) $(OFILES) $(OBJ_EXTRA)
	$(ACTION) "LD: $(PROG_ELF)"
ifeq ($(HOST),Cygwin)
	$(Q)$(LD) $(LDFLAGS) $(OFILES_WIN) $(OBJ_EXTRA) -Wl,--print-map -Wl,--cref -Wl,--sort-common -Wl,--start-group $(addprefix -l,$(LIBS)) -Wl,--end-group $(addprefix -L,$(LIBPATH_WIN)) -o $(PROG_ELF_WIN) > $(PROG_MAP)
else
	$(Q)$(LD) $(LDFLAGS) $(OFILES) $(OBJ_EXTRA) -Wl,--print-map -Wl,--cref -Wl,--sort-common -Wl,--start-group $(addprefix -l,$(LIBS)) -Wl,--end-group $(addprefix -L,$(LIBPATH)) -o $(PROG_ELF) > $(PROG_MAP)
endif

%.sym: %.elf
	$(ACTION) "SYM: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(OBJDUMP) -t $(PROG_ELF_WIN) | sort > $@
else
	$(Q)$(OBJDUMP) -t $< | sort > $@
endif

%.lst: %.elf
	$(ACTION) "LST: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(OBJDUMP) -w -d -t -S -r -z $(PROG_ELF_WIN) > $@
else
	$(Q)$(OBJDUMP) -w -d -t -S -r -z $< > $@
endif

ifeq ($(strip $(CROSS_COMPILE)),)
$(PROG_BIN): $(PROG_ELF)
	$(ACTION) "Strip: $(PROG_ELF)"
ifeq ($(HOST),Cygwin)
	$(Q)$(STRIP) -o $(PROG_BIN_WIN) $(PROG_ELF_WIN)
else
	$(Q)$(STRIP) -o $@ $<
endif
endif

%.bin: %.elf
	$(ACTION) "BIN: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(OBJCOPY) -j .init -j .text -j .ARM.extab -j .ARM.exidx -j .data --output-target binary $(subst \,\\,$(shell cygpath -w $<)) $(subst \,\\,$(shell cygpath -w $@))
else
	$(Q)$(OBJCOPY) -j .init -j .text -j .ARM.extab -j .ARM.exidx -j .data --output-target binary $< $@
endif

%.srec: %.elf
	$(ACTION) "SREC: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(OBJCOPY) -j .init -j .text -j .ARM.extab -j .ARM.exidx -j .data --output-target srec $(subst \,\\,$(shell cygpath -w $<)) $(subst \,\\,$(shell cygpath -w $@))
else
	$(Q)$(OBJCOPY) -j .init -j .text -j .ARM.extab -j .ARM.exidx -j .data --output-target srec $< $@
endif

#------------------------------------------------------------------------------ 
# Build tree
#------------------------------------------------------------------------------ 

$(ODIRS):
	$(ACTION) "Creating outdir: $@"
ifeq ($(HOST),Windows)
	$(Q)if not exist $(subst /,\,$@) $(MKDIR) $(subst /,\,$@)
else
	$(Q)$(MKDIR) $@
endif

$(LIBDIRS_INSTALL): | $(ODIRS)

$(LIBDIRS_ALL): | $(ODIRS)

$(HFILES_OUT) $(CFILES_OUT) $(SFILES_OUT): | $(ODIRS)

$(DFILES): | $(ODIRS) $(HFILES_OUT)

$(OFILES): | $(ODIRS) $(HFILES_OUT)

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

#------------------------------------------------------------------------------ 
# Extra stuff
#------------------------------------------------------------------------------ 

ifdef VERSION_MAJOR
  include $(SCRPTDIR)/version.mk
endif

