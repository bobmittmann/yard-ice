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
	constant MEM_CNT : natural := 2;
	constant MEM_SEL_BITS : natural := 2;
	constant MEM_ADDR_BITS : natural := 11;
	constant REG_CNT : natural := 8;
	constant REG_SEL_BITS : natural := 3;

	-- register select 
	constant REG_SEL_IST : std_logic_vector := "000";
	constant REG_SEL_IEN : std_logic_vector := "001";
	constant REG_SEL_CNT : std_logic_vector := "010";
	constant REG_SEL_DWN : std_logic_vector := "011";
	constant REG_SEL_SRC : std_logic_vector := "100";
	constant REG_SEL_DST : std_logic_vector := "101";
	constant REG_SEL_LEN : std_logic_vector := "110";
	constant REG_SEL_CTL : std_logic_vector := "111";
	-- interrupts
	constant IRQ_BITS : natural := 2;
	constant IRQ_MEMCPY : natural := 0;
	constant IRQ_1KHZ : natural := 1;

	-- clocks 
	signal s_clk_main : std_logic;
	signal s_clk_io: std_logic;
--	signal s_clk_1mhz : std_logic;
	-- clock strobes
	signal s_1mhz_stb : std_logic;
	signal s_1khz_stb : std_logic;
--	signal s_2hz_stb : std_logic;

	signal s_mem_addr : std_logic_vector(MEM_ADDR_BITS - 1 downto 0);

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

	-- interrupt requests
	signal s_irq : std_logic_vector(IRQ_BITS - 1 downto 0);
	signal s_irq_reg : std_logic_vector(IRQ_BITS - 1 downto 0);
	signal s_irq_set : std_logic_vector(IRQ_BITS - 1 downto 0);
	signal s_irq_out : std_logic;

	signal s_irq_memcpy : std_logic;
	signal s_irq_1khz : std_logic;
	-----------------------

	-- memcpy registers
	signal s_src_r : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_src_wr : std_logic;
	signal s_dst_r : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_dst_wr : std_logic;
	signal s_len_r : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_len_wr : std_logic;
	signal s_ctl_r : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_ctl_wr : std_logic;
	-- counter registers
	signal s_cnt_r : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_cnt_wr : std_logic;
	signal s_dwn_r : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_dwn_wr : std_logic;
	-- interrupt status register
	signal s_ist_r : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_ist_wr : std_logic;
	-- interrupt enable register
	signal s_ien_r : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_ien_wr : std_logic;


	-- Register access
	signal s_reg_wr_sel : std_logic_vector(REG_SEL_BITS - 1 downto 0);
	signal s_reg_wr_stb : std_logic;
	signal s_reg_rd_sel : std_logic_vector(REG_SEL_BITS - 1 downto 0);
	signal s_reg_din : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_reg_dout : std_logic_vector(DATA_WIDTH - 1 downto 0);

	-- Memory access
	signal s_mem1_wr : std_logic;

	signal s_mem_wr_stb : std_logic;
	signal s_mem_wr_sel : std_logic_vector(MEM_SEL_BITS - 1 downto 0);
	signal s_mem_rd_sel : std_logic_vector(MEM_SEL_BITS - 1 downto 0);

	signal s_mem1_dout : std_logic_vector(DATA_WIDTH - 1 downto 0);

	signal s_mem2_wr : std_logic;
	signal s_mem2_dout : std_logic_vector(DATA_WIDTH - 1 downto 0);

	signal s_mem_dout : std_logic_vector(DATA_WIDTH - 1 downto 0);
	signal s_mem_din : std_logic_vector(DATA_WIDTH - 1 downto 0);


	signal s_cpy_saddr : std_logic_vector(7 downto 0);
	signal s_cpy_sdata : std_logic_vector(DATA_WIDTH -1 downto 0);
	signal s_cpy_daddr : std_logic_vector(7 downto 0);
	signal s_cpy_ddata : std_logic_vector(DATA_WIDTH -1 downto 0);
	signal s_cpy_wr : std_logic;


	-- LEDs
	signal s_led1 : std_logic;
	signal s_led2 : std_logic;
	-----------------------

	-- global reset
	signal s_rst : std_logic;
	-- TAP output selection: NORMAL/LOOPBACK
	signal s_loopback_en : std_logic;

	function is_zero(x : std_logic_vector) return std_logic is
		variable y : std_logic := '0';
	begin
		for i in x'range loop
			y := y or x(i);
		end loop; 
		return not y;
	end is_zero;

