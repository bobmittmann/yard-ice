-- File:	jtagtool3.vhdl
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
-- Enhaced JTAG controller
-- 

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library work;
use work.all;

entity jtagtool3 is
port(
	-- Main clock
	mclk : in std_logic;
	-- STM32F Bux data ines
	fsmc_d : inout std_logic_vector(15 downto 0);
	-- STM32F Bus control lines
	fsmc_clk : in std_logic;
	fsmc_noe : in std_logic;
	fsmc_nwe : in std_logic;
	fsmc_nce : in std_logic;
	-- IRQ 
	irq : out std_logic;
	-- Serial port (USART)
	uart_rx : out std_logic;
	uart_tx : in std_logic;

	-- TAP extra
	tp_dbgrq : out std_logic;
	tp_dbgack : in std_logic;
	-- TAP port
	tp_tdo : in std_logic;
	tp_tck : out std_logic;
	tp_tdi : out std_logic;
	tp_tms : out std_logic;
	-- TAP return clock
	tp_rtck : in std_logic;
	-- TAP reset
	tp_trst : out std_logic;
	-- Target reset
	tp_rst : out std_logic;
	-- leds
	led_1 : out std_logic;
	led_2 : out std_logic;
	--
	clk_aux : in std_logic
	
	--DEBUG :
--	tap_state : out unsigned(3 downto 0)	
);
end jtagtool3;

architecture structure of jtagtool3 is
	
	constant DATA_WIDTH : natural := 16;
	constant ADDR_BITS : natural := 13;

	constant MEM_SEL_BITS : natural := 2;
	constant MEM_ADDR_BITS : natural := 12;

	constant VEC_LEN_BITS : natural := 10;
	constant VEC_ADDR_BITS : natural := 10;
	constant DESC_DATA_BITS : natural := 32;
	constant DESC_ADDR_BITS : natural := 8;
	constant PTR_DATA_BITS : natural := 16;
	constant PTR_ADDR_BITS : natural := 8;
	constant CKGEN_DIV_BITS : natural := 16;
	constant INSN_BITS : natural := 16;

	constant REG_SEL_BITS : natural := 4;

	-- configuration bits
	subtype CFG_CLK_SEL is natural range 3 downto 0;
	constant CFG_LOOP : natural := 4;
	constant CFG_RTCK_EN : natural := 5;
	constant CFG_RST : natural := 6;
	constant CFG_UART_EN : natural := 7;
	constant CFG_TAP_TRST : natural := 10;
	constant CFG_TAP_NRST : natural := 11;
	constant CFG_REG_BITS : natural := 12; -- width of cfg reg

	-- registers 
	constant REG_INSN : natural := 0;
	constant REG_STATUS : natural := 0;
	constant REG_INT_ST : natural := 1;
	constant REG_INT_EN : natural := 2;
	constant REG_3 : natural := 3;
	constant REG_CKGEN_DIV : natural := 4;
	constant REG_CKGEN_RTDIV : natural := 5;
	constant REG_CFG : natural := 6;
	constant REG_TMR : natural := 7;

	-- interrupts
	constant IRQ_TAP : natural := 0;
	constant IRQ_RTCKTMO : natural := 1;
	constant IRQ_1KHZ : natural := 2;
	constant IRQ_TMR : natural := 3;
	constant IRQ_BITS : natural := 4;

	-- clocks 
	signal s_clk_main : std_logic;
	signal s_clk_bus : std_logic;
	signal s_clk_120mhz : std_logic;
