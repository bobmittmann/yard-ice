-- File:	sram.vhdl
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
-- RAM
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sram is
generic (
	DATA_WIDTH : natural := 16;
	ADDR_WIDTH : natural := 8
);
port (
	clk : in std_logic := '0';
	addr : in std_logic_vector((ADDR_WIDTH - 1) downto 0) := 
								(others => '0');
	data : in std_logic_vector((DATA_WIDTH - 1) downto 0) := 
								(others => '0');
	we : in std_logic := '0';
	q : out std_logic_vector((DATA_WIDTH - 1) downto 0)
);
end sram;

architecture rtl of sram is
	-- Build a 2-D array type for the RAM
	subtype word_t is std_logic_vector((DATA_WIDTH - 1) downto 0);
	type memory_t is array(2**ADDR_WIDTH - 1 downto 0) of word_t;
	-- Declare the RAM signal.	
	signal mem: memory_t;

	signal s_addr : natural range 0 to 2**ADDR_WIDTH - 1;
begin
	s_addr <= to_integer(unsigned(addr));
	process(clk)
	begin
		if (rising_edge(clk)) then 
			if(we = '1') then
				mem(s_addr) <= data;
			end if;
			-- On a read during a write to the same address, the read will
			-- return the OLD data at the address
			q <= mem(s_addr); 
		end if;
	end process;
end rtl;