begin
	---------------------------------------------------------------------------
	-- main clock
	s_clk_main <= mclk;
	s_clk_io <= fsmc_clk;

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

	s_irq_1khz <= s_1khz_stb;

	---------------------------------------------------------------------------
	-- 2Hz clock generator
	cklgen_2hz : entity clk_scaler
		generic map (CLK_DIV => 500)
		port map (
			clk => s_clk_main,
			rst => s_rst, 
			en => s_1khz_stb
--			p_stb => s_2hz_stb
		);
	---------------------------------------------------------------------------

	s_mem_dout <= s_mem1_dout when s_mem_rd_sel = "00" else s_mem2_dout;

	s_mem1_wr <= s_mem_wr_stb when s_mem_wr_sel = "00" else '0';
	s_mem2_wr <= s_mem_wr_stb when s_mem_wr_sel = "10" else '0';

	---------------------------------------------------------------------------
	-- Auxiliary memory Rd/Wr
	mem1 : entity sram
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			ADDR_WIDTH => MEM_ADDR_BITS
		)
		port map (
			clk => s_clk_io,
			addr => s_mem_addr,
			we => s_mem1_wr, 
			data => s_mem_din,
			q => s_mem1_dout
		);
	---------------------------------------------------------------------------


	---------------------------------------------------------------------------
	-- Write only memory
	mem2wr : entity dpram
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			ADDR_WIDTH => 8
		)
		port map (
			clk1 => s_clk_io,
			addr1 => s_mem_addr(7 downto 0),
			we1 => s_mem2_wr, 
			data1 => s_mem_din,

			clk2 => s_clk_main,
			addr2 => s_cpy_saddr(7 downto 0),
			q2 => s_cpy_sdata
		);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- Read only memory
	mem2rd : entity dpram
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			ADDR_WIDTH => 8
		)
		port map (
			clk1 => s_clk_main,
			addr1 => s_cpy_daddr(7 downto 0),
			we1 => s_cpy_wr, 
			data1 => s_cpy_ddata,

			clk2 => s_clk_io,
			addr2 => s_mem_addr(7 downto 0),
			q2 => s_mem2_dout
		);
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------

	s_ist_wr <= s_reg_wr_stb when s_reg_wr_sel = REG_SEL_IST else '0';
	s_ien_wr <= s_reg_wr_stb when s_reg_wr_sel = REG_SEL_IEN else '0';
	s_cnt_wr <= s_reg_wr_stb when s_reg_wr_sel = REG_SEL_CNT else '0';
	s_dwn_wr <= s_reg_wr_stb when s_reg_wr_sel = REG_SEL_DWN else '0';
	s_src_wr <= s_reg_wr_stb when s_reg_wr_sel = REG_SEL_SRC else '0';
	s_dst_wr <= s_reg_wr_stb when s_reg_wr_sel = REG_SEL_DST else '0';
	s_len_wr <= s_reg_wr_stb when s_reg_wr_sel = REG_SEL_LEN else '0';
	s_ctl_wr <= s_reg_wr_stb when s_reg_wr_sel = REG_SEL_CTL else '0';

	---------------------------------------------------------------------------

	src_r : entity reg
		generic map (DATA_WIDTH => DATA_WIDTH, 
					 REG_BITS => 8) 
		port map (
			clk => s_clk_main,
			rst => s_rst,
			d => s_reg_din,
			ld => s_src_wr,
			q => s_src_r
			);

	dst_r : entity reg
		generic map (DATA_WIDTH => DATA_WIDTH, 
					 REG_BITS => 8) 
		port map (
			clk => s_clk_main,
			rst => s_rst,
			d => s_reg_din,
			ld => s_dst_wr,
			q => s_dst_r
			);

	len_r : entity reg
		generic map (DATA_WIDTH => DATA_WIDTH, 
					 REG_BITS => 8) 
		port map (
			clk => s_clk_main,
			rst => s_rst,
			d => s_reg_din,
			ld => s_len_wr,
			q => s_len_r
			);

	ctl_r : entity reg
		generic map (DATA_WIDTH => DATA_WIDTH, 
					 REG_BITS => DATA_WIDTH) 
		port map (
			clk => s_clk_main,
			rst => s_rst,
			d => s_reg_din,
			ld => s_ctl_wr,
			q => s_ctl_r
			);

	cnt_r : entity counter
		generic map (DATA_WIDTH => DATA_WIDTH, 
					 COUNT_BITS => DATA_WIDTH) 
		port map (
			clk => s_clk_main,
			rst => s_rst,
			d => s_reg_din,
			ld => s_cnt_wr,
			cin => s_1khz_stb,
			q => s_cnt_r
			);

	dwn_r : entity counter
		generic map (DATA_WIDTH => DATA_WIDTH, 
					 COUNT_BITS => DATA_WIDTH) 
		port map (
			clk => s_clk_main,
			rst => s_rst,
			d => s_reg_din,
			ld => s_dwn_wr,
			cin => s_1khz_stb,
			up => '0',
			q => s_dwn_r
			);

	ien_r : entity reg
		generic map (DATA_WIDTH => DATA_WIDTH, 
					 REG_BITS => IRQ_BITS) 
		port map (
			clk => s_clk_main,
			rst => s_rst,
			d => s_reg_din,
			ld => s_ien_wr,
			q => s_ien_r
			);

	ist_r : entity reg
		generic map (DATA_WIDTH => DATA_WIDTH, 
					 REG_BITS => IRQ_BITS) 
		port map (
			clk => s_clk_main,
			rst => s_rst,
			-- register set
			-- set individual bits
			set => '1',
			d_set => s_irq_set,
			clr => s_ist_wr,
			d_clr => s_reg_din(IRQ_BITS - 1 downto 0),
			q => s_ist_r
			);


	with s_reg_rd_sel(REG_SEL_BITS - 1 downto 0) select
		s_reg_dout <= 
		s_ist_r when REG_SEL_IST,
		s_ien_r when REG_SEL_IEN,
		s_cnt_r when REG_SEL_CNT,
		s_dwn_r when REG_SEL_DWN,
		s_src_r when REG_SEL_SRC, 
		s_dst_r when REG_SEL_DST, 
		s_len_r when REG_SEL_LEN,
		s_ctl_r when REG_SEL_CTL,
		(others => '0') when others; 

	---------------------------------------------------------------------------
	-- Memory copy engine
	mem2cpy : entity memcpy
		generic map (
			DATA_WIDTH => DATA_WIDTH, 
			ADDR_BITS => 8
		)
		port map (
			clk => s_clk_main,
			src => s_src_r(7 downto 0), 
			dst => s_dst_r(7 downto 0), 
			len => s_len_r(7 downto 0), 

			start => s_ctl_wr,

			saddr => s_cpy_saddr,
			sdata => s_cpy_sdata,

			daddr => s_cpy_daddr,
			ddata => s_cpy_ddata,
			wr => s_cpy_wr,
			done => s_irq_memcpy
		);
	---------------------------------------------------------------------------


	---------------------------------------------------------------------------
	-- interrupts assignments
	s_irq(IRQ_MEMCPY) <= s_irq_memcpy;
	s_irq(IRQ_1KHZ) <= s_irq_1khz;
	s_irq_out <= not is_zero(s_ien_r and s_ist_r);

	---------------------------------------------------------------------------
	-- interrupt request synchronous rising edge detect
	process(s_clk_main)
	begin
		if rising_edge(s_clk_main) then
			s_irq_reg <= s_irq;
		end if;
	end process;
	s_irq_set <= s_irq and not s_irq_reg;
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- LED 1
	led_1_drv : entity led_drv
		generic map (PULSE_CNT => 31, OUT_INV => false)
		port map (
			clk=> s_clk_main, 
			rst => s_rst, 
			en => s_1khz_stb, 
			trip => s_reg_wr_stb, 
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
--			trip => s_bus_rd, 
			trip => s_mem_wr_stb, 
			q => s_led2
		);

	---------------------------------------------------------------------------
	s_loopback_en <= '0';
	s_rst <= '0';

	---------------------------------------------------------------------------
	s_tap_trst <= s_mem_wr_stb;
--	s_tap_tdi <= fsmc_noe;
	s_tap_tdi <= s_reg_rd_sel(2);
	s_tap_tms <= s_reg_rd_sel(1); --s_bus_din(2);
--	s_tap_tdi <= '1' when (s_memc_st = MEMC_DLAT1) else '0';
--	s_tap_tms <= '1' when (s_memc_st = MEMC_DSTB1) else '0';
	s_tap_tck <= s_mem_wr_stb;
	s_tap_nrst <= s_reg_rd_sel(0);
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