--	signal s_clk_1mhz : std_logic;
--	signal s_clk_1khz : std_logic;
--	signal s_clk_2hz : std_logic;
	-- clock strobes
	signal s_1mhz_stb : std_logic;
	signal s_1khz_stb : std_logic;
	signal s_2hz_stb : std_logic;

	---------------------------------------------------------------------------
	-- Internal Bus
	---------------------------------------------------------------------------
	-- internal memory bus
	signal s_mem_addr : std_logic_vector(MEM_ADDR_BITS downto 1);

	signal s_mem_wr_stb : std_logic;
	signal s_mem_wr_sel : std_logic_vector(MEM_SEL_BITS - 1 downto 0);
	signal s_mem_rd_sel : std_logic_vector(MEM_SEL_BITS - 1 downto 0);

	signal s_mem1_wr : std_logic;
	signal s_mem1_dout : std_logic_vector(DATA_WIDTH - 1 downto 0);

	signal s_mem2_wr : std_logic;
	signal s_mem2_dout : std_logic_vector(DATA_WIDTH - 1 downto 0);

	signal s_mem_dout : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_mem_din : std_logic_vector(DATA_WIDTH - 1 downto 0);

	-- vector descriptor table memory access
	-- descriptor address
	signal s_desc_addr : std_logic_vector(DESC_ADDR_BITS downto 1);
	signal s_desc_data : std_logic_vector(DESC_DATA_BITS -1 downto 0);
	signal s_desc_mem_wr_stb : std_logic;

	-- vector descriptor pointer memory access
	signal s_ptr_addr : std_logic_vector(PTR_ADDR_BITS downto 1);
	signal s_ptr_data : std_logic_vector(PTR_DATA_BITS - 1 downto 0);
	signal s_ptr_mem_wr_stb : std_logic;

	-- Register access
	signal s_reg_wr_sel : std_logic_vector(REG_SEL_BITS - 1 downto 0);
	signal s_reg_wr_stb : std_logic;
	signal s_reg_rd_sel : std_logic_vector(REG_SEL_BITS - 1 downto 0);
	signal s_reg_din : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_reg_dout : std_logic_vector(DATA_WIDTH - 1 downto 0);


	---------------------------------------------------------------------------
	-- Registers  
	---------------------------------------------------------------------------

	-- instruction register
--	signal s_insn : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_status : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_insn_wr_stb : std_logic;
	-- TAP state
	signal s_tap_state : unsigned(3 downto 0);
	signal s_tap_busy : std_logic;

	-- interrupt status (R/C)
	signal s_irq_st : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_irq_st_wr_stb : std_logic;
	
	-- interrupt enable (W/O)
	signal s_irq_en : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_irq_en_wr_stb : std_logic;

	-- spare register (R/W)
	signal s_r3 : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_r3_wr_stb : std_logic;
	-- clock generator generator divider (R/W)
	signal s_ckgen_div : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_ckgen_div_wr_stb : std_logic;
	-- RTCK clock generator generator divider (R/W)
	signal s_ckgen_rtdiv : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_ckgen_rtdiv_wr_stb : std_logic;
	-- configuration register
	signal s_cfg : std_logic_vector(DATA_WIDTH - 1 downto 0); 
	signal s_cfg_wr_stb : std_logic; 
	-- timer register
	signal s_tmr : std_logic_vector(DATA_WIDTH - 1 downto 0); 
	signal s_tmr_wr_stb : std_logic; 
	signal s_tmr_cnt_stb : std_logic; 
	signal s_tmr_cnt_en : std_logic; 
	signal s_tmr_timeout : std_logic; 

	-- interrupt requests
	signal s_irq : std_logic_vector(IRQ_BITS - 1 downto 0);
	signal s_irq_reg : std_logic_vector(IRQ_BITS - 1 downto 0);
	signal s_irq_set : std_logic_vector(IRQ_BITS - 1 downto 0);
	signal s_irq_act : std_logic_vector(IRQ_BITS - 1 downto 0);
	signal s_irq_out : std_logic;

	signal s_irq_tap : std_logic;
	signal s_irq_rtcktmo : std_logic;
	signal s_irq_1khz : std_logic;
	signal s_irq_tmr : std_logic;

	-- global reset
	signal s_rst : std_logic;
	-- TAP output selection: NORMAL/LOOPBACK
	signal s_loopback_en : std_logic;
	-- Clock generator RTCK enable
	signal s_ckgen_rtck_en : std_logic;

	-- JTAG execution unit connections  
	-- Exec strobe
	signal s_exec_stb : std_logic;

	-- TAP signals
	signal s_tap_tdo : std_logic;
	signal s_tap_rtck : std_logic;
	signal s_tap_tck : std_logic;
	signal s_tap_tdi : std_logic;
	signal s_tap_tms : std_logic;
	signal s_tap_trst : std_logic;
	signal s_tap_nrst : std_logic;


	-- RX memory address
	signal s_rx_mem_addr : std_logic_vector((VEC_ADDR_BITS - 1) downto 0);
	-- RX memory data
	signal s_rx_mem_din : std_logic_vector((DATA_WIDTH - 1) downto 0);
	signal s_rx_mem_dout : std_logic_vector((DATA_WIDTH - 1) downto 0);
	-- RX memory write strobe
	signal s_rx_mem_wr_stb : std_logic;

	-- TX memory address
	signal s_tx_mem_addr : std_logic_vector((VEC_ADDR_BITS - 1) downto 0);
	-- TX memory data
	signal s_tx_mem_dout : std_logic_vector((DATA_WIDTH - 1) downto 0);
	-- TX memory read strobe
