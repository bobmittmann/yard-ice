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

ifndef JTAGTOOL_ADDR
  JTAGTOOL_ADDR = 192.168.0.128
endif

ifndef LOAD_ADDR
  LOAD_ADDR = 0x08000000
endif

jtagload: $(PROG_BIN) $(TFTPLOAD)
	$(ACTION) "Loading: $@"
	$(Q)$(TFTPLOAD) -q -a $(LOAD_ADDR) -h $(JTAGTOOL_ADDR) -r $(PROG_BIN) 

jtagreset: $(TFTPRESET)
	$(ACTION) "Reseting target..."
	$(Q)$(TFTPRESET) -q -a $(LOAD_ADDR) -h $(JTAGTOOL_ADDR) 

jtagrun: $(TFTPRUN)
	$(ACTION) "Running ..."
	$(Q)$(TFTPRUN) -q -a $(LOAD_ADDR) -h $(JTAGTOOL_ADDR) 

logview: $(PROG_ELF)
	$(ACTION) "DCC Logview: $@"
	$(Q) $(DCCLOGVIEW) -h $(JTAGTOOL_ADDR) $(PROG_ELF)

.PHONY: jtagload jtagreset jtagrun logview

