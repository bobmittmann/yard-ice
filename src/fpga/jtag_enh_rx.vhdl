-- File:	jtag_enh_rx.vhdl
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
-- JTAG RX shifter
--

library ieee;
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all;
use ieee.math_real.all;

entity jtag_enh_rx is
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
	-- vector address to store the input vector
	vec_addr : in std_logic_vector(VEC_ADDR_BITS - 1 downto 0) := (others => '0');
	-- offset in bits ...
	vec_offs : in std_logic_vector(VEC_OFFS_BITS - 1 downto 0) := (others => '0');
	-- vector shift direction ('0': LSB first, '1': MSB first)
	vec_msbf : in std_logic := '0';
	-- load/update the vector control data
	load_stb : in std_logic := '0'; 
	-- shift strobe
	shift_stb : in std_logic := '0';

	-- serial data input (TDO) 
	si : in std_logic;

	-- memory address 
	mem_addr : out std_logic_vector(VEC_ADDR_BITS - 1 downto 0);
	-- memory data data out
	mem_dout : out std_logic_vector(DATA_BITS - 1 downto 0) := (others => '0');	
	-- memory read ack
	mem_wr : out std_logic;

	-- DEBUG
	-- complete
	done : out std_logic;	
	-- buffer clear
	clear : out std_logic;	
	--
	fifo_get : out std_logic;	
	bit_pos : out std_logic_vector(VEC_OFFS_BITS - 1 downto 0);
	op_addr : out std_logic_vector(VEC_ADDR_BITS - 1 downto 0);
	op_len : out std_logic_vector(VEC_LEN_BITS - 1 downto 0);
	op_pos : out std_logic_vector(VEC_OFFS_BITS - 1 downto 0);
	op_msbf : out std_logic;	
	busy : out std_logic;
	start : out std_logic
);
end jtag_enh_rx;

architecture rtl of jtag_enh_rx is
	-- number of bits needed to store the data width
	constant DW_BITS : integer := integer(ceil(log2(real(DATA_BITS))));

	type vec_op_t is record
		len : unsigned(VEC_LEN_BITS - 1 downto 0);
		addr : unsigned(VEC_ADDR_BITS - 1 downto 0);
		pos : unsigned(DW_BITS - 1 downto 0);
		msbf : std_logic;
	end record vec_op_t;

	type vec_op_buf_t is array(3 downto 0) of vec_op_t;

	type vec_op_fifo_t is record
		buf : vec_op_buf_t;
		head : unsigned(1 downto 0);
		tail : unsigned(1 downto 0);
		empty : std_logic;
	end record vec_op_fifo_t;

	-- initial bit position
	signal s_pos_0 : unsigned(DW_BITS - 1 downto 0);
	-- initial word position in the vector memory
	signal s_addr_0 : unsigned(VEC_ADDR_BITS - 1 downto 0);
	-- current bit position
	signal s_pos_r0 : unsigned(DW_BITS - 1 downto 0);
	signal s_pos_r1 : unsigned(DW_BITS - 1 downto 0);
	-- current word position (address) in the vector memory
	signal s_addr_r0 : unsigned(VEC_ADDR_BITS - 1 downto 0);
	signal s_addr_r1 : unsigned(VEC_ADDR_BITS - 1 downto 0);
	signal s_addr_r2 : unsigned(VEC_ADDR_BITS - 1 downto 0);
	-- bit position wrap (underflow/overflow)
	signal s_wrap_stb : std_logic;

	-- bit count
	signal s_count_r : unsigned(VEC_LEN_BITS - 1 downto 0);
	signal s_cnt_z : std_logic;

	signal s_done_stb : std_logic;

	signal s_fifo_put : std_logic;
	signal s_fifo_get : std_logic;

	signal s_rx_msbf: std_logic;
	signal s_count_stb : std_logic;
	signal s_store_stb : std_logic;
	signal s_clear_stb : std_logic;
	-- receive buffer
	signal s_rxbuf : std_logic_vector(DATA_BITS - 1 downto 0);

	signal s_op_fifo : vec_op_fifo_t;
	signal s_rx_op : vec_op_t;
	signal s_op0 : vec_op_t;
	signal s_busy_r : std_logic;
	signal s_start_r : std_logic;