--	signal s_tx_mem_rd_stb : std_logic;
	signal s_tx_mem_wr_stb : std_logic;

	-- execution start acknowlegment
--	signal s_exec_ack : std_logic;
	-- execution end strobe
--	signal s_exec_end_stb : std_logic;

	
	-- serial port 
	signal s_uart_rx : std_logic;
	signal s_uart_tx : std_logic;
	signal s_uart_en : std_logic;
	-----------------------

	-----------------------
	signal s_dbgrq_out : std_logic;
	signal s_dbgack_in : std_logic;
	-----------------------

	function is_zero(x : std_logic_vector) return std_logic is
		variable y : std_logic := '0';
	begin
		for i in x'range loop
			y := y or x(i);
		end loop; 
		return not y;
	end is_zero;

	function reg(n : natural) return std_logic_vector is
	begin
		return std_logic_vector(to_unsigned(n, REG_SEL_BITS));
	end reg;

begin
	-- main clock (60MHz)
	s_clk_main <= mclk;

--	pll : cyc_pll port map(
--		inclk0  => s_clk_main,
--		c1 => s_clk_120mhz
--	);
           
	-- IO clock
--	s_clk_io <= s_clk_120mhz;
--	s_clk_io <= s_clk_main;
	s_clk_bus <= fsmc_clk;

	---------------------------------------------------------------------------
	-- CRAM bus adaptor
	cram_bus : entity cram_adaptor
		generic map (
			MEM_SEL_BITS => MEM_SEL_BITS,
			MEM_ADDR_BITS => MEM_ADDR_BITS,
			REG_SEL_BITS => REG_SEL_BITS)
		port map (
			clk => s_clk_main,
			rst => s_rst,

			cram_clk => fsmc_clk,
			cram_noe => fsmc_noe,
			cram_nwe => fsmc_nwe,
			cram_nce => fsmc_nce,
			cram_d => fsmc_d,

			mem_addr  => s_mem_addr,
			mem_rd_data => s_mem_dout,
			mem_rd_sel  => s_mem_rd_sel,
			mem_wr_data  => s_mem_din,
			mem_wr_sel  => s_mem_wr_sel,
			mem_wr_stb  => s_mem_wr_stb,

			reg_rd_data => s_reg_dout,
			reg_rd_sel  => s_reg_rd_sel,
			reg_wr_data  => s_reg_din,
			reg_wr_sel  => s_reg_wr_sel,
			reg_wr_stb  => s_reg_wr_stb
		);

	---------------------------------------------------------------------------
	-- Memory address range selection

	-- input selection decoder  (write)
	s_tx_mem_wr_stb <= s_mem_wr_stb when s_mem_wr_sel = "00" else '0';
	s_desc_mem_wr_stb <= s_mem_wr_stb when s_mem_wr_sel = "01" else '0';
	s_ptr_mem_wr_stb <= s_mem_wr_stb when s_mem_wr_sel = "10" else '0';

	---------------------------------------------------------------------------
	-- output selection multiplexer (read)
	with s_mem_rd_sel select
		s_mem_dout <= s_rx_mem_dout when "00",
		(others => '0') when others; 

	-- Memory map 
    --	
	--  1 xxx xxxx xxxx xxxx - Registers
	--  0 10x xxxx xxxx xxxx - Descriptor Pointers
	--  0 01x xxxx xxxx xxxx - Vector Descriptors
	--  0 00x xxxx xxxx xxxx - Vector Memory
	--
	-- +---------+---------------+---------------+
	-- | Address | Read          | Write         |
	-- +---------+---------------+---------------+ 
	-- |  0xffff |               |               |
	-- |   ...   | Invalid       | Invalid       |
	-- |         |               |               |
	-- |  0x8010 |               |               |
	-- +---------+---------------+---------------+ 
	-- |  0x800f |               |               |
	-- |   ...   | Registers     |  Control      |
	-- |         |               |  Registers    |
	-- |  0x8000 |               |               |
	-- +---------+---------------+---------------+ 
	-- |  0x7fff |               |               |
	-- |   ...   | Invalid       | Invalid       |
	-- |         |               |               |
	-- |  0x4200 |               |               |
	-- +---------+---------------+---------------+
	-- |  0x41ff |               |               |
	-- |   ...   |  Invalid      | Descriptor    |
	-- |         |               | Pointers      |
	-- |  0x4000 |               |               |
	-- +---------+---------------+---------------+ 
	-- |  0x3fff |               |               |
	-- |   ...   | Invalid       | Invalid       |
	-- |         |               |               |
	-- |  0x2100 |               |               |
	-- +---------+---------------+---------------+
	-- |  0x20ff |               |               |
	-- |   ...   | Vector        | Vector        |
	-- |         | Descriptors   | Descriptors   |
	-- |  0x2000 |               |               |
	-- +---------+---------------+---------------+
	-- |  0x1fff |               |               |
	-- |   ...   | Invalid       | Invalid       |
	-- |         |               |               |
	-- |  0x1000 |               |               |
	-- +---------+---------------+---------------+
	-- |  0x0fff |               |               |
	-- |   ...   | Vector TX     | Vector RX     |
	-- |         | Memory        | Memory        |
	-- |  0x0000 |               |               |
	-- +---------+---------------+---------------+
	--
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- register output multiplexer
	with s_reg_rd_sel select 
		s_reg_dout <= s_status when reg(REG_STATUS), 
		s_irq_st when reg(REG_INT_ST),
		s_irq_en when reg(REG_INT_EN),
		s_r3 when reg(REG_3),
		s_ckgen_div when reg(REG_CKGEN_DIV),
		s_ckgen_rtdiv when reg(REG_CKGEN_RTDIV),
		s_cfg when reg(REG_CFG),
		s_tmr when reg(REG_TMR),
		(others => '0') when others;

	-- register write enable decoder 
	s_insn_wr_stb <= s_reg_wr_stb 
		when s_reg_wr_sel = reg(REG_INSN) else '0';
	s_irq_st_wr_stb <= s_reg_wr_stb 
		when s_reg_wr_sel = reg(REG_INT_ST) else '0';
	s_irq_en_wr_stb <= s_reg_wr_stb 
		when s_reg_wr_sel = reg(REG_INT_EN) else '0';
	s_r3_wr_stb <= s_reg_wr_stb 
		when s_reg_wr_sel = reg(REG_3) else '0';
	s_ckgen_div_wr_stb <= s_reg_wr_stb 
		when s_reg_wr_sel = reg(REG_CKGEN_DIV) else '0';
	s_ckgen_rtdiv_wr_stb <= s_reg_wr_stb 
		when s_reg_wr_sel = reg(REG_CKGEN_RTDIV) else '0';
	s_cfg_wr_stb <= s_reg_wr_stb 
		when s_reg_wr_sel = reg(REG_CFG) else '0';
	s_tmr_wr_stb <= s_reg_wr_stb 
		when s_reg_wr_sel = reg(REG_TMR) else '0';
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- instruction register
--	insn_r : entity jtag_reg
--		generic map (DATA_WIDTH => DATA_WIDTH, REG_BITS => INSN_BITS)
--		port map (
--			-- system clock
--			clk => s_clk_main,
--			-- reset
--			rst => s_rst,
--			-- read signal 
--			ld => s_insn_wr_stb,
--			-- data in
--			d => s_bus_din,
--			-- data out
--			q => s_insn);
	s_exec_stb <= s_insn_wr_stb;
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- INTERRUPTS
	-- interrupts assignments
	s_irq(IRQ_TAP) <= s_irq_tap;
	s_irq(IRQ_RTCKTMO) <= s_irq_rtcktmo;
	s_irq(IRQ_1KHZ) <= s_irq_1khz;
	s_irq(IRQ_TMR) <= s_irq_tmr;
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- interrupt request synchronous edge detect
	process(s_clk_main)
	begin
		if rising_edge(s_clk_main) then
			s_irq_reg <= s_irq;
		end if;
	end process;
