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
-- DATE            $Date: 2003/01/08 12:56:12 $
--
-- LANGUAGE        VHDL 93
--
-- LIBRARY         ieee, work
--
-- ABSTRACT        DDR-SDRAM Controller Testbench
--                 This simple testbench shows, how to write and read a data block
--                 to the DDR SDRAM device using the controller core
--                 Automatic error check is included
--                 MAX_WORDS controls the number of write and read operations
--                 (each of 4 bytes), resulting in e.g. 8 kbytes, if MAX_WORDS=2048
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


library ieee, work;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.ddr_sdr_conf_pkg.all;

entity tb is
   procedure gen_clock(signal ckr : out std_logic; constant period : positive)
   is
   begin
      loop
         ckr <= '1', '0' after period/2 * ps;
         wait for period * ps;
      end loop;
   end;
   
end;

architecture struct of tb is

   -- Number of user data words for simulation
   constant MAX_WORDS    : integer := 2048;

   signal rst_n          : std_logic;  
   signal clk            : std_logic;  
   signal clk_fb         : std_logic;
   signal sys_rst_qn     : std_logic;
   signal sys_clk_out    : std_logic;
   
   signal cmd            : std_logic_vector(U_CMD_WIDTH-1 downto 0);
   signal cmd_vld        : std_logic;
   signal addr           : std_logic_vector(U_ADDR_WIDTH-1 downto 0);
   signal busy_q         : std_logic;
   signal data_in        : std_logic_vector(U_DATA_WIDTH-1 downto 0);
   signal data_req_q     : std_logic;
   signal data_out_q     : std_logic_vector(U_DATA_WIDTH-1 downto 0);
   signal data_vld_q     : std_logic;
   signal dcm_error_q    : std_logic;

   signal sdr_clk        :  std_logic;  
   signal sdr_clk_n      :  std_logic;  
   signal cke_q          :  std_logic;  
   signal cs_qn          :  std_logic;  
   signal ras_qn         :  std_logic;  
   signal cas_qn         :  std_logic;  
   signal we_qn          :  std_logic;  
   signal dm_q           :  std_logic_vector(DDR_DM_WIDTH-1 downto 0);  
   signal dqs_q          :  std_logic_vector(DDR_DQS_WIDTH-1 downto 0);  
   signal ba_q           :  std_logic_vector(DDR_BANK_WIDTH-1 downto 0);  
   signal a_q            :  std_logic_vector(DDR_ADDR_WIDTH -1 downto 0);  
   signal data           :  std_logic_vector(DDR_DATA_WIDTH -1 downto 0);  
   signal user_addr      : std_logic_vector(U_ADDR_WIDTH-1 downto 0);
       
   signal refresh        : boolean:= true;
   
    
