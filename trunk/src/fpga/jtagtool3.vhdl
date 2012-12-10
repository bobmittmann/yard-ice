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
	tp_dbgrq : inout std_logic;
	tp_dbgack : inout std_logic;
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
	constant CFG_UART_EN : natural := 10;
	constant CFG_TAP_TRST : natural := 10;
	constant CFG_TAP_NRST : natural := 11;
	constant CFG_REG_BITS : natural := 12; -- width of cfg reg

	-- registers 
	constant REG_INSN : unsigned := to_unsigned(0, REG_SEL_BITS);
	constant REG_STATUS : unsigned := to_unsigned(0, REG_SEL_BITS);
	constant REG_INT_ST : unsigned := to_unsigned(1, REG_SEL_BITS);
	constant REG_INT_EN : unsigned := to_unsigned(2, REG_SEL_BITS);
	constant REG_CKGEN_DIV : unsigned := to_unsigned(4, REG_SEL_BITS);
	constant REG_CKGEN_RTDIV : unsigned := to_unsigned(5, REG_SEL_BITS);
	constant REG_CFG : unsigned := to_unsigned(6, REG_SEL_BITS);
	constant REG_TMR : unsigned := to_unsigned(7, REG_SEL_BITS);
	-- interrupts
	constant IRQ_TAP : natural := 0;
	constant IRQ_RTCKTMO : natural := 1;
	constant IRQ_1KHZ : natural := 2;
	constant IRQ_BITS : natural := 3;

	-- clocks 
	signal s_clk_main : std_logic;
	signal s_clk_io : std_logic;
	signal s_clk_120mhz : std_logic;
--	signal s_clk_1mhz : std_logic;
--	signal s_clk_1khz : std_logic;
--	signal s_clk_2hz : std_logic;
	-- clock strobes
	signal s_1mhz_stb : std_logic;
	signal s_1khz_stb : std_logic;
	signal s_2hz_stb : std_logic;

	-- internal bus
	signal s_bus_din : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_bus_dout : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_bus_addr : std_logic_vector(ADDR_BITS - 1 downto 1);
	signal s_bus_wr : std_logic;
	signal s_bus_rd : std_logic;

	-- register selection
	signal s_reg_wr_stb : std_logic;
	signal s_reg_rd_stb : std_logic;
	signal s_reg_out : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_reg_buf : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_reg_sel : unsigned(REG_SEL_BITS - 1 downto 0);

	-- vector descriptor table memory access
	-- descriptor address
	signal s_desc_addr : std_logic_vector(DESC_ADDR_BITS - 1 downto 0);
	signal s_desc_data : std_logic_vector(DESC_DATA_BITS -1 downto 0);
	signal s_desc_mem_wr_stb : std_logic;
	signal s_desc_mem_dout : std_logic_vector((DATA_WIDTH - 1) downto 0);

	-- vector descriptor pointer memory access
	signal s_ptr_addr : std_logic_vector(PTR_ADDR_BITS - 1 downto 0);
	signal s_ptr_data : std_logic_vector(PTR_DATA_BITS - 1 downto 0);
	signal s_ptr_mem_wr_stb : std_logic;
	signal s_ptr_mem_dout : std_logic_vector((DATA_WIDTH - 1) downto 0);

	---------------------------------------------------------------------------
	-- Registers  
	---------------------------------------------------------------------------

	-- instruction register
--	signal s_insn : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_status : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_insn_wr_stb : std_logic;
	-- TAP state
	signal s_tap_state : unsigned(3 downto 0);

	-- interrupt status (R/O)
	signal s_irq_st : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_irq_st_rd_stb : std_logic;
	-- interrupt enable (W/O)
	signal s_irq_en : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_irq_en_wr_stb : std_logic;
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
	signal s_tmr_rd_stb : std_logic; 
	-- interrupt requests
	signal s_irq : std_logic_vector(IRQ_BITS - 1 downto 0);
	signal s_irq_reg : std_logic_vector(IRQ_BITS - 1 downto 0);
	signal s_irq_set : std_logic_vector(IRQ_BITS - 1 downto 0);
	signal s_irq_out : std_logic;

	signal s_irq_tap : std_logic;
	signal s_irq_1khz : std_logic;
	signal s_irq_rtcktmo : std_logic;

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
	signal s_tx_mem_din : std_logic_vector((DATA_WIDTH - 1) downto 0);
	signal s_tx_mem_dout : std_logic_vector((DATA_WIDTH - 1) downto 0);
	-- TX memory read strobe
