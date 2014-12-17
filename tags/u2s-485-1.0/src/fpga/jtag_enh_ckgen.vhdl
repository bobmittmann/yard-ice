-- File:	jtag_enh_ckgen.vhdl
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
-- JTAG clock generator
-- 

library ieee;
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all;

entity jtag_enh_ckgen is
generic ( 
	-- number of bits for the divisor 
	DIV_BITS : integer := 16
);
port (
	-- JTAG clock
	clk : in std_logic;
	-- assynchronous reset
	rst : in std_logic := '0';	
	-- TCK enable
	en : in std_logic := '1';
	-- divisor
	div : in std_logic_vector(DIV_BITS - 1 downto 0) := (1=>'1', others=>'0');
	-- RTCK enabled divisor
	rtdiv : in std_logic_vector(DIV_BITS - 1 downto 0) := (others=>'1');
	-- RTCK (return clock)
	rtck : 	in std_logic := '0';
	-- Enable return clock operation
	rtcken : in std_logic := '0';
	-- TCK output
	tck : out std_logic;
	-- TCK positive edge pulse
	iotckp : out std_logic;
	-- TCK negative edge pulse
	iotckn : out std_logic;
	-- RTCK timeout
	rtcktmo : out std_logic
);
end jtag_enh_ckgen;

architecture rtl of jtag_enh_ckgen is
	signal s_div_lo_r0 : unsigned(DIV_BITS - 1 downto 0);
	signal s_div_hi_r0 : unsigned(DIV_BITS - 1 downto 0);
	signal s_rtdiv_lo_r0 : unsigned(DIV_BITS - 1 downto 0);
	signal s_rtdiv_hi_r0 : unsigned(DIV_BITS - 1 downto 0);
	signal s_div_lo : unsigned(DIV_BITS - 2 downto 0);
	signal s_div_hi : unsigned(DIV_BITS - 2 downto 0);
	signal s_rtdiv_lo : unsigned(DIV_BITS - 2 downto 0);
	signal s_rtdiv_hi : unsigned(DIV_BITS - 2 downto 0);

	signal s_cnt_lo_r : unsigned(DIV_BITS - 2 downto 0);
	signal s_cnt_hi_r : unsigned(DIV_BITS - 2 downto 0);
	signal s_cmp_hi : unsigned(DIV_BITS - 2 downto 0);
	signal s_cmp_lo : unsigned(DIV_BITS - 2 downto 0);
	signal s_tcki_r0 : std_logic;	
	signal s_tcki_r1 : std_logic;	
	signal s_enabled : std_logic;

	signal s_tcken_r0 : std_logic;
	signal s_tcken_r1 : std_logic;	
	signal s_tcken : std_logic;	
	signal s_tck_r0 : std_logic;
	signal s_tck_r1 : std_logic;
	signal s_tck_fall : std_logic;
	signal s_tck_rise : std_logic;

	signal s_rtcken_r0 : std_logic;
	signal s_rtcken_r1 : std_logic;	
	signal s_rtcken_reg : std_logic;
	signal s_rtck_r0 : std_logic;
	signal s_rtck_r1 : std_logic;
	signal s_rtck_r2 : std_logic;	
	signal s_rtck_fall : std_logic;
	signal s_rtck_rise : std_logic;
	signal s_rtck_tmo : std_logic;
