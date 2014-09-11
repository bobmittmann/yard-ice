# File:     Makefile
# Author:   Robinson Mittmann (bobmittmann@gmail.com)
# Comment:
# Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

PROG = calc

CFILES = calc.c calc_vm.c calc_lex.c calc_dbg.c calc_sym.c calc_lib.c

INCPATH = ../include

LIBS =

# Generated files
CFILES_GEN = calc_ll.c
HFILES_GEN = calc_ll.h const_str.h

include cross.mk

ll: calc_ll.c calc_ll.h

calc_ll.h: calc.gr Makefile
	../../ell -e -h $<

calc_ll.c: calc.gr calc_ll.h Makefile
	../../ell -e -c $<

const_str.h: const_str.txt Makefile
	./mkstr.py -n const_str $< > $@

