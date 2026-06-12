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

all: release 

clean: release_clean debug_clean

help:
	@echo 'Targets:'
	@echo
	@echo '  all        - Build ThinkOS and YARD-ICE'
	@echo '  debug      - Build ThinkOS with debug options'
	@echo '  clean      - Remove most generated files'
	@echo

release:
	$(MAKE) -C boot all
	$(MAKE) -C app all

debug:
	$(MAKE) D=2 -C boot all
	$(MAKE) D=1 -C app all

release_clean:
	$(MAKE) D=1 -C boot clean
	$(MAKE) D=1 -C app clean

debug_clean:
	$(MAKE) D=1 -C boot clean
	$(MAKE) D=1 -C app clean

.PHONY: release debug release_clean debug_clean help

