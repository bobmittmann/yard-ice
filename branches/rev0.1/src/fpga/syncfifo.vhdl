-- File:	syncfifo.vhdl
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
use ieee.math_real.all;

entity syncfifo is
generic ( 
	DATA_WIDTH : natural := 8;
	ADDR_BITS : natural := 16;
	FIFO_DEPTH : natural := 8
);
port (
	rst : in std_logic := '0';
	en : in std_logic := '1';

	in_clk : in std_logic := '0';
	in_data : in std_logic_vector(DATA_WIDTH -1 downto 0) := (others => '0');
	in_addr : in std_logic_vector(ADDR_BITS -1 downto 0) := (others => '0');
	in_put : in std_logic := '0';

	out_clk : in std_logic := '0';
	out_data : out std_logic_vector(DATA_WIDTH -1 downto 0);
	out_addr : out std_logic_vector(ADDR_BITS - 1 downto 0);
	out_get : out std_logic
);
end syncfifo;

architecture rtl of syncfifo is
	constant CNT_BITS : natural := 2;
	
	signal s_sel_in : unsigned(CNT_BITS - 1 downto 0);
	signal s_sel_out : unsigned(CNT_BITS - 1 downto 0);
	signal s_syn_sel0 : unsigned(CNT_BITS - 1 downto 0);
	signal s_syn_sel1 : unsigned(CNT_BITS - 1 downto 0);

	signal s_fifo_get : std_logic;

	signal s_addr0 : std_logic_vector((ADDR_BITS - 1) downto 0);
	signal s_addr1 : std_logic_vector((ADDR_BITS - 1) downto 0);
	signal s_addr2 : std_logic_vector((ADDR_BITS - 1) downto 0);
	signal s_addr3 : std_logic_vector((ADDR_BITS - 1) downto 0);

	signal s_data0 : std_logic_vector((DATA_WIDTH - 1) downto 0);
	signal s_data1 : std_logic_vector((DATA_WIDTH - 1) downto 0);
	signal s_data2 : std_logic_vector((DATA_WIDTH - 1) downto 0);
	signal s_data3 : std_logic_vector((DATA_WIDTH - 1) downto 0);
begin
	process(in_clk, rst, in_put)
	begin
		if rst = '1' then
			s_sel_in <= (others => '0');
		elsif rising_edge(in_clk) and (in_put = '1') then
			case s_sel_in is
				when "00" => s_addr0 <= in_addr;
				when "01" => s_addr1 <= in_addr;
				when "10" => s_addr2 <= in_addr;
				when "11" => s_addr3 <= in_addr;
			end case;
			case s_sel_in is
				when "00" => s_data0 <= in_data;
				when "01" => s_data1 <= in_data;
				when "10" => s_data2 <= in_data;
				when "11" => s_data3 <= in_data;
			end case;
			s_sel_in <= s_sel_in + 1;
		end if;
	end process;

	process(out_clk, rst)
	begin
		if rst = '1' then
			s_syn_sel0 <= (others => '0');
			s_syn_sel1 <= (others => '0');
		elsif rising_edge(out_clk) then
			s_syn_sel0 <= s_sel_in; 
			s_syn_sel1 <= s_syn_sel0; 
		end if;
	end process;

	s_fifo_get <= '1' when (s_syn_sel1 /= s_sel_out) and (en = '1') else '0';

	process(out_clk, rst, s_fifo_get)
	begin
		if rst = '1' then
			s_sel_out <= (others => '0');
		elsif rising_edge(out_clk) and (s_fifo_get = '1') then
			s_sel_out <= s_sel_out + 1;
		end if;
	end process;

	with s_sel_out select
		out_data <= 
		s_data0 when "00", 
		s_data1 when "01", 
		s_data2 when "10", 
		s_data3 when "11";

	with s_sel_out select
		out_addr <= s_addr0 when "00", 
		s_addr1 when "01", 
		s_addr2 when "10", 
		s_addr3 when "11";

	out_get <= s_fifo_get;

end rtl;

