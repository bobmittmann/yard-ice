-- File:	memcpy.vhdl
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

library work;
use work.counter;

entity memcpy is
generic (
	DATA_WIDTH : natural := 16;
	ADDR_BITS : natural := 8
);
port (
	clk : in std_logic := '0';
	rst : in std_logic := '0';
	en : in std_logic := '1';

	-- source memory base address (start copy address)
	src : in std_logic_vector((ADDR_BITS - 1) downto 0) := (others => '0');
	-- destination memory base address
	dst : in std_logic_vector((ADDR_BITS - 1) downto 0) := (others => '0');
	-- destination memory base address
	len : in std_logic_vector((ADDR_BITS - 1) downto 0) := (others => '0');
	-- start strobe input
	start : in std_logic := '0';

	-- source memory copy address
	saddr : out std_logic_vector((ADDR_BITS - 1) downto 0);
	-- source memory data
	sdata : in std_logic_vector((DATA_WIDTH - 1) downto 0) := (others => '0');
	
	-- dest memory copy address
	daddr : out std_logic_vector((ADDR_BITS - 1) downto 0);
	-- dest memory data
	ddata : out std_logic_vector((DATA_WIDTH - 1) downto 0);
	-- dest memory write strobe 
	wr : out std_logic;

	-- end of transfer flag
	done : out std_logic
);
end memcpy;

architecture rtl of memcpy is
	signal s_done : std_logic;
	signal s_act : std_logic;
	signal s_en : std_logic;
	signal s_start : std_logic;
	signal s_act_r : std_logic;
begin

	s_en <= en;
	s_start <= start and s_en and not s_act;

	process (clk, rst)
	begin
		if (rst = '1') then
			s_act <= '0';
		elsif rising_edge(clk) then
			if (s_start = '1') then
				s_act <= '1';
			elsif (s_done = '1') then
				s_act <= '0';
			end if;
		end if;
	end process;

	cnt_r : entity counter
		generic map (DATA_WIDTH => ADDR_BITS, COUNT_BITS => ADDR_BITS) 
		port map (
			clk => clk,
			rst => rst,
			en => s_en,
			ld => s_start,
			cin => s_act,
			d => len,
			up => '0', -- down counter
			cout => s_done -- end of transfer
			);

	src_r : entity counter
		generic map (DATA_WIDTH => ADDR_BITS, COUNT_BITS => ADDR_BITS) 
		port map (
			clk => clk,
			rst => rst,
			en => s_en,
			ld => s_start,
			cin => s_act,
			d => src,
			q => saddr
			);

	process (clk, rst, s_en)
	begin
		if (rst = '1') then
			s_act_r <= '0';
		elsif rising_edge(clk) and (s_en = '1') then
			s_act_r <= s_act;
		end if;
	end process;

	dst_r : entity counter
		generic map (DATA_WIDTH => ADDR_BITS, COUNT_BITS => ADDR_BITS) 
		port map (
			clk => clk,
			rst => rst,
			en => s_en,
			ld => s_start,
			cin => s_act_r,
			d => dst,
			q => daddr
			);

	done <= s_done;
	wr <= s_act;
	ddata <= sdata;

end rtl;

