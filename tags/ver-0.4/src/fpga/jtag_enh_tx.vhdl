-- File:	jtag_enh_tx.vhdl
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
-- JTAG TX shifter
--

library ieee;
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all;
use ieee.math_real.all;

entity jtag_enh_tx is
generic ( 
	DATA_BITS : natural := 8;
	VEC_ADDR_BITS : natural := 10;
	VEC_LEN_BITS : natural := 10;
	VEC_OFFS_BITS : natural := 3
);
port ( 
	-- system clock
	clk : in std_logic;
	-- assynchronous reset
	rst : in std_logic := '0';
	-- vector (length - 1)
	-- the vector length is decremented by one to simplify the
	-- remaining operations and control structures
	vec_len : in std_logic_vector(VEC_LEN_BITS - 1 downto 0) := (others => '0');
	-- vector control
	-- vector address to read the output vector from
	vec_addr : in std_logic_vector(VEC_ADDR_BITS - 1 downto 0) := (others => '0');
	-- offset in bits ...
	vec_offs : in std_logic_vector(VEC_OFFS_BITS - 1 downto 0) := (others => '0');
	-- vector shift direction ('0': LSB first, '1': MSB first)
	vec_msbf : in std_logic := '0';
	-- load/update the vector control data
	load_stb : in std_logic := '0'; 
	-- shift strobe
	shift_stb : in std_logic := '0';

	-- memory address 
	mem_addr : out std_logic_vector(VEC_ADDR_BITS - 1 downto 0);
	-- memory data in
	mem_din : in std_logic_vector(DATA_BITS - 1 downto 0) := (others => '0');	
	-- memory read ack
	mem_rd : out std_logic;
	-- serial output 
	so : out std_logic
);	
end jtag_enh_tx;

architecture rtl of jtag_enh_tx is 
	-- number of bits needed to store the data width
	constant DW_BITS : integer := integer(ceil(log2(real(DATA_BITS))));

	-- initial bit position
	signal s_pos_0 : unsigned(DW_BITS - 1 downto 0);
	-- initial word position in the vector memory
	signal s_addr_0 : unsigned(VEC_ADDR_BITS - 1 downto 0);
	-- current bit position
	signal s_pos : unsigned(DW_BITS - 1 downto 0);
	-- current word position (address) in the vector memory
	signal s_addr : unsigned(VEC_ADDR_BITS - 1 downto 0);
	signal s_addr_r : unsigned(VEC_ADDR_BITS - 1 downto 0);
	signal s_sum : unsigned(VEC_ADDR_BITS - 1 downto 0);
	-- bit position wrap (underflow/overflow) registered
	signal s_pos_wrap : std_logic;
	-- most significant bit first registered
	signal s_msbf_r : std_logic;  
	signal s_msbf : std_logic;  

	-- transmit buffer
	signal s_txbuf : std_logic_vector(DATA_BITS - 1 downto 0);
	signal s_mem_rd_r0 : std_logic; 
	
begin 
	-- compute the initial bit position
	s_pos_0 <= unsigned(vec_offs) when (vec_msbf = '0') else 
		unsigned(vec_len(DW_BITS - 1 downto 0)) + unsigned(vec_offs);
	-- compute the initial word position
	s_addr_0 <= unsigned(vec_addr) when (vec_msbf = '0') else
		unsigned(vec_addr) + unsigned(vec_offs) +
		unsigned(vec_len(VEC_LEN_BITS - 1 downto DW_BITS));

	-----------------------------------------------------------------------
	-- update the msb flag register
	p_msbf: process (rst, clk, load_stb, vec_msbf)
	begin
		if (rst = '1') then
			s_msbf_r <= '0';
		elsif rising_edge(clk) and (load_stb = '1') then
			s_msbf_r <= vec_msbf;
		end if;
	end process p_msbf;
	-----------------------------------------------------------------------

	s_msbf <= vec_msbf when load_stb = '1' else s_msbf_r;

	-----------------------------------------------------------------------
	-- update the bit position
	-- increment the position if msbf == '1', decrement otherwise
	-- load new bit position if load_stb == '1'
	p_pos: process (rst, clk, load_stb, shift_stb, s_msbf_r, s_pos)
		variable cy : std_logic;
	begin
		cy := shift_stb;
		for i in 0 to DW_BITS - 1 loop
			if (rst = '1') then
				-- assynchronous reset
				s_pos(i) <= '0';
			elsif rising_edge(clk) then
				if (load_stb = '1') then
					-- load the new position
					s_pos(i) <= s_pos_0(i);
				else 
					s_pos(i) <= s_pos(i) xor cy;
				end if;
			end if;
			cy := (s_msbf_r xor s_pos(i)) and cy;
		end loop;
		s_pos_wrap <= cy;
	end process p_pos;
	-----------------------------------------------------------------------

	-----------------------------------------------------------------------
	p_add_sub: process (s_msbf, s_pos_wrap, s_addr_r)
		variable cy : std_logic;
	begin
		cy := s_pos_wrap;
		for i in 0 to VEC_ADDR_BITS - 1 loop
			s_sum(i) <= s_addr_r(i) xor cy;
			cy := (s_msbf xor s_addr_r(i)) and cy;
		end loop;
	end process p_add_sub;
	
	s_addr <= s_addr_0 when (load_stb = '1') else s_sum;
	-----------------------------------------------------------------------

	-----------------------------------------------------------------------
	-- update the address
	p_addr: process (rst, clk, load_stb, s_pos_wrap)
	begin
		if (rst = '1') then
			s_addr_r <= (others => '0');		
		elsif rising_edge(clk) then
			s_addr_r <= s_addr;
		end if;
	end process p_addr;
	-----------------------------------------------------------------------

	-----------------------------------------------------------------------
	-- read from memory
	p_mem_rd_ppln : process (rst, clk, s_pos_wrap, load_stb)
	begin
		if (rst = '1') then
			s_mem_rd_r0 <= '0';
		elsif rising_edge(clk) then
			s_mem_rd_r0 <= s_pos_wrap;
		end if;
	end process p_mem_rd_ppln;

	-- loads a word from memory into the transmit buffer
	s_txbuf <= mem_din;

	-----------------------------------------------------------------------
	mem_addr <= std_logic_vector(s_addr);
	mem_rd <= s_mem_rd_r0 or load_stb;

	-- select the bit to be transmitted from s_txbuf
	so <= s_txbuf(to_integer(s_pos));

end rtl;
