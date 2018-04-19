----------------------------------------------------------------------------
--
-- PROJECT         DDR-SDRAM Controller Core
--
-- AUTHOR          Markus Lemke
--                 markus@opencores.org
--                 www.array-electronics.de
--
-- DATE            $Date: 2003/03/19 06:53:31 $
--
-- ABSTRACT        Documentation File for the
--                 DDR SDRAM Controller Core
--
--
----------------------------------------------------------------------------
-- Copyright (C) 2002 Markus Lemke, www.array-electronics.de
--  
-- Everyone is permitted to copy and distribute  and  modify 
-- this  document  under  the  terms of the OpenIPCore Hardware
-- General  Public License "OHGPL" which can  be  read  in  the
-- file LICENSE.
--  
-- The  License  grants  you  the right to copy, modify  and
-- redistribute this file,  but only under  certain  conditions 
-- described in the License.  Among other things,  the  License
-- requires that  this  copyright  notice  and  the  associated
-- disclaimer  be preserved on  all copies.  Every copy of this
-- file must include a copy of the License, normally in a plain
-- ASCII text file named LICENSE.
--  
-- 
-- DISCLAIMER
-- 
-- THIS SOFTWARE  IS  PROVIDED  ``AS IS''  AND  WITHOUT ANY  
-- EXPRESS  OR  IMPLIED  WARRANTIES, INCLUDING, BUT NOT LIMITED 
-- TO, THE  IMPLIED  WARRANTIES OF  MERCHANTABILITY AND FITNESS
-- FOR A  PARTICULAR  PURPOSE.  IN NO EVENT SHALL THE AUTHOR OR 
-- CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
-- SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING,
-- BUT NOT  LIMITED TO,  PROCUREMENT  OF  SUBSTITUTE  GOODS  OR 
-- SERVICES;  LOSS  OF  USE,  DATA,  OR  PROFITS;  OR  BUSINESS 
-- INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY O LIABILITY,
-- WHETHER  IN CONTRACT, STRICT  LIABILITY, OR  TORT (INCLUDING 
-- NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT OF THE  USE 
-- OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
-- DAMAGE.                              
----------------------------------------------------------------------------

-- $Log: readme.txt,v $
-- Revision 1.1.2.2  2003/03/19 06:53:31  markus
-- Version 1.1: Bugfix User Interface,
-- simplified timing constraints
--
-- Revision 1.1.2.1  2003/01/08 14:58:27  markus
-- Initial Opencores Revision
--

Contents
---------

1. Files
2. Brief Introduction
3. Features
4. Functional Description
 4.1 Signals
 4.2 Clock Generation
 4.3 Commands
 4.4 User Adress Mapping
 4.5 Write Operation
 4.6 Read Operation
5. Core Configuration
6. Simulation
7. Hardware Implementation Notes
8. Extended DDR SDRAM Controller


1.) Files
----------
The DDR SDRAM Controller Core contains the following files:
  
 Synthesis : 
   
   ddr_sdr_conf_pkg.vhd
   reset.vhd
   ddr_dcm.vhd
   user_if.vhd
   ddr_sdr.vhd
   ddr_sdr.ucf
   
 Simulation :

   mt46v16m16.vhd
   tb.vhd

 and :
 
 readme.txt (this file)
 license
 

2.) Brief Introduction
-----------------------

DDR SDRAM Controller Core

-  has been designed for use in XILINX Virtex II FPGAs
-  works with DDR SDRAM Device MT46V16M16 without changes
-  may be easily adapted to any other DDR SDRAM device



3.) Features
-------------

-  up to 100 (133) MHz system clock frequency in -4 (-5) speed grade
-  BURST LENGTH : 2
-  CAS latency  : 2
-  Commands : NOP, READ, WRITE
-  Automatic, configurable Auto Refresh
-  Automatic precharge/activate when changing ROW/BANK
-  All main parameters are configurable
-  Unidirectional DQS signals (only for write operations)
-  Data mask signals not used


4.) Functional Description
---------------------------