begin 
	-- compute the initial bit position
	s_op0.pos <= unsigned(vec_offs) when (vec_msbf = '0') else 
		unsigned(vec_len(DW_BITS - 1 downto 0)) + unsigned(vec_offs);
	-- compute the initial word position
	s_op0.addr <= unsigned(vec_addr) when (vec_msbf = '0') else
		unsigned(vec_addr) + unsigned(vec_offs) +
		unsigned(vec_len(VEC_LEN_BITS - 1 downto DW_BITS));
	-- most significant bit first flag
	s_op0.msbf <= vec_msbf;
	-- vector length
	s_op0.len <= unsigned(vec_len);

	-----------------------------------------------------------------------
	process (rst, clk, load_stb, s_busy_r)
	begin
		if (rst = '1') then
			s_busy_r <= '0';
		elsif rising_edge(clk) then
			if (load_stb = '1') then
				s_busy_r <= '1';
			elsif (s_done_stb = '1') then
				s_busy_r <= '0';
			end if;
		end if;
	end process;
	-----------------------------------------------------------------------

	-----------------------------------------------------------------------
	process (rst, clk)
	begin
		if (rst = '1') then
			s_start_r <= '0';
		elsif rising_edge(clk) then
			s_start_r <= (load_stb and not s_busy_r);
		end if;
	end process;
	-----------------------------------------------------------------------

	s_fifo_get <= s_start_r or (s_busy_r and s_cnt_z and not s_op_fifo.empty);
	s_fifo_put <= load_stb;
	s_count_stb <= shift_stb;
	s_clear_stb <= s_store_stb; 
	s_done_stb <= s_busy_r and s_cnt_z and s_op_fifo.empty;

	-----------------------------------------------------------------------
	-- load the vector operands and store on fifo (length, address, msbf)
	p_fifo : process (rst, clk, s_fifo_put, s_fifo_get)
		variable op : vec_op_t;
	begin
		if (rst = '1') then
			s_op_fifo.head <= (others => '0');
			s_op_fifo.tail <= (others => '0');
			s_op_fifo.empty <= '1';
		elsif rising_edge(clk)  then
			if (s_fifo_put = '1') then
				s_op_fifo.buf(to_integer(s_op_fifo.head)) <= s_op0;
				s_op_fifo.head <= s_op_fifo.head + 1;
				s_op_fifo.empty <= '0';
			end if;
			if (s_fifo_get = '1') then
				s_op_fifo.tail <= s_op_fifo.tail + 1;
				if (s_op_fifo.tail + 1) = s_op_fifo.head then
					s_op_fifo.empty <= '1';
				end if;
			end if;
		end if;
	end process p_fifo;
	-----------------------------------------------------------------------

	s_rx_op <= s_op_fifo.buf(to_integer(s_op_fifo.tail)) when 
		(s_busy_r = '1') else s_op0;

	-----------------------------------------------------------------------
	-- get the vector operands from fifo 	
	process (rst, clk, s_fifo_get)
	begin
		if (rst = '1') then
			s_rx_msbf <= '0';
		elsif rising_edge(clk) and (s_fifo_get = '1') then
			s_rx_msbf <= s_rx_op.msbf;
		end if;
	end process;	

	-----------------------------------------------------------------------
	-- bit count 
	p_bit_count : process (rst, clk, s_fifo_get, s_count_stb, s_count_r)
		variable cy : std_logic;
	begin
		cy := s_count_stb;
		for i in 0 to VEC_LEN_BITS - 1 loop
			if (rst = '1') then
				-- assynchronous reset
				s_count_r(i) <= '0';
			elsif rising_edge(clk) then
				if (s_fifo_get = '1') then
					s_count_r(i) <= s_rx_op.len(i);
				else 
					s_count_r(i) <= s_count_r(i) xor cy;
				end if;
			end if;
			cy := cy and not s_count_r(i);
		end loop;
		s_cnt_z <= cy;
	end process p_bit_count;
	-----------------------------------------------------------------------
	
	-----------------------------------------------------------------------
	-- update the bit position
	-- increment the position if msbf == '1', decrement otherwise
	-- load new bit position if load_stb == '1'
	p_pos: process (rst, clk, s_fifo_get, s_count_stb, s_rx_msbf, s_pos_r1)
		variable cy : std_logic;
	begin
		cy := s_count_stb;
		for i in 0 to DW_BITS - 1 loop
			if (rst = '1') then
				-- assynchronous reset
				s_pos_r1(i) <= '0';
			elsif rising_edge(clk) then
				if (s_fifo_get = '1') then
					-- load the new position
					s_pos_r1(i) <= s_rx_op.pos(i);
				else 
					s_pos_r1(i) <= s_pos_r1(i) xor cy;
				end if;
			end if;
			cy := (s_rx_msbf xor s_pos_r1(i)) and cy;
		end loop;
		s_wrap_stb <= cy;
	end process p_pos;

	-----------------------------------------------------------------------
	-- update the address
	p_addr: process (rst, clk, s_fifo_get, s_wrap_stb, s_rx_msbf, s_addr_r1)
		variable cy : std_logic;
	begin
		cy := s_wrap_stb;
		for i in 0 to VEC_ADDR_BITS - 1 loop
			if (rst = '1') then
				s_addr_r1(i) <= '0';		
			elsif rising_edge(clk) then
				if (s_fifo_get = '1') then
					s_addr_r1(i) <= s_rx_op.addr(i);
				else
					s_addr_r1(i) <= s_addr_r1(i) xor cy;
				end if;
			end if;
			cy := (s_rx_msbf xor s_addr_r1(i)) and cy;
		end loop;
	end process p_addr;
	-----------------------------------------------------------------------

	-----------------------------------------------------------------------
	-- input capture
	p_rxcap: process (rst, clk, s_clear_stb, shift_stb, si)
	begin
		for i in 0 to DATA_BITS - 1 loop
			if (rst = '1') then
				s_rxbuf(i) <= '0';
			elsif rising_edge(clk) then
				if (shift_stb = '1') and (i = s_pos_r1) then
					s_rxbuf(i) <= si;
				elsif (s_clear_stb = '1') then
					s_rxbuf(i) <= '0';
				end if;
			end if;
		end loop;

	end process p_rxcap;
	-----------------------------------------------------------------------

	-----------------------------------------------------------------------
	-- update the store strobe signal
	p_store_stb : process (rst, clk, s_wrap_stb, s_cnt_z)
	begin
		if (rst = '1') then
			s_store_stb <= '0';
		elsif rising_edge(clk) then
			s_store_stb <= s_busy_r and (s_wrap_stb or s_cnt_z);
		end if;
	end process p_store_stb;
	-----------------------------------------------------------------------

	-----------------------------------------------------------------------
	p_addr_r : process (rst, clk, shift_stb)
	begin
		if (rst = '1') then
			s_addr_r2 <= (others => '0');
		elsif rising_edge(clk) and (shift_stb = '1') then
			s_addr_r2 <= s_addr_r1;
		end if;
	end process p_addr_r;
	-----------------------------------------------------------------------

	-----------------------------------------------------------------------
	mem_addr <= std_logic_vector(s_addr_r2);
	mem_dout <= s_rxbuf;	
	mem_wr <= s_store_stb;
	bit_pos <= std_logic_vector(s_pos_r1);
	done <= s_done_stb;
	clear <= s_clear_stb;
	fifo_get <= s_fifo_get;


	op_addr <= std_logic_vector(s_rx_op.addr);
	op_pos <= std_logic_vector(s_rx_op.pos);
	op_msbf <= s_rx_msbf;
	op_len <= std_logic_vector(s_rx_op.len);
	
	busy <= s_busy_r;
	start <= s_start_r;
end rtl;