begin 
   gen_clock(clk, DDR_CLK_PERIOD);

   -- Board Conection DDR SDRAM Clock -> DCM feedback
   clk_fb   <= sdr_clk after 1 ns;
   
   -- DDR SDRAM Controller Core
   i_ddr_sdr : entity work.ddr_sdr
      port map (
         -- Clocks and Reset
         rst_n                 => rst_n,
         clk                   => clk,

         sys_rst_qn            => sys_rst_qn,
         sys_clk_out           => sys_clk_out,
         clk_fb                => clk_fb,
      
         -- User Interface
         cmd                   => cmd,
         cmd_vld               => cmd_vld,
         addr                  => addr,
         busy_q                => busy_q,
      
         -- Input Data
         data_in               => data_in,
         data_req_q            => data_req_q,
         data_out_q            => data_out_q,
         data_vld_q            => data_vld_q,

         -- DDR SDRAM Signals
         sdr_clk               => sdr_clk,
         sdr_clk_n             => sdr_clk_n,
         cke_q                 => cke_q,
         cs_qn                 => cs_qn,
         ras_qn                => ras_qn,
         cas_qn                => cas_qn,
         we_qn                 => we_qn,
         dm_q                  => dm_q,
         dqs_q                 => dqs_q,
         ba_q                  => ba_q,
         a_q                   => a_q,
         data                  => data,
                             
         -- Status
         dcm_error_q           => dcm_error_q
     );
   
   -- MICRON DDR SDRAM Simulation Model
   i_mt46v16m16 : entity work.mt46v16m16 
      port map (
         dq    => data,
         dqs   => dqs_q,
         addr  => a_q,
         ba    => ba_q,
         clk   => sdr_clk,
         clk_n => sdr_clk_n,
         cke   => cke_q,
         cs_n  => cs_qn,
         ras_n => ras_qn,
         cas_n => cas_qn,
         we_n  => we_qn,
         dm    => dm_q
         );
         
   -- RESET
   rst_n <= '0', '1' after 100 ns;

   refresh <= true when falling_edge(ras_qn) and falling_edge(cas_qn) and we_qn='1' else false;
   
   process
      variable num_words : integer := 0;
   begin
      
      -- Write MAX_WORDS to DDR SDRAM Controller
      num_words := 0;
      cmd       <= WR_CMD;
      cmd_vld   <= '1';
      user_addr <= (others=>'0');
      
      while num_words < MAX_WORDS loop
         wait until rising_edge(sys_clk_out);
         
         if busy_q='0' then 
            user_addr <= user_addr +2;
            num_words := num_words+1;
         end if;
      end loop;
      cmd_vld <= '0';
      

      -- Wait some clocks before reading back from DDR SDRAM
      for i in 0 to 20 loop      
         wait until rising_edge(sys_clk_out);
      end loop;
      
      
      -- Read MAX_WORDS from DDR SDRAM Controller
      num_words := 0;
      cmd       <= RD_CMD;
      cmd_vld   <= '1';
      user_addr <= (others=>'0');
      
      while num_words < MAX_WORDS loop
         wait until rising_edge(sys_clk_out);
         
         if busy_q='0' then 
            user_addr <= user_addr +2;
            num_words := num_words+1;
         end if;
      end loop;
      cmd_vld <= '0';
      
      
      wait for 2000 ns;
      
      report "Simulation finished" severity failure;
   end process;
   
   -- User Adress Mapping
   addr <= user_addr(23 downto 9) & "0000" & user_addr(8 downto 0);
   
   
   -- Data Generator ------------------------------------------
   -- generates 32 bit Data continouosly,
   -- consisting of 16 bits LO- and Hi- words,
   -- using the following scheme: x"00010000", x"00030002", ...
   ------------------------------------------------------------
   process(sys_clk_out)
      variable wr_data : std_logic_vector(U_DATA_WIDTH/2-1 downto 0) := (others=>'0');
   begin
      if rising_edge(sys_clk_out) then
         data_in <= (others=>'0');
         
         if data_req_q = '1' then
            data_in(U_DATA_WIDTH/2-1 downto 0)            <= wr_data;
            data_in(U_DATA_WIDTH-1 downto U_DATA_WIDTH/2) <= wr_data +1;

            wr_data := wr_data +2;
            
         end if;
      
      end if;
   end process;

   -- Data Compare --------------------------------------------
   -- read data is compared to written data
   ------------------------------------------------------------
   process(sys_clk_out)
      variable rd_data : std_logic_vector(U_DATA_WIDTH/2-1 downto 0) := (others=>'0');
   begin
   
      if falling_edge(sys_clk_out) then 
         if data_vld_q = '1' then

            assert rd_data = data_out_q(U_DATA_WIDTH/2-1 downto 0)
               report "Fehler beim Lesen der Daten, Lo-Word"
                  severity error;

            assert rd_data+1 = data_out_q(U_DATA_WIDTH-1 downto U_DATA_WIDTH/2)
               report "Fehler beim Lesen der Daten, Hi-Word"
                  severity error;
            rd_data := rd_data +2;
         end if;
      end if;
   end process;

end architecture struct;
