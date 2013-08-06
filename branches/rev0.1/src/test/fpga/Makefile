# File:		Makefile
# Module: 
# Target:
# Comment:  
# Copyright(C) 2012 Bob Mittmann. All Rights Reserved.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 


PROG = fpga-test

CFILES = fpga-test.c 


LIBDIRS = ../../sdk/libstm32f ../../sdk/libcm3 ../../sdk/libutil \
		  ../../sdk/libthinkos ../../sdk/libc ../../sdk/libice-comm \
		  ../../sdk/libdrv ../../sdk/libtrace ../../sdk/libbitvec \
		  ../../sdk/libaltera \
		  ../../yard-ice/jtag3drv ../../yard-ice/jtag 

LIBS = drv stm32f cm3 util thinkos c ice-comm trace bitvec jtag3drv\
	   jtag altera

INCPATH = ../../sdk/include  ../../yard-ice/include

JTAGTOOL_ADDR = 192.168.0.80

CDEFS = CONFIG_H
FLAGS = -g -O2

HCLK_HZ = 24000000
include ../../mk/stm32f207.mk

