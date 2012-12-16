-- File:	counter.vhdl
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
-- Generic counter
--

library ieee;
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all;

entity counter is
generic (
	DATA_WIDTH : integer := 32;
	COUNT_BITS : integer := 8
);
port (
	-- data in
	d : in std_logic_vector(DATA_WIDTH - 1 downto 0) := (others => '0');
	-- system clock
	clk : in std_logic;
	-- assynchronous reset 
	rst : in std_logic := '0';
	-- enable
	en : in std_logic := '1';
	-- carry in
	cin : in std_logic := '1';
	-- load
	ld : in std_logic := '0';
	-- synchronous clear
	clr : in std_logic := '0';
	-- up/down count
	up : in std_logic := '1';
	-- data out 
	q : out std_logic_vector(DATA_WIDTH - 1 downto 0);
	-- carry out
	cout : out std_logic
);
end counter;

architecture rtl of counter is 
	signal s_count_reg : std_logic_vector(COUNT_BITS - 1 downto 0);

	function vector_fill(x : std_logic_vector(COUNT_BITS - 1 downto 0)) 
		return std_logic_vector is
		variable y: std_logic_vector(DATA_WIDTH - 1 downto 0);
	begin
		y(COUNT_BITS - 1 downto 0) := x;

		if (DATA_WIDTH > COUNT_BITS) then
			for i in COUNT_BITS to (DATA_WIDTH - 1) loop
				y(i) := '0';
			end loop; 
		end if;
		return y;
	end vector_fill;
begin
	p_count: process (clk, rst, en, clr, ld, cin, s_count_reg, up)
		variable cy : std_logic;
	begin
		cy := cin;
		for i in 0 to COUNT_BITS - 1 loop
			if (rst = '1') then
				s_count_reg(i) <= '0';
			elsif rising_edge(clk) then
				if (en = '1') then
					if (clr = '1') then
						s_count_reg(i) <= '0';
					elsif (ld = '1') then
						s_count_reg(i) <= d(i);
					else
						s_count_reg(i) <= s_count_reg(i) xor cy;
					end if;
				end if;
			end if;
			cy := (s_count_reg(i) xor not up) and cy;
		end loop;
		cout <= cy;
	end process p_count;

	-- output
	q <= vector_fill(s_count_reg);
end rtl;

