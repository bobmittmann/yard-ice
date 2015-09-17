#
# cc.mk 
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

.SUFFIXES:

ifneq (,$(INCPATH_WIN))
 compile = $(CC) $(addprefix -I,$(INCPATH_WIN)) $(DEFINES) $(CFLAGS)
 assemble = $(AS) $(addprefix -I,$(INCPATH_WIN)) $(DEFINES) $(SFLAGS)
else
 compile = $(CC) $(addprefix -I,$(INCPATH)) $(DEFINES) $(CFLAGS)
 assemble = $(AS) $(addprefix -I,$(INCPATH)) $(DEFINES) $(SFLAGS)
endif

#------------------------------------------------------------------------------ 
# Automatically generated source code files
#------------------------------------------------------------------------------ 

$(OUTDIR)/%.o : $(OUTDIR)/%.c 
	$(ACTION) "CC 2: $@"
	$(Q)$(compile) -o $@ -c $<

$(OUTDIR)/%.o : $(OUTDIR)/%.S 
	$(ACTION) "AS: $@"
	$(Q)$(assemble) -o $@ -c $<

$(OUTDIR)/%.o : $(OUTDIR)/%.s 
	$(ACTION) "AS: $@"
	$(Q)$(assemble) -o $@ -c $<

#------------------------------------------------------------------------------ 

#
# Cygwin hosts with Native Linux compilers are particularly problematic
# The make uses UNIX paths but the compiler uses Windows paths ????
#

$(OUTDIR)/%.o : $(SRCDIR)/%.c
	$(ACTION) "CC 1: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(compile) -o $(subst \,\\,$(shell cygpath -w $@)) -c $(subst \,\\,$(shell cygpath -w $<))
else ifeq ($(HOST),Windows)
	$(Q)$(compile) -MMD -o $@ -c $<
else ifeq ($(HOST),Msys)
	$(Q)$(compile) -MMD -o $@ -c $<
else
	$(Q)$(compile) -MMD -o $@ -c $<
endif

$(OUTDIR)/%.o : $(SRCDIR)/%.S
	$(ACTION) "AS: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(assemble) -o $(subst \,\\,$(shell cygpath -w $@)) -c $(subst \,\\,$(shell cygpath -w $<))
else
	$(Q)$(assemble) -MD -MP -MT $@ -o $@ -c $<
endif

$(OUTDIR)/%.o : $(SRCDIR)/%.s 
	$(ACTION) "AS: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(assemble) -o $(subst \,\\,$(shell cygpath -w $@)) -c $(subst \,\\,$(shell cygpath -w $<))
else
	$(Q)$(assemble) -MD -MP -MT $@ -o $@ -c $<
endif

#------------------------------------------------------------------------------ 
# some rules to compile files on adjacent folders...

$(OUTDIR)/%.o : $(SRCDIR)/../%.c 
	$(ACTION) "CC 2: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(compile) -o $(subst \,\\,$(shell cygpath -w $@)) -c $(subst \,\\,$(shell cygpath -w $<))
else
	$(Q)$(compile) -MMD -MP -MT $@ -o $@ -c $<
endif

$(OUTDIR)/%.o : $(SRCDIR)/../%.S
	$(ACTION) "AS: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(assemble) -o $(subst \,\\,$(shell cygpath -w $@)) -c $(subst \,\\,$(shell cygpath -w $<))
else
	$(Q)$(assemble) -MD -MP -MT $@ -o $@ -c $<
endif

$(OUTDIR)/%.o : $(SRCDIR)/../%.s
	$(ACTION) "AS: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(assemble) -o $(subst \,\\,$(shell cygpath -w $@)) -c $(subst \,\\,$(shell cygpath -w $<))
else
	$(Q)$(assemble) -MD -MP -MT $@ -o $@ -c $<
endif

#------------------------------------------------------------------------------ 
# some rules to compile files on parent folders...

$(OUTDIR)/%.o : $(SRCDIR)/../../%.c
	$(ACTION) "CC 3: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(compile) -o $(subst \,\\,$(shell cygpath -w $@)) -c $(subst \,\\,$(shell cygpath -w $<))
else
	$(Q)$(compile) -MMD -MP -MT $@ -o $@ -c $<
endif

$(OUTDIR)/%.o : $(SRCDIR)/../../%.S
	$(ACTION) "AS: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(assemble) -o $(subst \,\\,$(shell cygpath -w $@)) -c $(subst \,\\,$(shell cygpath -w $<))
else
	$(Q)$(assemble) -MD -MP -MT $@ -o $@ -c $<
endif

$(OUTDIR)/%.o : $(SRCDIR)/../../%.s
	$(ACTION) "AS: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(assemble) -o $(subst \,\\,$(shell cygpath -w $@)) -c $(subst \,\\,$(shell cygpath -w $<))
else
	$(Q)$(assemble) -MD -MP -MT $@ -o $@ -c $<
endif

#------------------------------------------------------------------------------ 
# some rules to compile files on grand-parent folders...

$(OUTDIR)/%.o : $(SRCDIR)/../../../%.c
	$(ACTION) "CC 4: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(compile) -o $(subst \,\\,$(shell cygpath -w $@)) -c $(subst \,\\,$(shell cygpath -w $<))
else
	$(Q)$(compile) -MMD -o $@ -c $<
endif

$(OUTDIR)/%.o : $(SRCDIR)/../../../%.S
	$(ACTION) "AS: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(assemble) -o $(subst \,\\,$(shell cygpath -w $@)) -c $(subst \,\\,$(shell cygpath -w $<))
else
	$(Q)$(assemble) -MD -MP -MT $@ -o $@ -c $<
endif

$(OUTDIR)/%.o : $(SRCDIR)/../../../%.s
	$(ACTION) "AS: $@"
ifeq ($(HOST),Cygwin)
	$(Q)$(assemble) -o $(subst \,\\,$(shell cygpath -w $@)) -c $(subst \,\\,$(shell cygpath -w $<))
else
	$(Q)$(assemble) -MD -MP -MT $@ -o $@ -c $<
endif

#------------------------------------------------------------------------------ 

