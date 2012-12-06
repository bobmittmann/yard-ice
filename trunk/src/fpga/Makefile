# File:     Makefile
# Author:   Robinson Mittmann (bobmittmann@gmail.com)
# Target:
# Comment:
# Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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

FPGA_PROJ = jtagtool3

FPGA_RBF_ADDR = 0x00010000

VHDL_FILES = cyc_pll.vhd \
			jtag_clk_scaler.vhdl\
			jtag_counter.vhdl\
			jtag_desc_ram.vhdl\
			jtag_enh_ckgen.vhdl\
			jtag_enh_ecu.vhdl\
			jtag_enh_io.vhdl\
			jtag_enh_rx.vhdl\
			jtag_enh_shift_ctrl.vhdl\
			jtag_enh_shifter.vhdl\
			jtag_enh_tx.vhdl\
			jtag_io_dpram.vhdl\
			jtag_led_drv.vhdl\
			jtag_reg.vhdl\
			jtagtool3.vhdl

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

include ../mk/altera.mk

rbf: $(FPGA_RBF)
	@$(TFTPLOAD) -a $(FPGA_RBF_ADDR) -h $(JTAGTOOL_ADDR) -r $<