--	s_irq_set <= s_irq and not s_irq_reg;
	s_irq_set <= s_irq;
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- interrupt status register
	int_st_r : entity jtag_reg
		generic map (DATA_WIDTH => DATA_WIDTH, REG_BITS => IRQ_BITS)
		port map (
			-- system clock
			clk => s_clk_main,
			-- reset
			rst => s_rst,
			-- write '1' to clear register bits
			clr => s_irq_st_wr_stb,
			d_clr => s_reg_din(IRQ_BITS - 1 downto 0),
			-- set individual bits
			set => '1',
			d_set => s_irq_set
			-- data out
--			q => s_irq_st
			);

	s_irq_st(IRQ_BITS - 1 downto 0) <= s_irq_reg;
	s_irq_st(DATA_WIDTH -1 downto IRQ_BITS) <= (others => '0');

	-- IRQ output 
	process(s_clk_main)
	begin
		if rising_edge(s_clk_main) then
			s_irq_out <= not is_zero(s_irq_en and s_irq);
		end if;
	end process;

	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- interrupt enable register
	int_en_r : entity jtag_reg
		generic map (DATA_WIDTH => DATA_WIDTH, REG_BITS => IRQ_BITS)
		port map (
			-- system clock
			clk => s_clk_main,
			-- reset
			rst => s_rst,
			-- read signal 
			ld => s_irq_en_wr_stb,
			-- data in
			d => s_reg_din,
			-- data out
			q => s_irq_en);

	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- configuration register
	cfg_r : entity jtag_reg
		generic map (DATA_WIDTH => DATA_WIDTH, REG_BITS => DATA_WIDTH) 
		port map (
			-- system clock
			clk => s_clk_main,
			-- reset
			rst => s_rst,
			-- write signal 
			ld => s_cfg_wr_stb,
			-- data in
			d => s_reg_din,
			-- data out
			q => s_cfg,
			-- reset
			clr => s_rst);

	s_rst <= s_cfg(CFG_RST); -- Global reset
	s_ckgen_rtck_en <= s_cfg(CFG_RTCK_EN);
	s_loopback_en <= s_cfg(CFG_LOOP);
	s_tap_trst <= '0' when s_cfg(CFG_TAP_TRST) = '1' else '1';
	s_tap_nrst <= '0' when s_cfg(CFG_TAP_NRST) = '1' else 'Z';
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- timer register
	tmr_r : entity counter
		generic map (DATA_WIDTH => DATA_WIDTH, COUNT_BITS => DATA_WIDTH) 
		port map (
			-- system clock
			clk => s_clk_main,
			-- reset
			rst => s_rst,
			up => '0',
			cin => s_tmr_cnt_stb,
			-- load on write signal 
			ld => s_tmr_wr_stb,
			d => s_reg_din,
			-- count 
			cout => s_tmr_timeout,
			-- data out
			q => s_tmr);

	s_tmr_cnt_stb <= s_1mhz_stb and s_tmr_cnt_en;

	process (s_clk_main, s_rst, s_tmr_wr_stb, s_tmr_timeout)
	begin
		if (s_rst = '1') then
			s_tmr_cnt_en <= '0';
		elsif rising_edge(s_clk_main) then
			if (s_tmr_wr_stb = '1') then
				s_tmr_cnt_en <= '1';
			elsif (s_tmr_timeout = '1') then
				s_tmr_cnt_en <= '0';
			end if;
		end if;
	end process;

	s_irq_tmr <= not s_tmr_cnt_en;

	---------------------------------------------------------------------------


	---------------------------------------------------------------------------
	-- spare register 
	r3 : entity jtag_reg
		generic map (DATA_WIDTH => DATA_WIDTH, REG_BITS => DATA_WIDTH) 
		port map (
			-- system clock
			clk => s_clk_main,
			-- reset 
			rst => s_rst,
			-- read signal 
			ld => s_r3_wr_stb,
			-- data in
			d => s_reg_din,
			-- data out
			q => s_r3);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- clock generator divider register
	ckgen_div_r : entity jtag_reg
		generic map (DATA_WIDTH => DATA_WIDTH, REG_BITS => CKGEN_DIV_BITS) 
		port map (
			-- system clock
			clk => s_clk_main,
			-- reset 
			rst => s_rst,
			-- read signal 
			ld => s_ckgen_div_wr_stb,
			-- data in
			d => s_reg_din,
			-- data out
			q => s_ckgen_div);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- clock generator RTCK divider register
	ckgen_rtdiv_r : entity jtag_reg
		generic map (DATA_WIDTH => DATA_WIDTH, REG_BITS => CKGEN_DIV_BITS) 
		port map (
			-- system clock
			clk => s_clk_main,
			-- reset 
			rst => s_rst,
			-- read signal 
			ld => s_ckgen_rtdiv_wr_stb,
			-- data in
			d => s_reg_din,
			-- data out
			q => s_ckgen_rtdiv);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- vector descriptors memory
	desc_mem : entity jtag_desc_ram
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			ADDR_WIDTH => DESC_ADDR_BITS + 1,
			DESC_DATA_BITS => DESC_DATA_BITS,
			DESC_ADDR_BITS=> DESC_ADDR_BITS
		)
		port map (
			io_clk => s_clk_bus,
			io_addr => s_mem_addr(DESC_ADDR_BITS + 1 downto 1),
			io_we => s_desc_mem_wr_stb, 
			io_data => s_mem_din,

			clk => s_clk_main,
			desc_addr => s_desc_addr,
			desc_q => s_desc_data
		);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- descriptor pointers memory (Write only)
	ptr_mem : entity dpram
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			ADDR_WIDTH => PTR_ADDR_BITS
		)
		port map (
			clk1 => s_clk_bus,
			addr1 => s_mem_addr(PTR_ADDR_BITS downto 1),
			we1 => s_ptr_mem_wr_stb, 
			data1 => s_mem_din,

			clk2 => s_clk_main,
			addr2 => s_ptr_addr,
			q2 => s_ptr_data
		);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- rx vector memory (Read only)
	rx_mem : entity dpram
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			ADDR_WIDTH => VEC_ADDR_BITS
		)
		port map (
			clk1 => s_clk_main,
			addr1 => s_rx_mem_addr,
			we1 => s_rx_mem_wr_stb, 
			data1 => s_rx_mem_din,
	--		data1 => s_tx_mem_dout,

			clk2 => s_clk_bus,
			addr2 => s_mem_addr(VEC_ADDR_BITS downto 1),
			q2 => s_rx_mem_dout
		);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- tx vector memory (Write only)
	tx_mem : entity dpram
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			ADDR_WIDTH => VEC_ADDR_BITS
		)
		port map (
			clk1 => s_clk_bus,
			addr1 => s_mem_addr(VEC_ADDR_BITS downto 1),
			we1 => s_tx_mem_wr_stb, 
			data1 => s_mem_din,

			clk2 => s_clk_main,
			addr2 => s_tx_mem_addr,
			q2 => s_tx_mem_dout
		);

