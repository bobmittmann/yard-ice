-- File:	led_drv.vhdl
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
-- LED driver
--

library ieee;
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all;
use ieee.math_real.all;

entity led_drv is
generic ( 
	PULSE_CNT : integer := 6;
	IN_LVL : boolean := false;
	IN_INV : boolean := false;
	OUT_INV : boolean := false
);
port (
	-- system clock
	clk : in std_logic;
	-- reset
	rst : in std_logic := '0';
	-- clock enable
	en : in std_logic := '1';
	-- delay trigger
	trip : in std_logic;
	-- output
	q : out std_logic
);
end led_drv;

architecture rtl of led_drv is
	constant CNT_BITS : natural := integer(ceil(log2(real(PULSE_CNT))));
		 
	signal s_count : unsigned(CNT_BITS - 1 downto 0);
	signal s_top : unsigned(CNT_BITS - 1 downto 0);

	signal s_drv_ff : std_logic;

	signal s_trip_r0 : std_logic;
	signal s_trip_r1 : std_logic;
	signal s_force_on : std_logic;
	signal s_set_stb : std_logic;
	signal s_clr_stb : std_logic;

	signal s_cnt_z : std_logic;
	signal s_cnt_ld : std_logic;
	signal s_cnt_en : std_logic;
begin 
    s_top <= to_unsigned(PULSE_CNT, CNT_BITS);

	p_level: process (clk, rst, trip)
	begin
		if (IN_LVL) then
			if (rst = '1') then
				s_force_on <= '0';
			elsif rising_edge(clk) then
				if (IN_INV) then
					s_force_on <= not trip;
				else
					s_force_on <= trip;
				end if;
			end if;
		else
			s_force_on <= '0';
		end if;
	end process p_level;

	p_trip_sync: process (clk, rst, trip)
	begin
		if (rst = '1') then
			s_trip_r0 <= '0';
			s_trip_r1 <= '0';
		elsif rising_edge(clk) then
			if (IN_INV) then
				s_trip_r0 <= not trip;
			else
				s_trip_r0 <= trip;
			end if;
			s_trip_r1 <= s_trip_r0; 
		end if;
	end process p_trip_sync;

	s_set_stb <= s_trip_r0 and not s_trip_r1 when not IN_LVL else s_force_on;
	s_clr_stb <= s_cnt_z;

	p_drv: process (clk, rst, en, s_cnt_ld, s_cnt_en)
	begin
		if (rst = '1') then
			s_drv_ff <= '0';
		elsif rising_edge(clk) then
			if (s_set_stb = '1') then
				s_drv_ff <= '1';
			elsif (s_clr_stb = '1') then
				s_drv_ff <= '0';
			end if;
		end if;
	end process p_drv;

	s_cnt_ld <= s_cnt_z or s_set_stb;
	s_cnt_en <= s_drv_ff and not s_force_on;

	p_count: process (clk, rst, en, s_cnt_ld, s_cnt_en, s_top)
	begin
		if (rst = '1') then
			s_count <= s_top;
		elsif rising_edge(clk) then
			if (s_cnt_ld = '1') then
				s_count <= s_top;			
			elsif (en = '1') and (s_cnt_en = '1') then
				s_count <= s_count - 1;
			end if;
		end if;
	end process p_count;

	s_cnt_z <= '1' when to_integer(s_count) = 0 else '0';

	q <= not s_drv_ff when (OUT_INV) else s_drv_ff;

end rtl;

