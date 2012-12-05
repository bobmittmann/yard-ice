-- File:	jtag_enh_shifter.vhdl
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
-- JTAG Enhanced shifter
-- Provides the timing signals to control the TX and RX shift registers
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;

library work;
use work.all;

entity jtag_enh_shifter is
generic (
	DATA_WIDTH : natural := 8;
	VEC_LEN_BITS : natural := 10;
	VEC_ADDR_BITS : natural := 10;
	PTR_DATA_BITS : natural := 16;
	PTR_ADDR_BITS : natural := 8;
	DESC_DATA_BITS : natural := 32;
	DESC_ADDR_BITS : natural := 7;
	CKGEN_DIV_BITS : natural := 16;
	INSN_BITS : natural := 16
);
port (
	-- system clock
	clk : in std_logic;
	-- IO clock
	ioclk : in std_logic := '0';	
	-- assynchronous reset
	rst : in std_logic := '0';

	-- exec strobe
	exec_stb : in std_logic := '0';
	-- instruction
	insn : in std_logic_vector(INSN_BITS - 1 downto 0) := (others => '0');

	-- RTCK (return clock)
	tdo : in std_logic := '0';
	rtck : in std_logic := '0';	

	-- Enable return clock operation
	ckgen_rtcken : in std_logic := '0';
	-- TCK divisor
	ckgen_div : in std_logic_vector(CKGEN_DIV_BITS - 1 downto 0) := 
		(1=>'1', others=>'0');
	-- RTCK enabled divisor
	ckgen_rtdiv : in std_logic_vector(CKGEN_DIV_BITS - 1 downto 0) := 
		(others=>'1');

	-- TCK output
	tck : out std_logic;
	tdi : out std_logic;
	tms : out std_logic;
	-- RTCK timeout
	rtcktmo : out std_logic;

	-- RX memory address
	rx_mem_addr : out std_logic_vector((VEC_ADDR_BITS - 1) downto 0);
	-- RX memory data
	rx_mem_data : out std_logic_vector((DATA_WIDTH - 1) downto 0);
	-- RX memory write strobe
	rx_mem_wr : out std_logic;

	-- TX memory address
	tx_mem_addr : out std_logic_vector((VEC_ADDR_BITS - 1) downto 0);
	-- TX memory data
	tx_mem_data : in std_logic_vector((DATA_WIDTH - 1) downto 0);
	-- TX memory read strobe
	tx_mem_rd : out std_logic;

	-- pointer address
	ptr_addr : out std_logic_vector(PTR_ADDR_BITS - 1 downto 0);
	-- pointer data
	ptr_data : in std_logic_vector(PTR_DATA_BITS - 1  downto 0);
	-- pointer read strobe
	ptr_rd_stb : out std_logic;

	-- descriptor address
	desc_addr : out std_logic_vector(DESC_ADDR_BITS - 1 downto 0);
	-- descriptor data
	desc_data : in std_logic_vector(DESC_DATA_BITS - 1 downto 0);
	-- descriptor read strobe
	desc_rd_stb : out std_logic;

	
	-- shift start acknowlegment
	exec_ack : out std_logic;
	exec_end : out std_logic;
	
	-- DEBUG
	-- Main clock domain TCK positive strobe
	tckp_stb : out std_logic;
	-- Main clock domain TCK negative strobe
	tckn_stb : out std_logic;
	-- TAP state
	tap_state : out unsigned(3 downto 0);
	insn_invalid : out std_logic;
	insn_reset : out std_logic;
	insn_goto : out std_logic;
	insn_drscan : out std_logic;
	insn_irscan : out std_logic;
	insn_runtest : out std_logic
);
end jtag_enh_shifter;