--	 s_tx_mem_dout <= "0101010100110011";
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- JTAG controller
	shifter : entity jtag_enh_shifter
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			VEC_LEN_BITS => VEC_LEN_BITS,
			VEC_ADDR_BITS => VEC_ADDR_BITS,
			DESC_DATA_BITS => DESC_DATA_BITS,
			DESC_ADDR_BITS => DESC_ADDR_BITS,
			PTR_DATA_BITS => PTR_DATA_BITS,
			PTR_ADDR_BITS => PTR_ADDR_BITS,
			CKGEN_DIV_BITS => CKGEN_DIV_BITS,
			INSN_BITS => INSN_BITS
		)
		port map (
			clk => s_clk_main,
			ioclk => s_clk_main,
			rst => s_rst,
			exec_stb => s_exec_stb,
			insn => s_reg_din(INSN_BITS - 1 downto 0),
			tdo => s_tap_tdo,
			rtck => s_tap_rtck,
			ckgen_rtcken => s_ckgen_rtck_en,
			ckgen_div => s_ckgen_div(CKGEN_DIV_BITS - 1 downto 0),
			ckgen_rtdiv => s_ckgen_rtdiv(CKGEN_DIV_BITS - 1 downto 0),
			tck => s_tap_tck,
			tdi => s_tap_tdi,
			tms => s_tap_tms,
			rtcktmo => s_irq_rtcktmo,

			rx_mem_addr => s_rx_mem_addr,
			rx_mem_data => s_rx_mem_din,
			rx_mem_wr => s_rx_mem_wr_stb,

			tx_mem_addr => s_tx_mem_addr,
			tx_mem_data => s_tx_mem_dout,
--			tx_mem_rd => s_tx_mem_rd_stb,

			desc_addr => s_desc_addr,
			desc_data => s_desc_data,

			ptr_addr => s_ptr_addr,
			ptr_data => s_ptr_data,

--			desc_rd_stb => s_desc_rd_stb,

--			exec_ack => s_exec_ack,
--			end_stb => s_exec_end_stb,
			tap_state => s_tap_state,
			busy => s_tap_busy
		);
		s_irq_tap <= not s_tap_busy;
	---------------------------------------------------------------------------

	s_status(15 downto 4) <= (others => '0');
	s_status(3 downto 0) <= std_logic_vector(s_tap_state);

	---------------------------------------------------------------------------
	-- 1MHz clock generator
	cklgen_1mhz : entity jtag_clk_scaler
		generic map (CLK_DIV => 60)
		port map (
			clk => s_clk_main,
			rst => s_rst, 
--			q => s_clk_1mhz,
			p_stb => s_1mhz_stb
		);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- 1KHz clock generator
	cklgen_1khz : entity jtag_clk_scaler
		generic map (CLK_DIV => 1000)
		port map (
			clk => s_clk_main,
			rst => s_rst, 
			en => s_1mhz_stb,
--			q => s_clk_1khz,
			p_stb => s_1khz_stb
		);
	s_irq_1khz <= s_1khz_stb;
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- 2Hz clock generator
	cklgen_2hz : entity jtag_clk_scaler
		generic map (CLK_DIV => 500)
		port map (
			clk => s_clk_main,
			rst => s_rst, 
			en => s_1khz_stb
--			q => s_clk_2hz,
--			p_stb => s_2hz_stb
		);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- LEDs assignment
	led_run_drv : entity jtag_led_drv
		generic map (PULSE_CNT => 31, OUT_INV => false)
		port map (
			clk=> s_clk_main, 
			rst => s_rst, 
			en => s_1khz_stb, 
			trip => s_reg_wr_stb, 
			q => led_1
		);

	-- Auxiliar port activity
	led_con_drv : entity jtag_led_drv
		generic map (PULSE_CNT => 31, OUT_INV => false)
		port map (
			clk=> s_clk_main, 
			rst => s_rst, 
			en => s_1khz_stb, 
			trip => s_mem_wr_stb, 
			q => led_2
		);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- DBG Req/Ack
	tp_dbgrq <= s_dbgrq_out;
	s_dbgack_in <= tp_dbgack;
	---------------------------------------------------------------------------

	s_uart_en <= s_cfg(CFG_UART_EN);

	---------------------------------------------------------------------------
	-- Serial port TX
	s_uart_tx <= uart_tx;
	s_dbgrq_out <= s_uart_tx when s_uart_en = '1' else '0';
--	s_dbgrq_out <= s_irq_out;
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- Serial port RX
	s_uart_rx <= s_dbgack_in when s_uart_en = '1' else '1';
	uart_rx <= s_uart_rx;
	---------------------------------------------------------------------------

	-- Interrupt
	irq <= s_irq_out;
	-- TAP
	s_tap_rtck <= tp_rtck; 
	s_tap_tdo <= s_tap_tdi when s_loopback_en = '1' else tp_tdo;
	tp_tdi <= s_tap_tdi when s_loopback_en = '0' else '0';
--	tp_tdi <= s_tap_tdi;
	tp_tms <= s_tap_tms;
	tp_tck <= s_tap_tck;
	tp_trst <= s_tap_trst;
--	tp_trst <= s_rx_mem_wr_stb;
	tp_rst <= s_tap_nrst;
	
--	tap_state <= s_tap_state;

end structure;

