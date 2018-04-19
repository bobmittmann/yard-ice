// Verilog netlist produced by program LSE :  version Diamond (64-bit) 3.10.1.112
// Netlist written on Thu Apr 19 11:22:35 2018
//
// Verilog Description of module yard_ice
//

module yard_ice (mclk, fsmc_d, fsmc_clk, fsmc_noe, fsmc_nwe, fsmc_nce, 
            irq, uart_rx, uart_tx, tp_dbgrq, tp_dbgack, tp_tdo, 
            tp_tck, tp_tdi, tp_tms, tp_rtck, tp_trst, tp_rst, led_1, 
            led_2, ddr_addr, ddr_cke, ddr_clk, ddr_casn, ddr_rasn, 
            ddr_wen, ddr_csn0, ddr_csn1, ddr_ba, ddr_ldqm, ddr_udqm, 
            ddr_dq, ddr_ldqs, ddr_udqs, clk_aux);   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(66[8:16])
    input mclk;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    inout [15:0]fsmc_d;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(71[2:8])
    input fsmc_clk;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(73[2:10])
    input fsmc_noe;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(74[2:10])
    input fsmc_nwe;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(75[2:10])
    input fsmc_nce;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(76[2:10])
    output irq;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(78[2:5])
    output uart_rx;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(80[2:9])
    input uart_tx;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(81[2:9])
    output tp_dbgrq;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(84[2:10])
    input tp_dbgack;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(85[2:11])
    input tp_tdo;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(87[2:8])
    output tp_tck;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(88[2:8])
    output tp_tdi;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(89[2:8])
    output tp_tms;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(90[2:8])
    input tp_rtck;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(92[2:9])
    output tp_trst;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(94[2:9])
    output tp_rst;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(96[2:8])
    output led_1;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(98[2:7])
    output led_2;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(99[2:7])
    output [12:0]ddr_addr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(101[2:10])
    output ddr_cke;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(102[2:9])
    output ddr_clk;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(103[2:9])
    output ddr_casn;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(104[2:10])
    output ddr_rasn;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(105[2:10])
    output ddr_wen;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(106[2:9])
    output ddr_csn0;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(107[2:10])
    output ddr_csn1 /* synthesis black_box_pad_pin=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(108[2:10])
    output [1:0]ddr_ba;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(109[2:8])
    output ddr_ldqm;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(110[2:10])
    output ddr_udqm;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(111[2:10])
    inout [15:0]ddr_dq;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(112[2:8])
    inout ddr_ldqs /* synthesis black_box_pad_pin=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(113[2:10])
    inout ddr_udqs /* synthesis black_box_pad_pin=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(114[2:10])
    input clk_aux;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(116[2:9])
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(73[2:10])
    
    wire GND_net, VCC_net;
    wire [15:0]s_cram_dout;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(107[9:20])
    
    wire n7, n8733, n4, n3239, fsmc_noe_c, fsmc_nce_c, irq_c, 
        uart_rx_c, uart_tx_c, tp_dbgrq_c, tp_dbgack_c, tp_tdo_c, tp_tck_c, 
        tp_tdi_c, tp_tms_c, tp_rtck_c, tp_trst_c, led_1_c, led_2_c, 
        s_1mhz_stb, s_1khz_stb, n2983, n1938, n1939, n1940, n2979, 
        n8340;
    wire [1:0]s_mem_rd_sel;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(196[9:21])
    wire [15:0]s_mem_dout;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(198[9:19])
    wire [8:1]s_desc_addr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(203[9:20])
    wire [31:0]s_desc_data;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(204[9:20])
    
    wire n7870;
    wire [8:1]s_ptr_addr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(208[9:19])
    wire [15:0]s_ptr_data;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(209[9:19])
    
    wire s_ptr_mem_wr_stb;
    wire [15:0]s_aux_mem_dout;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(214[9:23])
    
    wire s_aux_mem_wr_stb;
    wire [15:0]s_reg_din;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(221[9:18])
    wire [15:0]s_reg_dout;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(222[9:19])
    wire [15:0]s_status;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(231[9:17])
    
    wire s_tap_busy;
    wire [15:0]s_irq_st;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(239[9:17])
    
    wire n2978;
    wire [15:0]s_irq_en;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(243[9:17])
    
    wire s_irq_en_wr_stb;
    wire [15:0]s_r3;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(247[9:13])
    
    wire s_r3_wr_stb;
    wire [15:0]s_ckgen_div;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(251[9:20])
    
    wire s_ckgen_div_wr_stb;
    wire [15:0]s_ckgen_rtdiv;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(255[9:22])
    
    wire s_ckgen_rtdiv_wr_stb;
    wire [15:0]s_tmr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(260[9:14])
    
    wire s_tmr_cnt_en, n7869;
    wire [15:0]s_mem_wr_addr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(268[9:22])
    
    wire n2961;
    wire [15:0]s_mem_rd_addr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(269[9:22])
    wire [15:0]s_desc_lo;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(271[9:18])
    wire [15:0]s_desc_hi;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(272[9:18])
    wire [15:0]s_cfg;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(276[9:14])
    
    wire s_cfg_wr_stb, s_tmr_cnt_en_N_1;
    wire [3:0]s_irq;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(290[9:14])
    
    wire s_tap_tdo, s_tap_tdi;
    wire [9:0]s_rx_mem_addr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(318[9:22])
    wire [15:0]s_rx_mem_din;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(320[9:21])
    
    wire s_rx_mem_wr_stb, n8345, s_dbg_mem_rd, s_dbg_mem_wr, s_ddr_burstdet0, 
        s_ddr_burstdet1, s_ddr_casn, s_ddr_dtri0, s_ddr_dtri1, s_ddr_dqs0, 
        s_ddr_dqs1, s_ddr_dqstri0, s_ddr_dqstri1, s_ddr_freeze, s_ddr_lock, 
        s_ddr_rasn, s_ddr_read0, s_ddr_read1, s_ddr_readclksel00, s_ddr_readclksel01, 
        s_ddr_readclksel10, s_ddr_readclksel11, s_ddr_uddcntln, s_ddr_wen;
    wire [12:0]s_ddr_addr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(407[9:19])
    wire [1:0]s_ddr_ba;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(408[9:17])
    
    wire s_ddr_cke;
    wire [1:0]s_ddr_csn;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(410[9:18])
    wire [15:0]s_ddr_datain0;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(411[9:22])
    wire [15:0]s_ddr_datain1;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(412[9:22])
    wire [15:0]s_ddr_dataout0;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(413[9:23])
    
    wire n8337;
    wire [3:0]irq_N_8;
    
    wire irq_N_5, n4409, s_tmr_cnt_en_N_2, s_cram_next_st_1__N_24_c_0, 
        n7922, n7864, n3001, fsmc_d_out_10, n9424, n7863, n8046, 
        n2986, n2947, n2981, n2943, n8064, n8063, n7862, n8061, 
        q_15__N_51, fsmc_d_out_9, n3004, n7739, n7737, n2964, n2968, 
        n8336, n8056, n7735, n3003, fsmc_d_out_8, n8060, n8058, 
        n2971, n3008, n14, n8057, n7_adj_1607, n8055, n8054, n1, 
        n2, n2966, n1_adj_1608, n8052, n2_adj_1609, n1_adj_1610, 
        n8051, n2969, n2_adj_1611, n1_adj_1612, fsmc_d_out_7, n2973, 
        n2944, n8050, n2959, n2_adj_1613, n1_adj_1614, n7_adj_1615, 
        n9, n8049, n8048, n8047, n8045, n8044, n2989, n2951, 
        n8043, n2_adj_1616, n3411, n3007, n2957, fsmc_d_out_6, n8042, 
        n2962, n2963, n2996, n8041, n2_adj_1617, n2967, n1_adj_1618, 
        n2_adj_1619, n1_adj_1620, n2_adj_1621, n8062, n7_adj_1622, 
        n14_adj_1623, n8040, n2988, n2949, n8739, n8039, n2958, 
        n2_adj_1624, n1_adj_1625, n2_adj_1626, fsmc_d_out_5, n2999, 
        n1_adj_1627, n2970, n2965, n2992, n2_adj_1628, n1_adj_1629, 
        n2960, n3002, fsmc_d_out_4, n2956, n2993, n2991, n2955, 
        n8029, n8059, n7_adj_1630, n8028, n1_adj_1631, n8027, n2941, 
        n3009, n2997, n3010, fsmc_d_out_3, n2995, n2_adj_1632, n3000, 
        n1_adj_1633, n3005, n8020, n2994, n3006, n8019, n6, n8018, 
        n2_adj_1634, n2990, n2953, fsmc_d_out_14, fsmc_clk_c_enable_2, 
        n3455, n8017, n8016, n8015, n8013, n8012, n2_adj_1635, 
        n1_adj_1636, n1_adj_1637, n8011, fsmc_d_out_0, n2950, n2946, 
        n2985, n8004, n2945, n2984, n2952, n8003, n8741, n7_adj_1638, 
        n7_adj_1639, fsmc_d_out_13, s_we_lo, s_we_hi, n8002, n7998, 
        n7997, n7996, n7995, n7994, n2972, n7_adj_1640, n7993, 
        n7992, n2_adj_1641, n8404, n8014, n7868, n7937, buf_ckeo0, 
        buf_csno1, buf_csno0, buf_weno, buf_casno, buf_rasno, buf_bao1, 
        buf_bao0, buf_addro12, buf_addro11, buf_addro10, buf_addro9, 
        buf_addro8, buf_addro7, buf_addro6, buf_addro5, buf_addro4, 
        buf_addro3, buf_addro2, buf_addro1, buf_addro0, buf_clkout0, 
        dqtri1, dqo15, dqo14, dqo13, dqo12, dqo11, dqo10, dqo9, 
        dqo8, datain_t15, datain_t14, datain_t13, datain_t12, datain_t11, 
        datain_t10, datain_t9, datain_t8, dqsin1, dqsout1, dqstrio1, 
        dqtri0, dqo7, dqo6, dqo5, dqo4, dqo3, dqo2, dqo1, dqo0, 
        datain_t7, datain_t6, datain_t5, datain_t4, datain_t3, datain_t2, 
        datain_t1, datain_t0, dqsin0, dqsout0, dqstrio0, q_15__N_82, 
        n1_adj_1642, n2_adj_1643, q_15__N_80, n1_adj_1644, q_15__N_78, 
        q_15__N_76, n2954, q_15__N_74, q_15__N_72, n9428, q_15__N_70, 
        q_15__N_68, q_15__N_66, q_15__N_64, q_15__N_62, n2948, n2987, 
        n8053, q_15__N_56, q_15__N_52, n9426;
    wire [3:0]s_addr0;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(63[9:16])
    wire [3:0]s_addr1_adj_1783;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(64[9:16])
    wire [3:0]s_addr2_adj_1784;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(65[9:16])
    wire [3:0]s_addr3;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(66[9:16])
    
    wire n8738, n8736, n7940, n7_adj_1645, n7946, n5312, out_get_N_312, 
        n7_adj_1646, n4442, n7957, n7956, n7955, fsmc_d_out_15, 
        n7_adj_1647, n14_adj_1648, n14_adj_1649, n7_adj_1650;
    wire [9:0]s_addr_0;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(77[9:17])
    wire [3:0]s_pos_0_3__N_670;
    
    wire n7948, n7947, n8398, n7942, n8877, n7941, n7939, n7938, 
        n7930, n7929, n7928, n8737, n7924, n9425, n7923, n9419, 
        n3236, n7769, n8863, n8862, n9420, n8856, fsmc_d_out_11, 
        n9423, n8845, n8432, n8431, n8430, n8425, n8424, n8423, 
        n8843, n2998;
    wire [3:0]s_goto_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(196[9:21])
    
    wire n2942, n8838;
    wire [3:0]s_end_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(227[9:20])
    
    wire n8837, n8405, fsmc_d_out_12, fsmc_d_out_1, n8833, n8399, 
        n8396, n8832, n8395, fsmc_d_out_2, n8740, n9422, n9421, 
        n8390, n8388, n8387, n8731, mclk_c_enable_81, n2982, n8829, 
        n8828, n8384, n2980, n8383, n2_adj_1651, n8827, n1_adj_1652, 
        n8726, s_cnt_en_N_1514, n7761, n24, mclk_c_enable_112, n7_adj_1653, 
        n13, n8820, n8373, n7781, n20, n7680, n6_adj_1654, n8372, 
        n11, n7779, n8389, n7704, n8366, n8365, n7767, n20_adj_1655, 
        n7703, n11_adj_1656, n8735, n8362, n8361, n8342, n19, 
        n17, n22, fsmc_clk_c_enable_1, n8346, n8343, n8341, n7771;
    
    VHI i2 (.Z(VCC_net));
    FD1S3AX s_irq_out_66 (.D(irq_N_5), .CK(mclk_c), .Q(irq_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(636[3] 638[10])
    defparam s_irq_out_66.GSR = "DISABLED";
    FD1S3AX s_tmr_cnt_en_67 (.D(n4409), .CK(mclk_c), .Q(s_tmr_cnt_en));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(709[3] 717[10])
    defparam s_tmr_cnt_en_67.GSR = "ENABLED";
    FD1S3AX s_irq_reg_i1 (.D(n8856), .CK(mclk_c), .Q(s_irq_st[0]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(603[3] 605[10])
    defparam s_irq_reg_i1.GSR = "DISABLED";
    ddr_sdram_dual ddr_ctl (.clk_i(mclk_c), .rst_i(n9428), .cmd_stb_i(GND_net), 
            .cmd_dat_i({GND_net, GND_net, GND_net, GND_net, GND_net, 
            GND_net, GND_net, GND_net, GND_net, GND_net, GND_net, 
            GND_net, GND_net, GND_net, GND_net, GND_net}), .mwr_cyc_i(GND_net), 
            .mwr_we_i(GND_net), .mwr_stb_i(GND_net), .mwr_adr_i({GND_net, 
            GND_net, GND_net, GND_net, GND_net, GND_net, GND_net, 
            GND_net, GND_net, GND_net, GND_net, GND_net, GND_net, 
            GND_net, GND_net, GND_net, GND_net, GND_net, GND_net, 
            GND_net, GND_net}), .mwr_dat_i({GND_net, GND_net, GND_net, 
            GND_net, GND_net, GND_net, GND_net, GND_net, GND_net, 
            GND_net, GND_net, GND_net, GND_net, GND_net, GND_net, 
            GND_net, GND_net, GND_net, GND_net, GND_net, GND_net, 
            GND_net, GND_net, GND_net, GND_net, GND_net, GND_net, 
            GND_net, GND_net, GND_net, GND_net, GND_net}), .mwr_bst_i(GND_net), 
            .mrd_cyc_i(GND_net), .mrd_we_i(GND_net), .mrd_stb_i(GND_net), 
            .mrd_adr_i({GND_net, GND_net, GND_net, GND_net, GND_net, 
            GND_net, GND_net, GND_net, GND_net, GND_net, GND_net, 
            GND_net, GND_net, GND_net, GND_net, GND_net, GND_net, 
            GND_net, GND_net, GND_net, GND_net}), .mrd_bst_i(GND_net), 
            .phy_burstdet0_i(s_ddr_burstdet0), .phy_burstdet1_i(s_ddr_burstdet1), 
            .phy_casn_o(s_ddr_casn), .phy_dtri0_o(s_ddr_dtri0), .phy_dtri1_o(s_ddr_dtri1), 
            .phy_dqs0_o(s_ddr_dqs0), .phy_dqs1_o(s_ddr_dqs1), .phy_dqstri0_o(s_ddr_dqstri0), 
            .phy_dqstri1_o(s_ddr_dqstri1), .phy_freeze_o(s_ddr_freeze), 
            .phy_lock_i(s_ddr_lock), .phy_rasn_o(s_ddr_rasn), .phy_read0_o(s_ddr_read0), 
            .phy_read1_o(s_ddr_read1), .phy_readclksel00_o(s_ddr_readclksel00), 
            .phy_readclksel01_o(s_ddr_readclksel01), .phy_readclksel10_o(s_ddr_readclksel10), 
            .phy_readclksel11_o(s_ddr_readclksel11), .phy_uddcntln_o(s_ddr_uddcntln), 
            .phy_wen_o(s_ddr_wen), .phy_addr_o({s_ddr_addr}), .phy_ba_o({s_ddr_ba}), 
            .phy_cke_o(s_ddr_cke), .phy_csn0_o(s_ddr_csn[0]), .phy_csn1_o(s_ddr_csn[1]), 
            .phy_dat0_i({s_ddr_datain0}), .phy_dat1_i({s_ddr_datain1}), 
            .phy_dat0_o({s_ddr_dataout0}));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1107[12:33])
    PFUMX i5972 (.BLUT(n8027), .ALUT(n8028), .C0(q_15__N_80), .Z(n8029));
    \jtag_led_drv(31,false,false,false)  led_run_drv (.mclk_c(mclk_c), .led_1_c(led_1_c), 
            .out_get_N_312(out_get_N_312), .s_1khz_stb(s_1khz_stb));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1049[16:35])
    IB tp_rtck_pad (.I(tp_rtck), .O(tp_rtck_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(92[2:9])
    IB tp_tdo_pad (.I(tp_tdo), .O(tp_tdo_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(87[2:8])
    IB tp_dbgack_pad (.I(tp_dbgack), .O(tp_dbgack_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(85[2:11])
    IB uart_tx_pad (.I(uart_tx), .O(uart_tx_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(81[2:9])
    IB fsmc_nce_pad (.I(fsmc_nce), .O(fsmc_nce_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(76[2:10])
    IB s_cram_next_st_1__N_24_pad_0 (.I(fsmc_nwe), .O(s_cram_next_st_1__N_24_c_0));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(75[2:10])
    IB fsmc_noe_pad (.I(fsmc_noe), .O(fsmc_noe_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(74[2:10])
    IB fsmc_clk_pad (.I(fsmc_clk), .O(fsmc_clk_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(73[2:10])
    IB mclk_pad (.I(mclk), .O(mclk_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    OB ddr_udqm_pad (.I(GND_net), .O(ddr_udqm));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(111[2:10])
    OB ddr_ldqm_pad (.I(GND_net), .O(ddr_ldqm));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(110[2:10])
    OB led_2_pad (.I(led_2_c), .O(led_2));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(99[2:7])
    OB led_1_pad (.I(led_1_c), .O(led_1));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(98[2:7])
    OBZ tp_rst_pad (.I(GND_net), .T(n3236), .O(tp_rst));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(683[2:12])
    OB tp_trst_pad (.I(tp_trst_c), .O(tp_trst));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(94[2:9])
    OB tp_tms_pad (.I(tp_tms_c), .O(tp_tms));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(90[2:8])
    OB tp_tdi_pad (.I(tp_tdi_c), .O(tp_tdi));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(89[2:8])
    OB tp_tck_pad (.I(tp_tck_c), .O(tp_tck));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(88[2:8])
    OB tp_dbgrq_pad (.I(tp_dbgrq_c), .O(tp_dbgrq));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(84[2:10])
    OB uart_rx_pad (.I(uart_rx_c), .O(uart_rx));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(80[2:9])
    OB irq_pad (.I(irq_c), .O(irq));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(78[2:5])
    BB fsmc_d_pad_0 (.I(s_cram_dout[0]), .T(n3239), .B(fsmc_d[0]), .O(fsmc_d_out_0));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_1 (.I(s_cram_dout[1]), .T(n3239), .B(fsmc_d[1]), .O(fsmc_d_out_1));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_2 (.I(s_cram_dout[2]), .T(n3239), .B(fsmc_d[2]), .O(fsmc_d_out_2));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_3 (.I(s_cram_dout[3]), .T(n3239), .B(fsmc_d[3]), .O(fsmc_d_out_3));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_4 (.I(s_cram_dout[4]), .T(n3239), .B(fsmc_d[4]), .O(fsmc_d_out_4));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_5 (.I(s_cram_dout[5]), .T(n3239), .B(fsmc_d[5]), .O(fsmc_d_out_5));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_6 (.I(s_cram_dout[6]), .T(n3239), .B(fsmc_d[6]), .O(fsmc_d_out_6));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    PFUMX i5987 (.BLUT(n8041), .ALUT(n8042), .C0(q_15__N_80), .Z(n8044));
    BB fsmc_d_pad_7 (.I(s_cram_dout[7]), .T(n3239), .B(fsmc_d[7]), .O(fsmc_d_out_7));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_8 (.I(s_cram_dout[8]), .T(n3239), .B(fsmc_d[8]), .O(fsmc_d_out_8));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    PFUMX i5807 (.BLUT(n7862), .ALUT(n7863), .C0(q_15__N_80), .Z(n7864));
    BB fsmc_d_pad_9 (.I(s_cram_dout[9]), .T(n3239), .B(fsmc_d[9]), .O(fsmc_d_out_9));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_10 (.I(s_cram_dout[10]), .T(n3239), .B(fsmc_d[10]), 
       .O(fsmc_d_out_10));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_11 (.I(s_cram_dout[11]), .T(n3239), .B(fsmc_d[11]), 
       .O(fsmc_d_out_11));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_12 (.I(s_cram_dout[12]), .T(n3239), .B(fsmc_d[12]), 
       .O(fsmc_d_out_12));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    PFUMX i5993 (.BLUT(n8046), .ALUT(n8047), .C0(q_15__N_80), .Z(n8050));
    ROM256X1A mux_178_Mux_0 (.AD0(s_status[0]), .AD1(s_status[1]), .AD2(s_status[2]), 
            .AD3(s_status[3]), .AD4(s_end_state[0]), .AD5(s_end_state[1]), 
            .AD6(s_end_state[2]), .AD7(s_end_state[3]), .DO0(s_goto_state[0])) /* synthesis initstate=0xF7773767776747577F6733677767766777F7773777777767777F773377777776 */ ;
    defparam mux_178_Mux_0.initval = 256'hF7773767776747577F6733677767766777F7773777777767777F773377777776;
    ROM256X1A mux_178_Mux_1 (.AD0(s_status[0]), .AD1(s_status[1]), .AD2(s_status[2]), 
            .AD3(s_status[3]), .AD4(s_end_state[0]), .AD5(s_end_state[1]), 
            .AD6(s_end_state[2]), .AD7(s_end_state[3]), .DO0(s_goto_state[1])) /* synthesis initstate=0xB0307030103000103A3274303030323230B030F03090302030BA30F430B030B2 */ ;
    defparam mux_178_Mux_1.initval = 256'hB0307030103000103A3274303030323230B030F03090302030BA30F430B030B2;
    LUT4 i5959_3_lut (.A(s_ckgen_div[3]), .B(s_ckgen_rtdiv[3]), .C(q_15__N_82), 
         .Z(n8016)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5959_3_lut.init = 16'hcaca;
    LUT4 i5958_3_lut (.A(s_irq_en[3]), .B(s_r3[3]), .C(q_15__N_82), .Z(n8015)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5958_3_lut.init = 16'hcaca;
    LUT4 i5957_3_lut (.A(s_status[3]), .B(s_irq_st[3]), .C(q_15__N_82), 
         .Z(n8014)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5957_3_lut.init = 16'hcaca;
    LUT4 i5955_3_lut (.A(s_desc_lo[2]), .B(s_desc_hi[2]), .C(q_15__N_82), 
         .Z(n8012)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5955_3_lut.init = 16'hcaca;
    LUT4 i5954_3_lut (.A(s_mem_wr_addr[2]), .B(s_mem_rd_addr[2]), .C(q_15__N_82), 
         .Z(n8011)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5954_3_lut.init = 16'hcaca;
    PFUMX i6 (.BLUT(s_cnt_en_N_1514), .ALUT(n1_adj_1637), .C0(n7781), 
          .Z(n2_adj_1634));
    LUT4 i5946_3_lut (.A(s_desc_lo[3]), .B(s_desc_hi[3]), .C(q_15__N_82), 
         .Z(n8003)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5946_3_lut.init = 16'hcaca;
    LUT4 i5945_3_lut (.A(s_mem_wr_addr[3]), .B(s_mem_rd_addr[3]), .C(q_15__N_82), 
         .Z(n8002)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5945_3_lut.init = 16'hcaca;
    LUT4 i5938_3_lut (.A(s_cfg[0]), .B(s_tmr[0]), .C(q_15__N_82), .Z(n7995)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5938_3_lut.init = 16'hcaca;
    LUT4 i2343_1_lut_2_lut (.A(out_get_N_312), .B(n8341), .Z(n4442)) /* synthesis lut_function=(!(A (B))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(584[18] 585[44])
    defparam i2343_1_lut_2_lut.init = 16'h7777;
    LUT4 i5937_3_lut (.A(s_ckgen_div[0]), .B(s_ckgen_rtdiv[0]), .C(q_15__N_82), 
         .Z(n7994)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5937_3_lut.init = 16'hcaca;
    PFUMX i5994 (.BLUT(n8048), .ALUT(n8049), .C0(q_15__N_80), .Z(n8051));
    \jtag_reg(16,12)  mem_wr_addr (.\s_mem_wr_addr[1] (s_mem_wr_addr[1]), 
            .fsmc_clk_c(fsmc_clk_c), .s_dbg_mem_wr(s_dbg_mem_wr), .q_15__N_82(q_15__N_82), 
            .\s_mem_wr_addr[11] (s_mem_wr_addr[11]), .q_15__N_62(q_15__N_62), 
            .\s_mem_wr_addr[10] (s_mem_wr_addr[10]), .q_15__N_64(q_15__N_64), 
            .\s_mem_wr_addr[9] (s_mem_wr_addr[9]), .q_15__N_66(q_15__N_66), 
            .\s_mem_wr_addr[8] (s_mem_wr_addr[8]), .q_15__N_68(q_15__N_68), 
            .\s_mem_wr_addr[7] (s_mem_wr_addr[7]), .q_15__N_70(q_15__N_70), 
            .\s_mem_wr_addr[6] (s_mem_wr_addr[6]), .q_15__N_72(q_15__N_72), 
            .\s_mem_wr_addr[5] (s_mem_wr_addr[5]), .q_15__N_74(q_15__N_74), 
            .\s_mem_wr_addr[4] (s_mem_wr_addr[4]), .q_15__N_76(q_15__N_76), 
            .\s_mem_wr_addr[3] (s_mem_wr_addr[3]), .q_15__N_78(q_15__N_78), 
            .\s_mem_wr_addr[2] (s_mem_wr_addr[2]), .q_15__N_80(q_15__N_80));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(744[16:31])
    \jtag_reg(16,16)_U4  desc_lo (.s_desc_lo({s_desc_lo}), .mclk_c(mclk_c), 
            .mclk_c_enable_112(mclk_c_enable_112), .\s_desc_data[9] (s_desc_data[9]), 
            .\s_desc_data[8] (s_desc_data[8]), .\s_desc_data[7] (s_desc_data[7]), 
            .\s_desc_data[6] (s_desc_data[6]), .\s_desc_data[5] (s_desc_data[5]), 
            .\s_desc_data[4] (s_desc_data[4]), .s_pos_0_3__N_670({s_pos_0_3__N_670}), 
            .\s_desc_data[12] (s_desc_data[12]), .\s_desc_data[11] (s_desc_data[11]), 
            .\s_desc_data[15] (s_desc_data[15]), .\s_desc_data[10] (s_desc_data[10]), 
            .\s_desc_data[14] (s_desc_data[14]), .\s_desc_data[13] (s_desc_data[13]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(774[12:27])
    LUT4 i5936_3_lut (.A(s_irq_en[0]), .B(s_r3[0]), .C(q_15__N_82), .Z(n7993)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5936_3_lut.init = 16'hcaca;
    LUT4 i5935_3_lut (.A(s_status[0]), .B(s_irq_st[0]), .C(q_15__N_82), 
         .Z(n7992)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5935_3_lut.init = 16'hcaca;
    OB Inst23_OB1 (.I(buf_csno1), .O(ddr_csn1)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(363[17:19])
    OB Inst23_OB0 (.I(buf_csno0), .O(ddr_csn0)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(366[17:19])
    OB Inst21_OB (.I(buf_weno), .O(ddr_wen)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(377[16:18])
    OB Inst19_OB (.I(buf_casno), .O(ddr_casn)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(384[16:18])
    OB Inst17_OB (.I(buf_rasno), .O(ddr_rasn)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(391[16:18])
    OB Inst15_OB1 (.I(buf_bao1), .O(ddr_ba[1])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(398[17:19])
    OB Inst15_OB0 (.I(buf_bao0), .O(ddr_ba[0])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(401[17:19])
    OB Inst13_OB12 (.I(buf_addro12), .O(ddr_addr[12])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(412[18:20])
    OB Inst13_OB11 (.I(buf_addro11), .O(ddr_addr[11])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(415[18:20])
    OB Inst13_OB10 (.I(buf_addro10), .O(ddr_addr[10])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(418[18:20])
    OB Inst13_OB9 (.I(buf_addro9), .O(ddr_addr[9])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(421[17:19])
    OB Inst13_OB8 (.I(buf_addro8), .O(ddr_addr[8])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(424[17:19])
    OB Inst13_OB7 (.I(buf_addro7), .O(ddr_addr[7])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(427[17:19])
    OB Inst13_OB6 (.I(buf_addro6), .O(ddr_addr[6])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(430[17:19])
    OB Inst13_OB5 (.I(buf_addro5), .O(ddr_addr[5])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(433[17:19])
    OB Inst13_OB4 (.I(buf_addro4), .O(ddr_addr[4])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(436[17:19])
    OB Inst13_OB3 (.I(buf_addro3), .O(ddr_addr[3])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(439[17:19])
    OB Inst13_OB2 (.I(buf_addro2), .O(ddr_addr[2])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(442[17:19])
    OB Inst13_OB1 (.I(buf_addro1), .O(ddr_addr[1])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(445[17:19])
    OB Inst13_OB0 (.I(buf_addro0), .O(ddr_addr[0])) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(448[17:19])
    OB Inst11_OB0 (.I(buf_clkout0), .O(ddr_clk)) /* synthesis IO_TYPE="SSTL25D_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(503[17:19])
    \cram_adaptor(2,12,4)  cram_bus (.s_mem_rd_sel({s_mem_rd_sel}), .fsmc_clk_c(fsmc_clk_c), 
            .q_15__N_56(q_15__N_56), .s_reg_dout({s_reg_dout[15:4], Open_0, 
            Open_1, Open_2, s_reg_dout[0]}), .fsmc_nce_c(fsmc_nce_c), 
            .n9419(n9419), .s_cram_next_st_1__N_24_c_0(s_cram_next_st_1__N_24_c_0), 
            .s_dbg_mem_rd(s_dbg_mem_rd), .s_mem_dout({s_mem_dout}), .s_cram_dout({s_cram_dout}), 
            .fsmc_d_out_15(fsmc_d_out_15), .fsmc_noe_c(fsmc_noe_c), .n3239(n3239), 
            .q_15__N_52(q_15__N_52), .n9420(n9420), .s_ptr_mem_wr_stb(s_ptr_mem_wr_stb), 
            .s_aux_mem_wr_stb(s_aux_mem_wr_stb), .n2973(n2973), .n2972(n2972), 
            .n2956(n2956), .n1(n1_adj_1644), .n2971(n2971), .n2955(n2955), 
            .n1_adj_1(n1_adj_1642), .n2970(n2970), .n2954(n2954), .n1_adj_2(n1_adj_1636), 
            .n2969(n2969), .n2953(n2953), .n1_adj_3(n1_adj_1633), .n2968(n2968), 
            .n2952(n2952), .n1_adj_4(n1_adj_1629), .n2967(n2967), .n2951(n2951), 
            .n1_adj_5(n1_adj_1627), .n2966(n2966), .n2950(n2950), .n1_adj_6(n1_adj_1625), 
            .n2965(n2965), .n2949(n2949), .n1_adj_7(n1_adj_1631), .n2964(n2964), 
            .n2948(n2948), .n1_adj_8(n1_adj_1620), .n2963(n2963), .n2947(n2947), 
            .n1_adj_9(n1_adj_1618), .n2962(n2962), .n2946(n2946), .n1_adj_10(n1_adj_1614), 
            .n2961(n2961), .n2945(n2945), .n1_adj_11(n1_adj_1612), .n2960(n2960), 
            .n2944(n2944), .n1_adj_12(n1_adj_1610), .n2959(n2959), .n2943(n2943), 
            .n1_adj_13(n1_adj_1608), .n2958(n2958), .n2942(n2942), .n1_adj_14(n1), 
            .n2957(n2957), .n2941(n2941), .n1_adj_15(n1_adj_1652), .n9428(n9428), 
            .n8843(n8843), .s_dbg_mem_wr(s_dbg_mem_wr), .mclk_c(mclk_c), 
            .out_get_N_312(out_get_N_312), .n1938(n1938), .\s_addr0[0] (s_addr0[0]), 
            .q_15__N_82(q_15__N_82), .\s_addr1[0] (s_addr1_adj_1783[0]), 
            .s_addr2({s_addr2_adj_1784}), .s_addr3({s_addr3}), .fsmc_d_out_0(fsmc_d_out_0), 
            .n1940(n1940), .n1939(n1939), .n8862(n8862), .n8838(n8838), 
            .n8726(n8726), .n8387(n8387), .n8731(n8731), .n8365(n8365), 
            .n8733(n8733), .n8340(n8340), .\s_addr0[1] (s_addr0[1]), .q_15__N_80(q_15__N_80), 
            .q_15__N_78(q_15__N_78), .q_15__N_76(q_15__N_76), .\s_addr1[1] (s_addr1_adj_1783[1]), 
            .fsmc_d_out_1(fsmc_d_out_1), .fsmc_d_out_2(fsmc_d_out_2), .fsmc_d_out_3(fsmc_d_out_3), 
            .fsmc_d_out_4(fsmc_d_out_4), .fsmc_d_out_5(fsmc_d_out_5), .fsmc_d_out_6(fsmc_d_out_6), 
            .fsmc_d_out_7(fsmc_d_out_7), .fsmc_d_out_8(fsmc_d_out_8), .fsmc_d_out_9(fsmc_d_out_9), 
            .fsmc_d_out_10(fsmc_d_out_10), .fsmc_d_out_11(fsmc_d_out_11), 
            .fsmc_d_out_12(fsmc_d_out_12), .fsmc_d_out_13(fsmc_d_out_13), 
            .fsmc_d_out_14(fsmc_d_out_14), .n8863(n8863), .n20(n20), .n22(n22), 
            .s_reg_din({s_reg_din}), .n9421(n9421), .q_15__N_74(q_15__N_74), 
            .q_15__N_72(q_15__N_72), .q_15__N_70(q_15__N_70), .q_15__N_68(q_15__N_68), 
            .q_15__N_66(q_15__N_66), .q_15__N_64(q_15__N_64), .q_15__N_62(q_15__N_62), 
            .n9426(n9426), .\s_tmr[6] (s_tmr[6]), .\s_r3[6] (s_r3[6]), 
            .n8389(n8389), .n8390(n8390), .n8741(n8741), .n8004(n8004), 
            .n8020(n8020), .n8013(n8013), .n8045(n8045), .n8395(n8395), 
            .n8396(n8396), .n8740(n8740), .n8398(n8398), .n8399(n8399), 
            .n8739(n8739), .n8404(n8404), .n8405(n8405), .n8738(n8738), 
            .n8029(n8029), .n8052(n8052), .n8342(n8342), .n8343(n8343), 
            .n8737(n8737), .n8345(n8345), .n8346(n8346), .n8736(n8736), 
            .n8372(n8372), .n8373(n8373), .n8735(n8735), .n7942(n7942), 
            .n7(n7), .n9425(n9425), .n9424(n9424), .n9423(n9423), .n9422(n9422));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(447[13:32])
    BB Inst4_BB7 (.I(dqo15), .T(dqtri1), .B(ddr_dq[15]), .O(datain_t15)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(694[16:18])
    BB Inst4_BB6 (.I(dqo14), .T(dqtri1), .B(ddr_dq[14]), .O(datain_t14)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(698[16:18])
    OB Inst25_OB0 (.I(buf_ckeo0), .O(ddr_cke)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(356[17:19])
    BB Inst4_BB5 (.I(dqo13), .T(dqtri1), .B(ddr_dq[13]), .O(datain_t13)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(702[16:18])
    BB Inst4_BB4 (.I(dqo12), .T(dqtri1), .B(ddr_dq[12]), .O(datain_t12)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(706[16:18])
    BB Inst4_BB3 (.I(dqo11), .T(dqtri1), .B(ddr_dq[11]), .O(datain_t11)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(710[16:18])
    BB Inst4_BB2 (.I(dqo10), .T(dqtri1), .B(ddr_dq[10]), .O(datain_t10)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(714[16:18])
    BB Inst4_BB1 (.I(dqo9), .T(dqtri1), .B(ddr_dq[9]), .O(datain_t9)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(718[16:18])
    BB Inst4_BB0 (.I(dqo8), .T(dqtri1), .B(ddr_dq[8]), .O(datain_t8)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(722[16:18])
    BB Inst3_BB (.I(dqsout1), .T(dqstrio1), .B(ddr_udqs), .O(dqsin1)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(726[15:17])
    BB Inst2_BB7 (.I(dqo7), .T(dqtri0), .B(ddr_dq[7]), .O(datain_t7)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(729[16:18])
    BB Inst2_BB6 (.I(dqo6), .T(dqtri0), .B(ddr_dq[6]), .O(datain_t6)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(733[16:18])
    BB Inst2_BB5 (.I(dqo5), .T(dqtri0), .B(ddr_dq[5]), .O(datain_t5)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(737[16:18])
    BB Inst2_BB4 (.I(dqo4), .T(dqtri0), .B(ddr_dq[4]), .O(datain_t4)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(741[16:18])
    BB Inst2_BB3 (.I(dqo3), .T(dqtri0), .B(ddr_dq[3]), .O(datain_t3)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(745[16:18])
    BB Inst2_BB2 (.I(dqo2), .T(dqtri0), .B(ddr_dq[2]), .O(datain_t2)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(749[16:18])
    BB Inst2_BB1 (.I(dqo1), .T(dqtri0), .B(ddr_dq[1]), .O(datain_t1)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(753[16:18])
    BB Inst2_BB0 (.I(dqo0), .T(dqtri0), .B(ddr_dq[0]), .O(datain_t0)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(757[16:18])
    BB Inst1_BB (.I(dqsout0), .T(dqstrio0), .B(ddr_ldqs), .O(dqsin0)) /* synthesis IO_TYPE="SSTL25_I", syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(761[15:17])
    PFUMX i36 (.BLUT(n17), .ALUT(n7704), .C0(n1938), .Z(n19));
    LUT4 i5899_3_lut (.A(s_cfg[11]), .B(s_tmr[11]), .C(q_15__N_82), .Z(n7956)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5899_3_lut.init = 16'hcaca;
    LUT4 i5898_3_lut (.A(s_ckgen_div[11]), .B(s_ckgen_rtdiv[11]), .C(q_15__N_82), 
         .Z(n7955)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5898_3_lut.init = 16'hcaca;
    PFUMX i5998 (.BLUT(n8053), .ALUT(n8054), .C0(q_15__N_80), .Z(n8055));
    LUT4 i5890_3_lut (.A(s_cfg[10]), .B(s_tmr[10]), .C(q_15__N_82), .Z(n7947)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5890_3_lut.init = 16'hcaca;
    LUT4 i5889_3_lut (.A(s_ckgen_div[10]), .B(s_ckgen_rtdiv[10]), .C(q_15__N_82), 
         .Z(n7946)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5889_3_lut.init = 16'hcaca;
    LUT4 i5884_3_lut (.A(s_desc_lo[6]), .B(s_desc_hi[6]), .C(q_15__N_82), 
         .Z(n7941)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5884_3_lut.init = 16'hcaca;
    LUT4 i5883_3_lut (.A(s_mem_wr_addr[6]), .B(s_mem_rd_addr[6]), .C(q_15__N_82), 
         .Z(n7940)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5883_3_lut.init = 16'hcaca;
    LUT4 i5881_3_lut (.A(s_cfg[9]), .B(s_tmr[9]), .C(q_15__N_82), .Z(n7938)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5881_3_lut.init = 16'hcaca;
    LUT4 i5880_3_lut (.A(s_ckgen_div[9]), .B(s_ckgen_rtdiv[9]), .C(q_15__N_82), 
         .Z(n7937)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5880_3_lut.init = 16'hcaca;
    PFUMX i6001 (.BLUT(n8056), .ALUT(n8057), .C0(q_15__N_80), .Z(n8058));
    LUT4 n1978_bdd_4_lut (.A(n8863), .B(n24), .C(n8337), .D(n1939), 
         .Z(n8733)) /* synthesis lut_function=(A (C (D))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam n1978_bdd_4_lut.init = 16'hf044;
    LUT4 i5872_3_lut (.A(s_cfg[5]), .B(s_tmr[5]), .C(q_15__N_82), .Z(n7929)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5872_3_lut.init = 16'hcaca;
    LUT4 i5871_3_lut (.A(s_ckgen_div[5]), .B(s_ckgen_rtdiv[5]), .C(q_15__N_82), 
         .Z(n7928)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5871_3_lut.init = 16'hcaca;
    \jtag_reg(16,12)_U2  mem_rd_addr (.\s_mem_rd_addr[11] (s_mem_rd_addr[11]), 
            .fsmc_clk_c(fsmc_clk_c), .s_dbg_mem_rd(s_dbg_mem_rd), .q_15__N_62(q_15__N_62), 
            .\s_mem_rd_addr[10] (s_mem_rd_addr[10]), .q_15__N_64(q_15__N_64), 
            .\s_mem_rd_addr[9] (s_mem_rd_addr[9]), .q_15__N_66(q_15__N_66), 
            .\s_mem_rd_addr[8] (s_mem_rd_addr[8]), .q_15__N_68(q_15__N_68), 
            .\s_mem_rd_addr[7] (s_mem_rd_addr[7]), .q_15__N_70(q_15__N_70), 
            .\s_mem_rd_addr[6] (s_mem_rd_addr[6]), .q_15__N_72(q_15__N_72), 
            .\s_mem_rd_addr[5] (s_mem_rd_addr[5]), .q_15__N_74(q_15__N_74), 
            .\s_mem_rd_addr[4] (s_mem_rd_addr[4]), .q_15__N_76(q_15__N_76), 
            .\s_mem_rd_addr[3] (s_mem_rd_addr[3]), .q_15__N_78(q_15__N_78), 
            .\s_mem_rd_addr[2] (s_mem_rd_addr[2]), .q_15__N_80(q_15__N_80), 
            .\s_mem_rd_addr[1] (s_mem_rd_addr[1]), .q_15__N_82(q_15__N_82));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(759[16:31])
    \counter(16,16)_U0  tmr_r (.s_tmr({s_tmr}), .mclk_c(mclk_c), .s_reg_din({s_reg_din}), 
            .s_tmr_cnt_en_N_2(s_tmr_cnt_en_N_2), .n8845(n8845), .n8827(n8827), 
            .q_15__N_51(q_15__N_51), .n8877(n8877));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(688[10:24])
    LUT4 s_desc_lo_11__bdd_3_lut_6401 (.A(s_desc_lo[11]), .B(q_15__N_82), 
         .C(s_desc_hi[11]), .Z(n8389)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_11__bdd_3_lut_6401.init = 16'he2e2;
    LUT4 i5866_3_lut (.A(s_cfg[12]), .B(s_tmr[12]), .C(q_15__N_82), .Z(n7923)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5866_3_lut.init = 16'hcaca;
    LUT4 i5865_3_lut (.A(s_ckgen_div[12]), .B(s_ckgen_rtdiv[12]), .C(q_15__N_82), 
         .Z(n7922)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5865_3_lut.init = 16'hcaca;
    LUT4 s_desc_lo_11__bdd_3_lut (.A(s_mem_wr_addr[11]), .B(s_mem_rd_addr[11]), 
         .C(q_15__N_82), .Z(n8390)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_11__bdd_3_lut.init = 16'hcaca;
    PFUMX i6381 (.BLUT(n8365), .ALUT(n8361), .C0(n1938), .Z(n8366));
    LUT4 i5983_3_lut (.A(s_irq_en[2]), .B(s_r3[2]), .C(q_15__N_82), .Z(n8040)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5983_3_lut.init = 16'hcaca;
    PFUMX i6004 (.BLUT(n8059), .ALUT(n8060), .C0(q_15__N_80), .Z(n8061));
    LUT4 s_desc_lo_10__bdd_3_lut (.A(s_mem_wr_addr[10]), .B(s_mem_rd_addr[10]), 
         .C(q_15__N_82), .Z(n8396)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_10__bdd_3_lut.init = 16'hcaca;
    LUT4 s_desc_lo_10__bdd_3_lut_6406 (.A(s_desc_lo[10]), .B(q_15__N_82), 
         .C(s_desc_hi[10]), .Z(n8395)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_10__bdd_3_lut_6406.init = 16'he2e2;
    LUT4 i1_2_lut_3_lut_4_lut (.A(s_addr3[3]), .B(s_addr3[0]), .C(s_addr3[2]), 
         .D(s_addr3[1]), .Z(n7703)) /* synthesis lut_function=(!(A+((C+!(D))+!B))) */ ;
    defparam i1_2_lut_3_lut_4_lut.init = 16'h0400;
    LUT4 i1_2_lut_4_lut (.A(n8843), .B(q_15__N_56), .C(q_15__N_52), .D(q_15__N_82), 
         .Z(s_we_hi)) /* synthesis lut_function=(!(((C+!(D))+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(485[23:69])
    defparam i1_2_lut_4_lut.init = 16'h0800;
    LUT4 s_desc_lo_7__bdd_3_lut_6409 (.A(s_desc_lo[7]), .B(q_15__N_82), 
         .C(s_desc_hi[7]), .Z(n8398)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_7__bdd_3_lut_6409.init = 16'he2e2;
    PFUMX s_reg_rd_sel_3__I_0_Mux_6_i15 (.BLUT(n4), .ALUT(n7), .C0(n7779), 
          .Z(s_reg_dout[6]));
    LUT4 s_desc_lo_7__bdd_3_lut (.A(s_mem_wr_addr[7]), .B(s_mem_rd_addr[7]), 
         .C(q_15__N_82), .Z(n8399)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_7__bdd_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_4_lut_adj_205 (.A(n8843), .B(q_15__N_56), .C(q_15__N_52), 
         .D(q_15__N_82), .Z(s_we_lo)) /* synthesis lut_function=(!(((C+(D))+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(485[23:69])
    defparam i1_2_lut_4_lut_adj_205.init = 16'h0008;
    PFUMX i49 (.BLUT(n7767), .ALUT(n7761), .C0(n1939), .Z(n7769));
    LUT4 i6008_4_lut (.A(n8820), .B(n8829), .C(n8832), .D(n8828), .Z(n5312)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))) */ ;
    defparam i6008_4_lut.init = 16'h808a;
    LUT4 i5982_3_lut (.A(s_status[2]), .B(s_irq_st[2]), .C(q_15__N_82), 
         .Z(n8039)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5982_3_lut.init = 16'hcaca;
    LUT4 s_desc_lo_4__bdd_3_lut_6414 (.A(s_desc_lo[4]), .B(q_15__N_82), 
         .C(s_desc_hi[4]), .Z(n8404)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_4__bdd_3_lut_6414.init = 16'he2e2;
    LUT4 s_desc_lo_4__bdd_3_lut (.A(s_mem_wr_addr[4]), .B(s_mem_rd_addr[4]), 
         .C(q_15__N_82), .Z(n8405)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_4__bdd_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut (.A(out_get_N_312), .B(n8341), .Z(s_cfg_wr_stb)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut.init = 16'h8888;
    LUT4 i1_2_lut_4_lut_adj_206 (.A(n8843), .B(q_15__N_56), .C(q_15__N_52), 
         .D(q_15__N_64), .Z(fsmc_clk_c_enable_1)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(484[21:67])
    defparam i1_2_lut_4_lut_adj_206.init = 16'h0002;
    LUT4 i1_2_lut_4_lut_adj_207 (.A(n8843), .B(q_15__N_56), .C(q_15__N_52), 
         .D(q_15__N_64), .Z(fsmc_clk_c_enable_2)) /* synthesis lut_function=(!((B+(C+!(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(484[21:67])
    defparam i1_2_lut_4_lut_adj_207.init = 16'h0200;
    BB fsmc_d_pad_13 (.I(s_cram_dout[13]), .T(n3239), .B(fsmc_d[13]), 
       .O(fsmc_d_out_13));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    LUT4 i3_4_lut (.A(s_irq_en[3]), .B(n6), .C(irq_N_8[1]), .D(s_tmr_cnt_en), 
         .Z(irq_N_5)) /* synthesis lut_function=(A (B+(C+!(D)))+!A (B+(C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(420[9:18])
    defparam i3_4_lut.init = 16'hfcfe;
    PFUMX i6007 (.BLUT(n8062), .ALUT(n8063), .C0(q_15__N_80), .Z(n8064));
    LUT4 n7739_bdd_4_lut_6717 (.A(n7739), .B(n8863), .C(n8384), .D(n1939), 
         .Z(n8726)) /* synthesis lut_function=(A (C (D))+!A (B (C (D))+!B (C+!(D)))) */ ;
    defparam n7739_bdd_4_lut_6717.init = 16'hf011;
    \jtag_reg(16,16)_U5  desc_hi (.s_desc_hi({s_desc_hi}), .mclk_c(mclk_c), 
            .mclk_c_enable_112(mclk_c_enable_112), .\s_desc_data[16] (s_desc_data[16]), 
            .\s_desc_data[31] (s_desc_data[31]), .\s_desc_data[30] (s_desc_data[30]), 
            .\s_desc_data[29] (s_desc_data[29]), .\s_desc_data[28] (s_desc_data[28]), 
            .\s_desc_data[27] (s_desc_data[27]), .\s_desc_data[26] (s_desc_data[26]), 
            .\s_desc_data[25] (s_desc_data[25]), .\s_desc_data[24] (s_desc_data[24]), 
            .\s_desc_data[23] (s_desc_data[23]), .\s_desc_data[22] (s_desc_data[22]), 
            .\s_desc_data[21] (s_desc_data[21]), .\s_desc_data[20] (s_desc_data[20]), 
            .\s_desc_data[19] (s_desc_data[19]), .\s_desc_data[18] (s_desc_data[18]), 
            .\s_desc_data[17] (s_desc_data[17]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(788[12:27])
    LUT4 i1_2_lut_3_lut_4_lut_adj_208 (.A(s_addr3[3]), .B(s_addr3[0]), .C(s_addr3[2]), 
         .D(s_addr3[1]), .Z(n7704)) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_208.init = 16'h4000;
    PFUMX i5813 (.BLUT(n7868), .ALUT(n7869), .C0(q_15__N_80), .Z(n7870));
    GSR GSR_INST (.GSR(mclk_c_enable_81));
    PFUMX s_reg_rd_sel_3__I_0_Mux_4_i15 (.BLUT(n7_adj_1630), .ALUT(n8738), 
          .C0(q_15__N_76), .Z(s_reg_dout[4]));
    LUT4 i1144_1_lut (.A(s_cfg[11]), .Z(n3236)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(683[2:12])
    defparam i1144_1_lut.init = 16'h5555;
    LUT4 s_cfg_10__I_0_1_lut (.A(s_cfg[10]), .Z(tp_trst_c)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(682[16:59])
    defparam s_cfg_10__I_0_1_lut.init = 16'h5555;
    LUT4 i3157_2_lut (.A(s_tap_tdi), .B(s_cfg[4]), .Z(tp_tdi_c)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1097[12:55])
    defparam i3157_2_lut.init = 16'h2222;
    LUT4 i3153_2_lut (.A(uart_tx_c), .B(s_cfg[7]), .Z(tp_dbgrq_c)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1082[17:56])
    defparam i3153_2_lut.init = 16'h8888;
    LUT4 i3155_2_lut (.A(tp_dbgack_c), .B(s_cfg[7]), .Z(uart_rx_c)) /* synthesis lut_function=(A+!(B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1088[15:56])
    defparam i3155_2_lut.init = 16'hbbbb;
    PFUMX s_reg_rd_sel_3__I_0_Mux_7_i15 (.BLUT(n7_adj_1615), .ALUT(n8739), 
          .C0(q_15__N_76), .Z(s_reg_dout[7]));
    LUT4 i1_2_lut_adj_209 (.A(s_addr1_adj_1783[1]), .B(s_addr1_adj_1783[0]), 
         .Z(n13)) /* synthesis lut_function=(!((B)+!A)) */ ;
    defparam i1_2_lut_adj_209.init = 16'h2222;
    LUT4 i1_2_lut_adj_210 (.A(s_addr0[0]), .B(s_addr0[1]), .Z(n7_adj_1653)) /* synthesis lut_function=(!(A+!(B))) */ ;
    defparam i1_2_lut_adj_210.init = 16'h4444;
    LUT4 i1_2_lut_adj_211 (.A(s_addr1_adj_1783[1]), .B(s_addr1_adj_1783[0]), 
         .Z(n11)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_adj_211.init = 16'h8888;
    LUT4 i1_2_lut_adj_212 (.A(s_addr0[1]), .B(s_addr0[0]), .Z(n6_adj_1654)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_adj_212.init = 16'h8888;
    PFUMX s_reg_rd_sel_3__I_0_Mux_10_i15 (.BLUT(n7_adj_1645), .ALUT(n8740), 
          .C0(q_15__N_76), .Z(s_reg_dout[10]));
    BB fsmc_d_pad_14 (.I(s_cram_dout[14]), .T(n3239), .B(fsmc_d[14]), 
       .O(fsmc_d_out_14));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    ROM256X1A mux_178_Mux_2 (.AD0(s_status[0]), .AD1(s_status[1]), .AD2(s_status[2]), 
            .AD3(s_status[3]), .AD4(s_end_state[0]), .AD5(s_end_state[1]), 
            .AD6(s_end_state[2]), .AD7(s_end_state[3]), .DO0(s_goto_state[2])) /* synthesis initstate=0xB3B3F3B3B3B3B3B3B1B1B3B3B1B3B0B1B3B3B3F3B3B3B3B3B3B1B3B3B3B1B3B0 */ ;
    defparam mux_178_Mux_2.initval = 256'hB3B3F3B3B3B3B3B3B1B1B3B3B1B3B0B1B3B3B3F3B3B3B3B3B3B1B3B3B3B1B3B0;
    LUT4 i1_2_lut_adj_213 (.A(out_get_N_312), .B(n8388), .Z(s_ckgen_div_wr_stb)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_adj_213.init = 16'h8888;
    PFUMX s_reg_rd_sel_3__I_0_Mux_11_i15 (.BLUT(n7_adj_1640), .ALUT(n8741), 
          .C0(q_15__N_76), .Z(s_reg_dout[11]));
    LUT4 i1_2_lut_adj_214 (.A(out_get_N_312), .B(n8366), .Z(s_ckgen_rtdiv_wr_stb)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_adj_214.init = 16'h8888;
    BB fsmc_d_pad_15 (.I(s_cram_dout[15]), .T(n3239), .B(fsmc_d[15]), 
       .O(fsmc_d_out_15));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    LUT4 i5970_3_lut (.A(s_mem_wr_addr[1]), .B(s_mem_rd_addr[1]), .C(q_15__N_82), 
         .Z(n8027)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5970_3_lut.init = 16'hcaca;
    PFUMX s_reg_rd_sel_3__I_0_Mux_9_i15 (.BLUT(n7_adj_1639), .ALUT(n8735), 
          .C0(q_15__N_76), .Z(s_reg_dout[9]));
    LUT4 i5682_2_lut (.A(s_addr1_adj_1783[1]), .B(s_addr1_adj_1783[0]), 
         .Z(n7735)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i5682_2_lut.init = 16'heeee;
    LUT4 i5684_2_lut (.A(s_addr0[1]), .B(s_addr0[0]), .Z(n7737)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i5684_2_lut.init = 16'heeee;
    LUT4 i1_2_lut_adj_215 (.A(s_addr1_adj_1783[0]), .B(s_addr1_adj_1783[1]), 
         .Z(n11_adj_1656)) /* synthesis lut_function=(!((B)+!A)) */ ;
    defparam i1_2_lut_adj_215.init = 16'h2222;
    FD1S3AX s_irq_reg_i4 (.D(s_tmr_cnt_en_N_1), .CK(mclk_c), .Q(s_irq_st[3]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(603[3] 605[10])
    defparam s_irq_reg_i4.GSR = "DISABLED";
    FD1S3AX s_irq_reg_i3 (.D(s_1khz_stb), .CK(mclk_c), .Q(s_irq_st[2]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(603[3] 605[10])
    defparam s_irq_reg_i3.GSR = "DISABLED";
    FD1S3AX s_irq_reg_i2 (.D(s_irq[1]), .CK(mclk_c), .Q(s_irq_st[1]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(603[3] 605[10])
    defparam s_irq_reg_i2.GSR = "DISABLED";
    LUT4 n1978_bdd_4_lut_6655 (.A(n8863), .B(n20_adj_1655), .C(n8362), 
         .D(n1939), .Z(n8731)) /* synthesis lut_function=(A (C (D))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam n1978_bdd_4_lut_6655.init = 16'hf044;
    LUT4 i3307_2_lut (.A(s_aux_mem_dout[15]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1643)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3307_2_lut.init = 16'h8888;
    PFUMX s_reg_rd_sel_3__I_0_Mux_12_i15 (.BLUT(n7_adj_1607), .ALUT(n14), 
          .C0(q_15__N_76), .Z(s_reg_dout[12]));
    ddr_phy ddr_phy (.\s_cfg[6] (s_cfg[6]), .mclk_c(mclk_c), .cke_din({s_ddr_cke}), 
            .buf_ckeo0(buf_ckeo0), .VCC_net(VCC_net), .s_ddr_csn({s_ddr_csn}), 
            .buf_csno1(buf_csno1), .buf_csno0(buf_csno0), .s_ddr_wen(s_ddr_wen), 
            .buf_weno(buf_weno), .s_ddr_casn(s_ddr_casn), .buf_casno(buf_casno), 
            .s_ddr_rasn(s_ddr_rasn), .buf_rasno(buf_rasno), .s_ddr_ba({s_ddr_ba}), 
            .buf_bao1(buf_bao1), .buf_bao0(buf_bao0), .s_ddr_addr({s_ddr_addr}), 
            .buf_addro12(buf_addro12), .buf_addro11(buf_addro11), .buf_addro10(buf_addro10), 
            .buf_addro9(buf_addro9), .buf_addro8(buf_addro8), .buf_addro7(buf_addro7), 
            .buf_addro6(buf_addro6), .buf_addro5(buf_addro5), .buf_addro4(buf_addro4), 
            .buf_addro3(buf_addro3), .buf_addro2(buf_addro2), .buf_addro1(buf_addro1), 
            .buf_addro0(buf_addro0), .n9428(n9428), .buf_clkout0(buf_clkout0), 
            .GND_net(GND_net), .s_ddr_dqstri1(s_ddr_dqstri1), .dqstrio1(dqstrio1), 
            .s_ddr_dqstri0(s_ddr_dqstri0), .dqstrio0(dqstrio0), .s_ddr_dqs1(s_ddr_dqs1), 
            .dqsout1(dqsout1), .s_ddr_dqs0(s_ddr_dqs0), .dqsout0(dqsout0), 
            .s_ddr_dtri1(s_ddr_dtri1), .dqtri1(dqtri1), .s_ddr_dtri0(s_ddr_dtri0), 
            .dqtri0(dqtri0), .s_ddr_dataout0({s_ddr_dataout0}), .dqo15(dqo15), 
            .dqo14(dqo14), .dqo13(dqo13), .dqo12(dqo12), .dqo11(dqo11), 
            .dqo10(dqo10), .dqo9(dqo9), .dqo8(dqo8), .dqo7(dqo7), .dqo6(dqo6), 
            .dqo5(dqo5), .dqo4(dqo4), .dqo3(dqo3), .dqo2(dqo2), .dqo1(dqo1), 
            .dqo0(dqo0), .s_ddr_datain1({s_ddr_datain1}), .datain_t15(datain_t15), 
            .datain_t14(datain_t14), .datain_t13(datain_t13), .datain_t12(datain_t12), 
            .datain_t11(datain_t11), .datain_t10(datain_t10), .datain_t9(datain_t9), 
            .datain_t8(datain_t8), .s_ddr_datain0({s_ddr_datain0}), .datain_t7(datain_t7), 
            .datain_t6(datain_t6), .datain_t5(datain_t5), .datain_t4(datain_t4), 
            .datain_t3(datain_t3), .datain_t2(datain_t2), .datain_t1(datain_t1), 
            .datain_t0(datain_t0), .s_ddr_burstdet1(s_ddr_burstdet1), .s_ddr_read1(s_ddr_read1), 
            .s_ddr_readclksel01(s_ddr_readclksel01), .s_ddr_readclksel11(s_ddr_readclksel11), 
            .dqsin1(dqsin1), .s_ddr_burstdet0(s_ddr_burstdet0), .s_ddr_read0(s_ddr_read0), 
            .s_ddr_readclksel00(s_ddr_readclksel00), .s_ddr_readclksel10(s_ddr_readclksel10), 
            .dqsin0(dqsin0), .s_ddr_freeze(s_ddr_freeze), .s_ddr_lock(s_ddr_lock), 
            .s_ddr_uddcntln(s_ddr_uddcntln)) /* synthesis NGD_DRC_MASK=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    LUT4 i3308_2_lut (.A(s_aux_mem_dout[14]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1641)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3308_2_lut.init = 16'h8888;
    LUT4 i3316_2_lut (.A(s_aux_mem_dout[13]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1635)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3316_2_lut.init = 16'h8888;
    LUT4 i3322_2_lut (.A(s_aux_mem_dout[12]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1632)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3322_2_lut.init = 16'h8888;
    LUT4 i3323_2_lut (.A(s_aux_mem_dout[11]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1628)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3323_2_lut.init = 16'h8888;
    LUT4 i3324_2_lut (.A(s_aux_mem_dout[10]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1626)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3324_2_lut.init = 16'h8888;
    LUT4 i3325_2_lut (.A(s_aux_mem_dout[9]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1624)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3325_2_lut.init = 16'h8888;
    LUT4 i3326_2_lut (.A(s_aux_mem_dout[8]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1621)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3326_2_lut.init = 16'h8888;
    PFUMX s_reg_rd_sel_3__I_0_Mux_5_i15 (.BLUT(n7_adj_1650), .ALUT(n8736), 
          .C0(q_15__N_76), .Z(s_reg_dout[5]));
    LUT4 i3327_2_lut (.A(s_aux_mem_dout[7]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1619)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3327_2_lut.init = 16'h8888;
    LUT4 i3328_2_lut (.A(s_aux_mem_dout[6]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1617)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3328_2_lut.init = 16'h8888;
    LUT4 i2_4_lut (.A(s_irq_en[0]), .B(s_irq_en[2]), .C(s_tap_busy), .D(s_1khz_stb), 
         .Z(n6)) /* synthesis lut_function=(A (B ((D)+!C)+!B !(C))+!A (B (D))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(420[9:18])
    defparam i2_4_lut.init = 16'hce0a;
    LUT4 i3329_2_lut (.A(s_aux_mem_dout[5]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1613)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3329_2_lut.init = 16'h8888;
    PFUMX i6359 (.BLUT(n8340), .ALUT(n8336), .C0(n1938), .Z(n8341));
    \jtag_reg(16,16)_U8  cfg_r (.mclk_c(mclk_c), .n4442(n4442), .s_reg_din({s_reg_din}), 
            .n9428(n9428), .s_cfg({s_cfg}), .s_cfg_wr_stb(s_cfg_wr_stb), 
            .mclk_c_enable_81(mclk_c_enable_81));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(663[10:25])
    PFUMX s_reg_rd_sel_3__I_0_Mux_15_i15 (.BLUT(n7_adj_1646), .ALUT(n14_adj_1648), 
          .C0(q_15__N_76), .Z(s_reg_dout[15]));
    LUT4 i2310_4_lut (.A(s_tmr[15]), .B(s_tmr_cnt_en_N_2), .C(s_tmr_cnt_en), 
         .D(q_15__N_51), .Z(n4409)) /* synthesis lut_function=(A (B+(C))+!A (B+!((D)+!C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(709[3] 717[10])
    defparam i2310_4_lut.init = 16'hecfc;
    LUT4 n20_bdd_2_lut_3_lut (.A(n8863), .B(n8862), .C(n20), .Z(n8424)) /* synthesis lut_function=(!(A+(B+!(C)))) */ ;
    defparam n20_bdd_2_lut_3_lut.init = 16'h1010;
    LUT4 i5712_2_lut_3_lut (.A(n8863), .B(n8862), .C(n7739), .Z(n7767)) /* synthesis lut_function=(A+(B+(C))) */ ;
    defparam i5712_2_lut_3_lut.init = 16'hfefe;
    PFUMX s_reg_rd_sel_3__I_0_Mux_14_i15 (.BLUT(n7_adj_1647), .ALUT(n14_adj_1649), 
          .C0(q_15__N_76), .Z(s_reg_dout[14]));
    LUT4 n24_bdd_2_lut_6462_3_lut (.A(n8863), .B(n8862), .C(n24), .Z(n8431)) /* synthesis lut_function=(!(A+(B+!(C)))) */ ;
    defparam n24_bdd_2_lut_6462_3_lut.init = 16'h1010;
    PFUMX s_reg_rd_sel_3__I_0_Mux_13_i15 (.BLUT(n7_adj_1622), .ALUT(n14_adj_1623), 
          .C0(q_15__N_76), .Z(s_reg_dout[13]));
    PFUMX s_reg_rd_sel_3__I_0_Mux_8_i15 (.BLUT(n7_adj_1638), .ALUT(n8737), 
          .C0(q_15__N_76), .Z(s_reg_dout[8]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_0_i3 (.BLUT(n1_adj_1652), .ALUT(n2_adj_1651), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[0]));
    \jtag_reg(16,16)_U7  ckgen_div_r (.s_ckgen_div({s_ckgen_div}), .mclk_c(mclk_c), 
            .s_ckgen_div_wr_stb(s_ckgen_div_wr_stb), .s_reg_din({s_reg_din}));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(805[16:31])
    PFUMX s_mem_rd_sel_1__I_0_Mux_1_i3 (.BLUT(n1), .ALUT(n2_adj_1616), .C0(s_mem_rd_sel[1]), 
          .Z(s_mem_dout[1]));
    LUT4 i3330_2_lut (.A(s_aux_mem_dout[4]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1611)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3330_2_lut.init = 16'h8888;
    LUT4 i1_2_lut_adj_216 (.A(out_get_N_312), .B(n19), .Z(s_tmr_cnt_en_N_2)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_adj_216.init = 16'h8888;
    \jtag_reg(16,16)  r3 (.s_r3({s_r3}), .mclk_c(mclk_c), .s_r3_wr_stb(s_r3_wr_stb), 
            .s_reg_din({s_reg_din}));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(727[7:22])
    PFUMX s_mem_rd_sel_1__I_0_Mux_2_i3 (.BLUT(n1_adj_1608), .ALUT(n2), .C0(s_mem_rd_sel[1]), 
          .Z(s_mem_dout[2]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_3_i3 (.BLUT(n1_adj_1610), .ALUT(n2_adj_1609), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[3]));
    \jtag_desc_ram(16,9,32,8)  desc_mem (.fsmc_clk_c(fsmc_clk_c), .mclk_c(mclk_c), 
            .s_we_hi(s_we_hi), .VCC_net(VCC_net), .GND_net(GND_net), .n9425(n9425), 
            .s_desc_addr({s_desc_addr}), .q_15__N_78(q_15__N_78), .q_15__N_76(q_15__N_76), 
            .q_15__N_74(q_15__N_74), .q_15__N_72(q_15__N_72), .q_15__N_70(q_15__N_70), 
            .q_15__N_68(q_15__N_68), .q_15__N_66(q_15__N_66), .fsmc_d_out_0(fsmc_d_out_0), 
            .fsmc_d_out_1(fsmc_d_out_1), .fsmc_d_out_2(fsmc_d_out_2), .fsmc_d_out_3(fsmc_d_out_3), 
            .fsmc_d_out_4(fsmc_d_out_4), .fsmc_d_out_5(fsmc_d_out_5), .fsmc_d_out_6(fsmc_d_out_6), 
            .fsmc_d_out_7(fsmc_d_out_7), .fsmc_d_out_8(fsmc_d_out_8), .fsmc_d_out_9(fsmc_d_out_9), 
            .fsmc_d_out_10(fsmc_d_out_10), .fsmc_d_out_11(fsmc_d_out_11), 
            .fsmc_d_out_12(fsmc_d_out_12), .fsmc_d_out_13(fsmc_d_out_13), 
            .fsmc_d_out_14(fsmc_d_out_14), .fsmc_d_out_15(fsmc_d_out_15), 
            .\s_desc_data[16] (s_desc_data[16]), .\s_desc_data[17] (s_desc_data[17]), 
            .\s_desc_data[18] (s_desc_data[18]), .\s_desc_data[19] (s_desc_data[19]), 
            .\s_desc_data[20] (s_desc_data[20]), .\s_desc_data[21] (s_desc_data[21]), 
            .\s_desc_data[22] (s_desc_data[22]), .\s_desc_data[23] (s_desc_data[23]), 
            .\s_desc_data[24] (s_desc_data[24]), .\s_desc_data[25] (s_desc_data[25]), 
            .\s_desc_data[26] (s_desc_data[26]), .\s_desc_data[27] (s_desc_data[27]), 
            .\s_desc_data[28] (s_desc_data[28]), .\s_desc_data[29] (s_desc_data[29]), 
            .\s_desc_data[30] (s_desc_data[30]), .\s_desc_data[31] (s_desc_data[31]), 
            .s_we_lo(s_we_lo), .n9423(n9423), .s_pos_0_3__N_670({s_pos_0_3__N_670}), 
            .\s_desc_data[4] (s_desc_data[4]), .\s_desc_data[5] (s_desc_data[5]), 
            .\s_desc_data[6] (s_desc_data[6]), .\s_desc_data[7] (s_desc_data[7]), 
            .\s_desc_data[8] (s_desc_data[8]), .\s_desc_data[9] (s_desc_data[9]), 
            .\s_desc_data[10] (s_desc_data[10]), .\s_desc_data[11] (s_desc_data[11]), 
            .\s_desc_data[12] (s_desc_data[12]), .\s_desc_data[13] (s_desc_data[13]), 
            .\s_desc_data[14] (s_desc_data[14]), .\s_desc_data[15] (s_desc_data[15]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(839[13:33])
    PFUMX s_mem_rd_sel_1__I_0_Mux_4_i3 (.BLUT(n1_adj_1612), .ALUT(n2_adj_1611), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[4]));
    LUT4 i3331_2_lut (.A(s_aux_mem_dout[3]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1609)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3331_2_lut.init = 16'h8888;
    LUT4 i3332_2_lut (.A(s_aux_mem_dout[2]), .B(s_mem_rd_sel[0]), .Z(n2)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3332_2_lut.init = 16'h8888;
    LUT4 i3341_2_lut (.A(s_aux_mem_dout[1]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1616)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3341_2_lut.init = 16'h8888;
    LUT4 i3384_2_lut (.A(s_aux_mem_dout[0]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1651)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[2] 494[31])
    defparam i3384_2_lut.init = 16'h8888;
    LUT4 s_reg_rd_sel_3__I_0_Mux_8_i7_4_lut (.A(s_r3[8]), .B(n7870), .C(q_15__N_78), 
         .D(n3455), .Z(n7_adj_1638)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_8_i7_4_lut.init = 16'hcac0;
    LUT4 i3315_4_lut (.A(s_desc_lo[13]), .B(n3411), .C(s_desc_hi[13]), 
         .D(q_15__N_82), .Z(n14_adj_1623)) /* synthesis lut_function=(A (B (C+!(D)))+!A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam i3315_4_lut.init = 16'hc088;
    LUT4 s_reg_rd_sel_3__I_0_Mux_13_i7_4_lut (.A(s_r3[13]), .B(n8061), .C(q_15__N_78), 
         .D(n3455), .Z(n7_adj_1622)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_13_i7_4_lut.init = 16'hcac0;
    LUT4 i3313_4_lut (.A(s_desc_lo[14]), .B(n3411), .C(s_desc_hi[14]), 
         .D(q_15__N_82), .Z(n14_adj_1649)) /* synthesis lut_function=(A (B (C+!(D)))+!A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam i3313_4_lut.init = 16'hc088;
    LUT4 s_reg_rd_sel_3__I_0_Mux_14_i7_4_lut (.A(s_r3[14]), .B(n8064), .C(q_15__N_78), 
         .D(n3455), .Z(n7_adj_1647)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_14_i7_4_lut.init = 16'hcac0;
    LUT4 i3305_4_lut (.A(s_desc_lo[15]), .B(n3411), .C(s_desc_hi[15]), 
         .D(q_15__N_82), .Z(n14_adj_1648)) /* synthesis lut_function=(A (B (C+!(D)))+!A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam i3305_4_lut.init = 16'hc088;
    LUT4 s_reg_rd_sel_3__I_0_Mux_15_i7_4_lut (.A(s_r3[15]), .B(n7864), .C(q_15__N_78), 
         .D(n3455), .Z(n7_adj_1646)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_15_i7_4_lut.init = 16'hcac0;
    LUT4 s_reg_rd_sel_3__I_0_Mux_5_i7_4_lut (.A(s_r3[5]), .B(n7930), .C(q_15__N_78), 
         .D(n3455), .Z(n7_adj_1650)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_5_i7_4_lut.init = 16'hcac0;
    LUT4 i3195_4_lut (.A(s_desc_lo[12]), .B(n3411), .C(s_desc_hi[12]), 
         .D(q_15__N_82), .Z(n14)) /* synthesis lut_function=(A (B (C+!(D)))+!A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam i3195_4_lut.init = 16'hc088;
    LUT4 s_desc_lo_8__bdd_3_lut_6361 (.A(s_desc_lo[8]), .B(q_15__N_82), 
         .C(s_desc_hi[8]), .Z(n8342)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_8__bdd_3_lut_6361.init = 16'he2e2;
    LUT4 i1_4_lut (.A(out_get_N_312), .B(n8432), .C(n7680), .D(n1938), 
         .Z(s_irq_en_wr_stb)) /* synthesis lut_function=(A (B (C+!(D))+!B (C (D)))) */ ;
    defparam i1_4_lut.init = 16'ha088;
    PFUMX s_mem_rd_sel_1__I_0_Mux_5_i3 (.BLUT(n1_adj_1614), .ALUT(n2_adj_1613), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[5]));
    LUT4 s_desc_lo_8__bdd_3_lut (.A(s_mem_wr_addr[8]), .B(s_mem_rd_addr[8]), 
         .C(q_15__N_82), .Z(n8343)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_8__bdd_3_lut.init = 16'hcaca;
    \jtag_clk_scaler(1000)  cklgen_1khz (.GND_net(GND_net), .mclk_c(mclk_c), 
            .s_1mhz_stb(s_1mhz_stb), .s_1khz_stb(s_1khz_stb));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1022[16:38])
    \jtag_reg(16,16)_U6  ckgen_rtdiv_r (.s_ckgen_rtdiv({s_ckgen_rtdiv}), .mclk_c(mclk_c), 
            .s_ckgen_rtdiv_wr_stb(s_ckgen_rtdiv_wr_stb), .s_reg_din({s_reg_din}));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(822[18:33])
    LUT4 s_reg_rd_sel_3__I_0_Mux_12_i7_4_lut (.A(s_r3[12]), .B(n7924), .C(q_15__N_78), 
         .D(n3455), .Z(n7_adj_1607)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_12_i7_4_lut.init = 16'hcac0;
    LUT4 s_reg_rd_sel_3__I_0_Mux_9_i7_4_lut (.A(s_r3[9]), .B(n7939), .C(q_15__N_78), 
         .D(n3455), .Z(n7_adj_1639)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_9_i7_4_lut.init = 16'hcac0;
    LUT4 i5699_2_lut_rep_166_3_lut (.A(s_addr3[3]), .B(s_addr3[0]), .C(s_addr3[2]), 
         .Z(n8833)) /* synthesis lut_function=(A+(B+(C))) */ ;
    defparam i5699_2_lut_rep_166_3_lut.init = 16'hfefe;
    LUT4 i1_2_lut_3_lut_4_lut_adj_217 (.A(s_addr3[3]), .B(s_addr3[0]), .C(s_addr3[1]), 
         .D(s_addr3[2]), .Z(n7680)) /* synthesis lut_function=(!(A+(B+((D)+!C)))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_217.init = 16'h0010;
    LUT4 n1978_bdd_2_lut_6355_3_lut_4_lut (.A(s_addr3[3]), .B(s_addr3[0]), 
         .C(s_addr3[1]), .D(s_addr3[2]), .Z(n8336)) /* synthesis lut_function=(!(A+(B+!(C (D))))) */ ;
    defparam n1978_bdd_2_lut_6355_3_lut_4_lut.init = 16'h1000;
    LUT4 n7697_bdd_2_lut_6539_3_lut_4_lut (.A(s_addr3[3]), .B(s_addr3[0]), 
         .C(s_addr3[1]), .D(s_addr3[2]), .Z(n8383)) /* synthesis lut_function=(!(A+(B+(C+!(D))))) */ ;
    defparam n7697_bdd_2_lut_6539_3_lut_4_lut.init = 16'h0100;
    LUT4 s_reg_rd_sel_3__I_0_Mux_11_i7_4_lut (.A(s_r3[11]), .B(n7957), .C(q_15__N_78), 
         .D(n3455), .Z(n7_adj_1640)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_11_i7_4_lut.init = 16'hcac0;
    LUT4 s_reg_rd_sel_3__I_0_Mux_10_i7_4_lut (.A(s_r3[10]), .B(n7948), .C(q_15__N_78), 
         .D(n3455), .Z(n7_adj_1645)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_10_i7_4_lut.init = 16'hcac0;
    LUT4 s_reg_rd_sel_3__I_0_Mux_7_i7_4_lut (.A(s_r3[7]), .B(n8055), .C(q_15__N_78), 
         .D(n3455), .Z(n7_adj_1615)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_7_i7_4_lut.init = 16'hcac0;
    LUT4 s_desc_lo_5__bdd_3_lut_6364 (.A(s_desc_lo[5]), .B(q_15__N_82), 
         .C(s_desc_hi[5]), .Z(n8345)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_5__bdd_3_lut_6364.init = 16'he2e2;
    PFUMX s_mem_rd_sel_1__I_0_Mux_6_i3 (.BLUT(n1_adj_1618), .ALUT(n2_adj_1617), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[6]));
    LUT4 s_reg_rd_sel_3__I_0_Mux_4_i7_4_lut (.A(s_r3[4]), .B(n8058), .C(q_15__N_78), 
         .D(n3455), .Z(n7_adj_1630)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_4_i7_4_lut.init = 16'hcac0;
    PFUMX s_mem_rd_sel_1__I_0_Mux_7_i3 (.BLUT(n1_adj_1620), .ALUT(n2_adj_1619), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[7]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_8_i3 (.BLUT(n1_adj_1631), .ALUT(n2_adj_1621), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[8]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_9_i3 (.BLUT(n1_adj_1625), .ALUT(n2_adj_1624), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[9]));
    LUT4 s_desc_lo_5__bdd_3_lut (.A(s_mem_wr_addr[5]), .B(s_mem_rd_addr[5]), 
         .C(q_15__N_82), .Z(n8346)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_5__bdd_3_lut.init = 16'hcaca;
    PFUMX s_mem_rd_sel_1__I_0_Mux_10_i3 (.BLUT(n1_adj_1627), .ALUT(n2_adj_1626), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[10]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_11_i3 (.BLUT(n1_adj_1629), .ALUT(n2_adj_1628), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[11]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_12_i3 (.BLUT(n1_adj_1633), .ALUT(n2_adj_1632), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[12]));
    LUT4 i5812_3_lut (.A(s_cfg[8]), .B(s_tmr[8]), .C(q_15__N_82), .Z(n7869)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5812_3_lut.init = 16'hcaca;
    PFUMX s_mem_rd_sel_1__I_0_Mux_13_i3 (.BLUT(n1_adj_1636), .ALUT(n2_adj_1635), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[13]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_14_i3 (.BLUT(n1_adj_1642), .ALUT(n2_adj_1641), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[14]));
    LUT4 i5811_3_lut (.A(s_ckgen_div[8]), .B(s_ckgen_rtdiv[8]), .C(q_15__N_82), 
         .Z(n7868)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5811_3_lut.init = 16'hcaca;
    PFUMX s_mem_rd_sel_1__I_0_Mux_15_i3 (.BLUT(n1_adj_1644), .ALUT(n2_adj_1643), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[15]));
    LUT4 n1978_bdd_2_lut_6356_3_lut (.A(s_addr2_adj_1784[3]), .B(s_addr2_adj_1784[0]), 
         .C(s_addr2_adj_1784[1]), .Z(n8337)) /* synthesis lut_function=(!(A+(B+!(C)))) */ ;
    defparam n1978_bdd_2_lut_6356_3_lut.init = 16'h1010;
    L6MUX21 i5941 (.D0(n7996), .D1(n7997), .SD(q_15__N_78), .Z(n7998));
    LUT4 n7739_bdd_2_lut_6396_3_lut (.A(s_addr2_adj_1784[3]), .B(s_addr2_adj_1784[0]), 
         .C(s_addr2_adj_1784[1]), .Z(n8384)) /* synthesis lut_function=(!(A+(B+(C)))) */ ;
    defparam n7739_bdd_2_lut_6396_3_lut.init = 16'h0101;
    LUT4 n3882_bdd_3_lut_6498_4_lut (.A(s_addr3[3]), .B(s_addr3[0]), .C(s_addr3[2]), 
         .D(s_addr3[1]), .Z(n8361)) /* synthesis lut_function=(!(A+(((D)+!C)+!B))) */ ;
    defparam n3882_bdd_3_lut_6498_4_lut.init = 16'h0040;
    L6MUX21 i5963 (.D0(n8018), .D1(n8019), .SD(q_15__N_78), .Z(n8020));
    LUT4 n24_bdd_2_lut_6438_3_lut_4_lut (.A(s_addr2_adj_1784[3]), .B(s_addr2_adj_1784[0]), 
         .C(s_addr2_adj_1784[1]), .D(s_addr2_adj_1784[2]), .Z(n8430)) /* synthesis lut_function=(!(A+(B+((D)+!C)))) */ ;
    defparam n24_bdd_2_lut_6438_3_lut_4_lut.init = 16'h0010;
    LUT4 i5707_2_lut_3_lut_4_lut (.A(s_addr2_adj_1784[3]), .B(s_addr2_adj_1784[0]), 
         .C(s_addr2_adj_1784[1]), .D(s_addr2_adj_1784[2]), .Z(n7761)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i5707_2_lut_3_lut_4_lut.init = 16'hfffe;
    L6MUX21 i5988 (.D0(n8043), .D1(n8044), .SD(q_15__N_78), .Z(n8045));
    L6MUX21 i5995 (.D0(n8050), .D1(n8051), .SD(q_15__N_78), .Z(n8052));
    LUT4 i6006_3_lut (.A(s_cfg[14]), .B(s_tmr[14]), .C(q_15__N_82), .Z(n8063)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i6006_3_lut.init = 16'hcaca;
    PFUMX i46 (.BLUT(n7737), .ALUT(n7735), .C0(n1940), .Z(n7739));
    PFUMX i42 (.BLUT(n6_adj_1654), .ALUT(n11), .C0(n1940), .Z(n20));
    LUT4 i6005_3_lut (.A(s_ckgen_div[14]), .B(s_ckgen_rtdiv[14]), .C(q_15__N_82), 
         .Z(n8062)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i6005_3_lut.init = 16'hcaca;
    LUT4 i6292_3_lut (.A(q_15__N_76), .B(q_15__N_78), .C(q_15__N_80), 
         .Z(n7779)) /* synthesis lut_function=(A+((C)+!B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam i6292_3_lut.init = 16'hfbfb;
    \dpram(16,8)  ptr_mem (.fsmc_clk_c(fsmc_clk_c), .mclk_c(mclk_c), .s_ptr_mem_wr_stb(s_ptr_mem_wr_stb), 
            .VCC_net(VCC_net), .GND_net(GND_net), .q_15__N_82(q_15__N_82), 
            .s_ptr_addr({s_ptr_addr}), .q_15__N_80(q_15__N_80), .q_15__N_78(q_15__N_78), 
            .q_15__N_76(q_15__N_76), .q_15__N_74(q_15__N_74), .q_15__N_72(q_15__N_72), 
            .q_15__N_70(q_15__N_70), .q_15__N_68(q_15__N_68), .fsmc_d_out_0(fsmc_d_out_0), 
            .fsmc_d_out_1(fsmc_d_out_1), .fsmc_d_out_2(fsmc_d_out_2), .fsmc_d_out_3(fsmc_d_out_3), 
            .fsmc_d_out_4(fsmc_d_out_4), .fsmc_d_out_5(fsmc_d_out_5), .fsmc_d_out_6(fsmc_d_out_6), 
            .fsmc_d_out_7(fsmc_d_out_7), .fsmc_d_out_8(fsmc_d_out_8), .fsmc_d_out_9(fsmc_d_out_9), 
            .fsmc_d_out_10(fsmc_d_out_10), .fsmc_d_out_11(fsmc_d_out_11), 
            .fsmc_d_out_12(fsmc_d_out_12), .fsmc_d_out_13(fsmc_d_out_13), 
            .fsmc_d_out_14(fsmc_d_out_14), .fsmc_d_out_15(fsmc_d_out_15), 
            .s_ptr_data({s_ptr_data}));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(860[12:24])
    PFUMX i46_adj_218 (.BLUT(n7_adj_1653), .ALUT(n13), .C0(n1940), .Z(n24));
    LUT4 n20_bdd_2_lut_6432_3_lut_4_lut (.A(s_addr2_adj_1784[3]), .B(s_addr2_adj_1784[0]), 
         .C(s_addr2_adj_1784[2]), .D(s_addr2_adj_1784[1]), .Z(n8423)) /* synthesis lut_function=(!(A+((C+!(D))+!B))) */ ;
    defparam n20_bdd_2_lut_6432_3_lut_4_lut.init = 16'h0400;
    LUT4 i1_2_lut_rep_170_3_lut (.A(s_addr2_adj_1784[3]), .B(s_addr2_adj_1784[0]), 
         .C(s_addr2_adj_1784[1]), .Z(n8837)) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i1_2_lut_rep_170_3_lut.init = 16'h4040;
    LUT4 n1978_bdd_2_lut_6378_3_lut (.A(s_addr2_adj_1784[3]), .B(s_addr2_adj_1784[0]), 
         .C(s_addr2_adj_1784[1]), .Z(n8362)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;
    defparam n1978_bdd_2_lut_6378_3_lut.init = 16'h0404;
    \dpram(16,8)_U9  aux_mem (.fsmc_clk_c(fsmc_clk_c), .s_aux_mem_wr_stb(s_aux_mem_wr_stb), 
            .VCC_net(VCC_net), .GND_net(GND_net), .n9421(n9421), .n9424(n9424), 
            .q_15__N_78(q_15__N_78), .q_15__N_76(q_15__N_76), .q_15__N_74(q_15__N_74), 
            .q_15__N_72(q_15__N_72), .q_15__N_70(q_15__N_70), .q_15__N_68(q_15__N_68), 
            .fsmc_d_out_0(fsmc_d_out_0), .fsmc_d_out_1(fsmc_d_out_1), .fsmc_d_out_2(fsmc_d_out_2), 
            .fsmc_d_out_3(fsmc_d_out_3), .fsmc_d_out_4(fsmc_d_out_4), .fsmc_d_out_5(fsmc_d_out_5), 
            .fsmc_d_out_6(fsmc_d_out_6), .fsmc_d_out_7(fsmc_d_out_7), .fsmc_d_out_8(fsmc_d_out_8), 
            .fsmc_d_out_9(fsmc_d_out_9), .fsmc_d_out_10(fsmc_d_out_10), 
            .fsmc_d_out_11(fsmc_d_out_11), .fsmc_d_out_12(fsmc_d_out_12), 
            .fsmc_d_out_13(fsmc_d_out_13), .fsmc_d_out_14(fsmc_d_out_14), 
            .fsmc_d_out_15(fsmc_d_out_15), .s_aux_mem_dout({s_aux_mem_dout}));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(918[12:24])
    \jtag_led_drv(31,false,false,false)_U3  led_con_drv (.mclk_c(mclk_c), 
            .led_2_c(led_2_c), .n8843(n8843), .s_1khz_stb(s_1khz_stb));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1060[16:35])
    LUT4 i6142_3_lut_4_lut (.A(s_addr0[1]), .B(s_addr0[0]), .C(n1940), 
         .D(n11_adj_1656), .Z(n20_adj_1655)) /* synthesis lut_function=(A (C (D))+!A (B ((D)+!C)+!B (C (D)))) */ ;
    defparam i6142_3_lut_4_lut.init = 16'hf404;
    PFUMX i5986 (.BLUT(n8039), .ALUT(n8040), .C0(q_15__N_80), .Z(n8043));
    LUT4 s_reg_rd_sel_3__I_0_Mux_6_i4_3_lut (.A(s_ckgen_div[6]), .B(s_ckgen_rtdiv[6]), 
         .C(q_15__N_82), .Z(n4)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_6_i4_3_lut.init = 16'hcaca;
    LUT4 i5960_3_lut (.A(s_cfg[3]), .B(s_tmr[3]), .C(q_15__N_82), .Z(n8017)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5960_3_lut.init = 16'hcaca;
    PFUMX i6439 (.BLUT(n8431), .ALUT(n8430), .C0(n1939), .Z(n8432));
    LUT4 i6003_3_lut (.A(s_cfg[13]), .B(s_tmr[13]), .C(q_15__N_82), .Z(n8060)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i6003_3_lut.init = 16'hcaca;
    LUT4 i1266_2_lut (.A(q_15__N_82), .B(q_15__N_80), .Z(n3455)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam i1266_2_lut.init = 16'h8888;
    LUT4 tp_tdo_I_0_3_lut (.A(tp_tdo_c), .B(s_tap_tdi), .C(s_cfg[4]), 
         .Z(s_tap_tdo)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1096[15:61])
    defparam tp_tdo_I_0_3_lut.init = 16'hcaca;
    VLO i1 (.Z(GND_net));
    PFUMX i6433 (.BLUT(n8424), .ALUT(n8423), .C0(n1939), .Z(n8425));
    \jtag_reg(16,4)  int_en_r (.\s_irq_en[0] (s_irq_en[0]), .mclk_c(mclk_c), 
            .s_irq_en_wr_stb(s_irq_en_wr_stb), .\s_reg_din[0] (s_reg_din[0]), 
            .\s_irq_en[1] (s_irq_en[1]), .\s_reg_din[1] (s_reg_din[1]), 
            .\s_irq_en[2] (s_irq_en[2]), .\s_reg_din[2] (s_reg_din[2]), 
            .\s_irq_en[3] (s_irq_en[3]), .\s_reg_din[3] (s_reg_din[3]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(645[13:28])
    LUT4 i6002_3_lut (.A(s_ckgen_div[13]), .B(s_ckgen_rtdiv[13]), .C(q_15__N_82), 
         .Z(n8059)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i6002_3_lut.init = 16'hcaca;
    LUT4 s_1mhz_stb_I_0_2_lut_rep_210 (.A(s_1mhz_stb), .B(s_tmr_cnt_en), 
         .Z(n8877)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(705[19:46])
    defparam s_1mhz_stb_I_0_2_lut_rep_210.init = 16'h8888;
    LUT4 q_15__I_21_2_lut_rep_178_3_lut (.A(s_1mhz_stb), .B(s_tmr_cnt_en), 
         .C(s_tmr[0]), .Z(n8845)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(705[19:46])
    defparam q_15__I_21_2_lut_rep_178_3_lut.init = 16'h0808;
    LUT4 q_15__I_20_2_lut_rep_160_3_lut_4_lut (.A(s_1mhz_stb), .B(s_tmr_cnt_en), 
         .C(s_tmr[1]), .D(s_tmr[0]), .Z(n8827)) /* synthesis lut_function=(!(((C+(D))+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(705[19:46])
    defparam q_15__I_20_2_lut_rep_160_3_lut_4_lut.init = 16'h0008;
    TSALL TSALL_INST (.TSALL(GND_net));
    LUT4 i1_4_lut_adj_219 (.A(out_get_N_312), .B(n8425), .C(n7703), .D(n1938), 
         .Z(s_r3_wr_stb)) /* synthesis lut_function=(A (B (C+!(D))+!B (C (D)))) */ ;
    defparam i1_4_lut_adj_219.init = 16'ha088;
    LUT4 i6000_3_lut (.A(s_cfg[4]), .B(s_tmr[4]), .C(q_15__N_82), .Z(n8057)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i6000_3_lut.init = 16'hcaca;
    LUT4 i5999_3_lut (.A(s_ckgen_div[4]), .B(s_ckgen_rtdiv[4]), .C(q_15__N_82), 
         .Z(n8056)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5999_3_lut.init = 16'hcaca;
    LUT4 s_tmr_cnt_en_I_0_1_lut (.A(s_tmr_cnt_en), .Z(s_tmr_cnt_en_N_1)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(720[15:31])
    defparam s_tmr_cnt_en_I_0_1_lut.init = 16'h5555;
    LUT4 i5997_3_lut (.A(s_cfg[7]), .B(s_tmr[7]), .C(q_15__N_82), .Z(n8054)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5997_3_lut.init = 16'hcaca;
    LUT4 i5996_3_lut (.A(s_ckgen_div[7]), .B(s_ckgen_rtdiv[7]), .C(q_15__N_82), 
         .Z(n8053)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5996_3_lut.init = 16'hcaca;
    LUT4 i1_4_lut_adj_220 (.A(n8838), .B(n22), .C(n8837), .D(n1939), 
         .Z(n17)) /* synthesis lut_function=(A (B (C+!(D))+!B (C (D)))) */ ;
    defparam i1_4_lut_adj_220.init = 16'ha088;
    LUT4 i48_4_lut (.A(n7769), .B(s_addr3[1]), .C(n1938), .D(n8833), 
         .Z(n7771)) /* synthesis lut_function=(A (B+((D)+!C))+!A (B (C)+!B (C (D)))) */ ;
    defparam i48_4_lut.init = 16'hfaca;
    \dpram(16,10)  tx_mem (.fsmc_clk_c(fsmc_clk_c), .mclk_c(mclk_c), .fsmc_clk_c_enable_1(fsmc_clk_c_enable_1), 
            .VCC_net(VCC_net), .GND_net(GND_net), .q_15__N_82(q_15__N_82), 
            .s_addr_0({s_addr_0}), .q_15__N_80(q_15__N_80), .q_15__N_78(q_15__N_78), 
            .q_15__N_76(q_15__N_76), .q_15__N_74(q_15__N_74), .q_15__N_72(q_15__N_72), 
            .q_15__N_70(q_15__N_70), .q_15__N_68(q_15__N_68), .q_15__N_66(q_15__N_66), 
            .fsmc_d_out_0(fsmc_d_out_0), .fsmc_d_out_1(fsmc_d_out_1), .fsmc_d_out_2(fsmc_d_out_2), 
            .fsmc_d_out_3(fsmc_d_out_3), .fsmc_d_out_4(fsmc_d_out_4), .fsmc_d_out_5(fsmc_d_out_5), 
            .fsmc_d_out_6(fsmc_d_out_6), .fsmc_d_out_7(fsmc_d_out_7), .fsmc_d_out_8(fsmc_d_out_8), 
            .fsmc_d_out_9(fsmc_d_out_9), .fsmc_d_out_10(fsmc_d_out_10), 
            .fsmc_d_out_11(fsmc_d_out_11), .fsmc_d_out_12(fsmc_d_out_12), 
            .fsmc_d_out_13(fsmc_d_out_13), .fsmc_d_out_14(fsmc_d_out_14), 
            .fsmc_d_out_15(fsmc_d_out_15), .n2978(n2978), .n2979(n2979), 
            .n2980(n2980), .n2981(n2981), .n2982(n2982), .n2983(n2983), 
            .n2984(n2984), .n2985(n2985), .n2986(n2986), .n2987(n2987), 
            .n2988(n2988), .n2989(n2989), .n2990(n2990), .n2991(n2991), 
            .n2992(n2992), .n2993(n2993), .fsmc_clk_c_enable_2(fsmc_clk_c_enable_2), 
            .n9422(n9422), .n9426(n9426), .n2994(n2994), .n2995(n2995), 
            .n2996(n2996), .n2997(n2997), .n2998(n2998), .n2999(n2999), 
            .n3000(n3000), .n3001(n3001), .n3002(n3002), .n3003(n3003), 
            .n3004(n3004), .n3005(n3005), .n3006(n3006), .n3007(n3007), 
            .n3008(n3008), .n3009(n3009), .n3010(n3010));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(898[11:23])
    LUT4 i5992_3_lut (.A(s_cfg[1]), .B(s_tmr[1]), .C(q_15__N_82), .Z(n8049)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5992_3_lut.init = 16'hcaca;
    LUT4 i5991_3_lut (.A(s_ckgen_div[1]), .B(s_ckgen_rtdiv[1]), .C(q_15__N_82), 
         .Z(n8048)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5991_3_lut.init = 16'hcaca;
    LUT4 i5971_3_lut (.A(s_desc_lo[1]), .B(s_desc_hi[1]), .C(q_15__N_82), 
         .Z(n8028)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5971_3_lut.init = 16'hcaca;
    LUT4 i5990_3_lut (.A(s_irq_en[1]), .B(s_r3[1]), .C(q_15__N_82), .Z(n8047)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5990_3_lut.init = 16'hcaca;
    PUR PUR_INST (.PUR(VCC_net));
    defparam PUR_INST.RST_PULSE = 1;
    LUT4 s_desc_lo_9__bdd_3_lut (.A(s_mem_wr_addr[9]), .B(s_mem_rd_addr[9]), 
         .C(q_15__N_82), .Z(n8373)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_9__bdd_3_lut.init = 16'hcaca;
    LUT4 m1_lut (.Z(n9420)) /* synthesis lut_function=1, syn_instantiated=1 */ ;
    defparam m1_lut.init = 16'hffff;
    LUT4 i5989_3_lut (.A(s_status[1]), .B(s_irq_st[1]), .C(q_15__N_82), 
         .Z(n8046)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5989_3_lut.init = 16'hcaca;
    PFUMX i5867 (.BLUT(n7922), .ALUT(n7923), .C0(q_15__N_80), .Z(n7924));
    PFUMX i5873 (.BLUT(n7928), .ALUT(n7929), .C0(q_15__N_80), .Z(n7930));
    LUT4 i5806_3_lut (.A(s_cfg[15]), .B(s_tmr[15]), .C(q_15__N_82), .Z(n7863)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5806_3_lut.init = 16'hcaca;
    LUT4 i5805_3_lut (.A(s_ckgen_div[15]), .B(s_ckgen_rtdiv[15]), .C(q_15__N_82), 
         .Z(n7862)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5805_3_lut.init = 16'hcaca;
    LUT4 i5985_3_lut (.A(s_cfg[2]), .B(s_tmr[2]), .C(q_15__N_82), .Z(n8042)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5985_3_lut.init = 16'hcaca;
    LUT4 i5984_3_lut (.A(s_ckgen_div[2]), .B(s_ckgen_rtdiv[2]), .C(q_15__N_82), 
         .Z(n8041)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5984_3_lut.init = 16'hcaca;
    LUT4 s_desc_lo_9__bdd_3_lut_6387 (.A(s_desc_lo[9]), .B(q_15__N_82), 
         .C(s_desc_hi[9]), .Z(n8372)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_9__bdd_3_lut_6387.init = 16'he2e2;
    LUT4 s_reg_rd_sel_3__I_0_Mux_0_i15_4_lut (.A(n7998), .B(n9), .C(q_15__N_76), 
         .D(n3411), .Z(s_reg_dout[0])) /* synthesis lut_function=(A (B ((D)+!C)+!B !(C))+!A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_0_i15_4_lut.init = 16'hca0a;
    LUT4 s_reg_rd_sel_3__I_0_Mux_0_i9_3_lut (.A(s_desc_lo[0]), .B(s_desc_hi[0]), 
         .C(q_15__N_82), .Z(n9)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam s_reg_rd_sel_3__I_0_Mux_0_i9_3_lut.init = 16'hcaca;
    PFUMX i6399 (.BLUT(n8387), .ALUT(n8383), .C0(n1938), .Z(n8388));
    LUT4 m0_lut (.Z(n9419)) /* synthesis lut_function=0, syn_instantiated=1 */ ;
    defparam m0_lut.init = 16'h0000;
    \jtag_enh_shifter(16,10,10,16,8,32,8,16,16)  shifter (.n2993(n2993), .n3009(n3009), 
            .n3010(n3010), .n2992(n2992), .n3008(n3008), .n2991(n2991), 
            .n3007(n3007), .n2990(n2990), .n3006(n3006), .n2989(n2989), 
            .n3005(n3005), .n2988(n2988), .n3004(n3004), .n2987(n2987), 
            .n3003(n3003), .n2986(n2986), .n3002(n3002), .n2985(n2985), 
            .n3001(n3001), .n2984(n2984), .n3000(n3000), .n2983(n2983), 
            .n2999(n2999), .n2982(n2982), .n2998(n2998), .n2981(n2981), 
            .n2997(n2997), .n2980(n2980), .n2996(n2996), .n2979(n2979), 
            .n2995(n2995), .n2978(n2978), .n2994(n2994), .GND_net(GND_net), 
            .\s_desc_data[18] (s_desc_data[18]), .\s_desc_data[19] (s_desc_data[19]), 
            .s_addr_0({s_addr_0}), .\s_desc_data[16] (s_desc_data[16]), 
            .\s_desc_data[17] (s_desc_data[17]), .mclk_c(mclk_c), .\s_desc_data[14] (s_desc_data[14]), 
            .\s_desc_data[15] (s_desc_data[15]), .\s_desc_data[12] (s_desc_data[12]), 
            .\s_desc_data[13] (s_desc_data[13]), .\s_desc_data[10] (s_desc_data[10]), 
            .\s_desc_data[11] (s_desc_data[11]), .\s_desc_data[8] (s_desc_data[8]), 
            .\s_desc_data[30] (s_desc_data[30]), .\s_desc_data[9] (s_desc_data[9]), 
            .\s_desc_data[6] (s_desc_data[6]), .\s_desc_data[7] (s_desc_data[7]), 
            .\s_desc_data[4] (s_desc_data[4]), .\s_desc_data[5] (s_desc_data[5]), 
            .s_pos_0_3__N_670({s_pos_0_3__N_670}), .s_ptr_data({s_ptr_data}), 
            .s_desc_addr({s_desc_addr}), .n9428(n9428), .mclk_c_enable_112(mclk_c_enable_112), 
            .s_rx_mem_wr_stb(s_rx_mem_wr_stb), .\s_desc_data[31] (s_desc_data[31]), 
            .n9419(n9419), .s_rx_mem_addr({s_rx_mem_addr}), .s_rx_mem_din({s_rx_mem_din}), 
            .\s_desc_data[29] (s_desc_data[29]), .\s_desc_data[27] (s_desc_data[27]), 
            .\s_desc_data[28] (s_desc_data[28]), .\s_desc_data[25] (s_desc_data[25]), 
            .\s_desc_data[26] (s_desc_data[26]), .\s_desc_data[23] (s_desc_data[23]), 
            .\s_desc_data[24] (s_desc_data[24]), .\s_desc_data[21] (s_desc_data[21]), 
            .\s_desc_data[22] (s_desc_data[22]), .\s_desc_data[20] (s_desc_data[20]), 
            .n9420(n9420), .mclk_c_enable_81(mclk_c_enable_81), .tp_tms_c(tp_tms_c), 
            .s_tap_tdo(s_tap_tdo), .s_tap_tdi(s_tap_tdi), .\s_status[2] (s_status[2]), 
            .\s_status[3] (s_status[3]), .\s_status[0] (s_status[0]), .\s_status[1] (s_status[1]), 
            .s_end_state({s_end_state}), .s_reg_din({s_reg_din}), .n8820(n8820), 
            .n8832(n8832), .s_tap_busy(s_tap_busy), .n8829(n8829), .n8828(n8828), 
            .n8856(n8856), .out_get_N_312(out_get_N_312), .n7771(n7771), 
            .n2(n2_adj_1634), .n5312(n5312), .s_ptr_addr({s_ptr_addr}), 
            .s_cnt_en_N_1514(s_cnt_en_N_1514), .\s_goto_state[2] (s_goto_state[2]), 
            .\s_goto_state[0] (s_goto_state[0]), .\s_goto_state[1] (s_goto_state[1]), 
            .n1(n1_adj_1637), .n7781(n7781), .s_ckgen_div({s_ckgen_div}), 
            .\s_irq[1] (s_irq[1]), .s_ckgen_rtdiv({s_ckgen_rtdiv}), .\s_cfg[5] (s_cfg[5]), 
            .tp_rtck_c(tp_rtck_c), .tp_tck_c(tp_tck_c), .\s_irq_en[1] (s_irq_en[1]), 
            .\irq_N_8[1] (irq_N_8[1]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(953[12:35])
    PFUMX i5882 (.BLUT(n7937), .ALUT(n7938), .C0(q_15__N_80), .Z(n7939));
    PFUMX i5885 (.BLUT(n7940), .ALUT(n7941), .C0(q_15__N_80), .Z(n7942));
    PFUMX i5891 (.BLUT(n7946), .ALUT(n7947), .C0(q_15__N_80), .Z(n7948));
    PFUMX i5900 (.BLUT(n7955), .ALUT(n7956), .C0(q_15__N_80), .Z(n7957));
    LUT4 i1319_2_lut (.A(q_15__N_80), .B(q_15__N_78), .Z(n3411)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(556[2] 569[31])
    defparam i1319_2_lut.init = 16'h2222;
    \jtag_clk_scaler(60)  cklgen_1mhz (.mclk_c(mclk_c), .s_1mhz_stb(s_1mhz_stb), 
            .GND_net(GND_net));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1010[16:38])
    \dpram(16,10)_U1  rx_mem (.mclk_c(mclk_c), .fsmc_clk_c(fsmc_clk_c), 
            .VCC_net(VCC_net), .GND_net(GND_net), .s_rx_mem_addr({s_rx_mem_addr}), 
            .q_15__N_82(q_15__N_82), .q_15__N_80(q_15__N_80), .q_15__N_78(q_15__N_78), 
            .q_15__N_76(q_15__N_76), .q_15__N_74(q_15__N_74), .q_15__N_72(q_15__N_72), 
            .q_15__N_70(q_15__N_70), .q_15__N_68(q_15__N_68), .q_15__N_66(q_15__N_66), 
            .s_rx_mem_din({s_rx_mem_din}), .n2957(n2957), .n2958(n2958), 
            .n2959(n2959), .n2960(n2960), .n2961(n2961), .n2962(n2962), 
            .n2963(n2963), .n2964(n2964), .n2965(n2965), .n2966(n2966), 
            .n2967(n2967), .n2968(n2968), .n2969(n2969), .n2970(n2970), 
            .n2971(n2971), .n2972(n2972), .n2941(n2941), .n2942(n2942), 
            .n2943(n2943), .n2944(n2944), .n2945(n2945), .n2946(n2946), 
            .n2947(n2947), .n2948(n2948), .n2949(n2949), .n2950(n2950), 
            .n2951(n2951), .n2952(n2952), .n2953(n2953), .n2954(n2954), 
            .n2955(n2955), .n2956(n2956), .n2973(n2973), .q_15__N_64(q_15__N_64), 
            .s_rx_mem_wr_stb(s_rx_mem_wr_stb));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(879[11:23])
    PFUMX i5939 (.BLUT(n7992), .ALUT(n7993), .C0(q_15__N_80), .Z(n7996));
    PFUMX i5940 (.BLUT(n7994), .ALUT(n7995), .C0(q_15__N_80), .Z(n7997));
    PFUMX i5947 (.BLUT(n8002), .ALUT(n8003), .C0(q_15__N_80), .Z(n8004));
    PFUMX i5956 (.BLUT(n8011), .ALUT(n8012), .C0(q_15__N_80), .Z(n8013));
    PFUMX i5961 (.BLUT(n8014), .ALUT(n8015), .C0(q_15__N_80), .Z(n8018));
    PFUMX i5962 (.BLUT(n8016), .ALUT(n8017), .C0(q_15__N_80), .Z(n8019));
    
endmodule
//
// Verilog Description of module ddr_sdram_dual
//

module ddr_sdram_dual (clk_i, rst_i, cmd_stb_i, cmd_ack_o, cmd_stall_o, 
            cmd_dat_i, mwr_cyc_i, mwr_we_i, mwr_stb_i, mwr_ack_o, 
            mwr_stall_o, mwr_adr_i, mwr_dat_i, mwr_bst_i, mrd_cyc_i, 
            mrd_we_i, mrd_stb_i, mrd_ack_o, mrd_stall_o, mrd_adr_i, 
            mrd_dat_o, mrd_bst_i, phy_burstdet0_i, phy_burstdet1_i, 
            phy_casn_o, phy_dtri0_o, phy_dtri1_o, phy_dvalid0_o, phy_dvalid1_o, 
            phy_dqs0_o, phy_dqs1_o, phy_dqstri0_o, phy_dqstri1_o, phy_freeze_o, 
            phy_lock_i, phy_rasn_o, phy_read0_o, phy_read1_o, phy_readclksel00_o, 
            phy_readclksel01_o, phy_readclksel10_o, phy_readclksel11_o, 
            phy_uddcntln_o, phy_wen_o, phy_addr_o, phy_ba_o, phy_cke_o, 
            phy_csn0_o, phy_csn1_o, phy_dat0_i, phy_dat1_i, phy_dat0_o, 
            phy_dat1_o);   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(67[8:22])
    input clk_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(70[3:8])
    input rst_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(71[3:8])
    input cmd_stb_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(73[3:12])
    output cmd_ack_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(74[3:12])
    output cmd_stall_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(75[3:14])
    input [15:0]cmd_dat_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(76[3:12])
    input mwr_cyc_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(78[3:12])
    input mwr_we_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(79[3:11])
    input mwr_stb_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(80[3:12])
    output mwr_ack_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(81[3:12])
    output mwr_stall_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(82[3:14])
    input [20:0]mwr_adr_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(83[3:12])
    input [31:0]mwr_dat_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(84[3:12])
    input mwr_bst_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(85[3:12])
    input mrd_cyc_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(87[3:12])
    input mrd_we_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(88[3:11])
    input mrd_stb_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(89[3:12])
    output mrd_ack_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(90[3:12])
    output mrd_stall_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(91[3:14])
    input [20:0]mrd_adr_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(92[3:12])
    output [31:0]mrd_dat_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(93[3:12])
    input mrd_bst_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(94[3:12])
    input phy_burstdet0_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(96[3:18])
    input phy_burstdet1_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(97[3:18])
    output phy_casn_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(98[3:13])
    output phy_dtri0_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(99[3:14])
    output phy_dtri1_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(100[3:14])
    output phy_dvalid0_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(101[3:16])
    output phy_dvalid1_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(102[3:16])
    output phy_dqs0_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(103[3:13])
    output phy_dqs1_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(104[3:13])
    output phy_dqstri0_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(105[3:16])
    output phy_dqstri1_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(106[3:16])
    output phy_freeze_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(107[3:15])
    input phy_lock_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(108[3:13])
    output phy_rasn_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(109[3:13])
    output phy_read0_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(110[3:14])
    output phy_read1_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(111[3:14])
    output phy_readclksel00_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(112[3:21])
    output phy_readclksel01_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(113[3:21])
    output phy_readclksel10_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(114[3:21])
    output phy_readclksel11_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(115[3:21])
    output phy_uddcntln_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(116[3:17])
    output phy_wen_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(117[3:12])
    output [12:0]phy_addr_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(118[3:13])
    output [1:0]phy_ba_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(119[3:11])
    output phy_cke_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(120[3:12])
    output phy_csn0_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(121[3:13])
    output phy_csn1_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(122[3:13])
    input [15:0]phy_dat0_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(123[3:13])
    input [15:0]phy_dat1_i;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(124[3:13])
    output [15:0]phy_dat0_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(125[3:13])
    output [15:0]phy_dat1_o;   // c:/devel/yard-ice/fpga/lattice/vhdl/ddr_sdram_dual.vhdl(126[3:13])
    
    
    
endmodule
//
// Verilog Description of module \jtag_led_drv(31,false,false,false) 
//

module \jtag_led_drv(31,false,false,false)  (mclk_c, led_1_c, out_get_N_312, 
            s_1khz_stb);
    input mclk_c;
    output led_1_c;
    input out_get_N_312;
    input s_1khz_stb;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    wire [4:0]s_count;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(59[9:16])
    
    wire s_cnt_ld, n3509, mclk_c_enable_33, n4080, s_trip_r1, s_trip_r0, 
        n4418, n4078, n4076, n4082, n6, n4015, n8867, n8870;
    
    FD1S3JX s_count_i0_i0 (.D(n3509), .CK(mclk_c), .PD(s_cnt_ld), .Q(s_count[0])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1049, LSE_RLINE=1049 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i0.GSR = "ENABLED";
    FD1P3JX s_count_i0_i4 (.D(n4080), .SP(mclk_c_enable_33), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[4])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1049, LSE_RLINE=1049 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i4.GSR = "ENABLED";
    FD1S3AX s_trip_r1_31 (.D(s_trip_r0), .CK(mclk_c), .Q(s_trip_r1)) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1049, LSE_RLINE=1049 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(95[3] 105[10])
    defparam s_trip_r1_31.GSR = "ENABLED";
    FD1S3AX s_drv_ff_32 (.D(n4418), .CK(mclk_c), .Q(led_1_c)) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1049, LSE_RLINE=1049 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(113[3] 121[10])
    defparam s_drv_ff_32.GSR = "ENABLED";
    FD1S3AX s_trip_r0_30 (.D(out_get_N_312), .CK(mclk_c), .Q(s_trip_r0)) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1049, LSE_RLINE=1049 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(95[3] 105[10])
    defparam s_trip_r0_30.GSR = "ENABLED";
    FD1P3JX s_count_i0_i3 (.D(n4078), .SP(mclk_c_enable_33), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[3])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1049, LSE_RLINE=1049 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i3.GSR = "ENABLED";
    FD1P3JX s_count_i0_i2 (.D(n4076), .SP(mclk_c_enable_33), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[2])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1049, LSE_RLINE=1049 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i2.GSR = "ENABLED";
    FD1P3JX s_count_i0_i1 (.D(n4082), .SP(mclk_c_enable_33), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[1])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1049, LSE_RLINE=1049 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i1.GSR = "ENABLED";
    LUT4 i4_4_lut (.A(s_count[3]), .B(s_count[2]), .C(s_count[0]), .D(n6), 
         .Z(n4015)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(140[22:45])
    defparam i4_4_lut.init = 16'hfffe;
    LUT4 i1_2_lut (.A(s_count[1]), .B(s_count[4]), .Z(n6)) /* synthesis lut_function=(A+(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(140[22:45])
    defparam i1_2_lut.init = 16'heeee;
    LUT4 i1_3_lut_4_lut (.A(s_count[2]), .B(n8867), .C(s_count[3]), .D(s_count[4]), 
         .Z(n4080)) /* synthesis lut_function=(A (D)+!A (B (D)+!B (C (D)+!C !(D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i1_3_lut_4_lut.init = 16'hfe01;
    LUT4 i1_2_lut_adj_204 (.A(s_count[1]), .B(s_count[0]), .Z(n4082)) /* synthesis lut_function=(A (B)+!A !(B)) */ ;
    defparam i1_2_lut_adj_204.init = 16'h9999;
    LUT4 i950_2_lut_rep_200 (.A(s_count[1]), .B(s_count[0]), .Z(n8867)) /* synthesis lut_function=(A+(B)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i950_2_lut_rep_200.init = 16'heeee;
    LUT4 i1_2_lut_3_lut (.A(s_count[1]), .B(s_count[0]), .C(s_count[2]), 
         .Z(n4076)) /* synthesis lut_function=(A (C)+!A (B (C)+!B !(C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i1_2_lut_3_lut.init = 16'he1e1;
    LUT4 i1_2_lut_3_lut_4_lut (.A(s_count[1]), .B(s_count[0]), .C(s_count[3]), 
         .D(s_count[2]), .Z(n4078)) /* synthesis lut_function=(A (C)+!A (B (C)+!B (C (D)+!C !(D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i1_2_lut_3_lut_4_lut.init = 16'hf0e1;
    LUT4 z_I_0_2_lut_3_lut (.A(s_trip_r0), .B(s_trip_r1), .C(n4015), .Z(s_cnt_ld)) /* synthesis lut_function=(!(A (B (C))+!A (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(108[15:42])
    defparam z_I_0_2_lut_3_lut.init = 16'h2f2f;
    LUT4 i2_3_lut_4_lut (.A(s_trip_r0), .B(s_trip_r1), .C(n4015), .D(n8870), 
         .Z(mclk_c_enable_33)) /* synthesis lut_function=(A (((D)+!C)+!B)+!A ((D)+!C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(108[15:42])
    defparam i2_3_lut_4_lut.init = 16'hff2f;
    LUT4 i2318_3_lut_4_lut (.A(s_trip_r0), .B(s_trip_r1), .C(led_1_c), 
         .D(n4015), .Z(n4418)) /* synthesis lut_function=(A ((C (D))+!B)+!A (C (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(108[15:42])
    defparam i2318_3_lut_4_lut.init = 16'hf222;
    LUT4 i23_2_lut_rep_203 (.A(s_1khz_stb), .B(led_1_c), .Z(n8870)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(134[10:41])
    defparam i23_2_lut_rep_203.init = 16'h8888;
    LUT4 i1801_2_lut_3_lut (.A(s_1khz_stb), .B(led_1_c), .C(s_count[0]), 
         .Z(n3509)) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(134[10:41])
    defparam i1801_2_lut_3_lut.init = 16'h7878;
    
endmodule
//
// Verilog Description of module \jtag_reg(16,12) 
//

module \jtag_reg(16,12)  (\s_mem_wr_addr[1] , fsmc_clk_c, s_dbg_mem_wr, 
            q_15__N_82, \s_mem_wr_addr[11] , q_15__N_62, \s_mem_wr_addr[10] , 
            q_15__N_64, \s_mem_wr_addr[9] , q_15__N_66, \s_mem_wr_addr[8] , 
            q_15__N_68, \s_mem_wr_addr[7] , q_15__N_70, \s_mem_wr_addr[6] , 
            q_15__N_72, \s_mem_wr_addr[5] , q_15__N_74, \s_mem_wr_addr[4] , 
            q_15__N_76, \s_mem_wr_addr[3] , q_15__N_78, \s_mem_wr_addr[2] , 
            q_15__N_80);
    output \s_mem_wr_addr[1] ;
    input fsmc_clk_c;
    input s_dbg_mem_wr;
    input q_15__N_82;
    output \s_mem_wr_addr[11] ;
    input q_15__N_62;
    output \s_mem_wr_addr[10] ;
    input q_15__N_64;
    output \s_mem_wr_addr[9] ;
    input q_15__N_66;
    output \s_mem_wr_addr[8] ;
    input q_15__N_68;
    output \s_mem_wr_addr[7] ;
    input q_15__N_70;
    output \s_mem_wr_addr[6] ;
    input q_15__N_72;
    output \s_mem_wr_addr[5] ;
    input q_15__N_74;
    output \s_mem_wr_addr[4] ;
    input q_15__N_76;
    output \s_mem_wr_addr[3] ;
    input q_15__N_78;
    output \s_mem_wr_addr[2] ;
    input q_15__N_80;
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(73[2:10])
    
    FD1P3AX s_reg__i1 (.D(q_15__N_82), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[1] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=744, LSE_RLINE=744 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i1.GSR = "ENABLED";
    FD1P3AX s_reg__i11 (.D(q_15__N_62), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[11] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=744, LSE_RLINE=744 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i11.GSR = "ENABLED";
    FD1P3AX s_reg__i10 (.D(q_15__N_64), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[10] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=744, LSE_RLINE=744 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i10.GSR = "ENABLED";
    FD1P3AX s_reg__i9 (.D(q_15__N_66), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[9] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=744, LSE_RLINE=744 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i9.GSR = "ENABLED";
    FD1P3AX s_reg__i8 (.D(q_15__N_68), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[8] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=744, LSE_RLINE=744 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i8.GSR = "ENABLED";
    FD1P3AX s_reg__i7 (.D(q_15__N_70), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[7] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=744, LSE_RLINE=744 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i7.GSR = "ENABLED";
    FD1P3AX s_reg__i6 (.D(q_15__N_72), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[6] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=744, LSE_RLINE=744 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i6.GSR = "ENABLED";
    FD1P3AX s_reg__i5 (.D(q_15__N_74), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[5] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=744, LSE_RLINE=744 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i5.GSR = "ENABLED";
    FD1P3AX s_reg__i4 (.D(q_15__N_76), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[4] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=744, LSE_RLINE=744 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i4.GSR = "ENABLED";
    FD1P3AX s_reg__i3 (.D(q_15__N_78), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[3] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=744, LSE_RLINE=744 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i3.GSR = "ENABLED";
    FD1P3AX s_reg__i2 (.D(q_15__N_80), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[2] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=744, LSE_RLINE=744 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i2.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module \jtag_reg(16,16)_U4 
//

module \jtag_reg(16,16)_U4  (s_desc_lo, mclk_c, mclk_c_enable_112, \s_desc_data[9] , 
            \s_desc_data[8] , \s_desc_data[7] , \s_desc_data[6] , \s_desc_data[5] , 
            \s_desc_data[4] , s_pos_0_3__N_670, \s_desc_data[12] , \s_desc_data[11] , 
            \s_desc_data[15] , \s_desc_data[10] , \s_desc_data[14] , \s_desc_data[13] );
    output [15:0]s_desc_lo;
    input mclk_c;
    input mclk_c_enable_112;
    input \s_desc_data[9] ;
    input \s_desc_data[8] ;
    input \s_desc_data[7] ;
    input \s_desc_data[6] ;
    input \s_desc_data[5] ;
    input \s_desc_data[4] ;
    input [3:0]s_pos_0_3__N_670;
    input \s_desc_data[12] ;
    input \s_desc_data[11] ;
    input \s_desc_data[15] ;
    input \s_desc_data[10] ;
    input \s_desc_data[14] ;
    input \s_desc_data[13] ;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    FD1P3AX s_reg_i0_i9 (.D(\s_desc_data[9] ), .SP(mclk_c_enable_112), .CK(mclk_c), 
            .Q(s_desc_lo[9])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i9.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i8 (.D(\s_desc_data[8] ), .SP(mclk_c_enable_112), .CK(mclk_c), 
            .Q(s_desc_lo[8])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i8.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i7 (.D(\s_desc_data[7] ), .SP(mclk_c_enable_112), .CK(mclk_c), 
            .Q(s_desc_lo[7])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i7.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i6 (.D(\s_desc_data[6] ), .SP(mclk_c_enable_112), .CK(mclk_c), 
            .Q(s_desc_lo[6])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i6.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i5 (.D(\s_desc_data[5] ), .SP(mclk_c_enable_112), .CK(mclk_c), 
            .Q(s_desc_lo[5])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i5.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i4 (.D(\s_desc_data[4] ), .SP(mclk_c_enable_112), .CK(mclk_c), 
            .Q(s_desc_lo[4])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i4.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i3 (.D(s_pos_0_3__N_670[3]), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_lo[3])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i3.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i2 (.D(s_pos_0_3__N_670[2]), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_lo[2])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i2.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i1 (.D(s_pos_0_3__N_670[1]), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_lo[1])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i1.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i0 (.D(s_pos_0_3__N_670[0]), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_lo[0])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i0.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i12 (.D(\s_desc_data[12] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_lo[12])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i12.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i11 (.D(\s_desc_data[11] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_lo[11])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i11.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i15 (.D(\s_desc_data[15] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_lo[15])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i15.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i10 (.D(\s_desc_data[10] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_lo[10])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i10.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i14 (.D(\s_desc_data[14] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_lo[14])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i14.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i13 (.D(\s_desc_data[13] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_lo[13])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=774, LSE_RLINE=774 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i13.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module \cram_adaptor(2,12,4) 
//

module \cram_adaptor(2,12,4)  (s_mem_rd_sel, fsmc_clk_c, q_15__N_56, s_reg_dout, 
            fsmc_nce_c, n9419, s_cram_next_st_1__N_24_c_0, s_dbg_mem_rd, 
            s_mem_dout, s_cram_dout, fsmc_d_out_15, fsmc_noe_c, n3239, 
            q_15__N_52, n9420, s_ptr_mem_wr_stb, s_aux_mem_wr_stb, n2973, 
            n2972, n2956, n1, n2971, n2955, n1_adj_1, n2970, n2954, 
            n1_adj_2, n2969, n2953, n1_adj_3, n2968, n2952, n1_adj_4, 
            n2967, n2951, n1_adj_5, n2966, n2950, n1_adj_6, n2965, 
            n2949, n1_adj_7, n2964, n2948, n1_adj_8, n2963, n2947, 
            n1_adj_9, n2962, n2946, n1_adj_10, n2961, n2945, n1_adj_11, 
            n2960, n2944, n1_adj_12, n2959, n2943, n1_adj_13, n2958, 
            n2942, n1_adj_14, n2957, n2941, n1_adj_15, n9428, n8843, 
            s_dbg_mem_wr, mclk_c, out_get_N_312, n1938, \s_addr0[0] , 
            q_15__N_82, \s_addr1[0] , s_addr2, s_addr3, fsmc_d_out_0, 
            n1940, n1939, n8862, n8838, n8726, n8387, n8731, n8365, 
            n8733, n8340, \s_addr0[1] , q_15__N_80, q_15__N_78, q_15__N_76, 
            \s_addr1[1] , fsmc_d_out_1, fsmc_d_out_2, fsmc_d_out_3, 
            fsmc_d_out_4, fsmc_d_out_5, fsmc_d_out_6, fsmc_d_out_7, 
            fsmc_d_out_8, fsmc_d_out_9, fsmc_d_out_10, fsmc_d_out_11, 
            fsmc_d_out_12, fsmc_d_out_13, fsmc_d_out_14, n8863, n20, 
            n22, s_reg_din, n9421, q_15__N_74, q_15__N_72, q_15__N_70, 
            q_15__N_68, q_15__N_66, q_15__N_64, q_15__N_62, n9426, 
            \s_tmr[6] , \s_r3[6] , n8389, n8390, n8741, n8004, n8020, 
            n8013, n8045, n8395, n8396, n8740, n8398, n8399, n8739, 
            n8404, n8405, n8738, n8029, n8052, n8342, n8343, n8737, 
            n8345, n8346, n8736, n8372, n8373, n8735, n7942, n7, 
            n9425, n9424, n9423, n9422);
    output [1:0]s_mem_rd_sel;
    input fsmc_clk_c;
    output q_15__N_56;
    input [15:0]s_reg_dout;
    input fsmc_nce_c;
    input n9419;
    input s_cram_next_st_1__N_24_c_0;
    output s_dbg_mem_rd;
    input [15:0]s_mem_dout;
    output [15:0]s_cram_dout;
    input fsmc_d_out_15;
    input fsmc_noe_c;
    output n3239;
    output q_15__N_52;
    input n9420;
    output s_ptr_mem_wr_stb;
    output s_aux_mem_wr_stb;
    input n2973;
    input n2972;
    input n2956;
    output n1;
    input n2971;
    input n2955;
    output n1_adj_1;
    input n2970;
    input n2954;
    output n1_adj_2;
    input n2969;
    input n2953;
    output n1_adj_3;
    input n2968;
    input n2952;
    output n1_adj_4;
    input n2967;
    input n2951;
    output n1_adj_5;
    input n2966;
    input n2950;
    output n1_adj_6;
    input n2965;
    input n2949;
    output n1_adj_7;
    input n2964;
    input n2948;
    output n1_adj_8;
    input n2963;
    input n2947;
    output n1_adj_9;
    input n2962;
    input n2946;
    output n1_adj_10;
    input n2961;
    input n2945;
    output n1_adj_11;
    input n2960;
    input n2944;
    output n1_adj_12;
    input n2959;
    input n2943;
    output n1_adj_13;
    input n2958;
    input n2942;
    output n1_adj_14;
    input n2957;
    input n2941;
    output n1_adj_15;
    input n9428;
    output n8843;
    output s_dbg_mem_wr;
    input mclk_c;
    output out_get_N_312;
    output n1938;
    output \s_addr0[0] ;
    output q_15__N_82;
    output \s_addr1[0] ;
    output [3:0]s_addr2;
    output [3:0]s_addr3;
    input fsmc_d_out_0;
    output n1940;
    output n1939;
    output n8862;
    output n8838;
    input n8726;
    output n8387;
    input n8731;
    output n8365;
    input n8733;
    output n8340;
    output \s_addr0[1] ;
    output q_15__N_80;
    output q_15__N_78;
    output q_15__N_76;
    output \s_addr1[1] ;
    input fsmc_d_out_1;
    input fsmc_d_out_2;
    input fsmc_d_out_3;
    input fsmc_d_out_4;
    input fsmc_d_out_5;
    input fsmc_d_out_6;
    input fsmc_d_out_7;
    input fsmc_d_out_8;
    input fsmc_d_out_9;
    input fsmc_d_out_10;
    input fsmc_d_out_11;
    input fsmc_d_out_12;
    input fsmc_d_out_13;
    input fsmc_d_out_14;
    output n8863;
    input n20;
    output n22;
    output [15:0]s_reg_din;
    output n9421;
    output q_15__N_74;
    output q_15__N_72;
    output q_15__N_70;
    output q_15__N_68;
    output q_15__N_66;
    output q_15__N_64;
    output q_15__N_62;
    output n9426;
    input \s_tmr[6] ;
    input \s_r3[6] ;
    input n8389;
    input n8390;
    output n8741;
    input n8004;
    input n8020;
    input n8013;
    input n8045;
    input n8395;
    input n8396;
    output n8740;
    input n8398;
    input n8399;
    output n8739;
    input n8404;
    input n8405;
    output n8738;
    input n8029;
    input n8052;
    input n8342;
    input n8343;
    output n8737;
    input n8345;
    input n8346;
    output n8736;
    input n8372;
    input n8373;
    output n8735;
    input n7942;
    output n7;
    output n9425;
    output n9424;
    output n9423;
    output n9422;
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(73[2:10])
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    wire s_start_stb, s_cram_adv;
    wire [15:0]s_reg_rd_data;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(116[9:22])
    
    wire s_op_mem_rd, n2820, s_op_mem_rd_N_35;
    wire [3:0]n1896;
    
    wire s_op_mem_wr_N_42, s_op_mem_wr;
    wire [15:0]s_reg_dout_c;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(222[9:19])
    wire [15:0]s_cram_mem_rd_addr;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(104[9:27])
    wire [15:0]s_cram_wr_addr;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(103[9:23])
    
    wire n1903, s_cram_adi, fsmc_clk_c_enable_11, n8875, n8840, s_fifo_wr_stb;
    
    FD1S3AX s_cram_mem_rd_addr_i1 (.D(q_15__N_56), .CK(fsmc_clk_c), .Q(s_mem_rd_sel[0])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam s_cram_mem_rd_addr_i1.GSR = "ENABLED";
    FD1S3AX s_start_stb_136 (.D(s_cram_adv), .CK(fsmc_clk_c), .Q(s_start_stb)) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(200[3] 204[10])
    defparam s_start_stb_136.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i0 (.D(s_reg_dout[0]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[0])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i0.GSR = "ENABLED";
    FD1P3IX s_op_mem_rd_133 (.D(s_op_mem_rd_N_35), .SP(s_cram_adv), .CD(n2820), 
            .CK(fsmc_clk_c), .Q(s_op_mem_rd));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(185[3] 195[10])
    defparam s_op_mem_rd_133.GSR = "ENABLED";
    FD1S3BX s_cram_st_FSM_i0 (.D(n9419), .CK(fsmc_clk_c), .PD(fsmc_nce_c), 
            .Q(n1896[0]));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(141[3] 154[12])
    defparam s_cram_st_FSM_i0.GSR = "DISABLED";
    FD1S3AX s_reg_rd_data_i15 (.D(s_reg_dout[15]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[15])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i15.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i14 (.D(s_reg_dout[14]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[14])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i14.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i13 (.D(s_reg_dout[13]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[13])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i13.GSR = "ENABLED";
    LUT4 i6240_2_lut (.A(s_cram_next_st_1__N_24_c_0), .B(fsmc_nce_c), .Z(s_op_mem_wr_N_42)) /* synthesis lut_function=(!(A+(B))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(191[20:41])
    defparam i6240_2_lut.init = 16'h1111;
    FD1S3AX s_reg_rd_data_i12 (.D(s_reg_dout[12]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[12])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i12.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i11 (.D(s_reg_dout[11]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[11])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i11.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i10 (.D(s_reg_dout[10]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[10])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i10.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i9 (.D(s_reg_dout[9]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[9])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i9.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i8 (.D(s_reg_dout[8]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[8])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i8.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i7 (.D(s_reg_dout[7]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[7])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i7.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i6 (.D(s_reg_dout[6]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[6])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i6.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i5 (.D(s_reg_dout[5]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[5])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i5.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i4 (.D(s_reg_dout[4]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[4])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i4.GSR = "ENABLED";
    FD1P3IX s_op_mem_wr_132 (.D(s_op_mem_wr_N_42), .SP(s_cram_adv), .CD(n2820), 
            .CK(fsmc_clk_c), .Q(s_op_mem_wr));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(185[3] 195[10])
    defparam s_op_mem_wr_132.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i3 (.D(s_reg_dout_c[3]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[3])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i3.GSR = "ENABLED";
    LUT4 s_start_stb_I_0_140_2_lut (.A(s_start_stb), .B(s_op_mem_rd), .Z(s_dbg_mem_rd)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(290[16:43])
    defparam s_start_stb_I_0_140_2_lut.init = 16'h8888;
    FD1S3AX s_reg_rd_data_i2 (.D(s_reg_dout_c[2]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[2])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i2.GSR = "ENABLED";
    LUT4 s_reg_rd_data_15__I_0_i14_3_lut (.A(s_mem_dout[13]), .B(s_reg_rd_data[13]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[13])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i14_3_lut.init = 16'hcaca;
    FD1S3AX s_reg_rd_data_i1 (.D(s_reg_dout_c[1]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[1])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i1.GSR = "ENABLED";
    FD1S3AX s_cram_mem_rd_addr_i3 (.D(s_cram_wr_addr[15]), .CK(fsmc_clk_c), 
            .Q(s_cram_mem_rd_addr[15])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam s_cram_mem_rd_addr_i3.GSR = "ENABLED";
    LUT4 i805_2_lut (.A(fsmc_d_out_15), .B(s_cram_adv), .Z(n2820)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(185[3] 195[10])
    defparam i805_2_lut.init = 16'h8888;
    LUT4 cram_nwe_I_0_149_2_lut (.A(s_cram_next_st_1__N_24_c_0), .B(fsmc_nce_c), 
         .Z(s_op_mem_rd_N_35)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(192[20:45])
    defparam cram_nwe_I_0_149_2_lut.init = 16'h2222;
    LUT4 s_reg_rd_data_15__I_0_i1_3_lut (.A(s_mem_dout[0]), .B(s_reg_rd_data[0]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[0])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i1_3_lut.init = 16'hcaca;
    LUT4 i1147_2_lut (.A(fsmc_noe_c), .B(fsmc_nce_c), .Z(n3239)) /* synthesis lut_function=(A+(B)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(283[16:37])
    defparam i1147_2_lut.init = 16'heeee;
    FD1S3AX s_cram_mem_rd_addr_i2 (.D(q_15__N_52), .CK(fsmc_clk_c), .Q(s_mem_rd_sel[1])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=447, LSE_RLINE=447 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam s_cram_mem_rd_addr_i2.GSR = "ENABLED";
    LUT4 s_reg_rd_data_15__I_0_i2_3_lut (.A(s_mem_dout[1]), .B(s_reg_rd_data[1]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[1])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i2_3_lut.init = 16'hcaca;
    LUT4 s_reg_rd_data_15__I_0_i3_3_lut (.A(s_mem_dout[2]), .B(s_reg_rd_data[2]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[2])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i3_3_lut.init = 16'hcaca;
    LUT4 s_reg_rd_data_15__I_0_i4_3_lut (.A(s_mem_dout[3]), .B(s_reg_rd_data[3]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[3])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i4_3_lut.init = 16'hcaca;
    LUT4 s_reg_rd_data_15__I_0_i5_3_lut (.A(s_mem_dout[4]), .B(s_reg_rd_data[4]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[4])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i5_3_lut.init = 16'hcaca;
    LUT4 s_reg_rd_data_15__I_0_i6_3_lut (.A(s_mem_dout[5]), .B(s_reg_rd_data[5]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[5])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i6_3_lut.init = 16'hcaca;
    LUT4 s_reg_rd_data_15__I_0_i7_3_lut (.A(s_mem_dout[6]), .B(s_reg_rd_data[6]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[6])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i7_3_lut.init = 16'hcaca;
    LUT4 s_reg_rd_data_15__I_0_i8_3_lut (.A(s_mem_dout[7]), .B(s_reg_rd_data[7]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[7])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i8_3_lut.init = 16'hcaca;
    LUT4 s_reg_rd_data_15__I_0_i9_3_lut (.A(s_mem_dout[8]), .B(s_reg_rd_data[8]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[8])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i9_3_lut.init = 16'hcaca;
    LUT4 s_reg_rd_data_15__I_0_i10_3_lut (.A(s_mem_dout[9]), .B(s_reg_rd_data[9]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[9])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i10_3_lut.init = 16'hcaca;
    LUT4 s_reg_rd_data_15__I_0_i11_3_lut (.A(s_mem_dout[10]), .B(s_reg_rd_data[10]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[10])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i11_3_lut.init = 16'hcaca;
    LUT4 s_reg_rd_data_15__I_0_i12_3_lut (.A(s_mem_dout[11]), .B(s_reg_rd_data[11]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[11])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i12_3_lut.init = 16'hcaca;
    LUT4 s_reg_rd_data_15__I_0_i13_3_lut (.A(s_mem_dout[12]), .B(s_reg_rd_data[12]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[12])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i13_3_lut.init = 16'hcaca;
    FD1S3DX s_cram_st_FSM_i1 (.D(n1896[0]), .CK(fsmc_clk_c), .CD(fsmc_nce_c), 
            .Q(s_cram_adv));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(141[3] 154[12])
    defparam s_cram_st_FSM_i1.GSR = "DISABLED";
    FD1S3DX s_cram_st_FSM_i2 (.D(n1903), .CK(fsmc_clk_c), .CD(fsmc_nce_c), 
            .Q(n1896[2]));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(141[3] 154[12])
    defparam s_cram_st_FSM_i2.GSR = "DISABLED";
    FD1P3DX s_cram_st_FSM_i3 (.D(n9420), .SP(fsmc_clk_c_enable_11), .CK(fsmc_clk_c), 
            .CD(fsmc_nce_c), .Q(s_cram_adi));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(141[3] 154[12])
    defparam s_cram_st_FSM_i3.GSR = "DISABLED";
    LUT4 i1_2_lut_3_lut_4_lut (.A(s_cram_wr_addr[15]), .B(n8875), .C(q_15__N_56), 
         .D(q_15__N_52), .Z(s_ptr_mem_wr_stb)) /* synthesis lut_function=(!(A+((C+!(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(159[25:69])
    defparam i1_2_lut_3_lut_4_lut.init = 16'h0400;
    LUT4 i1_2_lut_3_lut_4_lut_adj_203 (.A(s_cram_wr_addr[15]), .B(n8875), 
         .C(q_15__N_56), .D(q_15__N_52), .Z(s_aux_mem_wr_stb)) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(159[25:69])
    defparam i1_2_lut_3_lut_4_lut_adj_203.init = 16'h4000;
    LUT4 i3173_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2972), 
         .D(n2956), .Z(n1)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3173_4_lut_4_lut.init = 16'h5140;
    LUT4 i3174_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2971), 
         .D(n2955), .Z(n1_adj_1)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3174_4_lut_4_lut.init = 16'h5140;
    LUT4 i3176_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2970), 
         .D(n2954), .Z(n1_adj_2)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3176_4_lut_4_lut.init = 16'h5140;
    LUT4 i3185_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2969), 
         .D(n2953), .Z(n1_adj_3)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3185_4_lut_4_lut.init = 16'h5140;
    LUT4 i3188_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2968), 
         .D(n2952), .Z(n1_adj_4)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3188_4_lut_4_lut.init = 16'h5140;
    LUT4 i3196_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2967), 
         .D(n2951), .Z(n1_adj_5)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3196_4_lut_4_lut.init = 16'h5140;
    LUT4 i3197_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2966), 
         .D(n2950), .Z(n1_adj_6)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3197_4_lut_4_lut.init = 16'h5140;
    LUT4 i3198_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2965), 
         .D(n2949), .Z(n1_adj_7)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3198_4_lut_4_lut.init = 16'h5140;
    LUT4 i3199_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2964), 
         .D(n2948), .Z(n1_adj_8)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3199_4_lut_4_lut.init = 16'h5140;
    LUT4 i3201_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2963), 
         .D(n2947), .Z(n1_adj_9)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3201_4_lut_4_lut.init = 16'h5140;
    LUT4 i3206_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2962), 
         .D(n2946), .Z(n1_adj_10)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3206_4_lut_4_lut.init = 16'h5140;
    LUT4 i3207_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2961), 
         .D(n2945), .Z(n1_adj_11)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3207_4_lut_4_lut.init = 16'h5140;
    LUT4 i3208_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2960), 
         .D(n2944), .Z(n1_adj_12)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3208_4_lut_4_lut.init = 16'h5140;
    LUT4 i3210_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2959), 
         .D(n2943), .Z(n1_adj_13)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3210_4_lut_4_lut.init = 16'h5140;
    LUT4 i3217_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2958), 
         .D(n2942), .Z(n1_adj_14)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3217_4_lut_4_lut.init = 16'h5140;
    LUT4 i3292_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2973), .C(n2957), 
         .D(n2941), .Z(n1_adj_15)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3292_4_lut_4_lut.init = 16'h5140;
    LUT4 s_reg_rd_data_15__I_0_i15_3_lut (.A(s_mem_dout[14]), .B(s_reg_rd_data[14]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[14])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i15_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_rep_208 (.A(s_cram_adi), .B(s_cram_next_st_1__N_24_c_0), 
         .Z(n8875)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(159[25:69])
    defparam i1_2_lut_rep_208.init = 16'h2222;
    LUT4 i2_3_lut_rep_173_4_lut (.A(s_cram_adi), .B(s_cram_next_st_1__N_24_c_0), 
         .C(n9428), .D(s_cram_wr_addr[15]), .Z(n8840)) /* synthesis lut_function=(!((B+(C+!(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(159[25:69])
    defparam i2_3_lut_rep_173_4_lut.init = 16'h0200;
    LUT4 i1_2_lut_3_lut (.A(s_cram_adi), .B(s_cram_next_st_1__N_24_c_0), 
         .C(s_cram_wr_addr[15]), .Z(s_fifo_wr_stb)) /* synthesis lut_function=(!((B+!(C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(159[25:69])
    defparam i1_2_lut_3_lut.init = 16'h2020;
    LUT4 s_reg_rd_data_15__I_0_i16_3_lut (.A(s_mem_dout[15]), .B(s_reg_rd_data[15]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[15])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i16_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_rep_176_3_lut (.A(s_cram_adi), .B(s_cram_next_st_1__N_24_c_0), 
         .C(s_cram_wr_addr[15]), .Z(n8843)) /* synthesis lut_function=(!((B+(C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(159[25:69])
    defparam i1_2_lut_rep_176_3_lut.init = 16'h0202;
    LUT4 i137_2_lut (.A(s_cram_next_st_1__N_24_c_0), .B(s_cram_adv), .Z(n1903)) /* synthesis lut_function=(!(A+!(B))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(141[3] 154[12])
    defparam i137_2_lut.init = 16'h4444;
    LUT4 i1_3_lut (.A(n1896[2]), .B(s_cram_next_st_1__N_24_c_0), .C(s_cram_adv), 
         .Z(fsmc_clk_c_enable_11)) /* synthesis lut_function=(A+(B (C))) */ ;
    defparam i1_3_lut.init = 16'heaea;
    LUT4 s_start_stb_I_0_141_2_lut (.A(s_start_stb), .B(s_op_mem_wr), .Z(s_dbg_mem_wr)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(291[16:43])
    defparam s_start_stb_I_0_141_2_lut.init = 16'h8888;
    \syncfifo(16,4,8)  reg_wr_fifo (.fsmc_clk_c(fsmc_clk_c), .s_fifo_wr_stb(s_fifo_wr_stb), 
            .mclk_c(mclk_c), .out_get_N_312(out_get_N_312), .n1938(n1938), 
            .s_addr0({Open_3, Open_4, \s_addr0[1] , \s_addr0[0] }), 
            .q_15__N_82(q_15__N_82), .s_addr1({Open_5, Open_6, \s_addr1[1] , 
            \s_addr1[0] }), .s_addr2({s_addr2}), .s_addr3({s_addr3}), 
            .fsmc_d_out_0(fsmc_d_out_0), .n1940(n1940), .n8840(n8840), 
            .n1939(n1939), .n8862(n8862), .n8838(n8838), .n8726(n8726), 
            .n8387(n8387), .n8731(n8731), .n8365(n8365), .n8733(n8733), 
            .n8340(n8340), .q_15__N_80(q_15__N_80), .q_15__N_78(q_15__N_78), 
            .q_15__N_76(q_15__N_76), .fsmc_d_out_1(fsmc_d_out_1), .fsmc_d_out_2(fsmc_d_out_2), 
            .fsmc_d_out_3(fsmc_d_out_3), .fsmc_d_out_4(fsmc_d_out_4), .fsmc_d_out_5(fsmc_d_out_5), 
            .fsmc_d_out_6(fsmc_d_out_6), .fsmc_d_out_7(fsmc_d_out_7), .fsmc_d_out_8(fsmc_d_out_8), 
            .fsmc_d_out_9(fsmc_d_out_9), .fsmc_d_out_10(fsmc_d_out_10), 
            .fsmc_d_out_11(fsmc_d_out_11), .fsmc_d_out_12(fsmc_d_out_12), 
            .fsmc_d_out_13(fsmc_d_out_13), .fsmc_d_out_14(fsmc_d_out_14), 
            .fsmc_d_out_15(fsmc_d_out_15), .n8863(n8863), .n20(n20), .n22(n22), 
            .s_reg_din({s_reg_din}));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(239[16:31])
    \counter(16,16)  addr_r (.q_15__N_78(q_15__N_78), .fsmc_clk_c(fsmc_clk_c), 
            .n9421(n9421), .q_15__N_80(q_15__N_80), .q_15__N_76(q_15__N_76), 
            .q_15__N_74(q_15__N_74), .q_15__N_72(q_15__N_72), .q_15__N_70(q_15__N_70), 
            .q_15__N_68(q_15__N_68), .q_15__N_66(q_15__N_66), .q_15__N_64(q_15__N_64), 
            .q_15__N_62(q_15__N_62), .q_15__N_56(q_15__N_56), .q_15__N_52(q_15__N_52), 
            .\s_cram_wr_addr[15] (s_cram_wr_addr[15]), .q_15__N_82(q_15__N_82), 
            .n9426(n9426), .fsmc_d_out_1(fsmc_d_out_1), .s_cram_adv(s_cram_adv), 
            .\s_tmr[6] (\s_tmr[6] ), .fsmc_d_out_2(fsmc_d_out_2), .fsmc_d_out_3(fsmc_d_out_3), 
            .fsmc_d_out_4(fsmc_d_out_4), .fsmc_d_out_5(fsmc_d_out_5), .fsmc_d_out_6(fsmc_d_out_6), 
            .fsmc_d_out_7(fsmc_d_out_7), .fsmc_d_out_8(fsmc_d_out_8), .fsmc_d_out_9(fsmc_d_out_9), 
            .fsmc_d_out_10(fsmc_d_out_10), .fsmc_d_out_11(fsmc_d_out_11), 
            .fsmc_d_out_12(fsmc_d_out_12), .fsmc_d_out_13(fsmc_d_out_13), 
            .fsmc_d_out_14(fsmc_d_out_14), .\s_r3[6] (\s_r3[6] ), .fsmc_d_out_15(fsmc_d_out_15), 
            .fsmc_d_out_0(fsmc_d_out_0), .s_cram_adi(s_cram_adi), .n8389(n8389), 
            .n8390(n8390), .n8741(n8741), .n8004(n8004), .n8020(n8020), 
            .\s_reg_dout[3] (s_reg_dout_c[3]), .n8013(n8013), .n8045(n8045), 
            .\s_reg_dout[2] (s_reg_dout_c[2]), .n8395(n8395), .n8396(n8396), 
            .n8740(n8740), .n8398(n8398), .n8399(n8399), .n8739(n8739), 
            .n8404(n8404), .n8405(n8405), .n8738(n8738), .n8029(n8029), 
            .n8052(n8052), .\s_reg_dout[1] (s_reg_dout_c[1]), .n8342(n8342), 
            .n8343(n8343), .n8737(n8737), .n8345(n8345), .n8346(n8346), 
            .n8736(n8736), .n8372(n8372), .n8373(n8373), .n8735(n8735), 
            .n7942(n7942), .n7(n7), .n9425(n9425), .n9424(n9424), .n9423(n9423), 
            .n9422(n9422));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(166[11:25])
    
endmodule
//
// Verilog Description of module \syncfifo(16,4,8) 
//

module \syncfifo(16,4,8)  (fsmc_clk_c, s_fifo_wr_stb, mclk_c, out_get_N_312, 
            n1938, s_addr0, q_15__N_82, s_addr1, s_addr2, s_addr3, 
            fsmc_d_out_0, n1940, n8840, n1939, n8862, n8838, n8726, 
            n8387, n8731, n8365, n8733, n8340, q_15__N_80, q_15__N_78, 
            q_15__N_76, fsmc_d_out_1, fsmc_d_out_2, fsmc_d_out_3, fsmc_d_out_4, 
            fsmc_d_out_5, fsmc_d_out_6, fsmc_d_out_7, fsmc_d_out_8, 
            fsmc_d_out_9, fsmc_d_out_10, fsmc_d_out_11, fsmc_d_out_12, 
            fsmc_d_out_13, fsmc_d_out_14, fsmc_d_out_15, n8863, n20, 
            n22, s_reg_din);
    input fsmc_clk_c;
    input s_fifo_wr_stb;
    input mclk_c;
    output out_get_N_312;
    output n1938;
    output [3:0]s_addr0;
    input q_15__N_82;
    output [3:0]s_addr1;
    output [3:0]s_addr2;
    output [3:0]s_addr3;
    input fsmc_d_out_0;
    output n1940;
    input n8840;
    output n1939;
    output n8862;
    output n8838;
    input n8726;
    output n8387;
    input n8731;
    output n8365;
    input n8733;
    output n8340;
    input q_15__N_80;
    input q_15__N_78;
    input q_15__N_76;
    input fsmc_d_out_1;
    input fsmc_d_out_2;
    input fsmc_d_out_3;
    input fsmc_d_out_4;
    input fsmc_d_out_5;
    input fsmc_d_out_6;
    input fsmc_d_out_7;
    input fsmc_d_out_8;
    input fsmc_d_out_9;
    input fsmc_d_out_10;
    input fsmc_d_out_11;
    input fsmc_d_out_12;
    input fsmc_d_out_13;
    input fsmc_d_out_14;
    input fsmc_d_out_15;
    output n8863;
    input n20;
    output n22;
    output [15:0]s_reg_din;
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(73[2:10])
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    wire [1:0]s_sel_in;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(56[9:17])
    wire [1:0]n13;
    wire [3:0]n1937;
    
    wire fsmc_clk_c_enable_38, fsmc_clk_c_enable_53, fsmc_clk_c_enable_68, 
        fsmc_clk_c_enable_83;
    wire [15:0]s_data0;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(68[9:16])
    wire [15:0]s_data1;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(69[9:16])
    wire [15:0]s_data2;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(70[9:16])
    wire [15:0]s_data3;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(71[9:16])
    wire [1:0]s_syn_sel0;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(58[9:19])
    wire [1:0]s_syn_sel1;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(59[9:19])
    
    wire n3523;
    wire [15:0]n1993;
    
    wire n3850, n3852, n3854, n3856, n3858, n3860, n3862, n2, 
        n3864, n3866, n3868, n3870, n3872, n3874, n3876, n3878;
    wire [3:0]s_addr0_c;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(63[9:16])
    wire [3:0]s_addr1_c;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(64[9:16])
    
    wire n7861;
    
    FD1P3AX s_sel_in_755__i0 (.D(n13[0]), .SP(s_fifo_wr_stb), .CK(fsmc_clk_c), 
            .Q(s_sel_in[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_sel_in_755__i0.GSR = "ENABLED";
    FD1P3AX s_sel_in_755__i1 (.D(n13[1]), .SP(s_fifo_wr_stb), .CK(fsmc_clk_c), 
            .Q(s_sel_in[1]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_sel_in_755__i1.GSR = "ENABLED";
    FD1P3AY s_sel_out_FSM_i0_i0 (.D(n1938), .SP(out_get_N_312), .CK(mclk_c), 
            .Q(n1937[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_sel_out_FSM_i0_i0.GSR = "ENABLED";
    FD1P3AX s_addr0_i0_i0 (.D(q_15__N_82), .SP(fsmc_clk_c_enable_38), .CK(fsmc_clk_c), 
            .Q(s_addr0[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr0_i0_i0.GSR = "DISABLED";
    FD1P3AX s_addr1_i0_i0 (.D(q_15__N_82), .SP(fsmc_clk_c_enable_53), .CK(fsmc_clk_c), 
            .Q(s_addr1[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr1_i0_i0.GSR = "DISABLED";
    FD1P3AX s_addr2_i0_i0 (.D(q_15__N_82), .SP(fsmc_clk_c_enable_68), .CK(fsmc_clk_c), 
            .Q(s_addr2[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr2_i0_i0.GSR = "DISABLED";
    FD1P3AX s_addr3_i0_i0 (.D(q_15__N_82), .SP(fsmc_clk_c_enable_83), .CK(fsmc_clk_c), 
            .Q(s_addr3[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr3_i0_i0.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i0 (.D(fsmc_d_out_0), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i0.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i0 (.D(fsmc_d_out_0), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i0.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i0 (.D(fsmc_d_out_0), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i0.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i0 (.D(fsmc_d_out_0), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i0.GSR = "DISABLED";
    FD1S3AX s_syn_sel0_i0 (.D(s_sel_in[0]), .CK(mclk_c), .Q(s_syn_sel0[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(96[3] 102[10])
    defparam s_syn_sel0_i0.GSR = "ENABLED";
    FD1S3AX s_syn_sel1_i0 (.D(s_syn_sel0[0]), .CK(mclk_c), .Q(s_syn_sel1[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(96[3] 102[10])
    defparam s_syn_sel1_i0.GSR = "ENABLED";
    LUT4 i1431_3_lut (.A(s_data2[0]), .B(s_data3[0]), .C(n1938), .Z(n3523)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1431_3_lut.init = 16'hcaca;
    LUT4 mux_174_i1_3_lut (.A(s_data0[0]), .B(s_data1[0]), .C(n1940), 
         .Z(n1993[0])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i1_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_3_lut (.A(s_sel_in[0]), .B(n8840), .C(s_sel_in[1]), 
         .Z(fsmc_clk_c_enable_53)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;
    defparam i1_2_lut_3_lut.init = 16'h0808;
    LUT4 i1_2_lut_3_lut_adj_202 (.A(s_sel_in[0]), .B(n8840), .C(s_sel_in[1]), 
         .Z(fsmc_clk_c_enable_83)) /* synthesis lut_function=(A (B (C))) */ ;
    defparam i1_2_lut_3_lut_adj_202.init = 16'h8080;
    LUT4 i6222_2_lut_3_lut (.A(s_sel_in[0]), .B(n8840), .C(s_sel_in[1]), 
         .Z(fsmc_clk_c_enable_38)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam i6222_2_lut_3_lut.init = 16'h0404;
    LUT4 i6219_2_lut_3_lut (.A(s_sel_in[0]), .B(n8840), .C(s_sel_in[1]), 
         .Z(fsmc_clk_c_enable_68)) /* synthesis lut_function=(!(A+!(B (C)))) */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam i6219_2_lut_3_lut.init = 16'h4040;
    LUT4 i1758_3_lut (.A(s_data2[1]), .B(s_data3[1]), .C(n1938), .Z(n3850)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1758_3_lut.init = 16'hcaca;
    LUT4 mux_174_i2_3_lut (.A(s_data0[1]), .B(s_data1[1]), .C(n1940), 
         .Z(n1993[1])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i2_3_lut.init = 16'hcaca;
    LUT4 i1760_3_lut (.A(s_data2[2]), .B(s_data3[2]), .C(n1938), .Z(n3852)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1760_3_lut.init = 16'hcaca;
    LUT4 mux_174_i3_3_lut (.A(s_data0[2]), .B(s_data1[2]), .C(n1940), 
         .Z(n1993[2])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i3_3_lut.init = 16'hcaca;
    LUT4 i1762_3_lut (.A(s_data2[3]), .B(s_data3[3]), .C(n1938), .Z(n3854)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1762_3_lut.init = 16'hcaca;
    LUT4 mux_174_i4_3_lut (.A(s_data0[3]), .B(s_data1[3]), .C(n1940), 
         .Z(n1993[3])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i4_3_lut.init = 16'hcaca;
    LUT4 i1764_3_lut (.A(s_data2[4]), .B(s_data3[4]), .C(n1938), .Z(n3856)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1764_3_lut.init = 16'hcaca;
    LUT4 mux_174_i5_3_lut (.A(s_data0[4]), .B(s_data1[4]), .C(n1940), 
         .Z(n1993[4])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i5_3_lut.init = 16'hcaca;
    LUT4 i1766_3_lut (.A(s_data2[5]), .B(s_data3[5]), .C(n1938), .Z(n3858)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1766_3_lut.init = 16'hcaca;
    LUT4 mux_174_i6_3_lut (.A(s_data0[5]), .B(s_data1[5]), .C(n1940), 
         .Z(n1993[5])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i6_3_lut.init = 16'hcaca;
    LUT4 i1768_3_lut (.A(s_data2[6]), .B(s_data3[6]), .C(n1938), .Z(n3860)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1768_3_lut.init = 16'hcaca;
    LUT4 mux_174_i7_3_lut (.A(s_data0[6]), .B(s_data1[6]), .C(n1940), 
         .Z(n1993[6])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i7_3_lut.init = 16'hcaca;
    LUT4 i5005_1_lut (.A(s_sel_in[0]), .Z(n13[0])) /* synthesis lut_function=(!(A)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i5005_1_lut.init = 16'h5555;
    LUT4 i1770_3_lut (.A(s_data2[7]), .B(s_data3[7]), .C(n1938), .Z(n3862)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1770_3_lut.init = 16'hcaca;
    LUT4 mux_174_i8_3_lut (.A(s_data0[7]), .B(s_data1[7]), .C(n1940), 
         .Z(n1993[7])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i8_3_lut.init = 16'hcaca;
    LUT4 s_syn_sel1_1__I_0_i3_4_lut (.A(s_syn_sel1[0]), .B(n2), .C(n1940), 
         .D(n1938), .Z(out_get_N_312)) /* synthesis lut_function=(A (B+!(C+(D)))+!A (B+(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(105[26:49])
    defparam s_syn_sel1_1__I_0_i3_4_lut.init = 16'hddde;
    LUT4 i1772_3_lut (.A(s_data2[8]), .B(s_data3[8]), .C(n1938), .Z(n3864)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1772_3_lut.init = 16'hcaca;
    LUT4 mux_174_i9_3_lut (.A(s_data0[8]), .B(s_data1[8]), .C(n1940), 
         .Z(n1993[8])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i9_3_lut.init = 16'hcaca;
    LUT4 s_syn_sel1_1__I_0_i2_3_lut (.A(s_syn_sel1[1]), .B(n1939), .C(n1938), 
         .Z(n2)) /* synthesis lut_function=(!(A (B+(C))+!A !(B+(C)))) */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(105[26:49])
    defparam s_syn_sel1_1__I_0_i2_3_lut.init = 16'h5656;
    LUT4 i1774_3_lut (.A(s_data2[9]), .B(s_data3[9]), .C(n1938), .Z(n3866)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1774_3_lut.init = 16'hcaca;
    LUT4 mux_174_i10_3_lut (.A(s_data0[9]), .B(s_data1[9]), .C(n1940), 
         .Z(n1993[9])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i10_3_lut.init = 16'hcaca;
    LUT4 i1776_3_lut (.A(s_data2[10]), .B(s_data3[10]), .C(n1938), .Z(n3868)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1776_3_lut.init = 16'hcaca;
    LUT4 mux_174_i11_3_lut (.A(s_data0[10]), .B(s_data1[10]), .C(n1940), 
         .Z(n1993[10])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i11_3_lut.init = 16'hcaca;
    LUT4 i1778_3_lut (.A(s_data2[11]), .B(s_data3[11]), .C(n1938), .Z(n3870)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1778_3_lut.init = 16'hcaca;
    LUT4 mux_174_i12_3_lut (.A(s_data0[11]), .B(s_data1[11]), .C(n1940), 
         .Z(n1993[11])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i12_3_lut.init = 16'hcaca;
    LUT4 i1780_3_lut (.A(s_data2[12]), .B(s_data3[12]), .C(n1938), .Z(n3872)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1780_3_lut.init = 16'hcaca;
    LUT4 mux_174_i13_3_lut (.A(s_data0[12]), .B(s_data1[12]), .C(n1940), 
         .Z(n1993[12])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i13_3_lut.init = 16'hcaca;
    LUT4 i1782_3_lut (.A(s_data2[13]), .B(s_data3[13]), .C(n1938), .Z(n3874)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1782_3_lut.init = 16'hcaca;
    LUT4 mux_174_i14_3_lut (.A(s_data0[13]), .B(s_data1[13]), .C(n1940), 
         .Z(n1993[13])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i14_3_lut.init = 16'hcaca;
    LUT4 i1784_3_lut (.A(s_data2[14]), .B(s_data3[14]), .C(n1938), .Z(n3876)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1784_3_lut.init = 16'hcaca;
    LUT4 mux_174_i15_3_lut (.A(s_data0[14]), .B(s_data1[14]), .C(n1940), 
         .Z(n1993[14])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i15_3_lut.init = 16'hcaca;
    LUT4 i1786_3_lut (.A(s_data2[15]), .B(s_data3[15]), .C(n1938), .Z(n3878)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1786_3_lut.init = 16'hcaca;
    LUT4 mux_174_i16_3_lut (.A(s_data0[15]), .B(s_data1[15]), .C(n1940), 
         .Z(n1993[15])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_174_i16_3_lut.init = 16'hcaca;
    LUT4 i5007_2_lut (.A(s_sel_in[1]), .B(s_sel_in[0]), .Z(n13[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i5007_2_lut.init = 16'h6666;
    LUT4 i1790_3_lut_rep_171 (.A(n8862), .B(s_addr2[2]), .C(n1939), .Z(n8838)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1790_3_lut_rep_171.init = 16'hcaca;
    LUT4 ddr_udqm_c_bdd_2_lut_6437_4_lut (.A(n8862), .B(s_addr2[2]), .C(n1939), 
         .D(n8726), .Z(n8387)) /* synthesis lut_function=(A (B (D)+!B !(C+!(D)))+!A (B (C (D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam ddr_udqm_c_bdd_2_lut_6437_4_lut.init = 16'hca00;
    LUT4 ddr_udqm_c_bdd_2_lut_6393_4_lut (.A(n8862), .B(s_addr2[2]), .C(n1939), 
         .D(n8731), .Z(n8365)) /* synthesis lut_function=(A (B (D)+!B !(C+!(D)))+!A (B (C (D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam ddr_udqm_c_bdd_2_lut_6393_4_lut.init = 16'hca00;
    LUT4 ddr_udqm_c_bdd_2_lut_6376_4_lut (.A(n8862), .B(s_addr2[2]), .C(n1939), 
         .D(n8733), .Z(n8340)) /* synthesis lut_function=(A (B (D)+!B !(C+!(D)))+!A (B (C (D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam ddr_udqm_c_bdd_2_lut_6376_4_lut.init = 16'hca00;
    FD1P3AX s_sel_out_FSM_i0_i1 (.D(n1937[0]), .SP(out_get_N_312), .CK(mclk_c), 
            .Q(n1940));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_sel_out_FSM_i0_i1.GSR = "ENABLED";
    FD1P3AX s_sel_out_FSM_i0_i2 (.D(n1940), .SP(out_get_N_312), .CK(mclk_c), 
            .Q(n1939));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_sel_out_FSM_i0_i2.GSR = "ENABLED";
    FD1P3AX s_sel_out_FSM_i0_i3 (.D(n1939), .SP(out_get_N_312), .CK(mclk_c), 
            .Q(n1938));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_sel_out_FSM_i0_i3.GSR = "ENABLED";
    FD1P3AX s_addr0_i0_i1 (.D(q_15__N_80), .SP(fsmc_clk_c_enable_38), .CK(fsmc_clk_c), 
            .Q(s_addr0[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr0_i0_i1.GSR = "DISABLED";
    FD1P3AX s_addr0_i0_i2 (.D(q_15__N_78), .SP(fsmc_clk_c_enable_38), .CK(fsmc_clk_c), 
            .Q(s_addr0_c[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr0_i0_i2.GSR = "DISABLED";
    FD1P3AX s_addr0_i0_i3 (.D(q_15__N_76), .SP(fsmc_clk_c_enable_38), .CK(fsmc_clk_c), 
            .Q(s_addr0_c[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr0_i0_i3.GSR = "DISABLED";
    FD1P3AX s_addr1_i0_i1 (.D(q_15__N_80), .SP(fsmc_clk_c_enable_53), .CK(fsmc_clk_c), 
            .Q(s_addr1[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr1_i0_i1.GSR = "DISABLED";
    FD1P3AX s_addr1_i0_i2 (.D(q_15__N_78), .SP(fsmc_clk_c_enable_53), .CK(fsmc_clk_c), 
            .Q(s_addr1_c[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr1_i0_i2.GSR = "DISABLED";
    FD1P3AX s_addr1_i0_i3 (.D(q_15__N_76), .SP(fsmc_clk_c_enable_53), .CK(fsmc_clk_c), 
            .Q(s_addr1_c[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr1_i0_i3.GSR = "DISABLED";
    FD1P3AX s_addr2_i0_i1 (.D(q_15__N_80), .SP(fsmc_clk_c_enable_68), .CK(fsmc_clk_c), 
            .Q(s_addr2[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr2_i0_i1.GSR = "DISABLED";
    FD1P3AX s_addr2_i0_i2 (.D(q_15__N_78), .SP(fsmc_clk_c_enable_68), .CK(fsmc_clk_c), 
            .Q(s_addr2[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr2_i0_i2.GSR = "DISABLED";
    FD1P3AX s_addr2_i0_i3 (.D(q_15__N_76), .SP(fsmc_clk_c_enable_68), .CK(fsmc_clk_c), 
            .Q(s_addr2[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr2_i0_i3.GSR = "DISABLED";
    FD1P3AX s_addr3_i0_i1 (.D(q_15__N_80), .SP(fsmc_clk_c_enable_83), .CK(fsmc_clk_c), 
            .Q(s_addr3[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr3_i0_i1.GSR = "DISABLED";
    FD1P3AX s_addr3_i0_i2 (.D(q_15__N_78), .SP(fsmc_clk_c_enable_83), .CK(fsmc_clk_c), 
            .Q(s_addr3[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr3_i0_i2.GSR = "DISABLED";
    FD1P3AX s_addr3_i0_i3 (.D(q_15__N_76), .SP(fsmc_clk_c_enable_83), .CK(fsmc_clk_c), 
            .Q(s_addr3[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr3_i0_i3.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i1 (.D(fsmc_d_out_1), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i1.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i2 (.D(fsmc_d_out_2), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i2.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i3 (.D(fsmc_d_out_3), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i3.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i4 (.D(fsmc_d_out_4), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[4])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i4.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i5 (.D(fsmc_d_out_5), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[5])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i5.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i6 (.D(fsmc_d_out_6), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[6])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i6.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i7 (.D(fsmc_d_out_7), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[7])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i7.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i8 (.D(fsmc_d_out_8), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[8])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i8.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i9 (.D(fsmc_d_out_9), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[9])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i9.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i10 (.D(fsmc_d_out_10), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[10])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i10.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i11 (.D(fsmc_d_out_11), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[11])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i11.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i12 (.D(fsmc_d_out_12), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[12])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i12.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i13 (.D(fsmc_d_out_13), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[13])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i13.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i14 (.D(fsmc_d_out_14), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[14])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i14.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i15 (.D(fsmc_d_out_15), .SP(fsmc_clk_c_enable_38), 
            .CK(fsmc_clk_c), .Q(s_data0[15])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i15.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i1 (.D(fsmc_d_out_1), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i1.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i2 (.D(fsmc_d_out_2), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i2.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i3 (.D(fsmc_d_out_3), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i3.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i4 (.D(fsmc_d_out_4), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[4])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i4.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i5 (.D(fsmc_d_out_5), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[5])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i5.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i6 (.D(fsmc_d_out_6), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[6])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i6.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i7 (.D(fsmc_d_out_7), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[7])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i7.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i8 (.D(fsmc_d_out_8), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[8])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i8.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i9 (.D(fsmc_d_out_9), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[9])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i9.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i10 (.D(fsmc_d_out_10), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[10])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i10.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i11 (.D(fsmc_d_out_11), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[11])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i11.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i12 (.D(fsmc_d_out_12), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[12])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i12.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i13 (.D(fsmc_d_out_13), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[13])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i13.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i14 (.D(fsmc_d_out_14), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[14])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i14.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i15 (.D(fsmc_d_out_15), .SP(fsmc_clk_c_enable_53), 
            .CK(fsmc_clk_c), .Q(s_data1[15])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i15.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i1 (.D(fsmc_d_out_1), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i1.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i2 (.D(fsmc_d_out_2), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i2.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i3 (.D(fsmc_d_out_3), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i3.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i4 (.D(fsmc_d_out_4), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[4])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i4.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i5 (.D(fsmc_d_out_5), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[5])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i5.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i6 (.D(fsmc_d_out_6), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[6])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i6.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i7 (.D(fsmc_d_out_7), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[7])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i7.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i8 (.D(fsmc_d_out_8), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[8])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i8.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i9 (.D(fsmc_d_out_9), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[9])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i9.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i10 (.D(fsmc_d_out_10), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[10])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i10.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i11 (.D(fsmc_d_out_11), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[11])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i11.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i12 (.D(fsmc_d_out_12), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[12])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i12.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i13 (.D(fsmc_d_out_13), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[13])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i13.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i14 (.D(fsmc_d_out_14), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[14])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i14.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i15 (.D(fsmc_d_out_15), .SP(fsmc_clk_c_enable_68), 
            .CK(fsmc_clk_c), .Q(s_data2[15])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i15.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i1 (.D(fsmc_d_out_1), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i1.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i2 (.D(fsmc_d_out_2), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i2.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i3 (.D(fsmc_d_out_3), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i3.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i4 (.D(fsmc_d_out_4), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[4])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i4.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i5 (.D(fsmc_d_out_5), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[5])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i5.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i6 (.D(fsmc_d_out_6), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[6])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i6.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i7 (.D(fsmc_d_out_7), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[7])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i7.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i8 (.D(fsmc_d_out_8), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[8])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i8.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i9 (.D(fsmc_d_out_9), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[9])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i9.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i10 (.D(fsmc_d_out_10), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[10])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i10.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i11 (.D(fsmc_d_out_11), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[11])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i11.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i12 (.D(fsmc_d_out_12), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[12])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i12.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i13 (.D(fsmc_d_out_13), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[13])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i13.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i14 (.D(fsmc_d_out_14), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[14])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i14.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i15 (.D(fsmc_d_out_15), .SP(fsmc_clk_c_enable_83), 
            .CK(fsmc_clk_c), .Q(s_data3[15])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i15.GSR = "DISABLED";
    FD1S3AX s_syn_sel0_i1 (.D(s_sel_in[1]), .CK(mclk_c), .Q(s_syn_sel0[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(96[3] 102[10])
    defparam s_syn_sel0_i1.GSR = "ENABLED";
    FD1S3AX s_syn_sel1_i1 (.D(s_syn_sel0[1]), .CK(mclk_c), .Q(s_syn_sel1[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(96[3] 102[10])
    defparam s_syn_sel1_i1.GSR = "ENABLED";
    LUT4 mux_170_i3_3_lut_rep_195 (.A(s_addr0_c[2]), .B(s_addr1_c[2]), .C(n1940), 
         .Z(n8862)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_170_i3_3_lut_rep_195.init = 16'hcaca;
    LUT4 mux_170_i4_3_lut_rep_196 (.A(s_addr0_c[3]), .B(s_addr1_c[3]), .C(n1940), 
         .Z(n8863)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_170_i4_3_lut_rep_196.init = 16'hcaca;
    LUT4 i1_2_lut_4_lut (.A(s_addr0_c[3]), .B(s_addr1_c[3]), .C(n1940), 
         .D(n20), .Z(n22)) /* synthesis lut_function=(!(A (B+!(C (D)))+!A (B (C+!(D))+!B !(D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1_2_lut_4_lut.init = 16'h3500;
    PFUMX i1787 (.BLUT(n1993[15]), .ALUT(n3878), .C0(n7861), .Z(s_reg_din[15]));
    PFUMX i1785 (.BLUT(n1993[14]), .ALUT(n3876), .C0(n7861), .Z(s_reg_din[14]));
    PFUMX i1783 (.BLUT(n1993[13]), .ALUT(n3874), .C0(n7861), .Z(s_reg_din[13]));
    PFUMX i1781 (.BLUT(n1993[12]), .ALUT(n3872), .C0(n7861), .Z(s_reg_din[12]));
    PFUMX i1779 (.BLUT(n1993[11]), .ALUT(n3870), .C0(n7861), .Z(s_reg_din[11]));
    PFUMX i1777 (.BLUT(n1993[10]), .ALUT(n3868), .C0(n7861), .Z(s_reg_din[10]));
    PFUMX i1775 (.BLUT(n1993[9]), .ALUT(n3866), .C0(n7861), .Z(s_reg_din[9]));
    PFUMX i1773 (.BLUT(n1993[8]), .ALUT(n3864), .C0(n7861), .Z(s_reg_din[8]));
    PFUMX i1771 (.BLUT(n1993[7]), .ALUT(n3862), .C0(n7861), .Z(s_reg_din[7]));
    PFUMX i1769 (.BLUT(n1993[6]), .ALUT(n3860), .C0(n7861), .Z(s_reg_din[6]));
    PFUMX i1767 (.BLUT(n1993[5]), .ALUT(n3858), .C0(n7861), .Z(s_reg_din[5]));
    PFUMX i1765 (.BLUT(n1993[4]), .ALUT(n3856), .C0(n7861), .Z(s_reg_din[4]));
    PFUMX i1763 (.BLUT(n1993[3]), .ALUT(n3854), .C0(n7861), .Z(s_reg_din[3]));
    PFUMX i1761 (.BLUT(n1993[2]), .ALUT(n3852), .C0(n7861), .Z(s_reg_din[2]));
    PFUMX i1759 (.BLUT(n1993[1]), .ALUT(n3850), .C0(n7861), .Z(s_reg_din[1]));
    PFUMX i1432 (.BLUT(n1993[0]), .ALUT(n3523), .C0(n7861), .Z(s_reg_din[0]));
    LUT4 i6226_2_lut (.A(n1938), .B(n1939), .Z(n7861)) /* synthesis lut_function=(A+(B)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i6226_2_lut.init = 16'heeee;
    
endmodule
//
// Verilog Description of module \counter(16,16) 
//

module \counter(16,16)  (q_15__N_78, fsmc_clk_c, n9421, q_15__N_80, 
            q_15__N_76, q_15__N_74, q_15__N_72, q_15__N_70, q_15__N_68, 
            q_15__N_66, q_15__N_64, q_15__N_62, q_15__N_56, q_15__N_52, 
            \s_cram_wr_addr[15] , q_15__N_82, n9426, fsmc_d_out_1, s_cram_adv, 
            \s_tmr[6] , fsmc_d_out_2, fsmc_d_out_3, fsmc_d_out_4, fsmc_d_out_5, 
            fsmc_d_out_6, fsmc_d_out_7, fsmc_d_out_8, fsmc_d_out_9, 
            fsmc_d_out_10, fsmc_d_out_11, fsmc_d_out_12, fsmc_d_out_13, 
            fsmc_d_out_14, \s_r3[6] , fsmc_d_out_15, fsmc_d_out_0, s_cram_adi, 
            n8389, n8390, n8741, n8004, n8020, \s_reg_dout[3] , 
            n8013, n8045, \s_reg_dout[2] , n8395, n8396, n8740, 
            n8398, n8399, n8739, n8404, n8405, n8738, n8029, n8052, 
            \s_reg_dout[1] , n8342, n8343, n8737, n8345, n8346, 
            n8736, n8372, n8373, n8735, n7942, n7, n9425, n9424, 
            n9423, n9422);
    output q_15__N_78;
    input fsmc_clk_c;
    output n9421;
    output q_15__N_80;
    output q_15__N_76;
    output q_15__N_74;
    output q_15__N_72;
    output q_15__N_70;
    output q_15__N_68;
    output q_15__N_66;
    output q_15__N_64;
    output q_15__N_62;
    output q_15__N_56;
    output q_15__N_52;
    output \s_cram_wr_addr[15] ;
    output q_15__N_82;
    output n9426;
    input fsmc_d_out_1;
    input s_cram_adv;
    input \s_tmr[6] ;
    input fsmc_d_out_2;
    input fsmc_d_out_3;
    input fsmc_d_out_4;
    input fsmc_d_out_5;
    input fsmc_d_out_6;
    input fsmc_d_out_7;
    input fsmc_d_out_8;
    input fsmc_d_out_9;
    input fsmc_d_out_10;
    input fsmc_d_out_11;
    input fsmc_d_out_12;
    input fsmc_d_out_13;
    input fsmc_d_out_14;
    input \s_r3[6] ;
    input fsmc_d_out_15;
    input fsmc_d_out_0;
    input s_cram_adi;
    input n8389;
    input n8390;
    output n8741;
    input n8004;
    input n8020;
    output \s_reg_dout[3] ;
    input n8013;
    input n8045;
    output \s_reg_dout[2] ;
    input n8395;
    input n8396;
    output n8740;
    input n8398;
    input n8399;
    output n8739;
    input n8404;
    input n8405;
    output n8738;
    input n8029;
    input n8052;
    output \s_reg_dout[1] ;
    input n8342;
    input n8343;
    output n8737;
    input n8345;
    input n8346;
    output n8736;
    input n8372;
    input n8373;
    output n8735;
    input n7942;
    output n7;
    output n9425;
    output n9424;
    output n9423;
    output n9422;
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(73[2:10])
    
    wire n8890, n8891, n8892, q_0__N_141, q_1__N_137, q_2__N_133, 
        q_3__N_129, q_4__N_125, q_5__N_121, q_6__N_117, q_7__N_113, 
        q_8__N_109, q_9__N_105, q_10__N_101, q_15__N_60, q_11__N_97, 
        q_15__N_58, q_12__N_93, q_13__N_89, q_14__N_85, q_15__N_49, 
        n8770, n8758, n8839, n8805, n8776, n8762, q_15__N_51, 
        n8783, n8766, n8864, n8813, n8824, n8793, n8788, n8754;
    
    PFUMX i6659 (.BLUT(n8890), .ALUT(n8891), .C0(q_15__N_78), .Z(n8892));
    FD1S3AX s_count_reg_0__180_rep_224 (.D(q_0__N_141), .CK(fsmc_clk_c), 
            .Q(n9421)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_0__180_rep_224.GSR = "ENABLED";
    FD1S3AX s_count_reg_1__181 (.D(q_1__N_137), .CK(fsmc_clk_c), .Q(q_15__N_80)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_1__181.GSR = "ENABLED";
    FD1S3AX s_count_reg_2__182 (.D(q_2__N_133), .CK(fsmc_clk_c), .Q(q_15__N_78)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_2__182.GSR = "ENABLED";
    FD1S3AX s_count_reg_3__183 (.D(q_3__N_129), .CK(fsmc_clk_c), .Q(q_15__N_76)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_3__183.GSR = "ENABLED";
    FD1S3AX s_count_reg_4__184 (.D(q_4__N_125), .CK(fsmc_clk_c), .Q(q_15__N_74)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_4__184.GSR = "ENABLED";
    FD1S3AX s_count_reg_5__185 (.D(q_5__N_121), .CK(fsmc_clk_c), .Q(q_15__N_72)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_5__185.GSR = "ENABLED";
    FD1S3AX s_count_reg_6__186 (.D(q_6__N_117), .CK(fsmc_clk_c), .Q(q_15__N_70)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_6__186.GSR = "ENABLED";
    FD1S3AX s_count_reg_7__187 (.D(q_7__N_113), .CK(fsmc_clk_c), .Q(q_15__N_68)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_7__187.GSR = "ENABLED";
    FD1S3AX s_count_reg_8__188 (.D(q_8__N_109), .CK(fsmc_clk_c), .Q(q_15__N_66)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_8__188.GSR = "ENABLED";
    FD1S3AX s_count_reg_9__189 (.D(q_9__N_105), .CK(fsmc_clk_c), .Q(q_15__N_64)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_9__189.GSR = "ENABLED";
    FD1S3AX s_count_reg_10__190 (.D(q_10__N_101), .CK(fsmc_clk_c), .Q(q_15__N_62)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_10__190.GSR = "ENABLED";
    FD1S3AX s_count_reg_11__191 (.D(q_11__N_97), .CK(fsmc_clk_c), .Q(q_15__N_60)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_11__191.GSR = "ENABLED";
    FD1S3AX s_count_reg_12__192 (.D(q_12__N_93), .CK(fsmc_clk_c), .Q(q_15__N_58)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_12__192.GSR = "ENABLED";
    FD1S3AX s_count_reg_13__193 (.D(q_13__N_89), .CK(fsmc_clk_c), .Q(q_15__N_56)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_13__193.GSR = "ENABLED";
    FD1S3AX s_count_reg_14__194 (.D(q_14__N_85), .CK(fsmc_clk_c), .Q(q_15__N_52)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_14__194.GSR = "ENABLED";
    FD1S3AX s_count_reg_15__195 (.D(q_15__N_49), .CK(fsmc_clk_c), .Q(\s_cram_wr_addr[15] )) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_15__195.GSR = "ENABLED";
    FD1S3AX s_count_reg_0__180 (.D(q_0__N_141), .CK(fsmc_clk_c), .Q(q_15__N_82)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_0__180.GSR = "ENABLED";
    LUT4 q_15__I_9_2_lut_rep_91_3_lut_4_lut (.A(q_15__N_62), .B(n8770), 
         .C(q_15__N_58), .D(q_15__N_60), .Z(n8758)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_9_2_lut_rep_91_3_lut_4_lut.init = 16'h8000;
    LUT4 q_15__I_17_2_lut_rep_138_3_lut_4_lut (.A(q_15__N_78), .B(n8839), 
         .C(q_15__N_74), .D(q_15__N_76), .Z(n8805)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_17_2_lut_rep_138_3_lut_4_lut.init = 16'h8000;
    FD1S3AX s_count_reg_1__181_rep_229 (.D(q_1__N_137), .CK(fsmc_clk_c), 
            .Q(n9426)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_1__181_rep_229.GSR = "ENABLED";
    LUT4 q_15__I_10_2_lut_rep_95_3_lut_4_lut (.A(q_15__N_64), .B(n8776), 
         .C(q_15__N_60), .D(q_15__N_62), .Z(n8762)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_10_2_lut_rep_95_3_lut_4_lut.init = 16'h8000;
    LUT4 q_15__I_7_2_lut_3_lut_4_lut (.A(q_15__N_58), .B(n8762), .C(q_15__N_52), 
         .D(q_15__N_56), .Z(q_15__N_51)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_7_2_lut_3_lut_4_lut.init = 16'h8000;
    LUT4 q_15__I_11_2_lut_rep_99_3_lut_4_lut (.A(q_15__N_66), .B(n8783), 
         .C(q_15__N_62), .D(q_15__N_64), .Z(n8766)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_11_2_lut_rep_99_3_lut_4_lut.init = 16'h8000;
    LUT4 q_1__I_49_4_lut (.A(q_15__N_80), .B(fsmc_d_out_1), .C(s_cram_adv), 
         .D(n8864), .Z(q_1__N_137)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_1__I_49_4_lut.init = 16'hc5ca;
    LUT4 q_15__I_18_2_lut_rep_146_3_lut_4_lut (.A(q_15__N_80), .B(n8864), 
         .C(q_15__N_76), .D(q_15__N_78), .Z(n8813)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_18_2_lut_rep_146_3_lut_4_lut.init = 16'h8000;
    LUT4 s_r3_6__bdd_4_lut_4_lut_then_2_lut (.A(q_15__N_82), .B(\s_tmr[6] ), 
         .Z(n8891)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_r3_6__bdd_4_lut_4_lut_then_2_lut.init = 16'h8888;
    LUT4 q_2__I_47_4_lut (.A(q_15__N_78), .B(fsmc_d_out_2), .C(s_cram_adv), 
         .D(n8839), .Z(q_2__N_133)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_2__I_47_4_lut.init = 16'hc5ca;
    LUT4 q_3__I_45_4_lut (.A(q_15__N_76), .B(fsmc_d_out_3), .C(s_cram_adv), 
         .D(n8824), .Z(q_3__N_129)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_3__I_45_4_lut.init = 16'hc5ca;
    LUT4 q_4__I_43_4_lut (.A(q_15__N_74), .B(fsmc_d_out_4), .C(s_cram_adv), 
         .D(n8813), .Z(q_4__N_125)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_4__I_43_4_lut.init = 16'hc5ca;
    LUT4 q_5__I_41_4_lut (.A(q_15__N_72), .B(fsmc_d_out_5), .C(s_cram_adv), 
         .D(n8805), .Z(q_5__N_121)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_5__I_41_4_lut.init = 16'hc5ca;
    LUT4 q_6__I_39_4_lut (.A(q_15__N_70), .B(fsmc_d_out_6), .C(s_cram_adv), 
         .D(n8793), .Z(q_6__N_117)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_6__I_39_4_lut.init = 16'hc5ca;
    LUT4 q_7__I_37_4_lut (.A(q_15__N_68), .B(fsmc_d_out_7), .C(s_cram_adv), 
         .D(n8788), .Z(q_7__N_113)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_7__I_37_4_lut.init = 16'hc5ca;
    LUT4 q_8__I_35_4_lut (.A(q_15__N_66), .B(fsmc_d_out_8), .C(s_cram_adv), 
         .D(n8783), .Z(q_8__N_109)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_8__I_35_4_lut.init = 16'hc5ca;
    LUT4 q_9__I_33_4_lut (.A(q_15__N_64), .B(fsmc_d_out_9), .C(s_cram_adv), 
         .D(n8776), .Z(q_9__N_105)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_9__I_33_4_lut.init = 16'hc5ca;
    LUT4 q_10__I_31_4_lut (.A(q_15__N_62), .B(fsmc_d_out_10), .C(s_cram_adv), 
         .D(n8770), .Z(q_10__N_101)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_10__I_31_4_lut.init = 16'hc5ca;
    LUT4 q_15__I_12_2_lut_rep_103_3_lut_4_lut (.A(q_15__N_68), .B(n8788), 
         .C(q_15__N_64), .D(q_15__N_66), .Z(n8770)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_12_2_lut_rep_103_3_lut_4_lut.init = 16'h8000;
    LUT4 q_11__I_29_4_lut (.A(q_15__N_60), .B(fsmc_d_out_11), .C(s_cram_adv), 
         .D(n8766), .Z(q_11__N_97)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_11__I_29_4_lut.init = 16'hc5ca;
    LUT4 q_12__I_27_4_lut (.A(q_15__N_58), .B(fsmc_d_out_12), .C(s_cram_adv), 
         .D(n8762), .Z(q_12__N_93)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_12__I_27_4_lut.init = 16'hc5ca;
    LUT4 q_13__I_25_4_lut (.A(q_15__N_56), .B(fsmc_d_out_13), .C(s_cram_adv), 
         .D(n8758), .Z(q_13__N_89)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_13__I_25_4_lut.init = 16'hc5ca;
    LUT4 q_14__I_23_4_lut (.A(q_15__N_52), .B(fsmc_d_out_14), .C(s_cram_adv), 
         .D(n8754), .Z(q_14__N_85)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_14__I_23_4_lut.init = 16'hc5ca;
    LUT4 s_r3_6__bdd_4_lut_4_lut_else_2_lut (.A(q_15__N_82), .B(q_15__N_80), 
         .C(\s_r3[6] ), .Z(n8890)) /* synthesis lut_function=(A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_r3_6__bdd_4_lut_4_lut_else_2_lut.init = 16'h8080;
    LUT4 q_15__I_6_4_lut (.A(\s_cram_wr_addr[15] ), .B(fsmc_d_out_15), .C(s_cram_adv), 
         .D(q_15__N_51), .Z(q_15__N_49)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_15__I_6_4_lut.init = 16'hc5ca;
    LUT4 q_0__I_51_4_lut (.A(q_15__N_82), .B(fsmc_d_out_0), .C(s_cram_adv), 
         .D(s_cram_adi), .Z(q_0__N_141)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_0__I_51_4_lut.init = 16'hc5ca;
    LUT4 q_15__I_13_2_lut_rep_109_3_lut_4_lut (.A(q_15__N_70), .B(n8793), 
         .C(q_15__N_66), .D(q_15__N_68), .Z(n8776)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_13_2_lut_rep_109_3_lut_4_lut.init = 16'h8000;
    LUT4 n8390_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n8389), 
         .D(n8390), .Z(n8741)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n8390_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 s_reg_rd_sel_3__I_0_Mux_3_i15_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_76), 
         .C(n8004), .D(n8020), .Z(\s_reg_dout[3] )) /* synthesis lut_function=(!(A (B+!(D))+!A !(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_reg_rd_sel_3__I_0_Mux_3_i15_4_lut_4_lut.init = 16'h7340;
    LUT4 s_reg_rd_sel_3__I_0_Mux_2_i15_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_76), 
         .C(n8013), .D(n8045), .Z(\s_reg_dout[2] )) /* synthesis lut_function=(!(A (B+!(D))+!A !(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_reg_rd_sel_3__I_0_Mux_2_i15_4_lut_4_lut.init = 16'h7340;
    LUT4 n8396_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n8395), 
         .D(n8396), .Z(n8740)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n8396_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 n8399_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n8398), 
         .D(n8399), .Z(n8739)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n8399_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 n8405_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n8404), 
         .D(n8405), .Z(n8738)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n8405_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 s_reg_rd_sel_3__I_0_Mux_1_i15_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_76), 
         .C(n8029), .D(n8052), .Z(\s_reg_dout[1] )) /* synthesis lut_function=(!(A (B+!(D))+!A !(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_reg_rd_sel_3__I_0_Mux_1_i15_4_lut_4_lut.init = 16'h7340;
    LUT4 n8343_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n8342), 
         .D(n8343), .Z(n8737)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n8343_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 n8346_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n8345), 
         .D(n8346), .Z(n8736)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n8346_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 n8373_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n8372), 
         .D(n8373), .Z(n8735)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n8373_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 s_reg_rd_sel_3__I_0_Mux_6_i7_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_76), 
         .C(n7942), .D(n8892), .Z(n7)) /* synthesis lut_function=(!(A (B+!(D))+!A !(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_reg_rd_sel_3__I_0_Mux_6_i7_4_lut_4_lut.init = 16'h7340;
    FD1S3AX s_count_reg_1__181_rep_228 (.D(q_1__N_137), .CK(fsmc_clk_c), 
            .Q(n9425)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_1__181_rep_228.GSR = "ENABLED";
    LUT4 q_15__I_14_2_lut_rep_116_3_lut_4_lut (.A(q_15__N_72), .B(n8805), 
         .C(q_15__N_68), .D(q_15__N_70), .Z(n8783)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_14_2_lut_rep_116_3_lut_4_lut.init = 16'h8000;
    LUT4 q_15__I_21_2_lut_rep_197 (.A(q_15__N_82), .B(s_cram_adi), .Z(n8864)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_21_2_lut_rep_197.init = 16'h8888;
    LUT4 q_15__I_20_2_lut_rep_172_3_lut (.A(q_15__N_82), .B(s_cram_adi), 
         .C(q_15__N_80), .Z(n8839)) /* synthesis lut_function=(A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_20_2_lut_rep_172_3_lut.init = 16'h8080;
    LUT4 q_15__I_19_2_lut_rep_157_3_lut_4_lut (.A(q_15__N_82), .B(s_cram_adi), 
         .C(q_15__N_78), .D(q_15__N_80), .Z(n8824)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_19_2_lut_rep_157_3_lut_4_lut.init = 16'h8000;
    FD1S3AX s_count_reg_1__181_rep_227 (.D(q_1__N_137), .CK(fsmc_clk_c), 
            .Q(n9424)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_1__181_rep_227.GSR = "ENABLED";
    FD1S3AX s_count_reg_1__181_rep_226 (.D(q_1__N_137), .CK(fsmc_clk_c), 
            .Q(n9423)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_1__181_rep_226.GSR = "ENABLED";
    FD1S3AX s_count_reg_0__180_rep_225 (.D(q_0__N_141), .CK(fsmc_clk_c), 
            .Q(n9422)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_0__180_rep_225.GSR = "ENABLED";
    LUT4 q_15__I_15_2_lut_rep_121_3_lut_4_lut (.A(q_15__N_74), .B(n8813), 
         .C(q_15__N_70), .D(q_15__N_72), .Z(n8788)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_15_2_lut_rep_121_3_lut_4_lut.init = 16'h8000;
    LUT4 q_15__I_8_2_lut_rep_87_3_lut_4_lut (.A(q_15__N_60), .B(n8766), 
         .C(q_15__N_56), .D(q_15__N_58), .Z(n8754)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_8_2_lut_rep_87_3_lut_4_lut.init = 16'h8000;
    LUT4 q_15__I_16_2_lut_rep_126_3_lut_4_lut (.A(q_15__N_76), .B(n8824), 
         .C(q_15__N_72), .D(q_15__N_74), .Z(n8793)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_16_2_lut_rep_126_3_lut_4_lut.init = 16'h8000;
    
endmodule
//
// Verilog Description of module \jtag_reg(16,12)_U2 
//

module \jtag_reg(16,12)_U2  (\s_mem_rd_addr[11] , fsmc_clk_c, s_dbg_mem_rd, 
            q_15__N_62, \s_mem_rd_addr[10] , q_15__N_64, \s_mem_rd_addr[9] , 
            q_15__N_66, \s_mem_rd_addr[8] , q_15__N_68, \s_mem_rd_addr[7] , 
            q_15__N_70, \s_mem_rd_addr[6] , q_15__N_72, \s_mem_rd_addr[5] , 
            q_15__N_74, \s_mem_rd_addr[4] , q_15__N_76, \s_mem_rd_addr[3] , 
            q_15__N_78, \s_mem_rd_addr[2] , q_15__N_80, \s_mem_rd_addr[1] , 
            q_15__N_82);
    output \s_mem_rd_addr[11] ;
    input fsmc_clk_c;
    input s_dbg_mem_rd;
    input q_15__N_62;
    output \s_mem_rd_addr[10] ;
    input q_15__N_64;
    output \s_mem_rd_addr[9] ;
    input q_15__N_66;
    output \s_mem_rd_addr[8] ;
    input q_15__N_68;
    output \s_mem_rd_addr[7] ;
    input q_15__N_70;
    output \s_mem_rd_addr[6] ;
    input q_15__N_72;
    output \s_mem_rd_addr[5] ;
    input q_15__N_74;
    output \s_mem_rd_addr[4] ;
    input q_15__N_76;
    output \s_mem_rd_addr[3] ;
    input q_15__N_78;
    output \s_mem_rd_addr[2] ;
    input q_15__N_80;
    output \s_mem_rd_addr[1] ;
    input q_15__N_82;
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(73[2:10])
    
    FD1P3AX s_reg__i11 (.D(q_15__N_62), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[11] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=759, LSE_RLINE=759 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i11.GSR = "ENABLED";
    FD1P3AX s_reg__i10 (.D(q_15__N_64), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[10] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=759, LSE_RLINE=759 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i10.GSR = "ENABLED";
    FD1P3AX s_reg__i9 (.D(q_15__N_66), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[9] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=759, LSE_RLINE=759 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i9.GSR = "ENABLED";
    FD1P3AX s_reg__i8 (.D(q_15__N_68), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[8] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=759, LSE_RLINE=759 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i8.GSR = "ENABLED";
    FD1P3AX s_reg__i7 (.D(q_15__N_70), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[7] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=759, LSE_RLINE=759 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i7.GSR = "ENABLED";
    FD1P3AX s_reg__i6 (.D(q_15__N_72), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[6] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=759, LSE_RLINE=759 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i6.GSR = "ENABLED";
    FD1P3AX s_reg__i5 (.D(q_15__N_74), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[5] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=759, LSE_RLINE=759 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i5.GSR = "ENABLED";
    FD1P3AX s_reg__i4 (.D(q_15__N_76), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[4] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=759, LSE_RLINE=759 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i4.GSR = "ENABLED";
    FD1P3AX s_reg__i3 (.D(q_15__N_78), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[3] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=759, LSE_RLINE=759 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i3.GSR = "ENABLED";
    FD1P3AX s_reg__i2 (.D(q_15__N_80), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[2] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=759, LSE_RLINE=759 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i2.GSR = "ENABLED";
    FD1P3AX s_reg__i1 (.D(q_15__N_82), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[1] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=759, LSE_RLINE=759 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i1.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module \counter(16,16)_U0 
//

module \counter(16,16)_U0  (s_tmr, mclk_c, s_reg_din, s_tmr_cnt_en_N_2, 
            n8845, n8827, q_15__N_51, n8877);
    output [15:0]s_tmr;
    input mclk_c;
    input [15:0]s_reg_din;
    input s_tmr_cnt_en_N_2;
    input n8845;
    input n8827;
    output q_15__N_51;
    input n8877;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    wire q_1__N_137, q_2__N_133, q_3__N_129, q_4__N_125, q_5__N_121, 
        q_6__N_117, q_7__N_113, q_8__N_109, q_9__N_105, q_10__N_101, 
        q_11__N_97, q_12__N_93, q_13__N_89, q_14__N_85, q_15__N_49, 
        q_0__N_141, n8760, n8748, n8773, n8755, n8819, n8810, 
        n8798, n8791, n8785, n8780, n8769, n8751;
    
    FD1S3AX s_count_reg_1__181 (.D(q_1__N_137), .CK(mclk_c), .Q(s_tmr[1])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_1__181.GSR = "ENABLED";
    FD1S3AX s_count_reg_2__182 (.D(q_2__N_133), .CK(mclk_c), .Q(s_tmr[2])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_2__182.GSR = "ENABLED";
    FD1S3AX s_count_reg_3__183 (.D(q_3__N_129), .CK(mclk_c), .Q(s_tmr[3])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_3__183.GSR = "ENABLED";
    FD1S3AX s_count_reg_4__184 (.D(q_4__N_125), .CK(mclk_c), .Q(s_tmr[4])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_4__184.GSR = "ENABLED";
    FD1S3AX s_count_reg_5__185 (.D(q_5__N_121), .CK(mclk_c), .Q(s_tmr[5])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_5__185.GSR = "ENABLED";
    FD1S3AX s_count_reg_6__186 (.D(q_6__N_117), .CK(mclk_c), .Q(s_tmr[6])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_6__186.GSR = "ENABLED";
    FD1S3AX s_count_reg_7__187 (.D(q_7__N_113), .CK(mclk_c), .Q(s_tmr[7])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_7__187.GSR = "ENABLED";
    FD1S3AX s_count_reg_8__188 (.D(q_8__N_109), .CK(mclk_c), .Q(s_tmr[8])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_8__188.GSR = "ENABLED";
    FD1S3AX s_count_reg_9__189 (.D(q_9__N_105), .CK(mclk_c), .Q(s_tmr[9])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_9__189.GSR = "ENABLED";
    FD1S3AX s_count_reg_10__190 (.D(q_10__N_101), .CK(mclk_c), .Q(s_tmr[10])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_10__190.GSR = "ENABLED";
    FD1S3AX s_count_reg_11__191 (.D(q_11__N_97), .CK(mclk_c), .Q(s_tmr[11])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_11__191.GSR = "ENABLED";
    FD1S3AX s_count_reg_12__192 (.D(q_12__N_93), .CK(mclk_c), .Q(s_tmr[12])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_12__192.GSR = "ENABLED";
    FD1S3AX s_count_reg_13__193 (.D(q_13__N_89), .CK(mclk_c), .Q(s_tmr[13])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_13__193.GSR = "ENABLED";
    FD1S3AX s_count_reg_14__194 (.D(q_14__N_85), .CK(mclk_c), .Q(s_tmr[14])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_14__194.GSR = "ENABLED";
    FD1S3AX s_count_reg_15__195 (.D(q_15__N_49), .CK(mclk_c), .Q(s_tmr[15])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_15__195.GSR = "ENABLED";
    FD1S3AX s_count_reg_0__180 (.D(q_0__N_141), .CK(mclk_c), .Q(s_tmr[0])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=688, LSE_RLINE=688 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_0__180.GSR = "ENABLED";
    LUT4 q_15__I_8_2_lut_rep_81_3_lut_4_lut (.A(s_tmr[11]), .B(n8760), .C(s_tmr[13]), 
         .D(s_tmr[12]), .Z(n8748)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_8_2_lut_rep_81_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_10_2_lut_rep_88_3_lut_4_lut (.A(s_tmr[9]), .B(n8773), .C(s_tmr[11]), 
         .D(s_tmr[10]), .Z(n8755)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_10_2_lut_rep_88_3_lut_4_lut.init = 16'h0004;
    LUT4 q_1__I_49_4_lut (.A(s_tmr[1]), .B(s_reg_din[1]), .C(s_tmr_cnt_en_N_2), 
         .D(n8845), .Z(q_1__N_137)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_1__I_49_4_lut.init = 16'hc5ca;
    LUT4 q_2__I_47_4_lut (.A(s_tmr[2]), .B(s_reg_din[2]), .C(s_tmr_cnt_en_N_2), 
         .D(n8827), .Z(q_2__N_133)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_2__I_47_4_lut.init = 16'hc5ca;
    LUT4 q_3__I_45_4_lut (.A(s_tmr[3]), .B(s_reg_din[3]), .C(s_tmr_cnt_en_N_2), 
         .D(n8819), .Z(q_3__N_129)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_3__I_45_4_lut.init = 16'hc5ca;
    LUT4 q_4__I_43_4_lut (.A(s_tmr[4]), .B(s_reg_din[4]), .C(s_tmr_cnt_en_N_2), 
         .D(n8810), .Z(q_4__N_125)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_4__I_43_4_lut.init = 16'hc5ca;
    LUT4 q_5__I_41_4_lut (.A(s_tmr[5]), .B(s_reg_din[5]), .C(s_tmr_cnt_en_N_2), 
         .D(n8798), .Z(q_5__N_121)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_5__I_41_4_lut.init = 16'hc5ca;
    LUT4 q_6__I_39_4_lut (.A(s_tmr[6]), .B(s_reg_din[6]), .C(s_tmr_cnt_en_N_2), 
         .D(n8791), .Z(q_6__N_117)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_6__I_39_4_lut.init = 16'hc5ca;
    LUT4 q_15__I_18_2_lut_rep_143_3_lut_4_lut (.A(s_tmr[1]), .B(n8845), 
         .C(s_tmr[3]), .D(s_tmr[2]), .Z(n8810)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_18_2_lut_rep_143_3_lut_4_lut.init = 16'h0004;
    LUT4 q_7__I_37_4_lut (.A(s_tmr[7]), .B(s_reg_din[7]), .C(s_tmr_cnt_en_N_2), 
         .D(n8785), .Z(q_7__N_113)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_7__I_37_4_lut.init = 16'hc5ca;
    LUT4 q_8__I_35_4_lut (.A(s_tmr[8]), .B(s_reg_din[8]), .C(s_tmr_cnt_en_N_2), 
         .D(n8780), .Z(q_8__N_109)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_8__I_35_4_lut.init = 16'hc5ca;
    LUT4 q_9__I_33_4_lut (.A(s_tmr[9]), .B(s_reg_din[9]), .C(s_tmr_cnt_en_N_2), 
         .D(n8773), .Z(q_9__N_105)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_9__I_33_4_lut.init = 16'hc5ca;
    LUT4 q_10__I_31_4_lut (.A(s_tmr[10]), .B(s_reg_din[10]), .C(s_tmr_cnt_en_N_2), 
         .D(n8769), .Z(q_10__N_101)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_10__I_31_4_lut.init = 16'hc5ca;
    LUT4 q_11__I_29_4_lut (.A(s_tmr[11]), .B(s_reg_din[11]), .C(s_tmr_cnt_en_N_2), 
         .D(n8760), .Z(q_11__N_97)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_11__I_29_4_lut.init = 16'hc5ca;
    LUT4 q_12__I_27_4_lut (.A(s_tmr[12]), .B(s_reg_din[12]), .C(s_tmr_cnt_en_N_2), 
         .D(n8755), .Z(q_12__N_93)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_12__I_27_4_lut.init = 16'hc5ca;
    LUT4 q_13__I_25_4_lut (.A(s_tmr[13]), .B(s_reg_din[13]), .C(s_tmr_cnt_en_N_2), 
         .D(n8751), .Z(q_13__N_89)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_13__I_25_4_lut.init = 16'hc5ca;
    LUT4 q_14__I_23_4_lut (.A(s_tmr[14]), .B(s_reg_din[14]), .C(s_tmr_cnt_en_N_2), 
         .D(n8748), .Z(q_14__N_85)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_14__I_23_4_lut.init = 16'hc5ca;
    LUT4 q_15__I_6_4_lut (.A(s_tmr[15]), .B(s_reg_din[15]), .C(s_tmr_cnt_en_N_2), 
         .D(q_15__N_51), .Z(q_15__N_49)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_15__I_6_4_lut.init = 16'hc5ca;
    LUT4 q_0__I_51_4_lut (.A(s_tmr[0]), .B(s_reg_din[0]), .C(s_tmr_cnt_en_N_2), 
         .D(n8877), .Z(q_0__N_141)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_0__I_51_4_lut.init = 16'hc5ca;
    LUT4 q_15__I_11_2_lut_rep_93_3_lut_4_lut (.A(s_tmr[8]), .B(n8780), .C(s_tmr[10]), 
         .D(s_tmr[9]), .Z(n8760)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_11_2_lut_rep_93_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_7_2_lut_3_lut_4_lut (.A(s_tmr[12]), .B(n8755), .C(s_tmr[14]), 
         .D(s_tmr[13]), .Z(q_15__N_51)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_7_2_lut_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_12_2_lut_rep_102_3_lut_4_lut (.A(s_tmr[7]), .B(n8785), 
         .C(s_tmr[9]), .D(s_tmr[8]), .Z(n8769)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_12_2_lut_rep_102_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_19_2_lut_rep_152_3_lut_4_lut (.A(s_tmr[0]), .B(n8877), 
         .C(s_tmr[2]), .D(s_tmr[1]), .Z(n8819)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_19_2_lut_rep_152_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_13_2_lut_rep_106_3_lut_4_lut (.A(s_tmr[6]), .B(n8791), 
         .C(s_tmr[8]), .D(s_tmr[7]), .Z(n8773)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_13_2_lut_rep_106_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_9_2_lut_rep_84_3_lut_4_lut (.A(s_tmr[10]), .B(n8769), .C(s_tmr[12]), 
         .D(s_tmr[11]), .Z(n8751)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_9_2_lut_rep_84_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_14_2_lut_rep_113_3_lut_4_lut (.A(s_tmr[5]), .B(n8798), 
         .C(s_tmr[7]), .D(s_tmr[6]), .Z(n8780)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_14_2_lut_rep_113_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_15_2_lut_rep_118_3_lut_4_lut (.A(s_tmr[4]), .B(n8810), 
         .C(s_tmr[6]), .D(s_tmr[5]), .Z(n8785)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_15_2_lut_rep_118_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_16_2_lut_rep_124_3_lut_4_lut (.A(s_tmr[3]), .B(n8819), 
         .C(s_tmr[5]), .D(s_tmr[4]), .Z(n8791)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_16_2_lut_rep_124_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_17_2_lut_rep_131_3_lut_4_lut (.A(s_tmr[2]), .B(n8827), 
         .C(s_tmr[4]), .D(s_tmr[3]), .Z(n8798)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_17_2_lut_rep_131_3_lut_4_lut.init = 16'h0004;
    
endmodule
//
// Verilog Description of module \jtag_reg(16,16)_U5 
//

module \jtag_reg(16,16)_U5  (s_desc_hi, mclk_c, mclk_c_enable_112, \s_desc_data[16] , 
            \s_desc_data[31] , \s_desc_data[30] , \s_desc_data[29] , \s_desc_data[28] , 
            \s_desc_data[27] , \s_desc_data[26] , \s_desc_data[25] , \s_desc_data[24] , 
            \s_desc_data[23] , \s_desc_data[22] , \s_desc_data[21] , \s_desc_data[20] , 
            \s_desc_data[19] , \s_desc_data[18] , \s_desc_data[17] );
    output [15:0]s_desc_hi;
    input mclk_c;
    input mclk_c_enable_112;
    input \s_desc_data[16] ;
    input \s_desc_data[31] ;
    input \s_desc_data[30] ;
    input \s_desc_data[29] ;
    input \s_desc_data[28] ;
    input \s_desc_data[27] ;
    input \s_desc_data[26] ;
    input \s_desc_data[25] ;
    input \s_desc_data[24] ;
    input \s_desc_data[23] ;
    input \s_desc_data[22] ;
    input \s_desc_data[21] ;
    input \s_desc_data[20] ;
    input \s_desc_data[19] ;
    input \s_desc_data[18] ;
    input \s_desc_data[17] ;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    FD1P3AX s_reg_i0_i0 (.D(\s_desc_data[16] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[0])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i0.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i15 (.D(\s_desc_data[31] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[15])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i15.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i14 (.D(\s_desc_data[30] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[14])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i14.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i13 (.D(\s_desc_data[29] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[13])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i13.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i12 (.D(\s_desc_data[28] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[12])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i12.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i11 (.D(\s_desc_data[27] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[11])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i11.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i10 (.D(\s_desc_data[26] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[10])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i10.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i9 (.D(\s_desc_data[25] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[9])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i9.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i8 (.D(\s_desc_data[24] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[8])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i8.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i7 (.D(\s_desc_data[23] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[7])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i7.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i6 (.D(\s_desc_data[22] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[6])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i6.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i5 (.D(\s_desc_data[21] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[5])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i5.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i4 (.D(\s_desc_data[20] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[4])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i4.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i3 (.D(\s_desc_data[19] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[3])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i3.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i2 (.D(\s_desc_data[18] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[2])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i2.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i1 (.D(\s_desc_data[17] ), .SP(mclk_c_enable_112), 
            .CK(mclk_c), .Q(s_desc_hi[1])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=788, LSE_RLINE=788 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i1.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module ddr_phy
//

module ddr_phy (\s_cfg[6] , mclk_c, cke_din, buf_ckeo0, VCC_net, s_ddr_csn, 
            buf_csno1, buf_csno0, s_ddr_wen, buf_weno, s_ddr_casn, 
            buf_casno, s_ddr_rasn, buf_rasno, s_ddr_ba, buf_bao1, 
            buf_bao0, s_ddr_addr, buf_addro12, buf_addro11, buf_addro10, 
            buf_addro9, buf_addro8, buf_addro7, buf_addro6, buf_addro5, 
            buf_addro4, buf_addro3, buf_addro2, buf_addro1, buf_addro0, 
            n9428, buf_clkout0, GND_net, s_ddr_dqstri1, dqstrio1, 
            s_ddr_dqstri0, dqstrio0, s_ddr_dqs1, dqsout1, s_ddr_dqs0, 
            dqsout0, s_ddr_dtri1, dqtri1, s_ddr_dtri0, dqtri0, s_ddr_dataout0, 
            dqo15, dqo14, dqo13, dqo12, dqo11, dqo10, dqo9, dqo8, 
            dqo7, dqo6, dqo5, dqo4, dqo3, dqo2, dqo1, dqo0, 
            s_ddr_datain1, datain_t15, datain_t14, datain_t13, datain_t12, 
            datain_t11, datain_t10, datain_t9, datain_t8, s_ddr_datain0, 
            datain_t7, datain_t6, datain_t5, datain_t4, datain_t3, 
            datain_t2, datain_t1, datain_t0, s_ddr_burstdet1, s_ddr_read1, 
            s_ddr_readclksel01, s_ddr_readclksel11, dqsin1, s_ddr_burstdet0, 
            s_ddr_read0, s_ddr_readclksel00, s_ddr_readclksel10, dqsin0, 
            s_ddr_freeze, s_ddr_lock, s_ddr_uddcntln) /* synthesis NGD_DRC_MASK=1 */ ;
    input \s_cfg[6] ;
    input mclk_c;
    input [0:0]cke_din;
    output buf_ckeo0;
    input VCC_net;
    input [1:0]s_ddr_csn;
    output buf_csno1;
    output buf_csno0;
    input s_ddr_wen;
    output buf_weno;
    input s_ddr_casn;
    output buf_casno;
    input s_ddr_rasn;
    output buf_rasno;
    input [1:0]s_ddr_ba;
    output buf_bao1;
    output buf_bao0;
    input [12:0]s_ddr_addr;
    output buf_addro12;
    output buf_addro11;
    output buf_addro10;
    output buf_addro9;
    output buf_addro8;
    output buf_addro7;
    output buf_addro6;
    output buf_addro5;
    output buf_addro4;
    output buf_addro3;
    output buf_addro2;
    output buf_addro1;
    output buf_addro0;
    input n9428;
    output buf_clkout0;
    input GND_net;
    input s_ddr_dqstri1;
    output dqstrio1;
    input s_ddr_dqstri0;
    output dqstrio0;
    input s_ddr_dqs1;
    output dqsout1;
    input s_ddr_dqs0;
    output dqsout0;
    input s_ddr_dtri1;
    output dqtri1;
    input s_ddr_dtri0;
    output dqtri0;
    input [15:0]s_ddr_dataout0;
    output dqo15;
    output dqo14;
    output dqo13;
    output dqo12;
    output dqo11;
    output dqo10;
    output dqo9;
    output dqo8;
    output dqo7;
    output dqo6;
    output dqo5;
    output dqo4;
    output dqo3;
    output dqo2;
    output dqo1;
    output dqo0;
    output [15:0]s_ddr_datain1;
    input datain_t15;
    input datain_t14;
    input datain_t13;
    input datain_t12;
    input datain_t11;
    input datain_t10;
    input datain_t9;
    input datain_t8;
    output [15:0]s_ddr_datain0;
    input datain_t7;
    input datain_t6;
    input datain_t5;
    input datain_t4;
    input datain_t3;
    input datain_t2;
    input datain_t1;
    input datain_t0;
    output s_ddr_burstdet1;
    input s_ddr_read1;
    input s_ddr_readclksel01;
    input s_ddr_readclksel11;
    input dqsin1;
    output s_ddr_burstdet0;
    input s_ddr_read0;
    input s_ddr_readclksel00;
    input s_ddr_readclksel10;
    input dqsin0;
    input s_ddr_freeze;
    output s_ddr_lock;
    input s_ddr_uddcntln;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    wire dqsr90_1 /* synthesis is_clock=1 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(91[12:20])
    wire dqsr90_0 /* synthesis is_clock=1 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(94[12:20])
    
    wire dqsw90_1, dqsw90_0, ddrclkpol1, ddrclkpol0, dqsdel;
    
    OFS1P3DX Inst24_OFS1P3DX0 (.D(cke_din[0]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_ckeo0)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(359[23:31])
    defparam Inst24_OFS1P3DX0.GSR = "ENABLED";
    OFS1P3DX Inst22_OFS1P3DX1 (.D(s_ddr_csn[1]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_csno1)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(369[23:31])
    defparam Inst22_OFS1P3DX1.GSR = "ENABLED";
    OFS1P3DX Inst22_OFS1P3DX0 (.D(s_ddr_csn[0]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_csno0)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(373[23:31])
    defparam Inst22_OFS1P3DX0.GSR = "ENABLED";
    OFS1P3DX Inst20_OFS1P3DX (.D(s_ddr_wen), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_weno)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(380[22:30])
    defparam Inst20_OFS1P3DX.GSR = "ENABLED";
    OFS1P3DX Inst18_OFS1P3DX (.D(s_ddr_casn), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_casno)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(387[22:30])
    defparam Inst18_OFS1P3DX.GSR = "ENABLED";
    OFS1P3DX Inst16_OFS1P3DX (.D(s_ddr_rasn), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_rasno)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(394[22:30])
    defparam Inst16_OFS1P3DX.GSR = "ENABLED";
    OFS1P3DX Inst14_OFS1P3DX1 (.D(s_ddr_ba[1]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_bao1)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(404[23:31])
    defparam Inst14_OFS1P3DX1.GSR = "ENABLED";
    OFS1P3DX Inst14_OFS1P3DX0 (.D(s_ddr_ba[0]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_bao0)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(408[23:31])
    defparam Inst14_OFS1P3DX0.GSR = "ENABLED";
    OFS1P3DX Inst12_OFS1P3DX12 (.D(s_ddr_addr[12]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_addro12)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(451[24:32])
    defparam Inst12_OFS1P3DX12.GSR = "ENABLED";
    OFS1P3DX Inst12_OFS1P3DX11 (.D(s_ddr_addr[11]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_addro11)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(455[24:32])
    defparam Inst12_OFS1P3DX11.GSR = "ENABLED";
    OFS1P3DX Inst12_OFS1P3DX10 (.D(s_ddr_addr[10]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_addro10)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(459[24:32])
    defparam Inst12_OFS1P3DX10.GSR = "ENABLED";
    OFS1P3DX Inst12_OFS1P3DX9 (.D(s_ddr_addr[9]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_addro9)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(463[23:31])
    defparam Inst12_OFS1P3DX9.GSR = "ENABLED";
    OFS1P3DX Inst12_OFS1P3DX8 (.D(s_ddr_addr[8]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_addro8)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(467[23:31])
    defparam Inst12_OFS1P3DX8.GSR = "ENABLED";
    OFS1P3DX Inst12_OFS1P3DX7 (.D(s_ddr_addr[7]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_addro7)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(471[23:31])
    defparam Inst12_OFS1P3DX7.GSR = "ENABLED";
    OFS1P3DX Inst12_OFS1P3DX6 (.D(s_ddr_addr[6]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_addro6)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(475[23:31])
    defparam Inst12_OFS1P3DX6.GSR = "ENABLED";
    OFS1P3DX Inst12_OFS1P3DX5 (.D(s_ddr_addr[5]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_addro5)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(479[23:31])
    defparam Inst12_OFS1P3DX5.GSR = "ENABLED";
    OFS1P3DX Inst12_OFS1P3DX4 (.D(s_ddr_addr[4]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_addro4)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(483[23:31])
    defparam Inst12_OFS1P3DX4.GSR = "ENABLED";
    OFS1P3DX Inst12_OFS1P3DX3 (.D(s_ddr_addr[3]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_addro3)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(487[23:31])
    defparam Inst12_OFS1P3DX3.GSR = "ENABLED";
    OFS1P3DX Inst12_OFS1P3DX2 (.D(s_ddr_addr[2]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_addro2)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(491[23:31])
    defparam Inst12_OFS1P3DX2.GSR = "ENABLED";
    OFS1P3DX Inst12_OFS1P3DX1 (.D(s_ddr_addr[1]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_addro1)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(495[23:31])
    defparam Inst12_OFS1P3DX1.GSR = "ENABLED";
    OFS1P3DX Inst12_OFS1P3DX0 (.D(s_ddr_addr[0]), .SP(VCC_net), .SCLK(mclk_c), 
            .CD(\s_cfg[6] ), .Q(buf_addro0)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/fpga/lattice/ddr_phy.vhd(499[23:31])
    defparam Inst12_OFS1P3DX0.GSR = "ENABLED";
    ODDRXE Inst10_ODDRXE0 (.D0(GND_net), .D1(VCC_net), .SCLK(mclk_c), 
           .RST(n9428), .Q(buf_clkout0)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst10_ODDRXE0.GSR = "ENABLED";
    TDDRA Inst9_TDDRA1 (.DQSW90(mclk_c), .SCLK(mclk_c), .TD(s_ddr_dqstri1), 
          .RST(n9428), .TQ(dqstrio1)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst9_TDDRA1.GSR = "ENABLED";
    defparam Inst9_TDDRA1.DQSW90_INVERT = "ENABLED";
    TDDRA Inst9_TDDRA0 (.DQSW90(mclk_c), .SCLK(mclk_c), .TD(s_ddr_dqstri0), 
          .RST(n9428), .TQ(dqstrio0)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst9_TDDRA0.GSR = "ENABLED";
    defparam Inst9_TDDRA0.DQSW90_INVERT = "ENABLED";
    ODDRDQSX1A Inst8_ODDRDQSX1A1 (.DQSW90(mclk_c), .SCLK(mclk_c), .D0(GND_net), 
            .D1(s_ddr_dqs1), .RST(n9428), .Q(dqsout1)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst8_ODDRDQSX1A1.GSR = "ENABLED";
    ODDRDQSX1A Inst8_ODDRDQSX1A0 (.DQSW90(mclk_c), .SCLK(mclk_c), .D0(GND_net), 
            .D1(s_ddr_dqs0), .RST(n9428), .Q(dqsout0)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst8_ODDRDQSX1A0.GSR = "ENABLED";
    TDDRA Inst7_TDDRA1 (.DQSW90(dqsw90_1), .SCLK(mclk_c), .TD(s_ddr_dtri1), 
          .RST(n9428), .TQ(dqtri1)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst7_TDDRA1.GSR = "ENABLED";
    defparam Inst7_TDDRA1.DQSW90_INVERT = "DISABLED";
    TDDRA Inst7_TDDRA0 (.DQSW90(dqsw90_0), .SCLK(mclk_c), .TD(s_ddr_dtri0), 
          .RST(n9428), .TQ(dqtri0)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst7_TDDRA0.GSR = "ENABLED";
    defparam Inst7_TDDRA0.DQSW90_INVERT = "DISABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_1_7 (.DQSW90(dqsw90_1), .SCLK(mclk_c), .D0(s_ddr_dataout0[7]), 
            .D1(s_ddr_dataout0[15]), .RST(n9428), .Q(dqo15)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_1_7.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_1_6 (.DQSW90(dqsw90_1), .SCLK(mclk_c), .D0(s_ddr_dataout0[6]), 
            .D1(s_ddr_dataout0[14]), .RST(n9428), .Q(dqo14)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_1_6.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_1_5 (.DQSW90(dqsw90_1), .SCLK(mclk_c), .D0(s_ddr_dataout0[5]), 
            .D1(s_ddr_dataout0[13]), .RST(n9428), .Q(dqo13)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_1_5.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_1_4 (.DQSW90(dqsw90_1), .SCLK(mclk_c), .D0(s_ddr_dataout0[4]), 
            .D1(s_ddr_dataout0[12]), .RST(n9428), .Q(dqo12)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_1_4.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_1_3 (.DQSW90(dqsw90_1), .SCLK(mclk_c), .D0(s_ddr_dataout0[3]), 
            .D1(s_ddr_dataout0[11]), .RST(n9428), .Q(dqo11)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_1_3.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_1_2 (.DQSW90(dqsw90_1), .SCLK(mclk_c), .D0(s_ddr_dataout0[2]), 
            .D1(s_ddr_dataout0[10]), .RST(n9428), .Q(dqo10)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_1_2.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_1_1 (.DQSW90(dqsw90_1), .SCLK(mclk_c), .D0(s_ddr_dataout0[1]), 
            .D1(s_ddr_dataout0[9]), .RST(n9428), .Q(dqo9)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_1_1.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_1_0 (.DQSW90(dqsw90_1), .SCLK(mclk_c), .D0(s_ddr_dataout0[0]), 
            .D1(s_ddr_dataout0[8]), .RST(n9428), .Q(dqo8)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_1_0.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_0_7 (.DQSW90(dqsw90_0), .SCLK(mclk_c), .D0(s_ddr_dataout0[7]), 
            .D1(s_ddr_dataout0[15]), .RST(n9428), .Q(dqo7)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_0_7.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_0_6 (.DQSW90(dqsw90_0), .SCLK(mclk_c), .D0(s_ddr_dataout0[6]), 
            .D1(s_ddr_dataout0[14]), .RST(n9428), .Q(dqo6)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_0_6.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_0_5 (.DQSW90(dqsw90_0), .SCLK(mclk_c), .D0(s_ddr_dataout0[5]), 
            .D1(s_ddr_dataout0[13]), .RST(n9428), .Q(dqo5)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_0_5.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_0_4 (.DQSW90(dqsw90_0), .SCLK(mclk_c), .D0(s_ddr_dataout0[4]), 
            .D1(s_ddr_dataout0[12]), .RST(n9428), .Q(dqo4)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_0_4.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_0_3 (.DQSW90(dqsw90_0), .SCLK(mclk_c), .D0(s_ddr_dataout0[3]), 
            .D1(s_ddr_dataout0[11]), .RST(n9428), .Q(dqo3)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_0_3.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_0_2 (.DQSW90(dqsw90_0), .SCLK(mclk_c), .D0(s_ddr_dataout0[2]), 
            .D1(s_ddr_dataout0[10]), .RST(n9428), .Q(dqo2)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_0_2.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_0_1 (.DQSW90(dqsw90_0), .SCLK(mclk_c), .D0(s_ddr_dataout0[1]), 
            .D1(s_ddr_dataout0[9]), .RST(n9428), .Q(dqo1)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_0_1.GSR = "ENABLED";
    ODDRDQSX1A Inst_ODDRDQSX1A_0_0 (.DQSW90(dqsw90_0), .SCLK(mclk_c), .D0(s_ddr_dataout0[0]), 
            .D1(s_ddr_dataout0[8]), .RST(n9428), .Q(dqo0)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_ODDRDQSX1A_0_0.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_1_7 (.D(datain_t15), .DQSR90(dqsr90_1), .DDRCLKPOL(ddrclkpol1), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain1[7]), .Q1(s_ddr_datain1[15])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_1_7.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_1_6 (.D(datain_t14), .DQSR90(dqsr90_1), .DDRCLKPOL(ddrclkpol1), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain1[6]), .Q1(s_ddr_datain1[14])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_1_6.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_1_5 (.D(datain_t13), .DQSR90(dqsr90_1), .DDRCLKPOL(ddrclkpol1), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain1[5]), .Q1(s_ddr_datain1[13])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_1_5.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_1_4 (.D(datain_t12), .DQSR90(dqsr90_1), .DDRCLKPOL(ddrclkpol1), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain1[4]), .Q1(s_ddr_datain1[12])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_1_4.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_1_3 (.D(datain_t11), .DQSR90(dqsr90_1), .DDRCLKPOL(ddrclkpol1), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain1[3]), .Q1(s_ddr_datain1[11])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_1_3.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_1_2 (.D(datain_t10), .DQSR90(dqsr90_1), .DDRCLKPOL(ddrclkpol1), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain1[2]), .Q1(s_ddr_datain1[10])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_1_2.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_1_1 (.D(datain_t9), .DQSR90(dqsr90_1), .DDRCLKPOL(ddrclkpol1), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain1[1]), .Q1(s_ddr_datain1[9])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_1_1.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_1_0 (.D(datain_t8), .DQSR90(dqsr90_1), .DDRCLKPOL(ddrclkpol1), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain1[0]), .Q1(s_ddr_datain1[8])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_1_0.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_0_7 (.D(datain_t7), .DQSR90(dqsr90_0), .DDRCLKPOL(ddrclkpol0), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain0[7]), .Q1(s_ddr_datain0[15])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_0_7.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_0_6 (.D(datain_t6), .DQSR90(dqsr90_0), .DDRCLKPOL(ddrclkpol0), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain0[6]), .Q1(s_ddr_datain0[14])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_0_6.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_0_5 (.D(datain_t5), .DQSR90(dqsr90_0), .DDRCLKPOL(ddrclkpol0), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain0[5]), .Q1(s_ddr_datain0[13])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_0_5.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_0_4 (.D(datain_t4), .DQSR90(dqsr90_0), .DDRCLKPOL(ddrclkpol0), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain0[4]), .Q1(s_ddr_datain0[12])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_0_4.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_0_3 (.D(datain_t3), .DQSR90(dqsr90_0), .DDRCLKPOL(ddrclkpol0), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain0[3]), .Q1(s_ddr_datain0[11])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_0_3.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_0_2 (.D(datain_t2), .DQSR90(dqsr90_0), .DDRCLKPOL(ddrclkpol0), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain0[2]), .Q1(s_ddr_datain0[10])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_0_2.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_0_1 (.D(datain_t1), .DQSR90(dqsr90_0), .DDRCLKPOL(ddrclkpol0), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain0[1]), .Q1(s_ddr_datain0[9])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_0_1.GSR = "ENABLED";
    IDDRDQSX1A Inst_IDDRDQSX1A_0_0 (.D(datain_t0), .DQSR90(dqsr90_0), .DDRCLKPOL(ddrclkpol0), 
            .SCLK(mclk_c), .RST(n9428), .Q0(s_ddr_datain0[0]), .Q1(s_ddr_datain0[8])) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst_IDDRDQSX1A_0_0.GSR = "ENABLED";
    DQSBUFH Inst6_DQSBUFH1 (.SCLK(mclk_c), .DQSI(dqsin1), .READ(s_ddr_read1), 
            .READCLKSEL1(s_ddr_readclksel11), .READCLKSEL0(s_ddr_readclksel01), 
            .RST(n9428), .DQSDEL(dqsdel), .DDRCLKPOL(ddrclkpol1), .DQSR90(dqsr90_1), 
            .DQSW90(dqsw90_1), .BURSTDET(s_ddr_burstdet1)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst6_DQSBUFH1.GSR = "ENABLED";
    defparam Inst6_DQSBUFH1.DQS_LI_DEL_ADJ = "PLUS";
    defparam Inst6_DQSBUFH1.DQS_LI_DEL_VAL = 0;
    defparam Inst6_DQSBUFH1.DQS_LO_DEL_ADJ = "PLUS";
    defparam Inst6_DQSBUFH1.DQS_LO_DEL_VAL = 0;
    defparam Inst6_DQSBUFH1.LPDDR = "DISABLED";
    DQSBUFH Inst6_DQSBUFH0 (.SCLK(mclk_c), .DQSI(dqsin0), .READ(s_ddr_read0), 
            .READCLKSEL1(s_ddr_readclksel10), .READCLKSEL0(s_ddr_readclksel00), 
            .RST(n9428), .DQSDEL(dqsdel), .DDRCLKPOL(ddrclkpol0), .DQSR90(dqsr90_0), 
            .DQSW90(dqsw90_0), .BURSTDET(s_ddr_burstdet0)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst6_DQSBUFH0.GSR = "ENABLED";
    defparam Inst6_DQSBUFH0.DQS_LI_DEL_ADJ = "PLUS";
    defparam Inst6_DQSBUFH0.DQS_LI_DEL_VAL = 0;
    defparam Inst6_DQSBUFH0.DQS_LO_DEL_ADJ = "PLUS";
    defparam Inst6_DQSBUFH0.DQS_LO_DEL_VAL = 0;
    defparam Inst6_DQSBUFH0.LPDDR = "DISABLED";
    DQSDLLC Inst5_DQSDLLC (.CLK(mclk_c), .RST(n9428), .UDDCNTLN(s_ddr_uddcntln), 
            .FREEZE(s_ddr_freeze), .LOCK(s_ddr_lock), .DQSDEL(dqsdel)) /* synthesis syn_instantiated=1, LSE_LINE_FILE_ID=41, LSE_LCOL=12, LSE_RCOL=26, LSE_LLINE=1167, LSE_RLINE=1167 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1167[12:26])
    defparam Inst5_DQSDLLC.GSR = "ENABLED";
    defparam Inst5_DQSDLLC.DEL_ADJ = "PLUS";
    defparam Inst5_DQSDLLC.DEL_VAL = 0;
    defparam Inst5_DQSDLLC.LOCK_SENSITIVITY = "LOW";
    defparam Inst5_DQSDLLC.FIN = "100.0";
    defparam Inst5_DQSDLLC.FORCE_MAX_DELAY = "NO";
    
endmodule
//
// Verilog Description of module \jtag_reg(16,16)_U8 
//

module \jtag_reg(16,16)_U8  (mclk_c, n4442, s_reg_din, n9428, s_cfg, 
            s_cfg_wr_stb, mclk_c_enable_81);
    input mclk_c;
    input n4442;
    input [15:0]s_reg_din;
    output n9428;
    output [15:0]s_cfg;
    input s_cfg_wr_stb;
    output mclk_c_enable_81;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    FD1S3IX s_reg_i6_rep_231 (.D(s_reg_din[6]), .CK(mclk_c), .CD(n4442), 
            .Q(n9428)) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i6_rep_231.GSR = "ENABLED";
    FD1P3AX s_reg_i0 (.D(s_reg_din[0]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[0])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0.GSR = "ENABLED";
    FD1P3AX s_reg_i15 (.D(s_reg_din[15]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[15])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i15.GSR = "ENABLED";
    FD1P3AX s_reg_i14 (.D(s_reg_din[14]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[14])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i14.GSR = "ENABLED";
    FD1P3AX s_reg_i13 (.D(s_reg_din[13]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[13])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i13.GSR = "ENABLED";
    FD1P3AX s_reg_i12 (.D(s_reg_din[12]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[12])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i12.GSR = "ENABLED";
    FD1P3AX s_reg_i11 (.D(s_reg_din[11]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[11])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i11.GSR = "ENABLED";
    FD1P3AX s_reg_i10 (.D(s_reg_din[10]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[10])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i10.GSR = "ENABLED";
    FD1P3AX s_reg_i9 (.D(s_reg_din[9]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[9])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i9.GSR = "ENABLED";
    FD1P3AX s_reg_i8 (.D(s_reg_din[8]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[8])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i8.GSR = "ENABLED";
    FD1P3AX s_reg_i7 (.D(s_reg_din[7]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[7])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i7.GSR = "ENABLED";
    FD1P3AX s_reg_i5 (.D(s_reg_din[5]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[5])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i5.GSR = "ENABLED";
    FD1P3AX s_reg_i4 (.D(s_reg_din[4]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[4])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i4.GSR = "ENABLED";
    FD1P3AX s_reg_i3 (.D(s_reg_din[3]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[3])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i3.GSR = "ENABLED";
    FD1P3AX s_reg_i2 (.D(s_reg_din[2]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[2])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i2.GSR = "ENABLED";
    FD1P3AX s_reg_i1 (.D(s_reg_din[1]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[1])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i1.GSR = "ENABLED";
    FD1S3IX s_reg_i6 (.D(s_reg_din[6]), .CK(mclk_c), .CD(n4442), .Q(s_cfg[6])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=663, LSE_RLINE=663 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i6.GSR = "ENABLED";
    LUT4 i278_1_lut (.A(n9428), .Z(mclk_c_enable_81)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam i278_1_lut.init = 16'h5555;
    
endmodule
//
// Verilog Description of module \jtag_reg(16,16)_U7 
//

module \jtag_reg(16,16)_U7  (s_ckgen_div, mclk_c, s_ckgen_div_wr_stb, 
            s_reg_din);
    output [15:0]s_ckgen_div;
    input mclk_c;
    input s_ckgen_div_wr_stb;
    input [15:0]s_reg_din;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    FD1P3AX s_reg_i0_i0 (.D(s_reg_din[0]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[0])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i0.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i1 (.D(s_reg_din[1]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[1])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i1.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i2 (.D(s_reg_din[2]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[2])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i2.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i3 (.D(s_reg_din[3]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[3])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i3.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i4 (.D(s_reg_din[4]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[4])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i4.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i5 (.D(s_reg_din[5]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[5])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i5.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i6 (.D(s_reg_din[6]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[6])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i6.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i7 (.D(s_reg_din[7]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[7])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i7.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i8 (.D(s_reg_din[8]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[8])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i8.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i9 (.D(s_reg_din[9]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[9])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i9.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i10 (.D(s_reg_din[10]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[10])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i10.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i11 (.D(s_reg_din[11]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[11])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i11.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i12 (.D(s_reg_din[12]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[12])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i12.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i13 (.D(s_reg_din[13]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[13])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i13.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i14 (.D(s_reg_din[14]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[14])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i14.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i15 (.D(s_reg_din[15]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[15])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=805, LSE_RLINE=805 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i15.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module \jtag_reg(16,16) 
//

module \jtag_reg(16,16)  (s_r3, mclk_c, s_r3_wr_stb, s_reg_din);
    output [15:0]s_r3;
    input mclk_c;
    input s_r3_wr_stb;
    input [15:0]s_reg_din;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    FD1P3AX s_reg_i0_i0 (.D(s_reg_din[0]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[0])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i0.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i1 (.D(s_reg_din[1]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[1])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i1.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i2 (.D(s_reg_din[2]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[2])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i2.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i3 (.D(s_reg_din[3]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[3])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i3.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i4 (.D(s_reg_din[4]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[4])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i4.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i5 (.D(s_reg_din[5]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[5])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i5.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i6 (.D(s_reg_din[6]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[6])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i6.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i7 (.D(s_reg_din[7]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[7])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i7.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i8 (.D(s_reg_din[8]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[8])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i8.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i9 (.D(s_reg_din[9]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[9])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i9.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i10 (.D(s_reg_din[10]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[10])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i10.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i11 (.D(s_reg_din[11]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[11])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i11.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i12 (.D(s_reg_din[12]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[12])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i12.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i13 (.D(s_reg_din[13]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[13])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i13.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i14 (.D(s_reg_din[14]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[14])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i14.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i15 (.D(s_reg_din[15]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[15])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=727, LSE_RLINE=727 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i15.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module \jtag_desc_ram(16,9,32,8) 
//

module \jtag_desc_ram(16,9,32,8)  (fsmc_clk_c, mclk_c, s_we_hi, VCC_net, 
            GND_net, n9425, s_desc_addr, q_15__N_78, q_15__N_76, q_15__N_74, 
            q_15__N_72, q_15__N_70, q_15__N_68, q_15__N_66, fsmc_d_out_0, 
            fsmc_d_out_1, fsmc_d_out_2, fsmc_d_out_3, fsmc_d_out_4, 
            fsmc_d_out_5, fsmc_d_out_6, fsmc_d_out_7, fsmc_d_out_8, 
            fsmc_d_out_9, fsmc_d_out_10, fsmc_d_out_11, fsmc_d_out_12, 
            fsmc_d_out_13, fsmc_d_out_14, fsmc_d_out_15, \s_desc_data[16] , 
            \s_desc_data[17] , \s_desc_data[18] , \s_desc_data[19] , \s_desc_data[20] , 
            \s_desc_data[21] , \s_desc_data[22] , \s_desc_data[23] , \s_desc_data[24] , 
            \s_desc_data[25] , \s_desc_data[26] , \s_desc_data[27] , \s_desc_data[28] , 
            \s_desc_data[29] , \s_desc_data[30] , \s_desc_data[31] , s_we_lo, 
            n9423, s_pos_0_3__N_670, \s_desc_data[4] , \s_desc_data[5] , 
            \s_desc_data[6] , \s_desc_data[7] , \s_desc_data[8] , \s_desc_data[9] , 
            \s_desc_data[10] , \s_desc_data[11] , \s_desc_data[12] , \s_desc_data[13] , 
            \s_desc_data[14] , \s_desc_data[15] );
    input fsmc_clk_c;
    input mclk_c;
    input s_we_hi;
    input VCC_net;
    input GND_net;
    input n9425;
    input [8:1]s_desc_addr;
    input q_15__N_78;
    input q_15__N_76;
    input q_15__N_74;
    input q_15__N_72;
    input q_15__N_70;
    input q_15__N_68;
    input q_15__N_66;
    input fsmc_d_out_0;
    input fsmc_d_out_1;
    input fsmc_d_out_2;
    input fsmc_d_out_3;
    input fsmc_d_out_4;
    input fsmc_d_out_5;
    input fsmc_d_out_6;
    input fsmc_d_out_7;
    input fsmc_d_out_8;
    input fsmc_d_out_9;
    input fsmc_d_out_10;
    input fsmc_d_out_11;
    input fsmc_d_out_12;
    input fsmc_d_out_13;
    input fsmc_d_out_14;
    input fsmc_d_out_15;
    output \s_desc_data[16] ;
    output \s_desc_data[17] ;
    output \s_desc_data[18] ;
    output \s_desc_data[19] ;
    output \s_desc_data[20] ;
    output \s_desc_data[21] ;
    output \s_desc_data[22] ;
    output \s_desc_data[23] ;
    output \s_desc_data[24] ;
    output \s_desc_data[25] ;
    output \s_desc_data[26] ;
    output \s_desc_data[27] ;
    output \s_desc_data[28] ;
    output \s_desc_data[29] ;
    output \s_desc_data[30] ;
    output \s_desc_data[31] ;
    input s_we_lo;
    input n9423;
    output [3:0]s_pos_0_3__N_670;
    output \s_desc_data[4] ;
    output \s_desc_data[5] ;
    output \s_desc_data[6] ;
    output \s_desc_data[7] ;
    output \s_desc_data[8] ;
    output \s_desc_data[9] ;
    output \s_desc_data[10] ;
    output \s_desc_data[11] ;
    output \s_desc_data[12] ;
    output \s_desc_data[13] ;
    output \s_desc_data[14] ;
    output \s_desc_data[15] ;
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(73[2:10])
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    PDPW8KC n29330 (.DI0(fsmc_d_out_0), .DI1(fsmc_d_out_1), .DI2(fsmc_d_out_2), 
            .DI3(fsmc_d_out_3), .DI4(fsmc_d_out_4), .DI5(fsmc_d_out_5), 
            .DI6(fsmc_d_out_6), .DI7(fsmc_d_out_7), .DI8(fsmc_d_out_8), 
            .DI9(fsmc_d_out_9), .DI10(fsmc_d_out_10), .DI11(fsmc_d_out_11), 
            .DI12(fsmc_d_out_12), .DI13(fsmc_d_out_13), .DI14(fsmc_d_out_14), 
            .DI15(fsmc_d_out_15), .DI16(GND_net), .DI17(GND_net), .ADW0(n9425), 
            .ADW1(q_15__N_78), .ADW2(q_15__N_76), .ADW3(q_15__N_74), .ADW4(q_15__N_72), 
            .ADW5(q_15__N_70), .ADW6(q_15__N_68), .ADW7(q_15__N_66), .ADW8(GND_net), 
            .BE0(VCC_net), .BE1(VCC_net), .CEW(s_we_hi), .CLKW(fsmc_clk_c), 
            .CSW0(GND_net), .CSW1(GND_net), .CSW2(GND_net), .ADR0(GND_net), 
            .ADR1(GND_net), .ADR2(GND_net), .ADR3(GND_net), .ADR4(s_desc_addr[1]), 
            .ADR5(s_desc_addr[2]), .ADR6(s_desc_addr[3]), .ADR7(s_desc_addr[4]), 
            .ADR8(s_desc_addr[5]), .ADR9(s_desc_addr[6]), .ADR10(s_desc_addr[7]), 
            .ADR11(s_desc_addr[8]), .ADR12(GND_net), .CER(VCC_net), .OCER(VCC_net), 
            .CLKR(mclk_c), .CSR0(GND_net), .CSR1(GND_net), .CSR2(GND_net), 
            .RST(GND_net), .DO0(\s_desc_data[25] ), .DO1(\s_desc_data[26] ), 
            .DO2(\s_desc_data[27] ), .DO3(\s_desc_data[28] ), .DO4(\s_desc_data[29] ), 
            .DO5(\s_desc_data[30] ), .DO6(\s_desc_data[31] ), .DO9(\s_desc_data[16] ), 
            .DO10(\s_desc_data[17] ), .DO11(\s_desc_data[18] ), .DO12(\s_desc_data[19] ), 
            .DO13(\s_desc_data[20] ), .DO14(\s_desc_data[21] ), .DO15(\s_desc_data[22] ), 
            .DO16(\s_desc_data[23] ), .DO17(\s_desc_data[24] ));
    defparam n29330.DATA_WIDTH_W = 18;
    defparam n29330.DATA_WIDTH_R = 18;
    defparam n29330.REGMODE = "NOREG";
    defparam n29330.CSDECODE_W = "0b000";
    defparam n29330.CSDECODE_R = "0b000";
    defparam n29330.GSR = "DISABLED";
    defparam n29330.RESETMODE = "SYNC";
    defparam n29330.ASYNC_RESET_RELEASE = "SYNC";
    defparam n29330.INIT_DATA = "STATIC";
    defparam n29330.INITVAL_00 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_01 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_02 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_03 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_04 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_05 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_06 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_07 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_08 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_09 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_0A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_0B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_0C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_0D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_0E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_0F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_10 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_11 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_12 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_13 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_14 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_15 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_16 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_17 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_18 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_19 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_1A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_1B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_1C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_1D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_1E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29330.INITVAL_1F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    PDPW8KC n29320 (.DI0(fsmc_d_out_0), .DI1(fsmc_d_out_1), .DI2(fsmc_d_out_2), 
            .DI3(fsmc_d_out_3), .DI4(fsmc_d_out_4), .DI5(fsmc_d_out_5), 
            .DI6(fsmc_d_out_6), .DI7(fsmc_d_out_7), .DI8(fsmc_d_out_8), 
            .DI9(fsmc_d_out_9), .DI10(fsmc_d_out_10), .DI11(fsmc_d_out_11), 
            .DI12(fsmc_d_out_12), .DI13(fsmc_d_out_13), .DI14(fsmc_d_out_14), 
            .DI15(fsmc_d_out_15), .DI16(GND_net), .DI17(GND_net), .ADW0(n9423), 
            .ADW1(q_15__N_78), .ADW2(q_15__N_76), .ADW3(q_15__N_74), .ADW4(q_15__N_72), 
            .ADW5(q_15__N_70), .ADW6(q_15__N_68), .ADW7(q_15__N_66), .ADW8(GND_net), 
            .BE0(VCC_net), .BE1(VCC_net), .CEW(s_we_lo), .CLKW(fsmc_clk_c), 
            .CSW0(GND_net), .CSW1(GND_net), .CSW2(GND_net), .ADR0(GND_net), 
            .ADR1(GND_net), .ADR2(GND_net), .ADR3(GND_net), .ADR4(s_desc_addr[1]), 
            .ADR5(s_desc_addr[2]), .ADR6(s_desc_addr[3]), .ADR7(s_desc_addr[4]), 
            .ADR8(s_desc_addr[5]), .ADR9(s_desc_addr[6]), .ADR10(s_desc_addr[7]), 
            .ADR11(s_desc_addr[8]), .ADR12(GND_net), .CER(VCC_net), .OCER(VCC_net), 
            .CLKR(mclk_c), .CSR0(GND_net), .CSR1(GND_net), .CSR2(GND_net), 
            .RST(GND_net), .DO0(\s_desc_data[9] ), .DO1(\s_desc_data[10] ), 
            .DO2(\s_desc_data[11] ), .DO3(\s_desc_data[12] ), .DO4(\s_desc_data[13] ), 
            .DO5(\s_desc_data[14] ), .DO6(\s_desc_data[15] ), .DO9(s_pos_0_3__N_670[0]), 
            .DO10(s_pos_0_3__N_670[1]), .DO11(s_pos_0_3__N_670[2]), .DO12(s_pos_0_3__N_670[3]), 
            .DO13(\s_desc_data[4] ), .DO14(\s_desc_data[5] ), .DO15(\s_desc_data[6] ), 
            .DO16(\s_desc_data[7] ), .DO17(\s_desc_data[8] ));
    defparam n29320.DATA_WIDTH_W = 18;
    defparam n29320.DATA_WIDTH_R = 18;
    defparam n29320.REGMODE = "NOREG";
    defparam n29320.CSDECODE_W = "0b000";
    defparam n29320.CSDECODE_R = "0b000";
    defparam n29320.GSR = "DISABLED";
    defparam n29320.RESETMODE = "SYNC";
    defparam n29320.ASYNC_RESET_RELEASE = "SYNC";
    defparam n29320.INIT_DATA = "STATIC";
    defparam n29320.INITVAL_00 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_01 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_02 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_03 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_04 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_05 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_06 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_07 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_08 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_09 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_0A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_0B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_0C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_0D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_0E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_0F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_10 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_11 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_12 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_13 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_14 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_15 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_16 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_17 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_18 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_19 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_1A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_1B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_1C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_1D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_1E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n29320.INITVAL_1F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    
endmodule
//
// Verilog Description of module \jtag_clk_scaler(1000) 
//

module \jtag_clk_scaler(1000)  (GND_net, mclk_c, s_1mhz_stb, s_1khz_stb);
    input GND_net;
    input mclk_c;
    input s_1mhz_stb;
    output s_1khz_stb;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    wire n7005;
    wire [9:0]s_count;   // c:/devel/yard-ice/src/fpga/jtag_clk_scaler.vhdl(54[9:16])
    wire [9:0]n35;
    
    wire n7006, n4445, p_stb_N_1558, n7007, n8, n7717, n7749, 
        n15, n20, n18, n7003, n7004;
    
    CCU2D s_count_754_add_4_7 (.A0(s_count[5]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[6]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7005), .COUT(n7006), .S0(n35[5]), .S1(n35[6]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754_add_4_7.INIT0 = 16'hfaaa;
    defparam s_count_754_add_4_7.INIT1 = 16'hfaaa;
    defparam s_count_754_add_4_7.INJECT1_0 = "NO";
    defparam s_count_754_add_4_7.INJECT1_1 = "NO";
    FD1P3IX s_count_754__i0 (.D(n35[0]), .SP(s_1mhz_stb), .CD(n4445), 
            .CK(mclk_c), .Q(s_count[0])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754__i0.GSR = "ENABLED";
    FD1S3AX p_stb_26 (.D(p_stb_N_1558), .CK(mclk_c), .Q(s_1khz_stb)) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=38, LSE_LLINE=1022, LSE_RLINE=1022 */ ;   // c:/devel/yard-ice/src/fpga/jtag_clk_scaler.vhdl(87[3] 93[10])
    defparam p_stb_26.GSR = "ENABLED";
    FD1P3IX s_count_754__i1 (.D(n35[1]), .SP(s_1mhz_stb), .CD(n4445), 
            .CK(mclk_c), .Q(s_count[1])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754__i1.GSR = "ENABLED";
    FD1P3IX s_count_754__i2 (.D(n35[2]), .SP(s_1mhz_stb), .CD(n4445), 
            .CK(mclk_c), .Q(s_count[2])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754__i2.GSR = "ENABLED";
    FD1P3IX s_count_754__i3 (.D(n35[3]), .SP(s_1mhz_stb), .CD(n4445), 
            .CK(mclk_c), .Q(s_count[3])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754__i3.GSR = "ENABLED";
    FD1P3IX s_count_754__i4 (.D(n35[4]), .SP(s_1mhz_stb), .CD(n4445), 
            .CK(mclk_c), .Q(s_count[4])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754__i4.GSR = "ENABLED";
    CCU2D s_count_754_add_4_11 (.A0(s_count[9]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n7007), .S0(n35[9]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754_add_4_11.INIT0 = 16'hfaaa;
    defparam s_count_754_add_4_11.INIT1 = 16'h0000;
    defparam s_count_754_add_4_11.INJECT1_0 = "NO";
    defparam s_count_754_add_4_11.INJECT1_1 = "NO";
    FD1P3IX s_count_754__i5 (.D(n35[5]), .SP(s_1mhz_stb), .CD(n4445), 
            .CK(mclk_c), .Q(s_count[5])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754__i5.GSR = "ENABLED";
    CCU2D s_count_754_add_4_9 (.A0(s_count[7]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[8]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7006), .COUT(n7007), .S0(n35[7]), .S1(n35[8]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754_add_4_9.INIT0 = 16'hfaaa;
    defparam s_count_754_add_4_9.INIT1 = 16'hfaaa;
    defparam s_count_754_add_4_9.INJECT1_0 = "NO";
    defparam s_count_754_add_4_9.INJECT1_1 = "NO";
    LUT4 i2325_4_lut (.A(s_1mhz_stb), .B(s_count[2]), .C(n8), .D(s_count[3]), 
         .Z(n4445)) /* synthesis lut_function=(!(((C+(D))+!B)+!A)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i2325_4_lut.init = 16'h0008;
    LUT4 i3_4_lut (.A(n7717), .B(s_count[4]), .C(n7749), .D(s_count[9]), 
         .Z(n8)) /* synthesis lut_function=((B+!(C (D)))+!A) */ ;
    defparam i3_4_lut.init = 16'hdfff;
    LUT4 i5666_2_lut (.A(s_count[5]), .B(s_count[7]), .Z(n7717)) /* synthesis lut_function=(A (B)) */ ;
    defparam i5666_2_lut.init = 16'h8888;
    LUT4 i5695_4_lut (.A(s_count[6]), .B(s_count[8]), .C(s_count[0]), 
         .D(s_count[1]), .Z(n7749)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i5695_4_lut.init = 16'h8000;
    LUT4 i10_4_lut (.A(n15), .B(n20), .C(s_count[2]), .D(s_1mhz_stb), 
         .Z(p_stb_N_1558)) /* synthesis lut_function=(!(((C+!(D))+!B)+!A)) */ ;
    defparam i10_4_lut.init = 16'h0800;
    LUT4 i4_2_lut (.A(s_count[4]), .B(s_count[1]), .Z(n15)) /* synthesis lut_function=(A (B)) */ ;
    defparam i4_2_lut.init = 16'h8888;
    LUT4 i9_4_lut (.A(s_count[7]), .B(n18), .C(s_count[6]), .D(s_count[9]), 
         .Z(n20)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;
    defparam i9_4_lut.init = 16'h0080;
    LUT4 i7_4_lut (.A(s_count[8]), .B(s_count[5]), .C(s_count[3]), .D(s_count[0]), 
         .Z(n18)) /* synthesis lut_function=(!(((C+!(D))+!B)+!A)) */ ;
    defparam i7_4_lut.init = 16'h0800;
    FD1P3IX s_count_754__i6 (.D(n35[6]), .SP(s_1mhz_stb), .CD(n4445), 
            .CK(mclk_c), .Q(s_count[6])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754__i6.GSR = "ENABLED";
    FD1P3IX s_count_754__i7 (.D(n35[7]), .SP(s_1mhz_stb), .CD(n4445), 
            .CK(mclk_c), .Q(s_count[7])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754__i7.GSR = "ENABLED";
    FD1P3IX s_count_754__i8 (.D(n35[8]), .SP(s_1mhz_stb), .CD(n4445), 
            .CK(mclk_c), .Q(s_count[8])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754__i8.GSR = "ENABLED";
    FD1P3IX s_count_754__i9 (.D(n35[9]), .SP(s_1mhz_stb), .CD(n4445), 
            .CK(mclk_c), .Q(s_count[9])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754__i9.GSR = "ENABLED";
    CCU2D s_count_754_add_4_3 (.A0(s_count[1]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[2]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7003), .COUT(n7004), .S0(n35[1]), .S1(n35[2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754_add_4_3.INIT0 = 16'hfaaa;
    defparam s_count_754_add_4_3.INIT1 = 16'hfaaa;
    defparam s_count_754_add_4_3.INJECT1_0 = "NO";
    defparam s_count_754_add_4_3.INJECT1_1 = "NO";
    CCU2D s_count_754_add_4_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[0]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .COUT(n7003), .S1(n35[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754_add_4_1.INIT0 = 16'hF000;
    defparam s_count_754_add_4_1.INIT1 = 16'h0555;
    defparam s_count_754_add_4_1.INJECT1_0 = "NO";
    defparam s_count_754_add_4_1.INJECT1_1 = "NO";
    CCU2D s_count_754_add_4_5 (.A0(s_count[3]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[4]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7004), .COUT(n7005), .S0(n35[3]), .S1(n35[4]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_754_add_4_5.INIT0 = 16'hfaaa;
    defparam s_count_754_add_4_5.INIT1 = 16'hfaaa;
    defparam s_count_754_add_4_5.INJECT1_0 = "NO";
    defparam s_count_754_add_4_5.INJECT1_1 = "NO";
    
endmodule
//
// Verilog Description of module \jtag_reg(16,16)_U6 
//

module \jtag_reg(16,16)_U6  (s_ckgen_rtdiv, mclk_c, s_ckgen_rtdiv_wr_stb, 
            s_reg_din);
    output [15:0]s_ckgen_rtdiv;
    input mclk_c;
    input s_ckgen_rtdiv_wr_stb;
    input [15:0]s_reg_din;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    FD1P3AX s_reg_i0_i0 (.D(s_reg_din[0]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[0])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i0.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i1 (.D(s_reg_din[1]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[1])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i1.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i2 (.D(s_reg_din[2]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[2])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i2.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i3 (.D(s_reg_din[3]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[3])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i3.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i4 (.D(s_reg_din[4]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[4])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i4.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i5 (.D(s_reg_din[5]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[5])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i5.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i6 (.D(s_reg_din[6]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[6])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i6.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i7 (.D(s_reg_din[7]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[7])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i7.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i8 (.D(s_reg_din[8]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[8])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i8.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i9 (.D(s_reg_din[9]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[9])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i9.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i10 (.D(s_reg_din[10]), .SP(s_ckgen_rtdiv_wr_stb), 
            .CK(mclk_c), .Q(s_ckgen_rtdiv[10])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i10.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i11 (.D(s_reg_din[11]), .SP(s_ckgen_rtdiv_wr_stb), 
            .CK(mclk_c), .Q(s_ckgen_rtdiv[11])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i11.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i12 (.D(s_reg_din[12]), .SP(s_ckgen_rtdiv_wr_stb), 
            .CK(mclk_c), .Q(s_ckgen_rtdiv[12])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i12.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i13 (.D(s_reg_din[13]), .SP(s_ckgen_rtdiv_wr_stb), 
            .CK(mclk_c), .Q(s_ckgen_rtdiv[13])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i13.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i14 (.D(s_reg_din[14]), .SP(s_ckgen_rtdiv_wr_stb), 
            .CK(mclk_c), .Q(s_ckgen_rtdiv[14])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i14.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i15 (.D(s_reg_din[15]), .SP(s_ckgen_rtdiv_wr_stb), 
            .CK(mclk_c), .Q(s_ckgen_rtdiv[15])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=822, LSE_RLINE=822 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i15.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module \dpram(16,8) 
//

module \dpram(16,8)  (fsmc_clk_c, mclk_c, s_ptr_mem_wr_stb, VCC_net, 
            GND_net, q_15__N_82, s_ptr_addr, q_15__N_80, q_15__N_78, 
            q_15__N_76, q_15__N_74, q_15__N_72, q_15__N_70, q_15__N_68, 
            fsmc_d_out_0, fsmc_d_out_1, fsmc_d_out_2, fsmc_d_out_3, 
            fsmc_d_out_4, fsmc_d_out_5, fsmc_d_out_6, fsmc_d_out_7, 
            fsmc_d_out_8, fsmc_d_out_9, fsmc_d_out_10, fsmc_d_out_11, 
            fsmc_d_out_12, fsmc_d_out_13, fsmc_d_out_14, fsmc_d_out_15, 
            s_ptr_data);
    input fsmc_clk_c;
    input mclk_c;
    input s_ptr_mem_wr_stb;
    input VCC_net;
    input GND_net;
    input q_15__N_82;
    input [8:1]s_ptr_addr;
    input q_15__N_80;
    input q_15__N_78;
    input q_15__N_76;
    input q_15__N_74;
    input q_15__N_72;
    input q_15__N_70;
    input q_15__N_68;
    input fsmc_d_out_0;
    input fsmc_d_out_1;
    input fsmc_d_out_2;
    input fsmc_d_out_3;
    input fsmc_d_out_4;
    input fsmc_d_out_5;
    input fsmc_d_out_6;
    input fsmc_d_out_7;
    input fsmc_d_out_8;
    input fsmc_d_out_9;
    input fsmc_d_out_10;
    input fsmc_d_out_11;
    input fsmc_d_out_12;
    input fsmc_d_out_13;
    input fsmc_d_out_14;
    input fsmc_d_out_15;
    output [15:0]s_ptr_data;
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(73[2:10])
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    PDPW8KC mem0 (.DI0(fsmc_d_out_0), .DI1(fsmc_d_out_1), .DI2(fsmc_d_out_2), 
            .DI3(fsmc_d_out_3), .DI4(fsmc_d_out_4), .DI5(fsmc_d_out_5), 
            .DI6(fsmc_d_out_6), .DI7(fsmc_d_out_7), .DI8(fsmc_d_out_8), 
            .DI9(fsmc_d_out_9), .DI10(fsmc_d_out_10), .DI11(fsmc_d_out_11), 
            .DI12(fsmc_d_out_12), .DI13(fsmc_d_out_13), .DI14(fsmc_d_out_14), 
            .DI15(fsmc_d_out_15), .DI16(GND_net), .DI17(GND_net), .ADW0(q_15__N_82), 
            .ADW1(q_15__N_80), .ADW2(q_15__N_78), .ADW3(q_15__N_76), .ADW4(q_15__N_74), 
            .ADW5(q_15__N_72), .ADW6(q_15__N_70), .ADW7(q_15__N_68), .ADW8(GND_net), 
            .BE0(VCC_net), .BE1(VCC_net), .CEW(s_ptr_mem_wr_stb), .CLKW(fsmc_clk_c), 
            .CSW0(GND_net), .CSW1(GND_net), .CSW2(GND_net), .ADR0(GND_net), 
            .ADR1(GND_net), .ADR2(GND_net), .ADR3(GND_net), .ADR4(s_ptr_addr[1]), 
            .ADR5(s_ptr_addr[2]), .ADR6(s_ptr_addr[3]), .ADR7(s_ptr_addr[4]), 
            .ADR8(s_ptr_addr[5]), .ADR9(s_ptr_addr[6]), .ADR10(s_ptr_addr[7]), 
            .ADR11(s_ptr_addr[8]), .ADR12(GND_net), .CER(VCC_net), .OCER(VCC_net), 
            .CLKR(mclk_c), .CSR0(GND_net), .CSR1(GND_net), .CSR2(GND_net), 
            .RST(GND_net), .DO0(s_ptr_data[9]), .DO1(s_ptr_data[10]), 
            .DO2(s_ptr_data[11]), .DO3(s_ptr_data[12]), .DO4(s_ptr_data[13]), 
            .DO5(s_ptr_data[14]), .DO6(s_ptr_data[15]), .DO9(s_ptr_data[0]), 
            .DO10(s_ptr_data[1]), .DO11(s_ptr_data[2]), .DO12(s_ptr_data[3]), 
            .DO13(s_ptr_data[4]), .DO14(s_ptr_data[5]), .DO15(s_ptr_data[6]), 
            .DO16(s_ptr_data[7]), .DO17(s_ptr_data[8]));
    defparam mem0.DATA_WIDTH_W = 18;
    defparam mem0.DATA_WIDTH_R = 18;
    defparam mem0.REGMODE = "NOREG";
    defparam mem0.CSDECODE_W = "0b000";
    defparam mem0.CSDECODE_R = "0b000";
    defparam mem0.GSR = "DISABLED";
    defparam mem0.RESETMODE = "SYNC";
    defparam mem0.ASYNC_RESET_RELEASE = "SYNC";
    defparam mem0.INIT_DATA = "STATIC";
    defparam mem0.INITVAL_00 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_01 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_02 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_03 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_04 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_05 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_06 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_07 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_08 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_09 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_10 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_11 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_12 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_13 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_14 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_15 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_16 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_17 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_18 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_19 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    
endmodule
//
// Verilog Description of module \dpram(16,8)_U9 
//

module \dpram(16,8)_U9  (fsmc_clk_c, s_aux_mem_wr_stb, VCC_net, GND_net, 
            n9421, n9424, q_15__N_78, q_15__N_76, q_15__N_74, q_15__N_72, 
            q_15__N_70, q_15__N_68, fsmc_d_out_0, fsmc_d_out_1, fsmc_d_out_2, 
            fsmc_d_out_3, fsmc_d_out_4, fsmc_d_out_5, fsmc_d_out_6, 
            fsmc_d_out_7, fsmc_d_out_8, fsmc_d_out_9, fsmc_d_out_10, 
            fsmc_d_out_11, fsmc_d_out_12, fsmc_d_out_13, fsmc_d_out_14, 
            fsmc_d_out_15, s_aux_mem_dout);
    input fsmc_clk_c;
    input s_aux_mem_wr_stb;
    input VCC_net;
    input GND_net;
    input n9421;
    input n9424;
    input q_15__N_78;
    input q_15__N_76;
    input q_15__N_74;
    input q_15__N_72;
    input q_15__N_70;
    input q_15__N_68;
    input fsmc_d_out_0;
    input fsmc_d_out_1;
    input fsmc_d_out_2;
    input fsmc_d_out_3;
    input fsmc_d_out_4;
    input fsmc_d_out_5;
    input fsmc_d_out_6;
    input fsmc_d_out_7;
    input fsmc_d_out_8;
    input fsmc_d_out_9;
    input fsmc_d_out_10;
    input fsmc_d_out_11;
    input fsmc_d_out_12;
    input fsmc_d_out_13;
    input fsmc_d_out_14;
    input fsmc_d_out_15;
    output [15:0]s_aux_mem_dout;
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(73[2:10])
    
    DP8KC mem0 (.DIA0(fsmc_d_out_0), .DIA1(fsmc_d_out_1), .DIA2(fsmc_d_out_2), 
          .DIA3(fsmc_d_out_3), .DIA4(fsmc_d_out_4), .DIA5(fsmc_d_out_5), 
          .DIA6(fsmc_d_out_6), .DIA7(fsmc_d_out_7), .DIA8(fsmc_d_out_8), 
          .ADA0(VCC_net), .ADA1(VCC_net), .ADA2(GND_net), .ADA3(n9421), 
          .ADA4(n9424), .ADA5(q_15__N_78), .ADA6(q_15__N_76), .ADA7(q_15__N_74), 
          .ADA8(q_15__N_72), .ADA9(q_15__N_70), .ADA10(q_15__N_68), .ADA11(GND_net), 
          .ADA12(GND_net), .CEA(VCC_net), .OCEA(VCC_net), .CLKA(fsmc_clk_c), 
          .WEA(s_aux_mem_wr_stb), .CSA0(GND_net), .CSA1(GND_net), .CSA2(GND_net), 
          .RSTA(GND_net), .DIB0(fsmc_d_out_9), .DIB1(fsmc_d_out_10), .DIB2(fsmc_d_out_11), 
          .DIB3(fsmc_d_out_12), .DIB4(fsmc_d_out_13), .DIB5(fsmc_d_out_14), 
          .DIB6(fsmc_d_out_15), .DIB7(GND_net), .DIB8(GND_net), .ADB0(VCC_net), 
          .ADB1(VCC_net), .ADB2(GND_net), .ADB3(n9421), .ADB4(n9424), 
          .ADB5(q_15__N_78), .ADB6(q_15__N_76), .ADB7(q_15__N_74), .ADB8(q_15__N_72), 
          .ADB9(q_15__N_70), .ADB10(q_15__N_68), .ADB11(GND_net), .ADB12(VCC_net), 
          .CEB(VCC_net), .OCEB(VCC_net), .CLKB(fsmc_clk_c), .WEB(s_aux_mem_wr_stb), 
          .CSB0(GND_net), .CSB1(GND_net), .CSB2(GND_net), .RSTB(GND_net), 
          .DOA0(s_aux_mem_dout[0]), .DOA1(s_aux_mem_dout[1]), .DOA2(s_aux_mem_dout[2]), 
          .DOA3(s_aux_mem_dout[3]), .DOA4(s_aux_mem_dout[4]), .DOA5(s_aux_mem_dout[5]), 
          .DOA6(s_aux_mem_dout[6]), .DOA7(s_aux_mem_dout[7]), .DOA8(s_aux_mem_dout[8]), 
          .DOB0(s_aux_mem_dout[9]), .DOB1(s_aux_mem_dout[10]), .DOB2(s_aux_mem_dout[11]), 
          .DOB3(s_aux_mem_dout[12]), .DOB4(s_aux_mem_dout[13]), .DOB5(s_aux_mem_dout[14]), 
          .DOB6(s_aux_mem_dout[15]));
    defparam mem0.DATA_WIDTH_A = 9;
    defparam mem0.DATA_WIDTH_B = 9;
    defparam mem0.REGMODE_A = "NOREG";
    defparam mem0.REGMODE_B = "NOREG";
    defparam mem0.CSDECODE_A = "0b000";
    defparam mem0.CSDECODE_B = "0b000";
    defparam mem0.WRITEMODE_A = "READBEFOREWRITE";
    defparam mem0.WRITEMODE_B = "READBEFOREWRITE";
    defparam mem0.GSR = "DISABLED";
    defparam mem0.RESETMODE = "SYNC";
    defparam mem0.ASYNC_RESET_RELEASE = "SYNC";
    defparam mem0.INIT_DATA = "STATIC";
    defparam mem0.INITVAL_00 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_01 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_02 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_03 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_04 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_05 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_06 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_07 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_08 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_09 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_10 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_11 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_12 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_13 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_14 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_15 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_16 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_17 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_18 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_19 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    
endmodule
//
// Verilog Description of module \jtag_led_drv(31,false,false,false)_U3 
//

module \jtag_led_drv(31,false,false,false)_U3  (mclk_c, led_2_c, n8843, 
            s_1khz_stb);
    input mclk_c;
    output led_2_c;
    input n8843;
    input s_1khz_stb;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    wire [4:0]s_count;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(59[9:16])
    
    wire mclk_c_enable_24, s_cnt_ld, n4068, n4070, n4074, n3507, 
        n4072, s_trip_r1, s_trip_r0, n4413, n6, n4014, n8872, 
        n8873;
    
    FD1P3JX s_count_i0_i4 (.D(n4068), .SP(mclk_c_enable_24), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[4])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1060, LSE_RLINE=1060 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i4.GSR = "ENABLED";
    FD1P3JX s_count_i0_i3 (.D(n4070), .SP(mclk_c_enable_24), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[3])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1060, LSE_RLINE=1060 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i3.GSR = "ENABLED";
    FD1P3JX s_count_i0_i2 (.D(n4074), .SP(mclk_c_enable_24), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[2])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1060, LSE_RLINE=1060 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i2.GSR = "ENABLED";
    FD1S3JX s_count_i0_i0 (.D(n3507), .CK(mclk_c), .PD(s_cnt_ld), .Q(s_count[0])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1060, LSE_RLINE=1060 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i0.GSR = "ENABLED";
    FD1P3JX s_count_i0_i1 (.D(n4072), .SP(mclk_c_enable_24), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[1])) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1060, LSE_RLINE=1060 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i1.GSR = "ENABLED";
    FD1S3AX s_trip_r1_31 (.D(s_trip_r0), .CK(mclk_c), .Q(s_trip_r1)) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1060, LSE_RLINE=1060 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(95[3] 105[10])
    defparam s_trip_r1_31.GSR = "ENABLED";
    FD1S3AX s_drv_ff_32 (.D(n4413), .CK(mclk_c), .Q(led_2_c)) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1060, LSE_RLINE=1060 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(113[3] 121[10])
    defparam s_drv_ff_32.GSR = "ENABLED";
    FD1S3AX s_trip_r0_30 (.D(n8843), .CK(mclk_c), .Q(s_trip_r0)) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=1060, LSE_RLINE=1060 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(95[3] 105[10])
    defparam s_trip_r0_30.GSR = "ENABLED";
    LUT4 i4_4_lut (.A(s_count[3]), .B(s_count[2]), .C(s_count[0]), .D(n6), 
         .Z(n4014)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(140[22:45])
    defparam i4_4_lut.init = 16'hfffe;
    LUT4 i1_2_lut (.A(s_count[1]), .B(s_count[4]), .Z(n6)) /* synthesis lut_function=(A+(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(140[22:45])
    defparam i1_2_lut.init = 16'heeee;
    LUT4 i1_2_lut_adj_201 (.A(s_count[1]), .B(s_count[0]), .Z(n4072)) /* synthesis lut_function=(A (B)+!A !(B)) */ ;
    defparam i1_2_lut_adj_201.init = 16'h9999;
    LUT4 i1_3_lut_4_lut (.A(s_count[2]), .B(n8872), .C(s_count[3]), .D(s_count[4]), 
         .Z(n4068)) /* synthesis lut_function=(A (D)+!A (B (D)+!B (C (D)+!C !(D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i1_3_lut_4_lut.init = 16'hfe01;
    LUT4 i990_2_lut_rep_205 (.A(s_count[1]), .B(s_count[0]), .Z(n8872)) /* synthesis lut_function=(A+(B)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i990_2_lut_rep_205.init = 16'heeee;
    LUT4 i1_2_lut_3_lut (.A(s_count[1]), .B(s_count[0]), .C(s_count[2]), 
         .Z(n4074)) /* synthesis lut_function=(A (C)+!A (B (C)+!B !(C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i1_2_lut_3_lut.init = 16'he1e1;
    LUT4 i1_2_lut_3_lut_4_lut (.A(s_count[1]), .B(s_count[0]), .C(s_count[3]), 
         .D(s_count[2]), .Z(n4070)) /* synthesis lut_function=(A (C)+!A (B (C)+!B (C (D)+!C !(D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i1_2_lut_3_lut_4_lut.init = 16'hf0e1;
    LUT4 i2_3_lut_4_lut (.A(s_1khz_stb), .B(led_2_c), .C(n4014), .D(n8873), 
         .Z(mclk_c_enable_24)) /* synthesis lut_function=(A (B+((D)+!C))+!A ((D)+!C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(134[10:41])
    defparam i2_3_lut_4_lut.init = 16'hff8f;
    LUT4 i1800_2_lut_3_lut (.A(s_1khz_stb), .B(led_2_c), .C(s_count[0]), 
         .Z(n3507)) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(134[10:41])
    defparam i1800_2_lut_3_lut.init = 16'h7878;
    LUT4 s_trip_r0_I_0_2_lut_rep_206 (.A(s_trip_r0), .B(s_trip_r1), .Z(n8873)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(108[15:42])
    defparam s_trip_r0_I_0_2_lut_rep_206.init = 16'h2222;
    LUT4 z_I_0_2_lut_3_lut (.A(s_trip_r0), .B(s_trip_r1), .C(n4014), .Z(s_cnt_ld)) /* synthesis lut_function=(!(A (B (C))+!A (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(108[15:42])
    defparam z_I_0_2_lut_3_lut.init = 16'h2f2f;
    LUT4 i2313_3_lut_4_lut (.A(s_trip_r0), .B(s_trip_r1), .C(led_2_c), 
         .D(n4014), .Z(n4413)) /* synthesis lut_function=(A ((C (D))+!B)+!A (C (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(108[15:42])
    defparam i2313_3_lut_4_lut.init = 16'hf222;
    
endmodule
//
// Verilog Description of module \jtag_reg(16,4) 
//

module \jtag_reg(16,4)  (\s_irq_en[0] , mclk_c, s_irq_en_wr_stb, \s_reg_din[0] , 
            \s_irq_en[1] , \s_reg_din[1] , \s_irq_en[2] , \s_reg_din[2] , 
            \s_irq_en[3] , \s_reg_din[3] );
    output \s_irq_en[0] ;
    input mclk_c;
    input s_irq_en_wr_stb;
    input \s_reg_din[0] ;
    output \s_irq_en[1] ;
    input \s_reg_din[1] ;
    output \s_irq_en[2] ;
    input \s_reg_din[2] ;
    output \s_irq_en[3] ;
    input \s_reg_din[3] ;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    FD1P3AX s_reg__i1 (.D(\s_reg_din[0] ), .SP(s_irq_en_wr_stb), .CK(mclk_c), 
            .Q(\s_irq_en[0] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=28, LSE_LLINE=645, LSE_RLINE=645 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i1.GSR = "ENABLED";
    FD1P3AX s_reg__i2 (.D(\s_reg_din[1] ), .SP(s_irq_en_wr_stb), .CK(mclk_c), 
            .Q(\s_irq_en[1] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=28, LSE_LLINE=645, LSE_RLINE=645 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i2.GSR = "ENABLED";
    FD1P3AX s_reg__i3 (.D(\s_reg_din[2] ), .SP(s_irq_en_wr_stb), .CK(mclk_c), 
            .Q(\s_irq_en[2] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=28, LSE_LLINE=645, LSE_RLINE=645 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i3.GSR = "ENABLED";
    FD1P3AX s_reg__i4 (.D(\s_reg_din[3] ), .SP(s_irq_en_wr_stb), .CK(mclk_c), 
            .Q(\s_irq_en[3] )) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=13, LSE_RCOL=28, LSE_LLINE=645, LSE_RLINE=645 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i4.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module TSALL
// module not written out since it is a black-box. 
//

//
// Verilog Description of module \dpram(16,10) 
//

module \dpram(16,10)  (fsmc_clk_c, mclk_c, fsmc_clk_c_enable_1, VCC_net, 
            GND_net, q_15__N_82, s_addr_0, q_15__N_80, q_15__N_78, 
            q_15__N_76, q_15__N_74, q_15__N_72, q_15__N_70, q_15__N_68, 
            q_15__N_66, fsmc_d_out_0, fsmc_d_out_1, fsmc_d_out_2, fsmc_d_out_3, 
            fsmc_d_out_4, fsmc_d_out_5, fsmc_d_out_6, fsmc_d_out_7, 
            fsmc_d_out_8, fsmc_d_out_9, fsmc_d_out_10, fsmc_d_out_11, 
            fsmc_d_out_12, fsmc_d_out_13, fsmc_d_out_14, fsmc_d_out_15, 
            n2978, n2979, n2980, n2981, n2982, n2983, n2984, n2985, 
            n2986, n2987, n2988, n2989, n2990, n2991, n2992, n2993, 
            fsmc_clk_c_enable_2, n9422, n9426, n2994, n2995, n2996, 
            n2997, n2998, n2999, n3000, n3001, n3002, n3003, n3004, 
            n3005, n3006, n3007, n3008, n3009, n3010);
    input fsmc_clk_c;
    input mclk_c;
    input fsmc_clk_c_enable_1;
    input VCC_net;
    input GND_net;
    input q_15__N_82;
    input [9:0]s_addr_0;
    input q_15__N_80;
    input q_15__N_78;
    input q_15__N_76;
    input q_15__N_74;
    input q_15__N_72;
    input q_15__N_70;
    input q_15__N_68;
    input q_15__N_66;
    input fsmc_d_out_0;
    input fsmc_d_out_1;
    input fsmc_d_out_2;
    input fsmc_d_out_3;
    input fsmc_d_out_4;
    input fsmc_d_out_5;
    input fsmc_d_out_6;
    input fsmc_d_out_7;
    input fsmc_d_out_8;
    input fsmc_d_out_9;
    input fsmc_d_out_10;
    input fsmc_d_out_11;
    input fsmc_d_out_12;
    input fsmc_d_out_13;
    input fsmc_d_out_14;
    input fsmc_d_out_15;
    output n2978;
    output n2979;
    output n2980;
    output n2981;
    output n2982;
    output n2983;
    output n2984;
    output n2985;
    output n2986;
    output n2987;
    output n2988;
    output n2989;
    output n2990;
    output n2991;
    output n2992;
    output n2993;
    input fsmc_clk_c_enable_2;
    input n9422;
    input n9426;
    output n2994;
    output n2995;
    output n2996;
    output n2997;
    output n2998;
    output n2999;
    output n3000;
    output n3001;
    output n3002;
    output n3003;
    output n3004;
    output n3005;
    output n3006;
    output n3007;
    output n3008;
    output n3009;
    output n3010;
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(73[2:10])
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    PDPW8KC mem0 (.DI0(fsmc_d_out_0), .DI1(fsmc_d_out_1), .DI2(fsmc_d_out_2), 
            .DI3(fsmc_d_out_3), .DI4(fsmc_d_out_4), .DI5(fsmc_d_out_5), 
            .DI6(fsmc_d_out_6), .DI7(fsmc_d_out_7), .DI8(fsmc_d_out_8), 
            .DI9(fsmc_d_out_9), .DI10(fsmc_d_out_10), .DI11(fsmc_d_out_11), 
            .DI12(fsmc_d_out_12), .DI13(fsmc_d_out_13), .DI14(fsmc_d_out_14), 
            .DI15(fsmc_d_out_15), .DI16(GND_net), .DI17(GND_net), .ADW0(q_15__N_82), 
            .ADW1(q_15__N_80), .ADW2(q_15__N_78), .ADW3(q_15__N_76), .ADW4(q_15__N_74), 
            .ADW5(q_15__N_72), .ADW6(q_15__N_70), .ADW7(q_15__N_68), .ADW8(q_15__N_66), 
            .BE0(VCC_net), .BE1(VCC_net), .CEW(fsmc_clk_c_enable_1), .CLKW(fsmc_clk_c), 
            .CSW0(GND_net), .CSW1(GND_net), .CSW2(GND_net), .ADR0(GND_net), 
            .ADR1(GND_net), .ADR2(GND_net), .ADR3(GND_net), .ADR4(s_addr_0[0]), 
            .ADR5(s_addr_0[1]), .ADR6(s_addr_0[2]), .ADR7(s_addr_0[3]), 
            .ADR8(s_addr_0[4]), .ADR9(s_addr_0[5]), .ADR10(s_addr_0[6]), 
            .ADR11(s_addr_0[7]), .ADR12(s_addr_0[8]), .CER(VCC_net), .OCER(VCC_net), 
            .CLKR(mclk_c), .CSR0(GND_net), .CSR1(GND_net), .CSR2(GND_net), 
            .RST(GND_net), .DO0(n2987), .DO1(n2988), .DO2(n2989), .DO3(n2990), 
            .DO4(n2991), .DO5(n2992), .DO6(n2993), .DO9(n2978), .DO10(n2979), 
            .DO11(n2980), .DO12(n2981), .DO13(n2982), .DO14(n2983), 
            .DO15(n2984), .DO16(n2985), .DO17(n2986));
    defparam mem0.DATA_WIDTH_W = 18;
    defparam mem0.DATA_WIDTH_R = 18;
    defparam mem0.REGMODE = "NOREG";
    defparam mem0.CSDECODE_W = "0b000";
    defparam mem0.CSDECODE_R = "0b000";
    defparam mem0.GSR = "DISABLED";
    defparam mem0.RESETMODE = "SYNC";
    defparam mem0.ASYNC_RESET_RELEASE = "SYNC";
    defparam mem0.INIT_DATA = "STATIC";
    defparam mem0.INITVAL_00 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_01 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_02 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_03 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_04 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_05 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_06 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_07 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_08 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_09 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_10 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_11 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_12 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_13 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_14 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_15 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_16 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_17 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_18 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_19 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    PDPW8KC mem1 (.DI0(fsmc_d_out_0), .DI1(fsmc_d_out_1), .DI2(fsmc_d_out_2), 
            .DI3(fsmc_d_out_3), .DI4(fsmc_d_out_4), .DI5(fsmc_d_out_5), 
            .DI6(fsmc_d_out_6), .DI7(fsmc_d_out_7), .DI8(fsmc_d_out_8), 
            .DI9(fsmc_d_out_9), .DI10(fsmc_d_out_10), .DI11(fsmc_d_out_11), 
            .DI12(fsmc_d_out_12), .DI13(fsmc_d_out_13), .DI14(fsmc_d_out_14), 
            .DI15(fsmc_d_out_15), .DI16(GND_net), .DI17(GND_net), .ADW0(n9422), 
            .ADW1(n9426), .ADW2(q_15__N_78), .ADW3(q_15__N_76), .ADW4(q_15__N_74), 
            .ADW5(q_15__N_72), .ADW6(q_15__N_70), .ADW7(q_15__N_68), .ADW8(q_15__N_66), 
            .BE0(VCC_net), .BE1(VCC_net), .CEW(fsmc_clk_c_enable_2), .CLKW(fsmc_clk_c), 
            .CSW0(GND_net), .CSW1(GND_net), .CSW2(GND_net), .ADR0(GND_net), 
            .ADR1(GND_net), .ADR2(GND_net), .ADR3(GND_net), .ADR4(s_addr_0[0]), 
            .ADR5(s_addr_0[1]), .ADR6(s_addr_0[2]), .ADR7(s_addr_0[3]), 
            .ADR8(s_addr_0[4]), .ADR9(s_addr_0[5]), .ADR10(s_addr_0[6]), 
            .ADR11(s_addr_0[7]), .ADR12(s_addr_0[8]), .CER(VCC_net), .OCER(VCC_net), 
            .CLKR(mclk_c), .CSR0(GND_net), .CSR1(GND_net), .CSR2(GND_net), 
            .RST(GND_net), .DO0(n3003), .DO1(n3004), .DO2(n3005), .DO3(n3006), 
            .DO4(n3007), .DO5(n3008), .DO6(n3009), .DO9(n2994), .DO10(n2995), 
            .DO11(n2996), .DO12(n2997), .DO13(n2998), .DO14(n2999), 
            .DO15(n3000), .DO16(n3001), .DO17(n3002));
    defparam mem1.DATA_WIDTH_W = 18;
    defparam mem1.DATA_WIDTH_R = 18;
    defparam mem1.REGMODE = "NOREG";
    defparam mem1.CSDECODE_W = "0b000";
    defparam mem1.CSDECODE_R = "0b000";
    defparam mem1.GSR = "DISABLED";
    defparam mem1.RESETMODE = "SYNC";
    defparam mem1.ASYNC_RESET_RELEASE = "SYNC";
    defparam mem1.INIT_DATA = "STATIC";
    defparam mem1.INITVAL_00 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_01 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_02 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_03 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_04 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_05 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_06 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_07 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_08 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_09 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_0A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_0B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_0C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_0D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_0E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_0F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_10 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_11 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_12 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_13 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_14 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_15 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_16 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_17 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_18 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_19 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_1A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_1B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_1C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_1D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_1E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_1F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    FD1S3AX _1104 (.D(s_addr_0[9]), .CK(mclk_c), .Q(n3010));
    defparam _1104.GSR = "DISABLED";
    
endmodule
//
// Verilog Description of module PUR
// module not written out since it is a black-box. 
//

//
// Verilog Description of module \jtag_enh_shifter(16,10,10,16,8,32,8,16,16) 
//

module \jtag_enh_shifter(16,10,10,16,8,32,8,16,16)  (n2993, n3009, n3010, 
            n2992, n3008, n2991, n3007, n2990, n3006, n2989, n3005, 
            n2988, n3004, n2987, n3003, n2986, n3002, n2985, n3001, 
            n2984, n3000, n2983, n2999, n2982, n2998, n2981, n2997, 
            n2980, n2996, n2979, n2995, n2978, n2994, GND_net, 
            \s_desc_data[18] , \s_desc_data[19] , s_addr_0, \s_desc_data[16] , 
            \s_desc_data[17] , mclk_c, \s_desc_data[14] , \s_desc_data[15] , 
            \s_desc_data[12] , \s_desc_data[13] , \s_desc_data[10] , \s_desc_data[11] , 
            \s_desc_data[8] , \s_desc_data[30] , \s_desc_data[9] , \s_desc_data[6] , 
            \s_desc_data[7] , \s_desc_data[4] , \s_desc_data[5] , s_pos_0_3__N_670, 
            s_ptr_data, s_desc_addr, n9428, mclk_c_enable_112, s_rx_mem_wr_stb, 
            \s_desc_data[31] , n9419, s_rx_mem_addr, s_rx_mem_din, \s_desc_data[29] , 
            \s_desc_data[27] , \s_desc_data[28] , \s_desc_data[25] , \s_desc_data[26] , 
            \s_desc_data[23] , \s_desc_data[24] , \s_desc_data[21] , \s_desc_data[22] , 
            \s_desc_data[20] , n9420, mclk_c_enable_81, tp_tms_c, s_tap_tdo, 
            s_tap_tdi, \s_status[2] , \s_status[3] , \s_status[0] , 
            \s_status[1] , s_end_state, s_reg_din, n8820, n8832, s_tap_busy, 
            n8829, n8828, n8856, out_get_N_312, n7771, n2, n5312, 
            s_ptr_addr, s_cnt_en_N_1514, \s_goto_state[2] , \s_goto_state[0] , 
            \s_goto_state[1] , n1, n7781, s_ckgen_div, \s_irq[1] , 
            s_ckgen_rtdiv, \s_cfg[5] , tp_rtck_c, tp_tck_c, \s_irq_en[1] , 
            \irq_N_8[1] );
    input n2993;
    input n3009;
    input n3010;
    input n2992;
    input n3008;
    input n2991;
    input n3007;
    input n2990;
    input n3006;
    input n2989;
    input n3005;
    input n2988;
    input n3004;
    input n2987;
    input n3003;
    input n2986;
    input n3002;
    input n2985;
    input n3001;
    input n2984;
    input n3000;
    input n2983;
    input n2999;
    input n2982;
    input n2998;
    input n2981;
    input n2997;
    input n2980;
    input n2996;
    input n2979;
    input n2995;
    input n2978;
    input n2994;
    input GND_net;
    input \s_desc_data[18] ;
    input \s_desc_data[19] ;
    output [9:0]s_addr_0;
    input \s_desc_data[16] ;
    input \s_desc_data[17] ;
    input mclk_c;
    input \s_desc_data[14] ;
    input \s_desc_data[15] ;
    input \s_desc_data[12] ;
    input \s_desc_data[13] ;
    input \s_desc_data[10] ;
    input \s_desc_data[11] ;
    input \s_desc_data[8] ;
    input \s_desc_data[30] ;
    input \s_desc_data[9] ;
    input \s_desc_data[6] ;
    input \s_desc_data[7] ;
    input \s_desc_data[4] ;
    input \s_desc_data[5] ;
    input [3:0]s_pos_0_3__N_670;
    input [15:0]s_ptr_data;
    output [8:1]s_desc_addr;
    input n9428;
    output mclk_c_enable_112;
    output s_rx_mem_wr_stb;
    input \s_desc_data[31] ;
    input n9419;
    output [9:0]s_rx_mem_addr;
    output [15:0]s_rx_mem_din;
    input \s_desc_data[29] ;
    input \s_desc_data[27] ;
    input \s_desc_data[28] ;
    input \s_desc_data[25] ;
    input \s_desc_data[26] ;
    input \s_desc_data[23] ;
    input \s_desc_data[24] ;
    input \s_desc_data[21] ;
    input \s_desc_data[22] ;
    input \s_desc_data[20] ;
    input n9420;
    input mclk_c_enable_81;
    output tp_tms_c;
    input s_tap_tdo;
    output s_tap_tdi;
    output \s_status[2] ;
    output \s_status[3] ;
    output \s_status[0] ;
    output \s_status[1] ;
    output [3:0]s_end_state;
    input [15:0]s_reg_din;
    output n8820;
    output n8832;
    output s_tap_busy;
    output n8829;
    output n8828;
    output n8856;
    input out_get_N_312;
    input n7771;
    input n2;
    input n5312;
    output [8:1]s_ptr_addr;
    output s_cnt_en_N_1514;
    input \s_goto_state[2] ;
    input \s_goto_state[0] ;
    input \s_goto_state[1] ;
    output n1;
    output n7781;
    input [15:0]s_ckgen_div;
    output \s_irq[1] ;
    input [15:0]s_ckgen_rtdiv;
    input \s_cfg[5] ;
    input tp_rtck_c;
    output tp_tck_c;
    input \s_irq_en[1] ;
    output \irq_N_8[1] ;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    wire mclk_c_enable_104 /* synthesis is_clock=1, SET_AS_NETWORK=mclk_c_enable_104 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    wire n9427, s_msbf, s_msbf_r, s_en_r1, n8771, s_tx_shift_stb, 
        s_so;
    wire [7:0]s_offs_r;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(80[9:17])
    
    wire n2873, n8774;
    wire [3:0]\s_op_fifo.buf_3_.msbf_N_1174 ;
    
    wire mclk_c_enable_242, tckn_stb, mclk_c_enable_129, s_en_r0, n8781, 
        mclk_c_enable_218, mclk_c_enable_206, mclk_c_enable_230, n8765, 
        n2898, mclk_c_enable_203;
    wire [9:0]s_count_r;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(121[9:18])
    
    wire s_count_r_0__N_893;
    wire [3:0]bit_pos;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(76[2:9])
    
    wire bit_pos_0__N_816, n8834, n8822, s_count_r_1__N_897, n8835, 
        op_msbf, n8836, s_offs_inc, s_mtdo, s_mtms, n7643, s_tcken, 
        s_tdien;
    
    \jtag_enh_tx(16,10,10,4)  tx (.n9427(n9427), .n2993(n2993), .n3009(n3009), 
            .n3010(n3010), .n2992(n2992), .n3008(n3008), .n2991(n2991), 
            .n3007(n3007), .n2990(n2990), .n3006(n3006), .n2989(n2989), 
            .n3005(n3005), .n2988(n2988), .n3004(n3004), .n2987(n2987), 
            .n3003(n3003), .n2986(n2986), .n3002(n3002), .n2985(n2985), 
            .n3001(n3001), .n2984(n2984), .n3000(n3000), .n2983(n2983), 
            .n2999(n2999), .n2982(n2982), .n2998(n2998), .n2981(n2981), 
            .n2997(n2997), .n2980(n2980), .n2996(n2996), .n2979(n2979), 
            .n2995(n2995), .n2978(n2978), .n2994(n2994), .s_msbf(s_msbf), 
            .GND_net(GND_net), .\s_desc_data[18] (\s_desc_data[18] ), .\s_desc_data[19] (\s_desc_data[19] ), 
            .s_addr_0({s_addr_0}), .\s_desc_data[16] (\s_desc_data[16] ), 
            .\s_desc_data[17] (\s_desc_data[17] ), .mclk_c(mclk_c), .s_msbf_r(s_msbf_r), 
            .\s_desc_data[14] (\s_desc_data[14] ), .\s_desc_data[15] (\s_desc_data[15] ), 
            .\s_desc_data[12] (\s_desc_data[12] ), .\s_desc_data[13] (\s_desc_data[13] ), 
            .\s_desc_data[10] (\s_desc_data[10] ), .\s_desc_data[11] (\s_desc_data[11] ), 
            .s_en_r1(s_en_r1), .n8771(n8771), .s_tx_shift_stb(s_tx_shift_stb), 
            .\s_desc_data[8] (\s_desc_data[8] ), .\s_desc_data[30] (\s_desc_data[30] ), 
            .\s_desc_data[9] (\s_desc_data[9] ), .\s_desc_data[6] (\s_desc_data[6] ), 
            .\s_desc_data[7] (\s_desc_data[7] ), .\s_desc_data[4] (\s_desc_data[4] ), 
            .\s_desc_data[5] (\s_desc_data[5] ), .s_so(s_so), .s_pos_0_3__N_670({s_pos_0_3__N_670}));   // c:/devel/yard-ice/src/fpga/jtag_enh_shifter.vhdl(236[7:25])
    \jtag_enh_shift_ctrl(10,8)  shift_ctrl (.s_offs_r({Open_7, Open_8, Open_9, 
            Open_10, Open_11, Open_12, Open_13, s_offs_r[0]}), .mclk_c(mclk_c), 
            .n2873(n2873), .s_ptr_data({s_ptr_data}), .GND_net(GND_net), 
            .s_desc_addr({s_desc_addr}), .\s_offs_r[1] (s_offs_r[1]), .n8774(n8774), 
            .n8771(n8771), .s_en_r1(s_en_r1), .\s_op_fifo.buf_3_.msbf_N_1174 ({\s_op_fifo.buf_3_.msbf_N_1174 }), 
            .n9428(n9428), .mclk_c_enable_242(mclk_c_enable_242), .tckn_stb(tckn_stb), 
            .mclk_c_enable_129(mclk_c_enable_129), .s_en_r0(s_en_r0), .n8781(n8781), 
            .mclk_c_enable_218(mclk_c_enable_218), .\s_desc_data[30] (\s_desc_data[30] ), 
            .s_msbf_r(s_msbf_r), .s_msbf(s_msbf), .mclk_c_enable_206(mclk_c_enable_206), 
            .mclk_c_enable_230(mclk_c_enable_230), .n8765(n8765), .s_tx_shift_stb(s_tx_shift_stb), 
            .n2898(n2898), .mclk_c_enable_112(mclk_c_enable_112), .s_pos_0_3__N_670({s_pos_0_3__N_670}), 
            .n9427(n9427), .\s_desc_data[9] (\s_desc_data[9] ), .\s_desc_data[8] (\s_desc_data[8] ), 
            .\s_desc_data[7] (\s_desc_data[7] ), .mclk_c_enable_203(mclk_c_enable_203), 
            .\s_count_r[0] (s_count_r[0]), .s_count_r_0__N_893(s_count_r_0__N_893), 
            .\bit_pos[0] (bit_pos[0]), .bit_pos_0__N_816(bit_pos_0__N_816), 
            .n8834(n8834), .\s_count_r[1] (s_count_r[1]), .n8822(n8822), 
            .s_count_r_1__N_897(s_count_r_1__N_897), .\bit_pos[1] (bit_pos[1]), 
            .n8835(n8835), .op_msbf(op_msbf), .n8836(n8836), .\s_desc_data[6] (\s_desc_data[6] ), 
            .\s_desc_data[5] (\s_desc_data[5] ), .\s_desc_data[4] (\s_desc_data[4] ), 
            .s_offs_inc(s_offs_inc));   // c:/devel/yard-ice/src/fpga/jtag_enh_shifter.vhdl(288[15:41])
    \jtag_enh_rx(16,10,10,4)  rx (.s_count_r_0__N_893(s_count_r_0__N_893), 
            .bit_pos_0__N_816(bit_pos_0__N_816), .mclk_c(mclk_c), .mclk_c_enable_112(mclk_c_enable_112), 
            .s_pos_0_3__N_670({s_pos_0_3__N_670}), .\s_op_fifo.buf_3_.msbf_N_1174 ({\s_op_fifo.buf_3_.msbf_N_1174 }), 
            .s_rx_mem_wr_stb(s_rx_mem_wr_stb), .\s_desc_data[4] (\s_desc_data[4] ), 
            .\s_desc_data[5] (\s_desc_data[5] ), .\s_desc_data[6] (\s_desc_data[6] ), 
            .\s_desc_data[7] (\s_desc_data[7] ), .\s_desc_data[8] (\s_desc_data[8] ), 
            .\s_desc_data[9] (\s_desc_data[9] ), .op_msbf(op_msbf), .n8834(n8834), 
            .s_count_r({Open_14, Open_15, Open_16, Open_17, Open_18, 
            Open_19, Open_20, Open_21, s_count_r[1], Open_22}), .n8836(n8836), 
            .bit_pos({Open_23, Open_24, bit_pos[1], Open_25}), .mclk_c_enable_242(mclk_c_enable_242), 
            .\s_desc_data[31] (\s_desc_data[31] ), .n9419(n9419), .mclk_c_enable_206(mclk_c_enable_206), 
            .n8822(n8822), .mclk_c_enable_230(mclk_c_enable_230), .mclk_c_enable_218(mclk_c_enable_218), 
            .s_rx_mem_addr({s_rx_mem_addr}), .mclk_c_enable_203(mclk_c_enable_203), 
            .\s_count_r[0] (s_count_r[0]), .\bit_pos[0] (bit_pos[0]), .s_mtdo(s_mtdo), 
            .s_rx_mem_din({s_rx_mem_din}), .\s_desc_data[29] (\s_desc_data[29] ), 
            .GND_net(GND_net), .\s_desc_data[27] (\s_desc_data[27] ), .\s_desc_data[28] (\s_desc_data[28] ), 
            .n8835(n8835), .\s_desc_data[25] (\s_desc_data[25] ), .\s_desc_data[26] (\s_desc_data[26] ), 
            .\s_desc_data[23] (\s_desc_data[23] ), .\s_desc_data[24] (\s_desc_data[24] ), 
            .\s_desc_data[21] (\s_desc_data[21] ), .\s_desc_data[22] (\s_desc_data[22] ), 
            .n9427(n9427), .\s_desc_data[20] (\s_desc_data[20] ), .s_count_r_1__N_897(s_count_r_1__N_897));   // c:/devel/yard-ice/src/fpga/jtag_enh_shifter.vhdl(263[7:25])
    jtag_enh_io io (.mclk_c_enable_104(mclk_c_enable_104), .tckn_stb(tckn_stb), 
            .n9420(n9420), .mclk_c(mclk_c), .mclk_c_enable_81(mclk_c_enable_81), 
            .tp_tms_c(tp_tms_c), .s_mtms(s_mtms), .s_mtdo(s_mtdo), .s_tap_tdo(s_tap_tdo), 
            .s_tap_tdi(s_tap_tdi), .n7643(n7643), .s_so(s_so));   // c:/devel/yard-ice/src/fpga/jtag_enh_shifter.vhdl(216[7:25])
    \jtag_enh_ecu(16,8,8)  exec_ctrl (.mclk_c(mclk_c), .GND_net(GND_net), 
            .s_tcken(s_tcken), .n9420(n9420), .\s_status[2] (\s_status[2] ), 
            .\s_status[3] (\s_status[3] ), .\s_status[0] (\s_status[0] ), 
            .\s_status[1] (\s_status[1] ), .s_end_state({s_end_state}), 
            .s_tdien(s_tdien), .s_reg_din({s_reg_din}), .s_mtms(s_mtms), 
            .n8820(n8820), .n8832(n8832), .s_en_r0(s_en_r0), .n8781(n8781), 
            .n8765(n8765), .\s_offs_r[1] (s_offs_r[1]), .n2898(n2898), 
            .s_offs_inc(s_offs_inc), .\s_offs_r[0] (s_offs_r[0]), .n2873(n2873), 
            .tckn_stb(tckn_stb), .s_en_r1(s_en_r1), .mclk_c_enable_129(mclk_c_enable_129), 
            .s_tap_busy(s_tap_busy), .n9428(n9428), .n8774(n8774), .n8829(n8829), 
            .n8828(n8828), .n8856(n8856), .out_get_N_312(out_get_N_312), 
            .n7771(n7771), .n2(n2), .n5312(n5312), .s_ptr_addr({s_ptr_addr}), 
            .s_cnt_en_N_1514(s_cnt_en_N_1514), .s_goto_state({Open_26, \s_goto_state[2] , 
            \s_goto_state[1] , \s_goto_state[0] }), .n1(n1), .n7781(n7781));   // c:/devel/yard-ice/src/fpga/jtag_enh_shifter.vhdl(313[14:33])
    \jtag_enh_ckgen(16)  ckgen (.mclk_c(mclk_c), .s_ckgen_div({s_ckgen_div}), 
            .GND_net(GND_net), .\s_irq[1] (\s_irq[1] ), .s_ckgen_rtdiv({s_ckgen_rtdiv}), 
            .s_tcken(s_tcken), .\s_cfg[5] (\s_cfg[5] ), .tp_rtck_c(tp_rtck_c), 
            .tp_tck_c(tp_tck_c), .\s_irq_en[1] (\s_irq_en[1] ), .\irq_N_8[1] (\irq_N_8[1] ), 
            .mclk_c_enable_104(mclk_c_enable_104), .s_tdien(s_tdien), .n7643(n7643));   // c:/devel/yard-ice/src/fpga/jtag_enh_shifter.vhdl(195[10:31])
    
endmodule
//
// Verilog Description of module \jtag_enh_tx(16,10,10,4) 
//

module \jtag_enh_tx(16,10,10,4)  (n9427, n2993, n3009, n3010, n2992, 
            n3008, n2991, n3007, n2990, n3006, n2989, n3005, n2988, 
            n3004, n2987, n3003, n2986, n3002, n2985, n3001, n2984, 
            n3000, n2983, n2999, n2982, n2998, n2981, n2997, n2980, 
            n2996, n2979, n2995, n2978, n2994, s_msbf, GND_net, 
            \s_desc_data[18] , \s_desc_data[19] , s_addr_0, \s_desc_data[16] , 
            \s_desc_data[17] , mclk_c, s_msbf_r, \s_desc_data[14] , 
            \s_desc_data[15] , \s_desc_data[12] , \s_desc_data[13] , \s_desc_data[10] , 
            \s_desc_data[11] , s_en_r1, n8771, s_tx_shift_stb, \s_desc_data[8] , 
            \s_desc_data[30] , \s_desc_data[9] , \s_desc_data[6] , \s_desc_data[7] , 
            \s_desc_data[4] , \s_desc_data[5] , s_so, s_pos_0_3__N_670);
    input n9427;
    input n2993;
    input n3009;
    input n3010;
    input n2992;
    input n3008;
    input n2991;
    input n3007;
    input n2990;
    input n3006;
    input n2989;
    input n3005;
    input n2988;
    input n3004;
    input n2987;
    input n3003;
    input n2986;
    input n3002;
    input n2985;
    input n3001;
    input n2984;
    input n3000;
    input n2983;
    input n2999;
    input n2982;
    input n2998;
    input n2981;
    input n2997;
    input n2980;
    input n2996;
    input n2979;
    input n2995;
    input n2978;
    input n2994;
    input s_msbf;
    input GND_net;
    input \s_desc_data[18] ;
    input \s_desc_data[19] ;
    output [9:0]s_addr_0;
    input \s_desc_data[16] ;
    input \s_desc_data[17] ;
    input mclk_c;
    output s_msbf_r;
    input \s_desc_data[14] ;
    input \s_desc_data[15] ;
    input \s_desc_data[12] ;
    input \s_desc_data[13] ;
    input \s_desc_data[10] ;
    input \s_desc_data[11] ;
    input s_en_r1;
    input n8771;
    input s_tx_shift_stb;
    input \s_desc_data[8] ;
    input \s_desc_data[30] ;
    input \s_desc_data[9] ;
    input \s_desc_data[6] ;
    input \s_desc_data[7] ;
    input \s_desc_data[4] ;
    input \s_desc_data[5] ;
    output s_so;
    input [3:0]s_pos_0_3__N_670;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    wire s_pos_3__N_687;
    wire [3:0]n1;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(75[9:16])
    wire [3:0]n2426;
    
    wire n7976, n7975, n7974, n7973, n7972, n7971, n7970, n7969, 
        n7968, n7967, n7966, n7965, n7964, n7963, n7962, n7961, 
        n8749;
    wire [9:0]s_addr_r;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(82[9:17])
    wire [9:0]s_sum;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(83[9:14])
    
    wire n8803, n8756, n8752, n7052, n7051, n8743;
    wire [3:0]s_pos;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(79[9:14])
    
    wire n8744, n8742, n8844, n8815, n7050;
    wire [5:0]n2616;
    
    wire n7049, n7048, n8826, n7985, n7986, n7989, n7987, n7988, 
        n7990, n8745, n8746, n7977, n7978, n7979, n7980, n7981, 
        n7982, n7983, n7984;
    
    PFUMX mux_510_i4 (.BLUT(s_pos_3__N_687), .ALUT(n1[3]), .C0(n9427), 
          .Z(n2426[3]));
    LUT4 i5919_3_lut (.A(n2993), .B(n3009), .C(n3010), .Z(n7976)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5919_3_lut.init = 16'hcaca;
    LUT4 i5918_3_lut (.A(n2992), .B(n3008), .C(n3010), .Z(n7975)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5918_3_lut.init = 16'hcaca;
    LUT4 i5917_3_lut (.A(n2991), .B(n3007), .C(n3010), .Z(n7974)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5917_3_lut.init = 16'hcaca;
    LUT4 i5916_3_lut (.A(n2990), .B(n3006), .C(n3010), .Z(n7973)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5916_3_lut.init = 16'hcaca;
    LUT4 i5915_3_lut (.A(n2989), .B(n3005), .C(n3010), .Z(n7972)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5915_3_lut.init = 16'hcaca;
    LUT4 i5914_3_lut (.A(n2988), .B(n3004), .C(n3010), .Z(n7971)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5914_3_lut.init = 16'hcaca;
    LUT4 i5913_3_lut (.A(n2987), .B(n3003), .C(n3010), .Z(n7970)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5913_3_lut.init = 16'hcaca;
    LUT4 i5912_3_lut (.A(n2986), .B(n3002), .C(n3010), .Z(n7969)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5912_3_lut.init = 16'hcaca;
    LUT4 i5911_3_lut (.A(n2985), .B(n3001), .C(n3010), .Z(n7968)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5911_3_lut.init = 16'hcaca;
    LUT4 i5910_3_lut (.A(n2984), .B(n3000), .C(n3010), .Z(n7967)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5910_3_lut.init = 16'hcaca;
    LUT4 i5909_3_lut (.A(n2983), .B(n2999), .C(n3010), .Z(n7966)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5909_3_lut.init = 16'hcaca;
    LUT4 i5908_3_lut (.A(n2982), .B(n2998), .C(n3010), .Z(n7965)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5908_3_lut.init = 16'hcaca;
    LUT4 i5907_3_lut (.A(n2981), .B(n2997), .C(n3010), .Z(n7964)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5907_3_lut.init = 16'hcaca;
    LUT4 i5906_3_lut (.A(n2980), .B(n2996), .C(n3010), .Z(n7963)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5906_3_lut.init = 16'hcaca;
    LUT4 i5905_3_lut (.A(n2979), .B(n2995), .C(n3010), .Z(n7962)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5905_3_lut.init = 16'hcaca;
    LUT4 i5904_3_lut (.A(n2978), .B(n2994), .C(n3010), .Z(n7961)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5904_3_lut.init = 16'hcaca;
    LUT4 s_addr_r_9__I_0_i5_2_lut_4_lut (.A(s_msbf), .B(n8749), .C(s_addr_r[3]), 
         .D(s_addr_r[4]), .Z(s_sum[4])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i5_2_lut_4_lut.init = 16'hb748;
    LUT4 s_sum_9__I_73_3_lut_rep_89 (.A(s_msbf), .B(n8803), .C(s_addr_r[0]), 
         .Z(n8756)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B (C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_73_3_lut_rep_89.init = 16'h4848;
    LUT4 s_sum_9__I_72_3_lut_rep_85_4_lut (.A(s_msbf), .B(n8803), .C(s_addr_r[0]), 
         .D(s_addr_r[1]), .Z(n8752)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_72_3_lut_rep_85_4_lut.init = 16'h4008;
    LUT4 s_addr_r_9__I_0_i2_2_lut_4_lut (.A(s_msbf), .B(n8803), .C(s_addr_r[0]), 
         .D(s_addr_r[1]), .Z(s_sum[1])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i2_2_lut_4_lut.init = 16'hb748;
    CCU2D add_517_11 (.A0(GND_net), .B0(n9427), .C0(s_sum[8]), .D0(\s_desc_data[18] ), 
          .A1(GND_net), .B1(n9427), .C1(s_sum[9]), .D1(\s_desc_data[19] ), 
          .CIN(n7052), .S0(s_addr_0[8]), .S1(s_addr_0[9]));   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam add_517_11.INIT0 = 16'h569a;
    defparam add_517_11.INIT1 = 16'h569a;
    defparam add_517_11.INJECT1_0 = "NO";
    defparam add_517_11.INJECT1_1 = "NO";
    CCU2D add_517_9 (.A0(GND_net), .B0(n9427), .C0(s_sum[6]), .D0(\s_desc_data[16] ), 
          .A1(GND_net), .B1(n9427), .C1(s_sum[7]), .D1(\s_desc_data[17] ), 
          .CIN(n7051), .COUT(n7052), .S0(s_addr_0[6]), .S1(s_addr_0[7]));   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam add_517_9.INIT0 = 16'h569a;
    defparam add_517_9.INIT1 = 16'h569a;
    defparam add_517_9.INJECT1_0 = "NO";
    defparam add_517_9.INJECT1_1 = "NO";
    LUT4 s_addr_r_9__I_0_i9_2_lut_4_lut (.A(s_msbf), .B(n8743), .C(s_addr_r[7]), 
         .D(s_addr_r[8]), .Z(s_sum[8])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i9_2_lut_4_lut.init = 16'hb748;
    FD1S3AX s_pos_0__91 (.D(n2426[0]), .CK(mclk_c), .Q(s_pos[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(126[4] 136[11])
    defparam s_pos_0__91.GSR = "ENABLED";
    FD1S3AX s_pos_1__92 (.D(n2426[1]), .CK(mclk_c), .Q(s_pos[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(126[4] 136[11])
    defparam s_pos_1__92.GSR = "ENABLED";
    LUT4 s_sum_9__I_71_3_lut_rep_82_4_lut (.A(s_msbf), .B(n8756), .C(s_addr_r[1]), 
         .D(s_addr_r[2]), .Z(n8749)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_71_3_lut_rep_82_4_lut.init = 16'h4008;
    FD1S3AX s_pos_2__93 (.D(n2426[2]), .CK(mclk_c), .Q(s_pos[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(126[4] 136[11])
    defparam s_pos_2__93.GSR = "ENABLED";
    LUT4 s_sum_9__I_66_3_lut_rep_75_4_lut (.A(s_msbf), .B(n8744), .C(s_addr_r[6]), 
         .D(s_addr_r[7]), .Z(n8742)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_66_3_lut_rep_75_4_lut.init = 16'h4008;
    FD1S3AX s_pos_3__94 (.D(n2426[3]), .CK(mclk_c), .Q(s_pos[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(126[4] 136[11])
    defparam s_pos_3__94.GSR = "ENABLED";
    FD1S3AX s_addr_r_i0 (.D(s_addr_0[0]), .CK(mclk_c), .Q(s_addr_r[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(161[3] 165[10])
    defparam s_addr_r_i0.GSR = "ENABLED";
    LUT4 s_addr_r_9__I_0_i3_2_lut_4_lut (.A(s_msbf), .B(n8756), .C(s_addr_r[1]), 
         .D(s_addr_r[2]), .Z(s_sum[2])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i3_2_lut_4_lut.init = 16'hb748;
    FD1S3AX s_msbf_r_90 (.D(s_msbf), .CK(mclk_c), .Q(s_msbf_r)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(107[3] 111[10])
    defparam s_msbf_r_90.GSR = "ENABLED";
    LUT4 s_pos_3__I_62_3_lut_rep_148_4_lut (.A(s_msbf_r), .B(n8844), .C(s_pos[1]), 
         .D(s_pos[2]), .Z(n8815)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(137[10:40])
    defparam s_pos_3__I_62_3_lut_rep_148_4_lut.init = 16'h4008;
    CCU2D add_517_7 (.A0(n2616[4]), .B0(n9427), .C0(s_sum[4]), .D0(\s_desc_data[14] ), 
          .A1(n2616[5]), .B1(n9427), .C1(s_sum[5]), .D1(\s_desc_data[15] ), 
          .CIN(n7050), .COUT(n7051), .S0(s_addr_0[4]), .S1(s_addr_0[5]));   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam add_517_7.INIT0 = 16'h74b8;
    defparam add_517_7.INIT1 = 16'h74b8;
    defparam add_517_7.INJECT1_0 = "NO";
    defparam add_517_7.INJECT1_1 = "NO";
    CCU2D add_517_5 (.A0(n2616[2]), .B0(n9427), .C0(s_sum[2]), .D0(\s_desc_data[12] ), 
          .A1(n2616[3]), .B1(n9427), .C1(s_sum[3]), .D1(\s_desc_data[13] ), 
          .CIN(n7049), .COUT(n7050), .S0(s_addr_0[2]), .S1(s_addr_0[3]));   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam add_517_5.INIT0 = 16'h74b8;
    defparam add_517_5.INIT1 = 16'h74b8;
    defparam add_517_5.INJECT1_0 = "NO";
    defparam add_517_5.INJECT1_1 = "NO";
    CCU2D add_517_3 (.A0(n2616[0]), .B0(n9427), .C0(s_sum[0]), .D0(\s_desc_data[10] ), 
          .A1(n2616[1]), .B1(n9427), .C1(s_sum[1]), .D1(\s_desc_data[11] ), 
          .CIN(n7048), .COUT(n7049), .S0(s_addr_0[0]), .S1(s_addr_0[1]));   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam add_517_3.INIT0 = 16'h74b8;
    defparam add_517_3.INIT1 = 16'h74b8;
    defparam add_517_3.INJECT1_0 = "NO";
    defparam add_517_3.INJECT1_1 = "NO";
    LUT4 s_pos_2__bdd_4_lut (.A(s_pos[2]), .B(n8826), .C(n1[2]), .D(n9427), 
         .Z(n2426[2])) /* synthesis lut_function=(A (B (C (D))+!B (C+!(D)))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam s_pos_2__bdd_4_lut.init = 16'hf066;
    LUT4 s_pos_1__bdd_4_lut (.A(s_pos[1]), .B(n8844), .C(n1[1]), .D(n9427), 
         .Z(n2426[1])) /* synthesis lut_function=(A (B (C (D))+!B (C+!(D)))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam s_pos_1__bdd_4_lut.init = 16'hf066;
    L6MUX21 i5932 (.D0(n7985), .D1(n7986), .SD(s_pos[2]), .Z(n7989));
    L6MUX21 i5933 (.D0(n7987), .D1(n7988), .SD(s_pos[2]), .Z(n7990));
    CCU2D add_517_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_en_r1), .B1(n8771), .C1(GND_net), .D1(GND_net), .COUT(n7048));   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam add_517_1.INIT0 = 16'hF000;
    defparam add_517_1.INIT1 = 16'hffff;
    defparam add_517_1.INJECT1_0 = "NO";
    defparam add_517_1.INJECT1_1 = "NO";
    LUT4 s_pos_0__bdd_4_lut (.A(s_pos[0]), .B(s_tx_shift_stb), .C(n1[0]), 
         .D(n9427), .Z(n2426[0])) /* synthesis lut_function=(A (B (C (D))+!B (C+!(D)))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam s_pos_0__bdd_4_lut.init = 16'hf066;
    LUT4 s_pos_3__I_64_3_lut_rep_177 (.A(s_msbf_r), .B(s_tx_shift_stb), 
         .C(s_pos[0]), .Z(n8844)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B (C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(137[10:40])
    defparam s_pos_3__I_64_3_lut_rep_177.init = 16'h4848;
    LUT4 s_pos_3__I_63_3_lut_rep_159_4_lut (.A(s_msbf_r), .B(s_tx_shift_stb), 
         .C(s_pos[0]), .D(s_pos[1]), .Z(n8826)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(137[10:40])
    defparam s_pos_3__I_63_3_lut_rep_159_4_lut.init = 16'h4008;
    LUT4 s_sum_9__I_67_3_lut_rep_76_4_lut (.A(s_msbf), .B(n8745), .C(s_addr_r[5]), 
         .D(s_addr_r[6]), .Z(n8743)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_67_3_lut_rep_76_4_lut.init = 16'h4008;
    LUT4 s_addr_r_9__I_0_i7_2_lut_4_lut (.A(s_msbf), .B(n8745), .C(s_addr_r[5]), 
         .D(s_addr_r[6]), .Z(s_sum[6])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i7_2_lut_4_lut.init = 16'hb748;
    FD1S3AX s_addr_r_i1 (.D(s_addr_0[1]), .CK(mclk_c), .Q(s_addr_r[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(161[3] 165[10])
    defparam s_addr_r_i1.GSR = "ENABLED";
    FD1S3AX s_addr_r_i2 (.D(s_addr_0[2]), .CK(mclk_c), .Q(s_addr_r[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(161[3] 165[10])
    defparam s_addr_r_i2.GSR = "ENABLED";
    FD1S3AX s_addr_r_i3 (.D(s_addr_0[3]), .CK(mclk_c), .Q(s_addr_r[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(161[3] 165[10])
    defparam s_addr_r_i3.GSR = "ENABLED";
    FD1S3AX s_addr_r_i4 (.D(s_addr_0[4]), .CK(mclk_c), .Q(s_addr_r[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(161[3] 165[10])
    defparam s_addr_r_i4.GSR = "ENABLED";
    FD1S3AX s_addr_r_i5 (.D(s_addr_0[5]), .CK(mclk_c), .Q(s_addr_r[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(161[3] 165[10])
    defparam s_addr_r_i5.GSR = "ENABLED";
    FD1S3AX s_addr_r_i6 (.D(s_addr_0[6]), .CK(mclk_c), .Q(s_addr_r[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(161[3] 165[10])
    defparam s_addr_r_i6.GSR = "ENABLED";
    FD1S3AX s_addr_r_i7 (.D(s_addr_0[7]), .CK(mclk_c), .Q(s_addr_r[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(161[3] 165[10])
    defparam s_addr_r_i7.GSR = "ENABLED";
    FD1S3AX s_addr_r_i8 (.D(s_addr_0[8]), .CK(mclk_c), .Q(s_addr_r[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(161[3] 165[10])
    defparam s_addr_r_i8.GSR = "ENABLED";
    FD1S3AX s_addr_r_i9 (.D(s_addr_0[9]), .CK(mclk_c), .Q(s_addr_r[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(161[3] 165[10])
    defparam s_addr_r_i9.GSR = "ENABLED";
    LUT4 s_addr_r_9__I_0_i10_4_lut (.A(s_addr_r[9]), .B(s_msbf), .C(n8742), 
         .D(s_addr_r[8]), .Z(s_sum[9])) /* synthesis lut_function=(A (B ((D)+!C)+!B !(C (D)))+!A !(B ((D)+!C)+!B !(C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(149[16:34])
    defparam s_addr_r_9__I_0_i10_4_lut.init = 16'h9a6a;
    LUT4 s_sum_9__I_68_3_lut_rep_77_4_lut (.A(s_msbf), .B(n8746), .C(s_addr_r[4]), 
         .D(s_addr_r[5]), .Z(n8744)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_68_3_lut_rep_77_4_lut.init = 16'h4008;
    LUT4 s_addr_r_9__I_0_i8_2_lut_4_lut (.A(s_msbf), .B(n8744), .C(s_addr_r[6]), 
         .D(s_addr_r[7]), .Z(s_sum[7])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i8_2_lut_4_lut.init = 16'hb748;
    LUT4 s_addr_r_9__I_0_i6_2_lut_4_lut (.A(s_msbf), .B(n8746), .C(s_addr_r[4]), 
         .D(s_addr_r[5]), .Z(s_sum[5])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i6_2_lut_4_lut.init = 16'hb748;
    L6MUX21 i5928 (.D0(n7977), .D1(n7978), .SD(s_pos[1]), .Z(n7985));
    L6MUX21 i5929 (.D0(n7979), .D1(n7980), .SD(s_pos[1]), .Z(n7986));
    L6MUX21 i5930 (.D0(n7981), .D1(n7982), .SD(s_pos[1]), .Z(n7987));
    L6MUX21 i5931 (.D0(n7983), .D1(n7984), .SD(s_pos[1]), .Z(n7988));
    LUT4 i3367_2_lut (.A(\s_desc_data[8] ), .B(\s_desc_data[30] ), .Z(n2616[4])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam i3367_2_lut.init = 16'h8888;
    LUT4 i3368_2_lut (.A(\s_desc_data[9] ), .B(\s_desc_data[30] ), .Z(n2616[5])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam i3368_2_lut.init = 16'h8888;
    LUT4 i3365_2_lut (.A(\s_desc_data[6] ), .B(\s_desc_data[30] ), .Z(n2616[2])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam i3365_2_lut.init = 16'h8888;
    LUT4 i3366_2_lut (.A(\s_desc_data[7] ), .B(\s_desc_data[30] ), .Z(n2616[3])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam i3366_2_lut.init = 16'h8888;
    LUT4 i3244_2_lut (.A(\s_desc_data[4] ), .B(\s_desc_data[30] ), .Z(n2616[0])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam i3244_2_lut.init = 16'h8888;
    LUT4 i3364_2_lut (.A(\s_desc_data[5] ), .B(\s_desc_data[30] ), .Z(n2616[1])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam i3364_2_lut.init = 16'h8888;
    LUT4 s_addr_r_9__I_0_i1_2_lut_4_lut (.A(s_msbf_r), .B(n8815), .C(s_pos[3]), 
         .D(s_addr_r[0]), .Z(s_sum[0])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(137[10:40])
    defparam s_addr_r_9__I_0_i1_2_lut_4_lut.init = 16'hb748;
    LUT4 i5934_3_lut (.A(n7989), .B(n7990), .C(s_pos[3]), .Z(s_so)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5934_3_lut.init = 16'hcaca;
    LUT4 i3230_2_lut (.A(s_pos_0_3__N_670[0]), .B(\s_desc_data[30] ), .Z(n1[0])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(96[13] 97[63])
    defparam i3230_2_lut.init = 16'h8888;
    LUT4 i3358_2_lut (.A(s_pos_0_3__N_670[1]), .B(\s_desc_data[30] ), .Z(n1[1])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(96[13] 97[63])
    defparam i3358_2_lut.init = 16'h8888;
    LUT4 i3359_2_lut (.A(s_pos_0_3__N_670[2]), .B(\s_desc_data[30] ), .Z(n1[2])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(96[13] 97[63])
    defparam i3359_2_lut.init = 16'h8888;
    LUT4 s_sum_9__I_70_3_lut_rep_79_4_lut (.A(s_msbf), .B(n8752), .C(s_addr_r[2]), 
         .D(s_addr_r[3]), .Z(n8746)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_70_3_lut_rep_79_4_lut.init = 16'h4008;
    LUT4 i3360_2_lut (.A(s_pos_0_3__N_670[3]), .B(\s_desc_data[30] ), .Z(n1[3])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(96[13] 97[63])
    defparam i3360_2_lut.init = 16'h8888;
    LUT4 s_pos_wrap_I_0_3_lut_rep_136_4_lut (.A(s_msbf_r), .B(n8826), .C(s_pos[2]), 
         .D(s_pos[3]), .Z(n8803)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(137[10:40])
    defparam s_pos_wrap_I_0_3_lut_rep_136_4_lut.init = 16'h4008;
    LUT4 s_pos_3__I_0_112_2_lut_4_lut (.A(s_msbf_r), .B(n8826), .C(s_pos[2]), 
         .D(s_pos[3]), .Z(s_pos_3__N_687)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(137[10:40])
    defparam s_pos_3__I_0_112_2_lut_4_lut.init = 16'hb748;
    PFUMX i5920 (.BLUT(n7961), .ALUT(n7962), .C0(s_pos[0]), .Z(n7977));
    PFUMX i5921 (.BLUT(n7963), .ALUT(n7964), .C0(s_pos[0]), .Z(n7978));
    LUT4 s_addr_r_9__I_0_i4_2_lut_4_lut (.A(s_msbf), .B(n8752), .C(s_addr_r[2]), 
         .D(s_addr_r[3]), .Z(s_sum[3])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i4_2_lut_4_lut.init = 16'hb748;
    PFUMX i5922 (.BLUT(n7965), .ALUT(n7966), .C0(s_pos[0]), .Z(n7979));
    LUT4 s_sum_9__I_69_3_lut_rep_78_4_lut (.A(s_msbf), .B(n8749), .C(s_addr_r[3]), 
         .D(s_addr_r[4]), .Z(n8745)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_69_3_lut_rep_78_4_lut.init = 16'h4008;
    PFUMX i5923 (.BLUT(n7967), .ALUT(n7968), .C0(s_pos[0]), .Z(n7980));
    PFUMX i5924 (.BLUT(n7969), .ALUT(n7970), .C0(s_pos[0]), .Z(n7981));
    PFUMX i5925 (.BLUT(n7971), .ALUT(n7972), .C0(s_pos[0]), .Z(n7982));
    PFUMX i5926 (.BLUT(n7973), .ALUT(n7974), .C0(s_pos[0]), .Z(n7983));
    PFUMX i5927 (.BLUT(n7975), .ALUT(n7976), .C0(s_pos[0]), .Z(n7984));
    
endmodule
//
// Verilog Description of module \jtag_enh_shift_ctrl(10,8) 
//

module \jtag_enh_shift_ctrl(10,8)  (s_offs_r, mclk_c, n2873, s_ptr_data, 
            GND_net, s_desc_addr, \s_offs_r[1] , n8774, n8771, s_en_r1, 
            \s_op_fifo.buf_3_.msbf_N_1174 , n9428, mclk_c_enable_242, 
            tckn_stb, mclk_c_enable_129, s_en_r0, n8781, mclk_c_enable_218, 
            \s_desc_data[30] , s_msbf_r, s_msbf, mclk_c_enable_206, 
            mclk_c_enable_230, n8765, s_tx_shift_stb, n2898, mclk_c_enable_112, 
            s_pos_0_3__N_670, n9427, \s_desc_data[9] , \s_desc_data[8] , 
            \s_desc_data[7] , mclk_c_enable_203, \s_count_r[0] , s_count_r_0__N_893, 
            \bit_pos[0] , bit_pos_0__N_816, n8834, \s_count_r[1] , n8822, 
            s_count_r_1__N_897, \bit_pos[1] , n8835, op_msbf, n8836, 
            \s_desc_data[6] , \s_desc_data[5] , \s_desc_data[4] , s_offs_inc);
    output [7:0]s_offs_r;
    input mclk_c;
    input n2873;
    input [15:0]s_ptr_data;
    input GND_net;
    output [8:1]s_desc_addr;
    output \s_offs_r[1] ;
    input n8774;
    output n8771;
    output s_en_r1;
    input [3:0]\s_op_fifo.buf_3_.msbf_N_1174 ;
    input n9428;
    output mclk_c_enable_242;
    input tckn_stb;
    input mclk_c_enable_129;
    output s_en_r0;
    input n8781;
    output mclk_c_enable_218;
    input \s_desc_data[30] ;
    input s_msbf_r;
    output s_msbf;
    output mclk_c_enable_206;
    output mclk_c_enable_230;
    output n8765;
    output s_tx_shift_stb;
    input n2898;
    output mclk_c_enable_112;
    input [3:0]s_pos_0_3__N_670;
    output n9427;
    input \s_desc_data[9] ;
    input \s_desc_data[8] ;
    input \s_desc_data[7] ;
    output mclk_c_enable_203;
    input \s_count_r[0] ;
    output s_count_r_0__N_893;
    input \bit_pos[0] ;
    output bit_pos_0__N_816;
    output n8834;
    input \s_count_r[1] ;
    output n8822;
    output s_count_r_1__N_897;
    input \bit_pos[1] ;
    output n8835;
    input op_msbf;
    output n8836;
    input \s_desc_data[6] ;
    input \s_desc_data[5] ;
    input \s_desc_data[4] ;
    output s_offs_inc;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    wire n7061;
    wire [7:0]s_offs_r_c;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(80[9:17])
    
    wire n7060, n7059, n7057;
    wire [7:0]s_stop_N_1313;
    
    wire n7056, n7055, n7054, n8757, n2885, n2877;
    wire [9:0]bit_cnt;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(68[2:9])
    
    wire bit_cnt_0__N_1203, n8761, n8750, s_en_r2, n2875, bit_cnt_9__N_1189, 
        bit_cnt_8__N_1250, bit_cnt_7__N_1244, mclk_c_enable_41, n4460, 
        n8841, n8806, n2889, n8777, n8753, n2881, s_rx_en_r1, 
        bit_cnt_6__N_1238, bit_cnt_5__N_1232, bit_cnt_4__N_1226, bit_cnt_3__N_1220, 
        bit_cnt_2__N_1214, n2896, n8784, s_stop_N_1312, n8866, n8814, 
        bit_cnt_1__N_1208, n8790, s_cnt_stb, n8747, n8795, n8825, 
        n6960, n6959, n13, n14;
    
    FD1S3AY s_offs_r_0__225 (.D(n2873), .CK(mclk_c), .Q(s_offs_r[0]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_0__225.GSR = "ENABLED";
    CCU2D desc_base_7__I_0_8 (.A0(s_ptr_data[14]), .B0(s_offs_r_c[6]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ptr_data[15]), .B1(s_offs_r_c[7]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7061), .S0(s_desc_addr[7]), .S1(s_desc_addr[8]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(202[32:51])
    defparam desc_base_7__I_0_8.INIT0 = 16'h5666;
    defparam desc_base_7__I_0_8.INIT1 = 16'h5666;
    defparam desc_base_7__I_0_8.INJECT1_0 = "NO";
    defparam desc_base_7__I_0_8.INJECT1_1 = "NO";
    CCU2D desc_base_7__I_0_6 (.A0(s_ptr_data[12]), .B0(s_offs_r_c[4]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ptr_data[13]), .B1(s_offs_r_c[5]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7060), .COUT(n7061), .S0(s_desc_addr[5]), 
          .S1(s_desc_addr[6]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(202[32:51])
    defparam desc_base_7__I_0_6.INIT0 = 16'h5666;
    defparam desc_base_7__I_0_6.INIT1 = 16'h5666;
    defparam desc_base_7__I_0_6.INJECT1_0 = "NO";
    defparam desc_base_7__I_0_6.INJECT1_1 = "NO";
    CCU2D desc_base_7__I_0_4 (.A0(s_ptr_data[10]), .B0(s_offs_r_c[2]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ptr_data[11]), .B1(s_offs_r_c[3]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7059), .COUT(n7060), .S0(s_desc_addr[3]), 
          .S1(s_desc_addr[4]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(202[32:51])
    defparam desc_base_7__I_0_4.INIT0 = 16'h5666;
    defparam desc_base_7__I_0_4.INIT1 = 16'h5666;
    defparam desc_base_7__I_0_4.INJECT1_0 = "NO";
    defparam desc_base_7__I_0_4.INJECT1_1 = "NO";
    CCU2D desc_base_7__I_0_2 (.A0(s_ptr_data[8]), .B0(s_offs_r[0]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ptr_data[9]), .B1(\s_offs_r[1] ), .C1(GND_net), 
          .D1(GND_net), .COUT(n7059), .S1(s_desc_addr[2]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(202[32:51])
    defparam desc_base_7__I_0_2.INIT0 = 16'h7000;
    defparam desc_base_7__I_0_2.INIT1 = 16'h5666;
    defparam desc_base_7__I_0_2.INJECT1_0 = "NO";
    defparam desc_base_7__I_0_2.INJECT1_1 = "NO";
    CCU2D add_191_9 (.A0(s_ptr_data[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n7057), 
          .S0(s_stop_N_1313[7]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_191_9.INIT0 = 16'h5aaa;
    defparam add_191_9.INIT1 = 16'h0000;
    defparam add_191_9.INJECT1_0 = "NO";
    defparam add_191_9.INJECT1_1 = "NO";
    CCU2D add_191_7 (.A0(s_ptr_data[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_ptr_data[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n7056), .COUT(n7057), .S0(s_stop_N_1313[5]), .S1(s_stop_N_1313[6]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_191_7.INIT0 = 16'h5aaa;
    defparam add_191_7.INIT1 = 16'h5aaa;
    defparam add_191_7.INJECT1_0 = "NO";
    defparam add_191_7.INJECT1_1 = "NO";
    CCU2D add_191_5 (.A0(s_ptr_data[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_ptr_data[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n7055), .COUT(n7056), .S0(s_stop_N_1313[3]), .S1(s_stop_N_1313[4]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_191_5.INIT0 = 16'h5aaa;
    defparam add_191_5.INIT1 = 16'h5aaa;
    defparam add_191_5.INJECT1_0 = "NO";
    defparam add_191_5.INJECT1_1 = "NO";
    CCU2D add_191_3 (.A0(s_ptr_data[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_ptr_data[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n7054), .COUT(n7055), .S0(s_stop_N_1313[1]), .S1(s_stop_N_1313[2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_191_3.INIT0 = 16'h5aaa;
    defparam add_191_3.INIT1 = 16'h5aaa;
    defparam add_191_3.INJECT1_0 = "NO";
    defparam add_191_3.INJECT1_1 = "NO";
    CCU2D add_191_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_ptr_data[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n7054), .S1(s_stop_N_1313[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_191_1.INIT0 = 16'hF000;
    defparam add_191_1.INIT1 = 16'h5555;
    defparam add_191_1.INJECT1_0 = "NO";
    defparam add_191_1.INJECT1_1 = "NO";
    LUT4 i3231_3_lut_4_lut (.A(n8757), .B(s_offs_r_c[3]), .C(n8774), .D(s_offs_r_c[4]), 
         .Z(n2885)) /* synthesis lut_function=(!(A (B (C+(D))+!B (C+!(D)))+!A (C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam i3231_3_lut_4_lut.init = 16'h0708;
    FD1S3AY s_offs_r_6__231 (.D(n2877), .CK(mclk_c), .Q(s_offs_r_c[6]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_6__231.GSR = "ENABLED";
    FD1S3IX s_cnt_r_0__214 (.D(bit_cnt_0__N_1203), .CK(mclk_c), .CD(n8774), 
            .Q(bit_cnt[0]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_0__214.GSR = "ENABLED";
    LUT4 s_offs_r_7__I_136_2_lut_rep_83_3_lut_4_lut (.A(n8761), .B(s_offs_r_c[2]), 
         .C(s_offs_r_c[4]), .D(s_offs_r_c[3]), .Z(n8750)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam s_offs_r_7__I_136_2_lut_rep_83_3_lut_4_lut.init = 16'h8000;
    LUT4 s_offs_r_7__I_139_2_lut_rep_94_4_lut (.A(s_offs_r[0]), .B(n8771), 
         .C(s_en_r2), .D(\s_offs_r[1] ), .Z(n8761)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam s_offs_r_7__I_139_2_lut_rep_94_4_lut.init = 16'h8000;
    LUT4 i1_2_lut_3_lut_4_lut (.A(n8771), .B(s_en_r1), .C(\s_op_fifo.buf_3_.msbf_N_1174 [0]), 
         .D(n9428), .Z(mclk_c_enable_242)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(121[15:49])
    defparam i1_2_lut_3_lut_4_lut.init = 16'h0080;
    FD1P3AX s_en_r2_213 (.D(s_en_r1), .SP(tckn_stb), .CK(mclk_c), .Q(s_en_r2));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(98[3] 112[10])
    defparam s_en_r2_213.GSR = "ENABLED";
    FD1S3AY s_offs_r_7__232 (.D(n2875), .CK(mclk_c), .Q(s_offs_r_c[7]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_7__232.GSR = "ENABLED";
    FD1P3IX s_cnt_r_9__223 (.D(bit_cnt_9__N_1189), .SP(mclk_c_enable_129), 
            .CD(n8774), .CK(mclk_c), .Q(bit_cnt[9]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_9__223.GSR = "ENABLED";
    FD1P3IX s_cnt_r_8__222 (.D(bit_cnt_8__N_1250), .SP(mclk_c_enable_129), 
            .CD(n8774), .CK(mclk_c), .Q(bit_cnt[8]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_8__222.GSR = "ENABLED";
    FD1P3IX s_cnt_r_7__221 (.D(bit_cnt_7__N_1244), .SP(mclk_c_enable_129), 
            .CD(n8774), .CK(mclk_c), .Q(bit_cnt[7]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_7__221.GSR = "ENABLED";
    FD1P3IX s_en_r0_211 (.D(n8781), .SP(mclk_c_enable_41), .CD(n4460), 
            .CK(mclk_c), .Q(s_en_r0)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=15, LSE_RCOL=41, LSE_LLINE=288, LSE_RLINE=288 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(98[3] 112[10])
    defparam s_en_r0_211.GSR = "ENABLED";
    LUT4 i1_2_lut_3_lut_4_lut_adj_197 (.A(n8771), .B(s_en_r1), .C(\s_op_fifo.buf_3_.msbf_N_1174 [2]), 
         .D(n9428), .Z(mclk_c_enable_218)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(121[15:49])
    defparam i1_2_lut_3_lut_4_lut_adj_197.init = 16'h0080;
    LUT4 s_msbf_r_I_0_3_lut_4_lut (.A(n8771), .B(s_en_r1), .C(\s_desc_data[30] ), 
         .D(s_msbf_r), .Z(s_msbf)) /* synthesis lut_function=(A (B (C)+!B (D))+!A (D)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(121[15:49])
    defparam s_msbf_r_I_0_3_lut_4_lut.init = 16'hf780;
    LUT4 i1_2_lut_3_lut_4_lut_adj_198 (.A(n8771), .B(s_en_r1), .C(\s_op_fifo.buf_3_.msbf_N_1174 [3]), 
         .D(n9428), .Z(mclk_c_enable_206)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(121[15:49])
    defparam i1_2_lut_3_lut_4_lut_adj_198.init = 16'h0080;
    LUT4 i1_2_lut_3_lut_4_lut_adj_199 (.A(n8771), .B(s_en_r1), .C(\s_op_fifo.buf_3_.msbf_N_1174 [1]), 
         .D(n9428), .Z(mclk_c_enable_230)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(121[15:49])
    defparam i1_2_lut_3_lut_4_lut_adj_199.init = 16'h0080;
    LUT4 i3277_2_lut_rep_139_3_lut_4_lut (.A(n8841), .B(bit_cnt[3]), .C(bit_cnt[5]), 
         .D(bit_cnt[4]), .Z(n8806)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3277_2_lut_rep_139_3_lut_4_lut.init = 16'hfffe;
    LUT4 i3232_3_lut_4_lut (.A(n8761), .B(s_offs_r_c[2]), .C(n8774), .D(s_offs_r_c[3]), 
         .Z(n2889)) /* synthesis lut_function=(!(A (B (C+(D))+!B (C+!(D)))+!A (C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam i3232_3_lut_4_lut.init = 16'h0708;
    LUT4 i3_3_lut_rep_98_4_lut (.A(tckn_stb), .B(n8777), .C(s_en_r2), 
         .D(s_offs_r[0]), .Z(n8765)) /* synthesis lut_function=(!((B+!(C (D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam i3_3_lut_rep_98_4_lut.init = 16'h2000;
    LUT4 i3229_3_lut_4_lut (.A(n8753), .B(s_offs_r_c[4]), .C(n8774), .D(s_offs_r_c[5]), 
         .Z(n2881)) /* synthesis lut_function=(!(A (B (C+(D))+!B (C+!(D)))+!A (C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam i3229_3_lut_4_lut.init = 16'h0708;
    FD1P3AX s_rx_en_r1_234 (.D(s_en_r2), .SP(tckn_stb), .CK(mclk_c), .Q(s_rx_en_r1));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(180[3] 190[10])
    defparam s_rx_en_r1_234.GSR = "ENABLED";
    FD1S3AX s_en_r1_212 (.D(s_en_r0), .CK(mclk_c), .Q(s_en_r1)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=15, LSE_RCOL=41, LSE_LLINE=288, LSE_RLINE=288 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(98[3] 112[10])
    defparam s_en_r1_212.GSR = "ENABLED";
    FD1P3IX s_cnt_r_6__220 (.D(bit_cnt_6__N_1238), .SP(mclk_c_enable_129), 
            .CD(n8774), .CK(mclk_c), .Q(bit_cnt[6]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_6__220.GSR = "ENABLED";
    FD1P3IX s_cnt_r_5__219 (.D(bit_cnt_5__N_1232), .SP(mclk_c_enable_129), 
            .CD(n8774), .CK(mclk_c), .Q(bit_cnt[5]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_5__219.GSR = "ENABLED";
    FD1P3IX s_cnt_r_4__218 (.D(bit_cnt_4__N_1226), .SP(mclk_c_enable_129), 
            .CD(n8774), .CK(mclk_c), .Q(bit_cnt[4]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_4__218.GSR = "ENABLED";
    FD1P3IX s_cnt_r_3__217 (.D(bit_cnt_3__N_1220), .SP(mclk_c_enable_129), 
            .CD(n8774), .CK(mclk_c), .Q(bit_cnt[3]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_3__217.GSR = "ENABLED";
    FD1P3IX s_cnt_r_2__216 (.D(bit_cnt_2__N_1214), .SP(mclk_c_enable_129), 
            .CD(n8774), .CK(mclk_c), .Q(bit_cnt[2]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_2__216.GSR = "ENABLED";
    LUT4 tx_shift_stb_I_0_3_lut (.A(s_en_r0), .B(tckn_stb), .C(s_en_r1), 
         .Z(s_tx_shift_stb)) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(194[18:51])
    defparam tx_shift_stb_I_0_3_lut.init = 16'hc8c8;
    FD1S3AY s_offs_r_1__226 (.D(n2898), .CK(mclk_c), .Q(\s_offs_r[1] ));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_1__226.GSR = "ENABLED";
    FD1S3AY s_offs_r_2__227 (.D(n2896), .CK(mclk_c), .Q(s_offs_r_c[2]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_2__227.GSR = "ENABLED";
    FD1S3AY s_offs_r_3__228 (.D(n2889), .CK(mclk_c), .Q(s_offs_r_c[3]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_3__228.GSR = "ENABLED";
    FD1S3AY s_offs_r_4__229 (.D(n2885), .CK(mclk_c), .Q(s_offs_r_c[4]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_4__229.GSR = "ENABLED";
    FD1S3AY s_offs_r_5__230 (.D(n2881), .CK(mclk_c), .Q(s_offs_r_c[5]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_5__230.GSR = "ENABLED";
    LUT4 i2361_2_lut_3_lut_4_lut (.A(n8784), .B(bit_cnt[9]), .C(s_en_r0), 
         .D(s_stop_N_1312), .Z(n4460)) /* synthesis lut_function=(!(A+(B+!(C (D))))) */ ;
    defparam i2361_2_lut_3_lut_4_lut.init = 16'h1000;
    LUT4 i3226_3_lut_4_lut (.A(n8750), .B(s_offs_r_c[5]), .C(n8774), .D(s_offs_r_c[6]), 
         .Z(n2877)) /* synthesis lut_function=(!(A (B (C+(D))+!B (C+!(D)))+!A (C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam i3226_3_lut_4_lut.init = 16'h0708;
    LUT4 i1_2_lut_3_lut_4_lut_adj_200 (.A(n8784), .B(bit_cnt[9]), .C(s_en_r0), 
         .D(s_stop_N_1312), .Z(mclk_c_enable_41)) /* synthesis lut_function=(!(A (C)+!A (B (C)+!B !((D)+!C)))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_200.init = 16'h1f0f;
    LUT4 desc_rd_stb_I_0_2_lut_rep_100_3_lut_4_lut (.A(n8784), .B(bit_cnt[9]), 
         .C(s_en_r1), .D(tckn_stb), .Z(mclk_c_enable_112)) /* synthesis lut_function=(!(A+(B+!(C (D))))) */ ;
    defparam desc_rd_stb_I_0_2_lut_rep_100_3_lut_4_lut.init = 16'h1000;
    LUT4 i5015_2_lut (.A(s_ptr_data[8]), .B(s_offs_r[0]), .Z(s_desc_addr[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i5015_2_lut.init = 16'h6666;
    LUT4 i3273_2_lut_rep_147_3_lut_4_lut (.A(n8866), .B(bit_cnt[2]), .C(bit_cnt[4]), 
         .D(bit_cnt[3]), .Z(n8814)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3273_2_lut_rep_147_3_lut_4_lut.init = 16'hfffe;
    FD1P3IX s_cnt_r_1__215 (.D(bit_cnt_1__N_1208), .SP(mclk_c_enable_129), 
            .CD(n8774), .CK(mclk_c), .Q(bit_cnt[1]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_1__215.GSR = "ENABLED";
    LUT4 i2_2_lut_rep_104_3_lut_4_lut (.A(n8790), .B(bit_cnt[8]), .C(tckn_stb), 
         .D(bit_cnt[9]), .Z(n8771)) /* synthesis lut_function=(!(A+(B+((D)+!C)))) */ ;
    defparam i2_2_lut_rep_104_3_lut_4_lut.init = 16'h0010;
    LUT4 bit_cnt_0__I_125_4_lut (.A(bit_cnt[0]), .B(s_pos_0_3__N_670[0]), 
         .C(n9427), .D(s_cnt_stb), .Z(bit_cnt_0__N_1203)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_0__I_125_4_lut.init = 16'hc5ca;
    LUT4 tckn_stb_I_0_2_lut (.A(tckn_stb), .B(s_en_r1), .Z(s_cnt_stb)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(123[15:35])
    defparam tckn_stb_I_0_2_lut.init = 16'h8888;
    LUT4 i3225_4_lut (.A(s_offs_r_c[7]), .B(n8774), .C(n8747), .D(s_offs_r_c[6]), 
         .Z(n2875)) /* synthesis lut_function=(!(A (B+(C (D)))+!A (B+!(C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam i3225_4_lut.init = 16'h1222;
    LUT4 bit_cnt_9__I_116_4_lut (.A(bit_cnt[9]), .B(\s_desc_data[9] ), .C(n9427), 
         .D(n8784), .Z(bit_cnt_9__N_1189)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_9__I_116_4_lut.init = 16'hcac5;
    LUT4 bit_cnt_8__I_133_4_lut (.A(bit_cnt[8]), .B(\s_desc_data[8] ), .C(n9427), 
         .D(n8790), .Z(bit_cnt_8__N_1250)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_8__I_133_4_lut.init = 16'hcac5;
    LUT4 bit_cnt_7__I_132_4_lut (.A(bit_cnt[7]), .B(\s_desc_data[7] ), .C(n9427), 
         .D(n8795), .Z(bit_cnt_7__N_1244)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_7__I_132_4_lut.init = 16'hcac5;
    LUT4 i3374_2_lut_rep_110_3_lut_4_lut (.A(n8795), .B(bit_cnt[7]), .C(bit_cnt[9]), 
         .D(bit_cnt[8]), .Z(n8777)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3374_2_lut_rep_110_3_lut_4_lut.init = 16'hfffe;
    LUT4 s_rx_en_r1_I_0_273_2_lut_rep_193 (.A(s_rx_en_r1), .B(tckn_stb), 
         .Z(mclk_c_enable_203)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam s_rx_en_r1_I_0_273_2_lut_rep_193.init = 16'h8888;
    LUT4 s_count_r_0__I_0_502_2_lut_3_lut (.A(s_rx_en_r1), .B(tckn_stb), 
         .C(\s_count_r[0] ), .Z(s_count_r_0__N_893)) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam s_count_r_0__I_0_502_2_lut_3_lut.init = 16'h7878;
    LUT4 bit_pos_0__I_0_2_lut_3_lut (.A(s_rx_en_r1), .B(tckn_stb), .C(\bit_pos[0] ), 
         .Z(bit_pos_0__N_816)) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam bit_pos_0__I_0_2_lut_3_lut.init = 16'h7878;
    LUT4 shift_stb_I_0_2_lut_rep_167_3_lut (.A(s_rx_en_r1), .B(tckn_stb), 
         .C(\s_count_r[0] ), .Z(n8834)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam shift_stb_I_0_2_lut_rep_167_3_lut.init = 16'h0808;
    LUT4 s_count_r_9__I_110_2_lut_rep_155_3_lut_4_lut (.A(s_rx_en_r1), .B(tckn_stb), 
         .C(\s_count_r[1] ), .D(\s_count_r[0] ), .Z(n8822)) /* synthesis lut_function=(!(((C+(D))+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam s_count_r_9__I_110_2_lut_rep_155_3_lut_4_lut.init = 16'h0008;
    LUT4 s_count_r_1__I_0_504_2_lut_3_lut_4_lut (.A(s_rx_en_r1), .B(tckn_stb), 
         .C(\s_count_r[1] ), .D(\s_count_r[0] ), .Z(s_count_r_1__N_897)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (C))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam s_count_r_1__I_0_504_2_lut_3_lut_4_lut.init = 16'hf078;
    LUT4 i2_3_lut_rep_168_4_lut (.A(s_rx_en_r1), .B(tckn_stb), .C(\bit_pos[0] ), 
         .D(\bit_pos[1] ), .Z(n8835)) /* synthesis lut_function=(!(((C+(D))+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam i2_3_lut_rep_168_4_lut.init = 16'h0008;
    LUT4 bit_pos_3__I_93_3_lut_rep_169_4_lut (.A(s_rx_en_r1), .B(tckn_stb), 
         .C(\bit_pos[0] ), .D(op_msbf), .Z(n8836)) /* synthesis lut_function=(!(((C (D)+!C !(D))+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam bit_pos_3__I_93_3_lut_rep_169_4_lut.init = 16'h0880;
    LUT4 i3267_2_lut_rep_199 (.A(bit_cnt[0]), .B(bit_cnt[1]), .Z(n8866)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i3267_2_lut_rep_199.init = 16'heeee;
    LUT4 i3283_2_lut_rep_117_3_lut_4_lut (.A(n8806), .B(bit_cnt[6]), .C(bit_cnt[8]), 
         .D(bit_cnt[7]), .Z(n8784)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3283_2_lut_rep_117_3_lut_4_lut.init = 16'hfffe;
    LUT4 i3269_2_lut_rep_174_3_lut (.A(bit_cnt[0]), .B(bit_cnt[1]), .C(bit_cnt[2]), 
         .Z(n8841)) /* synthesis lut_function=(A+(B+(C))) */ ;
    defparam i3269_2_lut_rep_174_3_lut.init = 16'hfefe;
    LUT4 i3271_2_lut_rep_158_3_lut_4_lut (.A(bit_cnt[0]), .B(bit_cnt[1]), 
         .C(bit_cnt[3]), .D(bit_cnt[2]), .Z(n8825)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3271_2_lut_rep_158_3_lut_4_lut.init = 16'hfffe;
    LUT4 bit_cnt_6__I_131_4_lut (.A(bit_cnt[6]), .B(\s_desc_data[6] ), .C(n9427), 
         .D(n8806), .Z(bit_cnt_6__N_1238)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_6__I_131_4_lut.init = 16'hcac5;
    LUT4 bit_cnt_5__I_130_4_lut (.A(bit_cnt[5]), .B(\s_desc_data[5] ), .C(n9427), 
         .D(n8814), .Z(bit_cnt_5__N_1232)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_5__I_130_4_lut.init = 16'hcac5;
    LUT4 bit_cnt_4__I_129_4_lut (.A(bit_cnt[4]), .B(\s_desc_data[4] ), .C(n9427), 
         .D(n8825), .Z(bit_cnt_4__N_1226)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_4__I_129_4_lut.init = 16'hcac5;
    LUT4 bit_cnt_3__I_128_4_lut (.A(bit_cnt[3]), .B(s_pos_0_3__N_670[3]), 
         .C(n9427), .D(n8841), .Z(bit_cnt_3__N_1220)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_3__I_128_4_lut.init = 16'hcac5;
    LUT4 bit_cnt_2__I_127_4_lut (.A(bit_cnt[2]), .B(s_pos_0_3__N_670[2]), 
         .C(n9427), .D(n8866), .Z(bit_cnt_2__N_1214)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_2__I_127_4_lut.init = 16'hcac5;
    CCU2D s_offs_r_7__I_0_8 (.A0(s_stop_N_1313[1]), .B0(\s_offs_r[1] ), 
          .C0(s_stop_N_1313[0]), .D0(s_offs_r[0]), .A1(GND_net), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n6960), .S1(s_stop_N_1312));
    defparam s_offs_r_7__I_0_8.INIT0 = 16'h9009;
    defparam s_offs_r_7__I_0_8.INIT1 = 16'hFFFF;
    defparam s_offs_r_7__I_0_8.INJECT1_0 = "YES";
    defparam s_offs_r_7__I_0_8.INJECT1_1 = "NO";
    CCU2D s_offs_r_7__I_0_7 (.A0(s_stop_N_1313[5]), .B0(s_offs_r_c[5]), 
          .C0(s_stop_N_1313[4]), .D0(s_offs_r_c[4]), .A1(s_stop_N_1313[3]), 
          .B1(s_offs_r_c[3]), .C1(s_stop_N_1313[2]), .D1(s_offs_r_c[2]), 
          .CIN(n6959), .COUT(n6960));
    defparam s_offs_r_7__I_0_7.INIT0 = 16'h9009;
    defparam s_offs_r_7__I_0_7.INIT1 = 16'h9009;
    defparam s_offs_r_7__I_0_7.INJECT1_0 = "YES";
    defparam s_offs_r_7__I_0_7.INJECT1_1 = "YES";
    CCU2D s_offs_r_7__I_0_0 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_stop_N_1313[7]), .B1(s_offs_r_c[7]), .C1(s_stop_N_1313[6]), 
          .D1(s_offs_r_c[6]), .COUT(n6959));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(174[22:55])
    defparam s_offs_r_7__I_0_0.INIT0 = 16'hF000;
    defparam s_offs_r_7__I_0_0.INIT1 = 16'h9009;
    defparam s_offs_r_7__I_0_0.INJECT1_0 = "NO";
    defparam s_offs_r_7__I_0_0.INJECT1_1 = "YES";
    LUT4 s_offs_r_7__I_138_2_lut_rep_90_3_lut (.A(n8765), .B(\s_offs_r[1] ), 
         .C(s_offs_r_c[2]), .Z(n8757)) /* synthesis lut_function=(A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam s_offs_r_7__I_138_2_lut_rep_90_3_lut.init = 16'h8080;
    LUT4 s_offs_r_7__I_137_2_lut_rep_86_3_lut_4_lut (.A(n8765), .B(\s_offs_r[1] ), 
         .C(s_offs_r_c[3]), .D(s_offs_r_c[2]), .Z(n8753)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam s_offs_r_7__I_137_2_lut_rep_86_3_lut_4_lut.init = 16'h8000;
    LUT4 i3241_3_lut_4_lut (.A(n8765), .B(\s_offs_r[1] ), .C(n8774), .D(s_offs_r_c[2]), 
         .Z(n2896)) /* synthesis lut_function=(!(A (B (C+(D))+!B (C+!(D)))+!A (C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam i3241_3_lut_4_lut.init = 16'h0708;
    LUT4 i3281_2_lut_rep_123_3_lut_4_lut (.A(n8814), .B(bit_cnt[5]), .C(bit_cnt[7]), 
         .D(bit_cnt[6]), .Z(n8790)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3281_2_lut_rep_123_3_lut_4_lut.init = 16'hfffe;
    LUT4 desc_rd_stb_I_0_2_lut_rep_230 (.A(n8784), .B(bit_cnt[9]), .C(s_en_r1), 
         .D(tckn_stb), .Z(n9427)) /* synthesis lut_function=(!(A+(B+!(C (D))))) */ ;
    defparam desc_rd_stb_I_0_2_lut_rep_230.init = 16'h1000;
    LUT4 bit_cnt_1__I_126_4_lut (.A(bit_cnt[1]), .B(s_pos_0_3__N_670[1]), 
         .C(n9427), .D(bit_cnt[0]), .Z(bit_cnt_1__N_1208)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_1__I_126_4_lut.init = 16'hcac5;
    LUT4 s_offs_inc_I_141_4_lut (.A(n8771), .B(s_en_r2), .C(n13), .D(n14), 
         .Z(s_offs_inc)) /* synthesis lut_function=(A (B)+!A !((C+(D))+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(148[16:64])
    defparam s_offs_inc_I_141_4_lut.init = 16'h888c;
    LUT4 s_offs_r_7__I_135_2_lut_rep_80_3_lut_4_lut (.A(n8757), .B(s_offs_r_c[3]), 
         .C(s_offs_r_c[5]), .D(s_offs_r_c[4]), .Z(n8747)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam s_offs_r_7__I_135_2_lut_rep_80_3_lut_4_lut.init = 16'h8000;
    LUT4 i5_4_lut (.A(s_offs_r[0]), .B(s_offs_r_c[6]), .C(s_offs_r_c[4]), 
         .D(s_offs_r_c[7]), .Z(n13)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i5_4_lut.init = 16'hfffe;
    LUT4 i6_4_lut (.A(s_offs_r_c[2]), .B(s_offs_r_c[5]), .C(s_offs_r_c[3]), 
         .D(\s_offs_r[1] ), .Z(n14)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i6_4_lut.init = 16'hfffe;
    LUT4 i3279_2_lut_rep_128_3_lut_4_lut (.A(n8825), .B(bit_cnt[4]), .C(bit_cnt[6]), 
         .D(bit_cnt[5]), .Z(n8795)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3279_2_lut_rep_128_3_lut_4_lut.init = 16'hfffe;
    
endmodule
//
// Verilog Description of module \jtag_enh_rx(16,10,10,4) 
//

module \jtag_enh_rx(16,10,10,4)  (s_count_r_0__N_893, bit_pos_0__N_816, 
            mclk_c, mclk_c_enable_112, s_pos_0_3__N_670, \s_op_fifo.buf_3_.msbf_N_1174 , 
            s_rx_mem_wr_stb, \s_desc_data[4] , \s_desc_data[5] , \s_desc_data[6] , 
            \s_desc_data[7] , \s_desc_data[8] , \s_desc_data[9] , op_msbf, 
            n8834, s_count_r, n8836, bit_pos, mclk_c_enable_242, \s_desc_data[31] , 
            n9419, mclk_c_enable_206, n8822, mclk_c_enable_230, mclk_c_enable_218, 
            s_rx_mem_addr, mclk_c_enable_203, \s_count_r[0] , \bit_pos[0] , 
            s_mtdo, s_rx_mem_din, \s_desc_data[29] , GND_net, \s_desc_data[27] , 
            \s_desc_data[28] , n8835, \s_desc_data[25] , \s_desc_data[26] , 
            \s_desc_data[23] , \s_desc_data[24] , \s_desc_data[21] , \s_desc_data[22] , 
            n9427, \s_desc_data[20] , s_count_r_1__N_897);
    input s_count_r_0__N_893;
    input bit_pos_0__N_816;
    input mclk_c;
    input mclk_c_enable_112;
    input [3:0]s_pos_0_3__N_670;
    output [3:0]\s_op_fifo.buf_3_.msbf_N_1174 ;
    output s_rx_mem_wr_stb;
    input \s_desc_data[4] ;
    input \s_desc_data[5] ;
    input \s_desc_data[6] ;
    input \s_desc_data[7] ;
    input \s_desc_data[8] ;
    input \s_desc_data[9] ;
    output op_msbf;
    input n8834;
    output [9:0]s_count_r;
    input n8836;
    output [3:0]bit_pos;
    input mclk_c_enable_242;
    input \s_desc_data[31] ;
    input n9419;
    input mclk_c_enable_206;
    input n8822;
    input mclk_c_enable_230;
    input mclk_c_enable_218;
    output [9:0]s_rx_mem_addr;
    input mclk_c_enable_203;
    output \s_count_r[0] ;
    output \bit_pos[0] ;
    input s_mtdo;
    output [15:0]s_rx_mem_din;
    input \s_desc_data[29] ;
    input GND_net;
    input \s_desc_data[27] ;
    input \s_desc_data[28] ;
    input n8835;
    input \s_desc_data[25] ;
    input \s_desc_data[26] ;
    input \s_desc_data[23] ;
    input \s_desc_data[24] ;
    input \s_desc_data[21] ;
    input \s_desc_data[22] ;
    input n9427;
    input \s_desc_data[20] ;
    input s_count_r_1__N_897;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    wire [9:0]\s_op0.addr ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(138[9:14])
    wire [9:0]op_addr_9__N_817;
    
    wire busy;
    wire [9:0]op_addr;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(77[2:9])
    
    wire n8021, n8022;
    wire [1:0]\s_op_fifo.tail ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire s_addr_r1_2__N_874, fifo_get;
    wire [9:0]n2428;
    
    wire s_addr_r1_1__N_876, n8024, n8025, bit_pos_3__N_804;
    wire [3:0]op_pos;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(79[2:8])
    wire [3:0]n2429;
    
    wire bit_pos_1__N_814;
    wire [9:0]op_len;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(78[2:8])
    wire [9:0]n2427;
    
    wire s_addr_r1_0__N_878, start, n7871, n7872;
    wire [9:0]\s_op_fifo.buf[2].addr ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    wire [9:0]\s_op_fifo.buf[3].addr ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n8009;
    wire [9:0]\s_op_fifo.buf[0].addr ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    wire [9:0]\s_op_fifo.buf[1].addr ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n8008, n8006, n8005;
    wire [3:0]\s_op_fifo.buf[2].pos ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    wire [3:0]\s_op_fifo.buf[3].pos ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n8000;
    wire [3:0]\s_op_fifo.buf[0].pos ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    wire [3:0]\s_op_fifo.buf[1].pos ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n7999;
    wire [9:0]op_len_9__N_827;
    
    wire n8865, mem_wr_N_1153;
    wire [9:0]\s_op_fifo.buf[2].len ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    wire [9:0]\s_op_fifo.buf[3].len ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n7953;
    wire [9:0]\s_op_fifo.buf[0].len ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    wire [9:0]\s_op_fifo.buf[1].len ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n7952;
    wire [1:0]n1;
    
    wire n8768;
    wire [9:0]s_addr_r1;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(115[9:18])
    
    wire n8759, s_addr_r1_7__N_864, s_addr_r1_9__N_842, n7926, n7925, 
        \s_op_fifo.buf_2_.msbf , \s_op_fifo.buf_3_.msbf , n7914;
    wire [9:0]s_count_r_c;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(121[9:18])
    
    wire n8802, s_count_r_3__N_905;
    wire [3:0]bit_pos_c;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(76[2:9])
    
    wire n8812, \s_rx_op.msbf , \s_op_fifo.buf_0_.msbf , \s_op_fifo.empty , 
        n7177, n8772, n8763, s_addr_r1_6__N_866, \s_op_fifo.buf_1_.msbf , 
        n7913, n7911, n7910, n7908, n7907, n7905, n7904, n7902, 
        n7901, n7899, n7898, n8876, n8775, n8804, n8779, s_addr_r1_5__N_868, 
        n8823, n7896, n7895, n8037, n7893, n7892, n8853, n8850, 
        mclk_c_enable_257, mem_dout_15__N_739, n8851, mclk_c_enable_253, 
        n7890, mem_dout_15__N_755, n8852, mclk_c_enable_249, mclk_c_enable_79, 
        mem_dout_15__N_799, n6, n7889, n7887, n7886, mem_dout_15__N_771, 
        n7884, n7883, n8854, mclk_c_enable_256, n7881, mem_dout_15__N_743, 
        mclk_c_enable_252, n7880, mem_dout_15__N_759, mclk_c_enable_248, 
        n7878, n7877, n8782, n8764, s_count_r_9__N_880, mem_dout_15__N_775, 
        n8855, mclk_c_enable_255, mem_dout_15__N_747, mclk_c_enable_251, 
        mem_dout_15__N_763, mclk_c_enable_247, mem_dout_15__N_779, n8857, 
        mclk_c_enable_245, mem_dout_15__N_787, mclk_c_enable_244, n7875, 
        n7874, n8036, mem_dout_15__N_791, mclk_c_enable_243, mem_dout_15__N_795, 
        n8811, n8030, n8031, n8034, n8033, n7044, n8786, s_addr_r1_4__N_870, 
        n8787, s_count_r_8__N_925, n7043, mclk_c_enable_254, mem_dout_15__N_751, 
        n8789, s_addr_r1_3__N_872, mclk_c_enable_250;
    wire [1:0]\s_op_fifo.tail_1__N_1054 ;
    
    wire n8880, n8881, n8882, mem_dout_15__N_767, \s_rx_op.msbf_N_1187 , 
        n8792, s_count_r_7__N_921, mclk_c_enable_246, mem_dout_15__N_783, 
        n8794;
    wire [3:0]op_pos_3__N_837;
    
    wire s_addr_r1_8__N_862;
    wire [1:0]\s_op_fifo.head ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n4_adj_1591, n7042, n7041, n7040, s_count_r_6__N_917, s_count_r_5__N_913, 
        s_count_r_4__N_909;
    
    LUT4 \s_op0.addr_9__I_0_i4_3_lut  (.A(\s_op0.addr [3]), .B(op_addr_9__N_817[3]), 
         .C(busy), .Z(op_addr[3])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i4_3_lut .init = 16'hcaca;
    PFUMX i5966 (.BLUT(n8021), .ALUT(n8022), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[7]));
    PFUMX mux_512_i3 (.BLUT(s_addr_r1_2__N_874), .ALUT(op_addr[2]), .C0(fifo_get), 
          .Z(n2428[2]));
    PFUMX mux_512_i2 (.BLUT(s_addr_r1_1__N_876), .ALUT(op_addr[1]), .C0(fifo_get), 
          .Z(n2428[1]));
    PFUMX i5969 (.BLUT(n8024), .ALUT(n8025), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[6]));
    PFUMX mux_513_i4 (.BLUT(bit_pos_3__N_804), .ALUT(op_pos[3]), .C0(fifo_get), 
          .Z(n2429[3]));
    PFUMX mux_513_i2 (.BLUT(bit_pos_1__N_814), .ALUT(op_pos[1]), .C0(fifo_get), 
          .Z(n2429[1]));
    PFUMX mux_511_i1 (.BLUT(s_count_r_0__N_893), .ALUT(op_len[0]), .C0(fifo_get), 
          .Z(n2427[0]));
    PFUMX mux_512_i1 (.BLUT(s_addr_r1_0__N_878), .ALUT(op_addr[0]), .C0(fifo_get), 
          .Z(n2428[0]));
    PFUMX mux_513_i1 (.BLUT(bit_pos_0__N_816), .ALUT(op_pos[0]), .C0(fifo_get), 
          .Z(n2429[0]));
    FD1S3IX s_start_r_394 (.D(mclk_c_enable_112), .CK(mclk_c), .CD(busy), 
            .Q(start));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(172[3] 176[10])
    defparam s_start_r_394.GSR = "ENABLED";
    PFUMX i5816 (.BLUT(n7871), .ALUT(n7872), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[3]));
    LUT4 i5952_3_lut (.A(\s_op_fifo.buf[2].addr [8]), .B(\s_op_fifo.buf[3].addr [8]), 
         .C(\s_op_fifo.tail [0]), .Z(n8009)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5952_3_lut.init = 16'hcaca;
    LUT4 i5951_3_lut (.A(\s_op_fifo.buf[0].addr [8]), .B(\s_op_fifo.buf[1].addr [8]), 
         .C(\s_op_fifo.tail [0]), .Z(n8008)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5951_3_lut.init = 16'hcaca;
    LUT4 \s_op0.addr_9__I_0_i5_3_lut  (.A(\s_op0.addr [4]), .B(op_addr_9__N_817[4]), 
         .C(busy), .Z(op_addr[4])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i5_3_lut .init = 16'hcaca;
    LUT4 i5949_3_lut (.A(\s_op_fifo.buf[2].addr [9]), .B(\s_op_fifo.buf[3].addr [9]), 
         .C(\s_op_fifo.tail [0]), .Z(n8006)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5949_3_lut.init = 16'hcaca;
    LUT4 i5948_3_lut (.A(\s_op_fifo.buf[0].addr [9]), .B(\s_op_fifo.buf[1].addr [9]), 
         .C(\s_op_fifo.tail [0]), .Z(n8005)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5948_3_lut.init = 16'hcaca;
    LUT4 i5943_3_lut (.A(\s_op_fifo.buf[2].pos [1]), .B(\s_op_fifo.buf[3].pos [1]), 
         .C(\s_op_fifo.tail [0]), .Z(n8000)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5943_3_lut.init = 16'hcaca;
    LUT4 i5942_3_lut (.A(\s_op_fifo.buf[0].pos [1]), .B(\s_op_fifo.buf[1].pos [1]), 
         .C(\s_op_fifo.tail [0]), .Z(n7999)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5942_3_lut.init = 16'hcaca;
    LUT4 \s_op0.addr_9__I_0_i6_3_lut  (.A(\s_op0.addr [5]), .B(op_addr_9__N_817[5]), 
         .C(busy), .Z(op_addr[5])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i6_3_lut .init = 16'hcaca;
    LUT4 \s_op0.addr_9__I_0_i7_3_lut  (.A(\s_op0.addr [6]), .B(op_addr_9__N_817[6]), 
         .C(busy), .Z(op_addr[6])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i7_3_lut .init = 16'hcaca;
    LUT4 \s_op0.addr_9__I_0_i8_3_lut  (.A(\s_op0.addr [7]), .B(op_addr_9__N_817[7]), 
         .C(busy), .Z(op_addr[7])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i8_3_lut .init = 16'hcaca;
    LUT4 vec_len_9__I_0_i2_3_lut (.A(s_pos_0_3__N_670[1]), .B(op_len_9__N_827[1]), 
         .C(busy), .Z(op_len[1])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_len_9__I_0_i2_3_lut.init = 16'hcaca;
    FD1P3AY \s_op_fifo.head_FSM_i0_i0  (.D(\s_op_fifo.buf_3_.msbf_N_1174 [3]), 
            .SP(mclk_c_enable_112), .CK(mclk_c), .Q(\s_op_fifo.buf_3_.msbf_N_1174 [0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam \s_op_fifo.head_FSM_i0_i0 .GSR = "ENABLED";
    FD1S3IX s_store_stb_441 (.D(mem_wr_N_1153), .CK(mclk_c), .CD(n8865), 
            .Q(s_rx_mem_wr_stb));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(317[3] 321[10])
    defparam s_store_stb_441.GSR = "ENABLED";
    LUT4 vec_len_9__I_0_i3_3_lut (.A(s_pos_0_3__N_670[2]), .B(op_len_9__N_827[2]), 
         .C(busy), .Z(op_len[2])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_len_9__I_0_i3_3_lut.init = 16'hcaca;
    LUT4 vec_len_9__I_0_i4_3_lut (.A(s_pos_0_3__N_670[3]), .B(op_len_9__N_827[3]), 
         .C(busy), .Z(op_len[3])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_len_9__I_0_i4_3_lut.init = 16'hcaca;
    LUT4 vec_len_9__I_0_i5_3_lut (.A(\s_desc_data[4] ), .B(op_len_9__N_827[4]), 
         .C(busy), .Z(op_len[4])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_len_9__I_0_i5_3_lut.init = 16'hcaca;
    LUT4 vec_len_9__I_0_i6_3_lut (.A(\s_desc_data[5] ), .B(op_len_9__N_827[5]), 
         .C(busy), .Z(op_len[5])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_len_9__I_0_i6_3_lut.init = 16'hcaca;
    LUT4 vec_len_9__I_0_i7_3_lut (.A(\s_desc_data[6] ), .B(op_len_9__N_827[6]), 
         .C(busy), .Z(op_len[6])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_len_9__I_0_i7_3_lut.init = 16'hcaca;
    LUT4 vec_len_9__I_0_i8_3_lut (.A(\s_desc_data[7] ), .B(op_len_9__N_827[7]), 
         .C(busy), .Z(op_len[7])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_len_9__I_0_i8_3_lut.init = 16'hcaca;
    LUT4 vec_len_9__I_0_i9_3_lut (.A(\s_desc_data[8] ), .B(op_len_9__N_827[8]), 
         .C(busy), .Z(op_len[8])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_len_9__I_0_i9_3_lut.init = 16'hcaca;
    LUT4 vec_len_9__I_0_i10_3_lut (.A(\s_desc_data[9] ), .B(op_len_9__N_827[9]), 
         .C(busy), .Z(op_len[9])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_len_9__I_0_i10_3_lut.init = 16'hcaca;
    LUT4 \s_op0.addr_9__I_0_i9_3_lut  (.A(\s_op0.addr [8]), .B(op_addr_9__N_817[8]), 
         .C(busy), .Z(op_addr[8])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i9_3_lut .init = 16'hcaca;
    LUT4 i5896_3_lut (.A(\s_op_fifo.buf[2].len [1]), .B(\s_op_fifo.buf[3].len [1]), 
         .C(\s_op_fifo.tail [0]), .Z(n7953)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5896_3_lut.init = 16'hcaca;
    LUT4 i5895_3_lut (.A(\s_op_fifo.buf[0].len [1]), .B(\s_op_fifo.buf[1].len [1]), 
         .C(\s_op_fifo.tail [0]), .Z(n7952)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5895_3_lut.init = 16'hcaca;
    LUT4 i1171_2_lut (.A(\s_op_fifo.tail [0]), .B(fifo_get), .Z(n1[0])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam i1171_2_lut.init = 16'h6666;
    LUT4 \s_op0.addr_9__I_0_i10_3_lut  (.A(\s_op0.addr [9]), .B(op_addr_9__N_817[9]), 
         .C(busy), .Z(op_addr[9])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i10_3_lut .init = 16'hcaca;
    LUT4 s_addr_r1_9__I_95_3_lut_rep_92_4_lut (.A(op_msbf), .B(n8768), .C(s_addr_r1[6]), 
         .D(s_addr_r1[7]), .Z(n8759)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_95_3_lut_rep_92_4_lut.init = 16'h4008;
    LUT4 s_addr_r1_7__I_0_2_lut_4_lut (.A(op_msbf), .B(n8768), .C(s_addr_r1[6]), 
         .D(s_addr_r1[7]), .Z(s_addr_r1_7__N_864)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_7__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 s_addr_r1_9__I_0_4_lut (.A(s_addr_r1[9]), .B(op_msbf), .C(n8759), 
         .D(s_addr_r1[8]), .Z(s_addr_r1_9__N_842)) /* synthesis lut_function=(A (B ((D)+!C)+!B !(C (D)))+!A !(B ((D)+!C)+!B !(C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(286[22:41])
    defparam s_addr_r1_9__I_0_4_lut.init = 16'h9a6a;
    LUT4 i5869_3_lut (.A(\s_op_fifo.buf[2].pos [2]), .B(\s_op_fifo.buf[3].pos [2]), 
         .C(\s_op_fifo.tail [0]), .Z(n7926)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5869_3_lut.init = 16'hcaca;
    LUT4 i5868_3_lut (.A(\s_op_fifo.buf[0].pos [2]), .B(\s_op_fifo.buf[1].pos [2]), 
         .C(\s_op_fifo.tail [0]), .Z(n7925)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5868_3_lut.init = 16'hcaca;
    LUT4 i5857_3_lut (.A(\s_op_fifo.buf_2_.msbf ), .B(\s_op_fifo.buf_3_.msbf ), 
         .C(\s_op_fifo.tail [0]), .Z(n7914)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5857_3_lut.init = 16'hcaca;
    LUT4 s_count_r_9__I_108_2_lut_rep_135_3_lut_4_lut (.A(n8834), .B(s_count_r[1]), 
         .C(s_count_r_c[3]), .D(s_count_r_c[2]), .Z(n8802)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_108_2_lut_rep_135_3_lut_4_lut.init = 16'h0002;
    LUT4 s_count_r_3__I_0_508_2_lut_3_lut_4_lut (.A(n8834), .B(s_count_r[1]), 
         .C(s_count_r_c[3]), .D(s_count_r_c[2]), .Z(s_count_r_3__N_905)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_3__I_0_508_2_lut_3_lut_4_lut.init = 16'hf0d2;
    LUT4 bit_pos_3__I_91_3_lut_rep_145_4_lut (.A(op_msbf), .B(n8836), .C(bit_pos[1]), 
         .D(bit_pos_c[2]), .Z(n8812)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam bit_pos_3__I_91_3_lut_rep_145_4_lut.init = 16'h4008;
    LUT4 i5964_3_lut (.A(\s_op_fifo.buf[0].addr [7]), .B(\s_op_fifo.buf[1].addr [7]), 
         .C(\s_op_fifo.tail [0]), .Z(n8021)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5964_3_lut.init = 16'hcaca;
    FD1P3AX s_rx_msbf_415 (.D(\s_rx_op.msbf ), .SP(fifo_get), .CK(mclk_c), 
            .Q(op_msbf));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(218[3] 222[10])
    defparam s_rx_msbf_415.GSR = "ENABLED";
    FD1P3AX \s_op_fifo.buf_0..msbf_410  (.D(\s_desc_data[31] ), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf_0_.msbf ));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..msbf_410 .GSR = "DISABLED";
    FD1P3JX \s_op_fifo.empty_414  (.D(n9419), .SP(mclk_c_enable_112), .PD(n7177), 
            .CK(mclk_c), .Q(\s_op_fifo.empty )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.empty_414 .GSR = "ENABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i0  (.D(s_pos_0_3__N_670[0]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i0 .GSR = "DISABLED";
    LUT4 s_addr_r1_9__I_96_3_lut_rep_96_4_lut (.A(op_msbf), .B(n8772), .C(s_addr_r1[5]), 
         .D(s_addr_r1[6]), .Z(n8763)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_96_3_lut_rep_96_4_lut.init = 16'h4008;
    LUT4 s_addr_r1_6__I_0_2_lut_4_lut (.A(op_msbf), .B(n8772), .C(s_addr_r1[5]), 
         .D(s_addr_r1[6]), .Z(s_addr_r1_6__N_866)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_6__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 s_count_r_2__bdd_4_lut (.A(s_count_r_c[2]), .B(n8822), .C(op_len[2]), 
         .D(fifo_get), .Z(n2427[2])) /* synthesis lut_function=(A (B (C (D))+!B (C+!(D)))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam s_count_r_2__bdd_4_lut.init = 16'hf066;
    LUT4 i5856_3_lut (.A(\s_op_fifo.buf_0_.msbf ), .B(\s_op_fifo.buf_1_.msbf ), 
         .C(\s_op_fifo.tail [0]), .Z(n7913)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5856_3_lut.init = 16'hcaca;
    LUT4 i5854_3_lut (.A(\s_op_fifo.buf[2].pos [0]), .B(\s_op_fifo.buf[3].pos [0]), 
         .C(\s_op_fifo.tail [0]), .Z(n7911)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5854_3_lut.init = 16'hcaca;
    LUT4 i5853_3_lut (.A(\s_op_fifo.buf[0].pos [0]), .B(\s_op_fifo.buf[1].pos [0]), 
         .C(\s_op_fifo.tail [0]), .Z(n7910)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5853_3_lut.init = 16'hcaca;
    LUT4 i5851_3_lut (.A(\s_op_fifo.buf[2].addr [0]), .B(\s_op_fifo.buf[3].addr [0]), 
         .C(\s_op_fifo.tail [0]), .Z(n7908)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5851_3_lut.init = 16'hcaca;
    LUT4 i5850_3_lut (.A(\s_op_fifo.buf[0].addr [0]), .B(\s_op_fifo.buf[1].addr [0]), 
         .C(\s_op_fifo.tail [0]), .Z(n7907)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5850_3_lut.init = 16'hcaca;
    LUT4 i5848_3_lut (.A(\s_op_fifo.buf[2].len [0]), .B(\s_op_fifo.buf[3].len [0]), 
         .C(\s_op_fifo.tail [0]), .Z(n7905)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5848_3_lut.init = 16'hcaca;
    LUT4 i5847_3_lut (.A(\s_op_fifo.buf[0].len [0]), .B(\s_op_fifo.buf[1].len [0]), 
         .C(\s_op_fifo.tail [0]), .Z(n7904)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5847_3_lut.init = 16'hcaca;
    LUT4 i5845_3_lut (.A(\s_op_fifo.buf[2].len [2]), .B(\s_op_fifo.buf[3].len [2]), 
         .C(\s_op_fifo.tail [0]), .Z(n7902)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5845_3_lut.init = 16'hcaca;
    LUT4 i5844_3_lut (.A(\s_op_fifo.buf[0].len [2]), .B(\s_op_fifo.buf[1].len [2]), 
         .C(\s_op_fifo.tail [0]), .Z(n7901)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5844_3_lut.init = 16'hcaca;
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i0  (.D(\s_op0.addr [0]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..msbf_406  (.D(\s_desc_data[31] ), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf_1_.msbf ));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..msbf_406 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i0  (.D(s_pos_0_3__N_670[0]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i0 .GSR = "DISABLED";
    LUT4 i5842_3_lut (.A(\s_op_fifo.buf[2].len [3]), .B(\s_op_fifo.buf[3].len [3]), 
         .C(\s_op_fifo.tail [0]), .Z(n7899)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5842_3_lut.init = 16'hcaca;
    LUT4 i5841_3_lut (.A(\s_op_fifo.buf[0].len [3]), .B(\s_op_fifo.buf[1].len [3]), 
         .C(\s_op_fifo.tail [0]), .Z(n7898)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5841_3_lut.init = 16'hcaca;
    FD1P3AX \s_op_fifo.buf_1..len_i0_i0  (.D(s_pos_0_3__N_670[0]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i0  (.D(s_pos_0_3__N_670[0]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i0 .GSR = "DISABLED";
    FD1S3AX \s_op_fifo.tail_i0_i0  (.D(n1[0]), .CK(mclk_c), .Q(\s_op_fifo.tail [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.tail_i0_i0 .GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i0 (.D(s_addr_r1[0]), .SP(mclk_c_enable_203), .CK(mclk_c), 
            .Q(s_rx_mem_addr[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i0.GSR = "ENABLED";
    FD1P3AX \s_op_fifo.buf_3..pos_i0_i0  (.D(n8876), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].pos [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..pos_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..msbf_402  (.D(\s_desc_data[31] ), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf_2_.msbf ));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..msbf_402 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i0  (.D(\s_op0.addr [0]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..pos_i0_i0  (.D(n8876), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].pos [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..pos_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i0  (.D(\s_op0.addr [0]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..pos_i0_i0  (.D(n8876), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].pos [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..pos_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i0  (.D(\s_op0.addr [0]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..pos_i0_i0  (.D(n8876), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].pos [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..pos_i0_i0 .GSR = "DISABLED";
    FD1S3AX s_count_r_0__416 (.D(n2427[0]), .CK(mclk_c), .Q(\s_count_r[0] )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_0__416.GSR = "ENABLED";
    FD1S3AX s_count_r_1__417 (.D(n2427[1]), .CK(mclk_c), .Q(s_count_r[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_1__417.GSR = "ENABLED";
    FD1S3AX s_count_r_2__418 (.D(n2427[2]), .CK(mclk_c), .Q(s_count_r_c[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_2__418.GSR = "ENABLED";
    FD1S3AX s_count_r_3__419 (.D(n2427[3]), .CK(mclk_c), .Q(s_count_r_c[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_3__419.GSR = "ENABLED";
    FD1S3AX s_count_r_4__420 (.D(n2427[4]), .CK(mclk_c), .Q(s_count_r_c[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_4__420.GSR = "ENABLED";
    LUT4 s_wrap_stb_I_0_2_lut_3_lut_4_lut (.A(n8775), .B(s_count_r_c[8]), 
         .C(n8804), .D(s_count_r_c[9]), .Z(mem_wr_N_1153)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_wrap_stb_I_0_2_lut_3_lut_4_lut.init = 16'hf0f2;
    FD1S3AX s_count_r_5__421 (.D(n2427[5]), .CK(mclk_c), .Q(s_count_r_c[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_5__421.GSR = "ENABLED";
    FD1S3AX s_count_r_6__422 (.D(n2427[6]), .CK(mclk_c), .Q(s_count_r_c[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_6__422.GSR = "ENABLED";
    FD1S3AX s_count_r_7__423 (.D(n2427[7]), .CK(mclk_c), .Q(s_count_r_c[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_7__423.GSR = "ENABLED";
    FD1S3AX s_count_r_8__424 (.D(n2427[8]), .CK(mclk_c), .Q(s_count_r_c[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_8__424.GSR = "ENABLED";
    FD1S3AX s_count_r_9__425 (.D(n2427[9]), .CK(mclk_c), .Q(s_count_r_c[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_9__425.GSR = "ENABLED";
    FD1S3AX s_pos_r1_0__426 (.D(n2429[0]), .CK(mclk_c), .Q(\bit_pos[0] )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(257[4] 267[11])
    defparam s_pos_r1_0__426.GSR = "ENABLED";
    FD1S3AX s_pos_r1_1__427 (.D(n2429[1]), .CK(mclk_c), .Q(bit_pos[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(257[4] 267[11])
    defparam s_pos_r1_1__427.GSR = "ENABLED";
    FD1S3AX s_pos_r1_2__428 (.D(n2429[2]), .CK(mclk_c), .Q(bit_pos_c[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(257[4] 267[11])
    defparam s_pos_r1_2__428.GSR = "ENABLED";
    FD1S3AX s_pos_r1_3__429 (.D(n2429[3]), .CK(mclk_c), .Q(bit_pos_c[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(257[4] 267[11])
    defparam s_pos_r1_3__429.GSR = "ENABLED";
    FD1S3AX s_addr_r1_0__430 (.D(n2428[0]), .CK(mclk_c), .Q(s_addr_r1[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_0__430.GSR = "ENABLED";
    FD1S3AX s_addr_r1_1__431 (.D(n2428[1]), .CK(mclk_c), .Q(s_addr_r1[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_1__431.GSR = "ENABLED";
    FD1S3AX s_addr_r1_2__432 (.D(n2428[2]), .CK(mclk_c), .Q(s_addr_r1[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_2__432.GSR = "ENABLED";
    FD1S3AX s_addr_r1_3__433 (.D(n2428[3]), .CK(mclk_c), .Q(s_addr_r1[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_3__433.GSR = "ENABLED";
    FD1S3AX s_addr_r1_4__434 (.D(n2428[4]), .CK(mclk_c), .Q(s_addr_r1[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_4__434.GSR = "ENABLED";
    FD1S3AX s_addr_r1_5__435 (.D(n2428[5]), .CK(mclk_c), .Q(s_addr_r1[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_5__435.GSR = "ENABLED";
    FD1S3AX s_addr_r1_6__436 (.D(n2428[6]), .CK(mclk_c), .Q(s_addr_r1[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_6__436.GSR = "ENABLED";
    LUT4 s_addr_r1_9__I_97_3_lut_rep_101_4_lut (.A(op_msbf), .B(n8779), 
         .C(s_addr_r1[4]), .D(s_addr_r1[5]), .Z(n8768)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_97_3_lut_rep_101_4_lut.init = 16'h4008;
    FD1S3AX s_addr_r1_7__437 (.D(n2428[7]), .CK(mclk_c), .Q(s_addr_r1[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_7__437.GSR = "ENABLED";
    LUT4 s_addr_r1_5__I_0_2_lut_4_lut (.A(op_msbf), .B(n8779), .C(s_addr_r1[4]), 
         .D(s_addr_r1[5]), .Z(s_addr_r1_5__N_868)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_5__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 bit_pos_2__bdd_4_lut (.A(bit_pos_c[2]), .B(n8823), .C(op_pos[2]), 
         .D(fifo_get), .Z(n2429[2])) /* synthesis lut_function=(A (B (C (D))+!B (C+!(D)))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam bit_pos_2__bdd_4_lut.init = 16'hf066;
    FD1S3AX s_addr_r1_8__438 (.D(n2428[8]), .CK(mclk_c), .Q(s_addr_r1[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_8__438.GSR = "ENABLED";
    FD1S3AX s_addr_r1_9__439 (.D(n2428[9]), .CK(mclk_c), .Q(s_addr_r1[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_9__439.GSR = "ENABLED";
    FD1P3AX \s_op_fifo.buf_3..msbf_398  (.D(\s_desc_data[31] ), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf_3_.msbf ));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..msbf_398 .GSR = "DISABLED";
    LUT4 i5839_3_lut (.A(\s_op_fifo.buf[2].len [4]), .B(\s_op_fifo.buf[3].len [4]), 
         .C(\s_op_fifo.tail [0]), .Z(n7896)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5839_3_lut.init = 16'hcaca;
    LUT4 i5838_3_lut (.A(\s_op_fifo.buf[0].len [4]), .B(\s_op_fifo.buf[1].len [4]), 
         .C(\s_op_fifo.tail [0]), .Z(n7895)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5838_3_lut.init = 16'hcaca;
    LUT4 i5980_3_lut (.A(\s_op_fifo.buf[2].pos [3]), .B(\s_op_fifo.buf[3].pos [3]), 
         .C(\s_op_fifo.tail [0]), .Z(n8037)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5980_3_lut.init = 16'hcaca;
    LUT4 i5836_3_lut (.A(\s_op_fifo.buf[2].len [5]), .B(\s_op_fifo.buf[3].len [5]), 
         .C(\s_op_fifo.tail [0]), .Z(n7893)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5836_3_lut.init = 16'hcaca;
    LUT4 i5835_3_lut (.A(\s_op_fifo.buf[0].len [5]), .B(\s_op_fifo.buf[1].len [5]), 
         .C(\s_op_fifo.tail [0]), .Z(n7892)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5835_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_3_lut_4_lut (.A(mclk_c_enable_203), .B(n8853), .C(s_rx_mem_wr_stb), 
         .D(n8850), .Z(mclk_c_enable_257)) /* synthesis lut_function=(A (B (C+(D))+!B (C))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut.init = 16'hf8f0;
    LUT4 i3264_2_lut_3_lut_4_lut (.A(mclk_c_enable_203), .B(n8853), .C(s_mtdo), 
         .D(n8850), .Z(mem_dout_15__N_739)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i3264_2_lut_3_lut_4_lut.init = 16'h8000;
    LUT4 i1_2_lut_3_lut_4_lut_adj_182 (.A(mclk_c_enable_203), .B(n8853), 
         .C(s_rx_mem_wr_stb), .D(n8851), .Z(mclk_c_enable_253)) /* synthesis lut_function=(A (B (C+!(D))+!B (C))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_182.init = 16'hf0f8;
    LUT4 i5833_3_lut (.A(\s_op_fifo.buf[2].len [6]), .B(\s_op_fifo.buf[3].len [6]), 
         .C(\s_op_fifo.tail [0]), .Z(n7890)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5833_3_lut.init = 16'hcaca;
    LUT4 i3260_2_lut_3_lut_4_lut (.A(mclk_c_enable_203), .B(n8853), .C(s_mtdo), 
         .D(n8851), .Z(mem_dout_15__N_755)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;
    defparam i3260_2_lut_3_lut_4_lut.init = 16'h0080;
    LUT4 i1_2_lut_3_lut_4_lut_adj_183 (.A(mclk_c_enable_203), .B(n8853), 
         .C(s_rx_mem_wr_stb), .D(n8852), .Z(mclk_c_enable_249)) /* synthesis lut_function=(A (B (C+!(D))+!B (C))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_183.init = 16'hf0f8;
    FD1P3AX s_rxbuf_i0 (.D(mem_dout_15__N_799), .SP(mclk_c_enable_79), .CK(mclk_c), 
            .Q(s_rx_mem_din[0])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i0.GSR = "ENABLED";
    FD1S3AX s_busy_r_393 (.D(n6), .CK(mclk_c), .Q(busy)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(157[3] 165[10])
    defparam s_busy_r_393.GSR = "ENABLED";
    LUT4 i5832_3_lut (.A(\s_op_fifo.buf[0].len [6]), .B(\s_op_fifo.buf[1].len [6]), 
         .C(\s_op_fifo.tail [0]), .Z(n7889)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5832_3_lut.init = 16'hcaca;
    LUT4 i5830_3_lut (.A(\s_op_fifo.buf[2].len [7]), .B(\s_op_fifo.buf[3].len [7]), 
         .C(\s_op_fifo.tail [0]), .Z(n7887)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5830_3_lut.init = 16'hcaca;
    LUT4 i5829_3_lut (.A(\s_op_fifo.buf[0].len [7]), .B(\s_op_fifo.buf[1].len [7]), 
         .C(\s_op_fifo.tail [0]), .Z(n7886)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5829_3_lut.init = 16'hcaca;
    LUT4 i3256_2_lut_3_lut_4_lut (.A(mclk_c_enable_203), .B(n8853), .C(s_mtdo), 
         .D(n8852), .Z(mem_dout_15__N_771)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;
    defparam i3256_2_lut_3_lut_4_lut.init = 16'h0080;
    LUT4 i5827_3_lut (.A(\s_op_fifo.buf[2].len [8]), .B(\s_op_fifo.buf[3].len [8]), 
         .C(\s_op_fifo.tail [0]), .Z(n7884)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5827_3_lut.init = 16'hcaca;
    LUT4 i5826_3_lut (.A(\s_op_fifo.buf[0].len [8]), .B(\s_op_fifo.buf[1].len [8]), 
         .C(\s_op_fifo.tail [0]), .Z(n7883)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5826_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_3_lut_4_lut_adj_184 (.A(mclk_c_enable_203), .B(n8854), 
         .C(s_rx_mem_wr_stb), .D(n8850), .Z(mclk_c_enable_256)) /* synthesis lut_function=(A (B (C)+!B (C+(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_184.init = 16'hf2f0;
    LUT4 i5824_3_lut (.A(\s_op_fifo.buf[2].len [9]), .B(\s_op_fifo.buf[3].len [9]), 
         .C(\s_op_fifo.tail [0]), .Z(n7881)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5824_3_lut.init = 16'hcaca;
    LUT4 i3263_2_lut_3_lut_4_lut (.A(mclk_c_enable_203), .B(n8854), .C(s_mtdo), 
         .D(n8850), .Z(mem_dout_15__N_743)) /* synthesis lut_function=(!((B+!(C (D)))+!A)) */ ;
    defparam i3263_2_lut_3_lut_4_lut.init = 16'h2000;
    LUT4 i1_2_lut_3_lut_4_lut_adj_185 (.A(mclk_c_enable_203), .B(n8854), 
         .C(s_rx_mem_wr_stb), .D(n8851), .Z(mclk_c_enable_252)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_185.init = 16'hf0f2;
    LUT4 i5823_3_lut (.A(\s_op_fifo.buf[0].len [9]), .B(\s_op_fifo.buf[1].len [9]), 
         .C(\s_op_fifo.tail [0]), .Z(n7880)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5823_3_lut.init = 16'hcaca;
    LUT4 i3259_2_lut_3_lut_4_lut (.A(mclk_c_enable_203), .B(n8854), .C(s_mtdo), 
         .D(n8851), .Z(mem_dout_15__N_759)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i3259_2_lut_3_lut_4_lut.init = 16'h0020;
    LUT4 i1_2_lut_3_lut_4_lut_adj_186 (.A(mclk_c_enable_203), .B(n8854), 
         .C(s_rx_mem_wr_stb), .D(n8852), .Z(mclk_c_enable_248)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_186.init = 16'hf0f2;
    LUT4 i5821_3_lut (.A(\s_op_fifo.buf[2].addr [1]), .B(\s_op_fifo.buf[3].addr [1]), 
         .C(\s_op_fifo.tail [0]), .Z(n7878)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5821_3_lut.init = 16'hcaca;
    LUT4 i5820_3_lut (.A(\s_op_fifo.buf[0].addr [1]), .B(\s_op_fifo.buf[1].addr [1]), 
         .C(\s_op_fifo.tail [0]), .Z(n7877)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5820_3_lut.init = 16'hcaca;
    LUT4 s_count_r_9__N_881_I_0_2_lut_rep_97_3_lut_4_lut (.A(n8782), .B(s_count_r_c[7]), 
         .C(s_count_r_c[9]), .D(s_count_r_c[8]), .Z(n8764)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__N_881_I_0_2_lut_rep_97_3_lut_4_lut.init = 16'h0002;
    LUT4 s_count_r_9__I_0_529_2_lut_3_lut_4_lut (.A(n8782), .B(s_count_r_c[7]), 
         .C(s_count_r_c[9]), .D(s_count_r_c[8]), .Z(s_count_r_9__N_880)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_0_529_2_lut_3_lut_4_lut.init = 16'hf0d2;
    LUT4 i3255_2_lut_3_lut_4_lut (.A(mclk_c_enable_203), .B(n8854), .C(s_mtdo), 
         .D(n8852), .Z(mem_dout_15__N_775)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i3255_2_lut_3_lut_4_lut.init = 16'h0020;
    LUT4 i1_2_lut_3_lut_4_lut_adj_187 (.A(mclk_c_enable_203), .B(n8855), 
         .C(s_rx_mem_wr_stb), .D(n8850), .Z(mclk_c_enable_255)) /* synthesis lut_function=(A (B (C)+!B (C+(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_187.init = 16'hf2f0;
    LUT4 i3262_2_lut_3_lut_4_lut (.A(mclk_c_enable_203), .B(n8855), .C(s_mtdo), 
         .D(n8850), .Z(mem_dout_15__N_747)) /* synthesis lut_function=(!((B+!(C (D)))+!A)) */ ;
    defparam i3262_2_lut_3_lut_4_lut.init = 16'h2000;
    LUT4 i1_2_lut_3_lut_4_lut_adj_188 (.A(mclk_c_enable_203), .B(n8855), 
         .C(s_rx_mem_wr_stb), .D(n8851), .Z(mclk_c_enable_251)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_188.init = 16'hf0f2;
    LUT4 i3258_2_lut_3_lut_4_lut (.A(mclk_c_enable_203), .B(n8855), .C(s_mtdo), 
         .D(n8851), .Z(mem_dout_15__N_763)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i3258_2_lut_3_lut_4_lut.init = 16'h0020;
    LUT4 i1_2_lut_3_lut_4_lut_adj_189 (.A(mclk_c_enable_203), .B(n8855), 
         .C(s_rx_mem_wr_stb), .D(n8852), .Z(mclk_c_enable_247)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_189.init = 16'hf0f2;
    LUT4 i3254_2_lut_3_lut_4_lut (.A(mclk_c_enable_203), .B(n8855), .C(s_mtdo), 
         .D(n8852), .Z(mem_dout_15__N_779)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i3254_2_lut_3_lut_4_lut.init = 16'h0020;
    LUT4 i1_2_lut_3_lut_4_lut_adj_190 (.A(mclk_c_enable_203), .B(n8857), 
         .C(s_rx_mem_wr_stb), .D(n8853), .Z(mclk_c_enable_245)) /* synthesis lut_function=(A (B (C)+!B (C+(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_190.init = 16'hf2f0;
    LUT4 i3252_2_lut_3_lut_4_lut (.A(mclk_c_enable_203), .B(n8857), .C(s_mtdo), 
         .D(n8853), .Z(mem_dout_15__N_787)) /* synthesis lut_function=(!((B+!(C (D)))+!A)) */ ;
    defparam i3252_2_lut_3_lut_4_lut.init = 16'h2000;
    LUT4 i1_2_lut_3_lut_4_lut_adj_191 (.A(mclk_c_enable_203), .B(n8857), 
         .C(s_rx_mem_wr_stb), .D(n8854), .Z(mclk_c_enable_244)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_191.init = 16'hf0f2;
    LUT4 i5818_3_lut (.A(\s_op_fifo.buf[2].addr [2]), .B(\s_op_fifo.buf[3].addr [2]), 
         .C(\s_op_fifo.tail [0]), .Z(n7875)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5818_3_lut.init = 16'hcaca;
    LUT4 i5817_3_lut (.A(\s_op_fifo.buf[0].addr [2]), .B(\s_op_fifo.buf[1].addr [2]), 
         .C(\s_op_fifo.tail [0]), .Z(n7874)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5817_3_lut.init = 16'hcaca;
    LUT4 i5979_3_lut (.A(\s_op_fifo.buf[0].pos [3]), .B(\s_op_fifo.buf[1].pos [3]), 
         .C(\s_op_fifo.tail [0]), .Z(n8036)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5979_3_lut.init = 16'hcaca;
    LUT4 i3251_2_lut_3_lut_4_lut (.A(mclk_c_enable_203), .B(n8857), .C(s_mtdo), 
         .D(n8854), .Z(mem_dout_15__N_791)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i3251_2_lut_3_lut_4_lut.init = 16'h0020;
    LUT4 i1_2_lut_3_lut_4_lut_adj_192 (.A(mclk_c_enable_203), .B(n8857), 
         .C(s_rx_mem_wr_stb), .D(n8855), .Z(mclk_c_enable_243)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_192.init = 16'hf0f2;
    LUT4 i3250_2_lut_3_lut_4_lut (.A(mclk_c_enable_203), .B(n8857), .C(s_mtdo), 
         .D(n8855), .Z(mem_dout_15__N_795)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i3250_2_lut_3_lut_4_lut.init = 16'h0020;
    LUT4 s_count_r_9__I_109_2_lut_rep_144_3_lut_4_lut (.A(mclk_c_enable_203), 
         .B(\s_count_r[0] ), .C(s_count_r_c[2]), .D(s_count_r[1]), .Z(n8811)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_109_2_lut_rep_144_3_lut_4_lut.init = 16'h0002;
    PFUMX i5975 (.BLUT(n8030), .ALUT(n8031), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[5]));
    LUT4 i5977_3_lut (.A(\s_op_fifo.buf[2].addr [4]), .B(\s_op_fifo.buf[3].addr [4]), 
         .C(\s_op_fifo.tail [0]), .Z(n8034)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5977_3_lut.init = 16'hcaca;
    LUT4 i5976_3_lut (.A(\s_op_fifo.buf[0].addr [4]), .B(\s_op_fifo.buf[1].addr [4]), 
         .C(\s_op_fifo.tail [0]), .Z(n8033)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5976_3_lut.init = 16'hcaca;
    LUT4 i5974_3_lut (.A(\s_op_fifo.buf[2].addr [5]), .B(\s_op_fifo.buf[3].addr [5]), 
         .C(\s_op_fifo.tail [0]), .Z(n8031)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5974_3_lut.init = 16'hcaca;
    CCU2D add_519_11 (.A0(\s_desc_data[29] ), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n7044), .S0(\s_op0.addr [9]));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(146[16] 148[53])
    defparam add_519_11.INIT0 = 16'h5aaa;
    defparam add_519_11.INIT1 = 16'h0000;
    defparam add_519_11.INJECT1_0 = "NO";
    defparam add_519_11.INJECT1_1 = "NO";
    LUT4 bit_pos_3__I_92_3_lut_rep_156_4_lut (.A(op_msbf), .B(mclk_c_enable_203), 
         .C(\bit_pos[0] ), .D(bit_pos[1]), .Z(n8823)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam bit_pos_3__I_92_3_lut_rep_156_4_lut.init = 16'h4008;
    LUT4 s_addr_r1_9__I_98_3_lut_rep_105_4_lut (.A(op_msbf), .B(n8786), 
         .C(s_addr_r1[3]), .D(s_addr_r1[4]), .Z(n8772)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_98_3_lut_rep_105_4_lut.init = 16'h4008;
    LUT4 s_addr_r1_4__I_0_2_lut_4_lut (.A(op_msbf), .B(n8786), .C(s_addr_r1[3]), 
         .D(s_addr_r1[4]), .Z(s_addr_r1_4__N_870)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_4__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 bit_pos_1__I_0_2_lut_4_lut (.A(op_msbf), .B(mclk_c_enable_203), 
         .C(\bit_pos[0] ), .D(bit_pos[1]), .Z(bit_pos_1__N_814)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam bit_pos_1__I_0_2_lut_4_lut.init = 16'hb748;
    FD1P3AX \s_op_fifo.head_FSM_i0_i1  (.D(\s_op_fifo.buf_3_.msbf_N_1174 [0]), 
            .SP(mclk_c_enable_112), .CK(mclk_c), .Q(\s_op_fifo.buf_3_.msbf_N_1174 [1]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam \s_op_fifo.head_FSM_i0_i1 .GSR = "ENABLED";
    FD1P3AX \s_op_fifo.head_FSM_i0_i2  (.D(\s_op_fifo.buf_3_.msbf_N_1174 [1]), 
            .SP(mclk_c_enable_112), .CK(mclk_c), .Q(\s_op_fifo.buf_3_.msbf_N_1174 [2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam \s_op_fifo.head_FSM_i0_i2 .GSR = "ENABLED";
    FD1P3AX \s_op_fifo.head_FSM_i0_i3  (.D(\s_op_fifo.buf_3_.msbf_N_1174 [2]), 
            .SP(mclk_c_enable_112), .CK(mclk_c), .Q(\s_op_fifo.buf_3_.msbf_N_1174 [3]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam \s_op_fifo.head_FSM_i0_i3 .GSR = "ENABLED";
    LUT4 s_count_r_8__I_0_518_2_lut_3_lut_4_lut (.A(n8787), .B(s_count_r_c[6]), 
         .C(s_count_r_c[8]), .D(s_count_r_c[7]), .Z(s_count_r_8__N_925)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_8__I_0_518_2_lut_3_lut_4_lut.init = 16'hf0d2;
    CCU2D add_519_9 (.A0(\s_desc_data[27] ), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(\s_desc_data[28] ), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7043), .COUT(n7044), .S0(\s_op0.addr [7]), 
          .S1(\s_op0.addr [8]));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(146[16] 148[53])
    defparam add_519_9.INIT0 = 16'h5aaa;
    defparam add_519_9.INIT1 = 16'h5aaa;
    defparam add_519_9.INJECT1_0 = "NO";
    defparam add_519_9.INJECT1_1 = "NO";
    LUT4 i5973_3_lut (.A(\s_op_fifo.buf[0].addr [5]), .B(\s_op_fifo.buf[1].addr [5]), 
         .C(\s_op_fifo.tail [0]), .Z(n8030)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5973_3_lut.init = 16'hcaca;
    FD1P3AX \s_op_fifo.buf_3..len_i0_i1  (.D(s_pos_0_3__N_670[1]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i2  (.D(s_pos_0_3__N_670[2]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i3  (.D(s_pos_0_3__N_670[3]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i4  (.D(\s_desc_data[4] ), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i5  (.D(\s_desc_data[5] ), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i6  (.D(\s_desc_data[6] ), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i7  (.D(\s_desc_data[7] ), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i8  (.D(\s_desc_data[8] ), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i9  (.D(\s_desc_data[9] ), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i1  (.D(\s_op0.addr [1]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i1 .GSR = "DISABLED";
    LUT4 i3227_2_lut_rep_183 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), .Z(n8850)) /* synthesis lut_function=(A (B)) */ ;
    defparam i3227_2_lut_rep_183.init = 16'h8888;
    LUT4 i1_2_lut_3_lut_4_lut_adj_193 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .C(s_rx_mem_wr_stb), .D(n8835), .Z(mclk_c_enable_254)) /* synthesis lut_function=(A (B (C+(D))+!B (C))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_193.init = 16'hf8f0;
    LUT4 i3261_2_lut_3_lut_4_lut (.A(bit_pos_c[2]), .B(bit_pos_c[3]), .C(s_mtdo), 
         .D(n8835), .Z(mem_dout_15__N_751)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i3261_2_lut_3_lut_4_lut.init = 16'h8000;
    PFUMX i5978 (.BLUT(n8033), .ALUT(n8034), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[4]));
    LUT4 bit_pos_3__I_0_457_i6_2_lut_rep_184 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .Z(n8851)) /* synthesis lut_function=(A+!(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam bit_pos_3__I_0_457_i6_2_lut_rep_184.init = 16'hbbbb;
    LUT4 i5815_3_lut (.A(\s_op_fifo.buf[2].addr [3]), .B(\s_op_fifo.buf[3].addr [3]), 
         .C(\s_op_fifo.tail [0]), .Z(n7872)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5815_3_lut.init = 16'hcaca;
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i2  (.D(\s_op0.addr [2]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i3  (.D(\s_op0.addr [3]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i4  (.D(\s_op0.addr [4]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i5  (.D(\s_op0.addr [5]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i6  (.D(\s_op0.addr [6]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i7  (.D(\s_op0.addr [7]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i8  (.D(\s_op0.addr [8]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i9  (.D(\s_op0.addr [9]), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i1  (.D(s_pos_0_3__N_670[1]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i2  (.D(s_pos_0_3__N_670[2]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i3  (.D(s_pos_0_3__N_670[3]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i4  (.D(\s_desc_data[4] ), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i5  (.D(\s_desc_data[5] ), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i6  (.D(\s_desc_data[6] ), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i7  (.D(\s_desc_data[7] ), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i8  (.D(\s_desc_data[8] ), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i9  (.D(\s_desc_data[9] ), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i1  (.D(s_pos_0_3__N_670[1]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i1 .GSR = "DISABLED";
    LUT4 s_addr_r1_9__I_99_3_lut_rep_112_4_lut (.A(op_msbf), .B(n8789), 
         .C(s_addr_r1[2]), .D(s_addr_r1[3]), .Z(n8779)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_99_3_lut_rep_112_4_lut.init = 16'h4008;
    LUT4 s_addr_r1_3__I_0_2_lut_4_lut (.A(op_msbf), .B(n8789), .C(s_addr_r1[2]), 
         .D(s_addr_r1[3]), .Z(s_addr_r1_3__N_872)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_3__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 i1_2_lut_3_lut_4_lut_adj_194 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .C(s_rx_mem_wr_stb), .D(n8835), .Z(mclk_c_enable_250)) /* synthesis lut_function=(A (C)+!A (B (C+(D))+!B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam i1_2_lut_3_lut_4_lut_adj_194.init = 16'hf4f0;
    FD1P3AX \s_op_fifo.buf_1..len_i0_i2  (.D(s_pos_0_3__N_670[2]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i3  (.D(s_pos_0_3__N_670[3]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i4  (.D(\s_desc_data[4] ), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i5  (.D(\s_desc_data[5] ), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i6  (.D(\s_desc_data[6] ), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i7  (.D(\s_desc_data[7] ), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i8  (.D(\s_desc_data[8] ), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i9  (.D(\s_desc_data[9] ), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i1  (.D(s_pos_0_3__N_670[1]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i2  (.D(s_pos_0_3__N_670[2]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i3  (.D(s_pos_0_3__N_670[3]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i4  (.D(\s_desc_data[4] ), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i5  (.D(\s_desc_data[5] ), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i6  (.D(\s_desc_data[6] ), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i7  (.D(\s_desc_data[7] ), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i8  (.D(\s_desc_data[8] ), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i9  (.D(\s_desc_data[9] ), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.tail_i0_i1  (.D(\s_op_fifo.tail_1__N_1054 [1]), .SP(fifo_get), 
            .CK(mclk_c), .Q(\s_op_fifo.tail [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.tail_i0_i1 .GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i1 (.D(s_addr_r1[1]), .SP(mclk_c_enable_203), .CK(mclk_c), 
            .Q(s_rx_mem_addr[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i1.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i2 (.D(s_addr_r1[2]), .SP(mclk_c_enable_203), .CK(mclk_c), 
            .Q(s_rx_mem_addr[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i2.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i3 (.D(s_addr_r1[3]), .SP(mclk_c_enable_203), .CK(mclk_c), 
            .Q(s_rx_mem_addr[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i3.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i4 (.D(s_addr_r1[4]), .SP(mclk_c_enable_203), .CK(mclk_c), 
            .Q(s_rx_mem_addr[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i4.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i5 (.D(s_addr_r1[5]), .SP(mclk_c_enable_203), .CK(mclk_c), 
            .Q(s_rx_mem_addr[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i5.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i6 (.D(s_addr_r1[6]), .SP(mclk_c_enable_203), .CK(mclk_c), 
            .Q(s_rx_mem_addr[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i6.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i7 (.D(s_addr_r1[7]), .SP(mclk_c_enable_203), .CK(mclk_c), 
            .Q(s_rx_mem_addr[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i7.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i8 (.D(s_addr_r1[8]), .SP(mclk_c_enable_203), .CK(mclk_c), 
            .Q(s_rx_mem_addr[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i8.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i9 (.D(s_addr_r1[9]), .SP(mclk_c_enable_203), .CK(mclk_c), 
            .Q(s_rx_mem_addr[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i9.GSR = "ENABLED";
    FD1P3AX \s_op_fifo.buf_3..pos_i0_i1  (.D(n8880), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].pos [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..pos_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..pos_i0_i2  (.D(n8881), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].pos [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..pos_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..pos_i0_i3  (.D(n8882), .SP(mclk_c_enable_206), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].pos [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..pos_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i1  (.D(\s_op0.addr [1]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i2  (.D(\s_op0.addr [2]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i3  (.D(\s_op0.addr [3]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i4  (.D(\s_op0.addr [4]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i5  (.D(\s_op0.addr [5]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i6  (.D(\s_op0.addr [6]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i7  (.D(\s_op0.addr [7]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i8  (.D(\s_op0.addr [8]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i9  (.D(\s_op0.addr [9]), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..pos_i0_i1  (.D(n8880), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].pos [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..pos_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..pos_i0_i2  (.D(n8881), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].pos [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..pos_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..pos_i0_i3  (.D(n8882), .SP(mclk_c_enable_218), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].pos [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..pos_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i1  (.D(\s_op0.addr [1]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i2  (.D(\s_op0.addr [2]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i3  (.D(\s_op0.addr [3]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i4  (.D(\s_op0.addr [4]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i5  (.D(\s_op0.addr [5]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i6  (.D(\s_op0.addr [6]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i7  (.D(\s_op0.addr [7]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i8  (.D(\s_op0.addr [8]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i9  (.D(\s_op0.addr [9]), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..pos_i0_i1  (.D(n8880), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].pos [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..pos_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..pos_i0_i2  (.D(n8881), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].pos [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..pos_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..pos_i0_i3  (.D(n8882), .SP(mclk_c_enable_230), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].pos [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..pos_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i1  (.D(\s_op0.addr [1]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i2  (.D(\s_op0.addr [2]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i3  (.D(\s_op0.addr [3]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i4  (.D(\s_op0.addr [4]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i5  (.D(\s_op0.addr [5]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i6  (.D(\s_op0.addr [6]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i7  (.D(\s_op0.addr [7]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i8  (.D(\s_op0.addr [8]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i9  (.D(\s_op0.addr [9]), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..pos_i0_i1  (.D(n8880), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].pos [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..pos_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..pos_i0_i2  (.D(n8881), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].pos [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..pos_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..pos_i0_i3  (.D(n8882), .SP(mclk_c_enable_242), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].pos [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..pos_i0_i3 .GSR = "DISABLED";
    FD1P3AX s_rxbuf_i1 (.D(mem_dout_15__N_795), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(s_rx_mem_din[1])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i1.GSR = "ENABLED";
    LUT4 i3257_2_lut_3_lut_4_lut (.A(bit_pos_c[2]), .B(bit_pos_c[3]), .C(s_mtdo), 
         .D(n8835), .Z(mem_dout_15__N_767)) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam i3257_2_lut_3_lut_4_lut.init = 16'h4000;
    LUT4 bit_pos_3__I_0_465_i6_2_lut_rep_185 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .Z(n8852)) /* synthesis lut_function=((B)+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam bit_pos_3__I_0_465_i6_2_lut_rep_185.init = 16'hdddd;
    LUT4 vec_msbf_I_0_3_lut (.A(\s_desc_data[31] ), .B(\s_rx_op.msbf_N_1187 ), 
         .C(busy), .Z(\s_rx_op.msbf )) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_msbf_I_0_3_lut.init = 16'hcaca;
    LUT4 s_count_r_9__I_104_2_lut_rep_108_3_lut_4_lut (.A(n8792), .B(s_count_r_c[5]), 
         .C(s_count_r_c[7]), .D(s_count_r_c[6]), .Z(n8775)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_104_2_lut_rep_108_3_lut_4_lut.init = 16'h0002;
    LUT4 s_count_r_7__I_0_516_2_lut_3_lut_4_lut (.A(n8792), .B(s_count_r_c[5]), 
         .C(s_count_r_c[7]), .D(s_count_r_c[6]), .Z(s_count_r_7__N_921)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_7__I_0_516_2_lut_3_lut_4_lut.init = 16'hf0d2;
    LUT4 i1_2_lut_3_lut_4_lut_adj_195 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .C(s_rx_mem_wr_stb), .D(n8835), .Z(mclk_c_enable_246)) /* synthesis lut_function=(A (B (C)+!B (C+(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam i1_2_lut_3_lut_4_lut_adj_195.init = 16'hf2f0;
    LUT4 i3253_2_lut_3_lut_4_lut (.A(bit_pos_c[2]), .B(bit_pos_c[3]), .C(s_mtdo), 
         .D(n8835), .Z(mem_dout_15__N_783)) /* synthesis lut_function=(!((B+!(C (D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam i3253_2_lut_3_lut_4_lut.init = 16'h2000;
    LUT4 i1_2_lut_rep_186 (.A(\bit_pos[0] ), .B(bit_pos[1]), .Z(n8853)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_rep_186.init = 16'h8888;
    LUT4 bit_pos_3__I_0_471_i5_2_lut_rep_187 (.A(\bit_pos[0] ), .B(bit_pos[1]), 
         .Z(n8854)) /* synthesis lut_function=(A+!(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam bit_pos_3__I_0_471_i5_2_lut_rep_187.init = 16'hbbbb;
    LUT4 s_addr_r1_9__I_100_3_lut_rep_119_4_lut (.A(op_msbf), .B(n8794), 
         .C(s_addr_r1[1]), .D(s_addr_r1[2]), .Z(n8786)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_100_3_lut_rep_119_4_lut.init = 16'h4008;
    LUT4 s_addr_r1_2__I_0_2_lut_4_lut (.A(op_msbf), .B(n8794), .C(s_addr_r1[1]), 
         .D(s_addr_r1[2]), .Z(s_addr_r1_2__N_874)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_2__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 bit_pos_3__I_0_465_i5_2_lut_rep_188 (.A(\bit_pos[0] ), .B(bit_pos[1]), 
         .Z(n8855)) /* synthesis lut_function=((B)+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam bit_pos_3__I_0_465_i5_2_lut_rep_188.init = 16'hdddd;
    FD1P3AX s_rxbuf_i2 (.D(mem_dout_15__N_791), .SP(mclk_c_enable_244), 
            .CK(mclk_c), .Q(s_rx_mem_din[2])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i2.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i3 (.D(mem_dout_15__N_787), .SP(mclk_c_enable_245), 
            .CK(mclk_c), .Q(s_rx_mem_din[3])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i3.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i4 (.D(mem_dout_15__N_783), .SP(mclk_c_enable_246), 
            .CK(mclk_c), .Q(s_rx_mem_din[4])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i4.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i5 (.D(mem_dout_15__N_779), .SP(mclk_c_enable_247), 
            .CK(mclk_c), .Q(s_rx_mem_din[5])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i5.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i6 (.D(mem_dout_15__N_775), .SP(mclk_c_enable_248), 
            .CK(mclk_c), .Q(s_rx_mem_din[6])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i6.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i7 (.D(mem_dout_15__N_771), .SP(mclk_c_enable_249), 
            .CK(mclk_c), .Q(s_rx_mem_din[7])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i7.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i8 (.D(mem_dout_15__N_767), .SP(mclk_c_enable_250), 
            .CK(mclk_c), .Q(s_rx_mem_din[8])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i8.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i9 (.D(mem_dout_15__N_763), .SP(mclk_c_enable_251), 
            .CK(mclk_c), .Q(s_rx_mem_din[9])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i9.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i10 (.D(mem_dout_15__N_759), .SP(mclk_c_enable_252), 
            .CK(mclk_c), .Q(s_rx_mem_din[10])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i10.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i11 (.D(mem_dout_15__N_755), .SP(mclk_c_enable_253), 
            .CK(mclk_c), .Q(s_rx_mem_din[11])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i11.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i12 (.D(mem_dout_15__N_751), .SP(mclk_c_enable_254), 
            .CK(mclk_c), .Q(s_rx_mem_din[12])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i12.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i13 (.D(mem_dout_15__N_747), .SP(mclk_c_enable_255), 
            .CK(mclk_c), .Q(s_rx_mem_din[13])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i13.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i14 (.D(mem_dout_15__N_743), .SP(mclk_c_enable_256), 
            .CK(mclk_c), .Q(s_rx_mem_din[14])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i14.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i15 (.D(mem_dout_15__N_739), .SP(mclk_c_enable_257), 
            .CK(mclk_c), .Q(s_rx_mem_din[15])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i15.GSR = "ENABLED";
    LUT4 start_I_0_4_lut (.A(start), .B(n8764), .C(\s_op_fifo.empty ), 
         .D(busy), .Z(fifo_get)) /* synthesis lut_function=(A+!((C+!(D))+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(180[16:75])
    defparam start_I_0_4_lut.init = 16'haeaa;
    PFUMX i5981 (.BLUT(n8036), .ALUT(n8037), .C0(\s_op_fifo.tail [1]), 
          .Z(op_pos_3__N_837[3]));
    LUT4 s_addr_r1_8__I_0_2_lut_4_lut (.A(op_msbf), .B(n8763), .C(s_addr_r1[7]), 
         .D(s_addr_r1[8]), .Z(s_addr_r1_8__N_862)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_8__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 bit_pos_3__I_0_474_i6_2_lut_rep_190 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .Z(n8857)) /* synthesis lut_function=(A+(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam bit_pos_3__I_0_474_i6_2_lut_rep_190.init = 16'heeee;
    LUT4 i6258_4_lut (.A(fifo_get), .B(\s_op_fifo.tail [0]), .C(\s_op_fifo.head [0]), 
         .D(n4_adj_1591), .Z(n7177)) /* synthesis lut_function=(!((B (C+!(D))+!B ((D)+!C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(201[4] 206[11])
    defparam i6258_4_lut.init = 16'h0820;
    LUT4 i3249_2_lut_3_lut_4_lut (.A(bit_pos_c[2]), .B(bit_pos_c[3]), .C(s_mtdo), 
         .D(n8835), .Z(mem_dout_15__N_799)) /* synthesis lut_function=(!(A+(B+!(C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam i3249_2_lut_3_lut_4_lut.init = 16'h1000;
    LUT4 i1_2_lut_3_lut_4_lut_adj_196 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .C(s_rx_mem_wr_stb), .D(n8835), .Z(mclk_c_enable_79)) /* synthesis lut_function=(A (C)+!A (B (C)+!B (C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam i1_2_lut_3_lut_4_lut_adj_196.init = 16'hf1f0;
    LUT4 i1_2_lut (.A(\s_op_fifo.buf_3_.msbf_N_1174 [1]), .B(\s_op_fifo.buf_3_.msbf_N_1174 [3]), 
         .Z(\s_op_fifo.head [0])) /* synthesis lut_function=(A+(B)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1_2_lut.init = 16'heeee;
    LUT4 i1_3_lut (.A(\s_op_fifo.tail [1]), .B(\s_op_fifo.buf_3_.msbf_N_1174 [2]), 
         .C(\s_op_fifo.buf_3_.msbf_N_1174 [3]), .Z(n4_adj_1591)) /* synthesis lut_function=(!(A (B+(C))+!A !(B+(C)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1_3_lut.init = 16'h5656;
    PFUMX i5819 (.BLUT(n7874), .ALUT(n7875), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[2]));
    CCU2D add_519_7 (.A0(\s_desc_data[9] ), .B0(\s_desc_data[31] ), .C0(\s_desc_data[25] ), 
          .D0(GND_net), .A1(\s_desc_data[26] ), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7042), .COUT(n7043), .S0(\s_op0.addr [5]), 
          .S1(\s_op0.addr [6]));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(146[16] 148[53])
    defparam add_519_7.INIT0 = 16'h7878;
    defparam add_519_7.INIT1 = 16'h5aaa;
    defparam add_519_7.INJECT1_0 = "NO";
    defparam add_519_7.INJECT1_1 = "NO";
    CCU2D add_519_5 (.A0(\s_desc_data[7] ), .B0(\s_desc_data[31] ), .C0(\s_desc_data[23] ), 
          .D0(GND_net), .A1(\s_desc_data[8] ), .B1(\s_desc_data[31] ), 
          .C1(\s_desc_data[24] ), .D1(GND_net), .CIN(n7041), .COUT(n7042), 
          .S0(\s_op0.addr [3]), .S1(\s_op0.addr [4]));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(146[16] 148[53])
    defparam add_519_5.INIT0 = 16'h7878;
    defparam add_519_5.INIT1 = 16'h7878;
    defparam add_519_5.INJECT1_0 = "NO";
    defparam add_519_5.INJECT1_1 = "NO";
    CCU2D add_519_3 (.A0(\s_desc_data[5] ), .B0(\s_desc_data[31] ), .C0(\s_desc_data[21] ), 
          .D0(GND_net), .A1(\s_desc_data[6] ), .B1(\s_desc_data[31] ), 
          .C1(\s_desc_data[22] ), .D1(GND_net), .CIN(n7040), .COUT(n7041), 
          .S0(\s_op0.addr [1]), .S1(\s_op0.addr [2]));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(146[16] 148[53])
    defparam add_519_3.INIT0 = 16'h7878;
    defparam add_519_3.INIT1 = 16'h7878;
    defparam add_519_3.INJECT1_0 = "NO";
    defparam add_519_3.INJECT1_1 = "NO";
    LUT4 s_count_r_9__I_105_2_lut_rep_115_3_lut_4_lut (.A(n8802), .B(s_count_r_c[4]), 
         .C(s_count_r_c[6]), .D(s_count_r_c[5]), .Z(n8782)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_105_2_lut_rep_115_3_lut_4_lut.init = 16'h0002;
    LUT4 s_count_r_6__I_0_514_2_lut_3_lut_4_lut (.A(n8802), .B(s_count_r_c[4]), 
         .C(s_count_r_c[6]), .D(s_count_r_c[5]), .Z(s_count_r_6__N_917)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_6__I_0_514_2_lut_3_lut_4_lut.init = 16'hf0d2;
    PFUMX i5822 (.BLUT(n7877), .ALUT(n7878), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[1]));
    PFUMX i5825 (.BLUT(n7880), .ALUT(n7881), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[9]));
    PFUMX i5828 (.BLUT(n7883), .ALUT(n7884), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[8]));
    PFUMX i5831 (.BLUT(n7886), .ALUT(n7887), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[7]));
    PFUMX i5834 (.BLUT(n7889), .ALUT(n7890), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[6]));
    LUT4 i2365_1_lut_rep_198 (.A(busy), .Z(n8865)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(157[3] 165[10])
    defparam i2365_1_lut_rep_198.init = 16'h5555;
    LUT4 s_addr_r1_9__I_101_3_lut_rep_122_4_lut (.A(op_msbf), .B(n8804), 
         .C(s_addr_r1[0]), .D(s_addr_r1[1]), .Z(n8789)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_101_3_lut_rep_122_4_lut.init = 16'h4008;
    LUT4 s_addr_r1_1__I_0_2_lut_4_lut (.A(op_msbf), .B(n8804), .C(s_addr_r1[0]), 
         .D(s_addr_r1[1]), .Z(s_addr_r1_1__N_876)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_1__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 i6269_4_lut_4_lut (.A(busy), .B(n8764), .C(\s_op_fifo.empty ), 
         .D(n9427), .Z(n6)) /* synthesis lut_function=(A (((D)+!C)+!B)+!A (D)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(157[3] 165[10])
    defparam i6269_4_lut_4_lut.init = 16'hff2a;
    PFUMX i5837 (.BLUT(n7892), .ALUT(n7893), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[5]));
    PFUMX i5840 (.BLUT(n7895), .ALUT(n7896), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[4]));
    CCU2D add_519_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(\s_desc_data[4] ), .B1(\s_desc_data[31] ), .C1(\s_desc_data[20] ), 
          .D1(GND_net), .COUT(n7040), .S1(\s_op0.addr [0]));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(146[16] 148[53])
    defparam add_519_1.INIT0 = 16'hF000;
    defparam add_519_1.INIT1 = 16'h7878;
    defparam add_519_1.INJECT1_0 = "NO";
    defparam add_519_1.INJECT1_1 = "NO";
    LUT4 s_count_r_9__I_106_2_lut_rep_120_3_lut_4_lut (.A(n8811), .B(s_count_r_c[3]), 
         .C(s_count_r_c[5]), .D(s_count_r_c[4]), .Z(n8787)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_106_2_lut_rep_120_3_lut_4_lut.init = 16'h0002;
    LUT4 i3235_2_lut_rep_209 (.A(s_pos_0_3__N_670[0]), .B(\s_desc_data[31] ), 
         .Z(n8876)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam i3235_2_lut_rep_209.init = 16'h8888;
    LUT4 s_count_r_5__I_0_512_2_lut_3_lut_4_lut (.A(n8811), .B(s_count_r_c[3]), 
         .C(s_count_r_c[5]), .D(s_count_r_c[4]), .Z(s_count_r_5__N_913)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_5__I_0_512_2_lut_3_lut_4_lut.init = 16'hf0d2;
    LUT4 \s_op0.pos_3__I_0_i1_3_lut_4_lut  (.A(s_pos_0_3__N_670[0]), .B(\s_desc_data[31] ), 
         .C(busy), .D(op_pos_3__N_837[0]), .Z(op_pos[0])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A (C (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam \s_op0.pos_3__I_0_i1_3_lut_4_lut .init = 16'hf808;
    LUT4 s_addr_r1_9__I_102_3_lut_rep_127_4_lut (.A(op_msbf), .B(n8812), 
         .C(bit_pos_c[3]), .D(s_addr_r1[0]), .Z(n8794)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam s_addr_r1_9__I_102_3_lut_rep_127_4_lut.init = 16'h4008;
    LUT4 s_addr_r1_0__I_0_2_lut_4_lut (.A(op_msbf), .B(n8812), .C(bit_pos_c[3]), 
         .D(s_addr_r1[0]), .Z(s_addr_r1_0__N_878)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam s_addr_r1_0__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 i3361_2_lut_rep_213 (.A(s_pos_0_3__N_670[1]), .B(\s_desc_data[31] ), 
         .Z(n8880)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam i3361_2_lut_rep_213.init = 16'h8888;
    LUT4 \s_op0.pos_3__I_0_i2_3_lut_4_lut  (.A(s_pos_0_3__N_670[1]), .B(\s_desc_data[31] ), 
         .C(busy), .D(op_pos_3__N_837[1]), .Z(op_pos[1])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A (C (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam \s_op0.pos_3__I_0_i2_3_lut_4_lut .init = 16'hf808;
    LUT4 i3362_2_lut_rep_214 (.A(s_pos_0_3__N_670[2]), .B(\s_desc_data[31] ), 
         .Z(n8881)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam i3362_2_lut_rep_214.init = 16'h8888;
    LUT4 \s_op0.pos_3__I_0_i3_3_lut_4_lut  (.A(s_pos_0_3__N_670[2]), .B(\s_desc_data[31] ), 
         .C(busy), .D(op_pos_3__N_837[2]), .Z(op_pos[2])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A (C (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam \s_op0.pos_3__I_0_i3_3_lut_4_lut .init = 16'hf808;
    LUT4 i3363_2_lut_rep_215 (.A(s_pos_0_3__N_670[3]), .B(\s_desc_data[31] ), 
         .Z(n8882)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam i3363_2_lut_rep_215.init = 16'h8888;
    LUT4 \s_op0.pos_3__I_0_i4_3_lut_4_lut  (.A(s_pos_0_3__N_670[3]), .B(\s_desc_data[31] ), 
         .C(busy), .D(op_pos_3__N_837[3]), .Z(op_pos[3])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A (C (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam \s_op0.pos_3__I_0_i4_3_lut_4_lut .init = 16'hf808;
    PFUMX i5843 (.BLUT(n7898), .ALUT(n7899), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[3]));
    PFUMX i5846 (.BLUT(n7901), .ALUT(n7902), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[2]));
    PFUMX i5849 (.BLUT(n7904), .ALUT(n7905), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[0]));
    PFUMX i5852 (.BLUT(n7907), .ALUT(n7908), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[0]));
    PFUMX i5855 (.BLUT(n7910), .ALUT(n7911), .C0(\s_op_fifo.tail [1]), 
          .Z(op_pos_3__N_837[0]));
    PFUMX i5858 (.BLUT(n7913), .ALUT(n7914), .C0(\s_op_fifo.tail [1]), 
          .Z(\s_rx_op.msbf_N_1187 ));
    LUT4 s_count_r_9__I_107_2_lut_rep_125_3_lut_4_lut (.A(n8822), .B(s_count_r_c[2]), 
         .C(s_count_r_c[4]), .D(s_count_r_c[3]), .Z(n8792)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_107_2_lut_rep_125_3_lut_4_lut.init = 16'h0002;
    LUT4 s_count_r_4__I_0_510_2_lut_3_lut_4_lut (.A(n8822), .B(s_count_r_c[2]), 
         .C(s_count_r_c[4]), .D(s_count_r_c[3]), .Z(s_count_r_4__N_909)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_4__I_0_510_2_lut_3_lut_4_lut.init = 16'hf0d2;
    LUT4 s_wrap_stb_I_115_3_lut_rep_137_4_lut (.A(op_msbf), .B(n8823), .C(bit_pos_c[2]), 
         .D(bit_pos_c[3]), .Z(n8804)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam s_wrap_stb_I_115_3_lut_rep_137_4_lut.init = 16'h4008;
    LUT4 bit_pos_3__I_0_2_lut_4_lut (.A(op_msbf), .B(n8823), .C(bit_pos_c[2]), 
         .D(bit_pos_c[3]), .Z(bit_pos_3__N_804)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam bit_pos_3__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 i1025_2_lut (.A(\s_op_fifo.tail [1]), .B(\s_op_fifo.tail [0]), 
         .Z(\s_op_fifo.tail_1__N_1054 [1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1025_2_lut.init = 16'h6666;
    LUT4 i5814_3_lut (.A(\s_op_fifo.buf[0].addr [3]), .B(\s_op_fifo.buf[1].addr [3]), 
         .C(\s_op_fifo.tail [0]), .Z(n7871)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5814_3_lut.init = 16'hcaca;
    PFUMX i5870 (.BLUT(n7925), .ALUT(n7926), .C0(\s_op_fifo.tail [1]), 
          .Z(op_pos_3__N_837[2]));
    PFUMX mux_512_i10 (.BLUT(s_addr_r1_9__N_842), .ALUT(op_addr[9]), .C0(fifo_get), 
          .Z(n2428[9]));
    LUT4 \s_op0.addr_9__I_0_i1_3_lut  (.A(\s_op0.addr [0]), .B(op_addr_9__N_817[0]), 
         .C(busy), .Z(op_addr[0])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i1_3_lut .init = 16'hcaca;
    PFUMX mux_512_i9 (.BLUT(s_addr_r1_8__N_862), .ALUT(op_addr[8]), .C0(fifo_get), 
          .Z(n2428[8]));
    LUT4 vec_len_9__I_0_i1_3_lut (.A(s_pos_0_3__N_670[0]), .B(op_len_9__N_827[0]), 
         .C(busy), .Z(op_len[0])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_len_9__I_0_i1_3_lut.init = 16'hcaca;
    PFUMX i5897 (.BLUT(n7952), .ALUT(n7953), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[1]));
    PFUMX mux_511_i10 (.BLUT(s_count_r_9__N_880), .ALUT(op_len[9]), .C0(fifo_get), 
          .Z(n2427[9]));
    PFUMX mux_511_i9 (.BLUT(s_count_r_8__N_925), .ALUT(op_len[8]), .C0(fifo_get), 
          .Z(n2427[8]));
    PFUMX mux_511_i8 (.BLUT(s_count_r_7__N_921), .ALUT(op_len[7]), .C0(fifo_get), 
          .Z(n2427[7]));
    LUT4 \s_op0.addr_9__I_0_i2_3_lut  (.A(\s_op0.addr [1]), .B(op_addr_9__N_817[1]), 
         .C(busy), .Z(op_addr[1])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i2_3_lut .init = 16'hcaca;
    PFUMX mux_511_i7 (.BLUT(s_count_r_6__N_917), .ALUT(op_len[6]), .C0(fifo_get), 
          .Z(n2427[6]));
    PFUMX mux_511_i6 (.BLUT(s_count_r_5__N_913), .ALUT(op_len[5]), .C0(fifo_get), 
          .Z(n2427[5]));
    LUT4 i5968_3_lut (.A(\s_op_fifo.buf[2].addr [6]), .B(\s_op_fifo.buf[3].addr [6]), 
         .C(\s_op_fifo.tail [0]), .Z(n8025)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5968_3_lut.init = 16'hcaca;
    PFUMX mux_511_i5 (.BLUT(s_count_r_4__N_909), .ALUT(op_len[4]), .C0(fifo_get), 
          .Z(n2427[4]));
    PFUMX mux_511_i4 (.BLUT(s_count_r_3__N_905), .ALUT(op_len[3]), .C0(fifo_get), 
          .Z(n2427[3]));
    LUT4 i5967_3_lut (.A(\s_op_fifo.buf[0].addr [6]), .B(\s_op_fifo.buf[1].addr [6]), 
         .C(\s_op_fifo.tail [0]), .Z(n8024)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5967_3_lut.init = 16'hcaca;
    LUT4 i5965_3_lut (.A(\s_op_fifo.buf[2].addr [7]), .B(\s_op_fifo.buf[3].addr [7]), 
         .C(\s_op_fifo.tail [0]), .Z(n8022)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5965_3_lut.init = 16'hcaca;
    PFUMX mux_511_i2 (.BLUT(s_count_r_1__N_897), .ALUT(op_len[1]), .C0(fifo_get), 
          .Z(n2427[1]));
    PFUMX mux_512_i8 (.BLUT(s_addr_r1_7__N_864), .ALUT(op_addr[7]), .C0(fifo_get), 
          .Z(n2428[7]));
    LUT4 \s_op0.addr_9__I_0_i3_3_lut  (.A(\s_op0.addr [2]), .B(op_addr_9__N_817[2]), 
         .C(busy), .Z(op_addr[2])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i3_3_lut .init = 16'hcaca;
    PFUMX mux_512_i7 (.BLUT(s_addr_r1_6__N_866), .ALUT(op_addr[6]), .C0(fifo_get), 
          .Z(n2428[6]));
    PFUMX mux_512_i6 (.BLUT(s_addr_r1_5__N_868), .ALUT(op_addr[5]), .C0(fifo_get), 
          .Z(n2428[5]));
    PFUMX mux_512_i5 (.BLUT(s_addr_r1_4__N_870), .ALUT(op_addr[4]), .C0(fifo_get), 
          .Z(n2428[4]));
    PFUMX i5944 (.BLUT(n7999), .ALUT(n8000), .C0(\s_op_fifo.tail [1]), 
          .Z(op_pos_3__N_837[1]));
    PFUMX i5950 (.BLUT(n8005), .ALUT(n8006), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[9]));
    PFUMX i5953 (.BLUT(n8008), .ALUT(n8009), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[8]));
    PFUMX mux_512_i4 (.BLUT(s_addr_r1_3__N_872), .ALUT(op_addr[3]), .C0(fifo_get), 
          .Z(n2428[3]));
    
endmodule
//
// Verilog Description of module jtag_enh_io
//

module jtag_enh_io (mclk_c_enable_104, tckn_stb, n9420, mclk_c, mclk_c_enable_81, 
            tp_tms_c, s_mtms, s_mtdo, s_tap_tdo, s_tap_tdi, n7643, 
            s_so);
    input mclk_c_enable_104;
    output tckn_stb;
    input n9420;
    input mclk_c;
    input mclk_c_enable_81;
    output tp_tms_c;
    input s_mtms;
    output s_mtdo;
    input s_tap_tdo;
    output s_tap_tdi;
    input n7643;
    input s_so;
    
    wire mclk_c_enable_104 /* synthesis is_clock=1, SET_AS_NETWORK=mclk_c_enable_104 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    
    wire s_tckn_r0_N_669;
    
    FD1S3DX s_tckn_r0_36 (.D(n9420), .CK(mclk_c_enable_104), .CD(tckn_stb), 
            .Q(s_tckn_r0_N_669)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=216, LSE_RLINE=216 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_io.vhdl(74[3] 78[10])
    defparam s_tckn_r0_36.GSR = "DISABLED";
    FD1P3AX s_tckn_r1_37 (.D(s_tckn_r0_N_669), .SP(mclk_c_enable_81), .CK(mclk_c), 
            .Q(tckn_stb));   // c:/devel/yard-ice/src/fpga/jtag_enh_io.vhdl(80[3] 84[10])
    defparam s_tckn_r1_37.GSR = "DISABLED";
    FD1P3AX s_tms_r0_41 (.D(s_mtms), .SP(mclk_c_enable_104), .CK(mclk_c), 
            .Q(tp_tms_c));   // c:/devel/yard-ice/src/fpga/jtag_enh_io.vhdl(112[3] 118[10])
    defparam s_tms_r0_41.GSR = "ENABLED";
    FD1P3AX s_tdo_r0_42 (.D(s_tap_tdo), .SP(mclk_c_enable_104), .CK(mclk_c), 
            .Q(s_mtdo));   // c:/devel/yard-ice/src/fpga/jtag_enh_io.vhdl(124[3] 128[10])
    defparam s_tdo_r0_42.GSR = "ENABLED";
    FD1P3IX s_tdi_r0_40 (.D(s_so), .SP(mclk_c_enable_104), .CD(n7643), 
            .CK(mclk_c), .Q(s_tap_tdi));   // c:/devel/yard-ice/src/fpga/jtag_enh_io.vhdl(112[3] 118[10])
    defparam s_tdi_r0_40.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module \jtag_enh_ecu(16,8,8) 
//

module \jtag_enh_ecu(16,8,8)  (mclk_c, GND_net, s_tcken, n9420, \s_status[2] , 
            \s_status[3] , \s_status[0] , \s_status[1] , s_end_state, 
            s_tdien, s_reg_din, s_mtms, n8820, n8832, s_en_r0, n8781, 
            n8765, \s_offs_r[1] , n2898, s_offs_inc, \s_offs_r[0] , 
            n2873, tckn_stb, s_en_r1, mclk_c_enable_129, s_tap_busy, 
            n9428, n8774, n8829, n8828, n8856, out_get_N_312, n7771, 
            n2, n5312, s_ptr_addr, s_cnt_en_N_1514, s_goto_state, 
            n1, n7781);
    input mclk_c;
    input GND_net;
    output s_tcken;
    input n9420;
    output \s_status[2] ;
    output \s_status[3] ;
    output \s_status[0] ;
    output \s_status[1] ;
    output [3:0]s_end_state;
    output s_tdien;
    input [15:0]s_reg_din;
    output s_mtms;
    output n8820;
    output n8832;
    input s_en_r0;
    output n8781;
    input n8765;
    input \s_offs_r[1] ;
    output n2898;
    input s_offs_inc;
    input \s_offs_r[0] ;
    output n2873;
    input tckn_stb;
    input s_en_r1;
    output mclk_c_enable_129;
    output s_tap_busy;
    input n9428;
    output n8774;
    output n8829;
    output n8828;
    output n8856;
    input out_get_N_312;
    input n7771;
    input n2;
    input n5312;
    output [8:1]s_ptr_addr;
    output s_cnt_en_N_1514;
    input [3:0]s_goto_state;
    output n1;
    output n7781;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    wire [7:0]s_cnt_reg;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(169[9:18])
    
    wire mclk_c_enable_102, busy_N_1509;
    wire [7:0]n333;
    
    wire n7066, n7065, n7064;
    wire [3:0]n514;
    wire [3:0]s_tap_state_next_3__N_1438;
    
    wire n7795;
    wire [3:0]s_tap_state_next;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(229[9:25])
    
    wire n7063;
    wire [3:0]n519;
    
    wire n7810;
    wire [3:0]s_tap_state_next_3__N_1461;
    
    wire n7799;
    wire [3:0]n504;
    wire [3:0]n509;
    
    wire n7812, mclk_c_enable_16, n2858;
    wire [3:0]s_drpause_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(204[9:24])
    
    wire n8818, n9414, n3933;
    wire [3:0]s_irpause_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(212[9:24])
    wire [15:0]mtms_N_1486;
    wire [15:0]s_insn_r0;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(188[9:18])
    
    wire n8816, n8895, n8894, n8883, n2554, n8884, n9418, n8897;
    wire [3:0]s_drscan_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(200[9:23])
    
    wire n5485, n8573, n182, insn_goto_N_1512, n9413, s_cnt_ovf, 
        n7184, n8580, n7201, n8104, n19, n20, s_pos_goto_en, tdi_en_N_1474, 
        n3519, s_pos_goto_en_N_1480;
    wire [3:0]s_irscan_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(208[9:23])
    
    wire n8808, n8291, n4405, mclk_c_enable_36, tdi_en_N_1472, n5526, 
        n7743, n9303, n9304, n9305, n9302, n8603, n8602, n8821, 
        n2494, n8579, n8576, n7710;
    wire [3:0]s_reset_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(220[9:22])
    
    wire s_exec_en, s_shift_en_r0, s_shift_en, n7745, n7719, n3503, 
        n7653, n4416, n8817, n8807, n8885, n126;
    wire [3:0]s_runtest_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(216[9:24])
    wire [0:0]n2503;
    
    wire n8575, n8800, n8796, n8577, n7804, n8801, n5, n4401, 
        n7785, n4, n8601, n7723;
    wire [10:0]n2050;
    
    wire n6962;
    wire [7:0]s_cnt_top;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(168[9:18])
    
    wire n6961, n5413, n3377, n8778, n4_adj_1590, n8293, n8294, 
        n8292, n8896, n3364;
    wire [3:0]s_goto_state_c;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(196[9:21])
    
    FD1P3IX s_cnt_reg__i0 (.D(n333[0]), .SP(mclk_c_enable_102), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i0.GSR = "ENABLED";
    CCU2D add_108_9 (.A0(s_cnt_reg[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n7066), 
          .S0(n333[7]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_108_9.INIT0 = 16'h5aaa;
    defparam add_108_9.INIT1 = 16'h0000;
    defparam add_108_9.INJECT1_0 = "NO";
    defparam add_108_9.INJECT1_1 = "NO";
    CCU2D add_108_7 (.A0(s_cnt_reg[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_cnt_reg[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n7065), .COUT(n7066), .S0(n333[5]), .S1(n333[6]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_108_7.INIT0 = 16'h5aaa;
    defparam add_108_7.INIT1 = 16'h5aaa;
    defparam add_108_7.INJECT1_0 = "NO";
    defparam add_108_7.INJECT1_1 = "NO";
    CCU2D add_108_5 (.A0(s_cnt_reg[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_cnt_reg[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n7064), .COUT(n7065), .S0(n333[3]), .S1(n333[4]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_108_5.INIT0 = 16'h5aaa;
    defparam add_108_5.INIT1 = 16'h5aaa;
    defparam add_108_5.INJECT1_0 = "NO";
    defparam add_108_5.INJECT1_1 = "NO";
    L6MUX21 s_tap_state_next_3__I_0_225_i1 (.D0(n514[0]), .D1(s_tap_state_next_3__N_1438[0]), 
            .SD(n7795), .Z(s_tap_state_next[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    CCU2D add_108_3 (.A0(s_cnt_reg[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_cnt_reg[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n7063), .COUT(n7064), .S0(n333[1]), .S1(n333[2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_108_3.INIT0 = 16'h5aaa;
    defparam add_108_3.INIT1 = 16'h5aaa;
    defparam add_108_3.INJECT1_0 = "NO";
    defparam add_108_3.INJECT1_1 = "NO";
    PFUMX s_tap_state_next_3__I_0_225_i2 (.BLUT(n519[1]), .ALUT(s_tap_state_next_3__N_1438[1]), 
          .C0(n7810), .Z(s_tap_state_next[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    PFUMX s_tap_state_next_3__I_154_i1 (.BLUT(n519[0]), .ALUT(s_tap_state_next_3__N_1461[0]), 
          .C0(n7799), .Z(s_tap_state_next_3__N_1438[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    CCU2D add_108_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_cnt_reg[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n7063), .S1(n333[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_108_1.INIT0 = 16'hF000;
    defparam add_108_1.INIT1 = 16'h5555;
    defparam add_108_1.INJECT1_0 = "NO";
    defparam add_108_1.INJECT1_1 = "NO";
    PFUMX mux_159_i2 (.BLUT(n504[1]), .ALUT(n509[1]), .C0(n7812), .Z(s_tap_state_next_3__N_1461[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    FD1P3IX s_tck_en_194 (.D(n9420), .SP(mclk_c_enable_16), .CD(n2858), 
            .CK(mclk_c), .Q(s_tcken));   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_tck_en_194.GSR = "DISABLED";
    PFUMX mux_157_i1 (.BLUT(n509[0]), .ALUT(s_drpause_state[0]), .C0(n8818), 
          .Z(n514[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    LUT4 i1261_3_lut_4_lut_4_lut (.A(\s_status[2] ), .B(\s_status[3] ), 
         .C(\s_status[0] ), .D(\s_status[1] ), .Z(s_drpause_state[0])) /* synthesis lut_function=(!(A (C (D))+!A (B (C (D))+!B (C (D)+!C !(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1097[3] 1162[12])
    defparam i1261_3_lut_4_lut_4_lut.init = 16'h0ffe;
    LUT4 i1839_3_lut_3_lut_4_lut_4_lut (.A(\s_status[0] ), .B(\s_status[1] ), 
         .C(\s_status[2] ), .D(n9414), .Z(n3933)) /* synthesis lut_function=(A ((C+!(D))+!B)+!A (B+((D)+!C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1024[3] 1089[12])
    defparam i1839_3_lut_3_lut_4_lut_4_lut.init = 16'hf7ef;
    LUT4 tap_state_3__I_0_201_Mux_0_i15_3_lut_4_lut_4_lut (.A(\s_status[0] ), 
         .B(\s_status[1] ), .C(\s_status[2] ), .D(\s_status[3] ), .Z(s_irpause_state[0])) /* synthesis lut_function=(!(A (B (C+!(D)))+!A !(B+((D)+!C)))) */ ;
    defparam tap_state_3__I_0_201_Mux_0_i15_3_lut_4_lut_4_lut.init = 16'h7f67;
    LUT4 mux_102_Mux_8_i15_4_lut_4_lut (.A(\s_status[0] ), .B(\s_status[1] ), 
         .C(\s_status[2] ), .D(\s_status[3] ), .Z(mtms_N_1486[8])) /* synthesis lut_function=(!(A (B+!(C))+!A (B+!(C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(411[22:33])
    defparam mux_102_Mux_8_i15_4_lut_4_lut.init = 16'h3020;
    LUT4 n182_bdd_4_lut_6663 (.A(s_insn_r0[12]), .B(s_insn_r0[14]), .C(s_insn_r0[13]), 
         .D(s_insn_r0[15]), .Z(n8816)) /* synthesis lut_function=(A+((C+!(D))+!B)) */ ;
    defparam n182_bdd_4_lut_6663.init = 16'hfbff;
    LUT4 mux_178_Mux_3_i221_4_lut_then_1_lut (.A(\s_status[3] ), .Z(n8895)) /* synthesis lut_function=(A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1316[3] 1381[12])
    defparam mux_178_Mux_3_i221_4_lut_then_1_lut.init = 16'haaaa;
    LUT4 mux_178_Mux_3_i221_4_lut_else_1_lut (.A(s_end_state[0]), .B(\s_status[3] ), 
         .C(\s_status[2] ), .D(\s_status[0] ), .Z(n8894)) /* synthesis lut_function=(A (B ((D)+!C)+!B !((D)+!C))+!A (B+(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1316[3] 1381[12])
    defparam mux_178_Mux_3_i221_4_lut_else_1_lut.init = 16'hdc7c;
    LUT4 n182_bdd_4_lut_6986 (.A(n8883), .B(n2554), .C(n8884), .D(n9418), 
         .Z(n8897)) /* synthesis lut_function=(!(A ((C (D))+!B)+!A !(B (C+(D))))) */ ;
    defparam n182_bdd_4_lut_6986.init = 16'h4cc8;
    LUT4 i1757_4_lut (.A(s_drscan_state[3]), .B(n5485), .C(n8573), .D(\s_status[3] ), 
         .Z(s_tap_state_next_3__N_1438[3])) /* synthesis lut_function=(A (B+((D)+!C))+!A (B (C)+!B (C (D)))) */ ;
    defparam i1757_4_lut.init = 16'hfaca;
    LUT4 n182_bdd_4_lut (.A(n182), .B(s_insn_r0[14]), .C(s_insn_r0[13]), 
         .D(s_insn_r0[15]), .Z(insn_goto_N_1512)) /* synthesis lut_function=(!((B (D)+!B (C (D)))+!A)) */ ;
    defparam n182_bdd_4_lut.init = 16'h02aa;
    LUT4 mux_565_i2_3_lut_rep_219 (.A(n182), .B(n8883), .C(n2554), .Z(n9413)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam mux_565_i2_3_lut_rep_219.init = 16'hcaca;
    LUT4 i2_3_lut (.A(\s_status[3] ), .B(s_cnt_ovf), .C(n3933), .Z(n7184)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;
    defparam i2_3_lut.init = 16'h0808;
    LUT4 i1_2_lut (.A(\s_status[1] ), .B(n8580), .Z(n7201)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut.init = 16'h8888;
    LUT4 i6232_3_lut (.A(n8104), .B(n19), .C(n20), .Z(n182)) /* synthesis lut_function=(!((B+(C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(321[22:55])
    defparam i6232_3_lut.init = 16'h0202;
    FD1P3IX s_pos_goto_en_193 (.D(s_pos_goto_en_N_1480), .SP(tdi_en_N_1474), 
            .CD(n3519), .CK(mclk_c), .Q(s_pos_goto_en));   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_pos_goto_en_193.GSR = "ENABLED";
    LUT4 i6230_4_lut (.A(s_insn_r0[8]), .B(s_insn_r0[5]), .C(s_insn_r0[11]), 
         .D(s_insn_r0[6]), .Z(n8104)) /* synthesis lut_function=(!(A+(B+(C+(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(321[22:55])
    defparam i6230_4_lut.init = 16'h0001;
    LUT4 i7_4_lut (.A(s_insn_r0[14]), .B(s_insn_r0[9]), .C(s_insn_r0[4]), 
         .D(s_insn_r0[12]), .Z(n19)) /* synthesis lut_function=(A+(B+(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(321[22:55])
    defparam i7_4_lut.init = 16'hfeff;
    LUT4 tap_state_3__I_0_200_Mux_1_i15_3_lut_4_lut_4_lut (.A(\s_status[2] ), 
         .B(\s_status[1] ), .C(\s_status[0] ), .D(\s_status[3] ), .Z(s_irscan_state[1])) /* synthesis lut_function=(!(A (B (C+!(D)))+!A (B (C+!(D))+!B !(C+(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1024[3] 1089[12])
    defparam tap_state_3__I_0_200_Mux_1_i15_3_lut_4_lut_4_lut.init = 16'h3f32;
    LUT4 i8_4_lut (.A(s_insn_r0[13]), .B(s_insn_r0[10]), .C(s_insn_r0[7]), 
         .D(s_insn_r0[15]), .Z(n20)) /* synthesis lut_function=(A+(B+(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(321[22:55])
    defparam i8_4_lut.init = 16'hfeff;
    LUT4 i1_3_lut (.A(s_insn_r0[15]), .B(s_insn_r0[14]), .C(s_insn_r0[13]), 
         .Z(n2554)) /* synthesis lut_function=(A (B+(C))) */ ;
    defparam i1_3_lut.init = 16'ha8a8;
    LUT4 insn_irscan_bdd_4_lut_6538 (.A(n8808), .B(n8816), .C(s_irpause_state[2]), 
         .D(\s_status[2] ), .Z(n8291)) /* synthesis lut_function=(A (B (D))+!A (C)) */ ;
    defparam insn_irscan_bdd_4_lut_6538.init = 16'hd850;
    LUT4 i2306_3_lut (.A(s_end_state[1]), .B(s_end_state[2]), .C(s_end_state[3]), 
         .Z(n4405)) /* synthesis lut_function=(A ((C)+!B)+!A !(B)) */ ;
    defparam i2306_3_lut.init = 16'hb3b3;
    FD1P3AX s_tdi_en_195 (.D(tdi_en_N_1472), .SP(mclk_c_enable_36), .CK(mclk_c), 
            .Q(s_tdien));   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_tdi_en_195.GSR = "DISABLED";
    LUT4 s_end_state_3__I_151_i4_4_lut (.A(insn_goto_N_1512), .B(s_insn_r0[11]), 
         .C(n5526), .D(s_insn_r0[3]), .Z(s_end_state[3])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A !((C)+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[3] 351[10])
    defparam s_end_state_3__I_151_i4_4_lut.init = 16'hac0c;
    LUT4 i6237_2_lut_rep_151_4_lut_3_lut_4_lut_3_lut (.A(s_insn_r0[12]), .B(s_insn_r0[14]), 
         .C(n2554), .Z(n8818)) /* synthesis lut_function=(!(A+(B+!(C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam i6237_2_lut_rep_151_4_lut_3_lut_4_lut_3_lut.init = 16'h1010;
    LUT4 s_end_state_3__I_151_i2_4_lut (.A(s_insn_r0[1]), .B(s_insn_r0[9]), 
         .C(n5526), .D(insn_goto_N_1512), .Z(s_end_state[1])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A !((C)+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[3] 351[10])
    defparam s_end_state_3__I_151_i2_4_lut.init = 16'hac0c;
    LUT4 s_end_state_3__I_151_i3_4_lut (.A(s_insn_r0[2]), .B(s_insn_r0[10]), 
         .C(n5526), .D(insn_goto_N_1512), .Z(s_end_state[2])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A !((C)+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[3] 351[10])
    defparam s_end_state_3__I_151_i3_4_lut.init = 16'hac0c;
    LUT4 i5689_2_lut (.A(s_end_state[0]), .B(s_end_state[1]), .Z(n7743)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i5689_2_lut.init = 16'heeee;
    LUT4 s_end_state_3__I_151_i1_4_lut (.A(s_insn_r0[0]), .B(s_insn_r0[8]), 
         .C(n5526), .D(insn_goto_N_1512), .Z(s_end_state[0])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A !((C)+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[3] 351[10])
    defparam s_end_state_3__I_151_i1_4_lut.init = 16'hac0c;
    FD1P3AX s_insn_r0_i0_i0 (.D(s_reg_din[0]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i0.GSR = "ENABLED";
    LUT4 s_status_1__bdd_3_lut (.A(\s_status[1] ), .B(\s_status[3] ), .C(\s_status[2] ), 
         .Z(n9303)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;
    defparam s_status_1__bdd_3_lut.init = 16'h0404;
    LUT4 ddr_udqm_c_bdd_2_lut (.A(n9304), .B(s_tap_state_next[0]), .Z(n9305)) /* synthesis lut_function=(A (B)) */ ;
    defparam ddr_udqm_c_bdd_2_lut.init = 16'h8888;
    LUT4 tap_state_3__I_0_199_Mux_1_i15_3_lut_4_lut (.A(\s_status[2] ), .B(\s_status[1] ), 
         .C(\s_status[0] ), .D(\s_status[3] ), .Z(s_drpause_state[1])) /* synthesis lut_function=(!(A (B ((D)+!C))+!A (B+!(C+!(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1097[3] 1162[12])
    defparam tap_state_3__I_0_199_Mux_1_i15_3_lut_4_lut.init = 16'h32b3;
    LUT4 s_status_1__bdd_4_lut_6747 (.A(\s_status[1] ), .B(\s_status[3] ), 
         .C(\s_status[2] ), .D(\s_status[0] ), .Z(s_irscan_state[2])) /* synthesis lut_function=(A (C (D))+!A (B (C)+!B (C+!(D)))) */ ;
    defparam s_status_1__bdd_4_lut_6747.init = 16'hf051;
    LUT4 s_status_1__bdd_4_lut (.A(\s_status[1] ), .B(\s_status[0] ), .C(\s_status[3] ), 
         .D(\s_status[2] ), .Z(n9302)) /* synthesis lut_function=(A (B (C (D)))+!A !(B+(C+!(D)))) */ ;
    defparam s_status_1__bdd_4_lut.init = 16'h8100;
    PFUMX i6561 (.BLUT(n8603), .ALUT(n8602), .C0(s_tap_state_next[2]), 
          .Z(s_mtms));
    LUT4 mux_565_i3_3_lut_rep_220 (.A(n182), .B(n8884), .C(n2554), .Z(n9414)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam mux_565_i3_3_lut_rep_220.init = 16'hcaca;
    LUT4 s_opc_2__I_0_235_i4_2_lut_rep_153_4_lut_3_lut (.A(n8883), .B(n2554), 
         .C(n8884), .Z(n8820)) /* synthesis lut_function=(A+!(B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam s_opc_2__I_0_235_i4_2_lut_rep_153_4_lut_3_lut.init = 16'hbfbf;
    LUT4 s_opc_2__I_0_232_i4_2_lut_rep_154_4_lut_3_lut (.A(n8883), .B(n2554), 
         .C(n8884), .Z(n8821)) /* synthesis lut_function=(((C)+!B)+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam s_opc_2__I_0_232_i4_2_lut_rep_154_4_lut_3_lut.init = 16'hf7f7;
    LUT4 mux_565_i1_3_lut_rep_165 (.A(n182), .B(n9418), .C(n2554), .Z(n8832)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam mux_565_i1_3_lut_rep_165.init = 16'hcaca;
    LUT4 i1_2_lut_4_lut_3_lut (.A(n9418), .B(n2554), .C(n8883), .Z(n2494)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam i1_2_lut_4_lut_3_lut.init = 16'h0808;
    PFUMX i6545 (.BLUT(n8579), .ALUT(n8576), .C0(n9414), .Z(n8580));
    LUT4 i3243_3_lut_4_lut (.A(s_en_r0), .B(n8781), .C(n8765), .D(\s_offs_r[1] ), 
         .Z(n2898)) /* synthesis lut_function=(!(A (C (D)+!C !(D))+!A (B+(C (D)+!C !(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1472[16] 1473[55])
    defparam i3243_3_lut_4_lut.init = 16'h0bb0;
    LUT4 i3224_3_lut_4_lut (.A(s_en_r0), .B(n8781), .C(s_offs_inc), .D(\s_offs_r[0] ), 
         .Z(n2873)) /* synthesis lut_function=(!(A (C (D)+!C !(D))+!A (B+(C (D)+!C !(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1472[16] 1473[55])
    defparam i3224_3_lut_4_lut.init = 16'h0bb0;
    LUT4 i1_2_lut_adj_173 (.A(\s_status[3] ), .B(\s_status[0] ), .Z(n7710)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i1_2_lut_adj_173.init = 16'heeee;
    LUT4 i2_3_lut_4_lut (.A(s_en_r0), .B(n8781), .C(tckn_stb), .D(s_en_r1), 
         .Z(mclk_c_enable_129)) /* synthesis lut_function=(A (C (D))+!A (B+(C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1472[16] 1473[55])
    defparam i2_3_lut_4_lut.init = 16'hf444;
    LUT4 i3427_3_lut (.A(\s_status[2] ), .B(\s_status[1] ), .C(\s_status[0] ), 
         .Z(s_reset_state[2])) /* synthesis lut_function=(A ((C)+!B)+!A !(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1243[3] 1308[12])
    defparam i3427_3_lut.init = 16'hb3b3;
    FD1S3AX s_exec_r1_189 (.D(s_tap_busy), .CK(mclk_c), .Q(s_exec_en)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(259[3] 269[10])
    defparam s_exec_r1_189.GSR = "ENABLED";
    FD1P3AY s_tap_state__i1 (.D(s_tap_state_next[0]), .SP(tckn_stb), .CK(mclk_c), 
            .Q(\s_status[0] )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_tap_state__i1.GSR = "ENABLED";
    FD1P3IX s_cnt_reg__i7 (.D(n333[7]), .SP(mclk_c_enable_102), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i7.GSR = "ENABLED";
    FD1P3IX s_cnt_reg__i6 (.D(n333[6]), .SP(mclk_c_enable_102), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i6.GSR = "ENABLED";
    FD1S3AX s_shift_en_r0_197 (.D(s_shift_en), .CK(mclk_c), .Q(s_shift_en_r0)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1477[3] 1481[10])
    defparam s_shift_en_r0_197.GSR = "ENABLED";
    LUT4 i5691_4_lut (.A(s_end_state[0]), .B(s_end_state[1]), .C(\s_status[0] ), 
         .D(\s_status[1] ), .Z(n7745)) /* synthesis lut_function=(!(A (B (C (D))+!B !((D)+!C))+!A !(B (C+!(D))+!B (C+(D))))) */ ;
    defparam i5691_4_lut.init = 16'h7bde;
    LUT4 i5668_4_lut (.A(s_end_state[2]), .B(s_end_state[3]), .C(\s_status[2] ), 
         .D(\s_status[3] ), .Z(n7719)) /* synthesis lut_function=(!(A (B (C (D))+!B !((D)+!C))+!A !(B (C+!(D))+!B (C+(D))))) */ ;
    defparam i5668_4_lut.init = 16'h7bde;
    LUT4 i1_4_lut (.A(s_tap_busy), .B(n182), .C(n3503), .D(n2554), .Z(n7653)) /* synthesis lut_function=(A (B (C+!(D))+!B (C (D)))) */ ;
    defparam i1_4_lut.init = 16'ha088;
    FD1P3IX s_cnt_reg__i5 (.D(n333[5]), .SP(mclk_c_enable_102), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i5.GSR = "ENABLED";
    FD1P3IX s_cnt_reg__i4 (.D(n333[4]), .SP(mclk_c_enable_102), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i4.GSR = "ENABLED";
    FD1P3IX s_cnt_reg__i3 (.D(n333[3]), .SP(mclk_c_enable_102), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i3.GSR = "ENABLED";
    FD1P3IX s_cnt_reg__i2 (.D(n333[2]), .SP(mclk_c_enable_102), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i2.GSR = "ENABLED";
    FD1P3IX s_cnt_reg__i1 (.D(n333[1]), .SP(mclk_c_enable_102), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i1.GSR = "ENABLED";
    LUT4 s_status_3__bdd_4_lut_6537 (.A(\s_status[3] ), .B(\s_status[2] ), 
         .C(\s_status[1] ), .D(\s_status[0] ), .Z(s_irpause_state[2])) /* synthesis lut_function=(A (B ((D)+!C)+!B !(C+(D)))+!A (B ((D)+!C)+!B !(C))) */ ;
    defparam s_status_3__bdd_4_lut_6537.init = 16'hcd0f;
    FD1S3AX s_exec_r0_188 (.D(n4416), .CK(mclk_c), .Q(s_tap_busy)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(259[3] 269[10])
    defparam s_exec_r0_188.GSR = "ENABLED";
    LUT4 s_status_2__bdd_4_lut_6578 (.A(\s_status[2] ), .B(\s_status[3] ), 
         .C(\s_status[1] ), .D(\s_status[0] ), .Z(s_drscan_state[1])) /* synthesis lut_function=(!(A (B (C))+!A (B (C+!(D))+!B (C (D))))) */ ;
    defparam s_status_2__bdd_4_lut_6578.init = 16'h2f3b;
    LUT4 i1427_2_lut_4_lut (.A(n8817), .B(n7745), .C(n7719), .D(n8807), 
         .Z(n3519)) /* synthesis lut_function=(A (B (D)+!B ((D)+!C))+!A (D)) */ ;
    defparam i1427_2_lut_4_lut.init = 16'hff02;
    LUT4 i872_2_lut_4_lut (.A(n8817), .B(n7745), .C(n7719), .D(n9428), 
         .Z(n2858)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;
    defparam i872_2_lut_4_lut.init = 16'h0002;
    LUT4 i1_2_lut_rep_114 (.A(s_shift_en_r0), .B(s_shift_en), .Z(n8781)) /* synthesis lut_function=(!(A+!(B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1472[16] 1473[55])
    defparam i1_2_lut_rep_114.init = 16'h4444;
    LUT4 i1_2_lut_rep_107_3_lut (.A(s_shift_en_r0), .B(s_shift_en), .C(s_en_r0), 
         .Z(n8774)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1472[16] 1473[55])
    defparam i1_2_lut_rep_107_3_lut.init = 16'h0404;
    LUT4 i1358_4_lut_4_lut (.A(\s_status[0] ), .B(n8885), .C(\s_status[3] ), 
         .D(n7743), .Z(n126)) /* synthesis lut_function=(A (B (C))+!A (B (C)+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1024[3] 1089[12])
    defparam i1358_4_lut_4_lut.init = 16'hc1c0;
    LUT4 s_status_3__bdd_4_lut_6552 (.A(\s_status[3] ), .B(\s_status[0] ), 
         .C(\s_status[2] ), .D(\s_status[1] ), .Z(s_runtest_state[0])) /* synthesis lut_function=(!(A (B (C+(D))+!B !((D)+!C))+!A (B (C+(D))))) */ ;
    defparam s_status_3__bdd_4_lut_6552.init = 16'h331f;
    LUT4 i1316_3_lut_rep_162_4_lut (.A(n8883), .B(n8884), .C(n2554), .D(n182), 
         .Z(n8829)) /* synthesis lut_function=(A (C+(D))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(325[26:45])
    defparam i1316_3_lut_rep_162_4_lut.init = 16'hefe0;
    LUT4 i1411_2_lut_3_lut (.A(n8883), .B(n8884), .C(n9418), .Z(n3503)) /* synthesis lut_function=(A+(B+(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(325[26:45])
    defparam i1411_2_lut_3_lut.init = 16'hfefe;
    LUT4 s_status_0__bdd_4_lut_6656 (.A(\s_status[0] ), .B(\s_status[3] ), 
         .C(\s_status[2] ), .D(\s_status[1] ), .Z(s_drscan_state[2])) /* synthesis lut_function=(A (C)+!A !(B (D)+!B ((D)+!C))) */ ;
    defparam s_status_0__bdd_4_lut_6656.init = 16'ha0f4;
    LUT4 i3450_3_lut_rep_161_4_lut (.A(n8883), .B(n8884), .C(n2554), .D(n182), 
         .Z(n8828)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A !(C+!(D))) */ ;
    defparam i3450_3_lut_rep_161_4_lut.init = 16'h8f80;
    LUT4 tdi_en_I_159_2_lut (.A(s_pos_goto_en), .B(s_exec_en), .Z(tdi_en_N_1474)) /* synthesis lut_function=(!(A+!(B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(372[10:53])
    defparam tdi_en_I_159_2_lut.init = 16'h4444;
    LUT4 i3234_2_lut (.A(n2503[0]), .B(n8897), .Z(s_pos_goto_en_N_1480)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(374[5] 395[12])
    defparam i3234_2_lut.init = 16'h8888;
    LUT4 i3_4_lut (.A(n3519), .B(n9428), .C(s_pos_goto_en), .D(s_exec_en), 
         .Z(mclk_c_enable_36)) /* synthesis lut_function=(!(A+(B+(C+!(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(372[10:53])
    defparam i3_4_lut.init = 16'h0100;
    FD1P3AX s_insn_r0_i0_i1 (.D(s_reg_din[1]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i1.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i2 (.D(s_reg_din[2]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i2.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i3 (.D(s_reg_din[3]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i3.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i4 (.D(s_reg_din[4]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i4.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i5 (.D(s_reg_din[5]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i5.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i6 (.D(s_reg_din[6]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i6.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i7 (.D(s_reg_din[7]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i7.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i8 (.D(s_reg_din[8]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i8.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i9 (.D(s_reg_din[9]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i9.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i10 (.D(s_reg_din[10]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[10])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i10.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i11 (.D(s_reg_din[11]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[11])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i11.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i12 (.D(s_reg_din[12]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[12])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i12.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i13 (.D(s_reg_din[13]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[13])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i13.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i14 (.D(s_reg_din[14]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[14])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i14.GSR = "ENABLED";
    FD1P3AX s_insn_r0_i0_i15 (.D(s_reg_din[15]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[15])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i15.GSR = "ENABLED";
    FD1P3AY s_tap_state__i2 (.D(s_tap_state_next[1]), .SP(tckn_stb), .CK(mclk_c), 
            .Q(\s_status[1] )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_tap_state__i2.GSR = "ENABLED";
    LUT4 ddr_udqm_c_bdd_2_lut_6564 (.A(n8575), .B(s_cnt_ovf), .Z(n8576)) /* synthesis lut_function=(A (B)) */ ;
    defparam ddr_udqm_c_bdd_2_lut_6564.init = 16'h8888;
    LUT4 s_opc_1__bdd_4_lut (.A(n9413), .B(\s_status[0] ), .C(\s_status[3] ), 
         .D(\s_status[2] ), .Z(n8575)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;
    defparam s_opc_1__bdd_4_lut.init = 16'h4008;
    LUT4 i6265_4_lut_4_lut (.A(n8800), .B(n8816), .C(n8796), .D(n8817), 
         .Z(n7799)) /* synthesis lut_function=(A (D)+!A (B (D)+!B ((D)+!C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6265_4_lut_4_lut.init = 16'hff01;
    LUT4 s_opc_0__bdd_4_lut_6568 (.A(n8832), .B(\s_status[3] ), .C(\s_status[2] ), 
         .D(s_en_r1), .Z(n8577)) /* synthesis lut_function=(!(A ((C+!(D))+!B)+!A (B+(C+!(D))))) */ ;
    defparam s_opc_0__bdd_4_lut_6568.init = 16'h0900;
    LUT4 s_tap_busy_I_0_1_lut_rep_189 (.A(s_tap_busy), .Z(n8856)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1002[16:30])
    defparam s_tap_busy_I_0_1_lut_rep_189.init = 16'h5555;
    LUT4 i2_3_lut_3_lut (.A(s_tap_busy), .B(out_get_N_312), .C(n7771), 
         .Z(busy_N_1509)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(1002[16:30])
    defparam i2_3_lut_3_lut.init = 16'h0404;
    LUT4 i6272_4_lut_4_lut (.A(n8800), .B(n7804), .C(n8796), .D(n8817), 
         .Z(n7810)) /* synthesis lut_function=(A (C+(D))+!A (B+(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6272_4_lut_4_lut.init = 16'hfff4;
    LUT4 i2302_4_lut_4_lut (.A(n8801), .B(n8816), .C(n8796), .D(n5), 
         .Z(n4401)) /* synthesis lut_function=(A (C)+!A (B (C)+!B (C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[8:27])
    defparam i2302_4_lut_4_lut.init = 16'hf1f0;
    FD1P3AY s_tap_state__i3 (.D(s_tap_state_next[2]), .SP(tckn_stb), .CK(mclk_c), 
            .Q(\s_status[2] )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_tap_state__i3.GSR = "ENABLED";
    FD1P3AY s_tap_state__i4 (.D(s_tap_state_next[3]), .SP(tckn_stb), .CK(mclk_c), 
            .Q(\s_status[3] )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_tap_state__i4.GSR = "ENABLED";
    LUT4 i6267_4_lut_4_lut (.A(n8800), .B(n7785), .C(n8796), .D(n8817), 
         .Z(n7795)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6267_4_lut_4_lut.init = 16'hfffe;
    LUT4 n4_bdd_4_lut_6560 (.A(n4), .B(s_tap_state_next[1]), .C(s_tap_state_next[0]), 
         .D(mtms_N_1486[8]), .Z(n8601)) /* synthesis lut_function=(A ((C (D))+!B)+!A (B (C (D)))) */ ;
    defparam n4_bdd_4_lut_6560.init = 16'he222;
    LUT4 n4_bdd_4_lut (.A(s_tap_state_next[3]), .B(n7723), .C(s_tap_state_next[1]), 
         .D(\s_status[3] ), .Z(n8603)) /* synthesis lut_function=(!(A ((C+!(D))+!B)+!A ((C+(D))+!B))) */ ;
    defparam n4_bdd_4_lut.init = 16'h0804;
    LUT4 n8601_bdd_3_lut (.A(n8601), .B(n9305), .C(s_tap_state_next[3]), 
         .Z(n8602)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam n8601_bdd_3_lut.init = 16'hcaca;
    LUT4 s_status_0__bdd_4_lut_6824 (.A(\s_status[0] ), .B(\s_status[3] ), 
         .C(\s_status[1] ), .D(\s_status[2] ), .Z(s_irscan_state[0])) /* synthesis lut_function=(!(A (C)+!A (B (C+!(D))+!B !(C+!(D))))) */ ;
    defparam s_status_0__bdd_4_lut_6824.init = 16'h1e1b;
    LUT4 i1_2_lut_4_lut (.A(\s_status[0] ), .B(\s_status[1] ), .C(\s_status[2] ), 
         .D(\s_status[3] ), .Z(n2050[4])) /* synthesis lut_function=(!(((C+!(D))+!B)+!A)) */ ;
    defparam i1_2_lut_4_lut.init = 16'h0800;
    LUT4 i2838_4_lut_4_lut (.A(\s_status[0] ), .B(\s_status[1] ), .C(\s_status[2] ), 
         .D(s_tap_state_next[0]), .Z(n7723)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (D)))) */ ;
    defparam i2838_4_lut_4_lut.init = 16'h4408;
    LUT4 i6261_3_lut_3_lut_4_lut_4_lut_4_lut_3_lut_1_lut_2_lut_4_lut_3_lut (.A(n8884), 
         .B(n2554), .C(n8883), .Z(tdi_en_N_1472)) /* synthesis lut_function=(!(A+!(B (C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam i6261_3_lut_3_lut_4_lut_4_lut_4_lut_3_lut_1_lut_2_lut_4_lut_3_lut.init = 16'h4040;
    CCU2D s_cnt_reg_7__I_0_8 (.A0(s_cnt_top[1]), .B0(s_cnt_reg[1]), .C0(s_cnt_top[0]), 
          .D0(s_cnt_reg[0]), .A1(GND_net), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6962), .S1(s_cnt_ovf));
    defparam s_cnt_reg_7__I_0_8.INIT0 = 16'h9009;
    defparam s_cnt_reg_7__I_0_8.INIT1 = 16'hFFFF;
    defparam s_cnt_reg_7__I_0_8.INJECT1_0 = "YES";
    defparam s_cnt_reg_7__I_0_8.INJECT1_1 = "NO";
    CCU2D s_cnt_reg_7__I_0_7 (.A0(s_cnt_top[5]), .B0(s_cnt_reg[5]), .C0(s_cnt_top[4]), 
          .D0(s_cnt_reg[4]), .A1(s_cnt_top[3]), .B1(s_cnt_reg[3]), .C1(s_cnt_top[2]), 
          .D1(s_cnt_reg[2]), .CIN(n6961), .COUT(n6962));
    defparam s_cnt_reg_7__I_0_7.INIT0 = 16'h9009;
    defparam s_cnt_reg_7__I_0_7.INIT1 = 16'h9009;
    defparam s_cnt_reg_7__I_0_7.INJECT1_0 = "YES";
    defparam s_cnt_reg_7__I_0_7.INJECT1_1 = "YES";
    LUT4 i3213_3_lut_3_lut_4_lut (.A(\s_status[2] ), .B(\s_status[3] ), 
         .C(\s_status[0] ), .D(\s_status[1] ), .Z(n2050[8])) /* synthesis lut_function=(!(A+(B+!(C (D)+!C !(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1097[3] 1162[12])
    defparam i3213_3_lut_3_lut_4_lut.init = 16'h1001;
    LUT4 i3464_2_lut_3_lut (.A(\s_status[2] ), .B(\s_status[3] ), .C(\s_status[1] ), 
         .Z(mtms_N_1486[10])) /* synthesis lut_function=(!(A+(B+(C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1097[3] 1162[12])
    defparam i3464_2_lut_3_lut.init = 16'h0101;
    CCU2D s_cnt_reg_7__I_0_0 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_top[7]), .B1(s_cnt_reg[7]), .C1(s_cnt_top[6]), 
          .D1(s_cnt_reg[6]), .COUT(n6961));   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(428[24:45])
    defparam s_cnt_reg_7__I_0_0.INIT0 = 16'hF000;
    defparam s_cnt_reg_7__I_0_0.INIT1 = 16'h9009;
    defparam s_cnt_reg_7__I_0_0.INJECT1_0 = "NO";
    defparam s_cnt_reg_7__I_0_0.INJECT1_1 = "YES";
    LUT4 i3373_3_lut_4_lut (.A(\s_status[3] ), .B(\s_status[2] ), .C(\s_status[0] ), 
         .D(\s_status[1] ), .Z(s_reset_state[0])) /* synthesis lut_function=(A (B+!(C (D)))+!A !(C (D))) */ ;
    defparam i3373_3_lut_4_lut.init = 16'h8fff;
    LUT4 i3321_2_lut_3_lut (.A(\s_status[0] ), .B(\s_status[3] ), .C(\s_status[1] ), 
         .Z(n5413)) /* synthesis lut_function=(A (B+!(C))+!A !(C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1243[3] 1308[12])
    defparam i3321_2_lut_3_lut.init = 16'h8f8f;
    LUT4 i1224_3_lut_3_lut_4_lut (.A(\s_status[0] ), .B(\s_status[3] ), 
         .C(\s_status[2] ), .D(\s_status[1] ), .Z(s_irpause_state[1])) /* synthesis lut_function=(!(A (B (C (D))+!B ((D)+!C))+!A ((D)+!C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1243[3] 1308[12])
    defparam i1224_3_lut_3_lut_4_lut.init = 16'h08f8;
    LUT4 i1285_4_lut_3_lut (.A(\s_status[0] ), .B(\s_status[1] ), .C(\s_status[3] ), 
         .Z(n3377)) /* synthesis lut_function=(A ((C)+!B)+!A !(B+!(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1389[3] 1454[12])
    defparam i1285_4_lut_3_lut.init = 16'hb2b2;
    PFUMX i6909 (.BLUT(n9303), .ALUT(n9302), .C0(s_tap_state_next[1]), 
          .Z(n9304));
    LUT4 n8578_bdd_2_lut_3_lut (.A(n8577), .B(\s_status[0] ), .C(s_en_r0), 
         .Z(n8579)) /* synthesis lut_function=(!((B+(C))+!A)) */ ;
    defparam n8578_bdd_2_lut_3_lut.init = 16'h0202;
    LUT4 i3393_2_lut_3_lut_4_lut (.A(\s_status[1] ), .B(\s_status[2] ), 
         .C(n8829), .D(n8832), .Z(n5485)) /* synthesis lut_function=(!(A+((C+!(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1316[3] 1381[12])
    defparam i3393_2_lut_3_lut_4_lut.init = 16'h0400;
    LUT4 i2_3_lut_4_lut_4_lut (.A(\s_status[3] ), .B(\s_status[0] ), .C(\s_status[2] ), 
         .D(\s_status[1] ), .Z(mtms_N_1486[11])) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam i2_3_lut_4_lut_4_lut.init = 16'h4000;
    LUT4 i2316_3_lut (.A(n8778), .B(busy_N_1509), .C(n7653), .Z(n4416)) /* synthesis lut_function=(A (B)+!A (B+(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(259[3] 269[10])
    defparam i2316_3_lut.init = 16'hdcdc;
    LUT4 mux_563_i2_3_lut_rep_216 (.A(s_insn_r0[12]), .B(s_insn_r0[13]), 
         .C(s_insn_r0[14]), .Z(n8883)) /* synthesis lut_function=(A (B (C))+!A (B+!(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam mux_563_i2_3_lut_rep_216.init = 16'hc5c5;
    LUT4 i1_3_lut_adj_174 (.A(busy_N_1509), .B(tckn_stb), .C(n2), .Z(mclk_c_enable_102)) /* synthesis lut_function=(A+(B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(263[7:45])
    defparam i1_3_lut_adj_174.init = 16'heaea;
    LUT4 i1_2_lut_rep_140 (.A(s_exec_en), .B(n7653), .Z(n8807)) /* synthesis lut_function=(!(A+!(B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(273[16:59])
    defparam i1_2_lut_rep_140.init = 16'h4444;
    LUT4 i1262_2_lut_3_lut (.A(s_exec_en), .B(n7653), .C(n9428), .Z(mclk_c_enable_16)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(273[16:59])
    defparam i1262_2_lut_3_lut.init = 16'h0404;
    LUT4 i6174_3_lut_4_lut_4_lut (.A(n8820), .B(n504[0]), .C(s_reset_state[0]), 
         .D(n8832), .Z(n519[0])) /* synthesis lut_function=(A (B)+!A (B (C+(D))+!B !((D)+!C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(338[11:28])
    defparam i6174_3_lut_4_lut_4_lut.init = 16'hccd8;
    LUT4 i3335_3_lut_rep_217 (.A(s_insn_r0[13]), .B(s_insn_r0[14]), .C(s_insn_r0[12]), 
         .Z(n8884)) /* synthesis lut_function=(!(A (B)+!A !((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam i3335_3_lut_rep_217.init = 16'h7373;
    LUT4 i6165_3_lut_4_lut_4_lut (.A(n8821), .B(s_irpause_state[0]), .C(s_irscan_state[0]), 
         .D(n8832), .Z(n509[0])) /* synthesis lut_function=(A (B)+!A (B (C+!(D))+!B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[8:27])
    defparam i6165_3_lut_4_lut_4_lut.init = 16'hd8cc;
    LUT4 i2163_2_lut_rep_218 (.A(\s_status[1] ), .B(\s_status[2] ), .Z(n8885)) /* synthesis lut_function=(A+!(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1024[3] 1089[12])
    defparam i2163_2_lut_rep_218.init = 16'hbbbb;
    LUT4 i1_2_lut_3_lut (.A(\s_status[1] ), .B(\s_status[2] ), .C(\s_status[0] ), 
         .Z(n4_adj_1590)) /* synthesis lut_function=(!(A (C)+!A (B+(C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1024[3] 1089[12])
    defparam i1_2_lut_3_lut.init = 16'h0b0b;
    LUT4 mux_155_i2_4_lut_4_lut (.A(\s_status[1] ), .B(\s_status[2] ), .C(n7710), 
         .D(n8816), .Z(n504[1])) /* synthesis lut_function=(A (D)+!A !((C+(D))+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1024[3] 1089[12])
    defparam mux_155_i2_4_lut_4_lut.init = 16'haa04;
    LUT4 mux_563_i1_3_lut_rep_223 (.A(s_insn_r0[12]), .B(s_insn_r0[13]), 
         .C(s_insn_r0[14]), .Z(n9418)) /* synthesis lut_function=(A (B+!(C))+!A !(B+!(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam mux_563_i1_3_lut_rep_223.init = 16'h9a9a;
    LUT4 mux_178_Mux_3_i236_3_lut_3_lut_4_lut (.A(\s_status[1] ), .B(\s_status[2] ), 
         .C(\s_status[3] ), .D(\s_status[0] ), .Z(s_drscan_state[3])) /* synthesis lut_function=(A (C)+!A !(B (C+(D))+!B !(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1024[3] 1089[12])
    defparam mux_178_Mux_3_i236_3_lut_3_lut_4_lut.init = 16'hb0b4;
    PFUMX i6338 (.BLUT(n8293), .ALUT(s_reset_state[2]), .C0(n4401), .Z(n8294));
    LUT4 i1_2_lut_3_lut_4_lut (.A(n9414), .B(n9413), .C(s_insn_r0[5]), 
         .D(n5312), .Z(s_cnt_top[5])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i1_2_lut_3_lut_4_lut.init = 16'h00b0;
    LUT4 i1_2_lut_3_lut_4_lut_adj_175 (.A(n9414), .B(n9413), .C(s_insn_r0[3]), 
         .D(n5312), .Z(s_cnt_top[3])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_175.init = 16'h00b0;
    LUT4 i3290_2_lut_3_lut_4_lut (.A(n9414), .B(n9413), .C(s_insn_r0[0]), 
         .D(n5312), .Z(s_cnt_top[0])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i3290_2_lut_3_lut_4_lut.init = 16'h00b0;
    LUT4 i3378_2_lut_3_lut_4_lut (.A(n9414), .B(n9413), .C(s_insn_r0[1]), 
         .D(n5312), .Z(s_cnt_top[1])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i3378_2_lut_3_lut_4_lut.init = 16'h00b0;
    LUT4 i1_2_lut_3_lut_adj_176 (.A(n9414), .B(n9413), .C(s_insn_r0[4]), 
         .Z(s_ptr_addr[5])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i1_2_lut_3_lut_adj_176.init = 16'h4040;
    LUT4 i1_2_lut_3_lut_adj_177 (.A(n9414), .B(n9413), .C(s_insn_r0[3]), 
         .Z(s_ptr_addr[4])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i1_2_lut_3_lut_adj_177.init = 16'h4040;
    LUT4 i3382_2_lut_3_lut (.A(n9414), .B(n9413), .C(s_insn_r0[2]), .Z(s_ptr_addr[3])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i3382_2_lut_3_lut.init = 16'h4040;
    LUT4 i1_2_lut_3_lut_4_lut_adj_178 (.A(n9414), .B(n9413), .C(s_insn_r0[4]), 
         .D(n5312), .Z(s_cnt_top[4])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_178.init = 16'h00b0;
    LUT4 i3381_2_lut_3_lut (.A(n9414), .B(n9413), .C(s_insn_r0[1]), .Z(s_ptr_addr[2])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i3381_2_lut_3_lut.init = 16'h4040;
    LUT4 insn_reset_I_0_4_lut (.A(n3377), .B(n8796), .C(n8816), .D(\s_status[2] ), 
         .Z(s_cnt_en_N_1514)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1467[57] 1469[46])
    defparam insn_reset_I_0_4_lut.init = 16'hcac0;
    LUT4 i1_2_lut_3_lut_adj_179 (.A(n9414), .B(n9413), .C(s_insn_r0[7]), 
         .Z(s_ptr_addr[8])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i1_2_lut_3_lut_adj_179.init = 16'h4040;
    LUT4 mux_156_i2_4_lut (.A(s_irscan_state[1]), .B(n5413), .C(n8796), 
         .D(\s_status[2] ), .Z(n509[1])) /* synthesis lut_function=(A (B ((D)+!C)+!B !(C))+!A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1458[62] 1463[45])
    defparam mux_156_i2_4_lut.init = 16'hca0a;
    LUT4 i3383_2_lut_3_lut (.A(n9414), .B(n9413), .C(s_insn_r0[6]), .Z(s_ptr_addr[7])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i3383_2_lut_3_lut.init = 16'h4040;
    LUT4 i1_2_lut_3_lut_adj_180 (.A(n9414), .B(n9413), .C(s_insn_r0[5]), 
         .Z(s_ptr_addr[6])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i1_2_lut_3_lut_adj_180.init = 16'h4040;
    LUT4 i3379_2_lut_3_lut_4_lut (.A(n9414), .B(n9413), .C(s_insn_r0[2]), 
         .D(n5312), .Z(s_cnt_top[2])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i3379_2_lut_3_lut_4_lut.init = 16'h00b0;
    LUT4 i1_2_lut_3_lut_4_lut_adj_181 (.A(n9414), .B(n9413), .C(s_insn_r0[7]), 
         .D(n5312), .Z(s_cnt_top[7])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_181.init = 16'h00b0;
    LUT4 i3380_2_lut_3_lut_4_lut (.A(n9414), .B(n9413), .C(s_insn_r0[6]), 
         .D(n5312), .Z(s_cnt_top[6])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i3380_2_lut_3_lut_4_lut.init = 16'h00b0;
    LUT4 i3291_2_lut_3_lut (.A(n9414), .B(n9413), .C(s_insn_r0[0]), .Z(s_ptr_addr[1])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i3291_2_lut_3_lut.init = 16'h4040;
    PFUMX mux_538_i1 (.BLUT(n7201), .ALUT(n7184), .C0(n2494), .Z(n2503[0]));
    LUT4 i2_4_lut (.A(\s_status[3] ), .B(n4_adj_1590), .C(n8801), .D(n8800), 
         .Z(s_shift_en)) /* synthesis lut_function=(!(A (((D)+!C)+!B)+!A !(B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1472[16] 1473[55])
    defparam i2_4_lut.init = 16'h4480;
    LUT4 i6176_3_lut_4_lut_4_lut (.A(n8821), .B(n514[1]), .C(s_drscan_state[1]), 
         .D(n8832), .Z(n519[1])) /* synthesis lut_function=(A (B)+!A (B (C+(D))+!B !((D)+!C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6176_3_lut_4_lut_4_lut.init = 16'hccd8;
    LUT4 s_drscan_state_2__bdd_3_lut_4_lut (.A(n8832), .B(n9413), .C(n8292), 
         .D(s_drscan_state[2]), .Z(n8293)) /* synthesis lut_function=(A (C)+!A (B (D)+!B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1458[62] 1459[40])
    defparam s_drscan_state_2__bdd_3_lut_4_lut.init = 16'hf4b0;
    LUT4 mux_155_i1_3_lut_4_lut_4_lut (.A(n8821), .B(\s_status[0] ), .C(s_irscan_state[1]), 
         .D(n8832), .Z(n504[0])) /* synthesis lut_function=(A (B)+!A (B (C+(D))+!B !((D)+!C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam mux_155_i1_3_lut_4_lut_4_lut.init = 16'hccd8;
    LUT4 insn_goto_I_0_238_2_lut_rep_150 (.A(insn_goto_N_1512), .B(s_pos_goto_en), 
         .Z(n8817)) /* synthesis lut_function=(A+(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(404[30] 405[24])
    defparam insn_goto_I_0_238_2_lut_rep_150.init = 16'heeee;
    LUT4 n8294_bdd_3_lut_4_lut (.A(insn_goto_N_1512), .B(s_pos_goto_en), 
         .C(s_goto_state[2]), .D(n8294), .Z(s_tap_state_next[2])) /* synthesis lut_function=(A (C)+!A (B (C)+!B (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(404[30] 405[24])
    defparam n8294_bdd_3_lut_4_lut.init = 16'hf1e0;
    LUT4 mux_159_i1_3_lut_4_lut (.A(insn_goto_N_1512), .B(s_pos_goto_en), 
         .C(s_goto_state[0]), .D(s_runtest_state[0]), .Z(s_tap_state_next_3__N_1461[0])) /* synthesis lut_function=(A (C)+!A (B (C)+!B (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(404[30] 405[24])
    defparam mux_159_i1_3_lut_4_lut.init = 16'hf1e0;
    LUT4 s_tap_state_next_3__I_154_i2_3_lut_4_lut (.A(insn_goto_N_1512), .B(s_pos_goto_en), 
         .C(s_goto_state[1]), .D(s_tap_state_next_3__N_1461[1]), .Z(s_tap_state_next_3__N_1438[1])) /* synthesis lut_function=(A (C)+!A (B (C)+!B (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(404[30] 405[24])
    defparam s_tap_state_next_3__I_154_i2_3_lut_4_lut.init = 16'hf1e0;
    LUT4 i1_3_lut_rep_111_4_lut (.A(insn_goto_N_1512), .B(s_pos_goto_en), 
         .C(n7719), .D(n7745), .Z(n8778)) /* synthesis lut_function=(!(A (C+(D))+!A ((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(404[30] 405[24])
    defparam i1_3_lut_rep_111_4_lut.init = 16'h000e;
    LUT4 i6161_3_lut_4_lut_4_lut (.A(n8832), .B(s_irpause_state[1]), .C(s_drpause_state[1]), 
         .D(n8828), .Z(n514[1])) /* synthesis lut_function=(A (B)+!A (B (C+!(D))+!B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[34:54])
    defparam i6161_3_lut_4_lut_4_lut.init = 16'hd8cc;
    LUT4 i6163_3_lut_4_lut_4_lut (.A(n8832), .B(n2050[4]), .C(n2050[8]), 
         .D(n8828), .Z(n1)) /* synthesis lut_function=(A (B)+!A (B (C+!(D))+!B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[34:54])
    defparam i6163_3_lut_4_lut_4_lut.init = 16'hd8cc;
    PFUMX i6336 (.BLUT(n8291), .ALUT(s_irscan_state[2]), .C0(n8801), .Z(n8292));
    LUT4 i5747_3_lut_3_lut_4_lut_4_lut_4_lut_4_lut (.A(n8832), .B(n8828), 
         .C(n8820), .D(n8821), .Z(n7804)) /* synthesis lut_function=(A (C+!(D))+!A !(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[34:54])
    defparam i5747_3_lut_3_lut_4_lut_4_lut_4_lut_4_lut.init = 16'hb1bb;
    LUT4 i5728_3_lut_3_lut_4_lut_4_lut_4_lut_4_lut (.A(n8832), .B(n8828), 
         .C(n8820), .D(n8821), .Z(n7785)) /* synthesis lut_function=(A (C (D))+!A !(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[34:54])
    defparam i5728_3_lut_3_lut_4_lut_4_lut_4_lut_4_lut.init = 16'hb111;
    LUT4 s_opc_2__I_0_235_i5_2_lut_rep_141_3_lut (.A(n9413), .B(n9414), 
         .C(n8832), .Z(n8808)) /* synthesis lut_function=(A+!(B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[8:28])
    defparam s_opc_2__I_0_235_i5_2_lut_rep_141_3_lut.init = 16'hbfbf;
    LUT4 insn_reset_bdd_4_lut_4_lut_4_lut_3_lut (.A(n9413), .B(n9414), .C(n8832), 
         .Z(n8573)) /* synthesis lut_function=(A (B (C))+!A !(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[8:28])
    defparam insn_reset_bdd_4_lut_4_lut_4_lut_3_lut.init = 16'h9191;
    LUT4 i1_2_lut_3_lut_3_lut_3_lut (.A(n9413), .B(n9414), .C(n8832), 
         .Z(n5)) /* synthesis lut_function=(A (C)+!A !(B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[8:28])
    defparam i1_2_lut_3_lut_3_lut_3_lut.init = 16'hb5b5;
    LUT4 i6248_2_lut_rep_129_2_lut_3_lut (.A(n9413), .B(n9414), .C(n8832), 
         .Z(n8796)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[8:28])
    defparam i6248_2_lut_rep_129_2_lut_3_lut.init = 16'h0404;
    LUT4 i6290_2_lut_2_lut_3_lut_4_lut_3_lut_4_lut (.A(n9413), .B(n9414), 
         .C(n8828), .D(n8832), .Z(n7781)) /* synthesis lut_function=(!(A ((D)+!C)+!A !(B (C+(D))+!B !((D)+!C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[8:28])
    defparam i6290_2_lut_2_lut_3_lut_4_lut_3_lut_4_lut.init = 16'h44f0;
    LUT4 i1272_4_lut (.A(n8896), .B(s_drscan_state[3]), .C(n4405), .D(s_end_state[1]), 
         .Z(n3364)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1316[3] 1381[12])
    defparam i1272_4_lut.init = 16'hc0ca;
    PFUMX s_tap_state_next_3__I_0_225_i4 (.BLUT(s_tap_state_next_3__N_1438[3]), 
          .ALUT(s_goto_state_c[3]), .C0(n8817), .Z(s_tap_state_next[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    LUT4 i6205_4_lut (.A(n126), .B(n3364), .C(n4405), .D(s_end_state[3]), 
         .Z(s_goto_state_c[3])) /* synthesis lut_function=(A (B+!(C+(D)))+!A (B (C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1458[22] 1465[15])
    defparam i6205_4_lut.init = 16'hccca;
    LUT4 i6270_2_lut_3_lut_3_lut_3_lut_3_lut_4_lut_3_lut (.A(n9413), .B(n9414), 
         .C(n8832), .Z(n7812)) /* synthesis lut_function=(!(A (B+!(C))+!A ((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6270_2_lut_3_lut_3_lut_3_lut_3_lut_4_lut_3_lut.init = 16'h2424;
    PFUMX s_tap_state_next_3__I_0_i4 (.BLUT(mtms_N_1486[11]), .ALUT(mtms_N_1486[10]), 
          .C0(s_tap_state_next[0]), .Z(n4)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    LUT4 i6256_2_lut_rep_134_2_lut_3_lut (.A(n9413), .B(n9414), .C(n8832), 
         .Z(n8801)) /* synthesis lut_function=(!((B+!(C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6256_2_lut_rep_134_2_lut_3_lut.init = 16'h2020;
    LUT4 i3433_2_lut_3_lut (.A(n9413), .B(n9414), .C(n5312), .Z(n5526)) /* synthesis lut_function=(A (B (C))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i3433_2_lut_3_lut.init = 16'hd0d0;
    LUT4 i6288_2_lut_rep_133_2_lut_3_lut (.A(n9413), .B(n9414), .C(n8832), 
         .Z(n8800)) /* synthesis lut_function=(!((B+(C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6288_2_lut_rep_133_2_lut_3_lut.init = 16'h0202;
    PFUMX i6661 (.BLUT(n8894), .ALUT(n8895), .C0(\s_status[1] ), .Z(n8896));
    
endmodule
//
// Verilog Description of module \jtag_enh_ckgen(16) 
//

module \jtag_enh_ckgen(16)  (mclk_c, s_ckgen_div, GND_net, \s_irq[1] , 
            s_ckgen_rtdiv, s_tcken, \s_cfg[5] , tp_rtck_c, tp_tck_c, 
            \s_irq_en[1] , \irq_N_8[1] , mclk_c_enable_104, s_tdien, 
            n7643);
    input mclk_c;
    input [15:0]s_ckgen_div;
    input GND_net;
    output \s_irq[1] ;
    input [15:0]s_ckgen_rtdiv;
    input s_tcken;
    input \s_cfg[5] ;
    input tp_rtck_c;
    output tp_tck_c;
    input \s_irq_en[1] ;
    output \irq_N_8[1] ;
    output mclk_c_enable_104;
    input s_tdien;
    output n7643;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    wire mclk_c_enable_104 /* synthesis is_clock=1, SET_AS_NETWORK=mclk_c_enable_104 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    wire [14:0]s_cnt_lo_r;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(71[9:19])
    
    wire mclk_c_enable_261, n4427;
    wire [14:0]n65;
    
    wire n7026;
    wire [14:0]s_div_hi;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(67[9:17])
    wire [16:0]s_div_lo_r0_14__N_475;
    
    wire n7027;
    wire [14:0]s_cnt_hi_r;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(72[9:19])
    
    wire mclk_c_enable_128, n4466;
    wire [14:0]n65_adj_1589;
    
    wire n7025, n7024, n7022, n7021, n6957;
    wire [14:0]s_cmp_hi;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(73[9:17])
    
    wire n6958, n7020, n7019, n7018, n6956, n6951;
    wire [14:0]s_cmp_lo;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(74[9:17])
    
    wire n6952, n7017, s_enabled, s_tck_r0, s_rtcken_reg, n2225, 
        n7016, n6955;
    wire [14:0]n194;
    
    wire mclk_c_enable_35, n8861, n8797, mclk_c_enable_85;
    wire [15:0]s_div_lo_r0;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(62[9:20])
    wire [15:0]s_rtdiv_hi_r0;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(65[9:22])
    wire [15:0]s_rtdiv_lo_r0;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(64[9:22])
    wire [16:0]s_rtdiv_lo_r0_14__N_490;
    wire [15:0]s_div_hi_r0;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(63[9:20])
    wire [14:0]s_div_lo;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(66[9:17])
    wire [14:0]s_rtdiv_hi;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(69[9:19])
    wire [14:0]s_rtdiv_lo;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(68[9:19])
    
    wire s_tcken_r0, s_tcken_r1, s_rtcken_r0, s_rtcken_r1, s_rtck_r0, 
        n7009, n7010, n7008, mclk_c_enable_158;
    wire [14:0]s_cmp_lo_14__N_550;
    
    wire n7014, n7013, s_rtck_r1, n8858, s_tcki_r0, mclk_c_enable_90, 
        s_tcki_r0_N_639, s_rtck_r2, s_rtck_r1_N_660, n6921, s_tcki_r1, 
        s_tck_r0_N_638, rtcktmo_N_629, n8888, n7038, n7012, n7037, 
        n7011, n7036, n7035, n8887, n7034, n7033, n6954, n7032, 
        n7030, n7029, n6953, n7028;
    
    FD1P3IX s_cnt_lo_r_756__i12 (.D(n65[12]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[12])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i12.GSR = "ENABLED";
    CCU2D sub_92_add_2_7 (.A0(s_ckgen_div[5]), .B0(s_div_hi[5]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[6]), .B1(s_div_hi[6]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7026), .COUT(n7027), .S0(s_div_lo_r0_14__N_475[5]), 
          .S1(s_div_lo_r0_14__N_475[6]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_92_add_2_7.INIT0 = 16'h5999;
    defparam sub_92_add_2_7.INIT1 = 16'h5999;
    defparam sub_92_add_2_7.INJECT1_0 = "NO";
    defparam sub_92_add_2_7.INJECT1_1 = "NO";
    FD1P3IX s_cnt_hi_r_757__i4 (.D(n65_adj_1589[4]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[4])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i4.GSR = "ENABLED";
    CCU2D sub_92_add_2_5 (.A0(s_ckgen_div[3]), .B0(s_div_hi[3]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[4]), .B1(s_div_hi[4]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7025), .COUT(n7026), .S0(s_div_lo_r0_14__N_475[3]), 
          .S1(s_div_lo_r0_14__N_475[4]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_92_add_2_5.INIT0 = 16'h5999;
    defparam sub_92_add_2_5.INIT1 = 16'h5999;
    defparam sub_92_add_2_5.INJECT1_0 = "NO";
    defparam sub_92_add_2_5.INJECT1_1 = "NO";
    CCU2D sub_92_add_2_3 (.A0(s_ckgen_div[1]), .B0(s_div_hi[1]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[2]), .B1(s_div_hi[2]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7024), .COUT(n7025), .S0(s_div_lo_r0_14__N_475[1]), 
          .S1(s_div_lo_r0_14__N_475[2]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_92_add_2_3.INIT0 = 16'h5999;
    defparam sub_92_add_2_3.INIT1 = 16'h5999;
    defparam sub_92_add_2_3.INJECT1_0 = "NO";
    defparam sub_92_add_2_3.INJECT1_1 = "NO";
    CCU2D sub_92_add_2_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_ckgen_div[0]), .B1(s_div_hi[0]), .C1(GND_net), .D1(GND_net), 
          .COUT(n7024), .S1(s_div_lo_r0_14__N_475[0]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_92_add_2_1.INIT0 = 16'h0000;
    defparam sub_92_add_2_1.INIT1 = 16'h5999;
    defparam sub_92_add_2_1.INJECT1_0 = "NO";
    defparam sub_92_add_2_1.INJECT1_1 = "NO";
    CCU2D s_cnt_hi_r_757_add_4_15 (.A0(s_cnt_hi_r[13]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[14]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7022), .S0(n65_adj_1589[13]), .S1(n65_adj_1589[14]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757_add_4_15.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_15.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_15.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_757_add_4_15.INJECT1_1 = "NO";
    CCU2D s_cnt_hi_r_757_add_4_13 (.A0(s_cnt_hi_r[11]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[12]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7021), .COUT(n7022), .S0(n65_adj_1589[11]), 
          .S1(n65_adj_1589[12]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757_add_4_13.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_13.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_13.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_757_add_4_13.INJECT1_1 = "NO";
    CCU2D s_cnt_hi_r_14__I_0_15_4975 (.A0(s_cmp_hi[3]), .B0(s_cnt_hi_r[3]), 
          .C0(s_cmp_hi[2]), .D0(s_cnt_hi_r[2]), .A1(s_cmp_hi[1]), .B1(s_cnt_hi_r[1]), 
          .C1(s_cmp_hi[0]), .D1(s_cnt_hi_r[0]), .CIN(n6957), .COUT(n6958));
    defparam s_cnt_hi_r_14__I_0_15_4975.INIT0 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_15_4975.INIT1 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_15_4975.INJECT1_0 = "YES";
    defparam s_cnt_hi_r_14__I_0_15_4975.INJECT1_1 = "YES";
    CCU2D s_cnt_hi_r_757_add_4_11 (.A0(s_cnt_hi_r[9]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[10]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7020), .COUT(n7021), .S0(n65_adj_1589[9]), 
          .S1(n65_adj_1589[10]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757_add_4_11.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_11.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_11.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_757_add_4_11.INJECT1_1 = "NO";
    FD1P3IX s_cnt_hi_r_757__i5 (.D(n65_adj_1589[5]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[5])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i5.GSR = "ENABLED";
    CCU2D s_cnt_hi_r_757_add_4_9 (.A0(s_cnt_hi_r[7]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[8]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7019), .COUT(n7020), .S0(n65_adj_1589[7]), 
          .S1(n65_adj_1589[8]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757_add_4_9.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_9.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_9.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_757_add_4_9.INJECT1_1 = "NO";
    CCU2D s_cnt_hi_r_757_add_4_7 (.A0(s_cnt_hi_r[5]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[6]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7018), .COUT(n7019), .S0(n65_adj_1589[5]), 
          .S1(n65_adj_1589[6]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757_add_4_7.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_7.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_7.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_757_add_4_7.INJECT1_1 = "NO";
    FD1P3IX s_cnt_lo_r_756__i13 (.D(n65[13]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[13])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i13.GSR = "ENABLED";
    CCU2D s_cnt_hi_r_14__I_0_13 (.A0(s_cmp_hi[7]), .B0(s_cnt_hi_r[7]), .C0(s_cmp_hi[6]), 
          .D0(s_cnt_hi_r[6]), .A1(s_cmp_hi[5]), .B1(s_cnt_hi_r[5]), .C1(s_cmp_hi[4]), 
          .D1(s_cnt_hi_r[4]), .CIN(n6956), .COUT(n6957));
    defparam s_cnt_hi_r_14__I_0_13.INIT0 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_13.INIT1 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_13.INJECT1_0 = "YES";
    defparam s_cnt_hi_r_14__I_0_13.INJECT1_1 = "YES";
    FD1P3IX s_cnt_lo_r_756__i14 (.D(n65[14]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[14])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i14.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_756__i11 (.D(n65[11]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[11])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i11.GSR = "ENABLED";
    CCU2D s_cnt_lo_r_14__I_0_11 (.A0(s_cmp_lo[11]), .B0(s_cnt_lo_r[11]), 
          .C0(s_cmp_lo[10]), .D0(s_cnt_lo_r[10]), .A1(s_cmp_lo[9]), .B1(s_cnt_lo_r[9]), 
          .C1(s_cmp_lo[8]), .D1(s_cnt_lo_r[8]), .CIN(n6951), .COUT(n6952));
    defparam s_cnt_lo_r_14__I_0_11.INIT0 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_11.INIT1 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_11.INJECT1_0 = "YES";
    defparam s_cnt_lo_r_14__I_0_11.INJECT1_1 = "YES";
    CCU2D s_cnt_hi_r_757_add_4_5 (.A0(s_cnt_hi_r[3]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[4]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7017), .COUT(n7018), .S0(n65_adj_1589[3]), 
          .S1(n65_adj_1589[4]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757_add_4_5.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_5.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_5.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_757_add_4_5.INJECT1_1 = "NO";
    LUT4 i1_3_lut_3_lut (.A(s_enabled), .B(s_tck_r0), .C(s_rtcken_reg), 
         .Z(n2225)) /* synthesis lut_function=(!(A ((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_3_lut_3_lut.init = 16'h5d5d;
    CCU2D s_cnt_hi_r_757_add_4_3 (.A0(s_cnt_hi_r[1]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[2]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7016), .COUT(n7017), .S0(n65_adj_1589[1]), 
          .S1(n65_adj_1589[2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757_add_4_3.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_3.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_757_add_4_3.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_757_add_4_3.INJECT1_1 = "NO";
    FD1P3IX s_cnt_hi_r_757__i3 (.D(n65_adj_1589[3]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[3])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i3.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_757__i2 (.D(n65_adj_1589[2]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[2])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i2.GSR = "ENABLED";
    CCU2D s_cnt_hi_r_757_add_4_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[0]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .COUT(n7016), .S1(n65_adj_1589[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757_add_4_1.INIT0 = 16'hF000;
    defparam s_cnt_hi_r_757_add_4_1.INIT1 = 16'h0555;
    defparam s_cnt_hi_r_757_add_4_1.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_757_add_4_1.INJECT1_1 = "NO";
    FD1P3IX s_cnt_hi_r_757__i1 (.D(n65_adj_1589[1]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[1])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i1.GSR = "ENABLED";
    CCU2D s_cnt_hi_r_14__I_0_11 (.A0(s_cmp_hi[11]), .B0(s_cnt_hi_r[11]), 
          .C0(s_cmp_hi[10]), .D0(s_cnt_hi_r[10]), .A1(s_cmp_hi[9]), .B1(s_cnt_hi_r[9]), 
          .C1(s_cmp_hi[8]), .D1(s_cnt_hi_r[8]), .CIN(n6955), .COUT(n6956));
    defparam s_cnt_hi_r_14__I_0_11.INIT0 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_11.INIT1 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_11.INJECT1_0 = "YES";
    defparam s_cnt_hi_r_14__I_0_11.INJECT1_1 = "YES";
    FD1P3AX s_cmp_hi_i0_i0 (.D(n194[0]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i0.GSR = "ENABLED";
    FD1P3IX s_rtck_tmo_117 (.D(s_rtcken_reg), .SP(mclk_c_enable_35), .CD(n8861), 
            .CK(mclk_c), .Q(\s_irq[1] ));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_rtck_tmo_117.GSR = "ENABLED";
    LUT4 i1_2_lut_3_lut_3_lut (.A(s_enabled), .B(n8797), .C(s_tck_r0), 
         .Z(mclk_c_enable_85)) /* synthesis lut_function=((B (C))+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut_3_lut_3_lut.init = 16'hd5d5;
    FD1S3AX s_div_lo_r0_i1 (.D(s_div_lo_r0_14__N_475[0]), .CK(mclk_c), .Q(s_div_lo_r0[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i1.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i1 (.D(s_ckgen_rtdiv[1]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i1.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i1 (.D(s_rtdiv_lo_r0_14__N_490[0]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i1.GSR = "ENABLED";
    FD1S3AX s_div_hi_i0 (.D(s_div_hi_r0[0]), .CK(mclk_c), .Q(s_div_hi[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i0.GSR = "ENABLED";
    FD1S3AX s_div_lo_i0 (.D(s_div_lo_r0[0]), .CK(mclk_c), .Q(s_div_lo[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i0.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i0 (.D(s_rtdiv_hi_r0[0]), .CK(mclk_c), .Q(s_rtdiv_hi[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i0.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i0 (.D(s_rtdiv_lo_r0[0]), .CK(mclk_c), .Q(s_rtdiv_lo[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i0.GSR = "ENABLED";
    FD1S3AX s_tcken_r0_106 (.D(s_tcken), .CK(mclk_c), .Q(s_tcken_r0)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(125[3] 135[10])
    defparam s_tcken_r0_106.GSR = "ENABLED";
    FD1S3AX s_tcken_r1_107 (.D(s_tcken_r0), .CK(mclk_c), .Q(s_tcken_r1)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(125[3] 135[10])
    defparam s_tcken_r1_107.GSR = "ENABLED";
    FD1S3AX s_rtcken_r0_108 (.D(\s_cfg[5] ), .CK(mclk_c), .Q(s_rtcken_r0)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(125[3] 135[10])
    defparam s_rtcken_r0_108.GSR = "ENABLED";
    FD1S3AX s_rtcken_r1_109 (.D(s_rtcken_r0), .CK(mclk_c), .Q(s_rtcken_r1)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(125[3] 135[10])
    defparam s_rtcken_r1_109.GSR = "ENABLED";
    FD1S3AX s_rtck_r0_110 (.D(tp_rtck_c), .CK(mclk_c), .Q(s_rtck_r0)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(144[3] 157[10])
    defparam s_rtck_r0_110.GSR = "ENABLED";
    CCU2D s_cnt_lo_r_756_add_4_5 (.A0(s_cnt_lo_r[3]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[4]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7009), .COUT(n7010), .S0(n65[3]), .S1(n65[4]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756_add_4_5.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_5.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_5.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_756_add_4_5.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_756_add_4_3 (.A0(s_cnt_lo_r[1]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[2]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7008), .COUT(n7009), .S0(n65[1]), .S1(n65[2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756_add_4_3.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_3.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_3.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_756_add_4_3.INJECT1_1 = "NO";
    FD1P3AX s_cmp_lo_i0 (.D(s_cmp_lo_14__N_550[0]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i0.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i1 (.D(s_ckgen_div[1]), .CK(mclk_c), .Q(s_div_hi_r0[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i1.GSR = "ENABLED";
    CCU2D s_cnt_lo_r_756_add_4_15 (.A0(s_cnt_lo_r[13]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[14]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7014), .S0(n65[13]), .S1(n65[14]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756_add_4_15.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_15.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_15.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_756_add_4_15.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_756_add_4_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[0]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .COUT(n7008), .S1(n65[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756_add_4_1.INIT0 = 16'hF000;
    defparam s_cnt_lo_r_756_add_4_1.INIT1 = 16'h0555;
    defparam s_cnt_lo_r_756_add_4_1.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_756_add_4_1.INJECT1_1 = "NO";
    FD1P3IX s_cnt_hi_r_757__i14 (.D(n65_adj_1589[14]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[14])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i14.GSR = "ENABLED";
    LUT4 i1_2_lut (.A(s_tck_r0), .B(s_enabled), .Z(mclk_c_enable_128)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut.init = 16'h8888;
    CCU2D s_cnt_lo_r_756_add_4_13 (.A0(s_cnt_lo_r[11]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[12]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7013), .COUT(n7014), .S0(n65[11]), .S1(n65[12]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756_add_4_13.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_13.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_13.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_756_add_4_13.INJECT1_1 = "NO";
    FD1P3IX s_cnt_hi_r_757__i13 (.D(n65_adj_1589[13]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[13])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i13.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_757__i12 (.D(n65_adj_1589[12]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[12])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i12.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_757__i11 (.D(n65_adj_1589[11]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[11])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i11.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_757__i10 (.D(n65_adj_1589[10]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[10])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i10.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_757__i9 (.D(n65_adj_1589[9]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[9])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i9.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_757__i8 (.D(n65_adj_1589[8]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[8])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i8.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_757__i7 (.D(n65_adj_1589[7]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[7])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i7.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_757__i6 (.D(n65_adj_1589[6]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[6])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i6.GSR = "ENABLED";
    FD1S3IX s_rtck_r1_111 (.D(s_rtck_r0), .CK(mclk_c), .CD(n8858), .Q(s_rtck_r1));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(144[3] 157[10])
    defparam s_rtck_r1_111.GSR = "ENABLED";
    FD1P3AX s_enabled_115 (.D(s_tcken_r1), .SP(mclk_c_enable_85), .CK(mclk_c), 
            .Q(s_enabled));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_enabled_115.GSR = "ENABLED";
    FD1P3AX s_tcki_r0_118 (.D(s_tcki_r0_N_639), .SP(mclk_c_enable_90), .CK(mclk_c), 
            .Q(s_tcki_r0)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_tcki_r0_118.GSR = "ENABLED";
    FD1S3IX s_rtck_r2_112 (.D(n6921), .CK(mclk_c), .CD(s_rtck_r1_N_660), 
            .Q(s_rtck_r2));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(144[3] 157[10])
    defparam s_rtck_r2_112.GSR = "ENABLED";
    FD1P3AX s_tck_r0_113 (.D(mclk_c_enable_261), .SP(mclk_c_enable_90), 
            .CK(mclk_c), .Q(s_tck_r0));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_tck_r0_113.GSR = "ENABLED";
    FD1S3IX s_tck_r1_114 (.D(s_tck_r0), .CK(mclk_c), .CD(n8861), .Q(tp_tck_c));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_tck_r1_114.GSR = "ENABLED";
    FD1S3IX s_rtcken_reg_116 (.D(s_rtcken_r1), .CK(mclk_c), .CD(n8861), 
            .Q(s_rtcken_reg));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_rtcken_reg_116.GSR = "ENABLED";
    FD1S3IX s_tcki_r1_119 (.D(s_tcki_r0), .CK(mclk_c), .CD(n8861), .Q(s_tcki_r1));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_tcki_r1_119.GSR = "ENABLED";
    LUT4 i1_2_lut_adj_171 (.A(\s_irq[1] ), .B(\s_irq_en[1] ), .Z(\irq_N_8[1] )) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut_adj_171.init = 16'h8888;
    FD1P3AX s_cmp_hi_i0_i1 (.D(n194[1]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i1.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i2 (.D(n194[2]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i2.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i3 (.D(n194[3]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i3.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i4 (.D(n194[4]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i4.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i5 (.D(n194[5]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i5.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i6 (.D(n194[6]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i6.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i7 (.D(n194[7]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i7.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i8 (.D(n194[8]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i8.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i9 (.D(n194[9]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i9.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i10 (.D(n194[10]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[10])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i10.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i11 (.D(n194[11]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[11])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i11.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i12 (.D(n194[12]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[12])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i12.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i13 (.D(n194[13]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[13])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i13.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i14 (.D(n194[14]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[14])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i14.GSR = "ENABLED";
    FD1S3AY s_div_lo_r0_i2 (.D(s_div_lo_r0_14__N_475[1]), .CK(mclk_c), .Q(s_div_lo_r0[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i2.GSR = "ENABLED";
    FD1S3AX s_div_lo_r0_i3 (.D(s_div_lo_r0_14__N_475[2]), .CK(mclk_c), .Q(s_div_lo_r0[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i3.GSR = "ENABLED";
    FD1S3AX s_div_lo_r0_i4 (.D(s_div_lo_r0_14__N_475[3]), .CK(mclk_c), .Q(s_div_lo_r0[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i4.GSR = "ENABLED";
    FD1S3AX s_div_lo_r0_i5 (.D(s_div_lo_r0_14__N_475[4]), .CK(mclk_c), .Q(s_div_lo_r0[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i5.GSR = "ENABLED";
    FD1S3AX s_div_lo_r0_i6 (.D(s_div_lo_r0_14__N_475[5]), .CK(mclk_c), .Q(s_div_lo_r0[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i6.GSR = "ENABLED";
    FD1S3AX s_div_lo_r0_i7 (.D(s_div_lo_r0_14__N_475[6]), .CK(mclk_c), .Q(s_div_lo_r0[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i7.GSR = "ENABLED";
    FD1S3AX s_div_lo_r0_i8 (.D(s_div_lo_r0_14__N_475[7]), .CK(mclk_c), .Q(s_div_lo_r0[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i8.GSR = "ENABLED";
    FD1S3AX s_div_lo_r0_i9 (.D(s_div_lo_r0_14__N_475[8]), .CK(mclk_c), .Q(s_div_lo_r0[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i9.GSR = "ENABLED";
    FD1S3AX s_div_lo_r0_i10 (.D(s_div_lo_r0_14__N_475[9]), .CK(mclk_c), 
            .Q(s_div_lo_r0[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i10.GSR = "ENABLED";
    FD1S3AX s_div_lo_r0_i11 (.D(s_div_lo_r0_14__N_475[10]), .CK(mclk_c), 
            .Q(s_div_lo_r0[10])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i11.GSR = "ENABLED";
    FD1S3AX s_div_lo_r0_i12 (.D(s_div_lo_r0_14__N_475[11]), .CK(mclk_c), 
            .Q(s_div_lo_r0[11])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i12.GSR = "ENABLED";
    FD1S3AX s_div_lo_r0_i13 (.D(s_div_lo_r0_14__N_475[12]), .CK(mclk_c), 
            .Q(s_div_lo_r0[12])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i13.GSR = "ENABLED";
    FD1S3AX s_div_lo_r0_i14 (.D(s_div_lo_r0_14__N_475[13]), .CK(mclk_c), 
            .Q(s_div_lo_r0[13])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i14.GSR = "ENABLED";
    FD1S3AX s_div_lo_r0_i15 (.D(s_div_lo_r0_14__N_475[14]), .CK(mclk_c), 
            .Q(s_div_lo_r0[14])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_r0_i15.GSR = "ENABLED";
    FD1S3AY s_rtdiv_hi_r0_i2 (.D(s_ckgen_rtdiv[2]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i2.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i3 (.D(s_ckgen_rtdiv[3]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i3.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i4 (.D(s_ckgen_rtdiv[4]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i4.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i5 (.D(s_ckgen_rtdiv[5]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i5.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i6 (.D(s_ckgen_rtdiv[6]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i6.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i7 (.D(s_ckgen_rtdiv[7]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i7.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i8 (.D(s_ckgen_rtdiv[8]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i8.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i9 (.D(s_ckgen_rtdiv[9]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i9.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i10 (.D(s_ckgen_rtdiv[10]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i10.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i11 (.D(s_ckgen_rtdiv[11]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[10])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i11.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i12 (.D(s_ckgen_rtdiv[12]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[11])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i12.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i13 (.D(s_ckgen_rtdiv[13]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[12])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i13.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i14 (.D(s_ckgen_rtdiv[14]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[13])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i14.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_r0_i15 (.D(s_ckgen_rtdiv[15]), .CK(mclk_c), .Q(s_rtdiv_hi_r0[14])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_r0_i15.GSR = "ENABLED";
    FD1S3AY s_rtdiv_lo_r0_i2 (.D(s_rtdiv_lo_r0_14__N_490[1]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i2.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i3 (.D(s_rtdiv_lo_r0_14__N_490[2]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i3.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i4 (.D(s_rtdiv_lo_r0_14__N_490[3]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i4.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i5 (.D(s_rtdiv_lo_r0_14__N_490[4]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i5.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i6 (.D(s_rtdiv_lo_r0_14__N_490[5]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i6.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i7 (.D(s_rtdiv_lo_r0_14__N_490[6]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i7.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i8 (.D(s_rtdiv_lo_r0_14__N_490[7]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i8.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i9 (.D(s_rtdiv_lo_r0_14__N_490[8]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i9.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i10 (.D(s_rtdiv_lo_r0_14__N_490[9]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i10.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i11 (.D(s_rtdiv_lo_r0_14__N_490[10]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[10])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i11.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i12 (.D(s_rtdiv_lo_r0_14__N_490[11]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[11])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i12.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i13 (.D(s_rtdiv_lo_r0_14__N_490[12]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[12])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i13.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i14 (.D(s_rtdiv_lo_r0_14__N_490[13]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[13])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i14.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_r0_i15 (.D(s_rtdiv_lo_r0_14__N_490[14]), .CK(mclk_c), 
            .Q(s_rtdiv_lo_r0[14])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_r0_i15.GSR = "ENABLED";
    FD1S3AY s_div_hi_i1 (.D(s_div_hi_r0[1]), .CK(mclk_c), .Q(s_div_hi[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i1.GSR = "ENABLED";
    FD1S3AX s_div_hi_i2 (.D(s_div_hi_r0[2]), .CK(mclk_c), .Q(s_div_hi[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i2.GSR = "ENABLED";
    FD1S3AX s_div_hi_i3 (.D(s_div_hi_r0[3]), .CK(mclk_c), .Q(s_div_hi[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i3.GSR = "ENABLED";
    FD1S3AX s_div_hi_i4 (.D(s_div_hi_r0[4]), .CK(mclk_c), .Q(s_div_hi[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i4.GSR = "ENABLED";
    FD1S3AX s_div_hi_i5 (.D(s_div_hi_r0[5]), .CK(mclk_c), .Q(s_div_hi[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i5.GSR = "ENABLED";
    FD1S3AX s_div_hi_i6 (.D(s_div_hi_r0[6]), .CK(mclk_c), .Q(s_div_hi[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i6.GSR = "ENABLED";
    FD1S3AX s_div_hi_i7 (.D(s_div_hi_r0[7]), .CK(mclk_c), .Q(s_div_hi[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i7.GSR = "ENABLED";
    FD1S3AX s_div_hi_i8 (.D(s_div_hi_r0[8]), .CK(mclk_c), .Q(s_div_hi[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i8.GSR = "ENABLED";
    FD1S3AX s_div_hi_i9 (.D(s_div_hi_r0[9]), .CK(mclk_c), .Q(s_div_hi[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i9.GSR = "ENABLED";
    FD1S3AX s_div_hi_i10 (.D(s_div_hi_r0[10]), .CK(mclk_c), .Q(s_div_hi[10])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i10.GSR = "ENABLED";
    FD1S3AX s_div_hi_i11 (.D(s_div_hi_r0[11]), .CK(mclk_c), .Q(s_div_hi[11])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i11.GSR = "ENABLED";
    FD1S3AX s_div_hi_i12 (.D(s_div_hi_r0[12]), .CK(mclk_c), .Q(s_div_hi[12])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i12.GSR = "ENABLED";
    FD1S3AX s_div_hi_i13 (.D(s_div_hi_r0[13]), .CK(mclk_c), .Q(s_div_hi[13])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i13.GSR = "ENABLED";
    FD1S3AX s_div_hi_i14 (.D(s_div_hi_r0[14]), .CK(mclk_c), .Q(s_div_hi[14])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_i14.GSR = "ENABLED";
    FD1S3AY s_div_lo_i1 (.D(s_div_lo_r0[1]), .CK(mclk_c), .Q(s_div_lo[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i1.GSR = "ENABLED";
    FD1S3AX s_div_lo_i2 (.D(s_div_lo_r0[2]), .CK(mclk_c), .Q(s_div_lo[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i2.GSR = "ENABLED";
    FD1S3AX s_div_lo_i3 (.D(s_div_lo_r0[3]), .CK(mclk_c), .Q(s_div_lo[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i3.GSR = "ENABLED";
    FD1S3AX s_div_lo_i4 (.D(s_div_lo_r0[4]), .CK(mclk_c), .Q(s_div_lo[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i4.GSR = "ENABLED";
    FD1S3AX s_div_lo_i5 (.D(s_div_lo_r0[5]), .CK(mclk_c), .Q(s_div_lo[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i5.GSR = "ENABLED";
    FD1S3AX s_div_lo_i6 (.D(s_div_lo_r0[6]), .CK(mclk_c), .Q(s_div_lo[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i6.GSR = "ENABLED";
    FD1S3AX s_div_lo_i7 (.D(s_div_lo_r0[7]), .CK(mclk_c), .Q(s_div_lo[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i7.GSR = "ENABLED";
    FD1S3AX s_div_lo_i8 (.D(s_div_lo_r0[8]), .CK(mclk_c), .Q(s_div_lo[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i8.GSR = "ENABLED";
    FD1S3AX s_div_lo_i9 (.D(s_div_lo_r0[9]), .CK(mclk_c), .Q(s_div_lo[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i9.GSR = "ENABLED";
    FD1S3AX s_div_lo_i10 (.D(s_div_lo_r0[10]), .CK(mclk_c), .Q(s_div_lo[10])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i10.GSR = "ENABLED";
    FD1S3AX s_div_lo_i11 (.D(s_div_lo_r0[11]), .CK(mclk_c), .Q(s_div_lo[11])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i11.GSR = "ENABLED";
    FD1S3AX s_div_lo_i12 (.D(s_div_lo_r0[12]), .CK(mclk_c), .Q(s_div_lo[12])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i12.GSR = "ENABLED";
    FD1S3AX s_div_lo_i13 (.D(s_div_lo_r0[13]), .CK(mclk_c), .Q(s_div_lo[13])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i13.GSR = "ENABLED";
    FD1S3AX s_div_lo_i14 (.D(s_div_lo_r0[14]), .CK(mclk_c), .Q(s_div_lo[14])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_lo_i14.GSR = "ENABLED";
    FD1S3AY s_rtdiv_hi_i1 (.D(s_rtdiv_hi_r0[1]), .CK(mclk_c), .Q(s_rtdiv_hi[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i1.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i2 (.D(s_rtdiv_hi_r0[2]), .CK(mclk_c), .Q(s_rtdiv_hi[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i2.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i3 (.D(s_rtdiv_hi_r0[3]), .CK(mclk_c), .Q(s_rtdiv_hi[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i3.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i4 (.D(s_rtdiv_hi_r0[4]), .CK(mclk_c), .Q(s_rtdiv_hi[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i4.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i5 (.D(s_rtdiv_hi_r0[5]), .CK(mclk_c), .Q(s_rtdiv_hi[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i5.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i6 (.D(s_rtdiv_hi_r0[6]), .CK(mclk_c), .Q(s_rtdiv_hi[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i6.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i7 (.D(s_rtdiv_hi_r0[7]), .CK(mclk_c), .Q(s_rtdiv_hi[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i7.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i8 (.D(s_rtdiv_hi_r0[8]), .CK(mclk_c), .Q(s_rtdiv_hi[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i8.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i9 (.D(s_rtdiv_hi_r0[9]), .CK(mclk_c), .Q(s_rtdiv_hi[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i9.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i10 (.D(s_rtdiv_hi_r0[10]), .CK(mclk_c), .Q(s_rtdiv_hi[10])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i10.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i11 (.D(s_rtdiv_hi_r0[11]), .CK(mclk_c), .Q(s_rtdiv_hi[11])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i11.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i12 (.D(s_rtdiv_hi_r0[12]), .CK(mclk_c), .Q(s_rtdiv_hi[12])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i12.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i13 (.D(s_rtdiv_hi_r0[13]), .CK(mclk_c), .Q(s_rtdiv_hi[13])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i13.GSR = "ENABLED";
    FD1S3AX s_rtdiv_hi_i14 (.D(s_rtdiv_hi_r0[14]), .CK(mclk_c), .Q(s_rtdiv_hi[14])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_hi_i14.GSR = "ENABLED";
    FD1S3AY s_rtdiv_lo_i1 (.D(s_rtdiv_lo_r0[1]), .CK(mclk_c), .Q(s_rtdiv_lo[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i1.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i2 (.D(s_rtdiv_lo_r0[2]), .CK(mclk_c), .Q(s_rtdiv_lo[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i2.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i3 (.D(s_rtdiv_lo_r0[3]), .CK(mclk_c), .Q(s_rtdiv_lo[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i3.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i4 (.D(s_rtdiv_lo_r0[4]), .CK(mclk_c), .Q(s_rtdiv_lo[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i4.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i5 (.D(s_rtdiv_lo_r0[5]), .CK(mclk_c), .Q(s_rtdiv_lo[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i5.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i6 (.D(s_rtdiv_lo_r0[6]), .CK(mclk_c), .Q(s_rtdiv_lo[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i6.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i7 (.D(s_rtdiv_lo_r0[7]), .CK(mclk_c), .Q(s_rtdiv_lo[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i7.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i8 (.D(s_rtdiv_lo_r0[8]), .CK(mclk_c), .Q(s_rtdiv_lo[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i8.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i9 (.D(s_rtdiv_lo_r0[9]), .CK(mclk_c), .Q(s_rtdiv_lo[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i9.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i10 (.D(s_rtdiv_lo_r0[10]), .CK(mclk_c), .Q(s_rtdiv_lo[10])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i10.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i11 (.D(s_rtdiv_lo_r0[11]), .CK(mclk_c), .Q(s_rtdiv_lo[11])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i11.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i12 (.D(s_rtdiv_lo_r0[12]), .CK(mclk_c), .Q(s_rtdiv_lo[12])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i12.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i13 (.D(s_rtdiv_lo_r0[13]), .CK(mclk_c), .Q(s_rtdiv_lo[13])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i13.GSR = "ENABLED";
    FD1S3AX s_rtdiv_lo_i14 (.D(s_rtdiv_lo_r0[14]), .CK(mclk_c), .Q(s_rtdiv_lo[14])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_rtdiv_lo_i14.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_757__i0 (.D(n65_adj_1589[0]), .SP(mclk_c_enable_128), 
            .CD(n4466), .CK(mclk_c), .Q(s_cnt_hi_r[0])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_757__i0.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_756__i0 (.D(n65[0]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[0])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i0.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_756__i1 (.D(n65[1]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[1])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i1.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_756__i2 (.D(n65[2]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[2])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i2.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_756__i3 (.D(n65[3]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[3])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i3.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_756__i4 (.D(n65[4]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[4])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i4.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_756__i5 (.D(n65[5]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[5])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i5.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_756__i6 (.D(n65[6]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[6])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i6.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i1 (.D(s_cmp_lo_14__N_550[1]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i1.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i2 (.D(s_cmp_lo_14__N_550[2]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i2.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i3 (.D(s_cmp_lo_14__N_550[3]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i3.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i4 (.D(s_cmp_lo_14__N_550[4]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i4.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i5 (.D(s_cmp_lo_14__N_550[5]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i5.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i6 (.D(s_cmp_lo_14__N_550[6]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i6.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i7 (.D(s_cmp_lo_14__N_550[7]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i7.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i8 (.D(s_cmp_lo_14__N_550[8]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i8.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i9 (.D(s_cmp_lo_14__N_550[9]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i9.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i10 (.D(s_cmp_lo_14__N_550[10]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[10])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i10.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i11 (.D(s_cmp_lo_14__N_550[11]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[11])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i11.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i12 (.D(s_cmp_lo_14__N_550[12]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[12])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i12.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i13 (.D(s_cmp_lo_14__N_550[13]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[13])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i13.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i14 (.D(s_cmp_lo_14__N_550[14]), .SP(mclk_c_enable_158), 
            .CK(mclk_c), .Q(s_cmp_lo[14])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i14.GSR = "ENABLED";
    FD1S3AY s_div_hi_r0_i2 (.D(s_ckgen_div[2]), .CK(mclk_c), .Q(s_div_hi_r0[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i2.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i3 (.D(s_ckgen_div[3]), .CK(mclk_c), .Q(s_div_hi_r0[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i3.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i4 (.D(s_ckgen_div[4]), .CK(mclk_c), .Q(s_div_hi_r0[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i4.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i5 (.D(s_ckgen_div[5]), .CK(mclk_c), .Q(s_div_hi_r0[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i5.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i6 (.D(s_ckgen_div[6]), .CK(mclk_c), .Q(s_div_hi_r0[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i6.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i7 (.D(s_ckgen_div[7]), .CK(mclk_c), .Q(s_div_hi_r0[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i7.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i8 (.D(s_ckgen_div[8]), .CK(mclk_c), .Q(s_div_hi_r0[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i8.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i9 (.D(s_ckgen_div[9]), .CK(mclk_c), .Q(s_div_hi_r0[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i9.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i10 (.D(s_ckgen_div[10]), .CK(mclk_c), .Q(s_div_hi_r0[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i10.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i11 (.D(s_ckgen_div[11]), .CK(mclk_c), .Q(s_div_hi_r0[10])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i11.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i12 (.D(s_ckgen_div[12]), .CK(mclk_c), .Q(s_div_hi_r0[11])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i12.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i13 (.D(s_ckgen_div[13]), .CK(mclk_c), .Q(s_div_hi_r0[12])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i13.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i14 (.D(s_ckgen_div[14]), .CK(mclk_c), .Q(s_div_hi_r0[13])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i14.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i15 (.D(s_ckgen_div[15]), .CK(mclk_c), .Q(s_div_hi_r0[14])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i15.GSR = "ENABLED";
    LUT4 mux_37_i1_3_lut (.A(s_div_hi[0]), .B(s_rtdiv_hi[0]), .C(s_rtcken_reg), 
         .Z(n194[0])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i1_3_lut.init = 16'hcaca;
    LUT4 i2340_2_lut_4_lut_4_lut (.A(mclk_c_enable_158), .B(s_rtck_r2), 
         .C(s_tck_r0_N_638), .D(s_rtck_r1), .Z(n4427)) /* synthesis lut_function=(!(A+!(B (C+!(D))+!B (C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i2340_2_lut_4_lut_4_lut.init = 16'h5054;
    LUT4 i1_2_lut_rep_191 (.A(s_rtcken_reg), .B(\s_irq[1] ), .Z(n8858)) /* synthesis lut_function=((B)+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut_rep_191.init = 16'hdddd;
    LUT4 i4943_1_lut_2_lut (.A(s_rtcken_reg), .B(\s_irq[1] ), .Z(n6921)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i4943_1_lut_2_lut.init = 16'h2222;
    LUT4 i2_3_lut_rep_192 (.A(s_tcki_r0), .B(s_tcki_r1), .C(s_enabled), 
         .Z(mclk_c_enable_104)) /* synthesis lut_function=(!((B+!(C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(255[12:36])
    defparam i2_3_lut_rep_192.init = 16'h2020;
    LUT4 i1_2_lut_4_lut (.A(s_tcki_r0), .B(s_tcki_r1), .C(s_enabled), 
         .D(s_tdien), .Z(n7643)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(255[12:36])
    defparam i1_2_lut_4_lut.init = 16'h0020;
    LUT4 s_div_lo_14__I_0_i1_4_lut (.A(s_rtdiv_lo[0]), .B(s_div_lo[0]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[0])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i1_4_lut.init = 16'hcac0;
    LUT4 i286_1_lut_rep_194 (.A(s_enabled), .Z(n8861)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i286_1_lut_rep_194.init = 16'h5555;
    FD1P3IX s_cnt_lo_r_756__i7 (.D(n65[7]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[7])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i7.GSR = "ENABLED";
    LUT4 i1_3_lut_rep_130 (.A(s_rtck_r1), .B(rtcktmo_N_629), .C(s_rtck_r2), 
         .Z(n8797)) /* synthesis lut_function=(A (B+!(C))+!A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(91[9:18])
    defparam i1_3_lut_rep_130.init = 16'hcece;
    LUT4 i2379_2_lut_4_lut (.A(s_rtck_r1), .B(rtcktmo_N_629), .C(s_rtck_r2), 
         .D(mclk_c_enable_128), .Z(n4466)) /* synthesis lut_function=(A (B (D)+!B !(C+!(D)))+!A (B (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(91[9:18])
    defparam i2379_2_lut_4_lut.init = 16'hce00;
    CCU2D s_cnt_hi_r_14__I_0_0 (.A0(s_cmp_hi[14]), .B0(s_cnt_hi_r[14]), 
          .C0(GND_net), .D0(GND_net), .A1(s_cmp_hi[13]), .B1(s_cnt_hi_r[13]), 
          .C1(s_cmp_hi[12]), .D1(s_cnt_hi_r[12]), .COUT(n6955));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(205[11:32])
    defparam s_cnt_hi_r_14__I_0_0.INIT0 = 16'h9000;
    defparam s_cnt_hi_r_14__I_0_0.INIT1 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_0.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_14__I_0_0.INJECT1_1 = "YES";
    LUT4 i2_4_lut_4_lut_4_lut_then_4_lut (.A(s_enabled), .B(s_rtck_r1), 
         .C(s_rtck_r2), .D(rtcktmo_N_629), .Z(n8888)) /* synthesis lut_function=((B ((D)+!C)+!B (D))+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i2_4_lut_4_lut_4_lut_then_4_lut.init = 16'hff5d;
    CCU2D sub_93_add_2_15 (.A0(s_ckgen_rtdiv[13]), .B0(s_rtdiv_hi[13]), 
          .C0(GND_net), .D0(GND_net), .A1(s_ckgen_rtdiv[14]), .B1(s_rtdiv_hi[14]), 
          .C1(GND_net), .D1(GND_net), .CIN(n7038), .S0(s_rtdiv_lo_r0_14__N_490[13]), 
          .S1(s_rtdiv_lo_r0_14__N_490[14]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_93_add_2_15.INIT0 = 16'h5999;
    defparam sub_93_add_2_15.INIT1 = 16'h5999;
    defparam sub_93_add_2_15.INJECT1_0 = "NO";
    defparam sub_93_add_2_15.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_756_add_4_11 (.A0(s_cnt_lo_r[9]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[10]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7012), .COUT(n7013), .S0(n65[9]), .S1(n65[10]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756_add_4_11.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_11.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_11.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_756_add_4_11.INJECT1_1 = "NO";
    CCU2D sub_93_add_2_13 (.A0(s_ckgen_rtdiv[11]), .B0(s_rtdiv_hi[11]), 
          .C0(GND_net), .D0(GND_net), .A1(s_ckgen_rtdiv[12]), .B1(s_rtdiv_hi[12]), 
          .C1(GND_net), .D1(GND_net), .CIN(n7037), .COUT(n7038), .S0(s_rtdiv_lo_r0_14__N_490[11]), 
          .S1(s_rtdiv_lo_r0_14__N_490[12]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_93_add_2_13.INIT0 = 16'h5999;
    defparam sub_93_add_2_13.INIT1 = 16'h5999;
    defparam sub_93_add_2_13.INJECT1_0 = "NO";
    defparam sub_93_add_2_13.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_756_add_4_9 (.A0(s_cnt_lo_r[7]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[8]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7011), .COUT(n7012), .S0(n65[7]), .S1(n65[8]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756_add_4_9.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_9.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_9.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_756_add_4_9.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_756_add_4_7 (.A0(s_cnt_lo_r[5]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[6]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7010), .COUT(n7011), .S0(n65[5]), .S1(n65[6]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756_add_4_7.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_7.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_756_add_4_7.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_756_add_4_7.INJECT1_1 = "NO";
    CCU2D sub_93_add_2_11 (.A0(s_ckgen_rtdiv[9]), .B0(s_rtdiv_hi[9]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_rtdiv[10]), .B1(s_rtdiv_hi[10]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7036), .COUT(n7037), .S0(s_rtdiv_lo_r0_14__N_490[9]), 
          .S1(s_rtdiv_lo_r0_14__N_490[10]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_93_add_2_11.INIT0 = 16'h5999;
    defparam sub_93_add_2_11.INIT1 = 16'h5999;
    defparam sub_93_add_2_11.INJECT1_0 = "NO";
    defparam sub_93_add_2_11.INJECT1_1 = "NO";
    CCU2D sub_93_add_2_9 (.A0(s_ckgen_rtdiv[7]), .B0(s_rtdiv_hi[7]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_rtdiv[8]), .B1(s_rtdiv_hi[8]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7035), .COUT(n7036), .S0(s_rtdiv_lo_r0_14__N_490[7]), 
          .S1(s_rtdiv_lo_r0_14__N_490[8]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_93_add_2_9.INIT0 = 16'h5999;
    defparam sub_93_add_2_9.INIT1 = 16'h5999;
    defparam sub_93_add_2_9.INJECT1_0 = "NO";
    defparam sub_93_add_2_9.INJECT1_1 = "NO";
    LUT4 i2_4_lut_4_lut_4_lut_else_4_lut (.A(s_enabled), .B(s_tck_r0_N_638), 
         .C(s_rtck_r1), .D(s_rtck_r2), .Z(n8887)) /* synthesis lut_function=((B+!(C+!(D)))+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i2_4_lut_4_lut_4_lut_else_4_lut.init = 16'hdfdd;
    CCU2D sub_93_add_2_7 (.A0(s_ckgen_rtdiv[5]), .B0(s_rtdiv_hi[5]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_rtdiv[6]), .B1(s_rtdiv_hi[6]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7034), .COUT(n7035), .S0(s_rtdiv_lo_r0_14__N_490[5]), 
          .S1(s_rtdiv_lo_r0_14__N_490[6]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_93_add_2_7.INIT0 = 16'h5999;
    defparam sub_93_add_2_7.INIT1 = 16'h5999;
    defparam sub_93_add_2_7.INJECT1_0 = "NO";
    defparam sub_93_add_2_7.INJECT1_1 = "NO";
    CCU2D sub_93_add_2_5 (.A0(s_ckgen_rtdiv[3]), .B0(s_rtdiv_hi[3]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_rtdiv[4]), .B1(s_rtdiv_hi[4]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7033), .COUT(n7034), .S0(s_rtdiv_lo_r0_14__N_490[3]), 
          .S1(s_rtdiv_lo_r0_14__N_490[4]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_93_add_2_5.INIT0 = 16'h5999;
    defparam sub_93_add_2_5.INIT1 = 16'h5999;
    defparam sub_93_add_2_5.INJECT1_0 = "NO";
    defparam sub_93_add_2_5.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_14__I_0_15 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n6954), .S0(s_tck_r0_N_638));
    defparam s_cnt_lo_r_14__I_0_15.INIT0 = 16'hFFFF;
    defparam s_cnt_lo_r_14__I_0_15.INIT1 = 16'h0000;
    defparam s_cnt_lo_r_14__I_0_15.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_14__I_0_15.INJECT1_1 = "NO";
    CCU2D sub_93_add_2_3 (.A0(s_ckgen_rtdiv[1]), .B0(s_rtdiv_hi[1]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_rtdiv[2]), .B1(s_rtdiv_hi[2]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7032), .COUT(n7033), .S0(s_rtdiv_lo_r0_14__N_490[1]), 
          .S1(s_rtdiv_lo_r0_14__N_490[2]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_93_add_2_3.INIT0 = 16'h5999;
    defparam sub_93_add_2_3.INIT1 = 16'h5999;
    defparam sub_93_add_2_3.INJECT1_0 = "NO";
    defparam sub_93_add_2_3.INJECT1_1 = "NO";
    CCU2D sub_93_add_2_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_ckgen_rtdiv[0]), .B1(s_rtdiv_hi[0]), .C1(GND_net), .D1(GND_net), 
          .COUT(n7032), .S1(s_rtdiv_lo_r0_14__N_490[0]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_93_add_2_1.INIT0 = 16'h0000;
    defparam sub_93_add_2_1.INIT1 = 16'h5999;
    defparam sub_93_add_2_1.INJECT1_0 = "NO";
    defparam sub_93_add_2_1.INJECT1_1 = "NO";
    CCU2D sub_92_add_2_15 (.A0(s_ckgen_div[13]), .B0(s_div_hi[13]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[14]), .B1(s_div_hi[14]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7030), .S0(s_div_lo_r0_14__N_475[13]), .S1(s_div_lo_r0_14__N_475[14]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_92_add_2_15.INIT0 = 16'h5999;
    defparam sub_92_add_2_15.INIT1 = 16'h5999;
    defparam sub_92_add_2_15.INJECT1_0 = "NO";
    defparam sub_92_add_2_15.INJECT1_1 = "NO";
    CCU2D sub_92_add_2_13 (.A0(s_ckgen_div[11]), .B0(s_div_hi[11]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[12]), .B1(s_div_hi[12]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7029), .COUT(n7030), .S0(s_div_lo_r0_14__N_475[11]), 
          .S1(s_div_lo_r0_14__N_475[12]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_92_add_2_13.INIT0 = 16'h5999;
    defparam sub_92_add_2_13.INIT1 = 16'h5999;
    defparam sub_92_add_2_13.INJECT1_0 = "NO";
    defparam sub_92_add_2_13.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_14__I_0_15_4974 (.A0(s_cmp_lo[3]), .B0(s_cnt_lo_r[3]), 
          .C0(s_cmp_lo[2]), .D0(s_cnt_lo_r[2]), .A1(s_cmp_lo[1]), .B1(s_cnt_lo_r[1]), 
          .C1(s_cmp_lo[0]), .D1(s_cnt_lo_r[0]), .CIN(n6953), .COUT(n6954));
    defparam s_cnt_lo_r_14__I_0_15_4974.INIT0 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_15_4974.INIT1 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_15_4974.INJECT1_0 = "YES";
    defparam s_cnt_lo_r_14__I_0_15_4974.INJECT1_1 = "YES";
    CCU2D s_cnt_lo_r_14__I_0_0 (.A0(s_cmp_lo[14]), .B0(s_cnt_lo_r[14]), 
          .C0(GND_net), .D0(GND_net), .A1(s_cmp_lo[13]), .B1(s_cnt_lo_r[13]), 
          .C1(s_cmp_lo[12]), .D1(s_cnt_lo_r[12]), .COUT(n6951));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(188[10:31])
    defparam s_cnt_lo_r_14__I_0_0.INIT0 = 16'h9000;
    defparam s_cnt_lo_r_14__I_0_0.INIT1 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_0.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_14__I_0_0.INJECT1_1 = "YES";
    LUT4 s_rtck_r1_I_0_140_1_lut (.A(s_rtck_r1), .Z(s_rtck_r1_N_660)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(247[17:30])
    defparam s_rtck_r1_I_0_140_1_lut.init = 16'h5555;
    FD1P3IX s_cnt_lo_r_756__i8 (.D(n65[8]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[8])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i8.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_756__i9 (.D(n65[9]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[9])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i9.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_756__i10 (.D(n65[10]), .SP(mclk_c_enable_261), .CD(n4427), 
            .CK(mclk_c), .Q(s_cnt_lo_r[10])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_756__i10.GSR = "ENABLED";
    CCU2D s_cnt_lo_r_14__I_0_13 (.A0(s_cmp_lo[7]), .B0(s_cnt_lo_r[7]), .C0(s_cmp_lo[6]), 
          .D0(s_cnt_lo_r[6]), .A1(s_cmp_lo[5]), .B1(s_cnt_lo_r[5]), .C1(s_cmp_lo[4]), 
          .D1(s_cnt_lo_r[4]), .CIN(n6952), .COUT(n6953));
    defparam s_cnt_lo_r_14__I_0_13.INIT0 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_13.INIT1 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_13.INJECT1_0 = "YES";
    defparam s_cnt_lo_r_14__I_0_13.INJECT1_1 = "YES";
    CCU2D sub_92_add_2_11 (.A0(s_ckgen_div[9]), .B0(s_div_hi[9]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[10]), .B1(s_div_hi[10]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7028), .COUT(n7029), .S0(s_div_lo_r0_14__N_475[9]), 
          .S1(s_div_lo_r0_14__N_475[10]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_92_add_2_11.INIT0 = 16'h5999;
    defparam sub_92_add_2_11.INIT1 = 16'h5999;
    defparam sub_92_add_2_11.INJECT1_0 = "NO";
    defparam sub_92_add_2_11.INJECT1_1 = "NO";
    LUT4 i1_2_lut_rep_212 (.A(s_tck_r0), .B(s_enabled), .Z(mclk_c_enable_158)) /* synthesis lut_function=(A+!(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut_rep_212.init = 16'hbbbb;
    LUT4 i1_2_lut_3_lut (.A(s_tck_r0), .B(s_enabled), .C(s_tcken_r1), 
         .Z(s_tcki_r0_N_639)) /* synthesis lut_function=(A (C)+!A !(B+!(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut_3_lut.init = 16'hb0b0;
    CCU2D s_cnt_hi_r_14__I_0_15 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n6958), .S0(rtcktmo_N_629));
    defparam s_cnt_hi_r_14__I_0_15.INIT0 = 16'hFFFF;
    defparam s_cnt_hi_r_14__I_0_15.INIT1 = 16'h0000;
    defparam s_cnt_hi_r_14__I_0_15.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_14__I_0_15.INJECT1_1 = "NO";
    LUT4 i1_2_lut_3_lut_adj_172 (.A(s_tck_r0), .B(s_enabled), .C(rtcktmo_N_629), 
         .Z(mclk_c_enable_35)) /* synthesis lut_function=(A (C)+!A !(B+!(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut_3_lut_adj_172.init = 16'hb0b0;
    LUT4 i4941_1_lut_rep_182_2_lut (.A(s_tck_r0), .B(s_enabled), .Z(mclk_c_enable_261)) /* synthesis lut_function=(!(A+!(B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i4941_1_lut_rep_182_2_lut.init = 16'h4444;
    CCU2D sub_92_add_2_9 (.A0(s_ckgen_div[7]), .B0(s_div_hi[7]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[8]), .B1(s_div_hi[8]), .C1(GND_net), 
          .D1(GND_net), .CIN(n7027), .COUT(n7028), .S0(s_div_lo_r0_14__N_475[7]), 
          .S1(s_div_lo_r0_14__N_475[8]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_92_add_2_9.INIT0 = 16'h5999;
    defparam sub_92_add_2_9.INIT1 = 16'h5999;
    defparam sub_92_add_2_9.INJECT1_0 = "NO";
    defparam sub_92_add_2_9.INJECT1_1 = "NO";
    PFUMX i6657 (.BLUT(n8887), .ALUT(n8888), .C0(s_tck_r0), .Z(mclk_c_enable_90));
    LUT4 mux_37_i2_3_lut (.A(s_div_hi[1]), .B(s_rtdiv_hi[1]), .C(s_rtcken_reg), 
         .Z(n194[1])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i2_3_lut.init = 16'hcaca;
    LUT4 mux_37_i3_3_lut (.A(s_div_hi[2]), .B(s_rtdiv_hi[2]), .C(s_rtcken_reg), 
         .Z(n194[2])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i3_3_lut.init = 16'hcaca;
    LUT4 mux_37_i4_3_lut (.A(s_div_hi[3]), .B(s_rtdiv_hi[3]), .C(s_rtcken_reg), 
         .Z(n194[3])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i4_3_lut.init = 16'hcaca;
    LUT4 mux_37_i5_3_lut (.A(s_div_hi[4]), .B(s_rtdiv_hi[4]), .C(s_rtcken_reg), 
         .Z(n194[4])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i5_3_lut.init = 16'hcaca;
    LUT4 mux_37_i6_3_lut (.A(s_div_hi[5]), .B(s_rtdiv_hi[5]), .C(s_rtcken_reg), 
         .Z(n194[5])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i6_3_lut.init = 16'hcaca;
    LUT4 mux_37_i7_3_lut (.A(s_div_hi[6]), .B(s_rtdiv_hi[6]), .C(s_rtcken_reg), 
         .Z(n194[6])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i7_3_lut.init = 16'hcaca;
    LUT4 mux_37_i8_3_lut (.A(s_div_hi[7]), .B(s_rtdiv_hi[7]), .C(s_rtcken_reg), 
         .Z(n194[7])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i8_3_lut.init = 16'hcaca;
    LUT4 mux_37_i9_3_lut (.A(s_div_hi[8]), .B(s_rtdiv_hi[8]), .C(s_rtcken_reg), 
         .Z(n194[8])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i9_3_lut.init = 16'hcaca;
    LUT4 mux_37_i10_3_lut (.A(s_div_hi[9]), .B(s_rtdiv_hi[9]), .C(s_rtcken_reg), 
         .Z(n194[9])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i10_3_lut.init = 16'hcaca;
    LUT4 mux_37_i11_3_lut (.A(s_div_hi[10]), .B(s_rtdiv_hi[10]), .C(s_rtcken_reg), 
         .Z(n194[10])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i11_3_lut.init = 16'hcaca;
    LUT4 mux_37_i12_3_lut (.A(s_div_hi[11]), .B(s_rtdiv_hi[11]), .C(s_rtcken_reg), 
         .Z(n194[11])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i12_3_lut.init = 16'hcaca;
    LUT4 mux_37_i13_3_lut (.A(s_div_hi[12]), .B(s_rtdiv_hi[12]), .C(s_rtcken_reg), 
         .Z(n194[12])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i13_3_lut.init = 16'hcaca;
    LUT4 mux_37_i14_3_lut (.A(s_div_hi[13]), .B(s_rtdiv_hi[13]), .C(s_rtcken_reg), 
         .Z(n194[13])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i14_3_lut.init = 16'hcaca;
    LUT4 mux_37_i15_3_lut (.A(s_div_hi[14]), .B(s_rtdiv_hi[14]), .C(s_rtcken_reg), 
         .Z(n194[14])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(181[6] 186[13])
    defparam mux_37_i15_3_lut.init = 16'hcaca;
    LUT4 s_div_lo_14__I_0_i2_4_lut (.A(s_rtdiv_lo[1]), .B(s_div_lo[1]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[1])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i2_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i3_4_lut (.A(s_rtdiv_lo[2]), .B(s_div_lo[2]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[2])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i3_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i4_4_lut (.A(s_rtdiv_lo[3]), .B(s_div_lo[3]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[3])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i4_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i5_4_lut (.A(s_rtdiv_lo[4]), .B(s_div_lo[4]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[4])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i5_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i6_4_lut (.A(s_rtdiv_lo[5]), .B(s_div_lo[5]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[5])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i6_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i7_4_lut (.A(s_rtdiv_lo[6]), .B(s_div_lo[6]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[6])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i7_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i8_4_lut (.A(s_rtdiv_lo[7]), .B(s_div_lo[7]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[7])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i8_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i9_4_lut (.A(s_rtdiv_lo[8]), .B(s_div_lo[8]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[8])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i9_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i10_4_lut (.A(s_rtdiv_lo[9]), .B(s_div_lo[9]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[9])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i10_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i11_4_lut (.A(s_rtdiv_lo[10]), .B(s_div_lo[10]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[10])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i11_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i12_4_lut (.A(s_rtdiv_lo[11]), .B(s_div_lo[11]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[11])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i12_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i13_4_lut (.A(s_rtdiv_lo[12]), .B(s_div_lo[12]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[12])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i13_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i14_4_lut (.A(s_rtdiv_lo[13]), .B(s_div_lo[13]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[13])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i14_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i15_4_lut (.A(s_rtdiv_lo[14]), .B(s_div_lo[14]), 
         .C(n2225), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[14])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i15_4_lut.init = 16'hcac0;
    
endmodule
//
// Verilog Description of module \jtag_clk_scaler(60) 
//

module \jtag_clk_scaler(60)  (mclk_c, s_1mhz_stb, GND_net);
    input mclk_c;
    output s_1mhz_stb;
    input GND_net;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    wire [5:0]s_count;   // c:/devel/yard-ice/src/fpga/jtag_clk_scaler.vhdl(54[9:16])
    
    wire n4422;
    wire [5:0]n23;
    
    wire s_hi, n7067, n7000, n7002, n7001;
    
    FD1S3IX s_count_753__i5 (.D(n23[5]), .CK(mclk_c), .CD(n4422), .Q(s_count[5])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i5.GSR = "ENABLED";
    FD1S3IX s_count_753__i4 (.D(n23[4]), .CK(mclk_c), .CD(n4422), .Q(s_count[4])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i4.GSR = "ENABLED";
    FD1S3IX s_count_753__i3 (.D(n23[3]), .CK(mclk_c), .CD(n4422), .Q(s_count[3])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i3.GSR = "ENABLED";
    FD1S3IX s_count_753__i2 (.D(n23[2]), .CK(mclk_c), .CD(n4422), .Q(s_count[2])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i2.GSR = "ENABLED";
    FD1S3IX s_count_753__i1 (.D(n23[1]), .CK(mclk_c), .CD(n4422), .Q(s_count[1])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i1.GSR = "ENABLED";
    FD1S3AX p_stb_26 (.D(s_hi), .CK(mclk_c), .Q(s_1mhz_stb)) /* synthesis LSE_LINE_FILE_ID=41, LSE_LCOL=16, LSE_RCOL=38, LSE_LLINE=1010, LSE_RLINE=1010 */ ;   // c:/devel/yard-ice/src/fpga/jtag_clk_scaler.vhdl(87[3] 93[10])
    defparam p_stb_26.GSR = "ENABLED";
    FD1S3IX s_count_753__i0 (.D(n23[0]), .CK(mclk_c), .CD(n4422), .Q(s_count[0])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i0.GSR = "ENABLED";
    LUT4 i6263_4_lut (.A(s_count[1]), .B(n7067), .C(s_count[5]), .D(s_count[2]), 
         .Z(n4422)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i6263_4_lut.init = 16'h0080;
    LUT4 i2_3_lut (.A(s_count[3]), .B(s_count[4]), .C(s_count[0]), .Z(n7067)) /* synthesis lut_function=(A (B (C))) */ ;
    defparam i2_3_lut.init = 16'h8080;
    LUT4 i6229_4_lut (.A(s_count[1]), .B(n7067), .C(s_count[2]), .D(s_count[5]), 
         .Z(s_hi)) /* synthesis lut_function=(!(A+(((D)+!C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_clk_scaler.vhdl(62[23:45])
    defparam i6229_4_lut.init = 16'h0040;
    CCU2D s_count_753_add_4_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[0]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .COUT(n7000), .S1(n23[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753_add_4_1.INIT0 = 16'hF000;
    defparam s_count_753_add_4_1.INIT1 = 16'h0555;
    defparam s_count_753_add_4_1.INJECT1_0 = "NO";
    defparam s_count_753_add_4_1.INJECT1_1 = "NO";
    CCU2D s_count_753_add_4_7 (.A0(s_count[5]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n7002), .S0(n23[5]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753_add_4_7.INIT0 = 16'hfaaa;
    defparam s_count_753_add_4_7.INIT1 = 16'h0000;
    defparam s_count_753_add_4_7.INJECT1_0 = "NO";
    defparam s_count_753_add_4_7.INJECT1_1 = "NO";
    CCU2D s_count_753_add_4_5 (.A0(s_count[3]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[4]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7001), .COUT(n7002), .S0(n23[3]), .S1(n23[4]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753_add_4_5.INIT0 = 16'hfaaa;
    defparam s_count_753_add_4_5.INIT1 = 16'hfaaa;
    defparam s_count_753_add_4_5.INJECT1_0 = "NO";
    defparam s_count_753_add_4_5.INJECT1_1 = "NO";
    CCU2D s_count_753_add_4_3 (.A0(s_count[1]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[2]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n7000), .COUT(n7001), .S0(n23[1]), .S1(n23[2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753_add_4_3.INIT0 = 16'hfaaa;
    defparam s_count_753_add_4_3.INIT1 = 16'hfaaa;
    defparam s_count_753_add_4_3.INJECT1_0 = "NO";
    defparam s_count_753_add_4_3.INJECT1_1 = "NO";
    
endmodule
//
// Verilog Description of module \dpram(16,10)_U1 
//

module \dpram(16,10)_U1  (mclk_c, fsmc_clk_c, VCC_net, GND_net, s_rx_mem_addr, 
            q_15__N_82, q_15__N_80, q_15__N_78, q_15__N_76, q_15__N_74, 
            q_15__N_72, q_15__N_70, q_15__N_68, q_15__N_66, s_rx_mem_din, 
            n2957, n2958, n2959, n2960, n2961, n2962, n2963, n2964, 
            n2965, n2966, n2967, n2968, n2969, n2970, n2971, n2972, 
            n2941, n2942, n2943, n2944, n2945, n2946, n2947, n2948, 
            n2949, n2950, n2951, n2952, n2953, n2954, n2955, n2956, 
            n2973, q_15__N_64, s_rx_mem_wr_stb);
    input mclk_c;
    input fsmc_clk_c;
    input VCC_net;
    input GND_net;
    input [9:0]s_rx_mem_addr;
    input q_15__N_82;
    input q_15__N_80;
    input q_15__N_78;
    input q_15__N_76;
    input q_15__N_74;
    input q_15__N_72;
    input q_15__N_70;
    input q_15__N_68;
    input q_15__N_66;
    input [15:0]s_rx_mem_din;
    output n2957;
    output n2958;
    output n2959;
    output n2960;
    output n2961;
    output n2962;
    output n2963;
    output n2964;
    output n2965;
    output n2966;
    output n2967;
    output n2968;
    output n2969;
    output n2970;
    output n2971;
    output n2972;
    output n2941;
    output n2942;
    output n2943;
    output n2944;
    output n2945;
    output n2946;
    output n2947;
    output n2948;
    output n2949;
    output n2950;
    output n2951;
    output n2952;
    output n2953;
    output n2954;
    output n2955;
    output n2956;
    output n2973;
    input q_15__N_64;
    input s_rx_mem_wr_stb;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(69[2:6])
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(73[2:10])
    
    wire mclk_c_enable_23, mclk_c_enable_31;
    
    PDPW8KC mem1 (.DI0(s_rx_mem_din[0]), .DI1(s_rx_mem_din[1]), .DI2(s_rx_mem_din[2]), 
            .DI3(s_rx_mem_din[3]), .DI4(s_rx_mem_din[4]), .DI5(s_rx_mem_din[5]), 
            .DI6(s_rx_mem_din[6]), .DI7(s_rx_mem_din[7]), .DI8(s_rx_mem_din[8]), 
            .DI9(s_rx_mem_din[9]), .DI10(s_rx_mem_din[10]), .DI11(s_rx_mem_din[11]), 
            .DI12(s_rx_mem_din[12]), .DI13(s_rx_mem_din[13]), .DI14(s_rx_mem_din[14]), 
            .DI15(s_rx_mem_din[15]), .DI16(GND_net), .DI17(GND_net), .ADW0(s_rx_mem_addr[0]), 
            .ADW1(s_rx_mem_addr[1]), .ADW2(s_rx_mem_addr[2]), .ADW3(s_rx_mem_addr[3]), 
            .ADW4(s_rx_mem_addr[4]), .ADW5(s_rx_mem_addr[5]), .ADW6(s_rx_mem_addr[6]), 
            .ADW7(s_rx_mem_addr[7]), .ADW8(s_rx_mem_addr[8]), .BE0(VCC_net), 
            .BE1(VCC_net), .CEW(mclk_c_enable_23), .CLKW(mclk_c), .CSW0(GND_net), 
            .CSW1(GND_net), .CSW2(GND_net), .ADR0(GND_net), .ADR1(GND_net), 
            .ADR2(GND_net), .ADR3(GND_net), .ADR4(q_15__N_82), .ADR5(q_15__N_80), 
            .ADR6(q_15__N_78), .ADR7(q_15__N_76), .ADR8(q_15__N_74), .ADR9(q_15__N_72), 
            .ADR10(q_15__N_70), .ADR11(q_15__N_68), .ADR12(q_15__N_66), 
            .CER(VCC_net), .OCER(VCC_net), .CLKR(fsmc_clk_c), .CSR0(GND_net), 
            .CSR1(GND_net), .CSR2(GND_net), .RST(GND_net), .DO0(n2966), 
            .DO1(n2967), .DO2(n2968), .DO3(n2969), .DO4(n2970), .DO5(n2971), 
            .DO6(n2972), .DO9(n2957), .DO10(n2958), .DO11(n2959), .DO12(n2960), 
            .DO13(n2961), .DO14(n2962), .DO15(n2963), .DO16(n2964), 
            .DO17(n2965));
    defparam mem1.DATA_WIDTH_W = 18;
    defparam mem1.DATA_WIDTH_R = 18;
    defparam mem1.REGMODE = "NOREG";
    defparam mem1.CSDECODE_W = "0b000";
    defparam mem1.CSDECODE_R = "0b000";
    defparam mem1.GSR = "DISABLED";
    defparam mem1.RESETMODE = "SYNC";
    defparam mem1.ASYNC_RESET_RELEASE = "SYNC";
    defparam mem1.INIT_DATA = "STATIC";
    defparam mem1.INITVAL_00 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_01 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_02 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_03 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_04 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_05 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_06 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_07 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_08 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_09 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_0A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_0B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_0C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_0D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_0E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_0F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_10 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_11 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_12 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_13 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_14 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_15 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_16 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_17 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_18 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_19 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_1A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_1B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_1C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_1D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_1E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem1.INITVAL_1F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    PDPW8KC mem0 (.DI0(s_rx_mem_din[0]), .DI1(s_rx_mem_din[1]), .DI2(s_rx_mem_din[2]), 
            .DI3(s_rx_mem_din[3]), .DI4(s_rx_mem_din[4]), .DI5(s_rx_mem_din[5]), 
            .DI6(s_rx_mem_din[6]), .DI7(s_rx_mem_din[7]), .DI8(s_rx_mem_din[8]), 
            .DI9(s_rx_mem_din[9]), .DI10(s_rx_mem_din[10]), .DI11(s_rx_mem_din[11]), 
            .DI12(s_rx_mem_din[12]), .DI13(s_rx_mem_din[13]), .DI14(s_rx_mem_din[14]), 
            .DI15(s_rx_mem_din[15]), .DI16(GND_net), .DI17(GND_net), .ADW0(s_rx_mem_addr[0]), 
            .ADW1(s_rx_mem_addr[1]), .ADW2(s_rx_mem_addr[2]), .ADW3(s_rx_mem_addr[3]), 
            .ADW4(s_rx_mem_addr[4]), .ADW5(s_rx_mem_addr[5]), .ADW6(s_rx_mem_addr[6]), 
            .ADW7(s_rx_mem_addr[7]), .ADW8(s_rx_mem_addr[8]), .BE0(VCC_net), 
            .BE1(VCC_net), .CEW(mclk_c_enable_31), .CLKW(mclk_c), .CSW0(GND_net), 
            .CSW1(GND_net), .CSW2(GND_net), .ADR0(GND_net), .ADR1(GND_net), 
            .ADR2(GND_net), .ADR3(GND_net), .ADR4(q_15__N_82), .ADR5(q_15__N_80), 
            .ADR6(q_15__N_78), .ADR7(q_15__N_76), .ADR8(q_15__N_74), .ADR9(q_15__N_72), 
            .ADR10(q_15__N_70), .ADR11(q_15__N_68), .ADR12(q_15__N_66), 
            .CER(VCC_net), .OCER(VCC_net), .CLKR(fsmc_clk_c), .CSR0(GND_net), 
            .CSR1(GND_net), .CSR2(GND_net), .RST(GND_net), .DO0(n2950), 
            .DO1(n2951), .DO2(n2952), .DO3(n2953), .DO4(n2954), .DO5(n2955), 
            .DO6(n2956), .DO9(n2941), .DO10(n2942), .DO11(n2943), .DO12(n2944), 
            .DO13(n2945), .DO14(n2946), .DO15(n2947), .DO16(n2948), 
            .DO17(n2949));
    defparam mem0.DATA_WIDTH_W = 18;
    defparam mem0.DATA_WIDTH_R = 18;
    defparam mem0.REGMODE = "NOREG";
    defparam mem0.CSDECODE_W = "0b000";
    defparam mem0.CSDECODE_R = "0b000";
    defparam mem0.GSR = "DISABLED";
    defparam mem0.RESETMODE = "SYNC";
    defparam mem0.ASYNC_RESET_RELEASE = "SYNC";
    defparam mem0.INIT_DATA = "STATIC";
    defparam mem0.INITVAL_00 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_01 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_02 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_03 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_04 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_05 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_06 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_07 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_08 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_09 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_0F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_10 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_11 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_12 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_13 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_14 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_15 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_16 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_17 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_18 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_19 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam mem0.INITVAL_1F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    FD1S3AX _932 (.D(q_15__N_64), .CK(fsmc_clk_c), .Q(n2973));
    defparam _932.GSR = "DISABLED";
    LUT4 i928_2_lut (.A(s_rx_mem_wr_stb), .B(s_rx_mem_addr[9]), .Z(mclk_c_enable_23)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/dpram.vhdl(65[9:16])
    defparam i928_2_lut.init = 16'h8888;
    LUT4 i927_2_lut (.A(s_rx_mem_wr_stb), .B(s_rx_mem_addr[9]), .Z(mclk_c_enable_31)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/dpram.vhdl(65[9:16])
    defparam i927_2_lut.init = 16'h2222;
    
endmodule