4.1) Signals
-------------

      // Clock and RESET signals
      rst_n              | IN  |  external asynchronous reset, low active
      
      clk                | IN  |  system clock (e.g. 100MHz), from fpga pad

      sys_rst_qn         | OUT |  sync reset low active, released after DCMs are locked,
                                  may be used by other modules inside the FPGA
                            
      sys_clk_out        | OUT |  system clock, dcm output, may be used by other modules 
                                  inside the FPGA as global clock
                                  
      clk_fb             | IN  |  DCM feedback clock, must be external connected to ddr_sdr_clk !
      
      // User Interface signals
      cmd                | IN  |  User command: READ, WRITE, NOP
      cmd_vld            | IN  |  User command valid (if '1')
      addr               | IN  |  User address, contains (ROW & BANK & COL), see Address Mapping 
      busy_q             | OUT |  Controller busy flag, commands are ignored when active
      
      // Data Interface
      data_in            | IN  |  User input data (written to DDR SDRAM)
      data_req_q         | OUT |  User data request, controls input data flow
      data_out_q         | OUT |  User data output (read from DDR SDRAM)
      data_vld_q         | OUT |  data_out_q is valid when '1'

      // DDR SDRAM external signals
      sdr_clk            | OUT |  DDR SDRAM Clock
      sdr_clk_n          | OUT |  Inverted DDR SDRAM Clock
      cke_q              | OUT |  DDR SDRAM clock enable
      cs_qn              | OUT |  DDR SDRAM /chip select
      ras_qn             | OUT |  DDR SDRAM /ras
      cas_qn             | OUT |  DDR SDRAM /cas
      we_qn              | OUT |  DDR SDRAM /write enable
      dm_q               | OUT |  DDR SDRAM data mask bits, all set to "0"
      dqs_q              | OUT |  DDR SDRAM data strobe, used only for write operations
      ba_q               | OUT |  DDR SDRAM bank select
      a_q                | OUT |  DDR SDRAM address bus 
      data               | INOUT |  DDR SDRAM bidirectional data bus
                          
      // Status signals
      dcm_error_q        | OUT |  Indicates DCM Errors

      
4.2) Clock Generation
----------------------

The DDR SDRAM Controller Core uses 2 XILINX Virtex2 DCM's to generate
the clocks for the DDR SDRAM device and the core.

The phase shift of DCM 0 should be adjusted, depending an board delays.
Set variable ps in the function <func_phase_shift> to the appropriate value
(ddr_sdr.vhd)

The actual value of DCM 0 phase shift is optimized for use with the
INSIGHT Xilinx Virtex2 DDR Development Board.

NOTE: clk_fb must be externally connected to sdr_clk, otherwise the DCMs
      will not lock.

      
4.3) Commands
--------------

The following user commands are supported:

cmd   command name
---   ------------
0x0   NOP
0x1   READ
0x2   WRITE


4.4)  User Address Mapping
---------------------------

The address lines are mapped to the DDR SDRAM device address bits as follows:

addr = (row, bank, column)

with

row (DDR_ADDR_WIDTH - 1 : 0)
bank (DDR_BANK_WIDTH -1 : 0)
column (DDR_ADDR_WIDTH - 1 : 0)

MSB is on the left side, LSB on the right.

NOTE: A simple increment by 2 may not work for address generation in most cases. See the example.


EXAMPLE:

DDR SDRAM = MT46V16M16 ( 4 Meg x 16 x 4 banks)

DDR_ADDR_WIDTH =  13
DDR_BANK_WIDTH =  2

USER_ADDR_WIDTH = 2 x DDR_ADDR_WIDTH + DDR_BANK_WIDTH = 28

In this case, addr is configured to 28 bits width (but only 24 bits are used).

According to the DDR SDRAM data sheet, all 13 DDR SDRAM address-bits are used for
row addressing (A12 - A0), but only 9 bits are used for column addressing (A8 - A0). 

A10 is the auto precharge bit, A9, A11, and A12 are not used when the column address is applied.

Assuming the user address signal is 'user_addr(23 downto 0)' and the auto precharge bit is not used,
then address mapping is done as follows:

addr <= user_addr(23 downto 9) & "0000" & user_addr(8 downto 0);

        |   DDR ROW & BANK   | & |   DDR COLUMN ADDR          |


        
4.5)  WRITE Operation
----------------------
A write operation is requested at the user interface by applying the WRITE command
to cmd and setting cmd_vld ='1'.

This write operation is accepted with the rising edge of sys_clk and busy_q ='0'.

Write data flow is controller by data_req_q. Data at data_in is registered
exactly 2 clocks after data_req_q was set '1':

             ____      ____      ____      ____      ____      
clk        _/    \____/    \____/    \____/    \____/    \____
              _________
data_req_q __/         \_______________________________________
           ______________________ ____________ ________________
data_in    _________XXX__________|_data valid |______XXX_______


