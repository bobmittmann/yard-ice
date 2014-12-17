-- File:	reg.vhdl
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
-- JTAG Register
--

library ieee;
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all;

entity reg is
generic ( 
	DATA_WIDTH : integer := 32;
	REG_BITS : integer := 8
);
port (
	-- system clock
	clk : in std_logic;
	-- clock enable
	en : in std_logic := '1';
	-- reset
	rst : in std_logic := '0';
	-- load signal 
	ld : in std_logic := '0';
	-- data in
	d : in std_logic_vector(DATA_WIDTH - 1 downto 0) := (others => '0');
	-- clear selected bits
	clr : in std_logic := '0';
	-- bits to turn off
	d_clr : in std_logic_vector(REG_BITS - 1 downto 0) := (others => '1');
	-- set selected bits
	set : in std_logic := '0';
	-- bits to turn on
	d_set : in std_logic_vector(REG_BITS - 1 downto 0) := (others => '1');
	-- data out
	q : out std_logic_vector(DATA_WIDTH - 1 downto 0)
);
end reg;

architecture rtl of reg is
	signal s_reg : std_logic_vector(REG_BITS - 1 downto 0);

	function vector_fill(x : std_logic_vector(REG_BITS - 1 downto 0)) 
		return std_logic_vector is
		variable y: std_logic_vector(DATA_WIDTH - 1 downto 0);
	begin
		y(REG_BITS - 1 downto 0) := x;

		if (DATA_WIDTH > REG_BITS) then
			for i in REG_BITS to (DATA_WIDTH - 1) loop
				y(i) := '0';
			end loop; 
		end if;
		return y;
	end vector_fill;

begin 
	process (clk, rst, en, ld, d, clr, d_clr, set, d_set)
	begin
		if (rst = '1') then
			s_reg <= (others => '0');
		elsif (rising_edge(clk)) then
			if (en = '1') then
				if (ld = '1') then
					-- load all bits
					s_reg <= d(REG_BITS - 1 downto 0);
				else
					-- set/clear individual bits 
					-- priority is setting
					for i in 0 to (REG_BITS - 1) loop
						if (set = '1') and (d_set(i) = '1') then
							s_reg(i) <= '1';
						elsif (clr = '1') and (d_clr(i) = '1') then
							s_reg(i) <= '0';
						end if;
					end loop;
				end if;
			end if;
		end if;
	end process;

	-- output
	q <= vector_fill(s_reg);
end rtl;

