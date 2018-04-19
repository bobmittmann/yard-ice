----------------------------------------------------------------------------
--
-- PROJECT         DDR-SDRAM Controller Core
--
-- AUTHOR          Bob Mittmann
--                 bobmittmann@gmail.com
--
-- SIMULATOR       
-- COMPILER        
--
-- DATE            
--
-- LANGUAGE        VHDL 93
--
-- LIBRARY         ieee, work
--
-- ABSTRACT        SDRAM DDR Controller
--

----------------------------------------------------------------------------
--
-- Copyright (C) 2018 Bob Mittmann
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

------------------------------------------------------------------------------------------------------------
-- SDRAM DDR Controller
-- 2x16 chips
-- 32bits pipelined read only interface
-- 32bits pipelined write interface
------------------------------------------------------------------------------------------------------------

library ieee, work;
use ieee.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ddr_sdram_dual is
   port (
		-- Clocks and Reset
		clk_i : in std_logic;
		rst_i : in std_logic;
		-- Wishbone Pipelined Controll/Command Interface
		cmd_stb_i : in std_logic;
		cmd_ack_o : out std_logic;
		cmd_stall_o : out std_logic;
		cmd_dat_i : in std_logic_vector(15 downto 0);
		-- Wishbone Pipelined Slave Write Memory Interface
		mwr_cyc_i : in std_logic;
		mwr_we_i : in std_logic;
		mwr_stb_i : in std_logic;
		mwr_ack_o : out std_logic;
		mwr_stall_o : out std_logic;
		mwr_adr_i : in std_logic_vector(20 downto 0);
		mwr_dat_i : in std_logic_vector(31 downto 0);
		mwr_bst_i : in std_logic;
		-- Wishbone Pipelined Slave Read Memory Interface
		mrd_cyc_i : in std_logic;
		mrd_we_i : in std_logic;
		mrd_stb_i : in std_logic;
		mrd_ack_o : out std_logic;
		mrd_stall_o : out std_logic;
		mrd_adr_i : in std_logic_vector(20 downto 0);
		mrd_dat_o : out std_logic_vector(31 downto 0);
		mrd_bst_i : in std_logic;
		-- SDRAM DDR Phy Interface
		phy_burstdet0_i : in std_logic;
		phy_burstdet1_i : in std_logic;
		phy_casn_o : out std_logic;
		phy_dtri0_o : out std_logic;
		phy_dtri1_o : out std_logic;
		phy_dvalid0_o : out std_logic;
		phy_dvalid1_o : out std_logic;
		phy_dqs0_o : out std_logic;
		phy_dqs1_o : out std_logic;
		phy_dqstri0_o : out std_logic;
		phy_dqstri1_o : out std_logic;
		phy_freeze_o : out std_logic;
		phy_lock_i : in std_logic;
		phy_rasn_o : out std_logic;
		phy_read0_o : out std_logic;
		phy_read1_o : out std_logic;
		phy_readclksel00_o : out std_logic;
		phy_readclksel01_o : out std_logic;
		phy_readclksel10_o : out std_logic;
		phy_readclksel11_o : out std_logic;
		phy_uddcntln_o : out std_logic;
		phy_wen_o : out std_logic;
		phy_addr_o : out std_logic_vector(12 downto 0);
		phy_ba_o : out std_logic_vector(1 downto 0);
		phy_cke_o : out std_logic;
		phy_csn0_o : out std_logic; -- Chip 0 #CS
		phy_csn1_o : out std_logic; -- Chip 1 #CS
		phy_dat0_i : in std_logic_vector(15 downto 0); -- Chip 0 data in
		phy_dat1_i : in std_logic_vector(15 downto 0); -- Chip 1 data in
		phy_dat0_o : out std_logic_vector(15 downto 0); -- Chip 0 data out
		phy_dat1_o : out std_logic_vector(15 downto 0) -- Chip 1 data out
	);
	
end;

architecture rtl of ddr_sdram_dual is


begin 


end architecture rtl;


