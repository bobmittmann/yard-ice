-- File:	jtag_enh_io.vhdl
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
-- JTAG Input/Output
-- Sinchronizes the input and output with the IO clock and system clock
-- 

library ieee;
use ieee.std_logic_1164.all; 

entity jtag_enh_io is
port ( 
	-- system clock (main cklock, master clock)
	mclk : in std_logic;	
	-- reset
	rst : in std_logic := '0';	
	-- IO clock
	ioclk : in std_logic;	
	-- IO domain TCK positive (rise) strobe
	iotckp : in std_logic;
	-- IO domain TCK negative (fall) strobe
	iotckn : in std_logic;
	--
	mtms : in std_logic := '0';
	mtdi : in std_logic := '0';
	-- JTAG TAP TDO
	tdo : in std_logic := '0';
	-- Main clock domain TCK positive strobe
	tckp_stb : out std_logic;
	-- Main clock domain TCK negative strobe
	tckn_stb : out std_logic;
	-- JTAG TAP TMS
	tms : out std_logic;
	-- JTAG TAP TDI
	tdi : out std_logic;
	mtdo : out std_logic
);
end jtag_enh_io;

architecture rtl of jtag_enh_io is 
	signal s_tckp_r0 : std_logic;	
	signal s_tckp_r1 : std_logic;

	signal s_tckn_r0 : std_logic;
	signal s_tckn_r1 : std_logic;

	signal s_tdo_r0 : std_logic;
	signal s_tdo_r1 : std_logic;
	signal s_tdi_r0 : std_logic;
	signal s_tms_r0 : std_logic;
begin 
	-- synchronize the TCKN signal 
	p_tckn_sync: process (rst, mclk, iotckn, s_tckn_r1)
	begin
		if (s_tckn_r1 = '1') then
			s_tckn_r0 <= '0';
		elsif rising_edge(iotckn) then
			s_tckn_r0 <= '1';
		end if;

		if (rst = '1') then
			s_tckn_r0 <= '0';
		elsif rising_edge(mclk) then
			s_tckn_r1 <= s_tckn_r0;
		end if;
	end process p_tckn_sync;	

	-- synchronize the TCKP 
	p_tckp_sync: process (rst, mclk, iotckp, s_tckp_r1)
	begin
		if (s_tckp_r1 = '1') then
			s_tckp_r0 <= '0';
		elsif rising_edge(iotckp) then
			s_tckp_r0 <= '1';
		end if;

		if (rst = '1') then
			s_tckp_r0 <= '0';
		elsif rising_edge(mclk) then
			s_tckp_r1 <= s_tckp_r0;
		end if;
	end process p_tckp_sync;

	-- synchronized clock edges 
	-- positive edge pulse
	tckn_stb <= s_tckn_r1;
	-- negative edge pulse
	tckp_stb <= s_tckp_r1;

	-- register the TDI and TMS signals
	p_txd: process (rst, ioclk, iotckn)
	begin
		if (rst = '1') then
			s_tdi_r0 <= '0';
			s_tms_r0 <= '0';
		elsif rising_edge(ioclk) and (iotckn = '1') then
			s_tdi_r0 <= mtdi;
			s_tms_r0 <= mtms;
		end if;
	end process p_txd;

	-- capture the TDO signal (IO clock domain)
	p_tdo_io: process (rst, ioclk, iotckn)
	begin
		if (rst = '1') then
			s_tdo_r0 <= '0';
		elsif rising_edge(ioclk) and (iotckn = '1') then
			s_tdo_r0 <= tdo;
		end if;
	end process p_tdo_io;

	tms <= s_tms_r0;
	tdi <= s_tdi_r0;
	mtdo <= s_tdo_r0;
end rtl;
