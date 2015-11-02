-- File:	jtag_enh_ecu.vhdl
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

-- State    Code
-- DREXIT2     0 -- Exit2-DR
-- DREXIT1     1 -- Exit1-DR
-- DRSHIFT     2 -- Shift-DR
-- DRPAUSE     3 -- Pause-DR
-- IRSELECT    4 -- Select-IR-Scan
-- DRUPDATE    5 -- Update-DR
-- DRCAPTURE   6 -- Capture-DR
-- DRSELECT    7 -- Select-DR-Scan
-- IREXIT2     8 -- Exit2-IR
-- IREXIT1     9 -- Exit1-IR
-- IRSHIFT    10 -- Shift-IR
-- IRPAUSE    11 -- Pause-IR
-- IRUPDATE   12 -- Update-IR
-- IDLE       13 -- Run-Test-Idle
-- IRCAPTURE  14 -- Capture-IR
-- RESET      15 -- Test-Logic-Reset

-- Current  Final
-- State    State    Default State Path
-- RESET    RESET    At least one TCK cycle applied with TMS = 1
-- RESET    IDLE     RESET-IDLE
-- RESET    DRPAUSE  RESET-IDLE-DRSELECT-DRCAPTURE-DREXIT1-DRPAUSE
-- RESET    IRPAUSE  RESET-IDLE-DRSELECT-IRSELECT-IRCAPTURE-IREXIT1-IRPAUSE
-- IDLE     RESET    IDLE-DRSELECT-IRSELECT-RESET
-- IDLE     IDLE     At least one TCK cycle applied with TMS = 0
-- IDLE     DRPAUSE  IDLE-DRSELECT-DRCAPTURE-DREXIT1-DRPAUSE
-- IDLE     IRPAUSE  IDLE-DRSELECT-IRSELECT-IRCAPTURE-IREXIT1-IRPAUSE
-- DRPAUSE  RESET    DRPAUSE-DREXIT2-DRUPDATE-DRSELECT-IRSELECT-RESET
-- DRPAUSE  IDLE     DRPAUSE-DREXIT2-DRUPDATE-IDLE
-- DRPAUSE  DRPAUSE  At least one TCK cycle applied with TMS = 0
-- DRPAUSE  IRPAUSE  DRPAUSE-DREXIT2-DRUPDATE-DRSELECT-
--                   IRSELECT-IRCAPTURE-IREXIT1-IRPAUSE
-- IRPAUSE  RESET    IRPAUSE-IREXIT2-IRUPDATE-DRSELECT-IRSELECT-RESET
-- IRPAUSE  IDLE     IRPAUSE-IREXIT2-IRUPDATE-IDLE
-- IRPAUSE  DRPAUSE  IRPAUSE-IREXIT2-IRUPDATE-DRSELECT-DRCAPTURE-DREXIT1-DRPAUSE
-- IRPAUSE  IRPAUSE  At least one TCK cycle applied with TMS = 0


-- 
-- JTAG Enhanced Execution Control Unit
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity jtag_enh_ecu is
generic (
	INSN_BITS : integer := 16;
	DESC_ADDR_BITS : natural := 7;
	PTR_ADDR_BITS : natural := 8
);
port (
	-- system clock
	clk : in std_logic;
	-- reset
	rst : in std_logic := '0';
	-- TCK positive edge strobe
	tckp_stb : in std_logic := '0';
	-- TCK negative edge strobe
	tckn_stb : in std_logic := '0';
	-- exec strobe
	exec_stb : in std_logic := '0';
	-- instruction
	insn : in std_logic_vector(INSN_BITS - 1 downto 0) := (others => '0');
	--
	-- end ofa transmission 
	tx_done_stb : in std_logic := '0';
	-- end of reception
	rx_done_stb : in std_logic := '0';
	

	-- enable tck output
	tck_en : out std_logic;
	-- enable TDI data
	tdi_en : out std_logic;
	-- system clock TMS
	mtms : out std_logic;
	-- enable shifter
	shift_en : out std_logic;
	-- pointer address
	ptr_addr : out std_logic_vector(PTR_ADDR_BITS - 1 downto 0);
	-- pointer read strobe
	ptr_rd_stb : out std_logic;

	start_ack : out std_logic;
	end_stb : out std_logic;
	busy : out std_logic;

	-- DEBUG
	tap_state : out unsigned(3 downto 0);
	insn_invalid : out std_logic;
	insn_reset : out std_logic;
	insn_goto : out std_logic;
	insn_drscan : out std_logic;
	insn_drpause : out std_logic;
	insn_irscan : out std_logic;
	insn_irpause : out std_logic;
	insn_runtest : out std_logic
);
end jtag_enh_ecu;

