-- File:	clk_scaler.vhdl
-- Author:	Robinson Mittmann (bobmittmann@gmail.com)
-- Target:
-- Comment:
-- Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
-- 
-- This program is free software; you can redistribute it and/or
-- modify it under the terms of the GNU General Public License
-- as published by the Free Software Foundation; either version 2
-- of the License, or (at your option) any later version.
-- 
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
-- 
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
-- 

-- 
-- Clock scaler
--

library ieee;
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all;
use ieee.math_real.all;

entity clk_scaler is
generic ( 
	CLK_DIV : natural := 8 
);
port (
	-- input clock
	clk : in std_logic;
	-- enable
	en : in std_logic := '1';
	-- clear
	rst : in std_logic := '0';
	-- output clock
	q : out std_logic;
	-- positive strobe (low to high transition)
	p_stb : out std_logic;
	-- negative strobe (high to low transition)
	n_stb : out std_logic
);
end clk_scaler;

architecture rtl of clk_scaler is
	constant CNT_BITS : natural := integer(ceil(log2(real(CLK_DIV))));
		 
	signal s_count : unsigned(CNT_BITS - 1 downto 0);
	signal s_comp : unsigned(CNT_BITS - 1 downto 0);
	
	signal s_dff : std_logic;
	signal s_lo : std_logic;
	signal s_hi : std_logic;
begin 
    s_comp <= to_unsigned(CLK_DIV - 1, CNT_BITS);
    s_hi <= '1' when (s_count = (s_comp / 2)) else '0';
    s_lo <= '1' when (s_count = s_comp) else '0';

	p_count: process (clk, rst, s_hi, s_lo)
	begin
		if (rst = '1') then
			s_count <= (others => '0');
			s_dff <= '0';
		elsif rising_edge(clk) then
			if (en = '1') then
				if (s_lo = '1') then
					s_dff <= '0';						
					s_count <= (others => '0');
				else
					if (s_hi = '1') then
						s_dff <= '1';
					end if;
					s_count <= s_count + 1;
				end if;		
			end if;
		end if;
	end process p_count;

	p_strobe: process (clk, rst, s_hi, s_lo)
	begin
		if (rst = '1') then
			p_stb <= '0';
			n_stb <= '0';
		elsif rising_edge(clk) then
    		p_stb <= s_hi and en;
		    n_stb <= s_lo and en;
		end if;
	end process p_strobe;

	q <= s_dff;
end rtl;

