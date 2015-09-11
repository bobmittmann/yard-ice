#
# jtag.mk 
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

ifndef TOOLSDIR
  $(error TOOLSDIR undefined!)
endif	

TFTPLOAD = $(TOOLSDIR)/tftp_load.py
TFTRESET = $(TOOLSDIR)/tftp_reset.py
DCCLOGVIEW = $(TOOLSDIR)/dcclog

ifeq ($(HOST),Windows)
 PYTHON := "C:\Python27\python"
else
 PYTHON := python
endif

ifndef JTAGTOOL_ADDR
  JTAGTOOL_ADDR = 192.168.0.128
endif

ifndef LOAD_ADDR
  LOAD_ADDR = 0x08000000
endif

ifdef JTAG_TARGET
  TARGET = -t $(JTAG_TARGET)
else
  TARGET = 
endif

ifeq (Windows,$(HOST))
  PROG_BIN := $(subst /,\,$(PROG_BIN))
  PROG_ELF := $(subst /,\,$(PROG_ELF))
  TFTPLOAD := $(subst /,\,$(TFTPLOAD))
  TFTPRESET := $(subst /,\,$(TFTPRESET))
  DCCLOGVIEW := $(subst /,\,$(DCCLOGVIEW))
endif

#$(info ----JTAG-JTAG-JTAG-JTAG----)
#$(info OS = '$(OS)')
#$(info HOST = '$(HOST)')
#$(info TOOLSDIR = '$(TOOLSDIR)')
#$(info DIRMODE = '$(DIRMODE)')
#$(info PROG_BIN = '$(PROG_BIN)')
#$(info TFTPLOAD = '$(TFTPLOAD)')
#$(info ----JTAG-JTAG-JTAG-JTAG----)

jtagload: $(PROG_BIN)
	$(ACTION) "Loading: $@"
	$(Q)$(PYTHON) $(TFTPLOAD) -q -i -e -r $(TARGET) -a $(LOAD_ADDR) -h $(JTAGTOOL_ADDR) $(PROG_BIN) 

jtagreset: 
	$(ACTION) "Reseting target..."
	$(Q)$(PYTHON) $(TFTPRESET) -q -a $(LOAD_ADDR) -h $(JTAGTOOL_ADDR) 

logview: $(PROG_ELF)
	$(ACTION) "DCC Logview: $@"
	$(Q) $(DCCLOGVIEW) -h $(JTAGTOOL_ADDR) $(PROG_ELF)

.PHONY: jtagload jtagreset logview