architecture rtl of jtag_enh_ecu is

	subtype tap_state_t is integer range 0 to 15;
	
	constant TAP_DREXIT2   : tap_state_t := 16#00#; -- Exit2-DR
	constant TAP_DREXIT1   : tap_state_t := 16#01#; -- Exit1-DR
	constant TAP_DRSHIFT   : tap_state_t := 16#02#; -- Shift-DR
	constant TAP_DRPAUSE   : tap_state_t := 16#03#; -- Pause-DR
	constant TAP_IRSELECT  : tap_state_t := 16#04#; -- Select-IR-Scan
	constant TAP_DRUPDATE  : tap_state_t := 16#05#; -- Update-DR
	constant TAP_DRCAPTURE : tap_state_t := 16#06#; -- Capture-DR
	constant TAP_DRSELECT  : tap_state_t := 16#07#; -- Select-DR-Scan
	constant TAP_IREXIT2   : tap_state_t := 16#08#; -- Exit2-IR
	constant TAP_IREXIT1   : tap_state_t := 16#09#; -- Exit1-IR
	constant TAP_IRSHIFT   : tap_state_t := 16#0A#; -- Shift-IR
	constant TAP_IRPAUSE   : tap_state_t := 16#0B#; -- Pause-IR
	constant TAP_IDLE      : tap_state_t := 16#0C#; -- Run-Test-Idle
	constant TAP_IRUPDATE  : tap_state_t := 16#0D#; -- Update-IR
	constant TAP_IRCAPTURE : tap_state_t := 16#0E#; -- Capture-IR
	constant TAP_RESET     : tap_state_t := 16#0F#; -- Test-Logic-Reset

	--
	type ctrl_opcode_t is (OPC_INVALID,
						   OPC_RUN_TEST, 
						   OPC_DR_SCAN, 
						   OPC_IR_SCAN,
						   OPC_RESET,
						   OPC_IR_PAUSE,
						   OPC_DR_PAUSE,
						   OPC_STATE
						  );
	--
	type ctrl_state_t is (CTRL_IDLE, CTRL_RUN, 
						  CTRL_SELECT_IR, CTRL_CAPTURE_IR, 
						  CTRL_SHIFT_IR, CTRL_EXIT_IR,
						  CTRL_SELECT_DR, CTRL_CAPTURE_DR, 
						  CTRL_SHIFT_DR, CTRL_EXIT_DR, CTRL_UPDATE, 
						  CTRL_RESET, CTRL_STOP, CTRL_READY, CTRL_START
						 );

	signal s_opc : ctrl_opcode_t;
	signal s_loop_cnt : unsigned(7 downto 0);

	signal s_cnt_top : unsigned(7 downto 0);
	signal s_cnt_reg  : unsigned(7 downto 0);
	signal s_cnt_ovf  : std_logic;
	signal s_cnt_en : std_logic;
	signal s_cnt_clr : std_logic;
	signal s_exec_en : std_logic;

	signal s_exec_r0 : std_logic;
	signal s_exec_r1 : std_logic;
	signal s_exec_stb : std_logic;
	signal s_start_stb : std_logic;
	signal s_tck_en : std_logic;
	signal s_tdi_en : std_logic;
	signal s_shift_en : std_logic;
	signal s_shift_en_r0 : std_logic; 
	signal s_tms : std_logic;
	signal s_end_stb : std_logic;

	signal s_stop : std_logic;

	signal s_insn_r0 : std_logic_vector(INSN_BITS - 1 downto 0);
	signal s_insn_ld : std_logic;

	signal s_goto_state_en : std_logic;
	signal s_pos_goto_en : std_logic;

	signal s_opc_valid : std_logic;

	signal s_goto_state : tap_state_t;
	signal s_goto_exec : std_logic;
	signal s_goto_stop : std_logic;

	signal s_drscan_state : tap_state_t;
	signal s_drscan_shift_en : std_logic; 
	signal s_drscan_goto_en : std_logic;

	signal s_drpause_state : tap_state_t;
	signal s_drpause_cnt_en : std_logic; 
	signal s_drpause_goto_en : std_logic;

	signal s_irscan_state : tap_state_t;
	signal s_irscan_shift_en : std_logic; 
	signal s_irscan_goto_en : std_logic;

	signal s_irpause_state : tap_state_t;
	signal s_irpause_cnt_en : std_logic; 
	signal s_irpause_goto_en : std_logic;

	signal s_runtest_state : tap_state_t;
	signal s_runtest_cnt_en : std_logic;
	signal s_runtest_goto_en : std_logic;

	signal s_reset_state : tap_state_t;
	signal s_reset_cnt_en : std_logic;
	signal s_reset_goto_en : std_logic;

	-- pointer address
	signal s_ptr_addr : std_logic_vector(PTR_ADDR_BITS - 1 downto 0);

	signal s_end_state : tap_state_t;
	signal s_tap_state : tap_state_t;
	signal s_tap_state_next : tap_state_t;

	type tms_matrix_t is array (tap_state_t, tap_state_t) of std_logic;

	-- return the TMS value from TAP transition (current, next state)
	constant tms_lookup : tms_matrix_t := (
		TAP_DREXIT2 =>   (TAP_DRSHIFT =>   '0', TAP_DRUPDATE => '1', others => 'X'),
		TAP_DREXIT1 =>   (TAP_DRPAUSE =>   '0', TAP_DRUPDATE => '1', others => 'X'),
		TAP_DRSHIFT =>   (TAP_DRSHIFT =>   '0', TAP_DREXIT1 =>  '1', others => 'X'),
		TAP_DRPAUSE =>   (TAP_DRPAUSE =>   '0', TAP_DREXIT2 =>  '1', others => 'X'),
		TAP_IRSELECT =>  (TAP_IRCAPTURE => '0', TAP_RESET =>    '1', others => 'X'),
		TAP_DRUPDATE =>  (TAP_IDLE =>      '0', TAP_DRSELECT => '1', others => 'X'),
		TAP_DRCAPTURE => (TAP_DRSHIFT =>   '0', TAP_DREXIT1 =>  '1', others => 'X'),
		TAP_DRSELECT =>  (TAP_DRCAPTURE => '0', TAP_IRSELECT => '1', others => 'X'),
		TAP_IREXIT2 =>   (TAP_IRSHIFT =>   '0', TAP_IRUPDATE => '1', others => 'X'),
		TAP_IREXIT1 =>   (TAP_IRPAUSE =>   '0', TAP_IRUPDATE => '1', others => 'X'),
		TAP_IRSHIFT =>   (TAP_IRSHIFT =>   '0', TAP_IREXIT1 =>  '1', others => 'X'),
		TAP_IRPAUSE =>   (TAP_IRPAUSE =>   '0', TAP_IREXIT2 =>  '1', others => 'X'),
		TAP_IRUPDATE =>  (TAP_IDLE =>      '0', TAP_DRSELECT => '1', others => 'X'),
		TAP_IDLE =>      (TAP_IDLE =>      '0', TAP_DRSELECT => '1', others => 'X'),
		TAP_IRCAPTURE => (TAP_IRSHIFT =>   '0', TAP_IREXIT1 =>  '1', others => 'X'),
		TAP_RESET =>     (TAP_IDLE =>      '0', TAP_RESET =>    '1', others => 'X')
	);
	
