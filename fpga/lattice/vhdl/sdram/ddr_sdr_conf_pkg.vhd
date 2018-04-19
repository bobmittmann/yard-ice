----------------------------------------------------------------------------
--
-- PROJECT         DDR-SDRAM Controller Core
--
-- AUTHOR          Markus Lemke
--                 markus@opencores.org
--                 www.array-electronics.de
--
-- SIMULATOR       Model Technology ModelSim 5.4
-- COMPILER        Exemplar Leonardo Spectrum 2001.1d
--
-- DATE            $Date: 2002/12/20 15:09:51 $
--
-- LANGUAGE        VHDL 93
--
-- LIBRARY         ieee
--
-- ABSTRACT        Core Configuration Package
--                 
----------------------------------------------------------------------------
--
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


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

package ddr_sdr_conf_pkg is

   -- DDR SDRAM Hardware defined constants
   constant DDR_DATA_WIDTH : positive := 16;  -- External DDR-SDRAM Module data bus width
   constant DDR_ADDR_WIDTH : positive := 13;  -- number of address lines to DDR-SDRAM Device/Module
   constant DDR_BANK_WIDTH : positive := 2;   -- Number of BANK address lines of external DDR-SDRAM
   constant DDR_DQS_WIDTH  : positive := DDR_DATA_WIDTH / 8; -- Number of data strobe lines
   constant DDR_DM_WIDTH   : positive := DDR_DATA_WIDTH / 8; -- Number of Data Mask Lines
   
   constant AUTO_PRECHARGE : positive := 10; -- bit-position in column address for auto precharge (see Data Sheet)
   
   -- Derived Constants for the Controller User Interface
   constant U_ADDR_WIDTH   : positive := (2 * DDR_ADDR_WIDTH) + DDR_BANK_WIDTH; -- UIF addr width including bank bits
   constant U_DATA_WIDTH   : positive := 2 * DDR_DATA_WIDTH;  -- because data is clocked by rising and falling edges
   constant U_CMD_WIDTH    : positive := 2; -- user interface command width
   
   -- DDR-SDR TIMING constants ------------------------------------------------------------------
   constant DDR_CLK_PERIOD : positive := 10000; -- ps, for simulation only

   -- After REFRESH_CLOCKS a refresh cycle is necessary, 64ms / 8192 = max every 7.8125 us refesh 
   constant REFRESH_CLOCKS : natural := 780;

   -- These values are for MT46V16M16 @ 100MHz 
   constant TRP  : positive  := 2;   -- precharge command period
   constant TRAS : positive  := 4;   -- active to precharge delay 
   constant TRFC : positive  := 8;   -- auto refresh command period
   constant TMRD : positive  := 2;   -- load mode register command cylce time
   constant TRCD : positive  := 2;   -- active to read or write delay !
   constant TWR  : positive  := 2;   -- write recovery time
   constant REFRESH_WINDOW : positive := 25;  -- refresh window
   
   -- User Interface Commands
   constant NOP_CMD     : std_logic_vector(U_CMD_WIDTH-1 downto 0) := "00";
   constant RD_CMD      : std_logic_vector(U_CMD_WIDTH-1 downto 0) := "01";
   constant WR_CMD      : std_logic_vector(U_CMD_WIDTH-1 downto 0) := "10";
   constant RFSH_CMD    : std_logic_vector(U_CMD_WIDTH-1 downto 0) := "11";
   
   -----------------------------------------------------------------------------------------
   --  MODE Register settings
   constant MODE_REG_DLL_RST : std_logic_vector(DDR_ADDR_WIDTH-1 downto 0) := "0000100100001"; -- CAS 2, BL=2, BT=Seq
   constant MODE_REG_DLL_ENA : std_logic_vector(DDR_ADDR_WIDTH-1 downto 0) := "0000000100001";
   constant CAS_LAT : integer := 2; -- fixed value for synthesis

   -- EXT_MODE_REG: dll enabled, reduced drive strength - all from datasheet
   constant EXT_MODE_REG       : std_logic_vector(DDR_ADDR_WIDTH -1 downto 0) := "00000000000" & '1' & '0';
   constant EXT_MODE_REG_BANK  : std_logic_vector(DDR_BANK_WIDTH -1 downto 0) := "01";
   
   -- BASE_MODE_REG
   constant BASE_MODE_REG_BANK : std_logic_vector(DDR_BANK_WIDTH -1 downto 0) := "00";

   -- Table of Timing Parameters as a function of operation
   type SDR_CMDS_TYPE is (NOP, MODE, EXT_MODE, ACTIVE, PRECHARGE, BURST_RD, BURST_WR, REFRESH, WAIT_WR);

   type PART_IF_TYPE is
      record
         cs_n    : std_logic;
         ras_n   : std_logic;
         cas_n   : std_logic;
         we_n    : std_logic;
         dqmb    : std_logic_vector(7 downto 0);
         ct      : integer range 0 to 10;  
      end record PART_IF_TYPE;

   type PART_IF_ARRAY_TYPE is array (SDR_CMDS_TYPE) of PART_IF_TYPE;

   constant COMMAND : PART_IF_ARRAY_TYPE :=
      -- command                 cs_n     ras_qn      cas_qn      we_qn        dqmb          ct
      (NOP              =>   (    '1',      '1',        '1',        '1',       x"00",        0        ),                                                 
       MODE             =>   (    '0',      '0',        '0',        '0',       x"00",        TMRD-1   ),  
       EXT_MODE         =>   (    '0',      '0',        '0',        '0',       x"00",        TMRD-1   ),  
       ACTIVE           =>   (    '0',      '0',        '1',        '1',       x"00",        TRCD-1   ),                                     
       PRECHARGE        =>   (    '0',      '0',        '1',        '0',       x"00",        TRP-1    ),                                     
       BURST_RD         =>   (    '0',      '1',        '0',        '1',       x"00",        0        ),  
       BURST_WR         =>   (    '0',      '1',        '0',        '0',       x"00",        0        ),  
       REFRESH          =>   (    '0',      '0',        '0',        '1',       x"00",        TRFC-1   ),                                     
       WAIT_WR          =>   (    '1',      '1',        '1',        '1',       x"00",        TWR+TRP-1 )  
       );

   -- Initial Wait Timing
   function get_init_max
      return unsigned;

end ddr_sdr_conf_pkg;

package body ddr_sdr_conf_pkg is
   -- 200 us Counter (Initialization)
   function get_init_max
      return unsigned
   is
      variable max : unsigned(14 downto 0);
   begin
      max := "111111111111111"; -- >= 200 us
      -- pragma translate_off
      max := "000000000010100";  -- 20 for simulation only -> you cannot wait 200us!
      -- pragma translate_on
      return max;
   end ;
end;