begin 
	-- synchronize the operational parameters 
	-- which are (possibly) in another clock domain
	p_parm_sync: process (rst, clk)
	begin
		if (rst = '1') then
			s_div_hi_r0 <= to_unsigned(2, DIV_BITS);
			s_div_lo_r0 <= to_unsigned(2, DIV_BITS);
			s_rtdiv_hi_r0 <= to_unsigned(2, DIV_BITS);
			s_rtdiv_lo_r0 <= to_unsigned(2, DIV_BITS);
			s_div_hi <= to_unsigned(2, DIV_BITS - 1);
			s_div_lo <= to_unsigned(2, DIV_BITS - 1);
			s_rtdiv_hi <= to_unsigned(2, DIV_BITS - 1);
			s_rtdiv_lo <= to_unsigned(2, DIV_BITS - 1);
		elsif rising_edge(clk) then
			s_div_hi_r0 <= (unsigned(div) / 2);
			s_div_lo_r0 <= (unsigned(div) - s_div_hi);
			s_rtdiv_hi_r0 <= (unsigned(rtdiv) / 2);
			s_rtdiv_lo_r0 <= (unsigned(rtdiv) - s_rtdiv_hi);
			s_div_hi <= s_div_hi_r0(DIV_BITS - 2 downto 0);
			s_div_lo <= s_div_lo_r0(DIV_BITS - 2 downto 0);
			s_rtdiv_hi <= s_rtdiv_hi_r0(DIV_BITS - 2 downto 0);
			s_rtdiv_lo <= s_rtdiv_lo_r0(DIV_BITS - 2 downto 0);
		end if;
	end process p_parm_sync;

	-- synchronize the clock enable signals
	p_en_sync: process (rst, clk)
	begin
		if (rst = '1') then
			s_tcken_r0 <= '0';
			s_tcken_r1 <= '0';
			s_rtcken_r0 <= '0';
			s_rtcken_r1 <= '0';
		elsif rising_edge(clk) then
			s_tcken_r0 <= en;
			s_tcken_r1 <= s_tcken_r0;
			s_rtcken_r0 <= rtcken;
			s_rtcken_r1 <= s_rtcken_r0;
		end if;
	end process p_en_sync;
	-- TCK enable synchronized
	s_tcken <= s_tcken_r1;

	-- synchronize the RTCK signal
	p_rtck_sync: process (clk, rst, s_rtcken_reg, 
						  s_rtck_tmo, s_rtck_r0, s_rtck_r1)
	begin
		if (rst = '1') then
			s_rtck_r0 <= '0';
			s_rtck_r1 <= '0';
			s_rtck_r2 <= '0';
		elsif rising_edge(clk) then
			s_rtck_r0 <= rtck;
			if (s_rtcken_reg = '0') or (s_rtck_tmo = '1') then
				s_rtck_r1 <= '0';
				s_rtck_r2 <= '0';
			else		
				s_rtck_r1 <= s_rtck_r0;
				s_rtck_r2 <= s_rtck_r1;
			end if;
		end if;
	end process p_rtck_sync;

	p_count: process (clk, rst, s_rtcken_r1, s_enabled, s_tcken)
	begin
		if (rst = '1') then
			s_tck_r0 <= '0';
			s_tck_r1 <= '0';
			s_enabled <= '0';
			s_rtcken_reg <= '0';
			s_rtck_tmo <= '0';
			s_tcki_r0 <= '0';
			s_tcki_r1 <= '0';
			s_cnt_hi_r <= (others => '0');
			s_cnt_lo_r <= (others => '0');
			s_cmp_hi <= (others => '0');
			s_cmp_lo <= (others => '0');
		elsif rising_edge(clk) then
			if (s_enabled = '1') then
				-- TCK enabled
				-- sample the RTCK enable signal
				s_rtcken_reg <= s_rtcken_r1;
				if (s_tck_r0 = '0') then
					-- reload the high timer comparator value					
					if (s_rtcken_reg = '1') then
						-- load an upper limmit (RTCK timeout)
						s_cmp_hi <= s_rtdiv_hi;
					else													
						s_cmp_hi <= s_div_hi;
					end if;					

					if (s_cnt_lo_r = s_cmp_lo) or (s_rtck_fall = '1') then
						-- clear the counter
						s_cnt_lo_r <= (others => '0');
						-- change the s_tck_r0
						s_tck_r0 <= '1';
						s_tcki_r0 <= '0';
					else
						-- increment the lo counter
						s_cnt_lo_r <= s_cnt_lo_r + 1;
					end if;	
				else
					-- reload the low timer comparator value
					if (s_rtcken_reg = '1') then
						s_cmp_lo <= s_rtdiv_lo;
					else
						s_cmp_lo <= s_div_lo;
					end if;					
					if  (s_cnt_hi_r = s_cmp_hi) then
						-- clear the counter
						s_cnt_hi_r <= (others => '0');
						-- change the state
						s_tck_r0 <= '0';
						-- sample the enable signal
						s_enabled <= s_tcken;						
						s_tcki_r0 <= s_tcken;
						-- set the RTCK timout flag if RTCK is enabled
						s_rtck_tmo <= s_rtcken_reg;
					elsif (s_rtck_rise = '1') then
						-- clear the counter
						s_cnt_hi_r <= (others => '0');
						-- change the state
						s_tck_r0 <= '0';
						-- sample the enable signal
						s_enabled <= s_tcken;
						s_tcki_r0 <= s_tcken;
					else
						-- increment the hi counter
						s_cnt_hi_r <= s_cnt_hi_r + 1;
					end if;
				end if;
				s_tck_r1 <= s_tck_r0;
				s_tcki_r1 <= s_tcki_r0;
			else
				-- TCK disabled
				s_enabled <= s_tcken;
				s_tcki_r0 <= s_tcken;
				-- load the low timer comparator value
				s_cmp_lo <= s_div_lo;
				-- disable the RTCK for the first half cycle
				s_rtcken_reg <= '0';
				-- clear the RTCK timeout flag
				s_rtck_tmo <= '0';				
				s_tck_r0 <= '0';
				s_tck_r1 <= '0';
				s_tcki_r1 <= '0';
			end if;
		end if;
	end process p_count;

	s_rtck_fall <= not s_rtck_r1 and s_rtck_r2;
	s_rtck_rise <= s_rtck_r1 and not s_rtck_r2;

	s_tck_rise  <= s_tck_r0 and not s_tck_r1;
	s_tck_fall <= s_tcki_r0 and not s_tcki_r1;

	tck <= s_tck_r1;
	iotckp <= s_enabled and s_tck_rise;
	iotckn <= s_enabled and s_tck_fall;
	rtcktmo <= s_rtck_tmo;

end rtl;

