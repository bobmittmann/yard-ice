-- File:	jtag_io_ctr.vhdl
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
-- JTAG IO Control
-- Provides the timing signals to control the TX and RX shift registers
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity jtag_enh_shift_ctrl is
generic (
	VEC_LEN_BITS : natural := 10;
	DESC_ADDR_BITS : natural := 7
);
port (
	-- system clock
	clk : in std_logic;
	-- assynchronous reset
	rst : in std_logic := '0';
	-- enable
	en : in std_logic := '0';
	-- TCK positive edge strobe
	tckp_stb : in std_logic := '0';
	-- TCK negative edge strobe
	tckn_stb : in std_logic := '0';
	-- length of current vecotr
	vec_len : in std_logic_vector(VEC_LEN_BITS - 1 downto 0);
	-- base address for the vector descriptor table
	desc_base : in std_logic_vector(DESC_ADDR_BITS - 1 downto 0);		
	-- number of entries in the vector descriptor table
	desc_len: in std_logic_vector(DESC_ADDR_BITS - 1 downto 0);
	-- current vector descriptor
	desc_addr : out std_logic_vector(DESC_ADDR_BITS - 1 downto 0);
	-- descriptor read strobe 
	desc_rd_stb : out std_logic;
	-- shifters control signals
	rx_shift_stb: out std_logic;
	tx_shift_stb: out std_logic;
	-- end of operation signals
	rx_done_stb : out std_logic;
	tx_done_stb : out std_logic;
	-- start of operation acknowledgment
	start_ack : out std_logic;

	-- DEBUG
	-- vector bit count
	bit_cnt : out std_logic_vector(VEC_LEN_BITS - 1 downto 0)
);
end jtag_enh_shift_ctrl;

architecture rtl of jtag_enh_shift_ctrl is
	signal s_clr_stb : std_logic;
	signal s_ld_stb : std_logic;

	signal s_cnt_r : unsigned(VEC_LEN_BITS - 1 downto 0);
	signal s_cnt_stb : std_logic;
	signal s_cnt_z : std_logic;

	signal s_offs_r : unsigned(DESC_ADDR_BITS - 1 downto 0);
	signal s_offs_inc : std_logic;
	signal s_offs_z : std_logic;

	signal s_en_r0 : std_logic;
	signal s_en_r1 : std_logic;
	signal s_en_r2 : std_logic;

	signal s_rx_en_r0 : std_logic;
	signal s_rx_en_r1 : std_logic;
	signal s_rx_en_r2 : std_logic;
	
	signal s_start_stb : std_logic;
	signal s_stop : std_logic;
begin	
	-----------------------------------------------------------------------
	p_enable : process (rst, clk, en, s_en_r0, s_en_r1, tckn_stb, s_stop)
	begin
		if (rst = '1') then
			s_en_r0 <= '0';
			s_en_r1 <= '0';
			s_en_r2 <= '0';
		elsif rising_edge(clk) then
			if (s_en_r0 = '0') then
				s_en_r0 <= en;
			elsif (s_stop = '1') then
				s_en_r0 <= '0';
			end if;
			s_en_r1 <= s_en_r0;
			if (tckn_stb = '1') then
				s_en_r2 <= s_en_r1;
			end if;
		end if;
	end process p_enable;

	s_start_stb <= en and not s_en_r0;
	-----------------------------------------------------------------------


	s_clr_stb <= s_start_stb;

	s_ld_stb <=  (s_cnt_z and tckn_stb) and s_en_r1;

	s_cnt_stb <= tckn_stb and s_en_r1;

	-----------------------------------------------------------------------
	p_count : process (rst, clk, s_clr_stb, s_ld_stb, s_cnt_stb, s_cnt_r)
		variable cy : std_logic;
	begin
		cy := '1';
		for i in 0 to VEC_LEN_BITS - 1 loop
			if (rst = '1') then
				s_cnt_r(i) <= '0';
			elsif rising_edge(clk) then
				if (s_clr_stb = '1') then
					s_cnt_r(i) <= '0';
				elsif (s_ld_stb = '1') then
					s_cnt_r(i) <= vec_len(i);
				elsif (s_cnt_stb = '1') then
					s_cnt_r(i) <= s_cnt_r(i) xor cy;
				end if;
			end if;
			cy := cy and not s_cnt_r(i);
		end loop;
		s_cnt_z <= cy;
	end process p_count;
	-----------------------------------------------------------------------

	s_offs_inc <= ((s_cnt_z and tckn_stb) or s_offs_z) and s_en_r2;

	-----------------------------------------------------------------------
	p_offs: process (rst, clk, s_clr_stb, s_offs_inc, s_offs_r)
		variable cy : std_logic;
		variable z : std_logic;
	begin
		cy := s_offs_inc;
		z := '1';
		for i in 0 to DESC_ADDR_BITS - 1 loop
			if (rst = '1') then
				s_offs_r(i) <= '1';
			elsif rising_edge(clk) then
				if (s_clr_stb = '1') then
					s_offs_r(i) <= '0';
				else
					s_offs_r(i) <= s_offs_r(i) xor cy;
				end if;
			end if;
			cy := cy and s_offs_r(i);
			z := z and not s_offs_r(i);
		end loop;
		s_offs_z <= z;
	end process p_offs;
	-----------------------------------------------------------------------
	
	s_stop <= '1' when (s_offs_r = unsigned(desc_len) + 1) and 
		(s_cnt_z = '1') else '0';

	-----------------------------------------------------------------------
	p_rx_en: process (rst, clk, en, tckn_stb)
	begin
		if (rst = '1') then
			s_rx_en_r0 <= '0';
			s_rx_en_r1 <= '0';
			s_rx_en_r2 <= '0';
		elsif rising_edge(clk) then
			if (tckn_stb = '1') then
				s_rx_en_r0 <= s_en_r1;
				s_rx_en_r1 <= s_rx_en_r0;
			end if;
			s_rx_en_r2 <= s_rx_en_r1;			
		end if;
	end process p_rx_en;
	-----------------------------------------------------------------------	

	tx_shift_stb <= (s_en_r0 or s_en_r1) and tckn_stb;	
	tx_done_stb <= s_en_r1 and not s_en_r0;

--	tx_done_stb <= s_en_r2 and not s_en_r1 and tckn_stb;

	rx_shift_stb <= s_rx_en_r1 and tckn_stb;
	rx_done_stb <= s_rx_en_r2 and not s_rx_en_r1;

	desc_addr <= std_logic_vector(unsigned(desc_base) + s_offs_r);
	desc_rd_stb <= s_ld_stb;

	start_ack <= s_start_stb;

	bit_cnt <= std_logic_vector(s_cnt_r);
end rtl;

