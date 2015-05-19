-- File:	syncbus.vhdl
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

entity syncbus is
generic ( 
	DATA_WIDTH : natural := 16;
	ADDR_BITS : natural := 16
);
port (
	io_clk : in std_logic := '0';
	io_wr : in std_logic;
	io_din : in std_logic_vector(DATA_WIDTH -1 downto 0) := (others => '0');
	io_addr : in std_logic_vector(ADDR_BITS -1 downto 0) := (others => '0');

	bus_rst : in std_logic := '0';
	bus_clk : in std_logic := '0';
	bus_din : out std_logic_vector(DATA_WIDTH -1 downto 0);
	bus_addr : out std_logic_vector(ADDR_BITS - 1 downto 0);
	bus_wr : out std_logic
);
end syncbus;

architecture rtl of syncbus is
	signal s_wr_syn: std_logic_vector(0 to 2);
begin 
	-- latch the I/O data and address
	process (io_clk, io_wr) 
	begin
		if rising_edge(io_clk) and (io_wr = '1') then
			bus_addr <= io_addr;
			bus_din <= io_din;
		end if;
	end process;

	-- synchronize on the rising edge ot WR signal
	process (io_clk, io_wr) 
	begin
		if s_wr_syn(2) = '1' then
			s_wr_syn(0) <= '0';
		elsif rising_edge(io_clk) and (io_wr = '1') then
			s_wr_syn(0) <= '1';
		end if;
	end process;

	process (bus_clk, bus_rst) 
	begin
		if bus_rst = '1' then
			s_wr_syn(1) <= '0';
			s_wr_syn(2) <= '0';
		elsif rising_edge(bus_clk) then
			s_wr_syn(1) <= s_wr_syn(0);
			s_wr_syn(2) <= s_wr_syn(1);
		end if;
	end process;

	-- generate a write strobe pulse
	bus_wr <= s_wr_syn(1) and not s_wr_syn(2);

end rtl;