architecture structure of jtag_enh_shifter is
	-- number of bits needed to store the data width
	constant VEC_OFFS_BITS : integer := integer(ceil(log2(real(DATA_WIDTH))));

	signal s_ptr_rd_stb : std_logic;
	signal s_desc_rd_stb : std_logic;
	signal s_rx_shift_stb: std_logic;
	signal s_tx_shift_stb: std_logic;
	signal s_start_ack : std_logic;
	signal s_tckp : std_logic;
	signal s_tckn : std_logic;
	signal s_tckp_stb : std_logic;
	signal s_tckn_stb : std_logic;

	signal s_mtms : std_logic;
	signal s_mtdi : std_logic;
	signal s_mtdo : std_logic;

	signal s_iotckp : std_logic;
	signal s_iotckn : std_logic;

	-- shift start strobe 
	signal s_shift_start_stb : std_logic;

	-- TCK enable
	signal s_tcken : std_logic;
	-- TDI enable
	signal s_tdien : std_logic;
	signal s_so : std_logic;

	-- end of transmission 
	signal s_tx_done_stb : std_logic;
	-- end of reception
	signal s_rx_done_stb : std_logic;

	-- descriptor table base address
	signal s_desc_base : std_logic_vector(DESC_ADDR_BITS - 1 downto 0);		
	signal s_desc_len : std_logic_vector(DESC_ADDR_BITS - 1 downto 0);		

	-- instruction decode flags
	signal s_insn_irscan : std_logic;
	signal s_insn_drscan : std_logic;

	-- vector descriptor 
	signal s_vec_len : std_logic_vector(VEC_LEN_BITS - 1 downto 0);
	signal s_tx_vec_addr : std_logic_vector(VEC_ADDR_BITS - 1 downto 0);
	signal s_tx_vec_msbf : std_logic;
	signal s_rx_vec_addr : std_logic_vector(VEC_ADDR_BITS - 1 downto 0);
	signal s_rx_vec_msbf : std_logic;

