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


PROG = fsmc-test

CFILES = fsmc-test.c

LIBDIRS = ../../../libstm32f ../../../libcm3 ../../../libutil \
		  ../../../libthinkos ../../../libc ../../../libice-comm \
		  ../../../libhexdump ../../../libdrv ../../../libaltera

LIBS = stm32f cm3 util thinkos c ice-comm hexdump drv altera

INCPATH = ../../../include

JTAGTOOL_ADDR = 192.168.0.80

CFLAGS = -g -O2

HCLK_HZ = 24000000
include ../../../../mk/stm32f207.mk

FPGA_RBF_ADDR = 0x08040000

FPGA_PROJ = fsmc_test
VHDL_FILES = clk_scaler.vhdl led_drv.vhdl sram.vhdl counter.vhdl memcpy.vhdl \
			 register.vhdl dpram.vhdl cram16.vhdl syncfifo.vhdl fsmc-test.vhdl \
			 cram_adaptor.vhdl
LOCATIONS = locations.txt
INSTANCES = instances.txt
FAMILY = Cyclone
DEVICE = EP1C3T100C8
SYNTHESIS_EFFORT = AUTO
OPTIMIZE_HOLD_TIMING = OFF
OPTIMIZE_MULTI_CORNER_TIMING = ON
OPTIMIZE_POWER_DURING_SYNTHESIS = OFF
HDL_MESSAGE_LEVEL = LEVEL2
STATE_MACHINE_PROCESSING = ONE-HOT
SYNTH_MESSAGE_LEVEL = LOW

include ../../../../mk/altera.mk

rbf: $(FPGA_RBF)
	@$(TFTPLOAD) -a $(FPGA_RBF_ADDR) -h $(JTAGTOOL_ADDR) -r $<

rbf-load:
	@$(TFTPLOAD) -a $(FPGA_RBF_ADDR) -h $(JTAGTOOL_ADDR) -r $(FPGA_RBF)