begin

	---------------------------------------------------------------------------
	-- synchronize the exec_stb signal and start the execution
	p_exec_syn: process (clk, rst, exec_stb, s_exec_r0, s_stop, s_opc_valid)
	begin
		if (rst = '1') then
			s_exec_r0 <= '0';
			s_exec_r1 <= '0';
		elsif rising_edge(clk) then
			if (s_exec_r0 = '0') and (exec_stb = '1') then
				s_exec_r0 <= '1';
			elsif (s_stop = '1') or (s_opc_valid = '0') then
				s_exec_r0 <= '0';
			end if;
			s_exec_r1 <= s_exec_r0;
		end if;
	end process p_exec_syn;

	s_start_stb <= exec_stb and not s_exec_r0;
	s_exec_stb <= s_exec_r0 and not s_exec_r1 and s_opc_valid;
	s_end_stb <= not s_exec_r0 and s_exec_r1;
	s_exec_en <= s_exec_r1;
--	s_end_stb <= not s_exec_r0;
	s_insn_ld <= s_start_stb;
	s_cnt_clr <= s_start_stb;

	ptr_rd_stb <= s_exec_stb;
	-----------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- load the instruction register
	p_insn_load: process (clk, rst, s_insn_ld)
	begin
		if (rst = '1') then
			s_insn_r0 <= (others => '0');
		elsif rising_edge(clk) and (s_insn_ld = '1') then
			s_insn_r0 <= insn;
		end if;
	end process p_insn_load;
	---------------------------------------------------------------------------

	-- Instructions encoding
	--
	-- IR_SCAN  | 1111 eeee vvvv vvvv |
	-- DR_SCAN  | 1110 eeee vvvv vvvv |
	--
	-- RESET    | 1101 eeee nnnn nnnn |
	-- RUN_TEST | 1100 eeee nnnn nnnn |
	-- IR_PAUSE | 1011 eeee nnnn nnnn |
	-- DR_PAUSE | 1010 eeee nnnn nnnn |
	--
	-- STATE    | 1001 0000 0000 eeee |
	--
	--
	-- eeee : end state (4 bits)
	-- vvvv vvvv : vector descriptor pointer
	-- nnnn nnnn : immediate 8 bits number

	---------------------------------------------------------------------------
	-- instruction decoding
	s_opc <= 
		OPC_IR_SCAN  when (s_insn_r0 and X"F000") = X"F000" else
		OPC_DR_SCAN  when (s_insn_r0 and X"F000") = X"E000" else
		OPC_RESET    when (s_insn_r0 and X"F000") = X"D000" else
		OPC_RUN_TEST when (s_insn_r0 and X"F000") = X"C000" else
		OPC_IR_PAUSE when (s_insn_r0 and X"F000") = X"B000" else
		OPC_DR_PAUSE when (s_insn_r0 and X"F000") = X"A000" else
		OPC_STATE     when (s_insn_r0 and X"FFF0") = X"9000" else
		OPC_INVALID;
	---------------------------------------------------------------------------

	s_opc_valid <= '0' when s_opc = OPC_INVALID else '1';

	---------------------------------------------------------------------------
	-- parameters decoding
	p_decode_parm: process (s_opc, s_insn_r0)
		variable state : unsigned(3 downto 0);
		variable count_top : unsigned(7 downto 0);
		variable addr : std_logic_vector(7 downto 0);
	begin
		if ((s_opc = OPC_IR_SCAN) or (s_opc = OPC_DR_SCAN)) then			
			state := unsigned(s_insn_r0(11 downto 8));
			count_top := (others => 'X');
			addr := s_insn_r0(7 downto 0);
		elsif ((s_opc = OPC_RESET) or (s_opc = OPC_RUN_TEST) or
			   (s_opc = OPC_IR_PAUSE) or (s_opc = OPC_DR_PAUSE)) then
			state := unsigned(s_insn_r0(11 downto 8));
			count_top := unsigned(s_insn_r0(7 downto 0));
			addr := (others => 'X');
		elsif (s_opc = OPC_STATE) then
			state := unsigned(s_insn_r0(3 downto 0));
			count_top := (others => 'X');
			addr := (others => 'X');
		else
			state := (others => 'X');
			count_top := (others => 'X');
			addr := (others => 'X');		
		end if;
		s_end_state <= to_integer(state);
		s_cnt_top <= count_top;
		s_ptr_addr <= addr(PTR_ADDR_BITS - 1 downto 0);
	end process	p_decode_parm;
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- instruction execution
	p_exec: process (clk, rst, s_exec_stb, tckn_stb)
	begin
		if (rst = '1') then
			s_tap_state <= TAP_RESET;
			s_pos_goto_en <= '0';
		elsif rising_edge(clk) then
			if (s_stop = '1') then
				s_tck_en <= '0'; 
				s_pos_goto_en <= '0';
			elsif (s_exec_stb = '1') then
				s_tck_en <= '1';
				s_pos_goto_en <= '0';
			elsif (s_pos_goto_en = '0') and (s_exec_en = '1') then 
				-- pos execution goto enable
				if (s_opc = OPC_DR_SCAN) then
					s_pos_goto_en <= s_drscan_goto_en;
					s_tdi_en <= '1';
				elsif (s_opc = OPC_DR_PAUSE) then
					s_pos_goto_en <= s_drpause_goto_en;
					s_tdi_en <= '0';
				elsif (s_opc = OPC_IR_SCAN) then
					s_pos_goto_en <= s_irscan_goto_en;
					s_tdi_en <= '1';
				elsif (s_opc = OPC_IR_PAUSE) then
					s_pos_goto_en <= s_irpause_goto_en;
					s_tdi_en <= '0';
				elsif (s_opc = OPC_RUN_TEST) then
					s_pos_goto_en <= s_runtest_goto_en;
					s_tdi_en <= '0';
				elsif (s_opc = OPC_RESET) then
					s_pos_goto_en <= s_reset_goto_en;
					s_tdi_en <= '0';
				else
					s_pos_goto_en <= '0';
					s_tdi_en <= '0';
				end if;
			end if;
			if (tckn_stb = '1') then
				s_tap_state <= s_tap_state_next;
			end if;

		end if;
	end process p_exec;

	s_goto_state_en <= '1' when (s_opc = OPC_STATE) or 
		(s_pos_goto_en = '1') else '0';
	---------------------------------------------------------------------------


	---------------------------------------------------------------------------

	s_tms <= tms_lookup(s_tap_state, s_tap_state_next);

	---------------------------------------------------------------------------
	-- loop count
	p_count: process (clk, rst, s_cnt_clr, s_cnt_en, tckn_stb)
	begin
		if (rst = '1') then
			s_cnt_reg <= (others => '0');
		elsif rising_edge(clk) then
			if (s_cnt_clr = '1')  then
				s_cnt_reg <= (others => '0');
			elsif (s_cnt_en = '1') and (tckn_stb = '1') then
				s_cnt_reg <= s_cnt_reg + 1;
			end if;
		end if;
	end process p_count;

	s_cnt_ovf <= '1' when s_cnt_reg = s_cnt_top else '0';
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- STATE instruction state machine
	-- causes the IEEE 1149.1 state machine to go to the specified state
	p_goto_state_fsm: process (s_end_state, s_tap_state)
	begin
		case s_end_state is
			when TAP_RESET  =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_RESET;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_IRSELECT;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_RESET;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_IDLE =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_IDLE;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_IRSELECT;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_IDLE;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_RESET;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_IDLE;
				end case;

			when TAP_DRSELECT =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_RESET;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_DRCAPTURE =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_DRCAPTURE;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DRCAPTURE;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_RESET;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_DRSHIFT =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_DRCAPTURE;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DRSHIFT;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DRSHIFT;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_RESET;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_DREXIT1 =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_DRCAPTURE;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_RESET;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_DRPAUSE =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_DRCAPTURE;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRPAUSE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DRPAUSE;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_RESET;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_DREXIT2 =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_DRCAPTURE;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRPAUSE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_RESET;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_DRUPDATE =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_DRCAPTURE;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_RESET;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_IRSELECT =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_IRSELECT;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_IRSELECT;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_IRCAPTURE =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_IRSELECT;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_IRCAPTURE;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IRCAPTURE;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_IRSHIFT =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_IRSELECT;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_IRCAPTURE;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IRSHIFT;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IRSHIFT;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_IREXIT1 =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_IRSELECT;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_IRCAPTURE;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_IRPAUSE =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_IRSELECT;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRPAUSE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_IRCAPTURE;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRPAUSE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IRPAUSE;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_IREXIT2 =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_IRSELECT;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRPAUSE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_IRCAPTURE;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRPAUSE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
				end case;

			when TAP_IRUPDATE =>
				case s_tap_state is
					when TAP_RESET =>
						s_goto_state <= TAP_IDLE;
					when TAP_IDLE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_DRSELECT =>
						s_goto_state <= TAP_IRSELECT;
					when TAP_DRCAPTURE =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DRSHIFT =>
						s_goto_state <= TAP_DREXIT1;
					when TAP_DREXIT1 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRPAUSE =>
						s_goto_state <= TAP_DREXIT2;
					when TAP_DREXIT2 =>
						s_goto_state <= TAP_DRUPDATE;
					when TAP_DRUPDATE =>
						s_goto_state <= TAP_DRSELECT;
					when TAP_IRSELECT =>
						s_goto_state <= TAP_IRCAPTURE;
					when TAP_IRCAPTURE =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IRSHIFT =>
						s_goto_state <= TAP_IREXIT1;
					when TAP_IREXIT1 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRPAUSE =>
						s_goto_state <= TAP_IREXIT2;
					when TAP_IREXIT2 =>
						s_goto_state <= TAP_IRUPDATE;
					when TAP_IRUPDATE =>
						s_goto_state <= TAP_IRUPDATE;
				end case;

		end case;
	end process p_goto_state_fsm;


	s_stop <= s_goto_state_en when (s_tap_state = s_end_state) else '0';
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- DR_SCAN instruction state machine
	p_drscan_fsm: process (s_tap_state, tx_done_stb)
	begin
		case s_tap_state is
			when TAP_RESET =>
				s_drscan_state <= TAP_IDLE;
				s_drscan_shift_en <= '0';
				s_drscan_goto_en <= '0';
			when TAP_IDLE =>
				s_drscan_state <= TAP_DRSELECT;
				s_drscan_shift_en <= '0';
				s_drscan_goto_en <= '0';
			when TAP_DRSELECT =>
				s_drscan_state <= TAP_DRCAPTURE;
				s_drscan_shift_en <= '0';
				s_drscan_goto_en <= '0';
			when TAP_DRCAPTURE =>
				s_drscan_state <= TAP_DRSHIFT;
				s_drscan_shift_en <= '1';
				s_drscan_goto_en <= '0';
			when TAP_DRSHIFT =>
				s_drscan_state <= TAP_DRSHIFT;
				s_drscan_shift_en <= '1';
				s_drscan_goto_en <= tx_done_stb;
			when TAP_DREXIT1 =>
				s_drscan_state <= TAP_DRPAUSE;
				s_drscan_shift_en <= '0';
				s_drscan_goto_en <= '0';
			when TAP_DRPAUSE =>
				s_drscan_state <= TAP_DREXIT2;
				s_drscan_shift_en <= '0';
				s_drscan_goto_en <= '0';
			when TAP_DREXIT2 =>
				s_drscan_state <= TAP_DRSHIFT;
				s_drscan_shift_en <= '1';
				s_drscan_goto_en <= '0';
			when TAP_DRUPDATE =>
				s_drscan_state <= TAP_DRSELECT;
				s_drscan_shift_en <= '0';
				s_drscan_goto_en <= '0';
			when TAP_IRSELECT =>
				s_drscan_state <= TAP_RESET;
				s_drscan_shift_en <= '0';
				s_drscan_goto_en <= '0';
			when TAP_IRCAPTURE =>
				s_drscan_state <= TAP_IREXIT1;
				s_drscan_shift_en <= '0';
				s_drscan_goto_en <= '0';
			when TAP_IRSHIFT =>
				s_drscan_state <= TAP_IREXIT1;
				s_drscan_shift_en <= '0';
				s_drscan_goto_en <= '0';
			when TAP_IREXIT1 =>
				s_drscan_state <= TAP_IRPAUSE;
				s_drscan_shift_en <= '0';
				s_drscan_goto_en <= '0';
			when TAP_IRPAUSE =>
				s_drscan_state <= TAP_IREXIT2;
				s_drscan_shift_en <= '0';
				s_drscan_goto_en <= '0';
			when TAP_IREXIT2 =>
				s_drscan_state <= TAP_IRUPDATE;
				s_drscan_shift_en <= '0';
				s_drscan_goto_en <= '0';
			when TAP_IRUPDATE =>
				s_drscan_state <= TAP_DRSELECT;
				s_drscan_shift_en <= '0';
				s_drscan_goto_en <= '0';
		end case;
	end process p_drscan_fsm;
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- DR_PAUSE instruction state machine
	p_drpause_fsm: process (s_tap_state, s_cnt_ovf)
	begin
		case s_tap_state is
			when TAP_RESET =>
				s_drpause_state <= TAP_IDLE;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
			when TAP_IDLE =>
				s_drpause_state <= TAP_DRSELECT;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
			when TAP_DRSELECT =>
				s_drpause_state <= TAP_DRCAPTURE;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
			when TAP_DRCAPTURE =>
				s_drpause_state <= TAP_DREXIT1;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
			when TAP_DRSHIFT =>
				s_drpause_state <= TAP_DREXIT1;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
			when TAP_DREXIT1 =>
				s_drpause_state <= TAP_DRPAUSE;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
			when TAP_DRPAUSE =>
				s_drpause_state <= TAP_DREXIT2;
				s_drpause_cnt_en <= '1';
				s_drpause_goto_en <= s_cnt_ovf;
			when TAP_DREXIT2 =>
				s_drpause_state <= TAP_DRSHIFT;
				s_drpause_cnt_en <= '1';
				s_drpause_goto_en <= '0';
			when TAP_DRUPDATE =>
				s_drpause_state <= TAP_DRSELECT;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
			when TAP_IRSELECT =>
				s_drpause_state <= TAP_RESET;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
			when TAP_IRCAPTURE =>
				s_drpause_state <= TAP_IREXIT1;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
			when TAP_IRSHIFT =>
				s_drpause_state <= TAP_IREXIT1;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
			when TAP_IREXIT1 =>
				s_drpause_state <= TAP_IRPAUSE;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
			when TAP_IRPAUSE =>
				s_drpause_state <= TAP_IREXIT2;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
			when TAP_IREXIT2 =>
				s_drpause_state <= TAP_IRUPDATE;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
			when TAP_IRUPDATE =>
				s_drpause_state <= TAP_DRSELECT;
				s_drpause_cnt_en <= '0';
				s_drpause_goto_en <= '0';
		end case;
	end process p_drpause_fsm;
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- IR_SCAN instruction state machine
	p_irscan_fsm: process (s_tap_state, tx_done_stb)
	begin
		case s_tap_state is
			when TAP_RESET =>
				s_irscan_state <= TAP_IDLE;
				s_irscan_shift_en <= '0';
				s_irscan_goto_en <= '0';
			when TAP_IDLE =>
				s_irscan_state <= TAP_DRSELECT;
				s_irscan_shift_en <= '0';
				s_irscan_goto_en <= '0';
			when TAP_DRCAPTURE =>
				s_irscan_state <= TAP_DREXIT1;
				s_irscan_shift_en <= '0';
				s_irscan_goto_en <= '0';
			when TAP_DRSHIFT =>
				s_irscan_state <= TAP_DREXIT1;
				s_irscan_shift_en <= '0';
				s_irscan_goto_en <= '0';
			when TAP_DREXIT1 =>
				s_irscan_state <= TAP_DRPAUSE;
				s_irscan_shift_en <= '0';
				s_irscan_goto_en <= '0';
			when TAP_DRPAUSE =>
				s_irscan_state <= TAP_DREXIT2;
				s_irscan_shift_en <= '0';
				s_irscan_goto_en <= '0';
			when TAP_DREXIT2 =>
				s_irscan_state <= TAP_DRUPDATE;
				s_irscan_shift_en <= '0';
				s_irscan_goto_en <= '0';
			when TAP_DRUPDATE =>
				s_irscan_state <= TAP_DRSELECT;
				s_irscan_shift_en <= '0';
				s_irscan_goto_en <= '0';
			when TAP_IRUPDATE =>
				s_irscan_state <= TAP_DRSELECT;
				s_irscan_shift_en <= '0';
				s_irscan_goto_en <= '0';
			when TAP_DRSELECT =>
				s_irscan_state <= TAP_IRSELECT;
				s_irscan_shift_en <= '0';
				s_irscan_goto_en <= '0';
			when TAP_IRSELECT =>
				s_irscan_state <= TAP_IRCAPTURE;
				s_irscan_shift_en <= '0';
				s_irscan_goto_en <= '0';
			when TAP_IRCAPTURE =>
				s_irscan_state <= TAP_IRSHIFT;
				s_irscan_shift_en <= '1';
				s_irscan_goto_en <= '0';
			when TAP_IREXIT1 =>
				s_irscan_state <= TAP_IRPAUSE;
				s_irscan_shift_en <= '0';
				s_irscan_goto_en <= '0';
			when TAP_IRPAUSE =>
				s_irscan_state <= TAP_IREXIT2;
				s_irscan_shift_en <= '0';
				s_irscan_goto_en <= '0';
			when TAP_IREXIT2 =>
				s_irscan_state <= TAP_IRSHIFT;
				s_irscan_shift_en <= '1';
				s_irscan_goto_en <= '0';
			when TAP_IRSHIFT =>
				s_irscan_state <= TAP_IRSHIFT;
				s_irscan_shift_en <= '1';
				s_irscan_goto_en <= tx_done_stb;
		end case;
	end process p_irscan_fsm;
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- IR_PAUSE instruction state machine
	p_irpause_fsm: process (s_tap_state, s_cnt_ovf)
	begin
		case s_tap_state is
			when TAP_RESET =>
				s_irpause_state <= TAP_IDLE;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_IDLE =>
				s_irpause_state <= TAP_DRSELECT;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_DRCAPTURE =>
				s_irpause_state <= TAP_DREXIT1;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_DRSHIFT =>
				s_irpause_state <= TAP_DREXIT1;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_DREXIT1 =>
				s_irpause_state <= TAP_DRUPDATE;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_DRPAUSE =>
				s_irpause_state <= TAP_DREXIT2;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_DREXIT2 =>
				s_irpause_state <= TAP_DRUPDATE;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_DRUPDATE =>
				s_irpause_state <= TAP_DRSELECT;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_IRUPDATE =>
				s_irpause_state <= TAP_DRSELECT;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_DRSELECT =>
				s_irpause_state <= TAP_IRSELECT;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_IRSELECT =>
				s_irpause_state <= TAP_IRCAPTURE;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_IRCAPTURE =>
				s_irpause_state <= TAP_IREXIT1;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_IRSHIFT =>
				s_irpause_state <= TAP_IREXIT1;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_IREXIT1 =>
				s_irpause_state <= TAP_IRPAUSE;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
			when TAP_IRPAUSE =>
				s_irpause_state <= TAP_IRPAUSE;
				s_irpause_cnt_en <= '1';
				s_irpause_goto_en <= s_cnt_ovf;
			when TAP_IREXIT2 =>
				s_irpause_state <= TAP_IRUPDATE;
				s_irpause_cnt_en <= '0';
				s_irpause_goto_en <= '0';
		end case;
	end process p_irpause_fsm;
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- RESET instruction state machine
	p_reset_fsm: process (s_tap_state, s_cnt_ovf)
	begin
		case s_tap_state is
			when TAP_DRSHIFT =>
				s_reset_state <= TAP_DREXIT1;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_DRCAPTURE =>
				s_reset_state <= TAP_DREXIT1;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_DREXIT1 =>
				s_reset_state <= TAP_DRUPDATE;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_DRPAUSE =>
				s_reset_state <= TAP_DREXIT2;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_DREXIT2 =>
				s_reset_state <= TAP_DRUPDATE;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_IRSHIFT =>
				s_reset_state <= TAP_IREXIT1;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_IRCAPTURE =>
				s_reset_state <= TAP_IREXIT1;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_IREXIT1 =>
				s_reset_state <= TAP_IRUPDATE;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_IRPAUSE =>
				s_reset_state <= TAP_IREXIT2;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_IREXIT2 =>
				s_reset_state <= TAP_IRUPDATE;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_DRUPDATE =>
				s_reset_state <= TAP_DRSELECT;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_IRUPDATE =>
				s_reset_state <= TAP_DRSELECT;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_IDLE =>
				s_reset_state <= TAP_DRSELECT;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_DRSELECT =>
				s_reset_state <= TAP_IRSELECT;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_IRSELECT =>
				s_reset_state <= TAP_RESET;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= '0';
			when TAP_RESET =>
				s_reset_state <= TAP_RESET;
				s_reset_cnt_en <= '1';
				s_reset_goto_en <= s_cnt_ovf;
		end case;
	end process p_reset_fsm;
	---------------------------------------------------------------------------

	---------------------------------------------------------------------------
	-- RUN_TEST instruction state machine
	p_runtest_fsm: process (s_tap_state, s_cnt_ovf)
	begin
		case s_tap_state is
			when TAP_DRSHIFT =>
				s_runtest_state <= TAP_DREXIT1;
				s_runtest_cnt_en <= '0';
				s_runtest_goto_en <= '0';
			when TAP_DRCAPTURE =>
				s_runtest_state <= TAP_DREXIT1;
				s_runtest_cnt_en <= '0';
				s_runtest_goto_en <= '0';
			when TAP_DREXIT1 =>
				s_runtest_state <= TAP_DRUPDATE;
				s_runtest_cnt_en <= '0';
				s_runtest_goto_en <= '0';
			when TAP_DRPAUSE =>
				s_runtest_state <= TAP_DREXIT2;
				s_runtest_cnt_en <= '0';
				s_runtest_goto_en <= '0';
			when TAP_DREXIT2 =>
				s_runtest_state <= TAP_DRUPDATE;
				s_runtest_cnt_en <= '0';
				s_runtest_goto_en <= '0';
			when TAP_IRSHIFT =>
				s_runtest_state <= TAP_IREXIT1;
				s_runtest_cnt_en <= '0';
				s_runtest_goto_en <= '0';
			when TAP_IRCAPTURE =>
				s_runtest_state <= TAP_IREXIT1;
				s_runtest_cnt_en <= '0';
				s_runtest_goto_en <= '0';
			when TAP_IREXIT1 =>
				s_runtest_state <= TAP_IRUPDATE;
				s_runtest_cnt_en <= '0';
				s_runtest_goto_en <= '0';
			when TAP_IRPAUSE =>
				s_runtest_state <= TAP_IREXIT2;
				s_runtest_cnt_en <= '0';
				s_runtest_goto_en <= '0';
			when TAP_IREXIT2 =>
				s_runtest_state <= TAP_IRUPDATE;
				s_runtest_cnt_en <= '0';
				s_runtest_goto_en <= '0';
			when TAP_DRSELECT =>
				s_runtest_state <= TAP_IRSELECT;
				s_runtest_cnt_en <= '0';
				s_runtest_goto_en <= '0';
			when TAP_IRSELECT =>
				s_runtest_state <= TAP_RESET;
				s_runtest_cnt_en <= '0';
				s_runtest_goto_en <= '0';
			when TAP_RESET =>
				s_runtest_state <= TAP_IDLE;
				s_runtest_cnt_en <= '1';
				s_runtest_goto_en <= '0';
			when TAP_DRUPDATE =>
				s_runtest_state <= TAP_IDLE;
				s_runtest_cnt_en <= '1';
				s_runtest_goto_en <= '0';
			when TAP_IRUPDATE =>
				s_runtest_state <= TAP_IDLE;
				s_runtest_cnt_en <= '1';
				s_runtest_goto_en <= '0';
			when TAP_IDLE =>
				s_runtest_state <= TAP_IDLE;
				s_runtest_cnt_en <= '1';
				s_runtest_goto_en <= s_cnt_ovf;
		end case;
	end process p_runtest_fsm;
	---------------------------------------------------------------------------

	s_tap_state_next <= s_goto_state when s_goto_state_en = '1' else
			s_reset_state when s_opc = OPC_RESET else
			s_drscan_state when s_opc = OPC_DR_SCAN else
			s_drpause_state when s_opc = OPC_DR_PAUSE else
			s_irscan_state when s_opc = OPC_IR_SCAN else
			s_irpause_state when s_opc = OPC_IR_PAUSE else
			s_runtest_state when s_opc = OPC_RUN_TEST else
			s_tap_state;

	s_cnt_en <= s_drpause_cnt_en when s_opc = OPC_DR_PAUSE else
			s_irpause_cnt_en when s_opc = OPC_IR_PAUSE else
			s_runtest_cnt_en when s_opc = OPC_RUN_TEST else 
			s_reset_cnt_en when s_opc = OPC_RESET else '0';

	s_shift_en <= s_drscan_shift_en when s_opc = OPC_DR_SCAN else
			s_irscan_shift_en when s_opc = OPC_IR_SCAN else '0';

	p_shift_stb: process (clk, rst, s_insn_ld)
	begin
		if (rst = '1') then
			s_shift_en_r0 <= '0';
		elsif rising_edge(clk) then
			s_shift_en_r0 <= s_shift_en;
		end if;
	end process p_shift_stb;
	---------------------------------------------------------------------------


	---------------------------------------------------------------------------

	mtms <= s_tms;
	shift_en <= s_shift_en and not s_shift_en_r0;
	tap_state <= to_unsigned(s_tap_state, 4);
	start_ack <= s_exec_stb; 
	tck_en <= s_tck_en;
	tdi_en <= s_tdi_en;
	ptr_addr <= s_ptr_addr;
	end_stb <= s_end_stb;
	busy <= s_exec_r0;

	insn_reset <= '1' when s_opc = OPC_RESET else '0';
	insn_goto <= s_goto_state_en;
	insn_drscan <= '1' when s_opc = OPC_DR_SCAN else '0';
	insn_drpause <= '1' when s_opc = OPC_DR_PAUSE else '0';
	insn_irscan <= '1' when s_opc = OPC_IR_SCAN else '0';
	insn_irpause <= '1' when s_opc = OPC_IR_PAUSE else '0';
	insn_runtest <= '1' when s_opc = OPC_RUN_TEST else '0';
	insn_invalid <= '1' when s_opc = OPC_INVALID else '0';

end rtl;

