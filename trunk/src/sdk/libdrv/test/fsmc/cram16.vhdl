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
-- JTAG Register
--

library ieee;
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all;

library work;
use work.counter;

entity cram16 is
generic ( 
	ADDR_BITS : integer := 16
);
port (
	-- reset
	rst : in std_logic := '0';
	-- JTAG controller bus
	dout : in std_logic_vector(15 downto 0) := (others => '0');
	din : out std_logic_vector(15 downto 0);
	addr : out std_logic_vector(ADDR_BITS - 1 downto 0);
	wr : out std_logic;
	raddr : out std_logic_vector(ADDR_BITS - 1 downto 0);
	rd : out std_logic;
	-- CRAM bus
	cram_clk : in std_logic := '0';
	cram_noe : in std_logic := '0';
	cram_nwe : in std_logic := '0';
	cram_nce : in std_logic := '0';
	cram_d : inout std_logic_vector(15 downto 0)
);
end cram16;

architecture rtl of cram16 is
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
	-- read strobe
	signal s_rd_stb0 : std_logic;
	signal s_rd_stb1 : std_logic;
	signal s_rd_stb2 : std_logic;
	signal s_raddr0 : std_logic_vector(ADDR_BITS - 1 downto 0);
	signal s_raddr1 : std_logic_vector(ADDR_BITS - 1 downto 0);
	signal s_raddr2 : std_logic_vector(ADDR_BITS - 1 downto 0);
	-- address latch
	signal s_addr_r : std_logic_vector(ADDR_BITS - 1 downto 0);
	-----------------------

	-- output latch
	signal s_dout_r : std_logic_vector(15 downto 0);

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
		generic map (DATA_WIDTH => 16, COUNT_BITS => ADDR_BITS) 
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
			q(ADDR_BITS - 1 downto 0) => s_addr_r(ADDR_BITS - 1 downto 0)
			);

	---------------------------------------------------------------------------
	addr <= s_addr_r;

	---------------------------------------------------------------------------
	-- input
	din <= cram_d;


	---------------------------------------------------------------------------
	-- output

	s_cram_oe <= (cram_noe nor cram_nce);

	process (rst, cram_clk, s_cram_rd, s_cram_oe)
	begin
		if (rst = '1') or (s_cram_oe = '0') then
			s_rd_stb0 <= '0';
			s_rd_stb1 <= '0';
			s_rd_stb2 <= '0';
		elsif falling_edge(cram_clk) then
			s_rd_stb0 <= s_cram_rd;
			s_rd_stb1 <= s_rd_stb0;
			s_rd_stb2 <= s_rd_stb1;
		end if;
	end process;

	process (rst, cram_clk)
	begin
		if (rst = '1') then
			s_raddr0 <= (others => '0');
			s_raddr1 <= (others => '0');
			s_raddr2 <= (others => '0');
		elsif rising_edge(cram_clk) then
			s_raddr0 <= s_addr_r;
			s_raddr1 <= s_raddr0;
			s_raddr2 <= s_raddr1;
		end if;
	end process;

--	cram_d <= s_dout_r when (s_cram_oe  = '1') else (others => 'Z');
	cram_d <= dout when (s_cram_oe  = '1') else (others => 'Z');
	---------------------------------------------------------------------------
	rd <= s_rd_stb2;
	raddr <= s_raddr2;

	wr <= s_cram_wr;
	---------------------------------------------------------------------------

end rtl;