If there is a difference in the ROW or BANK, used during the last access,
the controller performs a PRECHARGE ALL, followed by an ACTIVATE command.
Otherwise, the write operation starts immediately.


4.6)  READ Operation
----------------------

A read operation is requested at the user interface by applying the READ command
to cmd and setting cmd_vld ='1'.

This read operation is accepted with the rising edge of sys_clk and busy_q ='0'.

Read data at data_out_q is valid exactly when data_vld_q='1':

             ____      ____      ____      ____      ____      
clk        _/    \____/    \____/    \____/    \____/    \____
                        _________
data_vld_q ____________/         \____________________________
           ____________ _________ ____________________________
data_out_q _____XXX____|_d valid |______XXX___________________


If there is a difference in the ROW or BANK, used during the last access,
the controller performs a PRECHARGE ALL, followed by an ACTIVATE command.
Otherwise, the read operation starts immediately.

NOTE: dqs signals are still not used for reading data.


5.)   Core Configuration
-------------------------

All core configuration is done by changing parameters in the following file:

ddr_sdr_conf_pkg.vhd

The given comments in this file should be sufficient to adapt the core
to different DDR SDRAM Memory devices.
Note, that the CAS Latency may not be changed, as well as the burst length.


6.)   Simulation
-----------------

A simple testbench is available to demonstrate write and read operations.

First of all, you should compile the XILINX library 'unisim' into a subdirectory
of your working directory.
This may be done as follows (you are in your working directory):

vlib unisim

vcom -87 -work unisim  <XilinxPath>\vhdl\src\unisims\unisim_vpkg.vhd
vcom -87 -work unisim  <XilinxPath>\vhdl\src\unisims\unisim_vcomp.vhd
vcom -87 -work unisim  <XilinxPath>\vhdl\src\unisims\unisim_vital.vhd

where <XilinxPath> is the path to your XILINX software, e.g. c:\m4_2is2
(this may take some minutes ..have a cup of coffee here...)

The next step is compilation of all source files into the library 'work'.

vlib work
vcom -93 ddr_sdr_conf_pkg.vhd reset.vhd ddr_dcm.vhd user_if.vhd ddr_sdr.vhd mt46v16m16.vhd tb.vhd

Now you can start simulation using the following commands in MODELSIM:

vsim -t ps tb
view wave
add wave *
run -all

Simulation time is about 132.000 ns

At first, a block of data words is written to the DDR SDRAM. You may change the
value of MAX_WORDS in tb.vhd for longer or shorter simulation.
After writing, the data is read back from the DDR SDRAM device and checked
for errors.
Simulation stops automatically if you have set the checkbox "Brake on Assertion -> FAILURE"
in MODELSIM simulator. This is no error !

Don't care about the two warnings 'MAXCLKPERIOD VIOLATION ON CLKFX'. This is because both
DCM's get a reset a short time after the simulation has been started.



7.)   Hardware Implementation Notes
------------------------------------

* Global clock pins (GCK) at the same device edge should be used for the following signals:
         
clk         e.g. B11
clk_fb      e.g. F13

*  All data IO's must use both INFFs, OUTFFs and one of the Tristate-FFs,
   resulting in the following mapper report (example):

| data<0>                            | IOB     | BIDIR     | SSTL2_I     |          |      | INFF1 
|                                    |         |           |             |          |      | INFF2 
|                                    |         |           |             |          |      | OUTDDR
|                                    |         |           |             |          |      | ENFF2 

*  The IOSTANDARD should be SSTL2. Note that you have to use the same IOSTANDARD for all
   IOB's within one IO-bank !!!
   
*  SLEWRATE of all outputs driving DDR SDRAM signals should be set to FAST

*  The IOB delay element of all input signals should be switched OFF, using NODELAY

*  A global period timespec and an additional timespec for the reset path is sufficient
   for the entire core implementation.
   You may copy these contraints as well as the IOB-constraints from the example UCF-File <ddr_sdr.ucf>
   
*  The DDR SDRAM Controller design is synthesizable with XILINX XST synthesis tool when using
   the following option : ... -equivalent_register_removal No

   
   
8.)   Extended DDR SDRAM Core
------------------------------

If you are looking for a more powerful DDR SDRAM Controller Core with
advanced features, such as

- Multiple data channels
- Dynamic burst length support (2,4,8)
- Support for data mask signals
- Automatic bank management with four banks
- Data transfer speed up to 400 MBytes/s

please visit our homepage :

www.array-electronics.de

or send me an e-mail :

markus.lemke@array-electronics.de

 

