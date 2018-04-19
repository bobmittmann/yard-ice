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
-- DATE            $Date: 2002/12/20 15:09:33 $
--
-- LANGUAGE        VHDL 93
--
-- LIBRARY         ieee, unisim
--
-- ABSTRACT        Shift Register Generation for sync RESET
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



library ieee;
use ieee.std_logic_1164.all;
entity reset is
   port (
      clk : in std_logic;
      rst_in : in std_logic := '0';
      srst_q : out std_logic
      );
end;

-- pragma translate_off
library unisim;
-- pragma translate_on
architecture reset of reset is
   component fdp
      port ( d : in std_logic;
             c : in std_logic;
             pre : in std_logic;
             q : out std_logic );
   end component;
   -- pragma translate_off
   for all: fdp use entity unisim.fdp;
   -- pragma translate_on

   
   signal shift_q : std_logic_vector(3 downto 0);
   attribute preserve_signal : boolean;
   attribute preserve_signal of shift_q : signal is true;

begin
   
   fdp0: fdp port map ( d => rst_in,
                        c => clk,
                        pre => '0',
                        q => shift_q(3) );
   fdp1: fdp port map ( d => shift_q(3),
                        c => clk,
                        pre => '0',
                        q => shift_q(2) );
   fdp2: fdp port map ( d => shift_q(2),
                        c => clk,
                        pre => '0',
                        q => shift_q(1) );
   fdp3: fdp port map ( d => shift_q(1),
                        c => clk,
                        pre => '0',
                        q => shift_q(0) );
   
   srst_q <= shift_q(0);
end;