begin 
	-- split the pointer descriptor into fields
	s_desc_len <= ptr_data(PTR_DATA_BITS - DESC_ADDR_BITS - 1 downto 0);
	s_desc_base <= ptr_data(PTR_DATA_BITS - 1 downto 
							PTR_DATA_BITS - DESC_ADDR_BITS);

	-- split the vector descriptor into fields
	s_vec_len <= desc_data(VEC_LEN_BITS - 1 downto 0);
	s_tx_vec_addr <= desc_data(10 + VEC_ADDR_BITS - 1 downto 10);
	s_rx_vec_addr <= desc_data(20 + VEC_ADDR_BITS - 1 downto 20);
	s_tx_vec_msbf <= desc_data(30);
	s_rx_vec_msbf <= desc_data(31);

	-----------------------------------------------------------------------
	-- clock generator
	ckgen : entity jtag_enh_ckgen
		generic map (
			DIV_BITS => CKGEN_DIV_BITS
		)
		port map (
			clk => ioclk,
			rst => rst,
			en => s_tcken ,
			div => ckgen_div,
			rtdiv => ckgen_rtdiv,
			rtck => rtck,
			rtcken => ckgen_rtcken,
			tck => tck,
			iotckp => s_iotckp,
			iotckn => s_iotckn,
			rtcktmo => rtcktmo
		);
	-----------------------------------------------------------------------

	-----------------------------------------------------------------------
	-- input/output capture
	io : entity jtag_enh_io
		port map (
			mclk => clk,
			rst => rst,
			ioclk => ioclk,
			iotckp => s_iotckp,
			iotckn => s_iotckn,
			mtms => s_mtms,
			mtdi => s_mtdi,
			tdo => tdo,
			tckp_stb => s_tckp_stb,
			tckn_stb => s_tckn_stb,
			tms => tms,
			tdi => tdi,
			mtdo => s_mtdo
		);
	-----------------------------------------------------------------------

	-----------------------------------------------------------------------
	-- TX memory shift register
	tx : entity jtag_enh_tx
		generic map (
			DATA_BITS => DATA_WIDTH,
			VEC_ADDR_BITS => VEC_ADDR_BITS,
			VEC_LEN_BITS => VEC_LEN_BITS,
			VEC_OFFS_BITS => VEC_OFFS_BITS
		)
		port map (
			clk => clk,
			rst => rst,
			vec_len => s_vec_len,
			vec_addr => s_tx_vec_addr,
			vec_offs => (others => '0'),
			vec_msbf => s_tx_vec_msbf,
			load_stb => s_desc_rd_stb,
			shift_stb => s_tx_shift_stb,
			mem_addr => tx_mem_addr,
			mem_din => tx_mem_data,
			mem_rd => tx_mem_rd,
			so => s_so
		);

	s_mtdi <= s_so when (s_tdien = '1') else '0';
	-----------------------------------------------------------------------

	-----------------------------------------------------------------------
	-- RX memory shift register
	rx : entity jtag_enh_rx
		generic map (
			DATA_BITS => DATA_WIDTH,
			VEC_ADDR_BITS => VEC_ADDR_BITS,
			VEC_LEN_BITS => VEC_LEN_BITS,
			VEC_OFFS_BITS => VEC_OFFS_BITS
		)
		port map (
			clk => clk,
			rst => rst,
			vec_len => s_vec_len,
			vec_addr => s_rx_vec_addr,
			vec_offs => (others => '0'),
			vec_msbf => s_rx_vec_msbf,
			load_stb => s_desc_rd_stb,
			shift_stb => s_rx_shift_stb,
			si => s_mtdo,
			mem_addr => rx_mem_addr,
			mem_dout => rx_mem_data,
			mem_wr => rx_mem_wr
		);
	-----------------------------------------------------------------------

	-----------------------------------------------------------------------
	-- shift control 
	shift_ctrl : entity jtag_enh_shift_ctrl
		generic map (
			VEC_LEN_BITS => VEC_LEN_BITS,
			DESC_ADDR_BITS => DESC_ADDR_BITS
		)
		port map (
			clk => clk,
			rst => rst,
			en => s_shift_start_stb,
			tckp_stb => s_tckp_stb,
			tckn_stb => s_tckn_stb,
			vec_len => s_vec_len,
			desc_base => s_desc_base,
			desc_len => s_desc_len,
			desc_addr => desc_addr,
			desc_rd_stb => s_desc_rd_stb,
			rx_shift_stb => s_rx_shift_stb,
			tx_shift_stb => s_tx_shift_stb,
			rx_done_stb => s_rx_done_stb,
			tx_done_stb => s_tx_done_stb
		);
	-----------------------------------------------------------------------

	-----------------------------------------------------------------------
	-- Execution Control Unit
	exec_ctrl : entity jtag_enh_ecu
		generic map (
			INSN_BITS => INSN_BITS,
			DESC_ADDR_BITS => DESC_ADDR_BITS
		)
		port map (
			clk => clk,
			rst => rst,
			tckp_stb => s_tckp_stb,
			tckn_stb => s_tckn_stb,
			exec_stb => exec_stb,
			insn => insn,
			rx_done_stb => s_rx_done_stb,
			tx_done_stb => s_tx_done_stb,
			tck_en => s_tcken,
			mtms => s_mtms,
			tdi_en => s_tdien,
			shift_en => s_shift_start_stb,
			ptr_addr => ptr_addr,
			ptr_rd_stb => s_ptr_rd_stb,
			start_ack => exec_ack,
			end_stb => exec_end,
			tap_state => tap_state,
			insn_invalid => insn_invalid,
			insn_reset => insn_reset,
			insn_goto => insn_goto,
			insn_drscan => s_insn_drscan,
			insn_irscan => s_insn_irscan,
			insn_runtest => insn_runtest
		);
	-----------------------------------------------------------------------

	insn_drscan <= s_insn_drscan;
	insn_irscan <= s_insn_irscan;

	tckp_stb <= s_tckp_stb;
	tckn_stb <= s_tckn_stb;

	ptr_rd_stb <= s_ptr_rd_stb;
	desc_rd_stb <= s_desc_rd_stb;

end structure;

