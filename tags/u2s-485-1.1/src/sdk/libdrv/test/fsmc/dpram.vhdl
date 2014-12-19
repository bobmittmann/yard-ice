-- File:	dpram.vhdl
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

entity dpram is
generic (
	DATA_WIDTH : natural := 16;
	ADDR_WIDTH : natural := 8
);
port (
	clk1 : in std_logic := '0';
	addr1 : in std_logic_vector((ADDR_WIDTH - 1) downto 0) := (others => '0');
	data1 : in std_logic_vector((DATA_WIDTH - 1) downto 0) := (others => '0');
	we1 : in std_logic := '0';
	q1 : out std_logic_vector((DATA_WIDTH - 1) downto 0);

	clk2 : in std_logic := '0';
	addr2 : in std_logic_vector((ADDR_WIDTH - 1) downto 0) := (others => '0');
	data2 : in std_logic_vector((DATA_WIDTH - 1) downto 0) := (others => '0');
	we2 : in std_logic := '0';
	q2 : out std_logic_vector((DATA_WIDTH - 1) downto 0)
);
end dpram;

architecture rtl of dpram is
	-- Build a 2-D array type for the RAM
	subtype word_t is std_logic_vector((DATA_WIDTH - 1) downto 0);
	type memory_t is array(2**ADDR_WIDTH - 1 downto 0) of word_t;
	-- Declare the RAM signal.	
	signal mem: memory_t;

	signal s_addr1 : natural range 0 to 2**ADDR_WIDTH - 1;
	signal s_addr2 : natural range 0 to 2**ADDR_WIDTH - 1;
begin
	s_addr1 <= to_integer(unsigned(addr1));
	process(clk1)
	begin
		if (rising_edge(clk1)) then 
			if(we1 = '1') then
				mem(s_addr1) <= data1;
			end if;
			-- On a read during a write to the same address, the read will
			-- return the OLD data at the address
			q1 <= mem(s_addr1); 
		end if;
	end process;

	s_addr2 <= to_integer(unsigned(addr2));
	process(clk2)
	begin
		if (rising_edge(clk2)) then 
			if(we2 = '1') then
				mem(s_addr2) <= data2;
			end if;
			-- On a read during a write to the same address, the read will
			-- return the OLD data at the address
			q2 <= mem(s_addr2); 
		end if;
	end process;
end rtl;

