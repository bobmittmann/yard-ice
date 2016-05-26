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

#
# File:   Makefile
# Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

all: thinkos yard-ice

clean: thinkos_clean yard-ice_clean

help:
	@echo 'Targets:'
	@echo
	@echo '  all        - Build ThinkOS and YARD-ICE'
	@echo '  clean      - Remove most generated files'
	@echo

thinkos:
	$(MAKE) -C boot all

yard-ice:
	$(MAKE) -C app all

thinkos_clean:
	$(MAKE) -C boot clean

yard-ice_clean:
	$(MAKE) -C app clean

.PHONY: thinkos yard-ice thinkos_clean  yard-ice_clean help

