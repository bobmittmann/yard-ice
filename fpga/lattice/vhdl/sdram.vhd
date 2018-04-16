-- VHDL netlist generated by SCUBA Diamond (64-bit) 3.10.0.111.2
-- Module  Version: 5.8
--C:\lscc\diamond\3.10_x64\ispfpga\bin\nt64\scuba.exe -w -n sdram -lang vhdl -synth lse -bus_exp 7 -bb -arch xo2c00 -type iol -mode mem -io_type SSTL25_I -freq_in 100 -dqs 1 8 -sensitivity low 

-- Wed Apr 11 15:02:45 2018

library IEEE;
use IEEE.std_logic_1164.all;
-- synopsys translate_off
library MACHXO2;
use MACHXO2.components.all;
-- synopsys translate_on

entity sdram is
    port (
        burstdet0: out  std_logic; 
        datatri_0: in  std_logic; 
        datavalid_0: out  std_logic; 
        dqs_0: inout  std_logic; 
        dqso_0: in  std_logic; 
        dqstri_0: in  std_logic; 
        freeze: in  std_logic; 
        lock: out  std_logic; 
        read_0: in  std_logic; 
        readclksel0_0: in  std_logic; 
        readclksel1_0: in  std_logic; 
        reset: in  std_logic; 
        sclk: in  std_logic; 
        uddcntln: in  std_logic; 
        datain0: out  std_logic_vector(15 downto 0); 
        dataout0: in  std_logic_vector(15 downto 0); 
        dq_0: inout  std_logic_vector(7 downto 0));
end sdram;

architecture Structure of sdram is

    -- internal signal declarations
    signal sclk90: std_logic;
    signal scuba_vlo: std_logic;
    signal dqsw90_0: std_logic;
    signal dqsr90_0: std_logic;
    signal ddrclkpol0: std_logic;
    signal dqsdel: std_logic;
    signal db7: std_logic;
    signal db6: std_logic;
    signal db5: std_logic;
    signal db4: std_logic;
    signal db3: std_logic;
    signal db2: std_logic;
    signal db1: std_logic;
    signal db0: std_logic;
    signal da7: std_logic;
    signal da6: std_logic;
    signal da5: std_logic;
    signal da4: std_logic;
    signal da3: std_logic;
    signal da2: std_logic;
    signal da1: std_logic;
    signal da0: std_logic;
    signal qb7: std_logic;
    signal qb6: std_logic;
    signal qb5: std_logic;
    signal qb4: std_logic;
    signal qb3: std_logic;
    signal qb2: std_logic;
    signal qb1: std_logic;
    signal qb0: std_logic;
    signal qa7: std_logic;
    signal qa6: std_logic;
    signal qa5: std_logic;
    signal qa4: std_logic;
    signal qa3: std_logic;
    signal qa2: std_logic;
    signal qa1: std_logic;
    signal qa0: std_logic;
    signal dqtri0: std_logic;
    signal dqo7: std_logic;
    signal dqo6: std_logic;
    signal dqo5: std_logic;
    signal dqo4: std_logic;
    signal dqo3: std_logic;
    signal dqo2: std_logic;
    signal dqo1: std_logic;
    signal dqo0: std_logic;
    signal datain_t7: std_logic;
    signal datain_t6: std_logic;
    signal datain_t5: std_logic;
    signal datain_t4: std_logic;
    signal datain_t3: std_logic;
    signal datain_t2: std_logic;
    signal datain_t1: std_logic;
    signal datain_t0: std_logic;
    signal buf_dq_0i7: std_logic;
    signal buf_dq_0t7: std_logic;
    signal buf_dq_0o7: std_logic;
    signal buf_dq_0i6: std_logic;
    signal buf_dq_0t6: std_logic;
    signal buf_dq_0o6: std_logic;
    signal buf_dq_0i5: std_logic;
    signal buf_dq_0t5: std_logic;
    signal buf_dq_0o5: std_logic;
    signal buf_dq_0i4: std_logic;
    signal buf_dq_0t4: std_logic;
    signal buf_dq_0o4: std_logic;
    signal buf_dq_0i3: std_logic;
    signal buf_dq_0t3: std_logic;
    signal buf_dq_0o3: std_logic;
    signal buf_dq_0i2: std_logic;
    signal buf_dq_0t2: std_logic;
    signal buf_dq_0o2: std_logic;
    signal buf_dq_0i1: std_logic;
    signal buf_dq_0t1: std_logic;
    signal buf_dq_0o1: std_logic;
    signal buf_dq_0i0: std_logic;
    signal buf_dq_0t0: std_logic;
    signal buf_dq_0o0: std_logic;
    signal dqsin0: std_logic;
    signal dqsout0: std_logic;
    signal dqstrio0: std_logic;
    signal buf_dqs_0i: std_logic;
    signal buf_dqs_0t: std_logic;
    signal buf_dqs_0o: std_logic;

    -- local component declarations
    component VLO
        port (Z: out  std_logic);
    end component;
    component BB
        port (I: in  std_logic; T: in  std_logic; O: out  std_logic; 
            B: inout  std_logic);
    end component;
    component IDDRDQSX1A
        port (D: in  std_logic; DQSR90: in  std_logic; 
            DDRCLKPOL: in  std_logic; SCLK: in  std_logic; 
            RST: in  std_logic; Q0: out  std_logic; Q1: out  std_logic);
    end component;
    component ODDRDQSX1A
        port (DQSW90: in  std_logic; SCLK: in  std_logic; 
            D0: in  std_logic; D1: in  std_logic; RST: in  std_logic; 
            Q: out  std_logic);
    end component;
    component TDDRA
        generic (DQSW90_INVERT : in String);
        port (DQSW90: in  std_logic; SCLK: in  std_logic; 
            TD: in  std_logic; RST: in  std_logic; TQ: out  std_logic);
    end component;
    component DQSBUFH
        generic (LPDDR : in String);
        port (SCLK: in  std_logic; DQSI: in  std_logic; 
            READ: in  std_logic; READCLKSEL1: in  std_logic; 
            READCLKSEL0: in  std_logic; RST: in  std_logic; 
            DQSDEL: in  std_logic; DDRCLKPOL: out  std_logic; 
            DQSR90: out  std_logic; DQSW90: out  std_logic; 
            DATAVALID: out  std_logic; BURSTDET: out  std_logic);
    end component;
    component DQSDLLC
        generic (FORCE_MAX_DELAY : in String; FIN : in String; 
                LOCK_SENSITIVITY : in String);
        port (CLK: in  std_logic; RST: in  std_logic; 
            UDDCNTLN: in  std_logic; FREEZE: in  std_logic; 
            LOCK: out  std_logic; DQSDEL: out  std_logic);
    end component;
    attribute IO_TYPE : string; 
    attribute IO_TYPE of Inst2_BB7 : label is "SSTL25_I";
    attribute IO_TYPE of Inst2_BB6 : label is "SSTL25_I";
    attribute IO_TYPE of Inst2_BB5 : label is "SSTL25_I";
    attribute IO_TYPE of Inst2_BB4 : label is "SSTL25_I";
    attribute IO_TYPE of Inst2_BB3 : label is "SSTL25_I";
    attribute IO_TYPE of Inst2_BB2 : label is "SSTL25_I";
    attribute IO_TYPE of Inst2_BB1 : label is "SSTL25_I";
    attribute IO_TYPE of Inst2_BB0 : label is "SSTL25_I";
    attribute IO_TYPE of Inst1_BB : label is "SSTL25_I";
    attribute syn_keep : boolean;
    attribute NGD_DRC_MASK : integer;
    attribute NGD_DRC_MASK of Structure : architecture is 1;

