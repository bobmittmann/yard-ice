-- File:	jtag_reg.vhdl
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
-- CRAM Synchronous Bus Adaptor
--

library ieee;
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all;
use ieee.math_real.all;

library work;
use work.counter;
use work.syncfifo;

entity cram_adaptor is
generic ( 
	MEM_SEL_BITS : integer := 1;
	MEM_ADDR_BITS : integer := 15;
	REG_SEL_BITS : integer := 3
);
port (
	-- main clock
	clk: in std_logic := '0';
	-- reset
	rst : in std_logic := '0';
	-- CRAM bus
	cram_clk : in std_logic := '0';
	cram_noe : in std_logic := '0';
	cram_nwe : in std_logic := '0';
	cram_nce : in std_logic := '0';
	cram_d : inout std_logic_vector(15 downto 0);
	-- Memory access bus
	mem_addr : out std_logic_vector(MEM_ADDR_BITS - 1 downto 0);
	mem_rd_data : in std_logic_vector(15 downto 0) := (others => '0');
	mem_rd_sel : out std_logic_vector(MEM_SEL_BITS - 1 downto 0);
	mem_rd_stb : out std_logic;
	mem_wr_data : out std_logic_vector(15 downto 0);
	mem_wr_sel : out std_logic_vector(MEM_SEL_BITS - 1 downto 0);
	mem_wr_stb : out std_logic;
	-- Register access bus
	reg_rd_data : in std_logic_vector(15 downto 0) := (others => '0');
	reg_rd_sel : out std_logic_vector(REG_SEL_BITS - 1 downto 0);
	reg_wr_data : out std_logic_vector(15 downto 0);
	reg_wr_sel : out std_logic_vector(REG_SEL_BITS - 1 downto 0);
	reg_wr_stb : out std_logic;
	-- Debug
	dbg_mem_rd : out std_logic;
	dbg_mem_wr : out std_logic;
	dbg_reg_rd : out std_logic;
	dbg_reg_wr : out std_logic
);
end cram_adaptor;

architecture rtl of cram_adaptor is

	constant DATA_WIDTH : natural := 16;
	constant BUS_ADDR_BITS : natural := DATA_WIDTH;

	-- register/memory selection address bit
	constant REG_SEL : natural := BUS_ADDR_BITS - 1;

	constant MEM_SEL_TOP : natural := BUS_ADDR_BITS - 2;
	constant MEM_SEL_BOT : natural := MEM_SEL_TOP - MEM_SEL_BITS + 1;

	-----------------------
	-- bust state decoding
	type cram_st_t is (MEMC_IDLE, MEMC_ADV, MEMC_DLAT, MEMC_DSTB);
	signal s_cram_st : cram_st_t;
	signal s_cram_next_st : cram_st_t;
	-- clock enable
	signal s_cram_clk : std_logic;
	-- output enable
	signal s_cram_oe : std_logic;
	-- read 
	signal s_cram_rd : std_logic;
	-- write
	signal s_cram_wr : std_logic;
	-- address valid
	signal s_cram_adv : std_logic;
	-- address increment
	signal s_cram_adi : std_logic;
	-- address latch
	signal s_cram_wr_addr : std_logic_vector(BUS_ADDR_BITS - 1 downto 0);
	signal s_cram_mem_rd_addr : std_logic_vector(BUS_ADDR_BITS - 1 downto 0);
	signal s_cram_reg_rd_addr : std_logic_vector(BUS_ADDR_BITS - 1 downto 0);
	-----------------------
	signal s_cram_dout : std_logic_vector(DATA_WIDTH - 1 downto 0);
	-----------------------

	-----------------------
	signal s_mem_rd_en : std_logic;
	-----------------------
--	signal s_reg_rd_en : std_logic;
	signal s_reg_wr_addr: std_logic_vector(REG_SEL_BITS - 1 downto 0);
	signal s_reg_wr_stb : std_logic;
	signal s_reg_rd_data : std_logic_vector(DATA_WIDTH - 1 downto 0);

	-----------------------
	signal s_op_mem_rd : std_logic;
	signal s_op_mem_wr : std_logic;
	signal s_op_reg_rd : std_logic;
	signal s_op_reg_wr : std_logic;
	signal s_start_stb : std_logic;
