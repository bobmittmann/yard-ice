-- File:	fsm-test.vhdl
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

entity fsmc_test is
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
	-- auxiliar clock 
	clk_aux : in std_logic
	
);
end fsmc_test;

architecture structure of fsmc_test is
	constant DATA_WIDTH : natural := 16;
	constant MEM_BITS : natural := 11;

	-- clocks 
	signal s_clk_main : std_logic;
--	signal s_clk_1mhz : std_logic;
	-- clock strobes
	signal s_1mhz_stb : std_logic;
	signal s_1khz_stb : std_logic;
--	signal s_2hz_stb : std_logic;

	-- bus access 
	signal s_bus_dout : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_bus_din : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_bus_addr : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_bus_rd : std_logic;
	signal s_bus_wr : std_logic;
	-----------------------

	-- TAP signals
--	signal s_tap_tdo : std_logic;
--	signal s_tap_rtck : std_logic;
	signal s_tap_tck : std_logic;
	signal s_tap_tdi : std_logic;
	signal s_tap_tms : std_logic;
	signal s_tap_trst : std_logic;
	signal s_tap_nrst : std_logic;
	-- TAP extra 
	signal s_dbgack : std_logic;
	signal s_dbgrq : std_logic;
	-----------------------

	-- serial port 
	signal s_uart_rx : std_logic;
	signal s_uart_tx : std_logic;
	-----------------------

	-- IRQ
	signal s_irq_out : std_logic;
	-----------------------

	-- LEDs
	signal s_led1 : std_logic;
	signal s_led2 : std_logic;
	-----------------------

	-- global reset
	signal s_rst : std_logic;
	-- TAP output selection: NORMAL/LOOPBACK
	signal s_loopback_en : std_logic;

begin
	---------------------------------------------------------------------------
	-- main clock
	s_clk_main <= mclk;

	---------------------------------------------------------------------------
	-- CRAM bus adapter
	cram_bus : entity cram16
		generic map (ADDR_BITS => DATA_WIDTH)
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
	-- 1MHz clock generator
	cklgen_1mhz : entity clk_scaler
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
	cklgen_1khz : entity clk_scaler
		generic map (CLK_DIV => 1000)
		port map (
			clk => s_clk_main,
			rst => s_rst, 
			en => s_1mhz_stb,
--			q => s_clk_1khz,
			p_stb => s_1khz_stb
		);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- 2Hz clock generator
	cklgen_2hz : entity clk_scaler
		generic map (CLK_DIV => 500)
		port map (
			clk => s_clk_main,
			rst => s_rst, 
			en => s_1khz_stb
--			q => s_led2,
--			p_stb => s_2hz_stb
		);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- memory
	mem : entity sram
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			ADDR_WIDTH => MEM_BITS
		)
		port map (
			clk => s_clk_main,
			addr => s_bus_addr(MEM_BITS - 1 downto 0),
			we => s_bus_wr, 
			data => s_bus_din,
			q => s_bus_dout
		);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- LED 1
	led_1_drv : entity led_drv
		generic map (PULSE_CNT => 31, OUT_INV => false)
		port map (
			clk=> s_clk_main, 
			rst => s_rst, 
			en => s_1khz_stb, 
			trip => s_bus_wr, 
			q => s_led1
		);

	---------------------------------------------------------------------------
	-- LED 2
	led_2_drv : entity led_drv
		generic map (PULSE_CNT => 31, OUT_INV => false)
		port map (
			clk=> s_clk_main, 
			rst => s_rst, 
			en => s_1khz_stb, 
			trip => s_bus_rd, 
			q => s_led2
		);

	---------------------------------------------------------------------------
	s_loopback_en <= '0';
	s_irq_out <= '0';
	s_rst <= '0';

	---------------------------------------------------------------------------
	s_tap_trst <= s_bus_wr;
--	s_tap_tdi <= fsmc_noe;
	s_tap_tdi <= s_bus_din(1);
	s_tap_tms <= s_bus_din(2);
--	s_tap_tdi <= '1' when (s_memc_st = MEMC_DLAT1) else '0';
--	s_tap_tms <= '1' when (s_memc_st = MEMC_DSTB1) else '0';
	s_tap_tck <= fsmc_clk;
	s_tap_nrst <= s_bus_rd;
--	s_dbgrq <= s_bus_dout(2);

	---------------------------------------------------------------------------

	s_dbgrq <= s_uart_tx;
	s_uart_rx <= s_dbgack;

	---------------------------------------------------------------------------
	-- Output Assignments
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- Interrupt
	irq <= s_irq_out;

	---------------------------------------------------------------------------
	-- TAP
--	s_tap_rtck <= tp_rtck; 
--	s_tap_tdo <= s_tap_tdi when s_loopback_en = '1' else tp_tdo;
	tp_tdi <= s_tap_tdi when s_loopback_en = '0' else '0';
	tp_tms <= s_tap_tms;
	tp_tck <= s_tap_tck;
	tp_trst <= s_tap_trst;
	tp_rst <= s_tap_nrst;

	---------------------------------------------------------------------------
	-- LEDs
	led_1 <= s_led1;
	led_2 <= s_led2;

	---------------------------------------------------------------------------
	-- Aux pins DBGRQ/DBGACK
	tp_dbgrq <= s_dbgrq;
	s_dbgack <= tp_dbgack;

	---------------------------------------------------------------------------
	-- Serial port TX/RX
	s_uart_tx <= uart_tx;
	uart_rx <= s_uart_rx;

end structure;