begin
    -- component instantiation statements
    Inst7_TDDRA0: TDDRA
        generic map (DQSW90_INVERT=> "ENABLED")
        port map (DQSW90=>sclk, SCLK=>sclk, TD=>dqstri_0, RST=>reset, 
            TQ=>dqstrio0);

    scuba_vlo_inst: VLO
        port map (Z=>scuba_vlo);

    Inst6_ODDRDQSX1A0: ODDRDQSX1A
        port map (DQSW90=>sclk, SCLK=>sclk, D0=>scuba_vlo, D1=>dqso_0, 
            RST=>reset, Q=>dqsout0);

    Inst5_TDDRA0: TDDRA
        generic map (DQSW90_INVERT=> "DISABLED")
        port map (DQSW90=>dqsw90_0, SCLK=>sclk, TD=>datatri_0, 
            RST=>reset, TQ=>dqtri0);

    Inst_ODDRDQSX1A_0_7: ODDRDQSX1A
        port map (DQSW90=>dqsw90_0, SCLK=>sclk, D0=>da7, D1=>db7, 
            RST=>reset, Q=>dqo7);

    Inst_ODDRDQSX1A_0_6: ODDRDQSX1A
        port map (DQSW90=>dqsw90_0, SCLK=>sclk, D0=>da6, D1=>db6, 
            RST=>reset, Q=>dqo6);

    Inst_ODDRDQSX1A_0_5: ODDRDQSX1A
        port map (DQSW90=>dqsw90_0, SCLK=>sclk, D0=>da5, D1=>db5, 
            RST=>reset, Q=>dqo5);

    Inst_ODDRDQSX1A_0_4: ODDRDQSX1A
        port map (DQSW90=>dqsw90_0, SCLK=>sclk, D0=>da4, D1=>db4, 
            RST=>reset, Q=>dqo4);

    Inst_ODDRDQSX1A_0_3: ODDRDQSX1A
        port map (DQSW90=>dqsw90_0, SCLK=>sclk, D0=>da3, D1=>db3, 
            RST=>reset, Q=>dqo3);

    Inst_ODDRDQSX1A_0_2: ODDRDQSX1A
        port map (DQSW90=>dqsw90_0, SCLK=>sclk, D0=>da2, D1=>db2, 
            RST=>reset, Q=>dqo2);

    Inst_ODDRDQSX1A_0_1: ODDRDQSX1A
        port map (DQSW90=>dqsw90_0, SCLK=>sclk, D0=>da1, D1=>db1, 
            RST=>reset, Q=>dqo1);

    Inst_ODDRDQSX1A_0_0: ODDRDQSX1A
        port map (DQSW90=>dqsw90_0, SCLK=>sclk, D0=>da0, D1=>db0, 
            RST=>reset, Q=>dqo0);

    Inst_IDDRDQSX1A_0_7: IDDRDQSX1A
        port map (D=>datain_t7, DQSR90=>dqsr90_0, DDRCLKPOL=>ddrclkpol0, 
            SCLK=>sclk, RST=>reset, Q0=>qa7, Q1=>qb7);

    Inst_IDDRDQSX1A_0_6: IDDRDQSX1A
        port map (D=>datain_t6, DQSR90=>dqsr90_0, DDRCLKPOL=>ddrclkpol0, 
            SCLK=>sclk, RST=>reset, Q0=>qa6, Q1=>qb6);

    Inst_IDDRDQSX1A_0_5: IDDRDQSX1A
        port map (D=>datain_t5, DQSR90=>dqsr90_0, DDRCLKPOL=>ddrclkpol0, 
            SCLK=>sclk, RST=>reset, Q0=>qa5, Q1=>qb5);

    Inst_IDDRDQSX1A_0_4: IDDRDQSX1A
        port map (D=>datain_t4, DQSR90=>dqsr90_0, DDRCLKPOL=>ddrclkpol0, 
            SCLK=>sclk, RST=>reset, Q0=>qa4, Q1=>qb4);

    Inst_IDDRDQSX1A_0_3: IDDRDQSX1A
        port map (D=>datain_t3, DQSR90=>dqsr90_0, DDRCLKPOL=>ddrclkpol0, 
            SCLK=>sclk, RST=>reset, Q0=>qa3, Q1=>qb3);

    Inst_IDDRDQSX1A_0_2: IDDRDQSX1A
        port map (D=>datain_t2, DQSR90=>dqsr90_0, DDRCLKPOL=>ddrclkpol0, 
            SCLK=>sclk, RST=>reset, Q0=>qa2, Q1=>qb2);

    Inst_IDDRDQSX1A_0_1: IDDRDQSX1A
        port map (D=>datain_t1, DQSR90=>dqsr90_0, DDRCLKPOL=>ddrclkpol0, 
            SCLK=>sclk, RST=>reset, Q0=>qa1, Q1=>qb1);

    Inst_IDDRDQSX1A_0_0: IDDRDQSX1A
        port map (D=>datain_t0, DQSR90=>dqsr90_0, DDRCLKPOL=>ddrclkpol0, 
            SCLK=>sclk, RST=>reset, Q0=>qa0, Q1=>qb0);

    Inst4_DQSBUFH0: DQSBUFH
        generic map (LPDDR=> "DISABLED")
        port map (SCLK=>sclk, DQSI=>dqsin0, READ=>read_0, 
            READCLKSEL1=>readclksel1_0, READCLKSEL0=>readclksel0_0, 
            RST=>reset, DQSDEL=>dqsdel, DDRCLKPOL=>ddrclkpol0, 
            DQSR90=>dqsr90_0, DQSW90=>dqsw90_0, DATAVALID=>datavalid_0, 
            BURSTDET=>burstdet0);

    Inst3_DQSDLLC: DQSDLLC
        generic map (FORCE_MAX_DELAY=> "NO", FIN=> "100.0", 
        LOCK_SENSITIVITY=> "LOW")
        port map (CLK=>sclk, RST=>reset, UDDCNTLN=>uddcntln, 
            FREEZE=>freeze, LOCK=>lock, DQSDEL=>dqsdel);

    Inst2_BB7: BB
        port map (I=>buf_dq_0o7, T=>buf_dq_0t7, O=>buf_dq_0i7, 
            B=>dq_0(7));

    Inst2_BB6: BB
        port map (I=>buf_dq_0o6, T=>buf_dq_0t6, O=>buf_dq_0i6, 
            B=>dq_0(6));

    Inst2_BB5: BB
        port map (I=>buf_dq_0o5, T=>buf_dq_0t5, O=>buf_dq_0i5, 
            B=>dq_0(5));

    Inst2_BB4: BB
        port map (I=>buf_dq_0o4, T=>buf_dq_0t4, O=>buf_dq_0i4, 
            B=>dq_0(4));

    Inst2_BB3: BB
        port map (I=>buf_dq_0o3, T=>buf_dq_0t3, O=>buf_dq_0i3, 
            B=>dq_0(3));

    Inst2_BB2: BB
        port map (I=>buf_dq_0o2, T=>buf_dq_0t2, O=>buf_dq_0i2, 
            B=>dq_0(2));

    Inst2_BB1: BB
        port map (I=>buf_dq_0o1, T=>buf_dq_0t1, O=>buf_dq_0i1, 
            B=>dq_0(1));

    Inst2_BB0: BB
        port map (I=>buf_dq_0o0, T=>buf_dq_0t0, O=>buf_dq_0i0, 
            B=>dq_0(0));

    Inst1_BB: BB
        port map (I=>buf_dqs_0o, T=>buf_dqs_0t, O=>buf_dqs_0i, B=>dqs_0);

    sclk90 <= sclk;
    db7 <= dataout0(15);
    db6 <= dataout0(14);
    db5 <= dataout0(13);
    db4 <= dataout0(12);
    db3 <= dataout0(11);
    db2 <= dataout0(10);
    db1 <= dataout0(9);
    db0 <= dataout0(8);
    da7 <= dataout0(7);
    da6 <= dataout0(6);
    da5 <= dataout0(5);
    da4 <= dataout0(4);
    da3 <= dataout0(3);
    da2 <= dataout0(2);
    da1 <= dataout0(1);
    da0 <= dataout0(0);
    datain0(15) <= qb7;
    datain0(14) <= qb6;
    datain0(13) <= qb5;
    datain0(12) <= qb4;
    datain0(11) <= qb3;
    datain0(10) <= qb2;
    datain0(9) <= qb1;
    datain0(8) <= qb0;
    datain0(7) <= qa7;
    datain0(6) <= qa6;
    datain0(5) <= qa5;
    datain0(4) <= qa4;
    datain0(3) <= qa3;
    datain0(2) <= qa2;
    datain0(1) <= qa1;
    datain0(0) <= qa0;
    buf_dq_0t7 <= dqtri0;
    buf_dq_0t6 <= dqtri0;
    buf_dq_0t5 <= dqtri0;
    buf_dq_0t4 <= dqtri0;
    buf_dq_0t3 <= dqtri0;
    buf_dq_0t2 <= dqtri0;
    buf_dq_0t1 <= dqtri0;
    buf_dq_0t0 <= dqtri0;
    buf_dq_0o7 <= dqo7;
    buf_dq_0o6 <= dqo6;
    buf_dq_0o5 <= dqo5;
    buf_dq_0o4 <= dqo4;
    buf_dq_0o3 <= dqo3;
    buf_dq_0o2 <= dqo2;
    buf_dq_0o1 <= dqo1;
    buf_dq_0o0 <= dqo0;
    datain_t7 <= buf_dq_0i7;
    datain_t6 <= buf_dq_0i6;
    datain_t5 <= buf_dq_0i5;
    datain_t4 <= buf_dq_0i4;
    datain_t3 <= buf_dq_0i3;
    datain_t2 <= buf_dq_0i2;
    datain_t1 <= buf_dq_0i1;
    datain_t0 <= buf_dq_0i0;
    dqsin0 <= buf_dqs_0i;
    buf_dqs_0o <= dqsout0;
    buf_dqs_0t <= dqstrio0;
end Structure;

-- synopsys translate_off
library MACHXO2;
configuration Structure_CON of sdram is
    for Structure
        for all:VLO use entity MACHXO2.VLO(V); end for;
        for all:BB use entity MACHXO2.BB(V); end for;
        for all:IDDRDQSX1A use entity MACHXO2.IDDRDQSX1A(V); end for;
        for all:ODDRDQSX1A use entity MACHXO2.ODDRDQSX1A(V); end for;
        for all:TDDRA use entity MACHXO2.TDDRA(V); end for;
        for all:DQSBUFH use entity MACHXO2.DQSBUFH(V); end for;
        for all:DQSDLLC use entity MACHXO2.DQSDLLC(V); end for;
    end for;
end Structure_CON;

-- synopsys translate_on