--	signal s_tx_mem_rd_stb : std_logic;
	signal s_tx_mem_wr_stb : std_logic;

	-- execution start acknowlegment
--	signal s_exec_ack : std_logic;
	-- execution end strobe
	signal s_exec_end : std_logic;

	
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

begin
	-- main clock (60MHz)
	s_clk_main <= mclk;

--	pll : cyc_pll port map(
--		inclk0  => s_clk_main,
--		c1 => s_clk_120mhz
--	);
           
	-- IO clock
--	s_clk_io <= s_clk_120mhz;
	s_clk_io <= s_clk_main;

	---------------------------------------------------------------------------
	-- CRAM bus adapter
	cram_bus : entity jtag_cram16
		generic map (ADDR_BITS => ADDR_BITS - 1) 
		port map (
			clk => s_clk_main,
			rst => s_rst,

			dout => s_bus_dout,
			din  => s_bus_din,
			addr  => s_bus_addr,
			rd  => s_bus_rd,
			wr => s_bus_wr,

			cram_clk => fsmc_clk,
			cram_noe => fsmc_noe,
			cram_nwe => fsmc_nwe,
			cram_nce => fsmc_nce,
			cram_d => fsmc_d
		);

	---------------------------------------------------------------------------
	-- address range selection
	-- input selection decoder  (write)
	s_reg_rd_stb <= s_bus_rd when 
		(s_bus_addr(12) = '1' and s_bus_addr(11) = '1') else '0';
	s_reg_wr_stb <= s_bus_wr when 
		(s_bus_addr(12) = '1' and s_bus_addr(11) = '1') else '0';
	s_ptr_mem_wr_stb <= s_bus_wr when 
		(s_bus_addr(12) = '1' and s_bus_addr(11) = '0' and s_bus_addr(10) = '1') else '0';
	s_desc_mem_wr_stb <= s_bus_wr when 
		(s_bus_addr(12) = '1' and s_bus_addr(11) = '0' and s_bus_addr(10) = '0') else '0';
	s_tx_mem_wr_stb <= s_bus_wr when s_bus_addr(12) = '0' else '0';
	---------------------------------------------------------------------------
	-- output selection multiplexer (read)
	s_bus_dout <= 
		s_rx_mem_dout when (s_bus_addr(12) = '0') else
		s_reg_buf when (s_bus_addr(12) = '1' and s_bus_addr(11) = '1') else 
		s_desc_mem_dout;

	-- Memory map
	--
	-- +---------+---------------+---------------+
	-- | Address | Read          | Write         |
	-- +---------+---------------+---------------+ 
	-- |  0x1fff |               |               |
	-- |   ...   |  Invalid      | Invalid       |
	-- |         |               |               |
	-- |  0x1810 |               |               |
	-- +---------+---------------+---------------+ 
	-- |  0x180f |               |               |
	-- |   ...   |  Registers    |  Control      |
	-- |         |               |  Registers    |
	-- |  0x1800 |               |               |
	-- +---------+---------------+---------------+ 
	-- |  0x17ff |               |               |
	-- |   ...   |  Invalid      | Invalid       |
	-- |         |               |               |
	-- |  0x1600 |               |               |
	-- +---------+---------------+---------------+
	-- |  0x15ff |               |               |
	-- |   ...   |   Invalid     | Descriptor    |
	-- |         |               | Pointers      |
	-- |  0x1400 |               |               |
	-- +---------+---------------+---------------+
	-- |  0x13ff |               |               |
	-- |   ...   |   Invalid     | Vector        |
	-- |         |               | Descriptors   |
	-- |  0x1000 |               |               |
	-- +---------+---------------+---------------+
	-- |  0x0fff |               |               |
	-- |   ...   |  Vector TX    | Vector RX     |
	-- |         |  Memory       | Memory        |
	-- |  0x0000 |               |               |
	-- +---------+---------------+---------------+
	--
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- output buffer (read)
	process (s_clk_main, s_reg_rd_stb)
	begin
		if rising_edge(s_clk_main) and (s_reg_rd_stb = '1') then
			s_reg_buf <= s_reg_out;
		end if;
	end process;

	---------------------------------------------------------------------------
	-- register selection
	s_reg_sel <= unsigned(s_bus_addr(REG_SEL_BITS downto 1));
	-- register output multiplexer
	with s_reg_sel select 
		s_reg_out <= s_status when REG_STATUS, 
		s_irq_st when REG_INT_ST,
		s_irq_en when REG_INT_EN,
		s_ckgen_div when REG_CKGEN_DIV,
		s_ckgen_rtdiv when REG_CKGEN_RTDIV,
		s_cfg when REG_CFG,
		s_tmr when REG_TMR,
		(others => '0') when others;
	-- register write enable decoder 
	s_insn_wr_stb <= s_reg_wr_stb when (s_reg_sel = REG_INSN) else '0';