begin 
	---------------------------------------------------------------------------
	-- Synchronous CRAM type address multiplexed bus
	process (cram_clk, cram_nce)
	begin
		if (cram_nce = '1') then
			s_cram_st <= MEMC_IDLE;
		elsif rising_edge(cram_clk) then
			s_cram_st <= s_cram_next_st;
		end if;
	end process;

	process (s_cram_st, cram_noe, cram_nwe)
	begin
		case s_cram_st is
			when MEMC_IDLE =>
				s_cram_next_st <= MEMC_ADV;
			when MEMC_ADV =>
				if 	(cram_nwe = '0') then
					s_cram_next_st <= MEMC_DLAT;
				else
					s_cram_next_st <= MEMC_DSTB;
				end if;
			when MEMC_DLAT =>
				s_cram_next_st <= MEMC_DSTB;
			when MEMC_DSTB =>
				s_cram_next_st <= MEMC_DSTB;
		end case;
	end process;

	s_cram_rd <= '1' when ((s_cram_st = MEMC_DSTB) and (cram_noe = '0')) 
					else '0';
	s_cram_wr <= '1' when ((s_cram_st = MEMC_DSTB) and (cram_nwe = '0')) 
					else '0';
	s_cram_adv <= '1' when (s_cram_st = MEMC_ADV) else '0';
	s_cram_adi <= '1' when (s_cram_st = MEMC_DSTB) else '0';

	---------------------------------------------------------------------------
	-- Address latch / counter
	addr_r : entity counter
		generic map (DATA_WIDTH => DATA_WIDTH, COUNT_BITS => BUS_ADDR_BITS) 
		port map (
			-- I/O clock
			clk => cram_clk,
			-- reset
			rst => rst,
			-- load on address valid signal 
			ld => s_cram_adv,
			d => cram_d,
			-- count 
			cin => s_cram_adi,
			-- data out
			q(BUS_ADDR_BITS - 1 downto 0) => s_cram_wr_addr
			);
	---------------------------------------------------------------------------

	process (rst, cram_clk, s_cram_adv)
	begin
		if (rst = '1') then
			s_op_mem_wr <= '0';
			s_op_mem_rd <= '0';
			s_op_reg_wr <= '0';
			s_op_reg_rd <= '0';
		elsif rising_edge(cram_clk) and (s_cram_adv = '1') then
			s_op_mem_wr <= (cram_nwe nor cram_nce) and not cram_d(REG_SEL);
			s_op_mem_rd <= (cram_nwe and not cram_nce) and not cram_d(REG_SEL);
			s_op_reg_wr <= (cram_nwe nor cram_nce) and cram_d(REG_SEL);
			s_op_reg_rd <= (cram_nwe and not cram_nce) and cram_d(REG_SEL);
		end if;
	end process;

	process (rst, cram_clk, s_cram_adv)
	begin
		if (rst = '1') then
			s_start_stb <= '0';
		elsif rising_edge(cram_clk) then
			s_start_stb <= s_cram_adv;
		end if;
	end process;

	---------------------------------------------------------------------------
	-- The read selection lines have to be registered to delay 1 clock period
	process (rst, cram_clk)
	begin
		if (rst = '1') then
			s_cram_mem_rd_addr <= (others => '0');
		elsif rising_edge(cram_clk) then
			s_cram_mem_rd_addr <= s_cram_wr_addr;
		end if;
	end process;
	-- memory read selector
	s_mem_rd_en <= not s_cram_mem_rd_addr(REG_SEL);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- Memory bus interface
	mem_addr <= s_cram_wr_addr(MEM_ADDR_BITS - 1 downto 0);
	mem_wr_data <= cram_d;
	-- memory write strobe
	mem_wr_stb <= s_cram_wr and not s_cram_wr_addr(REG_SEL);
	-- memory block write selector
	mem_wr_sel <= s_cram_wr_addr(MEM_SEL_TOP downto MEM_SEL_BOT);
	-- memory block read strobe
	mem_rd_stb <= s_cram_rd and not s_cram_wr_addr(REG_SEL);
	-- memory block read selector
	mem_rd_sel <= s_cram_mem_rd_addr(MEM_SEL_TOP downto MEM_SEL_BOT);

	---------------------------------------------------------------------------
	-- Registers write address/data fifo 
	reg_wr_fifo : entity syncfifo 
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			ADDR_BITS => REG_SEL_BITS
		)
		port map (
			rst => rst,

			in_clk => cram_clk,
			in_data => cram_d,
			in_addr => s_cram_wr_addr(REG_SEL_BITS - 1 downto 0),
			in_put => s_cram_wr and s_cram_wr_addr(REG_SEL),

			out_clk => clk,
			out_data => reg_wr_data,
			out_addr => s_reg_wr_addr,
			out_get => s_reg_wr_stb
		);

	reg_wr_stb <= s_reg_wr_stb;
	reg_wr_sel <= s_reg_wr_addr;

	---------------------------------------------------------------------------
	-- Registers read address
	s_cram_reg_rd_addr <= s_cram_wr_addr;
	---------------------------------------------------------------------------
	-- Registers read selector
	reg_rd_sel <= s_cram_reg_rd_addr(REG_SEL_BITS - 1 downto 0);

	---------------------------------------------------------------------------
	-- Registers read latch
--	s_reg_rd_en <= s_cram_reg_rd_addr(REG_SEL) and s_cram_rd;
	process (rst, cram_clk)
	begin
		if (rst = '1') then
			s_reg_rd_data <= (others => '0');
		elsif rising_edge(cram_clk) then
			s_reg_rd_data <= reg_rd_data;
		end if;
	end process;

	---------------------------------------------------------------------------
	-- CRAM bus output
	s_cram_dout <= mem_rd_data when s_mem_rd_en = '1' else s_reg_rd_data;
	s_cram_oe <= (cram_noe nor cram_nce);
	cram_d <= s_cram_dout when (s_cram_oe  = '1') else (others => 'Z');


	dbg_reg_rd <= s_start_stb and s_op_reg_rd;
	dbg_reg_wr <= s_start_stb and s_op_reg_wr;

	dbg_mem_rd <= s_start_stb and s_op_mem_rd;
	dbg_mem_wr <= s_start_stb and s_op_mem_wr;

	---------------------------------------------------------------------------

end rtl;

