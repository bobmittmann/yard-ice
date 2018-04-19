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
-- DATE            $Date: 2003/03/04 08:29:07 $
--
-- LANGUAGE        VHDL 93
--
-- LIBRARY         ieee, work
--
-- ABSTRACT        DDR SDRAM Core User Interface
--                 This module is responsible for the valid/busy handshake
--                 of commands and to check for some conditions,
--                 e.g. whether a precharge or any delay is necessary.
--                 Auto Refresh is also initiated by this module
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
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;
use work.ddr_sdr_conf_pkg.all;

entity user_if is
   port (
      rst_int_n             : in std_logic; -- async reset, lo-active
      sys_clk               : in std_logic;
      
      -- user interface
      cmd                   : in std_logic_vector(U_CMD_WIDTH -1 downto 0); -- command: read, write, nop
      cmd_vld               : in std_logic;
      addr                  : in std_logic_vector(U_ADDR_WIDTH-1 downto 0); -- ROW, BANK, COLUMN
      busy_q                : out std_logic; -- busy flag, when active commands are ignored
      
      -- Datenaustausch mit Controller
      init_finished         : in boolean;
      new_cmd_q             : out boolean;
      new_cmd_ack           : in boolean;
      do_prech_q            : out boolean; -- precharge followed by activate
      do_wait_q             : out boolean; -- additional delay requested
      
      cmd_q                 : buffer std_logic_vector(U_CMD_WIDTH -1 downto 0);
      addr_q                : out std_logic_vector(U_ADDR_WIDTH -1 downto 0)
   );
end;

architecture behave of user_if is

   type IN_STATE_TYPE is (IDLE, READY, BUSY, REFRESH);
   signal state_q         : IN_STATE_TYPE;
   
   signal addr_int_q      : std_logic_vector(U_ADDR_WIDTH -1 downto 0);
   signal cmd_int_q       : std_logic_vector(U_CMD_WIDTH -1 downto 0);
   signal busy_int_q      : boolean;
   
   signal row_q : std_logic_vector(DDR_ADDR_WIDTH + DDR_BANK_WIDTH downto 0); 
   
   -- Refresh flags
   signal do_rfsh_q       : boolean;
   signal rfsh_now_q      : boolean;
   signal rfsh_req_q      : boolean;
   signal rfsh_ena_q      : boolean;
   signal internal_rfsh_q : boolean;

begin

   process (sys_clk, rst_int_n) is
      variable new_row : std_logic_vector(DDR_ADDR_WIDTH + DDR_BANK_WIDTH downto 0);
   begin 
      if rst_int_n = '0' then  

         addr_int_q      <= (others=>'0');
         cmd_int_q       <= (others=>'0');
         state_q         <= IDLE;
         new_cmd_q       <= false;
         do_prech_q      <= false;
         busy_int_q      <= true;
         do_wait_q       <= false;
         row_q           <= (others =>'1');
         do_rfsh_q       <= false;
         rfsh_ena_q      <= false;
         internal_rfsh_q <= false;
         rfsh_now_q      <= false;
        
      elsif rising_edge(sys_clk) then

         -- extract ROW from user address
         new_row := '0' & addr(addr_int_q'LEFT downto DDR_ADDR_WIDTH);
         
         internal_rfsh_q <= false;

         case state_q is
            when IDLE =>
               busy_int_q   <= true; -- as long as controller is in initialization state
               new_cmd_q    <= false;
               rfsh_ena_q   <= false;
               do_rfsh_q    <= false;
               rfsh_now_q   <= false;
               
               if init_finished then -- SDRAM initialization finished
                  -- now allow external commands
                  state_q     <= READY;
                  busy_int_q  <= false;
                  rfsh_ena_q  <= true;
               end if;
            
            when READY =>
               rfsh_now_q <= rfsh_req_q;
               -- Refresh has highest priority
               if cmd_vld='1' then -- new command requested
                  busy_int_q    <= true;
                  new_cmd_q     <= true;
                  state_q       <= BUSY;
               elsif rfsh_now_q then
                  busy_int_q    <= true;
                  new_cmd_q     <= true;
                  do_rfsh_q     <= true; -- Switch CMD-MUX to Refresh Command
                  state_q       <= REFRESH;
               end if;
            
            when BUSY =>
               if new_cmd_ack then
                  if rfsh_now_q then
                     new_cmd_q     <= true;
                     do_rfsh_q     <= true; -- Switch CMD-MUX to Refresh Command
                     state_q       <= REFRESH;
                  else
                     new_cmd_q  <= false;
                     state_q    <= READY;
                     busy_int_q <= false;
                  end if;
                  
               end if;
               
            when REFRESH =>
               rfsh_now_q      <= false;
               internal_rfsh_q <= true;
               if new_cmd_ack then
                  internal_rfsh_q <= false;
                  new_cmd_q <= false;
                  state_q       <= READY;
                  busy_int_q    <= false;
                  do_rfsh_q     <= false; -- Refresh command
               end if;
         end case;
         
         -- Store command and address by control of handshake signals
         if cmd_vld='1' and not busy_int_q then
            cmd_int_q    <= cmd;
            addr_int_q   <= addr;
            do_wait_q    <= false; -- default

            if cmd = WR_CMD or cmd = RD_CMD then
            
               if row_q /= new_row then -- new ROW requested, do precharge followed by activate
                  do_prech_q  <= true;    -- Precharge, Activate
                  do_wait_q   <= true;    -- tWR !
                  row_q       <= new_row; -- store this row and clear the 'first access' bit
               else
                  do_prech_q  <= false;
                  do_wait_q   <= false; -- tWR !
               end if;
               
               -- changing the command should cause a delay
               -- the same is true for changing the ROW (see above)
               if (cmd_int_q /= cmd) then
                  do_wait_q <= true;
               end if;
               
            end if;
            
         end if;
      end if;
   end process;
   
   -----------------------------------------------------------------------------------------------------------------------------------------------------
   -- REFRESH COUNTER : every 64 ms a complete refresh cycle is needed
   -- with every refresh the ref_cnt_q is loaded
   ------------------------------------------------------------------------------------------------------------------------------------------------------
   ref_cnt: block
      signal ref_cnt_q : integer range 0 to 9 * (REFRESH_CLOCKS -1); -- for Multiple Refresh
   begin 
      process (sys_clk, rst_int_n) is
      begin 
         if rst_int_n = '0' then 
         
            ref_cnt_q  <= REFRESH_CLOCKS -1;
            rfsh_req_q <= false;
            
         elsif rising_edge(sys_clk) then  
         
            if rfsh_ena_q then -- erst nach DDR_SDRAM_INIT AUTO-Refresh zulassen
            
               if do_rfsh_q then -- Internal Auto Refresh
                  ref_cnt_q <= REFRESH_CLOCKS -1;
               else
                  ref_cnt_q <= ref_cnt_q -1;
               end if;
               
            end if;

            assert ref_cnt_q >= 0 report "ref_cnt_q error!!!" severity failure;
            
            -- a refresh window is used rather than comparing the counter to zero
            -- to guarantee the maximum refresh period in all cases
            rfsh_req_q  <= (ref_cnt_q < REFRESH_WINDOW);

         end if;
      end process;
      
   end block ref_cnt;       

   -- Mapping internl signals -> ouputs
   addr_q    <= addr_int_q;
   cmd_q     <= cmd_int_q when not do_rfsh_q else RFSH_CMD;
   busy_q    <= '1' when busy_int_q else '0';
end;
         