--	s_irq_st_wr_stb <= s_reg_wr_stb when (s_reg_sel = REG_INT_ST) else '0';
	s_irq_en_wr_stb <= s_reg_wr_stb when (s_reg_sel = REG_INT_EN) else '0';
	s_ckgen_div_wr_stb <= s_reg_wr_stb when (s_reg_sel = REG_CKGEN_DIV) 
										else '0';
	s_ckgen_rtdiv_wr_stb <= s_reg_wr_stb when (s_reg_sel = REG_CKGEN_RTDIV)
										else '0';
	s_cfg_wr_stb <= s_reg_wr_stb when (s_reg_sel = REG_CFG) else '0';
	s_tmr_wr_stb <= s_reg_wr_stb when (s_reg_sel = REG_TMR) else '0';
	-- register read decoder 
	s_irq_st_rd_stb <= s_reg_rd_stb when (s_reg_sel = REG_INT_ST) else '0';
	s_tmr_rd_stb <= s_reg_rd_stb when (s_reg_sel = REG_TMR) else '0';
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
	s_irq_out <= not is_zero(s_irq_en and s_irq_st);
--	s_irq_out <= '0' when (s_irq_en and s_irq_st) = (others => '0') else '1';
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- interrupt request synchronous edge detect
	process(s_clk_main)
	begin
		if rising_edge(s_clk_main) then
			s_irq_reg <= s_irq;
		end if;
	end process;
	s_irq_set <= s_irq and not s_irq_reg;
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
			-- read signal 
			-- clear all
			clr => s_irq_st_rd_stb,
			-- register set
			-- set individual bits
			set => '1',
			d_set => s_irq_set,
			-- data out
			q => s_irq_st);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- interrupt enable register
	int_en_r : entity jtag_reg
		generic map (DATA_WIDTH => DATA_WIDTH, REG_BITS => IRQ_BITS)
		port map (
			-- system clock
			clk => s_clk_main,
			-- read signal 
			ld => s_irq_en_wr_stb,
			-- data in
			d => s_bus_din,
			-- data out
			q => s_irq_en);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- configuration register
	cfg_r : entity jtag_reg
		generic map (DATA_WIDTH => DATA_WIDTH, REG_BITS => CFG_REG_BITS) 
		port map (
			-- system clock
			clk => s_clk_main,
			-- write signal 
			ld => s_cfg_wr_stb,
			-- data in
			d => s_bus_din,
			-- data out
			q => s_cfg,
			-- reset
			clr => s_rst);

	s_rst <= s_cfg(CFG_RST); -- Global reset
	s_ckgen_rtck_en <= s_cfg(CFG_RTCK_EN);
	s_loopback_en <= s_cfg(CFG_LOOP);
	s_tap_trst <= '1' when s_cfg(CFG_TAP_TRST) = '1' else '0';
	s_tap_nrst <= '0' when s_cfg(CFG_TAP_NRST) = '1' else 'Z';
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- timer register
	tmr_r : entity jtag_counter
		generic map (DATA_WIDTH => DATA_WIDTH, COUNT_BITS => DATA_WIDTH) 
		port map (
			-- system clock
			clk => s_clk_main,
			-- reset
			rst => s_rst,
			-- load on write signal 
			ld => s_tmr_wr_stb,
			d => s_bus_din,
			-- clear on reading
			clr => s_tmr_rd_stb,
			-- count 
			cin => s_irq_set(IRQ_1KHZ),
			-- data out
			q => s_tmr);
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
			d => s_bus_din,
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
			d => s_bus_din,
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
			clk => s_clk_main,
			desc_addr => s_desc_addr,
			io_addr => s_bus_addr(DESC_ADDR_BITS + 1 downto 1),
			io_we => s_desc_mem_wr_stb, 
			io_data => s_bus_din,
			desc_q => s_desc_data,
			io_q => s_desc_mem_dout
		);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- descriptor pointers memory
	ptr_mem : entity jtag_io_dpram
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			ADDR_WIDTH => PTR_ADDR_BITS
		)
		port map (
			clk => s_clk_main,
			rd_addr => s_ptr_addr,
			wr_addr => s_bus_addr(PTR_ADDR_BITS downto 1),
			we => s_ptr_mem_wr_stb, 
			data => s_bus_din,
			q => s_ptr_data
		);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- rx vector memory
	rx_mem : entity jtag_io_dpram
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			ADDR_WIDTH => VEC_ADDR_BITS
		)
		port map (
			clk => s_clk_main,
			rd_addr => s_bus_addr(VEC_ADDR_BITS downto 1),
			wr_addr => s_rx_mem_addr,
			we => s_rx_mem_wr_stb, 
			data => s_rx_mem_din,
			q => s_rx_mem_dout
		);
	---------------------------------------------------------------------------

	s_tx_mem_din <= s_bus_din;

	---------------------------------------------------------------------------
	-- tx vector memory
	tx_mem : entity jtag_io_dpram
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			ADDR_WIDTH => VEC_ADDR_BITS
		)
		port map (
			clk => s_clk_main,
			rd_addr => s_tx_mem_addr,
			wr_addr => s_bus_addr(VEC_ADDR_BITS downto 1),
			we => s_tx_mem_wr_stb, 
			data => s_tx_mem_din,
			q => s_tx_mem_dout
		);
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
			ioclk => s_clk_io,
			rst => s_rst,
			exec_stb => s_exec_stb,
			insn => s_bus_din(INSN_BITS - 1 downto 0),
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
			exec_end => s_exec_end,
			tap_state => s_tap_state 
		);
		s_irq_tap <= s_exec_end;
	---------------------------------------------------------------------------

	s_status(15 downto 4) <= (others => '0');
	s_status(3 downto 0) <= std_logic_vector(s_tap_state);

	---------------------------------------------------------------------------
	-- 1MHz clock generator
	cklgen_1mhz : entity jtag_clk_scaler
		generic map (CLK_DIV => 75)
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
		generic map (PULSE_CNT => 31, OUT_INV => true)
		port map (
			clk=> s_clk_main, 
			rst => s_rst, 
			en => s_1khz_stb, 
			trip => s_bus_rd, 
			q => led_1
		);

	-- Auxiliar port activity
	led_con_drv : entity jtag_led_drv
		generic map (PULSE_CNT => 31, OUT_INV => true)
		port map (
			clk=> s_clk_main, 
			rst => s_rst, 
			en => s_1khz_stb, 
			trip => not(s_dbgack_in), 
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
	tp_rst <= s_tap_nrst;
	
--	tap_state <= s_tap_state;

end structure;

