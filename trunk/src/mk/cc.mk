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

compile = $(CC) $(addprefix -I,$(INCPATH)) $(DEFINES) $(CFLAGS)
assemble = $(AS) $(addprefix -I,$(INCPATH)) $(DEFINES) $(SFLAGS)

#------------------------------------------------------------------------------ 
# Automatically generated source code files
#------------------------------------------------------------------------------ 

$(DEPDIR)/%.d : $(OUTDIR)/%.c
	$(ACTION) "DEP .c 2: $@"
	$(Q)$(compile) -MT $@ -MD -MP -MM -c -o $@ $<

$(DEPDIR)/%.d : $(OUTDIR)/%.S
	$(ACTION) "DEP .S 2: $@"
	$(Q)$(assemble) -MT $@ -MD -MP -MM -c -o $@ $<

$(OUTDIR)/%.o : $(OUTDIR)/%.c $(DEPDIR)/%.d
	$(ACTION) "CC 2: $@"
	$(Q)$(compile) -o $@ -c $<

$(OUTDIR)/%.o : $(OUTDIR)/%.S $(DEPDIR)/%.d
	$(ACTION) "AS: $@"
	$(Q)$(assemble) -o $@ -c $<

#------------------------------------------------------------------------------ 

$(DEPDIR)/%.d : %.c
	$(ACTION) "DEP .c 1: $@"
	$(Q)$(compile) -MT $@ -MD -MP -MM -c -o $@ $<

$(DEPDIR)/%.d : %.S
	$(ACTION) "DEP .S 1: $@"
	$(Q)$(assemble) -MT $@ -MD -MP -MM -c -o $@ $<

$(OUTDIR)/%.o : %.c $(DEPDIR)/%.d
	$(ACTION) "CC 1: $@"
	$(Q)$(compile) -o $@ -c $<

$(OUTDIR)/%.o : %.S $(DEPDIR)/%.d
	$(ACTION) "AS: $@"
	$(Q)$(assemble) -o $@ -c $<

#------------------------------------------------------------------------------ 

