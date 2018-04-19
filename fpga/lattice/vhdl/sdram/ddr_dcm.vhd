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
-- DATE            $Date: 2002/12/20 15:08:54 $
--
-- LANGUAGE        VHDL 93
--
-- LIBRARY         ieee, unisim
--
-- ABSTRACT        DCM and FDDR components
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


------------------------------------------
-- MY_DCM
------------------------------------------
library ieee;
use ieee.std_logic_1164.all;

entity my_dcm is
   generic ( G_CLOCK_FEEDBACK : string    := "1X";
--             G_CLKDV_DIVIDE   : real      := 2.0;
             G_CLKFX_MULTIPLY : integer   := 5;
             G_CLKFX_DIVIDE   : integer   := 3;
             G_CLKOUT_PHASE_SHIFT: string := "FIXED";
             G_PHASE_SHIFT    : integer   := 0 ;
             G_CLKIN_PERIOD   : string              );
   port ( clkin   : in  std_ulogic;
          clkfb   : in  std_ulogic;
          clk0    : out std_ulogic;
          clkdv   : out std_ulogic;
          clk2x   : out std_ulogic;
          clkfx   : out std_ulogic;
          clk90   : out std_ulogic;
          clk180  : out std_ulogic;
          clk270   : out std_ulogic;
          rst     : in  std_ulogic;
          locked  : out std_ulogic
          );
begin
end;

-- for synthesis only
architecture syn of my_dcm is
   component dcm
      port (clkin   : in  std_ulogic;
            clkfb   : in  std_ulogic;
            clk0    : out std_ulogic;
            clkdv   : out std_ulogic;
            clk2x   : out std_ulogic;
            clkfx   : out std_ulogic;
            clk90   : out std_ulogic;
            clk180  : out std_ulogic;
            clk270   : out std_ulogic;
            rst     : in  std_ulogic;
            locked  : out std_ulogic
            );
   end component;
--   attribute clkdv_divide : string;
   attribute clock_feedback : string;
   attribute clkfx_divide : string;
   attribute clkfx_multiply : string;
   attribute clkin_period : string;
   attribute clkout_phase_shift : string;
   attribute phase_shift : string;
   attribute clock_feedback of dcm : component is G_CLOCK_FEEDBACK;
   attribute clkfx_multiply of dcm : component is integer'image(G_CLKFX_MULTIPLY);
   attribute clkfx_divide of dcm : component is integer'image(G_CLKFX_DIVIDE);
   attribute clkin_period of dcm : component is G_CLKIN_PERIOD;
   attribute clkout_phase_shift of dcm : component is G_CLKOUT_PHASE_SHIFT;
   attribute phase_shift of dcm : component is integer'image(G_PHASE_SHIFT);
--   attribute clkdv_divide of dcm : component is real'image(G_CLKDV_DIVIDE);
begin
   dcm_inst: dcm
      port map ( clkin => clkin,
                 clkfb => clkfb,
                 clk0  => clk0,
                 clkdv => clkdv,
                 clk2x => clk2x,
                 clkfx => clkfx,
                 clk90 => clk90,
                 clk180 => clk180,
                 clk270 => clk270,
                 rst   => rst,
                 locked => locked);
   
end;

-- pragma translate_off
-- for simulation only
library unisim;
architecture sim of my_dcm is
begin
   dcm: entity unisim.dcm generic map ( CLK_FEEDBACK => G_CLOCK_FEEDBACK,
--                                        CLKDV_DIVIDE => G_CLKDV_DIVIDE,
                                        CLKFX_DIVIDE => G_CLKFX_DIVIDE,
                                        CLKFX_MULTIPLY => G_CLKFX_MULTIPLY,
                                        CLKOUT_PHASE_SHIFT => G_CLKOUT_PHASE_SHIFT,
                                        PHASE_SHIFT => G_PHASE_SHIFT)
      port map ( clkin => clkin,
                 clkfb => clkfb,
                 clk0  => clk0,
                 clkdv => clkdv,
                 clk2x => clk2x,
                 clkfx => clkfx,
                 clk90 => clk90,
                 clk180 => clk180,
                 clk270 => clk270,
                 rst   => rst,
                 locked => locked);
end;
-- pragma translate_on

------------------------------------------
-- MY_FDDR
------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
entity my_fddr is
    port( d0 : in std_logic;
          d1 : in std_logic;
          ce : in std_logic;
          c1 : in std_logic;
          r  : in std_logic;
          s  : in std_logic;
          q  : out std_logic);
end;

-- pragma translate_off
library unisim;
-- pragma translate_on
architecture structure of my_fddr is
   component fddrrse
    port( d0 : in std_logic;
          d1 : in std_logic;
          ce : in std_logic;
          c0 : in std_logic;
          c1 : in std_logic;
          r  : in std_logic;
          s  : in std_logic;
          q  : out std_logic);
   end component;

   -- pragma translate_off
   for all: fddrrse use entity unisim.fddrrse;
   -- pragma translate_on

   signal c0 : std_logic;
begin
   c0 <= not c1;
   
   u1: fddrrse
      port map( d0 => d0,
                d1 => d1,
                ce => ce,
                c0 => c0,
                c1 => c1,
                r  => r,
                s  => s,
                q  => q);
end;
