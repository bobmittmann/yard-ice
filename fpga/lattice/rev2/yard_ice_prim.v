// Verilog netlist produced by program LSE :  version Diamond (64-bit) 3.10.0.111.2
// Netlist written on Wed Apr 11 14:48:25 2018
//
// Verilog Description of module yard_ice
//

module yard_ice (mclk, fsmc_d, fsmc_clk, fsmc_noe, fsmc_nwe, fsmc_nce, 
            irq, uart_rx, uart_tx, tp_dbgrq, tp_dbgack, tp_tdo, 
            tp_tck, tp_tdi, tp_tms, tp_rtck, tp_trst, tp_rst, led_1, 
            led_2, clk_aux);   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(33[8:16])
    input mclk;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    inout [15:0]fsmc_d;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(38[2:8])
    input fsmc_clk;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(40[2:10])
    input fsmc_noe;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(41[2:10])
    input fsmc_nwe;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(42[2:10])
    input fsmc_nce;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(43[2:10])
    output irq;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(45[2:5])
    output uart_rx;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(47[2:9])
    input uart_tx;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(48[2:9])
    output tp_dbgrq;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(51[2:10])
    input tp_dbgack;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(52[2:11])
    input tp_tdo;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(54[2:8])
    output tp_tck;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(55[2:8])
    output tp_tdi;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(56[2:8])
    output tp_tms;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(57[2:8])
    input tp_rtck;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(59[2:9])
    output tp_trst;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(61[2:9])
    output tp_rst;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(63[2:8])
    output led_1;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(65[2:7])
    output led_2;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(66[2:7])
    input clk_aux;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(68[2:9])
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(40[2:10])
    
    wire GND_net, VCC_net, n8185, n7, n7368, n2844, fsmc_noe_c, 
        fsmc_nce_c, irq_c, uart_rx_c, uart_tx_c, tp_dbgrq_c, tp_dbgack_c, 
        tp_tdo_c, tp_tck_c, tp_tdi_c, tp_tms_c, tp_rtck_c, tp_trst_c, 
        led_1_c, led_2_c, s_1mhz_stb, s_1khz_stb, n2588, n1543, 
        n1544, n1545, n2584;
    wire [1:0]s_mem_rd_sel;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(148[9:21])
    wire [15:0]s_mem_dout;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(150[9:19])
    wire [15:0]s_cram_dout;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(107[9:20])
    wire [8:1]s_desc_addr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(155[9:20])
    wire [31:0]s_desc_data;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(156[9:20])
    wire [8:1]s_ptr_addr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(160[9:19])
    wire [15:0]s_ptr_data;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(161[9:19])
    
    wire s_ptr_mem_wr_stb;
    wire [15:0]s_aux_mem_dout;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(166[9:23])
    
    wire s_aux_mem_wr_stb;
    wire [15:0]s_reg_din;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(173[9:18])
    wire [15:0]s_reg_dout;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(174[9:19])
    wire [15:0]s_status;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(183[9:17])
    
    wire s_tap_busy;
    wire [15:0]s_irq_st;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(191[9:17])
    
    wire n2583, n7804;
    wire [15:0]s_irq_en;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(195[9:17])
    
    wire s_irq_en_wr_stb;
    wire [15:0]s_r3;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(199[9:13])
    
    wire s_r3_wr_stb;
    wire [15:0]s_ckgen_div;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(203[9:20])
    
    wire s_ckgen_div_wr_stb;
    wire [15:0]s_ckgen_rtdiv;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(207[9:22])
    
    wire s_ckgen_rtdiv_wr_stb;
    wire [15:0]s_tmr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(212[9:14])
    
    wire s_tmr_cnt_en;
    wire [15:0]s_mem_wr_addr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(220[9:22])
    
    wire n2566;
    wire [15:0]s_mem_rd_addr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(221[9:22])
    wire [15:0]s_desc_lo;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(223[9:18])
    wire [15:0]s_desc_hi;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(224[9:18])
    wire [15:0]s_cfg;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(228[9:14])
    
    wire s_cfg_wr_stb, s_tmr_cnt_en_N_1;
    wire [3:0]s_irq;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(240[9:14])
    
    wire s_tap_tdo, s_tap_tdi;
    wire [9:0]s_rx_mem_addr;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(268[9:22])
    wire [15:0]s_rx_mem_din;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(270[9:21])
    
    wire s_rx_mem_wr_stb, n7313, n7312, n7310, n7787, s_dbg_mem_rd, 
        s_dbg_mem_wr, n7309, n7785;
    wire [3:0]irq_N_8;
    
    wire irq_N_5, n4020, s_tmr_cnt_en_N_2, s_cram_next_st_1__N_24_c_0, 
        n14, n7308, n7_adj_1607, n7784, n2606, n7510, n7509, fsmc_d_out_10, 
        n8855, n6, n7507, n7506, n7789, n7_adj_1608, n7185, n7501, 
        n7183, n7500, n7499, n8186, n2591, n2552, n2586, n2548, 
        n7181, q_15__N_51, fsmc_d_out_9, n2609, n2569, n2573, n7498, 
        n2608, fsmc_d_out_8, n7497, n2576, n2613, n7496, n7494, 
        n2, n1, n2571, n2_adj_1609, n1_adj_1610, n2_adj_1611, n2574, 
        n1_adj_1612, n2_adj_1613, fsmc_d_out_7, n2578, n1_adj_1614, 
        n2549, n7493, n2564, n2_adj_1615, n1_adj_1616, n7492, n9, 
        n7_adj_1617, n2594, n2556, n2977, n2612, n2562, n7491, 
        fsmc_d_out_6, n2567, n2568, n2601, n7490, n2572, n2_adj_1618, 
        n7489, n7487, n1_adj_1619, n7486, n2_adj_1620, n7485, n2593, 
        n2554, n4, fsmc_d_out_13, n2563, n2_adj_1621, n1_adj_1622, 
        n14_adj_1623, n2_adj_1624, fsmc_d_out_5, n2604, n1_adj_1625, 
        n7484, n2_adj_1626, n1_adj_1627, n7483, n2575, n2570, n2597, 
        n2_adj_1628, n1_adj_1629, n2565, n2607, fsmc_d_out_4, n2561, 
        n2598, n7482, n2596, n2560, n1_adj_1630, n2546, n2614, 
        n2602, n2615, fsmc_d_out_3, n2600, n2605, n2610, n2_adj_1631, 
        fsmc_d_out_14, n1_adj_1632, n2599, n7480, n2611, n2_adj_1633, 
        n2595, n2558, fsmc_clk_c_enable_2, n7_adj_1634, n7479, n7_adj_1635, 
        n7478, n2_adj_1636, n1_adj_1637, n2_adj_1638, n1_adj_1639, 
        n2_adj_1640, n7508, n1_adj_1641, n7781, n1_adj_1642, n7481, 
        n7780, fsmc_d_out_0, n2555, n2551, n2590, n7495, n2550, 
        n2589, n2557, n7456, n7383, n7395, n7471, n7470, s_we_lo, 
        s_we_hi, n7469, n7462, n7461, n7460, n7459, fsmc_d_out_11, 
        n7458, n7457, n2577, n7_adj_1643, n7311, n7380, n7455, 
        n7454, n7453, n2603, n8183, q_15__N_82, n7488, q_15__N_80, 
        q_15__N_78, q_15__N_76, n2559, q_15__N_74, q_15__N_72, q_15__N_70, 
        q_15__N_68, q_15__N_66, q_15__N_64, n8187, q_15__N_62, n2553, 
        n2592, q_15__N_56, q_15__N_52;
    wire [3:0]s_addr0;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(63[9:16])
    wire [3:0]s_addr1_adj_1782;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(64[9:16])
    wire [3:0]s_addr2_adj_1783;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(65[9:16])
    wire [3:0]s_addr3;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(66[9:16])
    
    wire n8857, n7_adj_1644, n8856, n8851, out_get_N_312, n7_adj_1645, 
        n4048, n7406, n7405, n7403, fsmc_d_out_15, n8184, n7_adj_1646, 
        n14_adj_1647, n14_adj_1648, n7_adj_1649, n7402;
    wire [9:0]s_addr_0;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(77[9:17])
    wire [3:0]s_pos_0_3__N_670;
    
    wire n8327, n7397, n7396, n7823, n2_adj_1650, n1_adj_1651, n7404, 
        n7_adj_1652, n8317, n8850, n7385, n2841, n7401, n7384, 
        n7382, n7381, n8312, n8311, n7376, n7375, n7374, n7370, 
        n7369, n7367, n7366, n8295, n4959, mclk_c_enable_118, n7365, 
        n8854, n8288, n8286, n7863;
    wire [3:0]s_goto_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(196[9:21])
    
    wire n2547, n7862, n7860, n7859, n7858;
    wire [3:0]s_end_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(227[9:20])
    
    wire n7505, n8284, n8283, fsmc_d_out_12, fsmc_d_out_1, n8279, 
        n8853, n8852, n8276, fsmc_d_out_2, n7215, n8274, n8273, 
        n8188, mclk_c_enable_91, n2587, n7836, n7835, n2585, n3064, 
        n8180, s_cnt_en_N_1514, n7830, n7829, n7207, n24, n7_adj_1653, 
        n13, n8264, n7824, n7821, n7820, n7227, n20, n7128, 
        n6_adj_1654, n11, n7861, n7225, n7801, n7150, n7213, n20_adj_1655, 
        n7149, n11_adj_1656, n7795, n8177, n7786, n7805, n7802, 
        n19, n7800, n17, n7799, n22, n8178, n7796, fsmc_clk_c_enable_1, 
        n7794, n7793, n8182, n7790, n7217;
    
    VHI i2 (.Z(VCC_net));
    FD1S3AX s_irq_out_65 (.D(irq_N_5), .CK(mclk_c), .Q(irq_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(524[3] 526[10])
    defparam s_irq_out_65.GSR = "DISABLED";
    FD1S3AX s_tmr_cnt_en_66 (.D(n4020), .CK(mclk_c), .Q(s_tmr_cnt_en));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(597[3] 605[10])
    defparam s_tmr_cnt_en_66.GSR = "ENABLED";
    FD1S3AX s_irq_reg_i1 (.D(n8317), .CK(mclk_c), .Q(s_irq_st[0]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[3] 493[10])
    defparam s_irq_reg_i1.GSR = "DISABLED";
    PFUMX i5808 (.BLUT(n7469), .ALUT(n7470), .C0(q_15__N_80), .Z(n7471));
    \jtag_led_drv(31,false,false,false)  led_run_drv (.mclk_c(mclk_c), .led_1_c(led_1_c), 
            .out_get_N_312(out_get_N_312), .s_1khz_stb(s_1khz_stb));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(937[16:35])
    \jtag_reg(16,16)_U5  desc_hi (.s_desc_hi({s_desc_hi}), .mclk_c(mclk_c), 
            .mclk_c_enable_118(mclk_c_enable_118), .\s_desc_data[16] (s_desc_data[16]), 
            .\s_desc_data[31] (s_desc_data[31]), .\s_desc_data[30] (s_desc_data[30]), 
            .\s_desc_data[29] (s_desc_data[29]), .\s_desc_data[28] (s_desc_data[28]), 
            .\s_desc_data[27] (s_desc_data[27]), .\s_desc_data[26] (s_desc_data[26]), 
            .\s_desc_data[25] (s_desc_data[25]), .\s_desc_data[24] (s_desc_data[24]), 
            .\s_desc_data[23] (s_desc_data[23]), .\s_desc_data[22] (s_desc_data[22]), 
            .\s_desc_data[21] (s_desc_data[21]), .\s_desc_data[20] (s_desc_data[20]), 
            .\s_desc_data[19] (s_desc_data[19]), .\s_desc_data[18] (s_desc_data[18]), 
            .\s_desc_data[17] (s_desc_data[17]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(676[12:27])
    IB tp_rtck_pad (.I(tp_rtck), .O(tp_rtck_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(59[2:9])
    IB tp_tdo_pad (.I(tp_tdo), .O(tp_tdo_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(54[2:8])
    IB tp_dbgack_pad (.I(tp_dbgack), .O(tp_dbgack_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(52[2:11])
    IB uart_tx_pad (.I(uart_tx), .O(uart_tx_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(48[2:9])
    IB fsmc_nce_pad (.I(fsmc_nce), .O(fsmc_nce_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(43[2:10])
    IB s_cram_next_st_1__N_24_pad_0 (.I(fsmc_nwe), .O(s_cram_next_st_1__N_24_c_0));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(42[2:10])
    IB fsmc_noe_pad (.I(fsmc_noe), .O(fsmc_noe_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(41[2:10])
    IB fsmc_clk_pad (.I(fsmc_clk), .O(fsmc_clk_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(40[2:10])
    IB mclk_pad (.I(mclk), .O(mclk_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    PFUMX i5829 (.BLUT(n7488), .ALUT(n7489), .C0(q_15__N_80), .Z(n7492));
    OB led_2_pad (.I(led_2_c), .O(led_2));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(66[2:7])
    OB led_1_pad (.I(led_1_c), .O(led_1));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(65[2:7])
    OBZ tp_rst_pad (.I(GND_net), .T(n2841), .O(tp_rst));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(571[2:12])
    OB tp_trst_pad (.I(tp_trst_c), .O(tp_trst));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(61[2:9])
    OB tp_tms_pad (.I(tp_tms_c), .O(tp_tms));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(57[2:8])
    OB tp_tdi_pad (.I(tp_tdi_c), .O(tp_tdi));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(56[2:8])
    OB tp_tck_pad (.I(tp_tck_c), .O(tp_tck));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(55[2:8])
    OB tp_dbgrq_pad (.I(tp_dbgrq_c), .O(tp_dbgrq));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(51[2:10])
    OB uart_rx_pad (.I(uart_rx_c), .O(uart_rx));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(47[2:9])
    OB irq_pad (.I(irq_c), .O(irq));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(45[2:5])
    BB fsmc_d_pad_0 (.I(s_cram_dout[0]), .T(n2844), .B(fsmc_d[0]), .O(fsmc_d_out_0));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_1 (.I(s_cram_dout[1]), .T(n2844), .B(fsmc_d[1]), .O(fsmc_d_out_1));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_2 (.I(s_cram_dout[2]), .T(n2844), .B(fsmc_d[2]), .O(fsmc_d_out_2));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_3 (.I(s_cram_dout[3]), .T(n2844), .B(fsmc_d[3]), .O(fsmc_d_out_3));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_4 (.I(s_cram_dout[4]), .T(n2844), .B(fsmc_d[4]), .O(fsmc_d_out_4));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_5 (.I(s_cram_dout[5]), .T(n2844), .B(fsmc_d[5]), .O(fsmc_d_out_5));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_6 (.I(s_cram_dout[6]), .T(n2844), .B(fsmc_d[6]), .O(fsmc_d_out_6));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_7 (.I(s_cram_dout[7]), .T(n2844), .B(fsmc_d[7]), .O(fsmc_d_out_7));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_8 (.I(s_cram_dout[8]), .T(n2844), .B(fsmc_d[8]), .O(fsmc_d_out_8));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_9 (.I(s_cram_dout[9]), .T(n2844), .B(fsmc_d[9]), .O(fsmc_d_out_9));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_10 (.I(s_cram_dout[10]), .T(n2844), .B(fsmc_d[10]), 
       .O(fsmc_d_out_10));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_11 (.I(s_cram_dout[11]), .T(n2844), .B(fsmc_d[11]), 
       .O(fsmc_d_out_11));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    BB fsmc_d_pad_12 (.I(s_cram_dout[12]), .T(n2844), .B(fsmc_d[12]), 
       .O(fsmc_d_out_12));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    PFUMX i5830 (.BLUT(n7490), .ALUT(n7491), .C0(q_15__N_80), .Z(n7493));
    LUT4 i5796_3_lut (.A(s_cfg[0]), .B(s_tmr[0]), .C(q_15__N_82), .Z(n7459)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5796_3_lut.init = 16'hcaca;
    ROM256X1A mux_177_Mux_0 (.AD0(s_status[0]), .AD1(s_status[1]), .AD2(s_status[2]), 
            .AD3(s_status[3]), .AD4(s_end_state[0]), .AD5(s_end_state[1]), 
            .AD6(s_end_state[2]), .AD7(s_end_state[3]), .DO0(s_goto_state[0])) /* synthesis initstate=0xF7773767776747577F6733677767766777F7773777777767777F773377777776 */ ;
    defparam mux_177_Mux_0.initval = 256'hF7773767776747577F6733677767766777F7773777777767777F773377777776;
    ROM256X1A mux_177_Mux_1 (.AD0(s_status[0]), .AD1(s_status[1]), .AD2(s_status[2]), 
            .AD3(s_status[3]), .AD4(s_end_state[0]), .AD5(s_end_state[1]), 
            .AD6(s_end_state[2]), .AD7(s_end_state[3]), .DO0(s_goto_state[1])) /* synthesis initstate=0xB0307030103000103A3274303030323230B030F03090302030BA30F430B030B2 */ ;
    defparam mux_177_Mux_1.initval = 256'hB0307030103000103A3274303030323230B030F03090302030BA30F430B030B2;
    LUT4 i5795_3_lut (.A(s_ckgen_div[0]), .B(s_ckgen_rtdiv[0]), .C(q_15__N_82), 
         .Z(n7458)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5795_3_lut.init = 16'hcaca;
    LUT4 i5794_3_lut (.A(s_irq_en[0]), .B(s_r3[0]), .C(q_15__N_82), .Z(n7457)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5794_3_lut.init = 16'hcaca;
    LUT4 i5793_3_lut (.A(s_status[0]), .B(s_irq_st[0]), .C(q_15__N_82), 
         .Z(n7456)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5793_3_lut.init = 16'hcaca;
    LUT4 i5791_3_lut (.A(s_desc_lo[3]), .B(s_desc_hi[3]), .C(q_15__N_82), 
         .Z(n7454)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5791_3_lut.init = 16'hcaca;
    LUT4 i5790_3_lut (.A(s_mem_wr_addr[3]), .B(s_mem_rd_addr[3]), .C(q_15__N_82), 
         .Z(n7453)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5790_3_lut.init = 16'hcaca;
    PFUMX i5836 (.BLUT(n7495), .ALUT(n7496), .C0(q_15__N_80), .Z(n7499));
    LUT4 i2342_1_lut_2_lut (.A(out_get_N_312), .B(n7785), .Z(n4048)) /* synthesis lut_function=(!(A (B))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(472[18] 473[44])
    defparam i2342_1_lut_2_lut.init = 16'h7777;
    LUT4 s_desc_lo_10__bdd_3_lut_6234 (.A(s_desc_lo[10]), .B(q_15__N_82), 
         .C(s_desc_hi[10]), .Z(n7823)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_10__bdd_3_lut_6234.init = 16'he2e2;
    PFUMX i5837 (.BLUT(n7497), .ALUT(n7498), .C0(q_15__N_80), .Z(n7500));
    PFUMX i6 (.BLUT(s_cnt_en_N_1514), .ALUT(n1_adj_1642), .C0(n7227), 
          .Z(n2_adj_1633));
    PFUMX i5647 (.BLUT(n7308), .ALUT(n7309), .C0(q_15__N_80), .Z(n7310));
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
            .\s_mem_wr_addr[2] (s_mem_wr_addr[2]), .q_15__N_80(q_15__N_80));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(632[16:31])
    \jtag_reg(16,16)_U4  desc_lo (.s_desc_lo({s_desc_lo}), .mclk_c(mclk_c), 
            .mclk_c_enable_118(mclk_c_enable_118), .\s_desc_data[15] (s_desc_data[15]), 
            .\s_desc_data[14] (s_desc_data[14]), .\s_desc_data[13] (s_desc_data[13]), 
            .\s_desc_data[12] (s_desc_data[12]), .\s_desc_data[11] (s_desc_data[11]), 
            .\s_desc_data[10] (s_desc_data[10]), .\s_desc_data[9] (s_desc_data[9]), 
            .\s_desc_data[8] (s_desc_data[8]), .\s_desc_data[7] (s_desc_data[7]), 
            .\s_desc_data[6] (s_desc_data[6]), .\s_desc_data[5] (s_desc_data[5]), 
            .\s_desc_data[4] (s_desc_data[4]), .s_pos_0_3__N_670({s_pos_0_3__N_670}));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(662[12:27])
    LUT4 i5742_3_lut (.A(s_cfg[4]), .B(s_tmr[4]), .C(q_15__N_82), .Z(n7405)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5742_3_lut.init = 16'hcaca;
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
            .\s_mem_rd_addr[1] (s_mem_rd_addr[1]), .q_15__N_82(q_15__N_82));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(647[16:31])
    LUT4 i5741_3_lut (.A(s_ckgen_div[4]), .B(s_ckgen_rtdiv[4]), .C(q_15__N_82), 
         .Z(n7404)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5741_3_lut.init = 16'hcaca;
    LUT4 i5739_3_lut (.A(s_cfg[7]), .B(s_tmr[7]), .C(q_15__N_82), .Z(n7402)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5739_3_lut.init = 16'hcaca;
    LUT4 i5738_3_lut (.A(s_ckgen_div[7]), .B(s_ckgen_rtdiv[7]), .C(q_15__N_82), 
         .Z(n7401)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5738_3_lut.init = 16'hcaca;
    LUT4 i5733_3_lut (.A(s_cfg[10]), .B(s_tmr[10]), .C(q_15__N_82), .Z(n7396)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5733_3_lut.init = 16'hcaca;
    LUT4 i5732_3_lut (.A(s_ckgen_div[10]), .B(s_ckgen_rtdiv[10]), .C(q_15__N_82), 
         .Z(n7395)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5732_3_lut.init = 16'hcaca;
    PFUMX i5844 (.BLUT(n7505), .ALUT(n7506), .C0(q_15__N_80), .Z(n7507));
    PFUMX i36 (.BLUT(n17), .ALUT(n7150), .C0(n1543), .Z(n19));
    LUT4 s_desc_lo_10__bdd_3_lut (.A(s_mem_wr_addr[10]), .B(s_mem_rd_addr[10]), 
         .C(q_15__N_82), .Z(n7824)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_10__bdd_3_lut.init = 16'hcaca;
    LUT4 i5821_3_lut (.A(s_cfg[3]), .B(s_tmr[3]), .C(q_15__N_82), .Z(n7484)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5821_3_lut.init = 16'hcaca;
    LUT4 s_desc_lo_7__bdd_3_lut (.A(s_mem_wr_addr[7]), .B(s_mem_rd_addr[7]), 
         .C(q_15__N_82), .Z(n7830)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_7__bdd_3_lut.init = 16'hcaca;
    \counter(16,16)_U0  tmr_r (.s_tmr({s_tmr}), .mclk_c(mclk_c), .q_15__N_51(q_15__N_51), 
            .s_reg_din({s_reg_din}), .s_tmr_cnt_en_N_2(s_tmr_cnt_en_N_2), 
            .n8295(n8295), .n8276(n8276), .n8327(n8327));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(576[10:24])
    LUT4 n1583_bdd_4_lut (.A(n8312), .B(n24), .C(n7781), .D(n1544), 
         .Z(n8180)) /* synthesis lut_function=(A (C (D))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam n1583_bdd_4_lut.init = 16'hf044;
    LUT4 i5721_3_lut (.A(s_cfg[9]), .B(s_tmr[9]), .C(q_15__N_82), .Z(n7384)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5721_3_lut.init = 16'hcaca;
    LUT4 i5720_3_lut (.A(s_ckgen_div[9]), .B(s_ckgen_rtdiv[9]), .C(q_15__N_82), 
         .Z(n7383)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5720_3_lut.init = 16'hcaca;
    LUT4 i5718_3_lut (.A(s_cfg[11]), .B(s_tmr[11]), .C(q_15__N_82), .Z(n7381)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5718_3_lut.init = 16'hcaca;
    LUT4 i5717_3_lut (.A(s_ckgen_div[11]), .B(s_ckgen_rtdiv[11]), .C(q_15__N_82), 
         .Z(n7380)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5717_3_lut.init = 16'hcaca;
    LUT4 i5712_3_lut (.A(s_desc_lo[6]), .B(s_desc_hi[6]), .C(q_15__N_82), 
         .Z(n7375)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5712_3_lut.init = 16'hcaca;
    LUT4 i5711_3_lut (.A(s_mem_wr_addr[6]), .B(s_mem_rd_addr[6]), .C(q_15__N_82), 
         .Z(n7374)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5711_3_lut.init = 16'hcaca;
    LUT4 s_desc_lo_7__bdd_3_lut_6239 (.A(s_desc_lo[7]), .B(q_15__N_82), 
         .C(s_desc_hi[7]), .Z(n7829)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_7__bdd_3_lut_6239.init = 16'he2e2;
    LUT4 i5706_3_lut (.A(s_cfg[12]), .B(s_tmr[12]), .C(q_15__N_82), .Z(n7369)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5706_3_lut.init = 16'hcaca;
    LUT4 i5705_3_lut (.A(s_ckgen_div[12]), .B(s_ckgen_rtdiv[12]), .C(q_15__N_82), 
         .Z(n7368)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5705_3_lut.init = 16'hcaca;
    LUT4 i5703_3_lut (.A(s_cfg[8]), .B(s_tmr[8]), .C(q_15__N_82), .Z(n7366)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5703_3_lut.init = 16'hcaca;
    LUT4 i5702_3_lut (.A(s_ckgen_div[8]), .B(s_ckgen_rtdiv[8]), .C(q_15__N_82), 
         .Z(n7365)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5702_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut (.A(out_get_N_312), .B(n7785), .Z(s_cfg_wr_stb)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut.init = 16'h8888;
    PFUMX i5847 (.BLUT(n7508), .ALUT(n7509), .C0(q_15__N_80), .Z(n7510));
    TSALL TSALL_INST (.TSALL(GND_net));
    PFUMX i5650 (.BLUT(n7311), .ALUT(n7312), .C0(q_15__N_80), .Z(n7313));
    PFUMX i6211 (.BLUT(n7799), .ALUT(n7795), .C0(n1543), .Z(n7800));
    LUT4 i1_2_lut_4_lut (.A(n8288), .B(q_15__N_56), .C(q_15__N_52), .D(q_15__N_82), 
         .Z(s_we_hi)) /* synthesis lut_function=(!(((C+!(D))+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(373[23:69])
    defparam i1_2_lut_4_lut.init = 16'h0800;
    LUT4 s_desc_lo_4__bdd_3_lut (.A(s_mem_wr_addr[4]), .B(s_mem_rd_addr[4]), 
         .C(q_15__N_82), .Z(n7836)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_4__bdd_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_4_lut_adj_204 (.A(n8288), .B(q_15__N_56), .C(q_15__N_52), 
         .D(q_15__N_82), .Z(s_we_lo)) /* synthesis lut_function=(!(((C+(D))+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(373[23:69])
    defparam i1_2_lut_4_lut_adj_204.init = 16'h0008;
    LUT4 s_desc_lo_4__bdd_3_lut_6244 (.A(s_desc_lo[4]), .B(q_15__N_82), 
         .C(s_desc_hi[4]), .Z(n7835)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_4__bdd_3_lut_6244.init = 16'he2e2;
    LUT4 i1_2_lut_4_lut_adj_205 (.A(n8288), .B(q_15__N_56), .C(q_15__N_52), 
         .D(q_15__N_64), .Z(fsmc_clk_c_enable_1)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(372[21:67])
    defparam i1_2_lut_4_lut_adj_205.init = 16'h0002;
    LUT4 i1_2_lut_4_lut_adj_206 (.A(n8288), .B(q_15__N_56), .C(q_15__N_52), 
         .D(q_15__N_64), .Z(fsmc_clk_c_enable_2)) /* synthesis lut_function=(!((B+(C+!(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(372[21:67])
    defparam i1_2_lut_4_lut_adj_206.init = 16'h0200;
    LUT4 i5820_3_lut (.A(s_ckgen_div[3]), .B(s_ckgen_rtdiv[3]), .C(q_15__N_82), 
         .Z(n7483)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5820_3_lut.init = 16'hcaca;
    LUT4 i3_4_lut (.A(s_irq_en[3]), .B(n6), .C(irq_N_8[1]), .D(s_tmr_cnt_en), 
         .Z(irq_N_5)) /* synthesis lut_function=(A (B+(C+!(D)))+!A (B+(C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(309[9:18])
    defparam i3_4_lut.init = 16'hfcfe;
    PFUMX s_reg_rd_sel_3__I_0_Mux_6_i15 (.BLUT(n4), .ALUT(n7_adj_1617), 
          .C0(n7225), .Z(s_reg_dout[6]));
    PFUMX i49 (.BLUT(n7213), .ALUT(n7207), .C0(n1544), .Z(n7215));
    BB fsmc_d_pad_13 (.I(s_cram_dout[13]), .T(n2844), .B(fsmc_d[13]), 
       .O(fsmc_d_out_13));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    LUT4 i5848_4_lut (.A(n8264), .B(n8273), .C(n8279), .D(n8274), .Z(n4959)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))) */ ;
    defparam i5848_4_lut.init = 16'h808a;
    LUT4 n20_bdd_2_lut_6264_3_lut_4_lut (.A(s_addr2_adj_1783[3]), .B(s_addr2_adj_1783[0]), 
         .C(s_addr2_adj_1783[2]), .D(s_addr2_adj_1783[1]), .Z(n7858)) /* synthesis lut_function=(!(A+((C+!(D))+!B))) */ ;
    defparam n20_bdd_2_lut_6264_3_lut_4_lut.init = 16'h0400;
    LUT4 i2_4_lut (.A(s_irq_en[0]), .B(s_irq_en[2]), .C(s_tap_busy), .D(s_1khz_stb), 
         .Z(n6)) /* synthesis lut_function=(A (B ((D)+!C)+!B !(C))+!A (B (D))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(309[9:18])
    defparam i2_4_lut.init = 16'hce0a;
    LUT4 tp_tdo_I_0_3_lut (.A(tp_tdo_c), .B(s_tap_tdi), .C(s_cfg[4]), 
         .Z(s_tap_tdo)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(984[15:61])
    defparam tp_tdo_I_0_3_lut.init = 16'hcaca;
    LUT4 i5819_3_lut (.A(s_irq_en[3]), .B(s_r3[3]), .C(q_15__N_82), .Z(n7482)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5819_3_lut.init = 16'hcaca;
    LUT4 i5818_3_lut (.A(s_status[3]), .B(s_irq_st[3]), .C(q_15__N_82), 
         .Z(n7481)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5818_3_lut.init = 16'hcaca;
    LUT4 n7185_bdd_4_lut_6559 (.A(n7185), .B(n8312), .C(n7796), .D(n1544), 
         .Z(n8177)) /* synthesis lut_function=(A (C (D))+!A (B (C (D))+!B (C+!(D)))) */ ;
    defparam n7185_bdd_4_lut_6559.init = 16'hf011;
    LUT4 n1583_bdd_4_lut_6494 (.A(n8312), .B(n20_adj_1655), .C(n7790), 
         .D(n1544), .Z(n8178)) /* synthesis lut_function=(A (C (D))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam n1583_bdd_4_lut_6494.init = 16'hf044;
    GSR GSR_INST (.GSR(mclk_c_enable_91));
    PFUMX s_reg_rd_sel_3__I_0_Mux_4_i15 (.BLUT(n7_adj_1634), .ALUT(n8186), 
          .C0(q_15__N_76), .Z(s_reg_dout[4]));
    LUT4 i1143_1_lut (.A(s_cfg[11]), .Z(n2841)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(571[2:12])
    defparam i1143_1_lut.init = 16'h5555;
    LUT4 i2315_4_lut (.A(s_tmr[15]), .B(s_tmr_cnt_en_N_2), .C(s_tmr_cnt_en), 
         .D(q_15__N_51), .Z(n4020)) /* synthesis lut_function=(A (B+(C))+!A (B+!((D)+!C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(597[3] 605[10])
    defparam i2315_4_lut.init = 16'hecfc;
    LUT4 s_cfg_10__I_0_1_lut (.A(s_cfg[10]), .Z(tp_trst_c)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(570[16:59])
    defparam s_cfg_10__I_0_1_lut.init = 16'h5555;
    LUT4 i3165_2_lut (.A(s_tap_tdi), .B(s_cfg[4]), .Z(tp_tdi_c)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(985[12:55])
    defparam i3165_2_lut.init = 16'h2222;
    LUT4 i3161_2_lut (.A(uart_tx_c), .B(s_cfg[7]), .Z(tp_dbgrq_c)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(970[17:56])
    defparam i3161_2_lut.init = 16'h8888;
    LUT4 i3163_2_lut (.A(tp_dbgack_c), .B(s_cfg[7]), .Z(uart_rx_c)) /* synthesis lut_function=(A+!(B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(976[15:56])
    defparam i3163_2_lut.init = 16'hbbbb;
    \cram_adaptor(2,12,4)  cram_bus (.s_mem_rd_sel({s_mem_rd_sel}), .fsmc_clk_c(fsmc_clk_c), 
            .q_15__N_56(q_15__N_56), .s_reg_dout({s_reg_dout[15:4], Open_0, 
            Open_1, Open_2, s_reg_dout[0]}), .fsmc_nce_c(fsmc_nce_c), 
            .n8850(n8850), .s_dbg_mem_rd(s_dbg_mem_rd), .q_15__N_52(q_15__N_52), 
            .fsmc_d_out_15(fsmc_d_out_15), .s_cram_next_st_1__N_24_c_0(s_cram_next_st_1__N_24_c_0), 
            .s_mem_dout({s_mem_dout}), .s_cram_dout({s_cram_dout}), .fsmc_noe_c(fsmc_noe_c), 
            .n2844(n2844), .s_ptr_mem_wr_stb(s_ptr_mem_wr_stb), .s_aux_mem_wr_stb(s_aux_mem_wr_stb), 
            .s_dbg_mem_wr(s_dbg_mem_wr), .n2578(n2578), .n2577(n2577), 
            .n2561(n2561), .n1(n1_adj_1641), .n2576(n2576), .n2560(n2560), 
            .n1_adj_1(n1_adj_1639), .n2575(n2575), .n2559(n2559), .n1_adj_2(n1_adj_1637), 
            .n2574(n2574), .n2558(n2558), .n1_adj_3(n1_adj_1632), .n2573(n2573), 
            .n2557(n2557), .n1_adj_4(n1_adj_1629), .n2572(n2572), .n2556(n2556), 
            .n1_adj_5(n1_adj_1627), .n2571(n2571), .n2555(n2555), .n1_adj_6(n1_adj_1625), 
            .n2570(n2570), .n2554(n2554), .n1_adj_7(n1_adj_1622), .n2569(n2569), 
            .n2553(n2553), .n1_adj_8(n1_adj_1630), .n2568(n2568), .n2552(n2552), 
            .n1_adj_9(n1_adj_1619), .n2567(n2567), .n2551(n2551), .n1_adj_10(n1_adj_1616), 
            .n2566(n2566), .n2550(n2550), .n1_adj_11(n1_adj_1614), .n2565(n2565), 
            .n2549(n2549), .n1_adj_12(n1_adj_1612), .n2564(n2564), .n2548(n2548), 
            .n1_adj_13(n1_adj_1610), .n2563(n2563), .n2547(n2547), .n1_adj_14(n1), 
            .n2562(n2562), .n2546(n2546), .n1_adj_15(n1_adj_1651), .\s_cfg[6] (s_cfg[6]), 
            .n8288(n8288), .n1545(n1545), .n8311(n8311), .mclk_c(mclk_c), 
            .out_get_N_312(out_get_N_312), .n1543(n1543), .\s_addr0[0] (s_addr0[0]), 
            .q_15__N_82(q_15__N_82), .\s_addr1[0] (s_addr1_adj_1782[0]), 
            .n8312(n8312), .s_addr2({s_addr2_adj_1783}), .s_addr3({s_addr3}), 
            .fsmc_d_out_0(fsmc_d_out_0), .n20(n20), .n22(n22), .n1544(n1544), 
            .\s_addr0[1] (s_addr0[1]), .q_15__N_80(q_15__N_80), .q_15__N_78(q_15__N_78), 
            .q_15__N_76(q_15__N_76), .\s_addr1[1] (s_addr1_adj_1782[1]), 
            .fsmc_d_out_1(fsmc_d_out_1), .fsmc_d_out_2(fsmc_d_out_2), .fsmc_d_out_3(fsmc_d_out_3), 
            .fsmc_d_out_4(fsmc_d_out_4), .fsmc_d_out_5(fsmc_d_out_5), .fsmc_d_out_6(fsmc_d_out_6), 
            .fsmc_d_out_7(fsmc_d_out_7), .fsmc_d_out_8(fsmc_d_out_8), .fsmc_d_out_9(fsmc_d_out_9), 
            .fsmc_d_out_10(fsmc_d_out_10), .fsmc_d_out_11(fsmc_d_out_11), 
            .fsmc_d_out_12(fsmc_d_out_12), .fsmc_d_out_13(fsmc_d_out_13), 
            .fsmc_d_out_14(fsmc_d_out_14), .n8284(n8284), .n8177(n8177), 
            .n7799(n7799), .n8178(n8178), .n7793(n7793), .n8180(n8180), 
            .n7784(n7784), .s_reg_din({s_reg_din}), .q_15__N_74(q_15__N_74), 
            .q_15__N_72(q_15__N_72), .q_15__N_70(q_15__N_70), .q_15__N_68(q_15__N_68), 
            .q_15__N_66(q_15__N_66), .q_15__N_64(q_15__N_64), .q_15__N_62(q_15__N_62), 
            .n8852(n8852), .n8857(n8857), .n7455(n7455), .n7487(n7487), 
            .n7480(n7480), .n7501(n7501), .n8856(n8856), .n7471(n7471), 
            .n7494(n7494), .n7823(n7823), .n7824(n7824), .n8188(n8188), 
            .n7835(n7835), .n7836(n7836), .n8186(n8186), .n7829(n7829), 
            .n7830(n7830), .n8187(n8187), .n7786(n7786), .n7787(n7787), 
            .n8185(n8185), .n7801(n7801), .n7802(n7802), .n8184(n8184), 
            .n7804(n7804), .n7805(n7805), .n8183(n8183), .n7820(n7820), 
            .n7821(n7821), .n8182(n8182), .n7376(n7376), .n7(n7_adj_1617), 
            .n8855(n8855), .n8854(n8854), .n8853(n8853), .\s_tmr[6] (s_tmr[6]), 
            .\s_r3[6] (s_r3[6]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(335[13:32])
    BB fsmc_d_pad_14 (.I(s_cram_dout[14]), .T(n2844), .B(fsmc_d[14]), 
       .O(fsmc_d_out_14));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    PFUMX s_reg_rd_sel_3__I_0_Mux_7_i15 (.BLUT(n7_adj_1652), .ALUT(n8187), 
          .C0(q_15__N_76), .Z(s_reg_dout[7]));
    PFUMX i6205 (.BLUT(n7793), .ALUT(n7789), .C0(n1543), .Z(n7794));
    ROM256X1A mux_177_Mux_2 (.AD0(s_status[0]), .AD1(s_status[1]), .AD2(s_status[2]), 
            .AD3(s_status[3]), .AD4(s_end_state[0]), .AD5(s_end_state[1]), 
            .AD6(s_end_state[2]), .AD7(s_end_state[3]), .DO0(s_goto_state[2])) /* synthesis initstate=0xB3B3F3B3B3B3B3B3B1B1B3B3B1B3B0B1B3B3B3F3B3B3B3B3B3B1B3B3B3B1B3B0 */ ;
    defparam mux_177_Mux_2.initval = 256'hB3B3F3B3B3B3B3B3B1B1B3B3B1B3B0B1B3B3B3F3B3B3B3B3B3B1B3B3B3B1B3B0;
    BB fsmc_d_pad_15 (.I(s_cram_dout[15]), .T(n2844), .B(fsmc_d[15]), 
       .O(fsmc_d_out_15));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(284[2:8])
    PFUMX s_reg_rd_sel_3__I_0_Mux_10_i15 (.BLUT(n7_adj_1644), .ALUT(n8188), 
          .C0(q_15__N_76), .Z(s_reg_dout[10]));
    PFUMX s_reg_rd_sel_3__I_0_Mux_11_i15 (.BLUT(n7_adj_1607), .ALUT(n8182), 
          .C0(q_15__N_76), .Z(s_reg_dout[11]));
    FD1S3AX s_irq_reg_i4 (.D(s_tmr_cnt_en_N_1), .CK(mclk_c), .Q(s_irq_st[3]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[3] 493[10])
    defparam s_irq_reg_i4.GSR = "DISABLED";
    LUT4 i1_2_lut_adj_207 (.A(out_get_N_312), .B(n7800), .Z(s_ckgen_div_wr_stb)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_adj_207.init = 16'h8888;
    LUT4 i1_2_lut_adj_208 (.A(out_get_N_312), .B(n7794), .Z(s_ckgen_rtdiv_wr_stb)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_adj_208.init = 16'h8888;
    FD1S3AX s_irq_reg_i3 (.D(s_1khz_stb), .CK(mclk_c), .Q(s_irq_st[2]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[3] 493[10])
    defparam s_irq_reg_i3.GSR = "DISABLED";
    FD1S3AX s_irq_reg_i2 (.D(s_irq[1]), .CK(mclk_c), .Q(s_irq_st[1]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(491[3] 493[10])
    defparam s_irq_reg_i2.GSR = "DISABLED";
    PFUMX s_reg_rd_sel_3__I_0_Mux_9_i15 (.BLUT(n7_adj_1649), .ALUT(n8183), 
          .C0(q_15__N_76), .Z(s_reg_dout[9]));
    LUT4 i1_2_lut_adj_209 (.A(s_addr1_adj_1782[1]), .B(s_addr1_adj_1782[0]), 
         .Z(n13)) /* synthesis lut_function=(!((B)+!A)) */ ;
    defparam i1_2_lut_adj_209.init = 16'h2222;
    LUT4 i1_2_lut_adj_210 (.A(s_addr0[0]), .B(s_addr0[1]), .Z(n7_adj_1653)) /* synthesis lut_function=(!(A+!(B))) */ ;
    defparam i1_2_lut_adj_210.init = 16'h4444;
    LUT4 i1_2_lut_adj_211 (.A(s_addr1_adj_1782[1]), .B(s_addr1_adj_1782[0]), 
         .Z(n11)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_adj_211.init = 16'h8888;
    LUT4 i1_2_lut_adj_212 (.A(s_addr0[1]), .B(s_addr0[0]), .Z(n6_adj_1654)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_adj_212.init = 16'h8888;
    LUT4 i5522_2_lut (.A(s_addr1_adj_1782[1]), .B(s_addr1_adj_1782[0]), 
         .Z(n7181)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i5522_2_lut.init = 16'heeee;
    LUT4 i5524_2_lut (.A(s_addr0[1]), .B(s_addr0[0]), .Z(n7183)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i5524_2_lut.init = 16'heeee;
    PFUMX s_reg_rd_sel_3__I_0_Mux_12_i15 (.BLUT(n7_adj_1643), .ALUT(n14), 
          .C0(q_15__N_76), .Z(s_reg_dout[12]));
    LUT4 i1_2_lut_adj_213 (.A(s_addr1_adj_1782[0]), .B(s_addr1_adj_1782[1]), 
         .Z(n11_adj_1656)) /* synthesis lut_function=(!((B)+!A)) */ ;
    defparam i1_2_lut_adj_213.init = 16'h2222;
    LUT4 s_desc_lo_5__bdd_3_lut_6197 (.A(s_desc_lo[5]), .B(q_15__N_82), 
         .C(s_desc_hi[5]), .Z(n7786)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_5__bdd_3_lut_6197.init = 16'he2e2;
    \jtag_reg(16,16)_U8  cfg_r (.s_cfg({s_cfg}), .mclk_c(mclk_c), .s_cfg_wr_stb(s_cfg_wr_stb), 
            .s_reg_din({s_reg_din}), .n4048(n4048), .mclk_c_enable_91(mclk_c_enable_91));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(551[10:25])
    PFUMX s_reg_rd_sel_3__I_0_Mux_8_i15 (.BLUT(n7), .ALUT(n8184), .C0(q_15__N_76), 
          .Z(s_reg_dout[8]));
    LUT4 i3327_2_lut (.A(s_aux_mem_dout[15]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1640)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3327_2_lut.init = 16'h8888;
    LUT4 i3328_2_lut (.A(s_aux_mem_dout[14]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1638)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3328_2_lut.init = 16'h8888;
    LUT4 i3329_2_lut (.A(s_aux_mem_dout[13]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1636)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3329_2_lut.init = 16'h8888;
    LUT4 i3330_2_lut (.A(s_aux_mem_dout[12]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1631)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3330_2_lut.init = 16'h8888;
    LUT4 i3331_2_lut (.A(s_aux_mem_dout[11]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1628)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3331_2_lut.init = 16'h8888;
    PFUMX s_reg_rd_sel_3__I_0_Mux_15_i15 (.BLUT(n7_adj_1645), .ALUT(n14_adj_1647), 
          .C0(q_15__N_76), .Z(s_reg_dout[15]));
    LUT4 i3332_2_lut (.A(s_aux_mem_dout[10]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1626)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3332_2_lut.init = 16'h8888;
    LUT4 i3333_2_lut (.A(s_aux_mem_dout[9]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1624)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3333_2_lut.init = 16'h8888;
    \jtag_reg(16,16)_U7  ckgen_div_r (.s_ckgen_div({s_ckgen_div}), .mclk_c(mclk_c), 
            .s_ckgen_div_wr_stb(s_ckgen_div_wr_stb), .s_reg_din({s_reg_din}));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(693[16:31])
    PFUMX s_reg_rd_sel_3__I_0_Mux_14_i15 (.BLUT(n7_adj_1646), .ALUT(n14_adj_1648), 
          .C0(q_15__N_76), .Z(s_reg_dout[14]));
    LUT4 i48_4_lut (.A(n7215), .B(s_addr3[1]), .C(n1543), .D(n8286), 
         .Z(n7217)) /* synthesis lut_function=(A (B+((D)+!C))+!A (B (C)+!B (C (D)))) */ ;
    defparam i48_4_lut.init = 16'hfaca;
    LUT4 i3334_2_lut (.A(s_aux_mem_dout[8]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1621)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3334_2_lut.init = 16'h8888;
    LUT4 i3335_2_lut (.A(s_aux_mem_dout[7]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1620)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3335_2_lut.init = 16'h8888;
    PFUMX s_reg_rd_sel_3__I_0_Mux_13_i15 (.BLUT(n7_adj_1635), .ALUT(n14_adj_1623), 
          .C0(q_15__N_76), .Z(s_reg_dout[13]));
    LUT4 i3336_2_lut (.A(s_aux_mem_dout[6]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1618)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3336_2_lut.init = 16'h8888;
    LUT4 i5816_3_lut (.A(s_desc_lo[1]), .B(s_desc_hi[1]), .C(q_15__N_82), 
         .Z(n7479)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5816_3_lut.init = 16'hcaca;
    PFUMX s_reg_rd_sel_3__I_0_Mux_5_i15 (.BLUT(n7_adj_1608), .ALUT(n8185), 
          .C0(q_15__N_76), .Z(s_reg_dout[5]));
    LUT4 s_desc_lo_5__bdd_3_lut (.A(s_mem_wr_addr[5]), .B(s_mem_rd_addr[5]), 
         .C(q_15__N_82), .Z(n7787)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_5__bdd_3_lut.init = 16'hcaca;
    LUT4 i3339_2_lut (.A(s_aux_mem_dout[5]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1615)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3339_2_lut.init = 16'h8888;
    LUT4 i5815_3_lut (.A(s_mem_wr_addr[1]), .B(s_mem_rd_addr[1]), .C(q_15__N_82), 
         .Z(n7478)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5815_3_lut.init = 16'hcaca;
    LUT4 i3340_2_lut (.A(s_aux_mem_dout[4]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1613)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3340_2_lut.init = 16'h8888;
    LUT4 i3341_2_lut (.A(s_aux_mem_dout[3]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1611)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3341_2_lut.init = 16'h8888;
    PFUMX s_mem_rd_sel_1__I_0_Mux_0_i3 (.BLUT(n1_adj_1651), .ALUT(n2_adj_1650), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[0]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_1_i3 (.BLUT(n1), .ALUT(n2), .C0(s_mem_rd_sel[1]), 
          .Z(s_mem_dout[1]));
    LUT4 i1_2_lut_adj_214 (.A(out_get_N_312), .B(n19), .Z(s_tmr_cnt_en_N_2)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_adj_214.init = 16'h8888;
    \jtag_desc_ram(16,9,32,8)  desc_mem (.fsmc_clk_c(fsmc_clk_c), .mclk_c(mclk_c), 
            .s_we_hi(s_we_hi), .VCC_net(VCC_net), .GND_net(GND_net), .n8856(n8856), 
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
            .s_we_lo(s_we_lo), .n8854(n8854), .s_pos_0_3__N_670({s_pos_0_3__N_670}), 
            .\s_desc_data[4] (s_desc_data[4]), .\s_desc_data[5] (s_desc_data[5]), 
            .\s_desc_data[6] (s_desc_data[6]), .\s_desc_data[7] (s_desc_data[7]), 
            .\s_desc_data[8] (s_desc_data[8]), .\s_desc_data[9] (s_desc_data[9]), 
            .\s_desc_data[10] (s_desc_data[10]), .\s_desc_data[11] (s_desc_data[11]), 
            .\s_desc_data[12] (s_desc_data[12]), .\s_desc_data[13] (s_desc_data[13]), 
            .\s_desc_data[14] (s_desc_data[14]), .\s_desc_data[15] (s_desc_data[15]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(727[13:33])
    PFUMX s_mem_rd_sel_1__I_0_Mux_2_i3 (.BLUT(n1_adj_1610), .ALUT(n2_adj_1609), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[2]));
    LUT4 n20_bdd_2_lut_3_lut (.A(n8312), .B(n8311), .C(n20), .Z(n7859)) /* synthesis lut_function=(!(A+(B+!(C)))) */ ;
    defparam n20_bdd_2_lut_3_lut.init = 16'h1010;
    LUT4 i5552_2_lut_3_lut (.A(n8312), .B(n8311), .C(n7185), .Z(n7213)) /* synthesis lut_function=(A+(B+(C))) */ ;
    defparam i5552_2_lut_3_lut.init = 16'hfefe;
    LUT4 n24_bdd_2_lut_6339_3_lut (.A(n8312), .B(n8311), .C(n24), .Z(n7862)) /* synthesis lut_function=(!(A+(B+!(C)))) */ ;
    defparam n24_bdd_2_lut_6339_3_lut.init = 16'h1010;
    \jtag_reg(16,16)  r3 (.s_r3({s_r3}), .mclk_c(mclk_c), .s_r3_wr_stb(s_r3_wr_stb), 
            .s_reg_din({s_reg_din}));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(615[7:22])
    LUT4 i1_2_lut_rep_169_3_lut (.A(s_addr2_adj_1783[3]), .B(s_addr2_adj_1783[0]), 
         .C(s_addr2_adj_1783[1]), .Z(n8283)) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i1_2_lut_rep_169_3_lut.init = 16'h4040;
    LUT4 i3342_2_lut (.A(s_aux_mem_dout[2]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1609)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3342_2_lut.init = 16'h8888;
    LUT4 s_reg_rd_sel_3__I_0_Mux_0_i15_4_lut (.A(n7462), .B(n9), .C(q_15__N_76), 
         .D(n2977), .Z(s_reg_dout[0])) /* synthesis lut_function=(A (B ((D)+!C)+!B !(C))+!A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_0_i15_4_lut.init = 16'hca0a;
    \jtag_clk_scaler(1000)  cklgen_1khz (.GND_net(GND_net), .s_1khz_stb(s_1khz_stb), 
            .mclk_c(mclk_c), .s_1mhz_stb(s_1mhz_stb));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(910[16:38])
    LUT4 i3351_2_lut (.A(s_aux_mem_dout[1]), .B(s_mem_rd_sel[0]), .Z(n2)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3351_2_lut.init = 16'h8888;
    LUT4 s_reg_rd_sel_3__I_0_Mux_0_i9_3_lut (.A(s_desc_lo[0]), .B(s_desc_hi[0]), 
         .C(q_15__N_82), .Z(n9)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_0_i9_3_lut.init = 16'hcaca;
    LUT4 i3398_2_lut (.A(s_aux_mem_dout[0]), .B(s_mem_rd_sel[0]), .Z(n2_adj_1650)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(379[2] 382[31])
    defparam i3398_2_lut.init = 16'h8888;
    LUT4 s_reg_rd_sel_3__I_0_Mux_5_i7_4_lut (.A(s_r3[5]), .B(n7313), .C(q_15__N_78), 
         .D(n3064), .Z(n7_adj_1608)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_5_i7_4_lut.init = 16'hcac0;
    LUT4 i3317_4_lut (.A(s_desc_lo[13]), .B(n2977), .C(s_desc_hi[13]), 
         .D(q_15__N_82), .Z(n14_adj_1623)) /* synthesis lut_function=(A (B (C+!(D)))+!A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam i3317_4_lut.init = 16'hc088;
    LUT4 s_reg_rd_sel_3__I_0_Mux_13_i7_4_lut (.A(s_r3[13]), .B(n7507), .C(q_15__N_78), 
         .D(n3064), .Z(n7_adj_1635)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_13_i7_4_lut.init = 16'hcac0;
    LUT4 i3310_4_lut (.A(s_desc_lo[14]), .B(n2977), .C(s_desc_hi[14]), 
         .D(q_15__N_82), .Z(n14_adj_1648)) /* synthesis lut_function=(A (B (C+!(D)))+!A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam i3310_4_lut.init = 16'hc088;
    LUT4 s_reg_rd_sel_3__I_0_Mux_14_i7_4_lut (.A(s_r3[14]), .B(n7510), .C(q_15__N_78), 
         .D(n3064), .Z(n7_adj_1646)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_14_i7_4_lut.init = 16'hcac0;
    LUT4 i3231_4_lut (.A(s_desc_lo[15]), .B(n2977), .C(s_desc_hi[15]), 
         .D(q_15__N_82), .Z(n14_adj_1647)) /* synthesis lut_function=(A (B (C+!(D)))+!A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam i3231_4_lut.init = 16'hc088;
    LUT4 i1260_2_lut (.A(q_15__N_80), .B(q_15__N_78), .Z(n2977)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam i1260_2_lut.init = 16'h2222;
    PFUMX s_mem_rd_sel_1__I_0_Mux_3_i3 (.BLUT(n1_adj_1612), .ALUT(n2_adj_1611), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[3]));
    LUT4 s_reg_rd_sel_3__I_0_Mux_15_i7_4_lut (.A(s_r3[15]), .B(n7310), .C(q_15__N_78), 
         .D(n3064), .Z(n7_adj_1645)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_15_i7_4_lut.init = 16'hcac0;
    PFUMX i5817 (.BLUT(n7478), .ALUT(n7479), .C0(q_15__N_80), .Z(n7480));
    LUT4 s_reg_rd_sel_3__I_0_Mux_8_i7_4_lut (.A(s_r3[8]), .B(n7367), .C(q_15__N_78), 
         .D(n3064), .Z(n7)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_8_i7_4_lut.init = 16'hcac0;
    LUT4 i1_4_lut (.A(out_get_N_312), .B(n7863), .C(n7128), .D(n1543), 
         .Z(s_irq_en_wr_stb)) /* synthesis lut_function=(A (B (C+!(D))+!B (C (D)))) */ ;
    defparam i1_4_lut.init = 16'ha088;
    LUT4 i3182_4_lut (.A(s_desc_lo[12]), .B(n2977), .C(s_desc_hi[12]), 
         .D(q_15__N_82), .Z(n14)) /* synthesis lut_function=(A (B (C+!(D)))+!A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam i3182_4_lut.init = 16'hc088;
    LUT4 s_reg_rd_sel_3__I_0_Mux_12_i7_4_lut (.A(s_r3[12]), .B(n7370), .C(q_15__N_78), 
         .D(n3064), .Z(n7_adj_1643)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_12_i7_4_lut.init = 16'hcac0;
    LUT4 s_reg_rd_sel_3__I_0_Mux_9_i7_4_lut (.A(s_r3[9]), .B(n7385), .C(q_15__N_78), 
         .D(n3064), .Z(n7_adj_1649)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_9_i7_4_lut.init = 16'hcac0;
    PFUMX s_mem_rd_sel_1__I_0_Mux_4_i3 (.BLUT(n1_adj_1614), .ALUT(n2_adj_1613), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[4]));
    LUT4 s_reg_rd_sel_3__I_0_Mux_11_i7_4_lut (.A(s_r3[11]), .B(n7382), .C(q_15__N_78), 
         .D(n3064), .Z(n7_adj_1607)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_11_i7_4_lut.init = 16'hcac0;
    PFUMX s_mem_rd_sel_1__I_0_Mux_5_i3 (.BLUT(n1_adj_1616), .ALUT(n2_adj_1615), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[5]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_6_i3 (.BLUT(n1_adj_1619), .ALUT(n2_adj_1618), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[6]));
    LUT4 s_reg_rd_sel_3__I_0_Mux_10_i7_4_lut (.A(s_r3[10]), .B(n7397), .C(q_15__N_78), 
         .D(n3064), .Z(n7_adj_1644)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_10_i7_4_lut.init = 16'hcac0;
    PFUMX s_mem_rd_sel_1__I_0_Mux_7_i3 (.BLUT(n1_adj_1630), .ALUT(n2_adj_1620), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[7]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_8_i3 (.BLUT(n1_adj_1622), .ALUT(n2_adj_1621), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[8]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_9_i3 (.BLUT(n1_adj_1625), .ALUT(n2_adj_1624), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[9]));
    LUT4 s_reg_rd_sel_3__I_0_Mux_7_i7_4_lut (.A(s_r3[7]), .B(n7403), .C(q_15__N_78), 
         .D(n3064), .Z(n7_adj_1652)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_7_i7_4_lut.init = 16'hcac0;
    PFUMX s_mem_rd_sel_1__I_0_Mux_10_i3 (.BLUT(n1_adj_1627), .ALUT(n2_adj_1626), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[10]));
    LUT4 i6132_3_lut (.A(q_15__N_76), .B(q_15__N_78), .C(q_15__N_80), 
         .Z(n7225)) /* synthesis lut_function=(A+((C)+!B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam i6132_3_lut.init = 16'hfbfb;
    PFUMX s_mem_rd_sel_1__I_0_Mux_11_i3 (.BLUT(n1_adj_1629), .ALUT(n2_adj_1628), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[11]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_12_i3 (.BLUT(n1_adj_1632), .ALUT(n2_adj_1631), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[12]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_13_i3 (.BLUT(n1_adj_1637), .ALUT(n2_adj_1636), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[13]));
    LUT4 s_reg_rd_sel_3__I_0_Mux_4_i7_4_lut (.A(s_r3[4]), .B(n7406), .C(q_15__N_78), 
         .D(n3064), .Z(n7_adj_1634)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_4_i7_4_lut.init = 16'hcac0;
    PFUMX s_mem_rd_sel_1__I_0_Mux_14_i3 (.BLUT(n1_adj_1639), .ALUT(n2_adj_1638), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[14]));
    PFUMX s_mem_rd_sel_1__I_0_Mux_15_i3 (.BLUT(n1_adj_1641), .ALUT(n2_adj_1640), 
          .C0(s_mem_rd_sel[1]), .Z(s_mem_dout[15]));
    L6MUX21 i5799 (.D0(n7460), .D1(n7461), .SD(q_15__N_78), .Z(n7462));
    L6MUX21 i5824 (.D0(n7485), .D1(n7486), .SD(q_15__N_78), .Z(n7487));
    L6MUX21 i5831 (.D0(n7492), .D1(n7493), .SD(q_15__N_78), .Z(n7494));
    L6MUX21 i5838 (.D0(n7499), .D1(n7500), .SD(q_15__N_78), .Z(n7501));
    PFUMX i6195 (.BLUT(n7784), .ALUT(n7780), .C0(n1543), .Z(n7785));
    PFUMX i46 (.BLUT(n7183), .ALUT(n7181), .C0(n1545), .Z(n7185));
    LUT4 n1583_bdd_2_lut_6192_3_lut (.A(s_addr2_adj_1783[3]), .B(s_addr2_adj_1783[0]), 
         .C(s_addr2_adj_1783[1]), .Z(n7781)) /* synthesis lut_function=(!(A+(B+!(C)))) */ ;
    defparam n1583_bdd_2_lut_6192_3_lut.init = 16'h1010;
    PFUMX i42 (.BLUT(n6_adj_1654), .ALUT(n11), .C0(n1545), .Z(n20));
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
            .s_ptr_data({s_ptr_data}));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(748[12:24])
    LUT4 n7185_bdd_2_lut_6207_3_lut (.A(s_addr2_adj_1783[3]), .B(s_addr2_adj_1783[0]), 
         .C(s_addr2_adj_1783[1]), .Z(n7796)) /* synthesis lut_function=(!(A+(B+(C)))) */ ;
    defparam n7185_bdd_2_lut_6207_3_lut.init = 16'h0101;
    LUT4 n24_bdd_2_lut_6267_3_lut_4_lut (.A(s_addr2_adj_1783[3]), .B(s_addr2_adj_1783[0]), 
         .C(s_addr2_adj_1783[1]), .D(s_addr2_adj_1783[2]), .Z(n7861)) /* synthesis lut_function=(!(A+(B+((D)+!C)))) */ ;
    defparam n24_bdd_2_lut_6267_3_lut_4_lut.init = 16'h0010;
    LUT4 i5547_2_lut_3_lut_4_lut (.A(s_addr2_adj_1783[3]), .B(s_addr2_adj_1783[0]), 
         .C(s_addr2_adj_1783[1]), .D(s_addr2_adj_1783[2]), .Z(n7207)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i5547_2_lut_3_lut_4_lut.init = 16'hfffe;
    PFUMX i46_adj_215 (.BLUT(n7_adj_1653), .ALUT(n13), .C0(n1545), .Z(n24));
    PFUMX i5822 (.BLUT(n7481), .ALUT(n7482), .C0(q_15__N_80), .Z(n7485));
    \dpram(16,8)_U9  aux_mem (.fsmc_clk_c(fsmc_clk_c), .s_aux_mem_wr_stb(s_aux_mem_wr_stb), 
            .VCC_net(VCC_net), .GND_net(GND_net), .n8852(n8852), .n8855(n8855), 
            .q_15__N_78(q_15__N_78), .q_15__N_76(q_15__N_76), .q_15__N_74(q_15__N_74), 
            .q_15__N_72(q_15__N_72), .q_15__N_70(q_15__N_70), .q_15__N_68(q_15__N_68), 
            .fsmc_d_out_0(fsmc_d_out_0), .fsmc_d_out_1(fsmc_d_out_1), .fsmc_d_out_2(fsmc_d_out_2), 
            .fsmc_d_out_3(fsmc_d_out_3), .fsmc_d_out_4(fsmc_d_out_4), .fsmc_d_out_5(fsmc_d_out_5), 
            .fsmc_d_out_6(fsmc_d_out_6), .fsmc_d_out_7(fsmc_d_out_7), .fsmc_d_out_8(fsmc_d_out_8), 
            .fsmc_d_out_9(fsmc_d_out_9), .fsmc_d_out_10(fsmc_d_out_10), 
            .fsmc_d_out_11(fsmc_d_out_11), .fsmc_d_out_12(fsmc_d_out_12), 
            .fsmc_d_out_13(fsmc_d_out_13), .fsmc_d_out_14(fsmc_d_out_14), 
            .fsmc_d_out_15(fsmc_d_out_15), .s_aux_mem_dout({s_aux_mem_dout}));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(806[12:24])
    LUT4 i1_2_lut_3_lut_4_lut (.A(s_addr3[3]), .B(s_addr3[0]), .C(s_addr3[2]), 
         .D(s_addr3[1]), .Z(n7149)) /* synthesis lut_function=(!(A+((C+!(D))+!B))) */ ;
    defparam i1_2_lut_3_lut_4_lut.init = 16'h0400;
    LUT4 n3482_bdd_3_lut_6343_4_lut (.A(s_addr3[3]), .B(s_addr3[0]), .C(s_addr3[2]), 
         .D(s_addr3[1]), .Z(n7789)) /* synthesis lut_function=(!(A+(((D)+!C)+!B))) */ ;
    defparam n3482_bdd_3_lut_6343_4_lut.init = 16'h0040;
    LUT4 i1_2_lut_3_lut_4_lut_adj_216 (.A(s_addr3[3]), .B(s_addr3[0]), .C(s_addr3[2]), 
         .D(s_addr3[1]), .Z(n7150)) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_216.init = 16'h4000;
    LUT4 n1583_bdd_2_lut_6201_3_lut (.A(s_addr2_adj_1783[3]), .B(s_addr2_adj_1783[0]), 
         .C(s_addr2_adj_1783[1]), .Z(n7790)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;
    defparam n1583_bdd_2_lut_6201_3_lut.init = 16'h0404;
    LUT4 n1583_bdd_2_lut_6191_3_lut_4_lut (.A(s_addr3[3]), .B(s_addr3[0]), 
         .C(s_addr3[1]), .D(s_addr3[2]), .Z(n7780)) /* synthesis lut_function=(!(A+(B+!(C (D))))) */ ;
    defparam n1583_bdd_2_lut_6191_3_lut_4_lut.init = 16'h1000;
    LUT4 n7145_bdd_2_lut_6219_3_lut_4_lut (.A(s_addr3[3]), .B(s_addr3[0]), 
         .C(s_addr3[1]), .D(s_addr3[2]), .Z(n7795)) /* synthesis lut_function=(!(A+(B+(C+!(D))))) */ ;
    defparam n7145_bdd_2_lut_6219_3_lut_4_lut.init = 16'h0100;
    LUT4 i5537_2_lut_rep_172_3_lut (.A(s_addr3[3]), .B(s_addr3[0]), .C(s_addr3[2]), 
         .Z(n8286)) /* synthesis lut_function=(A+(B+(C))) */ ;
    defparam i5537_2_lut_rep_172_3_lut.init = 16'hfefe;
    LUT4 i1_2_lut_3_lut_4_lut_adj_217 (.A(s_addr3[3]), .B(s_addr3[0]), .C(s_addr3[1]), 
         .D(s_addr3[2]), .Z(n7128)) /* synthesis lut_function=(!(A+(B+((D)+!C)))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_217.init = 16'h0010;
    LUT4 i1275_2_lut (.A(q_15__N_82), .B(q_15__N_80), .Z(n3064)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam i1275_2_lut.init = 16'h8888;
    LUT4 s_reg_rd_sel_3__I_0_Mux_6_i4_3_lut (.A(s_ckgen_div[6]), .B(s_ckgen_rtdiv[6]), 
         .C(q_15__N_82), .Z(n4)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(444[2] 457[31])
    defparam s_reg_rd_sel_3__I_0_Mux_6_i4_3_lut.init = 16'hcaca;
    LUT4 i5982_3_lut_4_lut (.A(s_addr0[1]), .B(s_addr0[0]), .C(n1545), 
         .D(n11_adj_1656), .Z(n20_adj_1655)) /* synthesis lut_function=(A (C (D))+!A (B ((D)+!C)+!B (C (D)))) */ ;
    defparam i5982_3_lut_4_lut.init = 16'hf404;
    LUT4 i1_4_lut_adj_218 (.A(out_get_N_312), .B(n7860), .C(n7149), .D(n1543), 
         .Z(s_r3_wr_stb)) /* synthesis lut_function=(A (B (C+!(D))+!B (C (D)))) */ ;
    defparam i1_4_lut_adj_218.init = 16'ha088;
    \jtag_led_drv(31,false,false,false)_U3  led_con_drv (.mclk_c(mclk_c), 
            .led_2_c(led_2_c), .n8288(n8288), .s_1khz_stb(s_1khz_stb));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(948[16:35])
    LUT4 i5649_3_lut (.A(s_cfg[5]), .B(s_tmr[5]), .C(q_15__N_82), .Z(n7312)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5649_3_lut.init = 16'hcaca;
    LUT4 i5648_3_lut (.A(s_ckgen_div[5]), .B(s_ckgen_rtdiv[5]), .C(q_15__N_82), 
         .Z(n7311)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5648_3_lut.init = 16'hcaca;
    LUT4 s_tmr_cnt_en_I_0_1_lut (.A(s_tmr_cnt_en), .Z(s_tmr_cnt_en_N_1)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(608[15:31])
    defparam s_tmr_cnt_en_I_0_1_lut.init = 16'h5555;
    PUR PUR_INST (.PUR(VCC_net));
    defparam PUR_INST.RST_PULSE = 1;
    PFUMX i5823 (.BLUT(n7483), .ALUT(n7484), .C0(q_15__N_80), .Z(n7486));
    \jtag_reg(16,4)  int_en_r (.\s_irq_en[0] (s_irq_en[0]), .mclk_c(mclk_c), 
            .s_irq_en_wr_stb(s_irq_en_wr_stb), .\s_reg_din[0] (s_reg_din[0]), 
            .\s_irq_en[1] (s_irq_en[1]), .\s_reg_din[1] (s_reg_din[1]), 
            .\s_irq_en[2] (s_irq_en[2]), .\s_reg_din[2] (s_reg_din[2]), 
            .\s_irq_en[3] (s_irq_en[3]), .\s_reg_din[3] (s_reg_din[3]));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(533[13:28])
    PFUMX i6268 (.BLUT(n7862), .ALUT(n7861), .C0(n1544), .Z(n7863));
    LUT4 i5846_3_lut (.A(s_cfg[14]), .B(s_tmr[14]), .C(q_15__N_82), .Z(n7509)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5846_3_lut.init = 16'hcaca;
    LUT4 i5845_3_lut (.A(s_ckgen_div[14]), .B(s_ckgen_rtdiv[14]), .C(q_15__N_82), 
         .Z(n7508)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5845_3_lut.init = 16'hcaca;
    PFUMX i6265 (.BLUT(n7859), .ALUT(n7858), .C0(n1544), .Z(n7860));
    LUT4 s_desc_lo_8__bdd_3_lut_6213 (.A(s_desc_lo[8]), .B(q_15__N_82), 
         .C(s_desc_hi[8]), .Z(n7801)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_8__bdd_3_lut_6213.init = 16'he2e2;
    LUT4 s_desc_lo_8__bdd_3_lut (.A(s_mem_wr_addr[8]), .B(s_mem_rd_addr[8]), 
         .C(q_15__N_82), .Z(n7802)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_8__bdd_3_lut.init = 16'hcaca;
    LUT4 s_1mhz_stb_I_0_2_lut_rep_213 (.A(s_1mhz_stb), .B(s_tmr_cnt_en), 
         .Z(n8327)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(593[19:46])
    defparam s_1mhz_stb_I_0_2_lut_rep_213.init = 16'h8888;
    LUT4 q_15__I_21_2_lut_rep_181_3_lut (.A(s_1mhz_stb), .B(s_tmr_cnt_en), 
         .C(s_tmr[0]), .Z(n8295)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(593[19:46])
    defparam q_15__I_21_2_lut_rep_181_3_lut.init = 16'h0808;
    LUT4 s_desc_lo_9__bdd_3_lut_6216 (.A(s_desc_lo[9]), .B(q_15__N_82), 
         .C(s_desc_hi[9]), .Z(n7804)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_9__bdd_3_lut_6216.init = 16'he2e2;
    LUT4 q_15__I_20_2_lut_rep_162_3_lut_4_lut (.A(s_1mhz_stb), .B(s_tmr_cnt_en), 
         .C(s_tmr[1]), .D(s_tmr[0]), .Z(n8276)) /* synthesis lut_function=(!(((C+(D))+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(593[19:46])
    defparam q_15__I_20_2_lut_rep_162_3_lut_4_lut.init = 16'h0008;
    LUT4 i5843_3_lut (.A(s_cfg[13]), .B(s_tmr[13]), .C(q_15__N_82), .Z(n7506)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5843_3_lut.init = 16'hcaca;
    LUT4 i1_4_lut_adj_219 (.A(n8284), .B(n22), .C(n8283), .D(n1544), 
         .Z(n17)) /* synthesis lut_function=(A (B (C+!(D))+!B (C (D)))) */ ;
    defparam i1_4_lut_adj_219.init = 16'ha088;
    LUT4 i5842_3_lut (.A(s_ckgen_div[13]), .B(s_ckgen_rtdiv[13]), .C(q_15__N_82), 
         .Z(n7505)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5842_3_lut.init = 16'hcaca;
    LUT4 s_desc_lo_9__bdd_3_lut (.A(s_mem_wr_addr[9]), .B(s_mem_rd_addr[9]), 
         .C(q_15__N_82), .Z(n7805)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_9__bdd_3_lut.init = 16'hcaca;
    LUT4 i5646_3_lut (.A(s_cfg[15]), .B(s_tmr[15]), .C(q_15__N_82), .Z(n7309)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5646_3_lut.init = 16'hcaca;
    \jtag_reg(16,16)_U6  ckgen_rtdiv_r (.s_ckgen_rtdiv({s_ckgen_rtdiv}), .mclk_c(mclk_c), 
            .s_ckgen_rtdiv_wr_stb(s_ckgen_rtdiv_wr_stb), .s_reg_din({s_reg_din}));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(710[18:33])
    LUT4 i5645_3_lut (.A(s_ckgen_div[15]), .B(s_ckgen_rtdiv[15]), .C(q_15__N_82), 
         .Z(n7308)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5645_3_lut.init = 16'hcaca;
    LUT4 i5835_3_lut (.A(s_cfg[1]), .B(s_tmr[1]), .C(q_15__N_82), .Z(n7498)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5835_3_lut.init = 16'hcaca;
    LUT4 i5834_3_lut (.A(s_ckgen_div[1]), .B(s_ckgen_rtdiv[1]), .C(q_15__N_82), 
         .Z(n7497)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5834_3_lut.init = 16'hcaca;
    LUT4 i5833_3_lut (.A(s_irq_en[1]), .B(s_r3[1]), .C(q_15__N_82), .Z(n7496)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5833_3_lut.init = 16'hcaca;
    LUT4 i5832_3_lut (.A(s_status[1]), .B(s_irq_st[1]), .C(q_15__N_82), 
         .Z(n7495)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5832_3_lut.init = 16'hcaca;
    LUT4 m1_lut (.Z(n8851)) /* synthesis lut_function=1, syn_instantiated=1 */ ;
    defparam m1_lut.init = 16'hffff;
    LUT4 i5828_3_lut (.A(s_cfg[2]), .B(s_tmr[2]), .C(q_15__N_82), .Z(n7491)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5828_3_lut.init = 16'hcaca;
    LUT4 i5827_3_lut (.A(s_ckgen_div[2]), .B(s_ckgen_rtdiv[2]), .C(q_15__N_82), 
         .Z(n7490)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5827_3_lut.init = 16'hcaca;
    PFUMX i5704 (.BLUT(n7365), .ALUT(n7366), .C0(q_15__N_80), .Z(n7367));
    PFUMX i5707 (.BLUT(n7368), .ALUT(n7369), .C0(q_15__N_80), .Z(n7370));
    LUT4 i5826_3_lut (.A(s_irq_en[2]), .B(s_r3[2]), .C(q_15__N_82), .Z(n7489)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5826_3_lut.init = 16'hcaca;
    PFUMX i5713 (.BLUT(n7374), .ALUT(n7375), .C0(q_15__N_80), .Z(n7376));
    LUT4 i5825_3_lut (.A(s_status[2]), .B(s_irq_st[2]), .C(q_15__N_82), 
         .Z(n7488)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5825_3_lut.init = 16'hcaca;
    PFUMX i5719 (.BLUT(n7380), .ALUT(n7381), .C0(q_15__N_80), .Z(n7382));
    \jtag_clk_scaler(60)  cklgen_1mhz (.mclk_c(mclk_c), .s_1mhz_stb(s_1mhz_stb), 
            .GND_net(GND_net));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(898[16:38])
    PFUMX i5722 (.BLUT(n7383), .ALUT(n7384), .C0(q_15__N_80), .Z(n7385));
    LUT4 m0_lut (.Z(n8850)) /* synthesis lut_function=0, syn_instantiated=1 */ ;
    defparam m0_lut.init = 16'h0000;
    LUT4 i5807_3_lut (.A(s_desc_lo[2]), .B(s_desc_hi[2]), .C(q_15__N_82), 
         .Z(n7470)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5807_3_lut.init = 16'hcaca;
    LUT4 i5806_3_lut (.A(s_mem_wr_addr[2]), .B(s_mem_rd_addr[2]), .C(q_15__N_82), 
         .Z(n7469)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5806_3_lut.init = 16'hcaca;
    \jtag_enh_shifter(16,10,10,16,8,32,8,16,16)  shifter (.n2598(n2598), .n2614(n2614), 
            .n2615(n2615), .n2597(n2597), .n2613(n2613), .n2596(n2596), 
            .n2612(n2612), .n2595(n2595), .n2611(n2611), .n2594(n2594), 
            .n2610(n2610), .n2593(n2593), .n2609(n2609), .n2592(n2592), 
            .n2608(n2608), .n2591(n2591), .n2607(n2607), .n2590(n2590), 
            .n2606(n2606), .n2589(n2589), .n2605(n2605), .n2588(n2588), 
            .n2604(n2604), .n2587(n2587), .n2603(n2603), .n2586(n2586), 
            .n2602(n2602), .n2585(n2585), .n2601(n2601), .n2584(n2584), 
            .n2600(n2600), .n2583(n2583), .n2599(n2599), .mclk_c(mclk_c), 
            .s_addr_0({s_addr_0}), .GND_net(GND_net), .\s_desc_data[18] (s_desc_data[18]), 
            .\s_desc_data[19] (s_desc_data[19]), .\s_desc_data[16] (s_desc_data[16]), 
            .\s_desc_data[17] (s_desc_data[17]), .\s_desc_data[14] (s_desc_data[14]), 
            .\s_desc_data[15] (s_desc_data[15]), .\s_desc_data[12] (s_desc_data[12]), 
            .\s_desc_data[13] (s_desc_data[13]), .\s_desc_data[10] (s_desc_data[10]), 
            .\s_desc_data[11] (s_desc_data[11]), .\s_desc_data[8] (s_desc_data[8]), 
            .\s_desc_data[30] (s_desc_data[30]), .\s_desc_data[9] (s_desc_data[9]), 
            .\s_desc_data[6] (s_desc_data[6]), .\s_desc_data[7] (s_desc_data[7]), 
            .\s_desc_data[4] (s_desc_data[4]), .\s_desc_data[5] (s_desc_data[5]), 
            .s_pos_0_3__N_670({s_pos_0_3__N_670}), .s_ptr_data({s_ptr_data}), 
            .\s_cfg[6] (s_cfg[6]), .mclk_c_enable_118(mclk_c_enable_118), 
            .s_desc_addr({s_desc_addr}), .s_rx_mem_wr_stb(s_rx_mem_wr_stb), 
            .\s_desc_data[31] (s_desc_data[31]), .s_rx_mem_addr({s_rx_mem_addr}), 
            .n8850(n8850), .s_rx_mem_din({s_rx_mem_din}), .\s_desc_data[29] (s_desc_data[29]), 
            .\s_desc_data[27] (s_desc_data[27]), .\s_desc_data[28] (s_desc_data[28]), 
            .\s_desc_data[25] (s_desc_data[25]), .\s_desc_data[26] (s_desc_data[26]), 
            .\s_desc_data[23] (s_desc_data[23]), .\s_desc_data[24] (s_desc_data[24]), 
            .\s_desc_data[21] (s_desc_data[21]), .\s_desc_data[22] (s_desc_data[22]), 
            .\s_desc_data[20] (s_desc_data[20]), .n8851(n8851), .mclk_c_enable_91(mclk_c_enable_91), 
            .tp_tms_c(tp_tms_c), .s_tap_tdo(s_tap_tdo), .s_tap_tdi(s_tap_tdi), 
            .\s_status[0] (s_status[0]), .\s_status[1] (s_status[1]), .\s_status[2] (s_status[2]), 
            .\s_status[3] (s_status[3]), .s_end_state({s_end_state}), .s_reg_din({s_reg_din}), 
            .n8264(n8264), .n8279(n8279), .s_tap_busy(s_tap_busy), .n2(n2_adj_1633), 
            .n8273(n8273), .n8274(n8274), .n8317(n8317), .out_get_N_312(out_get_N_312), 
            .n7217(n7217), .n4959(n4959), .s_ptr_addr({s_ptr_addr}), .s_cnt_en_N_1514(s_cnt_en_N_1514), 
            .\s_goto_state[0] (s_goto_state[0]), .\s_goto_state[2] (s_goto_state[2]), 
            .\s_goto_state[1] (s_goto_state[1]), .n1(n1_adj_1642), .n7227(n7227), 
            .\s_irq[1] (s_irq[1]), .s_ckgen_rtdiv({s_ckgen_rtdiv}), .\s_cfg[5] (s_cfg[5]), 
            .tp_rtck_c(tp_rtck_c), .s_ckgen_div({s_ckgen_div}), .\s_irq_en[1] (s_irq_en[1]), 
            .\irq_N_8[1] (irq_N_8[1]), .tp_tck_c(tp_tck_c));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(841[12:35])
    PFUMX i5734 (.BLUT(n7395), .ALUT(n7396), .C0(q_15__N_80), .Z(n7397));
    PFUMX i5740 (.BLUT(n7401), .ALUT(n7402), .C0(q_15__N_80), .Z(n7403));
    PFUMX i5743 (.BLUT(n7404), .ALUT(n7405), .C0(q_15__N_80), .Z(n7406));
    LUT4 s_desc_lo_11__bdd_3_lut (.A(s_mem_wr_addr[11]), .B(s_mem_rd_addr[11]), 
         .C(q_15__N_82), .Z(n7821)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam s_desc_lo_11__bdd_3_lut.init = 16'hcaca;
    LUT4 s_desc_lo_11__bdd_3_lut_6231 (.A(s_desc_lo[11]), .B(q_15__N_82), 
         .C(s_desc_hi[11]), .Z(n7820)) /* synthesis lut_function=(A ((C)+!B)+!A (B (C))) */ ;
    defparam s_desc_lo_11__bdd_3_lut_6231.init = 16'he2e2;
    \dpram(16,10)_U1  rx_mem (.mclk_c(mclk_c), .fsmc_clk_c(fsmc_clk_c), 
            .VCC_net(VCC_net), .GND_net(GND_net), .s_rx_mem_addr({s_rx_mem_addr}), 
            .q_15__N_82(q_15__N_82), .q_15__N_80(q_15__N_80), .q_15__N_78(q_15__N_78), 
            .q_15__N_76(q_15__N_76), .q_15__N_74(q_15__N_74), .q_15__N_72(q_15__N_72), 
            .q_15__N_70(q_15__N_70), .q_15__N_68(q_15__N_68), .q_15__N_66(q_15__N_66), 
            .s_rx_mem_din({s_rx_mem_din}), .n2562(n2562), .n2563(n2563), 
            .n2564(n2564), .n2565(n2565), .n2566(n2566), .n2567(n2567), 
            .n2568(n2568), .n2569(n2569), .n2570(n2570), .n2571(n2571), 
            .n2572(n2572), .n2573(n2573), .n2574(n2574), .n2575(n2575), 
            .n2576(n2576), .n2577(n2577), .n2546(n2546), .n2547(n2547), 
            .n2548(n2548), .n2549(n2549), .n2550(n2550), .n2551(n2551), 
            .n2552(n2552), .n2553(n2553), .n2554(n2554), .n2555(n2555), 
            .n2556(n2556), .n2557(n2557), .n2558(n2558), .n2559(n2559), 
            .n2560(n2560), .n2561(n2561), .n2578(n2578), .q_15__N_64(q_15__N_64), 
            .s_rx_mem_wr_stb(s_rx_mem_wr_stb));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(767[11:23])
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
            .fsmc_d_out_15(fsmc_d_out_15), .n2583(n2583), .n2584(n2584), 
            .n2585(n2585), .n2586(n2586), .n2587(n2587), .n2588(n2588), 
            .n2589(n2589), .n2590(n2590), .n2591(n2591), .n2592(n2592), 
            .n2593(n2593), .n2594(n2594), .n2595(n2595), .n2596(n2596), 
            .n2597(n2597), .n2598(n2598), .fsmc_clk_c_enable_2(fsmc_clk_c_enable_2), 
            .n8853(n8853), .n8857(n8857), .n2599(n2599), .n2600(n2600), 
            .n2601(n2601), .n2602(n2602), .n2603(n2603), .n2604(n2604), 
            .n2605(n2605), .n2606(n2606), .n2607(n2607), .n2608(n2608), 
            .n2609(n2609), .n2610(n2610), .n2611(n2611), .n2612(n2612), 
            .n2613(n2613), .n2614(n2614), .n2615(n2615));   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(786[11:23])
    VLO i1 (.Z(GND_net));
    PFUMX i5792 (.BLUT(n7453), .ALUT(n7454), .C0(q_15__N_80), .Z(n7455));
    PFUMX i5797 (.BLUT(n7456), .ALUT(n7457), .C0(q_15__N_80), .Z(n7460));
    PFUMX i5798 (.BLUT(n7458), .ALUT(n7459), .C0(q_15__N_80), .Z(n7461));
    
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
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    wire [4:0]s_count;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(59[9:16])
    
    wire s_cnt_ld, n3132, mclk_c_enable_59, n3693, s_trip_r1, s_trip_r0, 
        n4024, n3691, n3689, n3695, n6, n3628, n8316, n8319;
    
    FD1S3JX s_count_i0_i0 (.D(n3132), .CK(mclk_c), .PD(s_cnt_ld), .Q(s_count[0])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=937, LSE_RLINE=937 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i0.GSR = "ENABLED";
    FD1P3JX s_count_i0_i4 (.D(n3693), .SP(mclk_c_enable_59), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[4])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=937, LSE_RLINE=937 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i4.GSR = "ENABLED";
    FD1S3AX s_trip_r1_31 (.D(s_trip_r0), .CK(mclk_c), .Q(s_trip_r1)) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=937, LSE_RLINE=937 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(95[3] 105[10])
    defparam s_trip_r1_31.GSR = "ENABLED";
    FD1S3AX s_drv_ff_32 (.D(n4024), .CK(mclk_c), .Q(led_1_c)) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=937, LSE_RLINE=937 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(113[3] 121[10])
    defparam s_drv_ff_32.GSR = "ENABLED";
    FD1S3AX s_trip_r0_30 (.D(out_get_N_312), .CK(mclk_c), .Q(s_trip_r0)) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=937, LSE_RLINE=937 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(95[3] 105[10])
    defparam s_trip_r0_30.GSR = "ENABLED";
    FD1P3JX s_count_i0_i3 (.D(n3691), .SP(mclk_c_enable_59), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[3])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=937, LSE_RLINE=937 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i3.GSR = "ENABLED";
    FD1P3JX s_count_i0_i2 (.D(n3689), .SP(mclk_c_enable_59), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[2])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=937, LSE_RLINE=937 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i2.GSR = "ENABLED";
    FD1P3JX s_count_i0_i1 (.D(n3695), .SP(mclk_c_enable_59), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[1])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=937, LSE_RLINE=937 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i1.GSR = "ENABLED";
    LUT4 i4_4_lut (.A(s_count[3]), .B(s_count[2]), .C(s_count[0]), .D(n6), 
         .Z(n3628)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(140[22:45])
    defparam i4_4_lut.init = 16'hfffe;
    LUT4 i1_2_lut (.A(s_count[1]), .B(s_count[4]), .Z(n6)) /* synthesis lut_function=(A+(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(140[22:45])
    defparam i1_2_lut.init = 16'heeee;
    LUT4 i1_3_lut_4_lut (.A(s_count[2]), .B(n8316), .C(s_count[3]), .D(s_count[4]), 
         .Z(n3693)) /* synthesis lut_function=(A (D)+!A (B (D)+!B (C (D)+!C !(D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i1_3_lut_4_lut.init = 16'hfe01;
    LUT4 i1_2_lut_adj_203 (.A(s_count[1]), .B(s_count[0]), .Z(n3695)) /* synthesis lut_function=(A (B)+!A !(B)) */ ;
    defparam i1_2_lut_adj_203.init = 16'h9999;
    LUT4 i949_2_lut_rep_202 (.A(s_count[1]), .B(s_count[0]), .Z(n8316)) /* synthesis lut_function=(A+(B)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i949_2_lut_rep_202.init = 16'heeee;
    LUT4 i1_2_lut_3_lut (.A(s_count[1]), .B(s_count[0]), .C(s_count[2]), 
         .Z(n3689)) /* synthesis lut_function=(A (C)+!A (B (C)+!B !(C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i1_2_lut_3_lut.init = 16'he1e1;
    LUT4 i1_2_lut_3_lut_4_lut (.A(s_count[1]), .B(s_count[0]), .C(s_count[3]), 
         .D(s_count[2]), .Z(n3691)) /* synthesis lut_function=(A (C)+!A (B (C)+!B (C (D)+!C !(D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i1_2_lut_3_lut_4_lut.init = 16'hf0e1;
    LUT4 z_I_0_2_lut_3_lut (.A(s_trip_r0), .B(s_trip_r1), .C(n3628), .Z(s_cnt_ld)) /* synthesis lut_function=(!(A (B (C))+!A (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(108[15:42])
    defparam z_I_0_2_lut_3_lut.init = 16'h2f2f;
    LUT4 i2_3_lut_4_lut (.A(s_trip_r0), .B(s_trip_r1), .C(n3628), .D(n8319), 
         .Z(mclk_c_enable_59)) /* synthesis lut_function=(A (((D)+!C)+!B)+!A ((D)+!C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(108[15:42])
    defparam i2_3_lut_4_lut.init = 16'hff2f;
    LUT4 i2318_3_lut_4_lut (.A(s_trip_r0), .B(s_trip_r1), .C(led_1_c), 
         .D(n3628), .Z(n4024)) /* synthesis lut_function=(A ((C (D))+!B)+!A (C (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(108[15:42])
    defparam i2318_3_lut_4_lut.init = 16'hf222;
    LUT4 i23_2_lut_rep_205 (.A(s_1khz_stb), .B(led_1_c), .Z(n8319)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(134[10:41])
    defparam i23_2_lut_rep_205.init = 16'h8888;
    LUT4 i1814_2_lut_3_lut (.A(s_1khz_stb), .B(led_1_c), .C(s_count[0]), 
         .Z(n3132)) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(134[10:41])
    defparam i1814_2_lut_3_lut.init = 16'h7878;
    
endmodule
//
// Verilog Description of module \jtag_reg(16,16)_U5 
//

module \jtag_reg(16,16)_U5  (s_desc_hi, mclk_c, mclk_c_enable_118, \s_desc_data[16] , 
            \s_desc_data[31] , \s_desc_data[30] , \s_desc_data[29] , \s_desc_data[28] , 
            \s_desc_data[27] , \s_desc_data[26] , \s_desc_data[25] , \s_desc_data[24] , 
            \s_desc_data[23] , \s_desc_data[22] , \s_desc_data[21] , \s_desc_data[20] , 
            \s_desc_data[19] , \s_desc_data[18] , \s_desc_data[17] );
    output [15:0]s_desc_hi;
    input mclk_c;
    input mclk_c_enable_118;
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
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    FD1P3AX s_reg_i0_i0 (.D(\s_desc_data[16] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[0])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i0.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i15 (.D(\s_desc_data[31] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[15])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i15.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i14 (.D(\s_desc_data[30] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[14])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i14.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i13 (.D(\s_desc_data[29] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[13])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i13.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i12 (.D(\s_desc_data[28] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[12])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i12.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i11 (.D(\s_desc_data[27] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[11])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i11.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i10 (.D(\s_desc_data[26] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[10])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i10.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i9 (.D(\s_desc_data[25] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[9])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i9.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i8 (.D(\s_desc_data[24] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[8])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i8.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i7 (.D(\s_desc_data[23] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[7])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i7.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i6 (.D(\s_desc_data[22] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[6])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i6.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i5 (.D(\s_desc_data[21] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[5])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i5.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i4 (.D(\s_desc_data[20] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[4])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i4.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i3 (.D(\s_desc_data[19] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[3])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i3.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i2 (.D(\s_desc_data[18] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[2])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i2.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i1 (.D(\s_desc_data[17] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_hi[1])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=676, LSE_RLINE=676 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i1.GSR = "ENABLED";
    
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
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(40[2:10])
    
    FD1P3AX s_reg__i1 (.D(q_15__N_82), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[1] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=632, LSE_RLINE=632 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i1.GSR = "ENABLED";
    FD1P3AX s_reg__i11 (.D(q_15__N_62), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[11] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=632, LSE_RLINE=632 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i11.GSR = "ENABLED";
    FD1P3AX s_reg__i10 (.D(q_15__N_64), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[10] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=632, LSE_RLINE=632 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i10.GSR = "ENABLED";
    FD1P3AX s_reg__i9 (.D(q_15__N_66), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[9] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=632, LSE_RLINE=632 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i9.GSR = "ENABLED";
    FD1P3AX s_reg__i8 (.D(q_15__N_68), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[8] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=632, LSE_RLINE=632 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i8.GSR = "ENABLED";
    FD1P3AX s_reg__i7 (.D(q_15__N_70), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[7] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=632, LSE_RLINE=632 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i7.GSR = "ENABLED";
    FD1P3AX s_reg__i6 (.D(q_15__N_72), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[6] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=632, LSE_RLINE=632 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i6.GSR = "ENABLED";
    FD1P3AX s_reg__i5 (.D(q_15__N_74), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[5] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=632, LSE_RLINE=632 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i5.GSR = "ENABLED";
    FD1P3AX s_reg__i4 (.D(q_15__N_76), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[4] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=632, LSE_RLINE=632 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i4.GSR = "ENABLED";
    FD1P3AX s_reg__i3 (.D(q_15__N_78), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[3] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=632, LSE_RLINE=632 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i3.GSR = "ENABLED";
    FD1P3AX s_reg__i2 (.D(q_15__N_80), .SP(s_dbg_mem_wr), .CK(fsmc_clk_c), 
            .Q(\s_mem_wr_addr[2] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=632, LSE_RLINE=632 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i2.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module \jtag_reg(16,16)_U4 
//

module \jtag_reg(16,16)_U4  (s_desc_lo, mclk_c, mclk_c_enable_118, \s_desc_data[15] , 
            \s_desc_data[14] , \s_desc_data[13] , \s_desc_data[12] , \s_desc_data[11] , 
            \s_desc_data[10] , \s_desc_data[9] , \s_desc_data[8] , \s_desc_data[7] , 
            \s_desc_data[6] , \s_desc_data[5] , \s_desc_data[4] , s_pos_0_3__N_670);
    output [15:0]s_desc_lo;
    input mclk_c;
    input mclk_c_enable_118;
    input \s_desc_data[15] ;
    input \s_desc_data[14] ;
    input \s_desc_data[13] ;
    input \s_desc_data[12] ;
    input \s_desc_data[11] ;
    input \s_desc_data[10] ;
    input \s_desc_data[9] ;
    input \s_desc_data[8] ;
    input \s_desc_data[7] ;
    input \s_desc_data[6] ;
    input \s_desc_data[5] ;
    input \s_desc_data[4] ;
    input [3:0]s_pos_0_3__N_670;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    FD1P3AX s_reg_i0_i15 (.D(\s_desc_data[15] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_lo[15])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i15.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i14 (.D(\s_desc_data[14] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_lo[14])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i14.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i13 (.D(\s_desc_data[13] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_lo[13])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i13.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i12 (.D(\s_desc_data[12] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_lo[12])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i12.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i11 (.D(\s_desc_data[11] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_lo[11])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i11.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i10 (.D(\s_desc_data[10] ), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_lo[10])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i10.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i9 (.D(\s_desc_data[9] ), .SP(mclk_c_enable_118), .CK(mclk_c), 
            .Q(s_desc_lo[9])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i9.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i8 (.D(\s_desc_data[8] ), .SP(mclk_c_enable_118), .CK(mclk_c), 
            .Q(s_desc_lo[8])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i8.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i7 (.D(\s_desc_data[7] ), .SP(mclk_c_enable_118), .CK(mclk_c), 
            .Q(s_desc_lo[7])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i7.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i6 (.D(\s_desc_data[6] ), .SP(mclk_c_enable_118), .CK(mclk_c), 
            .Q(s_desc_lo[6])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i6.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i5 (.D(\s_desc_data[5] ), .SP(mclk_c_enable_118), .CK(mclk_c), 
            .Q(s_desc_lo[5])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i5.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i4 (.D(\s_desc_data[4] ), .SP(mclk_c_enable_118), .CK(mclk_c), 
            .Q(s_desc_lo[4])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i4.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i3 (.D(s_pos_0_3__N_670[3]), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_lo[3])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i3.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i2 (.D(s_pos_0_3__N_670[2]), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_lo[2])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i2.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i1 (.D(s_pos_0_3__N_670[1]), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_lo[1])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i1.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i0 (.D(s_pos_0_3__N_670[0]), .SP(mclk_c_enable_118), 
            .CK(mclk_c), .Q(s_desc_lo[0])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=12, LSE_RCOL=27, LSE_LLINE=662, LSE_RLINE=662 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i0.GSR = "ENABLED";
    
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
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(40[2:10])
    
    FD1P3AX s_reg__i11 (.D(q_15__N_62), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[11] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=647, LSE_RLINE=647 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i11.GSR = "ENABLED";
    FD1P3AX s_reg__i10 (.D(q_15__N_64), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[10] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=647, LSE_RLINE=647 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i10.GSR = "ENABLED";
    FD1P3AX s_reg__i9 (.D(q_15__N_66), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[9] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=647, LSE_RLINE=647 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i9.GSR = "ENABLED";
    FD1P3AX s_reg__i8 (.D(q_15__N_68), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[8] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=647, LSE_RLINE=647 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i8.GSR = "ENABLED";
    FD1P3AX s_reg__i7 (.D(q_15__N_70), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[7] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=647, LSE_RLINE=647 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i7.GSR = "ENABLED";
    FD1P3AX s_reg__i6 (.D(q_15__N_72), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[6] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=647, LSE_RLINE=647 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i6.GSR = "ENABLED";
    FD1P3AX s_reg__i5 (.D(q_15__N_74), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[5] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=647, LSE_RLINE=647 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i5.GSR = "ENABLED";
    FD1P3AX s_reg__i4 (.D(q_15__N_76), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[4] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=647, LSE_RLINE=647 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i4.GSR = "ENABLED";
    FD1P3AX s_reg__i3 (.D(q_15__N_78), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[3] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=647, LSE_RLINE=647 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i3.GSR = "ENABLED";
    FD1P3AX s_reg__i2 (.D(q_15__N_80), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[2] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=647, LSE_RLINE=647 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i2.GSR = "ENABLED";
    FD1P3AX s_reg__i1 (.D(q_15__N_82), .SP(s_dbg_mem_rd), .CK(fsmc_clk_c), 
            .Q(\s_mem_rd_addr[1] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=647, LSE_RLINE=647 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i1.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module \counter(16,16)_U0 
//

module \counter(16,16)_U0  (s_tmr, mclk_c, q_15__N_51, s_reg_din, s_tmr_cnt_en_N_2, 
            n8295, n8276, n8327);
    output [15:0]s_tmr;
    input mclk_c;
    output q_15__N_51;
    input [15:0]s_reg_din;
    input s_tmr_cnt_en_N_2;
    input n8295;
    input n8276;
    input n8327;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    wire q_1__N_137, q_2__N_133, q_3__N_129, q_4__N_125, q_5__N_121, 
        q_6__N_117, q_7__N_113, q_8__N_109, q_9__N_105, q_10__N_101, 
        q_11__N_97, q_12__N_93, q_13__N_89, q_14__N_85, q_15__N_49, 
        q_0__N_141, n8216, n8201, n8206, n8267, n8256, n8247, 
        n8238, n8233, n8227, n8220, n8210, n8197;
    
    FD1S3AX s_count_reg_1__181 (.D(q_1__N_137), .CK(mclk_c), .Q(s_tmr[1])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_1__181.GSR = "ENABLED";
    FD1S3AX s_count_reg_2__182 (.D(q_2__N_133), .CK(mclk_c), .Q(s_tmr[2])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_2__182.GSR = "ENABLED";
    FD1S3AX s_count_reg_3__183 (.D(q_3__N_129), .CK(mclk_c), .Q(s_tmr[3])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_3__183.GSR = "ENABLED";
    FD1S3AX s_count_reg_4__184 (.D(q_4__N_125), .CK(mclk_c), .Q(s_tmr[4])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_4__184.GSR = "ENABLED";
    FD1S3AX s_count_reg_5__185 (.D(q_5__N_121), .CK(mclk_c), .Q(s_tmr[5])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_5__185.GSR = "ENABLED";
    FD1S3AX s_count_reg_6__186 (.D(q_6__N_117), .CK(mclk_c), .Q(s_tmr[6])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_6__186.GSR = "ENABLED";
    FD1S3AX s_count_reg_7__187 (.D(q_7__N_113), .CK(mclk_c), .Q(s_tmr[7])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_7__187.GSR = "ENABLED";
    FD1S3AX s_count_reg_8__188 (.D(q_8__N_109), .CK(mclk_c), .Q(s_tmr[8])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_8__188.GSR = "ENABLED";
    FD1S3AX s_count_reg_9__189 (.D(q_9__N_105), .CK(mclk_c), .Q(s_tmr[9])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_9__189.GSR = "ENABLED";
    FD1S3AX s_count_reg_10__190 (.D(q_10__N_101), .CK(mclk_c), .Q(s_tmr[10])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_10__190.GSR = "ENABLED";
    FD1S3AX s_count_reg_11__191 (.D(q_11__N_97), .CK(mclk_c), .Q(s_tmr[11])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_11__191.GSR = "ENABLED";
    FD1S3AX s_count_reg_12__192 (.D(q_12__N_93), .CK(mclk_c), .Q(s_tmr[12])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_12__192.GSR = "ENABLED";
    FD1S3AX s_count_reg_13__193 (.D(q_13__N_89), .CK(mclk_c), .Q(s_tmr[13])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_13__193.GSR = "ENABLED";
    FD1S3AX s_count_reg_14__194 (.D(q_14__N_85), .CK(mclk_c), .Q(s_tmr[14])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_14__194.GSR = "ENABLED";
    FD1S3AX s_count_reg_15__195 (.D(q_15__N_49), .CK(mclk_c), .Q(s_tmr[15])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_15__195.GSR = "ENABLED";
    FD1S3AX s_count_reg_0__180 (.D(q_0__N_141), .CK(mclk_c), .Q(s_tmr[0])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=24, LSE_LLINE=576, LSE_RLINE=576 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_0__180.GSR = "ENABLED";
    LUT4 q_15__I_9_2_lut_rep_87_3_lut_4_lut (.A(s_tmr[10]), .B(n8216), .C(s_tmr[12]), 
         .D(s_tmr[11]), .Z(n8201)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_9_2_lut_rep_87_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_7_2_lut_3_lut_4_lut (.A(s_tmr[12]), .B(n8206), .C(s_tmr[14]), 
         .D(s_tmr[13]), .Z(q_15__N_51)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_7_2_lut_3_lut_4_lut.init = 16'h0004;
    LUT4 q_1__I_49_4_lut (.A(s_tmr[1]), .B(s_reg_din[1]), .C(s_tmr_cnt_en_N_2), 
         .D(n8295), .Z(q_1__N_137)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_1__I_49_4_lut.init = 16'hc5ca;
    LUT4 q_2__I_47_4_lut (.A(s_tmr[2]), .B(s_reg_din[2]), .C(s_tmr_cnt_en_N_2), 
         .D(n8276), .Z(q_2__N_133)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_2__I_47_4_lut.init = 16'hc5ca;
    LUT4 q_3__I_45_4_lut (.A(s_tmr[3]), .B(s_reg_din[3]), .C(s_tmr_cnt_en_N_2), 
         .D(n8267), .Z(q_3__N_129)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_3__I_45_4_lut.init = 16'hc5ca;
    LUT4 q_4__I_43_4_lut (.A(s_tmr[4]), .B(s_reg_din[4]), .C(s_tmr_cnt_en_N_2), 
         .D(n8256), .Z(q_4__N_125)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_4__I_43_4_lut.init = 16'hc5ca;
    LUT4 q_5__I_41_4_lut (.A(s_tmr[5]), .B(s_reg_din[5]), .C(s_tmr_cnt_en_N_2), 
         .D(n8247), .Z(q_5__N_121)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_5__I_41_4_lut.init = 16'hc5ca;
    LUT4 q_6__I_39_4_lut (.A(s_tmr[6]), .B(s_reg_din[6]), .C(s_tmr_cnt_en_N_2), 
         .D(n8238), .Z(q_6__N_117)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_6__I_39_4_lut.init = 16'hc5ca;
    LUT4 q_7__I_37_4_lut (.A(s_tmr[7]), .B(s_reg_din[7]), .C(s_tmr_cnt_en_N_2), 
         .D(n8233), .Z(q_7__N_113)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_7__I_37_4_lut.init = 16'hc5ca;
    LUT4 q_8__I_35_4_lut (.A(s_tmr[8]), .B(s_reg_din[8]), .C(s_tmr_cnt_en_N_2), 
         .D(n8227), .Z(q_8__N_109)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_8__I_35_4_lut.init = 16'hc5ca;
    LUT4 q_9__I_33_4_lut (.A(s_tmr[9]), .B(s_reg_din[9]), .C(s_tmr_cnt_en_N_2), 
         .D(n8220), .Z(q_9__N_105)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_9__I_33_4_lut.init = 16'hc5ca;
    LUT4 q_10__I_31_4_lut (.A(s_tmr[10]), .B(s_reg_din[10]), .C(s_tmr_cnt_en_N_2), 
         .D(n8216), .Z(q_10__N_101)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_10__I_31_4_lut.init = 16'hc5ca;
    LUT4 q_11__I_29_4_lut (.A(s_tmr[11]), .B(s_reg_din[11]), .C(s_tmr_cnt_en_N_2), 
         .D(n8210), .Z(q_11__N_97)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_11__I_29_4_lut.init = 16'hc5ca;
    LUT4 q_15__I_10_2_lut_rep_92_3_lut_4_lut (.A(s_tmr[9]), .B(n8220), .C(s_tmr[11]), 
         .D(s_tmr[10]), .Z(n8206)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_10_2_lut_rep_92_3_lut_4_lut.init = 16'h0004;
    LUT4 q_12__I_27_4_lut (.A(s_tmr[12]), .B(s_reg_din[12]), .C(s_tmr_cnt_en_N_2), 
         .D(n8206), .Z(q_12__N_93)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_12__I_27_4_lut.init = 16'hc5ca;
    LUT4 q_13__I_25_4_lut (.A(s_tmr[13]), .B(s_reg_din[13]), .C(s_tmr_cnt_en_N_2), 
         .D(n8201), .Z(q_13__N_89)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_13__I_25_4_lut.init = 16'hc5ca;
    LUT4 q_14__I_23_4_lut (.A(s_tmr[14]), .B(s_reg_din[14]), .C(s_tmr_cnt_en_N_2), 
         .D(n8197), .Z(q_14__N_85)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_14__I_23_4_lut.init = 16'hc5ca;
    LUT4 q_15__I_6_4_lut (.A(s_tmr[15]), .B(s_reg_din[15]), .C(s_tmr_cnt_en_N_2), 
         .D(q_15__N_51), .Z(q_15__N_49)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_15__I_6_4_lut.init = 16'hc5ca;
    LUT4 q_0__I_51_4_lut (.A(s_tmr[0]), .B(s_reg_din[0]), .C(s_tmr_cnt_en_N_2), 
         .D(n8327), .Z(q_0__N_141)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_0__I_51_4_lut.init = 16'hc5ca;
    LUT4 q_15__I_18_2_lut_rep_142_3_lut_4_lut (.A(s_tmr[1]), .B(n8295), 
         .C(s_tmr[3]), .D(s_tmr[2]), .Z(n8256)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_18_2_lut_rep_142_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_11_2_lut_rep_96_3_lut_4_lut (.A(s_tmr[8]), .B(n8227), .C(s_tmr[10]), 
         .D(s_tmr[9]), .Z(n8210)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_11_2_lut_rep_96_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_12_2_lut_rep_102_3_lut_4_lut (.A(s_tmr[7]), .B(n8233), 
         .C(s_tmr[9]), .D(s_tmr[8]), .Z(n8216)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_12_2_lut_rep_102_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_13_2_lut_rep_106_3_lut_4_lut (.A(s_tmr[6]), .B(n8238), 
         .C(s_tmr[8]), .D(s_tmr[7]), .Z(n8220)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_13_2_lut_rep_106_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_19_2_lut_rep_153_3_lut_4_lut (.A(s_tmr[0]), .B(n8327), 
         .C(s_tmr[2]), .D(s_tmr[1]), .Z(n8267)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_19_2_lut_rep_153_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_8_2_lut_rep_83_3_lut_4_lut (.A(s_tmr[11]), .B(n8210), .C(s_tmr[13]), 
         .D(s_tmr[12]), .Z(n8197)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_8_2_lut_rep_83_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_14_2_lut_rep_113_3_lut_4_lut (.A(s_tmr[5]), .B(n8247), 
         .C(s_tmr[7]), .D(s_tmr[6]), .Z(n8227)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_14_2_lut_rep_113_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_15_2_lut_rep_119_3_lut_4_lut (.A(s_tmr[4]), .B(n8256), 
         .C(s_tmr[6]), .D(s_tmr[5]), .Z(n8233)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_15_2_lut_rep_119_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_16_2_lut_rep_124_3_lut_4_lut (.A(s_tmr[3]), .B(n8267), 
         .C(s_tmr[5]), .D(s_tmr[4]), .Z(n8238)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_16_2_lut_rep_124_3_lut_4_lut.init = 16'h0004;
    LUT4 q_15__I_17_2_lut_rep_133_3_lut_4_lut (.A(s_tmr[2]), .B(n8276), 
         .C(s_tmr[4]), .D(s_tmr[3]), .Z(n8247)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_17_2_lut_rep_133_3_lut_4_lut.init = 16'h0004;
    
endmodule
//
// Verilog Description of module TSALL
// module not written out since it is a black-box. 
//

//
// Verilog Description of module \cram_adaptor(2,12,4) 
//

module \cram_adaptor(2,12,4)  (s_mem_rd_sel, fsmc_clk_c, q_15__N_56, s_reg_dout, 
            fsmc_nce_c, n8850, s_dbg_mem_rd, q_15__N_52, fsmc_d_out_15, 
            s_cram_next_st_1__N_24_c_0, s_mem_dout, s_cram_dout, fsmc_noe_c, 
            n2844, s_ptr_mem_wr_stb, s_aux_mem_wr_stb, s_dbg_mem_wr, 
            n2578, n2577, n2561, n1, n2576, n2560, n1_adj_1, n2575, 
            n2559, n1_adj_2, n2574, n2558, n1_adj_3, n2573, n2557, 
            n1_adj_4, n2572, n2556, n1_adj_5, n2571, n2555, n1_adj_6, 
            n2570, n2554, n1_adj_7, n2569, n2553, n1_adj_8, n2568, 
            n2552, n1_adj_9, n2567, n2551, n1_adj_10, n2566, n2550, 
            n1_adj_11, n2565, n2549, n1_adj_12, n2564, n2548, n1_adj_13, 
            n2563, n2547, n1_adj_14, n2562, n2546, n1_adj_15, \s_cfg[6] , 
            n8288, n1545, n8311, mclk_c, out_get_N_312, n1543, \s_addr0[0] , 
            q_15__N_82, \s_addr1[0] , n8312, s_addr2, s_addr3, fsmc_d_out_0, 
            n20, n22, n1544, \s_addr0[1] , q_15__N_80, q_15__N_78, 
            q_15__N_76, \s_addr1[1] , fsmc_d_out_1, fsmc_d_out_2, fsmc_d_out_3, 
            fsmc_d_out_4, fsmc_d_out_5, fsmc_d_out_6, fsmc_d_out_7, 
            fsmc_d_out_8, fsmc_d_out_9, fsmc_d_out_10, fsmc_d_out_11, 
            fsmc_d_out_12, fsmc_d_out_13, fsmc_d_out_14, n8284, n8177, 
            n7799, n8178, n7793, n8180, n7784, s_reg_din, q_15__N_74, 
            q_15__N_72, q_15__N_70, q_15__N_68, q_15__N_66, q_15__N_64, 
            q_15__N_62, n8852, n8857, n7455, n7487, n7480, n7501, 
            n8856, n7471, n7494, n7823, n7824, n8188, n7835, n7836, 
            n8186, n7829, n7830, n8187, n7786, n7787, n8185, n7801, 
            n7802, n8184, n7804, n7805, n8183, n7820, n7821, n8182, 
            n7376, n7, n8855, n8854, n8853, \s_tmr[6] , \s_r3[6] );
    output [1:0]s_mem_rd_sel;
    input fsmc_clk_c;
    output q_15__N_56;
    input [15:0]s_reg_dout;
    input fsmc_nce_c;
    input n8850;
    output s_dbg_mem_rd;
    output q_15__N_52;
    input fsmc_d_out_15;
    input s_cram_next_st_1__N_24_c_0;
    input [15:0]s_mem_dout;
    output [15:0]s_cram_dout;
    input fsmc_noe_c;
    output n2844;
    output s_ptr_mem_wr_stb;
    output s_aux_mem_wr_stb;
    output s_dbg_mem_wr;
    input n2578;
    input n2577;
    input n2561;
    output n1;
    input n2576;
    input n2560;
    output n1_adj_1;
    input n2575;
    input n2559;
    output n1_adj_2;
    input n2574;
    input n2558;
    output n1_adj_3;
    input n2573;
    input n2557;
    output n1_adj_4;
    input n2572;
    input n2556;
    output n1_adj_5;
    input n2571;
    input n2555;
    output n1_adj_6;
    input n2570;
    input n2554;
    output n1_adj_7;
    input n2569;
    input n2553;
    output n1_adj_8;
    input n2568;
    input n2552;
    output n1_adj_9;
    input n2567;
    input n2551;
    output n1_adj_10;
    input n2566;
    input n2550;
    output n1_adj_11;
    input n2565;
    input n2549;
    output n1_adj_12;
    input n2564;
    input n2548;
    output n1_adj_13;
    input n2563;
    input n2547;
    output n1_adj_14;
    input n2562;
    input n2546;
    output n1_adj_15;
    input \s_cfg[6] ;
    output n8288;
    output n1545;
    output n8311;
    input mclk_c;
    output out_get_N_312;
    output n1543;
    output \s_addr0[0] ;
    output q_15__N_82;
    output \s_addr1[0] ;
    output n8312;
    output [3:0]s_addr2;
    output [3:0]s_addr3;
    input fsmc_d_out_0;
    input n20;
    output n22;
    output n1544;
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
    output n8284;
    input n8177;
    output n7799;
    input n8178;
    output n7793;
    input n8180;
    output n7784;
    output [15:0]s_reg_din;
    output q_15__N_74;
    output q_15__N_72;
    output q_15__N_70;
    output q_15__N_68;
    output q_15__N_66;
    output q_15__N_64;
    output q_15__N_62;
    output n8852;
    output n8857;
    input n7455;
    input n7487;
    input n7480;
    input n7501;
    output n8856;
    input n7471;
    input n7494;
    input n7823;
    input n7824;
    output n8188;
    input n7835;
    input n7836;
    output n8186;
    input n7829;
    input n7830;
    output n8187;
    input n7786;
    input n7787;
    output n8185;
    input n7801;
    input n7802;
    output n8184;
    input n7804;
    input n7805;
    output n8183;
    input n7820;
    input n7821;
    output n8182;
    input n7376;
    output n7;
    output n8855;
    output n8854;
    output n8853;
    input \s_tmr[6] ;
    input \s_r3[6] ;
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(40[2:10])
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    wire s_start_stb, s_cram_adv;
    wire [15:0]s_reg_rd_data;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(116[9:22])
    
    wire s_op_mem_rd, n2425, s_op_mem_rd_N_35;
    wire [3:0]n1501;
    
    wire s_op_mem_wr, s_op_mem_wr_N_42;
    wire [15:0]s_reg_dout_c;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(174[9:19])
    wire [15:0]s_cram_mem_rd_addr;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(104[9:27])
    wire [15:0]s_cram_wr_addr;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(103[9:23])
    
    wire n1508, s_cram_adi, n6532, n8325, n8287, s_fifo_wr_stb;
    
    FD1S3AX s_cram_mem_rd_addr_i1 (.D(q_15__N_56), .CK(fsmc_clk_c), .Q(s_mem_rd_sel[0])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam s_cram_mem_rd_addr_i1.GSR = "ENABLED";
    FD1S3AX s_start_stb_136 (.D(s_cram_adv), .CK(fsmc_clk_c), .Q(s_start_stb)) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(200[3] 204[10])
    defparam s_start_stb_136.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i0 (.D(s_reg_dout[0]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[0])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i0.GSR = "ENABLED";
    FD1P3IX s_op_mem_rd_133 (.D(s_op_mem_rd_N_35), .SP(s_cram_adv), .CD(n2425), 
            .CK(fsmc_clk_c), .Q(s_op_mem_rd));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(185[3] 195[10])
    defparam s_op_mem_rd_133.GSR = "ENABLED";
    FD1S3BX s_cram_st_FSM_i0 (.D(n8850), .CK(fsmc_clk_c), .PD(fsmc_nce_c), 
            .Q(n1501[0]));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(141[3] 154[12])
    defparam s_cram_st_FSM_i0.GSR = "DISABLED";
    FD1S3AX s_reg_rd_data_i15 (.D(s_reg_dout[15]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[15])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i15.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i14 (.D(s_reg_dout[14]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[14])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i14.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i13 (.D(s_reg_dout[13]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[13])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i13.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i12 (.D(s_reg_dout[12]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[12])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i12.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i11 (.D(s_reg_dout[11]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[11])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i11.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i10 (.D(s_reg_dout[10]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[10])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i10.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i9 (.D(s_reg_dout[9]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[9])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i9.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i8 (.D(s_reg_dout[8]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[8])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i8.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i7 (.D(s_reg_dout[7]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[7])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i7.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i6 (.D(s_reg_dout[6]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[6])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i6.GSR = "ENABLED";
    FD1P3IX s_op_mem_wr_132 (.D(s_op_mem_wr_N_42), .SP(s_cram_adv), .CD(n2425), 
            .CK(fsmc_clk_c), .Q(s_op_mem_wr));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(185[3] 195[10])
    defparam s_op_mem_wr_132.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i5 (.D(s_reg_dout[5]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[5])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i5.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i4 (.D(s_reg_dout[4]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[4])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i4.GSR = "ENABLED";
    LUT4 s_start_stb_I_0_140_2_lut (.A(s_start_stb), .B(s_op_mem_rd), .Z(s_dbg_mem_rd)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(290[16:43])
    defparam s_start_stb_I_0_140_2_lut.init = 16'h8888;
    FD1S3AX s_reg_rd_data_i3 (.D(s_reg_dout_c[3]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[3])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i3.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i2 (.D(s_reg_dout_c[2]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[2])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i2.GSR = "ENABLED";
    FD1S3AX s_reg_rd_data_i1 (.D(s_reg_dout_c[1]), .CK(fsmc_clk_c), .Q(s_reg_rd_data[1])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(273[3] 277[10])
    defparam s_reg_rd_data_i1.GSR = "ENABLED";
    FD1S3AX s_cram_mem_rd_addr_i3 (.D(s_cram_wr_addr[15]), .CK(fsmc_clk_c), 
            .Q(s_cram_mem_rd_addr[15])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam s_cram_mem_rd_addr_i3.GSR = "ENABLED";
    FD1S3AX s_cram_mem_rd_addr_i2 (.D(q_15__N_52), .CK(fsmc_clk_c), .Q(s_mem_rd_sel[1])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=32, LSE_LLINE=335, LSE_RLINE=335 */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam s_cram_mem_rd_addr_i2.GSR = "ENABLED";
    LUT4 i804_2_lut (.A(fsmc_d_out_15), .B(s_cram_adv), .Z(n2425)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(185[3] 195[10])
    defparam i804_2_lut.init = 16'h8888;
    LUT4 cram_nwe_I_0_149_2_lut (.A(s_cram_next_st_1__N_24_c_0), .B(fsmc_nce_c), 
         .Z(s_op_mem_rd_N_35)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(192[20:45])
    defparam cram_nwe_I_0_149_2_lut.init = 16'h2222;
    LUT4 s_reg_rd_data_15__I_0_i1_3_lut (.A(s_mem_dout[0]), .B(s_reg_rd_data[0]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[0])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i1_3_lut.init = 16'hcaca;
    LUT4 i1146_2_lut (.A(fsmc_noe_c), .B(fsmc_nce_c), .Z(n2844)) /* synthesis lut_function=(A+(B)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(283[16:37])
    defparam i1146_2_lut.init = 16'heeee;
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
    FD1S3DX s_cram_st_FSM_i1 (.D(n1501[0]), .CK(fsmc_clk_c), .CD(fsmc_nce_c), 
            .Q(s_cram_adv));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(141[3] 154[12])
    defparam s_cram_st_FSM_i1.GSR = "DISABLED";
    FD1S3DX s_cram_st_FSM_i2 (.D(n1508), .CK(fsmc_clk_c), .CD(fsmc_nce_c), 
            .Q(n1501[2]));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(141[3] 154[12])
    defparam s_cram_st_FSM_i2.GSR = "DISABLED";
    FD1S3DX s_cram_st_FSM_i3 (.D(n6532), .CK(fsmc_clk_c), .CD(fsmc_nce_c), 
            .Q(s_cram_adi));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(141[3] 154[12])
    defparam s_cram_st_FSM_i3.GSR = "DISABLED";
    LUT4 i1_2_lut_3_lut_4_lut (.A(s_cram_wr_addr[15]), .B(n8325), .C(q_15__N_56), 
         .D(q_15__N_52), .Z(s_ptr_mem_wr_stb)) /* synthesis lut_function=(!(A+((C+!(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(159[25:69])
    defparam i1_2_lut_3_lut_4_lut.init = 16'h0400;
    LUT4 i1_2_lut_3_lut_4_lut_adj_202 (.A(s_cram_wr_addr[15]), .B(n8325), 
         .C(q_15__N_56), .D(q_15__N_52), .Z(s_aux_mem_wr_stb)) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(159[25:69])
    defparam i1_2_lut_3_lut_4_lut_adj_202.init = 16'h4000;
    LUT4 s_start_stb_I_0_141_2_lut (.A(s_start_stb), .B(s_op_mem_wr), .Z(s_dbg_mem_wr)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(291[16:43])
    defparam s_start_stb_I_0_141_2_lut.init = 16'h8888;
    LUT4 i6080_2_lut (.A(s_cram_next_st_1__N_24_c_0), .B(fsmc_nce_c), .Z(s_op_mem_wr_N_42)) /* synthesis lut_function=(!(A+(B))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(191[20:41])
    defparam i6080_2_lut.init = 16'h1111;
    LUT4 i3186_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2577), 
         .D(n2561), .Z(n1)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3186_4_lut_4_lut.init = 16'h5140;
    LUT4 i3187_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2576), 
         .D(n2560), .Z(n1_adj_1)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3187_4_lut_4_lut.init = 16'h5140;
    LUT4 i3188_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2575), 
         .D(n2559), .Z(n1_adj_2)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3188_4_lut_4_lut.init = 16'h5140;
    LUT4 i3198_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2574), 
         .D(n2558), .Z(n1_adj_3)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3198_4_lut_4_lut.init = 16'h5140;
    LUT4 i3202_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2573), 
         .D(n2557), .Z(n1_adj_4)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3202_4_lut_4_lut.init = 16'h5140;
    LUT4 i3209_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2572), 
         .D(n2556), .Z(n1_adj_5)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3209_4_lut_4_lut.init = 16'h5140;
    LUT4 i3210_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2571), 
         .D(n2555), .Z(n1_adj_6)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3210_4_lut_4_lut.init = 16'h5140;
    LUT4 i3211_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2570), 
         .D(n2554), .Z(n1_adj_7)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3211_4_lut_4_lut.init = 16'h5140;
    LUT4 s_reg_rd_data_15__I_0_i14_3_lut (.A(s_mem_dout[13]), .B(s_reg_rd_data[13]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[13])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i14_3_lut.init = 16'hcaca;
    LUT4 i3212_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2569), 
         .D(n2553), .Z(n1_adj_8)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3212_4_lut_4_lut.init = 16'h5140;
    LUT4 i3213_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2568), 
         .D(n2552), .Z(n1_adj_9)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3213_4_lut_4_lut.init = 16'h5140;
    LUT4 i3217_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2567), 
         .D(n2551), .Z(n1_adj_10)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3217_4_lut_4_lut.init = 16'h5140;
    LUT4 i3220_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2566), 
         .D(n2550), .Z(n1_adj_11)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3220_4_lut_4_lut.init = 16'h5140;
    LUT4 i3221_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2565), 
         .D(n2549), .Z(n1_adj_12)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3221_4_lut_4_lut.init = 16'h5140;
    LUT4 i3224_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2564), 
         .D(n2548), .Z(n1_adj_13)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3224_4_lut_4_lut.init = 16'h5140;
    LUT4 i3230_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2563), 
         .D(n2547), .Z(n1_adj_14)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3230_4_lut_4_lut.init = 16'h5140;
    LUT4 i3305_4_lut_4_lut (.A(s_mem_rd_sel[0]), .B(n2578), .C(n2562), 
         .D(n2546), .Z(n1_adj_15)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(211[3] 215[10])
    defparam i3305_4_lut_4_lut.init = 16'h5140;
    LUT4 s_reg_rd_data_15__I_0_i15_3_lut (.A(s_mem_dout[14]), .B(s_reg_rd_data[14]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[14])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i15_3_lut.init = 16'hcaca;
    LUT4 s_reg_rd_data_15__I_0_i16_3_lut (.A(s_mem_dout[15]), .B(s_reg_rd_data[15]), 
         .C(s_cram_mem_rd_addr[15]), .Z(s_cram_dout[15])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(282[17:70])
    defparam s_reg_rd_data_15__I_0_i16_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_rep_211 (.A(s_cram_adi), .B(s_cram_next_st_1__N_24_c_0), 
         .Z(n8325)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(159[25:69])
    defparam i1_2_lut_rep_211.init = 16'h2222;
    LUT4 i2_3_lut_rep_173_4_lut (.A(s_cram_adi), .B(s_cram_next_st_1__N_24_c_0), 
         .C(\s_cfg[6] ), .D(s_cram_wr_addr[15]), .Z(n8287)) /* synthesis lut_function=(!((B+(C+!(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(159[25:69])
    defparam i2_3_lut_rep_173_4_lut.init = 16'h0200;
    LUT4 i1_2_lut_3_lut (.A(s_cram_adi), .B(s_cram_next_st_1__N_24_c_0), 
         .C(s_cram_wr_addr[15]), .Z(s_fifo_wr_stb)) /* synthesis lut_function=(!((B+!(C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(159[25:69])
    defparam i1_2_lut_3_lut.init = 16'h2020;
    LUT4 i1_2_lut_rep_174_3_lut (.A(s_cram_adi), .B(s_cram_next_st_1__N_24_c_0), 
         .C(s_cram_wr_addr[15]), .Z(n8288)) /* synthesis lut_function=(!((B+(C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(159[25:69])
    defparam i1_2_lut_rep_174_3_lut.init = 16'h0202;
    LUT4 i136_2_lut (.A(s_cram_next_st_1__N_24_c_0), .B(s_cram_adv), .Z(n1508)) /* synthesis lut_function=(!(A+!(B))) */ ;   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(141[3] 154[12])
    defparam i136_2_lut.init = 16'h4444;
    LUT4 i2_4_lut (.A(s_cram_adi), .B(s_cram_next_st_1__N_24_c_0), .C(n1501[2]), 
         .D(s_cram_adv), .Z(n6532)) /* synthesis lut_function=(A+(B (C+(D))+!B (C))) */ ;
    defparam i2_4_lut.init = 16'hfefa;
    \syncfifo(16,4,8)  reg_wr_fifo (.fsmc_clk_c(fsmc_clk_c), .s_fifo_wr_stb(s_fifo_wr_stb), 
            .n1542({Open_3, Open_4, n1545, Open_5}), .n8311(n8311), 
            .mclk_c(mclk_c), .out_get_N_312(out_get_N_312), .n1543(n1543), 
            .\s_addr0[0] (\s_addr0[0] ), .q_15__N_82(q_15__N_82), .\s_addr1[0] (\s_addr1[0] ), 
            .n8312(n8312), .s_addr2({s_addr2}), .s_addr3({s_addr3}), .fsmc_d_out_0(fsmc_d_out_0), 
            .n8287(n8287), .n20(n20), .n22(n22), .n1544(n1544), .\s_addr0[1] (\s_addr0[1] ), 
            .q_15__N_80(q_15__N_80), .q_15__N_78(q_15__N_78), .q_15__N_76(q_15__N_76), 
            .\s_addr1[1] (\s_addr1[1] ), .fsmc_d_out_1(fsmc_d_out_1), .fsmc_d_out_2(fsmc_d_out_2), 
            .fsmc_d_out_3(fsmc_d_out_3), .fsmc_d_out_4(fsmc_d_out_4), .fsmc_d_out_5(fsmc_d_out_5), 
            .fsmc_d_out_6(fsmc_d_out_6), .fsmc_d_out_7(fsmc_d_out_7), .fsmc_d_out_8(fsmc_d_out_8), 
            .fsmc_d_out_9(fsmc_d_out_9), .fsmc_d_out_10(fsmc_d_out_10), 
            .fsmc_d_out_11(fsmc_d_out_11), .fsmc_d_out_12(fsmc_d_out_12), 
            .fsmc_d_out_13(fsmc_d_out_13), .fsmc_d_out_14(fsmc_d_out_14), 
            .fsmc_d_out_15(fsmc_d_out_15), .n8284(n8284), .n8177(n8177), 
            .n7799(n7799), .n8178(n8178), .n7793(n7793), .n8180(n8180), 
            .n7784(n7784), .s_reg_din({s_reg_din}));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(239[16:31])
    \counter(16,16)  addr_r (.q_15__N_80(q_15__N_80), .fsmc_clk_c(fsmc_clk_c), 
            .q_15__N_78(q_15__N_78), .q_15__N_76(q_15__N_76), .q_15__N_74(q_15__N_74), 
            .q_15__N_72(q_15__N_72), .q_15__N_70(q_15__N_70), .q_15__N_68(q_15__N_68), 
            .q_15__N_66(q_15__N_66), .q_15__N_64(q_15__N_64), .q_15__N_62(q_15__N_62), 
            .q_15__N_56(q_15__N_56), .q_15__N_52(q_15__N_52), .\s_cram_wr_addr[15] (s_cram_wr_addr[15]), 
            .q_15__N_82(q_15__N_82), .n8852(n8852), .n8857(n8857), .fsmc_d_out_1(fsmc_d_out_1), 
            .s_cram_adv(s_cram_adv), .fsmc_d_out_2(fsmc_d_out_2), .fsmc_d_out_3(fsmc_d_out_3), 
            .fsmc_d_out_4(fsmc_d_out_4), .fsmc_d_out_5(fsmc_d_out_5), .fsmc_d_out_6(fsmc_d_out_6), 
            .fsmc_d_out_7(fsmc_d_out_7), .fsmc_d_out_8(fsmc_d_out_8), .fsmc_d_out_9(fsmc_d_out_9), 
            .fsmc_d_out_10(fsmc_d_out_10), .fsmc_d_out_11(fsmc_d_out_11), 
            .fsmc_d_out_12(fsmc_d_out_12), .fsmc_d_out_13(fsmc_d_out_13), 
            .fsmc_d_out_14(fsmc_d_out_14), .fsmc_d_out_15(fsmc_d_out_15), 
            .fsmc_d_out_0(fsmc_d_out_0), .s_cram_adi(s_cram_adi), .n7455(n7455), 
            .n7487(n7487), .\s_reg_dout[3] (s_reg_dout_c[3]), .n7480(n7480), 
            .n7501(n7501), .\s_reg_dout[1] (s_reg_dout_c[1]), .n8856(n8856), 
            .n7471(n7471), .n7494(n7494), .\s_reg_dout[2] (s_reg_dout_c[2]), 
            .n7823(n7823), .n7824(n7824), .n8188(n8188), .n7835(n7835), 
            .n7836(n7836), .n8186(n8186), .n7829(n7829), .n7830(n7830), 
            .n8187(n8187), .n7786(n7786), .n7787(n7787), .n8185(n8185), 
            .n7801(n7801), .n7802(n7802), .n8184(n8184), .n7804(n7804), 
            .n7805(n7805), .n8183(n8183), .n7820(n7820), .n7821(n7821), 
            .n8182(n8182), .n7376(n7376), .n7(n7), .n8855(n8855), .n8854(n8854), 
            .n8853(n8853), .\s_tmr[6] (\s_tmr[6] ), .\s_r3[6] (\s_r3[6] ));   // c:/devel/yard-ice/src/fpga/cram_adaptor.vhdl(166[11:25])
    
endmodule
//
// Verilog Description of module \syncfifo(16,4,8) 
//

module \syncfifo(16,4,8)  (fsmc_clk_c, s_fifo_wr_stb, n1542, n8311, 
            mclk_c, out_get_N_312, n1543, \s_addr0[0] , q_15__N_82, 
            \s_addr1[0] , n8312, s_addr2, s_addr3, fsmc_d_out_0, n8287, 
            n20, n22, n1544, \s_addr0[1] , q_15__N_80, q_15__N_78, 
            q_15__N_76, \s_addr1[1] , fsmc_d_out_1, fsmc_d_out_2, fsmc_d_out_3, 
            fsmc_d_out_4, fsmc_d_out_5, fsmc_d_out_6, fsmc_d_out_7, 
            fsmc_d_out_8, fsmc_d_out_9, fsmc_d_out_10, fsmc_d_out_11, 
            fsmc_d_out_12, fsmc_d_out_13, fsmc_d_out_14, fsmc_d_out_15, 
            n8284, n8177, n7799, n8178, n7793, n8180, n7784, s_reg_din);
    input fsmc_clk_c;
    input s_fifo_wr_stb;
    output [3:0]n1542;
    output n8311;
    input mclk_c;
    output out_get_N_312;
    output n1543;
    output \s_addr0[0] ;
    input q_15__N_82;
    output \s_addr1[0] ;
    output n8312;
    output [3:0]s_addr2;
    output [3:0]s_addr3;
    input fsmc_d_out_0;
    input n8287;
    input n20;
    output n22;
    output n1544;
    output \s_addr0[1] ;
    input q_15__N_80;
    input q_15__N_78;
    input q_15__N_76;
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
    input fsmc_d_out_15;
    output n8284;
    input n8177;
    output n7799;
    input n8178;
    output n7793;
    input n8180;
    output n7784;
    output [15:0]s_reg_din;
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(40[2:10])
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    wire [1:0]s_sel_in;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(56[9:17])
    wire [1:0]n13;
    wire [3:0]s_addr0;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(63[9:16])
    wire [3:0]s_addr1;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(64[9:16])
    wire [3:0]n1542_c;
    
    wire fsmc_clk_c_enable_37, fsmc_clk_c_enable_52, fsmc_clk_c_enable_67, 
        fsmc_clk_c_enable_82;
    wire [15:0]s_data0;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(68[9:16])
    wire [15:0]s_data1;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(69[9:16])
    wire [15:0]s_data2;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(70[9:16])
    wire [15:0]s_data3;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(71[9:16])
    wire [1:0]s_syn_sel0;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(58[9:19])
    wire [1:0]s_syn_sel1;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(59[9:19])
    
    wire n3146;
    wire [15:0]n1598;
    
    wire n7307, n3450, n3452, n3454, n3456, n3458, n3460, n3462, 
        n3464, n3466, n3468, n3470, n3472, n3474, n3476, n3478, 
        n2;
    
    FD1P3AX s_sel_in_754__i0 (.D(n13[0]), .SP(s_fifo_wr_stb), .CK(fsmc_clk_c), 
            .Q(s_sel_in[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_sel_in_754__i0.GSR = "ENABLED";
    FD1P3AX s_sel_in_754__i1 (.D(n13[1]), .SP(s_fifo_wr_stb), .CK(fsmc_clk_c), 
            .Q(s_sel_in[1]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_sel_in_754__i1.GSR = "ENABLED";
    LUT4 mux_169_i3_3_lut_rep_197 (.A(s_addr0[2]), .B(s_addr1[2]), .C(n1542[1]), 
         .Z(n8311)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_169_i3_3_lut_rep_197.init = 16'hcaca;
    FD1P3AY s_sel_out_FSM_i0_i0 (.D(n1543), .SP(out_get_N_312), .CK(mclk_c), 
            .Q(n1542_c[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_sel_out_FSM_i0_i0.GSR = "ENABLED";
    FD1P3AX s_addr0_i0_i0 (.D(q_15__N_82), .SP(fsmc_clk_c_enable_37), .CK(fsmc_clk_c), 
            .Q(\s_addr0[0] )) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr0_i0_i0.GSR = "DISABLED";
    FD1P3AX s_addr1_i0_i0 (.D(q_15__N_82), .SP(fsmc_clk_c_enable_52), .CK(fsmc_clk_c), 
            .Q(\s_addr1[0] )) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr1_i0_i0.GSR = "DISABLED";
    LUT4 mux_169_i4_3_lut_rep_198 (.A(s_addr0[3]), .B(s_addr1[3]), .C(n1542[1]), 
         .Z(n8312)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_169_i4_3_lut_rep_198.init = 16'hcaca;
    FD1P3AX s_addr2_i0_i0 (.D(q_15__N_82), .SP(fsmc_clk_c_enable_67), .CK(fsmc_clk_c), 
            .Q(s_addr2[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr2_i0_i0.GSR = "DISABLED";
    FD1P3AX s_addr3_i0_i0 (.D(q_15__N_82), .SP(fsmc_clk_c_enable_82), .CK(fsmc_clk_c), 
            .Q(s_addr3[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr3_i0_i0.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i0 (.D(fsmc_d_out_0), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i0.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i0 (.D(fsmc_d_out_0), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i0.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i0 (.D(fsmc_d_out_0), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i0.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i0 (.D(fsmc_d_out_0), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i0.GSR = "DISABLED";
    FD1S3AX s_syn_sel0_i0 (.D(s_sel_in[0]), .CK(mclk_c), .Q(s_syn_sel0[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(96[3] 102[10])
    defparam s_syn_sel0_i0.GSR = "ENABLED";
    FD1S3AX s_syn_sel1_i0 (.D(s_syn_sel0[0]), .CK(mclk_c), .Q(s_syn_sel1[0])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(96[3] 102[10])
    defparam s_syn_sel1_i0.GSR = "ENABLED";
    LUT4 i1448_3_lut (.A(s_data2[0]), .B(s_data3[0]), .C(n1543), .Z(n3146)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1448_3_lut.init = 16'hcaca;
    LUT4 mux_173_i1_3_lut (.A(s_data0[0]), .B(s_data1[0]), .C(n1542[1]), 
         .Z(n1598[0])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i1_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_3_lut (.A(s_sel_in[0]), .B(n8287), .C(s_sel_in[1]), 
         .Z(fsmc_clk_c_enable_52)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam i1_2_lut_3_lut.init = 16'h0808;
    LUT4 i1_2_lut_3_lut_adj_201 (.A(s_sel_in[0]), .B(n8287), .C(s_sel_in[1]), 
         .Z(fsmc_clk_c_enable_82)) /* synthesis lut_function=(A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam i1_2_lut_3_lut_adj_201.init = 16'h8080;
    LUT4 i6065_2_lut_3_lut (.A(s_sel_in[0]), .B(n8287), .C(s_sel_in[1]), 
         .Z(fsmc_clk_c_enable_37)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam i6065_2_lut_3_lut.init = 16'h0404;
    LUT4 i1_2_lut_4_lut (.A(s_addr0[3]), .B(s_addr1[3]), .C(n1542[1]), 
         .D(n20), .Z(n22)) /* synthesis lut_function=(!(A (B+!(C (D)))+!A (B (C+!(D))+!B !(D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1_2_lut_4_lut.init = 16'h3500;
    LUT4 i6062_2_lut_3_lut (.A(s_sel_in[0]), .B(n8287), .C(s_sel_in[1]), 
         .Z(fsmc_clk_c_enable_67)) /* synthesis lut_function=(!(A+!(B (C)))) */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam i6062_2_lut_3_lut.init = 16'h4040;
    LUT4 i6066_2_lut (.A(n1543), .B(n1544), .Z(n7307)) /* synthesis lut_function=(A+(B)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i6066_2_lut.init = 16'heeee;
    LUT4 i4845_1_lut (.A(s_sel_in[0]), .Z(n13[0])) /* synthesis lut_function=(!(A)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i4845_1_lut.init = 16'h5555;
    LUT4 i1752_3_lut (.A(s_data2[1]), .B(s_data3[1]), .C(n1543), .Z(n3450)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1752_3_lut.init = 16'hcaca;
    LUT4 mux_173_i2_3_lut (.A(s_data0[1]), .B(s_data1[1]), .C(n1542[1]), 
         .Z(n1598[1])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i2_3_lut.init = 16'hcaca;
    LUT4 i1754_3_lut (.A(s_data2[2]), .B(s_data3[2]), .C(n1543), .Z(n3452)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1754_3_lut.init = 16'hcaca;
    LUT4 mux_173_i3_3_lut (.A(s_data0[2]), .B(s_data1[2]), .C(n1542[1]), 
         .Z(n1598[2])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i3_3_lut.init = 16'hcaca;
    LUT4 i1756_3_lut (.A(s_data2[3]), .B(s_data3[3]), .C(n1543), .Z(n3454)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1756_3_lut.init = 16'hcaca;
    LUT4 mux_173_i4_3_lut (.A(s_data0[3]), .B(s_data1[3]), .C(n1542[1]), 
         .Z(n1598[3])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i4_3_lut.init = 16'hcaca;
    LUT4 i1758_3_lut (.A(s_data2[4]), .B(s_data3[4]), .C(n1543), .Z(n3456)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1758_3_lut.init = 16'hcaca;
    LUT4 mux_173_i5_3_lut (.A(s_data0[4]), .B(s_data1[4]), .C(n1542[1]), 
         .Z(n1598[4])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i5_3_lut.init = 16'hcaca;
    LUT4 i1760_3_lut (.A(s_data2[5]), .B(s_data3[5]), .C(n1543), .Z(n3458)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1760_3_lut.init = 16'hcaca;
    LUT4 mux_173_i6_3_lut (.A(s_data0[5]), .B(s_data1[5]), .C(n1542[1]), 
         .Z(n1598[5])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i6_3_lut.init = 16'hcaca;
    LUT4 i1762_3_lut (.A(s_data2[6]), .B(s_data3[6]), .C(n1543), .Z(n3460)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1762_3_lut.init = 16'hcaca;
    LUT4 mux_173_i7_3_lut (.A(s_data0[6]), .B(s_data1[6]), .C(n1542[1]), 
         .Z(n1598[6])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i7_3_lut.init = 16'hcaca;
    LUT4 i1764_3_lut (.A(s_data2[7]), .B(s_data3[7]), .C(n1543), .Z(n3462)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1764_3_lut.init = 16'hcaca;
    LUT4 mux_173_i8_3_lut (.A(s_data0[7]), .B(s_data1[7]), .C(n1542[1]), 
         .Z(n1598[7])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i8_3_lut.init = 16'hcaca;
    LUT4 i1766_3_lut (.A(s_data2[8]), .B(s_data3[8]), .C(n1543), .Z(n3464)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1766_3_lut.init = 16'hcaca;
    LUT4 mux_173_i9_3_lut (.A(s_data0[8]), .B(s_data1[8]), .C(n1542[1]), 
         .Z(n1598[8])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i9_3_lut.init = 16'hcaca;
    LUT4 i1768_3_lut (.A(s_data2[9]), .B(s_data3[9]), .C(n1543), .Z(n3466)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1768_3_lut.init = 16'hcaca;
    LUT4 mux_173_i10_3_lut (.A(s_data0[9]), .B(s_data1[9]), .C(n1542[1]), 
         .Z(n1598[9])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i10_3_lut.init = 16'hcaca;
    LUT4 i1770_3_lut (.A(s_data2[10]), .B(s_data3[10]), .C(n1543), .Z(n3468)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1770_3_lut.init = 16'hcaca;
    LUT4 mux_173_i11_3_lut (.A(s_data0[10]), .B(s_data1[10]), .C(n1542[1]), 
         .Z(n1598[10])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i11_3_lut.init = 16'hcaca;
    LUT4 i1772_3_lut (.A(s_data2[11]), .B(s_data3[11]), .C(n1543), .Z(n3470)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1772_3_lut.init = 16'hcaca;
    LUT4 mux_173_i12_3_lut (.A(s_data0[11]), .B(s_data1[11]), .C(n1542[1]), 
         .Z(n1598[11])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i12_3_lut.init = 16'hcaca;
    LUT4 i1774_3_lut (.A(s_data2[12]), .B(s_data3[12]), .C(n1543), .Z(n3472)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1774_3_lut.init = 16'hcaca;
    LUT4 mux_173_i13_3_lut (.A(s_data0[12]), .B(s_data1[12]), .C(n1542[1]), 
         .Z(n1598[12])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i13_3_lut.init = 16'hcaca;
    LUT4 i1776_3_lut (.A(s_data2[13]), .B(s_data3[13]), .C(n1543), .Z(n3474)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1776_3_lut.init = 16'hcaca;
    LUT4 mux_173_i14_3_lut (.A(s_data0[13]), .B(s_data1[13]), .C(n1542[1]), 
         .Z(n1598[13])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i14_3_lut.init = 16'hcaca;
    LUT4 i1778_3_lut (.A(s_data2[14]), .B(s_data3[14]), .C(n1543), .Z(n3476)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1778_3_lut.init = 16'hcaca;
    LUT4 mux_173_i15_3_lut (.A(s_data0[14]), .B(s_data1[14]), .C(n1542[1]), 
         .Z(n1598[14])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i15_3_lut.init = 16'hcaca;
    LUT4 i1780_3_lut (.A(s_data2[15]), .B(s_data3[15]), .C(n1543), .Z(n3478)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1780_3_lut.init = 16'hcaca;
    LUT4 mux_173_i16_3_lut (.A(s_data0[15]), .B(s_data1[15]), .C(n1542[1]), 
         .Z(n1598[15])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam mux_173_i16_3_lut.init = 16'hcaca;
    LUT4 i4847_2_lut (.A(s_sel_in[1]), .B(s_sel_in[0]), .Z(n13[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i4847_2_lut.init = 16'h6666;
    FD1P3AX s_sel_out_FSM_i0_i1 (.D(n1542_c[0]), .SP(out_get_N_312), .CK(mclk_c), 
            .Q(n1542[1]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_sel_out_FSM_i0_i1.GSR = "ENABLED";
    FD1P3AX s_sel_out_FSM_i0_i2 (.D(n1542[1]), .SP(out_get_N_312), .CK(mclk_c), 
            .Q(n1544));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_sel_out_FSM_i0_i2.GSR = "ENABLED";
    FD1P3AX s_sel_out_FSM_i0_i3 (.D(n1544), .SP(out_get_N_312), .CK(mclk_c), 
            .Q(n1543));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_sel_out_FSM_i0_i3.GSR = "ENABLED";
    FD1P3AX s_addr0_i0_i1 (.D(q_15__N_80), .SP(fsmc_clk_c_enable_37), .CK(fsmc_clk_c), 
            .Q(\s_addr0[1] )) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr0_i0_i1.GSR = "DISABLED";
    FD1P3AX s_addr0_i0_i2 (.D(q_15__N_78), .SP(fsmc_clk_c_enable_37), .CK(fsmc_clk_c), 
            .Q(s_addr0[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr0_i0_i2.GSR = "DISABLED";
    FD1P3AX s_addr0_i0_i3 (.D(q_15__N_76), .SP(fsmc_clk_c_enable_37), .CK(fsmc_clk_c), 
            .Q(s_addr0[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr0_i0_i3.GSR = "DISABLED";
    FD1P3AX s_addr1_i0_i1 (.D(q_15__N_80), .SP(fsmc_clk_c_enable_52), .CK(fsmc_clk_c), 
            .Q(\s_addr1[1] )) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr1_i0_i1.GSR = "DISABLED";
    FD1P3AX s_addr1_i0_i2 (.D(q_15__N_78), .SP(fsmc_clk_c_enable_52), .CK(fsmc_clk_c), 
            .Q(s_addr1[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr1_i0_i2.GSR = "DISABLED";
    FD1P3AX s_addr1_i0_i3 (.D(q_15__N_76), .SP(fsmc_clk_c_enable_52), .CK(fsmc_clk_c), 
            .Q(s_addr1[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr1_i0_i3.GSR = "DISABLED";
    FD1P3AX s_addr2_i0_i1 (.D(q_15__N_80), .SP(fsmc_clk_c_enable_67), .CK(fsmc_clk_c), 
            .Q(s_addr2[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr2_i0_i1.GSR = "DISABLED";
    FD1P3AX s_addr2_i0_i2 (.D(q_15__N_78), .SP(fsmc_clk_c_enable_67), .CK(fsmc_clk_c), 
            .Q(s_addr2[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr2_i0_i2.GSR = "DISABLED";
    FD1P3AX s_addr2_i0_i3 (.D(q_15__N_76), .SP(fsmc_clk_c_enable_67), .CK(fsmc_clk_c), 
            .Q(s_addr2[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr2_i0_i3.GSR = "DISABLED";
    FD1P3AX s_addr3_i0_i1 (.D(q_15__N_80), .SP(fsmc_clk_c_enable_82), .CK(fsmc_clk_c), 
            .Q(s_addr3[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr3_i0_i1.GSR = "DISABLED";
    FD1P3AX s_addr3_i0_i2 (.D(q_15__N_78), .SP(fsmc_clk_c_enable_82), .CK(fsmc_clk_c), 
            .Q(s_addr3[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr3_i0_i2.GSR = "DISABLED";
    FD1P3AX s_addr3_i0_i3 (.D(q_15__N_76), .SP(fsmc_clk_c_enable_82), .CK(fsmc_clk_c), 
            .Q(s_addr3[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_addr3_i0_i3.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i1 (.D(fsmc_d_out_1), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i1.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i2 (.D(fsmc_d_out_2), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i2.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i3 (.D(fsmc_d_out_3), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i3.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i4 (.D(fsmc_d_out_4), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[4])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i4.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i5 (.D(fsmc_d_out_5), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[5])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i5.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i6 (.D(fsmc_d_out_6), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[6])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i6.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i7 (.D(fsmc_d_out_7), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[7])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i7.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i8 (.D(fsmc_d_out_8), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[8])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i8.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i9 (.D(fsmc_d_out_9), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[9])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i9.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i10 (.D(fsmc_d_out_10), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[10])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i10.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i11 (.D(fsmc_d_out_11), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[11])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i11.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i12 (.D(fsmc_d_out_12), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[12])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i12.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i13 (.D(fsmc_d_out_13), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[13])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i13.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i14 (.D(fsmc_d_out_14), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[14])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i14.GSR = "DISABLED";
    FD1P3AX s_data0_i0_i15 (.D(fsmc_d_out_15), .SP(fsmc_clk_c_enable_37), 
            .CK(fsmc_clk_c), .Q(s_data0[15])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data0_i0_i15.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i1 (.D(fsmc_d_out_1), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i1.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i2 (.D(fsmc_d_out_2), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i2.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i3 (.D(fsmc_d_out_3), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i3.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i4 (.D(fsmc_d_out_4), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[4])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i4.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i5 (.D(fsmc_d_out_5), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[5])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i5.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i6 (.D(fsmc_d_out_6), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[6])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i6.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i7 (.D(fsmc_d_out_7), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[7])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i7.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i8 (.D(fsmc_d_out_8), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[8])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i8.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i9 (.D(fsmc_d_out_9), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[9])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i9.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i10 (.D(fsmc_d_out_10), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[10])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i10.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i11 (.D(fsmc_d_out_11), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[11])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i11.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i12 (.D(fsmc_d_out_12), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[12])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i12.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i13 (.D(fsmc_d_out_13), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[13])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i13.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i14 (.D(fsmc_d_out_14), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[14])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i14.GSR = "DISABLED";
    FD1P3AX s_data1_i0_i15 (.D(fsmc_d_out_15), .SP(fsmc_clk_c_enable_52), 
            .CK(fsmc_clk_c), .Q(s_data1[15])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data1_i0_i15.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i1 (.D(fsmc_d_out_1), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i1.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i2 (.D(fsmc_d_out_2), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i2.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i3 (.D(fsmc_d_out_3), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i3.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i4 (.D(fsmc_d_out_4), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[4])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i4.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i5 (.D(fsmc_d_out_5), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[5])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i5.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i6 (.D(fsmc_d_out_6), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[6])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i6.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i7 (.D(fsmc_d_out_7), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[7])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i7.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i8 (.D(fsmc_d_out_8), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[8])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i8.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i9 (.D(fsmc_d_out_9), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[9])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i9.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i10 (.D(fsmc_d_out_10), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[10])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i10.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i11 (.D(fsmc_d_out_11), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[11])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i11.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i12 (.D(fsmc_d_out_12), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[12])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i12.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i13 (.D(fsmc_d_out_13), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[13])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i13.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i14 (.D(fsmc_d_out_14), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[14])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i14.GSR = "DISABLED";
    FD1P3AX s_data2_i0_i15 (.D(fsmc_d_out_15), .SP(fsmc_clk_c_enable_67), 
            .CK(fsmc_clk_c), .Q(s_data2[15])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data2_i0_i15.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i1 (.D(fsmc_d_out_1), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i1.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i2 (.D(fsmc_d_out_2), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[2])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i2.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i3 (.D(fsmc_d_out_3), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[3])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i3.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i4 (.D(fsmc_d_out_4), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[4])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i4.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i5 (.D(fsmc_d_out_5), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[5])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i5.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i6 (.D(fsmc_d_out_6), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[6])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i6.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i7 (.D(fsmc_d_out_7), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[7])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i7.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i8 (.D(fsmc_d_out_8), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[8])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i8.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i9 (.D(fsmc_d_out_9), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[9])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i9.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i10 (.D(fsmc_d_out_10), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[10])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i10.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i11 (.D(fsmc_d_out_11), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[11])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i11.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i12 (.D(fsmc_d_out_12), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[12])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i12.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i13 (.D(fsmc_d_out_13), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[13])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i13.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i14 (.D(fsmc_d_out_14), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[14])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i14.GSR = "DISABLED";
    FD1P3AX s_data3_i0_i15 (.D(fsmc_d_out_15), .SP(fsmc_clk_c_enable_82), 
            .CK(fsmc_clk_c), .Q(s_data3[15])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(75[3] 91[10])
    defparam s_data3_i0_i15.GSR = "DISABLED";
    FD1S3AX s_syn_sel0_i1 (.D(s_sel_in[1]), .CK(mclk_c), .Q(s_syn_sel0[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(96[3] 102[10])
    defparam s_syn_sel0_i1.GSR = "ENABLED";
    FD1S3AX s_syn_sel1_i1 (.D(s_syn_sel0[1]), .CK(mclk_c), .Q(s_syn_sel1[1])) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=239, LSE_RLINE=239 */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(96[3] 102[10])
    defparam s_syn_sel1_i1.GSR = "ENABLED";
    LUT4 i1784_3_lut_rep_170 (.A(n8311), .B(s_addr2[2]), .C(n1544), .Z(n8284)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1784_3_lut_rep_170.init = 16'hcaca;
    LUT4 n344_bdd_2_lut_6224_4_lut (.A(n8311), .B(s_addr2[2]), .C(n1544), 
         .D(n8177), .Z(n7799)) /* synthesis lut_function=(A (B (D)+!B !(C+!(D)))+!A (B (C (D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam n344_bdd_2_lut_6224_4_lut.init = 16'hca00;
    LUT4 n344_bdd_2_lut_6210_4_lut (.A(n8311), .B(s_addr2[2]), .C(n1544), 
         .D(n8178), .Z(n7793)) /* synthesis lut_function=(A (B (D)+!B !(C+!(D)))+!A (B (C (D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam n344_bdd_2_lut_6210_4_lut.init = 16'hca00;
    LUT4 n344_bdd_2_lut_6204_4_lut (.A(n8311), .B(s_addr2[2]), .C(n1544), 
         .D(n8180), .Z(n7784)) /* synthesis lut_function=(A (B (D)+!B !(C+!(D)))+!A (B (C (D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam n344_bdd_2_lut_6204_4_lut.init = 16'hca00;
    LUT4 s_syn_sel1_1__I_0_i3_4_lut (.A(s_syn_sel1[0]), .B(n2), .C(n1542[1]), 
         .D(n1543), .Z(out_get_N_312)) /* synthesis lut_function=(A (B+!(C+(D)))+!A (B+(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(105[26:49])
    defparam s_syn_sel1_1__I_0_i3_4_lut.init = 16'hddde;
    LUT4 s_syn_sel1_1__I_0_i2_3_lut (.A(s_syn_sel1[1]), .B(n1544), .C(n1543), 
         .Z(n2)) /* synthesis lut_function=(!(A (B+(C))+!A !(B+(C)))) */ ;   // c:/devel/yard-ice/src/fpga/syncfifo.vhdl(105[26:49])
    defparam s_syn_sel1_1__I_0_i2_3_lut.init = 16'h5656;
    PFUMX i1781 (.BLUT(n1598[15]), .ALUT(n3478), .C0(n7307), .Z(s_reg_din[15]));
    PFUMX i1779 (.BLUT(n1598[14]), .ALUT(n3476), .C0(n7307), .Z(s_reg_din[14]));
    PFUMX i1777 (.BLUT(n1598[13]), .ALUT(n3474), .C0(n7307), .Z(s_reg_din[13]));
    PFUMX i1775 (.BLUT(n1598[12]), .ALUT(n3472), .C0(n7307), .Z(s_reg_din[12]));
    PFUMX i1773 (.BLUT(n1598[11]), .ALUT(n3470), .C0(n7307), .Z(s_reg_din[11]));
    PFUMX i1771 (.BLUT(n1598[10]), .ALUT(n3468), .C0(n7307), .Z(s_reg_din[10]));
    PFUMX i1769 (.BLUT(n1598[9]), .ALUT(n3466), .C0(n7307), .Z(s_reg_din[9]));
    PFUMX i1767 (.BLUT(n1598[8]), .ALUT(n3464), .C0(n7307), .Z(s_reg_din[8]));
    PFUMX i1765 (.BLUT(n1598[7]), .ALUT(n3462), .C0(n7307), .Z(s_reg_din[7]));
    PFUMX i1763 (.BLUT(n1598[6]), .ALUT(n3460), .C0(n7307), .Z(s_reg_din[6]));
    PFUMX i1761 (.BLUT(n1598[5]), .ALUT(n3458), .C0(n7307), .Z(s_reg_din[5]));
    PFUMX i1759 (.BLUT(n1598[4]), .ALUT(n3456), .C0(n7307), .Z(s_reg_din[4]));
    PFUMX i1757 (.BLUT(n1598[3]), .ALUT(n3454), .C0(n7307), .Z(s_reg_din[3]));
    PFUMX i1755 (.BLUT(n1598[2]), .ALUT(n3452), .C0(n7307), .Z(s_reg_din[2]));
    PFUMX i1753 (.BLUT(n1598[1]), .ALUT(n3450), .C0(n7307), .Z(s_reg_din[1]));
    PFUMX i1449 (.BLUT(n1598[0]), .ALUT(n3146), .C0(n7307), .Z(s_reg_din[0]));
    
endmodule
//
// Verilog Description of module \counter(16,16) 
//

module \counter(16,16)  (q_15__N_80, fsmc_clk_c, q_15__N_78, q_15__N_76, 
            q_15__N_74, q_15__N_72, q_15__N_70, q_15__N_68, q_15__N_66, 
            q_15__N_64, q_15__N_62, q_15__N_56, q_15__N_52, \s_cram_wr_addr[15] , 
            q_15__N_82, n8852, n8857, fsmc_d_out_1, s_cram_adv, fsmc_d_out_2, 
            fsmc_d_out_3, fsmc_d_out_4, fsmc_d_out_5, fsmc_d_out_6, 
            fsmc_d_out_7, fsmc_d_out_8, fsmc_d_out_9, fsmc_d_out_10, 
            fsmc_d_out_11, fsmc_d_out_12, fsmc_d_out_13, fsmc_d_out_14, 
            fsmc_d_out_15, fsmc_d_out_0, s_cram_adi, n7455, n7487, 
            \s_reg_dout[3] , n7480, n7501, \s_reg_dout[1] , n8856, 
            n7471, n7494, \s_reg_dout[2] , n7823, n7824, n8188, 
            n7835, n7836, n8186, n7829, n7830, n8187, n7786, n7787, 
            n8185, n7801, n7802, n8184, n7804, n7805, n8183, n7820, 
            n7821, n8182, n7376, n7, n8855, n8854, n8853, \s_tmr[6] , 
            \s_r3[6] );
    output q_15__N_80;
    input fsmc_clk_c;
    output q_15__N_78;
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
    output n8852;
    output n8857;
    input fsmc_d_out_1;
    input s_cram_adv;
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
    input fsmc_d_out_0;
    input s_cram_adi;
    input n7455;
    input n7487;
    output \s_reg_dout[3] ;
    input n7480;
    input n7501;
    output \s_reg_dout[1] ;
    output n8856;
    input n7471;
    input n7494;
    output \s_reg_dout[2] ;
    input n7823;
    input n7824;
    output n8188;
    input n7835;
    input n7836;
    output n8186;
    input n7829;
    input n7830;
    output n8187;
    input n7786;
    input n7787;
    output n8185;
    input n7801;
    input n7802;
    output n8184;
    input n7804;
    input n7805;
    output n8183;
    input n7820;
    input n7821;
    output n8182;
    input n7376;
    output n7;
    output n8855;
    output n8854;
    output n8853;
    input \s_tmr[6] ;
    input \s_r3[6] ;
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(40[2:10])
    
    wire q_1__N_137, q_2__N_133, q_3__N_129, q_4__N_125, q_5__N_121, 
        q_6__N_117, q_7__N_113, q_8__N_109, q_9__N_105, q_10__N_101, 
        q_15__N_60, q_11__N_97, q_15__N_58, q_12__N_93, q_13__N_89, 
        q_14__N_85, q_15__N_49, q_0__N_141, n8285, n8251, n8218, 
        n8204, n8208, q_15__N_51, n8223, n8231, n8213, n8313, 
        n8270, n8259, n8244, n8236, n8200, n8339, n8338, n8337;
    
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
    FD1S3AX s_count_reg_0__180_rep_223 (.D(q_0__N_141), .CK(fsmc_clk_c), 
            .Q(n8852)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_0__180_rep_223.GSR = "ENABLED";
    LUT4 q_15__I_17_2_lut_rep_137_3_lut_4_lut (.A(q_15__N_78), .B(n8285), 
         .C(q_15__N_74), .D(q_15__N_76), .Z(n8251)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_17_2_lut_rep_137_3_lut_4_lut.init = 16'h8000;
    FD1S3AX s_count_reg_1__181_rep_228 (.D(q_1__N_137), .CK(fsmc_clk_c), 
            .Q(n8857)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_1__181_rep_228.GSR = "ENABLED";
    LUT4 q_15__I_9_2_lut_rep_90_3_lut_4_lut (.A(q_15__N_62), .B(n8218), 
         .C(q_15__N_58), .D(q_15__N_60), .Z(n8204)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_9_2_lut_rep_90_3_lut_4_lut.init = 16'h8000;
    LUT4 q_15__I_7_2_lut_3_lut_4_lut (.A(q_15__N_58), .B(n8208), .C(q_15__N_52), 
         .D(q_15__N_56), .Z(q_15__N_51)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_7_2_lut_3_lut_4_lut.init = 16'h8000;
    LUT4 q_15__I_10_2_lut_rep_94_3_lut_4_lut (.A(q_15__N_64), .B(n8223), 
         .C(q_15__N_60), .D(q_15__N_62), .Z(n8208)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_10_2_lut_rep_94_3_lut_4_lut.init = 16'h8000;
    LUT4 q_15__I_11_2_lut_rep_99_3_lut_4_lut (.A(q_15__N_66), .B(n8231), 
         .C(q_15__N_62), .D(q_15__N_64), .Z(n8213)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_11_2_lut_rep_99_3_lut_4_lut.init = 16'h8000;
    LUT4 q_1__I_49_4_lut (.A(q_15__N_80), .B(fsmc_d_out_1), .C(s_cram_adv), 
         .D(n8313), .Z(q_1__N_137)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_1__I_49_4_lut.init = 16'hc5ca;
    LUT4 q_2__I_47_4_lut (.A(q_15__N_78), .B(fsmc_d_out_2), .C(s_cram_adv), 
         .D(n8285), .Z(q_2__N_133)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_2__I_47_4_lut.init = 16'hc5ca;
    LUT4 q_3__I_45_4_lut (.A(q_15__N_76), .B(fsmc_d_out_3), .C(s_cram_adv), 
         .D(n8270), .Z(q_3__N_129)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_3__I_45_4_lut.init = 16'hc5ca;
    LUT4 q_4__I_43_4_lut (.A(q_15__N_74), .B(fsmc_d_out_4), .C(s_cram_adv), 
         .D(n8259), .Z(q_4__N_125)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_4__I_43_4_lut.init = 16'hc5ca;
    LUT4 q_5__I_41_4_lut (.A(q_15__N_72), .B(fsmc_d_out_5), .C(s_cram_adv), 
         .D(n8251), .Z(q_5__N_121)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_5__I_41_4_lut.init = 16'hc5ca;
    LUT4 q_6__I_39_4_lut (.A(q_15__N_70), .B(fsmc_d_out_6), .C(s_cram_adv), 
         .D(n8244), .Z(q_6__N_117)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_6__I_39_4_lut.init = 16'hc5ca;
    LUT4 q_7__I_37_4_lut (.A(q_15__N_68), .B(fsmc_d_out_7), .C(s_cram_adv), 
         .D(n8236), .Z(q_7__N_113)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_7__I_37_4_lut.init = 16'hc5ca;
    LUT4 q_15__I_18_2_lut_rep_145_3_lut_4_lut (.A(q_15__N_80), .B(n8313), 
         .C(q_15__N_76), .D(q_15__N_78), .Z(n8259)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_18_2_lut_rep_145_3_lut_4_lut.init = 16'h8000;
    LUT4 q_8__I_35_4_lut (.A(q_15__N_66), .B(fsmc_d_out_8), .C(s_cram_adv), 
         .D(n8231), .Z(q_8__N_109)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_8__I_35_4_lut.init = 16'hc5ca;
    LUT4 q_15__I_12_2_lut_rep_104_3_lut_4_lut (.A(q_15__N_68), .B(n8236), 
         .C(q_15__N_64), .D(q_15__N_66), .Z(n8218)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_12_2_lut_rep_104_3_lut_4_lut.init = 16'h8000;
    LUT4 q_9__I_33_4_lut (.A(q_15__N_64), .B(fsmc_d_out_9), .C(s_cram_adv), 
         .D(n8223), .Z(q_9__N_105)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_9__I_33_4_lut.init = 16'hc5ca;
    LUT4 q_10__I_31_4_lut (.A(q_15__N_62), .B(fsmc_d_out_10), .C(s_cram_adv), 
         .D(n8218), .Z(q_10__N_101)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_10__I_31_4_lut.init = 16'hc5ca;
    LUT4 q_11__I_29_4_lut (.A(q_15__N_60), .B(fsmc_d_out_11), .C(s_cram_adv), 
         .D(n8213), .Z(q_11__N_97)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_11__I_29_4_lut.init = 16'hc5ca;
    LUT4 q_12__I_27_4_lut (.A(q_15__N_58), .B(fsmc_d_out_12), .C(s_cram_adv), 
         .D(n8208), .Z(q_12__N_93)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_12__I_27_4_lut.init = 16'hc5ca;
    LUT4 q_13__I_25_4_lut (.A(q_15__N_56), .B(fsmc_d_out_13), .C(s_cram_adv), 
         .D(n8204), .Z(q_13__N_89)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_13__I_25_4_lut.init = 16'hc5ca;
    LUT4 q_14__I_23_4_lut (.A(q_15__N_52), .B(fsmc_d_out_14), .C(s_cram_adv), 
         .D(n8200), .Z(q_14__N_85)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_14__I_23_4_lut.init = 16'hc5ca;
    LUT4 q_15__I_6_4_lut (.A(\s_cram_wr_addr[15] ), .B(fsmc_d_out_15), .C(s_cram_adv), 
         .D(q_15__N_51), .Z(q_15__N_49)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_15__I_6_4_lut.init = 16'hc5ca;
    LUT4 q_0__I_51_4_lut (.A(q_15__N_82), .B(fsmc_d_out_0), .C(s_cram_adv), 
         .D(s_cram_adi), .Z(q_0__N_141)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(85[6] 91[13])
    defparam q_0__I_51_4_lut.init = 16'hc5ca;
    LUT4 q_15__I_13_2_lut_rep_109_3_lut_4_lut (.A(q_15__N_70), .B(n8244), 
         .C(q_15__N_66), .D(q_15__N_68), .Z(n8223)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_13_2_lut_rep_109_3_lut_4_lut.init = 16'h8000;
    LUT4 q_15__I_14_2_lut_rep_117_3_lut_4_lut (.A(q_15__N_72), .B(n8251), 
         .C(q_15__N_68), .D(q_15__N_70), .Z(n8231)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_14_2_lut_rep_117_3_lut_4_lut.init = 16'h8000;
    LUT4 s_reg_rd_sel_3__I_0_Mux_3_i15_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_76), 
         .C(n7455), .D(n7487), .Z(\s_reg_dout[3] )) /* synthesis lut_function=(!(A (B+!(D))+!A !(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_reg_rd_sel_3__I_0_Mux_3_i15_4_lut_4_lut.init = 16'h7340;
    LUT4 s_reg_rd_sel_3__I_0_Mux_1_i15_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_76), 
         .C(n7480), .D(n7501), .Z(\s_reg_dout[1] )) /* synthesis lut_function=(!(A (B+!(D))+!A !(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_reg_rd_sel_3__I_0_Mux_1_i15_4_lut_4_lut.init = 16'h7340;
    FD1S3AX s_count_reg_1__181_rep_227 (.D(q_1__N_137), .CK(fsmc_clk_c), 
            .Q(n8856)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_1__181_rep_227.GSR = "ENABLED";
    LUT4 s_reg_rd_sel_3__I_0_Mux_2_i15_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_76), 
         .C(n7471), .D(n7494), .Z(\s_reg_dout[2] )) /* synthesis lut_function=(!(A (B+!(D))+!A !(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_reg_rd_sel_3__I_0_Mux_2_i15_4_lut_4_lut.init = 16'h7340;
    LUT4 n7824_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n7823), 
         .D(n7824), .Z(n8188)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n7824_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 n7836_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n7835), 
         .D(n7836), .Z(n8186)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n7836_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 n7830_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n7829), 
         .D(n7830), .Z(n8187)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n7830_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 n7787_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n7786), 
         .D(n7787), .Z(n8185)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n7787_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 n7802_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n7801), 
         .D(n7802), .Z(n8184)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n7802_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 n7805_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n7804), 
         .D(n7805), .Z(n8183)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n7805_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 n7821_bdd_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_80), .C(n7820), 
         .D(n7821), .Z(n8182)) /* synthesis lut_function=(!(A+!(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam n7821_bdd_4_lut_4_lut.init = 16'h5140;
    LUT4 s_reg_rd_sel_3__I_0_Mux_6_i7_4_lut_4_lut (.A(q_15__N_78), .B(q_15__N_76), 
         .C(n7376), .D(n8339), .Z(n7)) /* synthesis lut_function=(!(A (B+!(D))+!A !(B (C)+!B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_reg_rd_sel_3__I_0_Mux_6_i7_4_lut_4_lut.init = 16'h7340;
    FD1S3AX s_count_reg_1__181_rep_226 (.D(q_1__N_137), .CK(fsmc_clk_c), 
            .Q(n8855)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_1__181_rep_226.GSR = "ENABLED";
    LUT4 q_15__I_21_2_lut_rep_199 (.A(q_15__N_82), .B(s_cram_adi), .Z(n8313)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_21_2_lut_rep_199.init = 16'h8888;
    LUT4 q_15__I_20_2_lut_rep_171_3_lut (.A(q_15__N_82), .B(s_cram_adi), 
         .C(q_15__N_80), .Z(n8285)) /* synthesis lut_function=(A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_20_2_lut_rep_171_3_lut.init = 16'h8080;
    LUT4 q_15__I_19_2_lut_rep_156_3_lut_4_lut (.A(q_15__N_82), .B(s_cram_adi), 
         .C(q_15__N_78), .D(q_15__N_80), .Z(n8270)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_19_2_lut_rep_156_3_lut_4_lut.init = 16'h8000;
    FD1S3AX s_count_reg_1__181_rep_225 (.D(q_1__N_137), .CK(fsmc_clk_c), 
            .Q(n8854)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_1__181_rep_225.GSR = "ENABLED";
    FD1S3AX s_count_reg_0__180_rep_224 (.D(q_0__N_141), .CK(fsmc_clk_c), 
            .Q(n8853)) /* synthesis LSE_LINE_FILE_ID=21, LSE_LCOL=11, LSE_RCOL=25, LSE_LLINE=166, LSE_RLINE=166 */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_count_reg_0__180_rep_224.GSR = "ENABLED";
    LUT4 q_15__I_8_2_lut_rep_86_3_lut_4_lut (.A(q_15__N_60), .B(n8213), 
         .C(q_15__N_56), .D(q_15__N_58), .Z(n8200)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_8_2_lut_rep_86_3_lut_4_lut.init = 16'h8000;
    LUT4 q_15__I_15_2_lut_rep_122_3_lut_4_lut (.A(q_15__N_74), .B(n8259), 
         .C(q_15__N_70), .D(q_15__N_72), .Z(n8236)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_15_2_lut_rep_122_3_lut_4_lut.init = 16'h8000;
    LUT4 q_15__I_16_2_lut_rep_130_3_lut_4_lut (.A(q_15__N_76), .B(n8270), 
         .C(q_15__N_72), .D(q_15__N_74), .Z(n8244)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(94[10:44])
    defparam q_15__I_16_2_lut_rep_130_3_lut_4_lut.init = 16'h8000;
    LUT4 s_r3_6__bdd_4_lut_4_lut_then_2_lut (.A(q_15__N_82), .B(\s_tmr[6] ), 
         .Z(n8338)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_r3_6__bdd_4_lut_4_lut_then_2_lut.init = 16'h8888;
    LUT4 s_r3_6__bdd_4_lut_4_lut_else_2_lut (.A(q_15__N_82), .B(q_15__N_80), 
         .C(\s_r3[6] ), .Z(n8337)) /* synthesis lut_function=(A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/counter.vhdl(81[4] 93[11])
    defparam s_r3_6__bdd_4_lut_4_lut_else_2_lut.init = 16'h8080;
    PFUMX i6498 (.BLUT(n8337), .ALUT(n8338), .C0(q_15__N_78), .Z(n8339));
    
endmodule
//
// Verilog Description of module \jtag_reg(16,16)_U8 
//

module \jtag_reg(16,16)_U8  (s_cfg, mclk_c, s_cfg_wr_stb, s_reg_din, 
            n4048, mclk_c_enable_91);
    output [15:0]s_cfg;
    input mclk_c;
    input s_cfg_wr_stb;
    input [15:0]s_reg_din;
    input n4048;
    output mclk_c_enable_91;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    FD1P3AX s_reg_i0 (.D(s_reg_din[0]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[0])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0.GSR = "ENABLED";
    FD1P3AX s_reg_i15 (.D(s_reg_din[15]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[15])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i15.GSR = "ENABLED";
    FD1P3AX s_reg_i14 (.D(s_reg_din[14]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[14])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i14.GSR = "ENABLED";
    FD1P3AX s_reg_i13 (.D(s_reg_din[13]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[13])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i13.GSR = "ENABLED";
    FD1P3AX s_reg_i12 (.D(s_reg_din[12]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[12])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i12.GSR = "ENABLED";
    FD1P3AX s_reg_i11 (.D(s_reg_din[11]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[11])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i11.GSR = "ENABLED";
    FD1P3AX s_reg_i10 (.D(s_reg_din[10]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[10])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i10.GSR = "ENABLED";
    FD1P3AX s_reg_i9 (.D(s_reg_din[9]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[9])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i9.GSR = "ENABLED";
    FD1P3AX s_reg_i8 (.D(s_reg_din[8]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[8])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i8.GSR = "ENABLED";
    FD1P3AX s_reg_i7 (.D(s_reg_din[7]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[7])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i7.GSR = "ENABLED";
    FD1P3AX s_reg_i5 (.D(s_reg_din[5]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[5])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i5.GSR = "ENABLED";
    FD1P3AX s_reg_i4 (.D(s_reg_din[4]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[4])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i4.GSR = "ENABLED";
    FD1P3AX s_reg_i3 (.D(s_reg_din[3]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[3])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i3.GSR = "ENABLED";
    FD1P3AX s_reg_i2 (.D(s_reg_din[2]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[2])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i2.GSR = "ENABLED";
    FD1P3AX s_reg_i1 (.D(s_reg_din[1]), .SP(s_cfg_wr_stb), .CK(mclk_c), 
            .Q(s_cfg[1])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i1.GSR = "ENABLED";
    FD1S3IX s_reg_i6 (.D(s_reg_din[6]), .CK(mclk_c), .CD(n4048), .Q(s_cfg[6])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=10, LSE_RCOL=25, LSE_LLINE=551, LSE_RLINE=551 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i6.GSR = "ENABLED";
    LUT4 i277_1_lut (.A(s_cfg[6]), .Z(mclk_c_enable_91)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam i277_1_lut.init = 16'h5555;
    
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
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    FD1P3AX s_reg_i0_i0 (.D(s_reg_din[0]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[0])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i0.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i1 (.D(s_reg_din[1]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[1])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i1.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i2 (.D(s_reg_din[2]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[2])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i2.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i3 (.D(s_reg_din[3]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[3])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i3.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i4 (.D(s_reg_din[4]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[4])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i4.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i5 (.D(s_reg_din[5]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[5])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i5.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i6 (.D(s_reg_din[6]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[6])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i6.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i7 (.D(s_reg_din[7]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[7])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i7.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i8 (.D(s_reg_din[8]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[8])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i8.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i9 (.D(s_reg_din[9]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[9])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i9.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i10 (.D(s_reg_din[10]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[10])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i10.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i11 (.D(s_reg_din[11]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[11])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i11.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i12 (.D(s_reg_din[12]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[12])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i12.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i13 (.D(s_reg_din[13]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[13])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i13.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i14 (.D(s_reg_din[14]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[14])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i14.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i15 (.D(s_reg_din[15]), .SP(s_ckgen_div_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_div[15])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=31, LSE_LLINE=693, LSE_RLINE=693 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i15.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module \jtag_desc_ram(16,9,32,8) 
//

module \jtag_desc_ram(16,9,32,8)  (fsmc_clk_c, mclk_c, s_we_hi, VCC_net, 
            GND_net, n8856, s_desc_addr, q_15__N_78, q_15__N_76, q_15__N_74, 
            q_15__N_72, q_15__N_70, q_15__N_68, q_15__N_66, fsmc_d_out_0, 
            fsmc_d_out_1, fsmc_d_out_2, fsmc_d_out_3, fsmc_d_out_4, 
            fsmc_d_out_5, fsmc_d_out_6, fsmc_d_out_7, fsmc_d_out_8, 
            fsmc_d_out_9, fsmc_d_out_10, fsmc_d_out_11, fsmc_d_out_12, 
            fsmc_d_out_13, fsmc_d_out_14, fsmc_d_out_15, \s_desc_data[16] , 
            \s_desc_data[17] , \s_desc_data[18] , \s_desc_data[19] , \s_desc_data[20] , 
            \s_desc_data[21] , \s_desc_data[22] , \s_desc_data[23] , \s_desc_data[24] , 
            \s_desc_data[25] , \s_desc_data[26] , \s_desc_data[27] , \s_desc_data[28] , 
            \s_desc_data[29] , \s_desc_data[30] , \s_desc_data[31] , s_we_lo, 
            n8854, s_pos_0_3__N_670, \s_desc_data[4] , \s_desc_data[5] , 
            \s_desc_data[6] , \s_desc_data[7] , \s_desc_data[8] , \s_desc_data[9] , 
            \s_desc_data[10] , \s_desc_data[11] , \s_desc_data[12] , \s_desc_data[13] , 
            \s_desc_data[14] , \s_desc_data[15] );
    input fsmc_clk_c;
    input mclk_c;
    input s_we_hi;
    input VCC_net;
    input GND_net;
    input n8856;
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
    input n8854;
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
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(40[2:10])
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    PDPW8KC n25380 (.DI0(fsmc_d_out_0), .DI1(fsmc_d_out_1), .DI2(fsmc_d_out_2), 
            .DI3(fsmc_d_out_3), .DI4(fsmc_d_out_4), .DI5(fsmc_d_out_5), 
            .DI6(fsmc_d_out_6), .DI7(fsmc_d_out_7), .DI8(fsmc_d_out_8), 
            .DI9(fsmc_d_out_9), .DI10(fsmc_d_out_10), .DI11(fsmc_d_out_11), 
            .DI12(fsmc_d_out_12), .DI13(fsmc_d_out_13), .DI14(fsmc_d_out_14), 
            .DI15(fsmc_d_out_15), .DI16(GND_net), .DI17(GND_net), .ADW0(n8856), 
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
    defparam n25380.DATA_WIDTH_W = 18;
    defparam n25380.DATA_WIDTH_R = 18;
    defparam n25380.REGMODE = "NOREG";
    defparam n25380.CSDECODE_W = "0b000";
    defparam n25380.CSDECODE_R = "0b000";
    defparam n25380.GSR = "DISABLED";
    defparam n25380.RESETMODE = "SYNC";
    defparam n25380.ASYNC_RESET_RELEASE = "SYNC";
    defparam n25380.INIT_DATA = "STATIC";
    defparam n25380.INITVAL_00 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_01 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_02 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_03 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_04 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_05 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_06 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_07 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_08 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_09 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_0A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_0B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_0C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_0D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_0E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_0F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_10 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_11 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_12 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_13 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_14 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_15 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_16 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_17 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_18 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_19 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_1A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_1B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_1C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_1D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_1E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25380.INITVAL_1F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    PDPW8KC n25370 (.DI0(fsmc_d_out_0), .DI1(fsmc_d_out_1), .DI2(fsmc_d_out_2), 
            .DI3(fsmc_d_out_3), .DI4(fsmc_d_out_4), .DI5(fsmc_d_out_5), 
            .DI6(fsmc_d_out_6), .DI7(fsmc_d_out_7), .DI8(fsmc_d_out_8), 
            .DI9(fsmc_d_out_9), .DI10(fsmc_d_out_10), .DI11(fsmc_d_out_11), 
            .DI12(fsmc_d_out_12), .DI13(fsmc_d_out_13), .DI14(fsmc_d_out_14), 
            .DI15(fsmc_d_out_15), .DI16(GND_net), .DI17(GND_net), .ADW0(n8854), 
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
    defparam n25370.DATA_WIDTH_W = 18;
    defparam n25370.DATA_WIDTH_R = 18;
    defparam n25370.REGMODE = "NOREG";
    defparam n25370.CSDECODE_W = "0b000";
    defparam n25370.CSDECODE_R = "0b000";
    defparam n25370.GSR = "DISABLED";
    defparam n25370.RESETMODE = "SYNC";
    defparam n25370.ASYNC_RESET_RELEASE = "SYNC";
    defparam n25370.INIT_DATA = "STATIC";
    defparam n25370.INITVAL_00 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_01 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_02 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_03 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_04 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_05 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_06 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_07 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_08 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_09 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_0A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_0B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_0C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_0D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_0E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_0F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_10 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_11 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_12 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_13 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_14 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_15 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_16 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_17 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_18 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_19 = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_1A = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_1B = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_1C = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_1D = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_1E = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    defparam n25370.INITVAL_1F = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000";
    
endmodule
//
// Verilog Description of module \jtag_reg(16,16) 
//

module \jtag_reg(16,16)  (s_r3, mclk_c, s_r3_wr_stb, s_reg_din);
    output [15:0]s_r3;
    input mclk_c;
    input s_r3_wr_stb;
    input [15:0]s_reg_din;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    FD1P3AX s_reg_i0_i0 (.D(s_reg_din[0]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[0])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i0.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i1 (.D(s_reg_din[1]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[1])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i1.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i2 (.D(s_reg_din[2]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[2])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i2.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i3 (.D(s_reg_din[3]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[3])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i3.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i4 (.D(s_reg_din[4]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[4])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i4.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i5 (.D(s_reg_din[5]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[5])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i5.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i6 (.D(s_reg_din[6]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[6])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i6.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i7 (.D(s_reg_din[7]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[7])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i7.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i8 (.D(s_reg_din[8]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[8])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i8.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i9 (.D(s_reg_din[9]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[9])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i9.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i10 (.D(s_reg_din[10]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[10])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i10.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i11 (.D(s_reg_din[11]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[11])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i11.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i12 (.D(s_reg_din[12]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[12])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i12.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i13 (.D(s_reg_din[13]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[13])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i13.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i14 (.D(s_reg_din[14]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[14])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i14.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i15 (.D(s_reg_din[15]), .SP(s_r3_wr_stb), .CK(mclk_c), 
            .Q(s_r3[15])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=7, LSE_RCOL=22, LSE_LLINE=615, LSE_RLINE=615 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i15.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module \jtag_clk_scaler(1000) 
//

module \jtag_clk_scaler(1000)  (GND_net, s_1khz_stb, mclk_c, s_1mhz_stb);
    input GND_net;
    output s_1khz_stb;
    input mclk_c;
    input s_1mhz_stb;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    wire n6451;
    wire [9:0]s_count;   // c:/devel/yard-ice/src/fpga/jtag_clk_scaler.vhdl(54[9:16])
    wire [9:0]n35;
    
    wire n6452, p_stb_N_1558, n6453, n15, n20, n18, n4050, n6449, 
        n6450, n8, n7169, n7199;
    
    CCU2D s_count_753_add_4_7 (.A0(s_count[5]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[6]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6451), .COUT(n6452), .S0(n35[5]), .S1(n35[6]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753_add_4_7.INIT0 = 16'hfaaa;
    defparam s_count_753_add_4_7.INIT1 = 16'hfaaa;
    defparam s_count_753_add_4_7.INJECT1_0 = "NO";
    defparam s_count_753_add_4_7.INJECT1_1 = "NO";
    FD1S3AX p_stb_26 (.D(p_stb_N_1558), .CK(mclk_c), .Q(s_1khz_stb)) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=38, LSE_LLINE=910, LSE_RLINE=910 */ ;   // c:/devel/yard-ice/src/fpga/jtag_clk_scaler.vhdl(87[3] 93[10])
    defparam p_stb_26.GSR = "ENABLED";
    CCU2D s_count_753_add_4_11 (.A0(s_count[9]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n6453), .S0(n35[9]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753_add_4_11.INIT0 = 16'hfaaa;
    defparam s_count_753_add_4_11.INIT1 = 16'h0000;
    defparam s_count_753_add_4_11.INJECT1_0 = "NO";
    defparam s_count_753_add_4_11.INJECT1_1 = "NO";
    CCU2D s_count_753_add_4_9 (.A0(s_count[7]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[8]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6452), .COUT(n6453), .S0(n35[7]), .S1(n35[8]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753_add_4_9.INIT0 = 16'hfaaa;
    defparam s_count_753_add_4_9.INIT1 = 16'hfaaa;
    defparam s_count_753_add_4_9.INJECT1_0 = "NO";
    defparam s_count_753_add_4_9.INJECT1_1 = "NO";
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
    FD1P3IX s_count_753__i1 (.D(n35[1]), .SP(s_1mhz_stb), .CD(n4050), 
            .CK(mclk_c), .Q(s_count[1])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i1.GSR = "ENABLED";
    FD1P3IX s_count_753__i2 (.D(n35[2]), .SP(s_1mhz_stb), .CD(n4050), 
            .CK(mclk_c), .Q(s_count[2])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i2.GSR = "ENABLED";
    FD1P3IX s_count_753__i3 (.D(n35[3]), .SP(s_1mhz_stb), .CD(n4050), 
            .CK(mclk_c), .Q(s_count[3])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i3.GSR = "ENABLED";
    FD1P3IX s_count_753__i4 (.D(n35[4]), .SP(s_1mhz_stb), .CD(n4050), 
            .CK(mclk_c), .Q(s_count[4])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i4.GSR = "ENABLED";
    FD1P3IX s_count_753__i5 (.D(n35[5]), .SP(s_1mhz_stb), .CD(n4050), 
            .CK(mclk_c), .Q(s_count[5])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i5.GSR = "ENABLED";
    FD1P3IX s_count_753__i6 (.D(n35[6]), .SP(s_1mhz_stb), .CD(n4050), 
            .CK(mclk_c), .Q(s_count[6])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i6.GSR = "ENABLED";
    FD1P3IX s_count_753__i7 (.D(n35[7]), .SP(s_1mhz_stb), .CD(n4050), 
            .CK(mclk_c), .Q(s_count[7])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i7.GSR = "ENABLED";
    FD1P3IX s_count_753__i8 (.D(n35[8]), .SP(s_1mhz_stb), .CD(n4050), 
            .CK(mclk_c), .Q(s_count[8])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i8.GSR = "ENABLED";
    FD1P3IX s_count_753__i9 (.D(n35[9]), .SP(s_1mhz_stb), .CD(n4050), 
            .CK(mclk_c), .Q(s_count[9])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i9.GSR = "ENABLED";
    CCU2D s_count_753_add_4_3 (.A0(s_count[1]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[2]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6449), .COUT(n6450), .S0(n35[1]), .S1(n35[2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753_add_4_3.INIT0 = 16'hfaaa;
    defparam s_count_753_add_4_3.INIT1 = 16'hfaaa;
    defparam s_count_753_add_4_3.INJECT1_0 = "NO";
    defparam s_count_753_add_4_3.INJECT1_1 = "NO";
    CCU2D s_count_753_add_4_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[0]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .COUT(n6449), .S1(n35[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753_add_4_1.INIT0 = 16'hF000;
    defparam s_count_753_add_4_1.INIT1 = 16'h0555;
    defparam s_count_753_add_4_1.INJECT1_0 = "NO";
    defparam s_count_753_add_4_1.INJECT1_1 = "NO";
    CCU2D s_count_753_add_4_5 (.A0(s_count[3]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[4]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6450), .COUT(n6451), .S0(n35[3]), .S1(n35[4]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753_add_4_5.INIT0 = 16'hfaaa;
    defparam s_count_753_add_4_5.INIT1 = 16'hfaaa;
    defparam s_count_753_add_4_5.INJECT1_0 = "NO";
    defparam s_count_753_add_4_5.INJECT1_1 = "NO";
    FD1P3IX s_count_753__i0 (.D(n35[0]), .SP(s_1mhz_stb), .CD(n4050), 
            .CK(mclk_c), .Q(s_count[0])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_753__i0.GSR = "ENABLED";
    LUT4 i2351_4_lut (.A(s_1mhz_stb), .B(s_count[2]), .C(n8), .D(s_count[3]), 
         .Z(n4050)) /* synthesis lut_function=(!(((C+(D))+!B)+!A)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i2351_4_lut.init = 16'h0008;
    LUT4 i3_4_lut (.A(n7169), .B(s_count[4]), .C(n7199), .D(s_count[9]), 
         .Z(n8)) /* synthesis lut_function=((B+!(C (D)))+!A) */ ;
    defparam i3_4_lut.init = 16'hdfff;
    LUT4 i5511_2_lut (.A(s_count[5]), .B(s_count[7]), .Z(n7169)) /* synthesis lut_function=(A (B)) */ ;
    defparam i5511_2_lut.init = 16'h8888;
    LUT4 i5539_4_lut (.A(s_count[6]), .B(s_count[8]), .C(s_count[0]), 
         .D(s_count[1]), .Z(n7199)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i5539_4_lut.init = 16'h8000;
    
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
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(40[2:10])
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
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
            n8852, n8855, q_15__N_78, q_15__N_76, q_15__N_74, q_15__N_72, 
            q_15__N_70, q_15__N_68, fsmc_d_out_0, fsmc_d_out_1, fsmc_d_out_2, 
            fsmc_d_out_3, fsmc_d_out_4, fsmc_d_out_5, fsmc_d_out_6, 
            fsmc_d_out_7, fsmc_d_out_8, fsmc_d_out_9, fsmc_d_out_10, 
            fsmc_d_out_11, fsmc_d_out_12, fsmc_d_out_13, fsmc_d_out_14, 
            fsmc_d_out_15, s_aux_mem_dout);
    input fsmc_clk_c;
    input s_aux_mem_wr_stb;
    input VCC_net;
    input GND_net;
    input n8852;
    input n8855;
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
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(40[2:10])
    
    DP8KC mem0 (.DIA0(fsmc_d_out_0), .DIA1(fsmc_d_out_1), .DIA2(fsmc_d_out_2), 
          .DIA3(fsmc_d_out_3), .DIA4(fsmc_d_out_4), .DIA5(fsmc_d_out_5), 
          .DIA6(fsmc_d_out_6), .DIA7(fsmc_d_out_7), .DIA8(fsmc_d_out_8), 
          .ADA0(VCC_net), .ADA1(VCC_net), .ADA2(GND_net), .ADA3(n8852), 
          .ADA4(n8855), .ADA5(q_15__N_78), .ADA6(q_15__N_76), .ADA7(q_15__N_74), 
          .ADA8(q_15__N_72), .ADA9(q_15__N_70), .ADA10(q_15__N_68), .ADA11(GND_net), 
          .ADA12(GND_net), .CEA(VCC_net), .OCEA(VCC_net), .CLKA(fsmc_clk_c), 
          .WEA(s_aux_mem_wr_stb), .CSA0(GND_net), .CSA1(GND_net), .CSA2(GND_net), 
          .RSTA(GND_net), .DIB0(fsmc_d_out_9), .DIB1(fsmc_d_out_10), .DIB2(fsmc_d_out_11), 
          .DIB3(fsmc_d_out_12), .DIB4(fsmc_d_out_13), .DIB5(fsmc_d_out_14), 
          .DIB6(fsmc_d_out_15), .DIB7(GND_net), .DIB8(GND_net), .ADB0(VCC_net), 
          .ADB1(VCC_net), .ADB2(GND_net), .ADB3(n8852), .ADB4(n8855), 
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

module \jtag_led_drv(31,false,false,false)_U3  (mclk_c, led_2_c, n8288, 
            s_1khz_stb);
    input mclk_c;
    output led_2_c;
    input n8288;
    input s_1khz_stb;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    wire [4:0]s_count;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(59[9:16])
    
    wire mclk_c_enable_29, s_cnt_ld, n3681, n3683, n3687, n3124, 
        n3685, s_trip_r1, s_trip_r0, n4026, n6, n3627, n8320, 
        n8321;
    
    FD1P3JX s_count_i0_i4 (.D(n3681), .SP(mclk_c_enable_29), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[4])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=948, LSE_RLINE=948 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i4.GSR = "ENABLED";
    FD1P3JX s_count_i0_i3 (.D(n3683), .SP(mclk_c_enable_29), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[3])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=948, LSE_RLINE=948 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i3.GSR = "ENABLED";
    FD1P3JX s_count_i0_i2 (.D(n3687), .SP(mclk_c_enable_29), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[2])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=948, LSE_RLINE=948 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i2.GSR = "ENABLED";
    FD1S3JX s_count_i0_i0 (.D(n3124), .CK(mclk_c), .PD(s_cnt_ld), .Q(s_count[0])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=948, LSE_RLINE=948 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i0.GSR = "ENABLED";
    FD1P3JX s_count_i0_i1 (.D(n3685), .SP(mclk_c_enable_29), .PD(s_cnt_ld), 
            .CK(mclk_c), .Q(s_count[1])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=948, LSE_RLINE=948 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(129[3] 137[10])
    defparam s_count_i0_i1.GSR = "ENABLED";
    FD1S3AX s_trip_r1_31 (.D(s_trip_r0), .CK(mclk_c), .Q(s_trip_r1)) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=948, LSE_RLINE=948 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(95[3] 105[10])
    defparam s_trip_r1_31.GSR = "ENABLED";
    FD1S3AX s_drv_ff_32 (.D(n4026), .CK(mclk_c), .Q(led_2_c)) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=948, LSE_RLINE=948 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(113[3] 121[10])
    defparam s_drv_ff_32.GSR = "ENABLED";
    FD1S3AX s_trip_r0_30 (.D(n8288), .CK(mclk_c), .Q(s_trip_r0)) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=35, LSE_LLINE=948, LSE_RLINE=948 */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(95[3] 105[10])
    defparam s_trip_r0_30.GSR = "ENABLED";
    LUT4 i4_4_lut (.A(s_count[3]), .B(s_count[2]), .C(s_count[0]), .D(n6), 
         .Z(n3627)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(140[22:45])
    defparam i4_4_lut.init = 16'hfffe;
    LUT4 i1_2_lut (.A(s_count[1]), .B(s_count[4]), .Z(n6)) /* synthesis lut_function=(A+(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(140[22:45])
    defparam i1_2_lut.init = 16'heeee;
    LUT4 i1_2_lut_adj_200 (.A(s_count[1]), .B(s_count[0]), .Z(n3685)) /* synthesis lut_function=(A (B)+!A !(B)) */ ;
    defparam i1_2_lut_adj_200.init = 16'h9999;
    LUT4 i1_3_lut_4_lut (.A(s_count[2]), .B(n8320), .C(s_count[3]), .D(s_count[4]), 
         .Z(n3681)) /* synthesis lut_function=(A (D)+!A (B (D)+!B (C (D)+!C !(D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i1_3_lut_4_lut.init = 16'hfe01;
    LUT4 i2_3_lut_4_lut (.A(s_1khz_stb), .B(led_2_c), .C(n3627), .D(n8321), 
         .Z(mclk_c_enable_29)) /* synthesis lut_function=(A (B+((D)+!C))+!A ((D)+!C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(134[10:41])
    defparam i2_3_lut_4_lut.init = 16'hff8f;
    LUT4 i1809_2_lut_3_lut (.A(s_1khz_stb), .B(led_2_c), .C(s_count[0]), 
         .Z(n3124)) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(134[10:41])
    defparam i1809_2_lut_3_lut.init = 16'h7878;
    LUT4 i989_2_lut_rep_206 (.A(s_count[1]), .B(s_count[0]), .Z(n8320)) /* synthesis lut_function=(A+(B)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i989_2_lut_rep_206.init = 16'heeee;
    LUT4 i1_2_lut_3_lut (.A(s_count[1]), .B(s_count[0]), .C(s_count[2]), 
         .Z(n3687)) /* synthesis lut_function=(A (C)+!A (B (C)+!B !(C))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i1_2_lut_3_lut.init = 16'he1e1;
    LUT4 i1_2_lut_3_lut_4_lut (.A(s_count[1]), .B(s_count[0]), .C(s_count[3]), 
         .D(s_count[2]), .Z(n3683)) /* synthesis lut_function=(A (C)+!A (B (C)+!B (C (D)+!C !(D)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1308[12:13])
    defparam i1_2_lut_3_lut_4_lut.init = 16'hf0e1;
    LUT4 s_trip_r0_I_0_2_lut_rep_207 (.A(s_trip_r0), .B(s_trip_r1), .Z(n8321)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(108[15:42])
    defparam s_trip_r0_I_0_2_lut_rep_207.init = 16'h2222;
    LUT4 z_I_0_2_lut_3_lut (.A(s_trip_r0), .B(s_trip_r1), .C(n3627), .Z(s_cnt_ld)) /* synthesis lut_function=(!(A (B (C))+!A (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(108[15:42])
    defparam z_I_0_2_lut_3_lut.init = 16'h2f2f;
    LUT4 i2320_3_lut_4_lut (.A(s_trip_r0), .B(s_trip_r1), .C(led_2_c), 
         .D(n3627), .Z(n4026)) /* synthesis lut_function=(A ((C (D))+!B)+!A (C (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_led_drv.vhdl(108[15:42])
    defparam i2320_3_lut_4_lut.init = 16'hf222;
    
endmodule
//
// Verilog Description of module PUR
// module not written out since it is a black-box. 
//

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
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    FD1P3AX s_reg__i1 (.D(\s_reg_din[0] ), .SP(s_irq_en_wr_stb), .CK(mclk_c), 
            .Q(\s_irq_en[0] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=28, LSE_LLINE=533, LSE_RLINE=533 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i1.GSR = "ENABLED";
    FD1P3AX s_reg__i2 (.D(\s_reg_din[1] ), .SP(s_irq_en_wr_stb), .CK(mclk_c), 
            .Q(\s_irq_en[1] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=28, LSE_LLINE=533, LSE_RLINE=533 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i2.GSR = "ENABLED";
    FD1P3AX s_reg__i3 (.D(\s_reg_din[2] ), .SP(s_irq_en_wr_stb), .CK(mclk_c), 
            .Q(\s_irq_en[2] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=28, LSE_LLINE=533, LSE_RLINE=533 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i3.GSR = "ENABLED";
    FD1P3AX s_reg__i4 (.D(\s_reg_din[3] ), .SP(s_irq_en_wr_stb), .CK(mclk_c), 
            .Q(\s_irq_en[3] )) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=13, LSE_RCOL=28, LSE_LLINE=533, LSE_RLINE=533 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg__i4.GSR = "ENABLED";
    
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
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    FD1P3AX s_reg_i0_i0 (.D(s_reg_din[0]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[0])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i0.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i1 (.D(s_reg_din[1]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[1])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i1.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i2 (.D(s_reg_din[2]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[2])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i2.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i3 (.D(s_reg_din[3]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[3])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i3.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i4 (.D(s_reg_din[4]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[4])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i4.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i5 (.D(s_reg_din[5]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[5])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i5.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i6 (.D(s_reg_din[6]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[6])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i6.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i7 (.D(s_reg_din[7]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[7])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i7.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i8 (.D(s_reg_din[8]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[8])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i8.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i9 (.D(s_reg_din[9]), .SP(s_ckgen_rtdiv_wr_stb), .CK(mclk_c), 
            .Q(s_ckgen_rtdiv[9])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i9.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i10 (.D(s_reg_din[10]), .SP(s_ckgen_rtdiv_wr_stb), 
            .CK(mclk_c), .Q(s_ckgen_rtdiv[10])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i10.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i11 (.D(s_reg_din[11]), .SP(s_ckgen_rtdiv_wr_stb), 
            .CK(mclk_c), .Q(s_ckgen_rtdiv[11])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i11.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i12 (.D(s_reg_din[12]), .SP(s_ckgen_rtdiv_wr_stb), 
            .CK(mclk_c), .Q(s_ckgen_rtdiv[12])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i12.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i13 (.D(s_reg_din[13]), .SP(s_ckgen_rtdiv_wr_stb), 
            .CK(mclk_c), .Q(s_ckgen_rtdiv[13])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i13.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i14 (.D(s_reg_din[14]), .SP(s_ckgen_rtdiv_wr_stb), 
            .CK(mclk_c), .Q(s_ckgen_rtdiv[14])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i14.GSR = "ENABLED";
    FD1P3AX s_reg_i0_i15 (.D(s_reg_din[15]), .SP(s_ckgen_rtdiv_wr_stb), 
            .CK(mclk_c), .Q(s_ckgen_rtdiv[15])) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=18, LSE_RCOL=33, LSE_LLINE=710, LSE_RLINE=710 */ ;   // c:/devel/yard-ice/src/fpga/jtag_reg.vhdl(79[3] 98[10])
    defparam s_reg_i0_i15.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module \jtag_clk_scaler(60) 
//

module \jtag_clk_scaler(60)  (mclk_c, s_1mhz_stb, GND_net);
    input mclk_c;
    output s_1mhz_stb;
    input GND_net;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    wire [5:0]s_count;   // c:/devel/yard-ice/src/fpga/jtag_clk_scaler.vhdl(54[9:16])
    
    wire n4033;
    wire [5:0]n23;
    
    wire s_hi, n6527, n6446, n6448, n6447;
    
    FD1S3IX s_count_752__i5 (.D(n23[5]), .CK(mclk_c), .CD(n4033), .Q(s_count[5])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_752__i5.GSR = "ENABLED";
    FD1S3AX p_stb_26 (.D(s_hi), .CK(mclk_c), .Q(s_1mhz_stb)) /* synthesis LSE_LINE_FILE_ID=39, LSE_LCOL=16, LSE_RCOL=38, LSE_LLINE=898, LSE_RLINE=898 */ ;   // c:/devel/yard-ice/src/fpga/jtag_clk_scaler.vhdl(87[3] 93[10])
    defparam p_stb_26.GSR = "ENABLED";
    FD1S3IX s_count_752__i4 (.D(n23[4]), .CK(mclk_c), .CD(n4033), .Q(s_count[4])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_752__i4.GSR = "ENABLED";
    FD1S3IX s_count_752__i3 (.D(n23[3]), .CK(mclk_c), .CD(n4033), .Q(s_count[3])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_752__i3.GSR = "ENABLED";
    FD1S3IX s_count_752__i2 (.D(n23[2]), .CK(mclk_c), .CD(n4033), .Q(s_count[2])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_752__i2.GSR = "ENABLED";
    FD1S3IX s_count_752__i1 (.D(n23[1]), .CK(mclk_c), .CD(n4033), .Q(s_count[1])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_752__i1.GSR = "ENABLED";
    FD1S3IX s_count_752__i0 (.D(n23[0]), .CK(mclk_c), .CD(n4033), .Q(s_count[0])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_752__i0.GSR = "ENABLED";
    LUT4 i6099_4_lut (.A(s_count[1]), .B(n6527), .C(s_count[5]), .D(s_count[2]), 
         .Z(n4033)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i6099_4_lut.init = 16'h0080;
    LUT4 i2_3_lut (.A(s_count[3]), .B(s_count[4]), .C(s_count[0]), .Z(n6527)) /* synthesis lut_function=(A (B (C))) */ ;
    defparam i2_3_lut.init = 16'h8080;
    LUT4 i6069_4_lut (.A(s_count[1]), .B(n6527), .C(s_count[2]), .D(s_count[5]), 
         .Z(s_hi)) /* synthesis lut_function=(!(A+(((D)+!C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_clk_scaler.vhdl(62[23:45])
    defparam i6069_4_lut.init = 16'h0040;
    CCU2D s_count_752_add_4_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[0]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .COUT(n6446), .S1(n23[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_752_add_4_1.INIT0 = 16'hF000;
    defparam s_count_752_add_4_1.INIT1 = 16'h0555;
    defparam s_count_752_add_4_1.INJECT1_0 = "NO";
    defparam s_count_752_add_4_1.INJECT1_1 = "NO";
    CCU2D s_count_752_add_4_7 (.A0(s_count[5]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n6448), .S0(n23[5]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_752_add_4_7.INIT0 = 16'hfaaa;
    defparam s_count_752_add_4_7.INIT1 = 16'h0000;
    defparam s_count_752_add_4_7.INJECT1_0 = "NO";
    defparam s_count_752_add_4_7.INJECT1_1 = "NO";
    CCU2D s_count_752_add_4_5 (.A0(s_count[3]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[4]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6447), .COUT(n6448), .S0(n23[3]), .S1(n23[4]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_752_add_4_5.INIT0 = 16'hfaaa;
    defparam s_count_752_add_4_5.INIT1 = 16'hfaaa;
    defparam s_count_752_add_4_5.INJECT1_0 = "NO";
    defparam s_count_752_add_4_5.INJECT1_1 = "NO";
    CCU2D s_count_752_add_4_3 (.A0(s_count[1]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_count[2]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6446), .COUT(n6447), .S0(n23[1]), .S1(n23[2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_count_752_add_4_3.INIT0 = 16'hfaaa;
    defparam s_count_752_add_4_3.INIT1 = 16'hfaaa;
    defparam s_count_752_add_4_3.INJECT1_0 = "NO";
    defparam s_count_752_add_4_3.INJECT1_1 = "NO";
    
endmodule
//
// Verilog Description of module \jtag_enh_shifter(16,10,10,16,8,32,8,16,16) 
//

module \jtag_enh_shifter(16,10,10,16,8,32,8,16,16)  (n2598, n2614, n2615, 
            n2597, n2613, n2596, n2612, n2595, n2611, n2594, n2610, 
            n2593, n2609, n2592, n2608, n2591, n2607, n2590, n2606, 
            n2589, n2605, n2588, n2604, n2587, n2603, n2586, n2602, 
            n2585, n2601, n2584, n2600, n2583, n2599, mclk_c, 
            s_addr_0, GND_net, \s_desc_data[18] , \s_desc_data[19] , 
            \s_desc_data[16] , \s_desc_data[17] , \s_desc_data[14] , \s_desc_data[15] , 
            \s_desc_data[12] , \s_desc_data[13] , \s_desc_data[10] , \s_desc_data[11] , 
            \s_desc_data[8] , \s_desc_data[30] , \s_desc_data[9] , \s_desc_data[6] , 
            \s_desc_data[7] , \s_desc_data[4] , \s_desc_data[5] , s_pos_0_3__N_670, 
            s_ptr_data, \s_cfg[6] , mclk_c_enable_118, s_desc_addr, 
            s_rx_mem_wr_stb, \s_desc_data[31] , s_rx_mem_addr, n8850, 
            s_rx_mem_din, \s_desc_data[29] , \s_desc_data[27] , \s_desc_data[28] , 
            \s_desc_data[25] , \s_desc_data[26] , \s_desc_data[23] , \s_desc_data[24] , 
            \s_desc_data[21] , \s_desc_data[22] , \s_desc_data[20] , n8851, 
            mclk_c_enable_91, tp_tms_c, s_tap_tdo, s_tap_tdi, \s_status[0] , 
            \s_status[1] , \s_status[2] , \s_status[3] , s_end_state, 
            s_reg_din, n8264, n8279, s_tap_busy, n2, n8273, n8274, 
            n8317, out_get_N_312, n7217, n4959, s_ptr_addr, s_cnt_en_N_1514, 
            \s_goto_state[0] , \s_goto_state[2] , \s_goto_state[1] , n1, 
            n7227, \s_irq[1] , s_ckgen_rtdiv, \s_cfg[5] , tp_rtck_c, 
            s_ckgen_div, \s_irq_en[1] , \irq_N_8[1] , tp_tck_c);
    input n2598;
    input n2614;
    input n2615;
    input n2597;
    input n2613;
    input n2596;
    input n2612;
    input n2595;
    input n2611;
    input n2594;
    input n2610;
    input n2593;
    input n2609;
    input n2592;
    input n2608;
    input n2591;
    input n2607;
    input n2590;
    input n2606;
    input n2589;
    input n2605;
    input n2588;
    input n2604;
    input n2587;
    input n2603;
    input n2586;
    input n2602;
    input n2585;
    input n2601;
    input n2584;
    input n2600;
    input n2583;
    input n2599;
    input mclk_c;
    output [9:0]s_addr_0;
    input GND_net;
    input \s_desc_data[18] ;
    input \s_desc_data[19] ;
    input \s_desc_data[16] ;
    input \s_desc_data[17] ;
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
    input \s_cfg[6] ;
    output mclk_c_enable_118;
    output [8:1]s_desc_addr;
    output s_rx_mem_wr_stb;
    input \s_desc_data[31] ;
    output [9:0]s_rx_mem_addr;
    input n8850;
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
    input n8851;
    input mclk_c_enable_91;
    output tp_tms_c;
    input s_tap_tdo;
    output s_tap_tdi;
    output \s_status[0] ;
    output \s_status[1] ;
    output \s_status[2] ;
    output \s_status[3] ;
    output [3:0]s_end_state;
    input [15:0]s_reg_din;
    output n8264;
    output n8279;
    output s_tap_busy;
    input n2;
    output n8273;
    output n8274;
    output n8317;
    input out_get_N_312;
    input n7217;
    input n4959;
    output [8:1]s_ptr_addr;
    output s_cnt_en_N_1514;
    input \s_goto_state[0] ;
    input \s_goto_state[2] ;
    input \s_goto_state[1] ;
    output n1;
    output n7227;
    output \s_irq[1] ;
    input [15:0]s_ckgen_rtdiv;
    input \s_cfg[5] ;
    input tp_rtck_c;
    input [15:0]s_ckgen_div;
    input \s_irq_en[1] ;
    output \irq_N_8[1] ;
    output tp_tck_c;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    wire mclk_c_enable_115 /* synthesis is_clock=1, SET_AS_NETWORK=mclk_c_enable_115 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    wire n8858, s_msbf, s_msbf_r, s_tx_shift_stb, s_en_r1, n8217, 
        s_so;
    wire [7:0]s_offs_r;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(80[9:17])
    
    wire n2478;
    wire [3:0]\s_op_fifo.buf_3_.msbf_N_1174 ;
    
    wire mclk_c_enable_243, mclk_c_enable_219, n8221, mclk_c_enable_207, 
        mclk_c_enable_231, tckn_stb, mclk_c_enable_137, n8212, s_en_r0, 
        n8228, n2503, mclk_c_enable_204;
    wire [9:0]s_count_r;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(121[9:18])
    
    wire s_count_r_0__N_893;
    wire [3:0]bit_pos;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(76[2:9])
    
    wire bit_pos_0__N_816, n8282, n8289, n8272, s_count_r_1__N_897, 
        op_msbf, n8294, s_offs_inc, s_mtdo, s_mtms, n7090, s_tcken, 
        s_tdien;
    
    \jtag_enh_tx(16,10,10,4)  tx (.n8858(n8858), .n2598(n2598), .n2614(n2614), 
            .n2615(n2615), .n2597(n2597), .n2613(n2613), .n2596(n2596), 
            .n2612(n2612), .n2595(n2595), .n2611(n2611), .n2594(n2594), 
            .n2610(n2610), .n2593(n2593), .n2609(n2609), .n2592(n2592), 
            .n2608(n2608), .n2591(n2591), .n2607(n2607), .n2590(n2590), 
            .n2606(n2606), .n2589(n2589), .n2605(n2605), .n2588(n2588), 
            .n2604(n2604), .n2587(n2587), .n2603(n2603), .n2586(n2586), 
            .n2602(n2602), .n2585(n2585), .n2601(n2601), .n2584(n2584), 
            .n2600(n2600), .n2583(n2583), .n2599(n2599), .s_msbf(s_msbf), 
            .mclk_c(mclk_c), .s_addr_0({s_addr_0}), .s_msbf_r(s_msbf_r), 
            .s_tx_shift_stb(s_tx_shift_stb), .GND_net(GND_net), .\s_desc_data[18] (\s_desc_data[18] ), 
            .\s_desc_data[19] (\s_desc_data[19] ), .\s_desc_data[16] (\s_desc_data[16] ), 
            .\s_desc_data[17] (\s_desc_data[17] ), .\s_desc_data[14] (\s_desc_data[14] ), 
            .\s_desc_data[15] (\s_desc_data[15] ), .\s_desc_data[12] (\s_desc_data[12] ), 
            .\s_desc_data[13] (\s_desc_data[13] ), .\s_desc_data[10] (\s_desc_data[10] ), 
            .\s_desc_data[11] (\s_desc_data[11] ), .s_en_r1(s_en_r1), .n8217(n8217), 
            .s_so(s_so), .\s_desc_data[8] (\s_desc_data[8] ), .\s_desc_data[30] (\s_desc_data[30] ), 
            .\s_desc_data[9] (\s_desc_data[9] ), .\s_desc_data[6] (\s_desc_data[6] ), 
            .\s_desc_data[7] (\s_desc_data[7] ), .\s_desc_data[4] (\s_desc_data[4] ), 
            .\s_desc_data[5] (\s_desc_data[5] ), .s_pos_0_3__N_670({s_pos_0_3__N_670}));   // c:/devel/yard-ice/src/fpga/jtag_enh_shifter.vhdl(236[7:25])
    \jtag_enh_shift_ctrl(10,8)  shift_ctrl (.s_ptr_data({s_ptr_data}), .GND_net(GND_net), 
            .s_offs_r({Open_6, Open_7, Open_8, Open_9, Open_10, Open_11, 
            Open_12, s_offs_r[0]}), .mclk_c(mclk_c), .n2478(n2478), 
            .\s_desc_data[4] (\s_desc_data[4] ), .n8858(n8858), .s_pos_0_3__N_670({s_pos_0_3__N_670}), 
            .n8217(n8217), .s_en_r1(s_en_r1), .\s_op_fifo.buf_3_.msbf_N_1174 ({\s_op_fifo.buf_3_.msbf_N_1174 }), 
            .\s_cfg[6] (\s_cfg[6] ), .mclk_c_enable_243(mclk_c_enable_243), 
            .mclk_c_enable_219(mclk_c_enable_219), .\s_desc_data[30] (\s_desc_data[30] ), 
            .s_msbf_r(s_msbf_r), .s_msbf(s_msbf), .n8221(n8221), .mclk_c_enable_207(mclk_c_enable_207), 
            .mclk_c_enable_231(mclk_c_enable_231), .\s_offs_r[1] (s_offs_r[1]), 
            .tckn_stb(tckn_stb), .mclk_c_enable_137(mclk_c_enable_137), 
            .n8212(n8212), .s_en_r0(s_en_r0), .n8228(n8228), .mclk_c_enable_118(mclk_c_enable_118), 
            .s_tx_shift_stb(s_tx_shift_stb), .n2503(n2503), .s_desc_addr({s_desc_addr}), 
            .\s_desc_data[9] (\s_desc_data[9] ), .\s_desc_data[8] (\s_desc_data[8] ), 
            .\s_desc_data[7] (\s_desc_data[7] ), .\s_desc_data[6] (\s_desc_data[6] ), 
            .\s_desc_data[5] (\s_desc_data[5] ), .mclk_c_enable_204(mclk_c_enable_204), 
            .\s_count_r[0] (s_count_r[0]), .s_count_r_0__N_893(s_count_r_0__N_893), 
            .\bit_pos[0] (bit_pos[0]), .bit_pos_0__N_816(bit_pos_0__N_816), 
            .\bit_pos[1] (bit_pos[1]), .n8282(n8282), .n8289(n8289), .\s_count_r[1] (s_count_r[1]), 
            .n8272(n8272), .s_count_r_1__N_897(s_count_r_1__N_897), .op_msbf(op_msbf), 
            .n8294(n8294), .s_offs_inc(s_offs_inc));   // c:/devel/yard-ice/src/fpga/jtag_enh_shifter.vhdl(288[15:41])
    \jtag_enh_rx(16,10,10,4)  rx (.s_count_r_0__N_893(s_count_r_0__N_893), 
            .mclk_c(mclk_c), .mclk_c_enable_118(mclk_c_enable_118), .bit_pos_0__N_816(bit_pos_0__N_816), 
            .s_pos_0_3__N_670({s_pos_0_3__N_670}), .\s_op_fifo.buf_3_.msbf_N_1174 ({\s_op_fifo.buf_3_.msbf_N_1174 }), 
            .\s_desc_data[4] (\s_desc_data[4] ), .\s_desc_data[5] (\s_desc_data[5] ), 
            .\s_desc_data[6] (\s_desc_data[6] ), .\s_desc_data[7] (\s_desc_data[7] ), 
            .\s_desc_data[8] (\s_desc_data[8] ), .\s_desc_data[9] (\s_desc_data[9] ), 
            .op_msbf(op_msbf), .s_rx_mem_wr_stb(s_rx_mem_wr_stb), .mclk_c_enable_243(mclk_c_enable_243), 
            .\s_desc_data[31] (\s_desc_data[31] ), .mclk_c_enable_207(mclk_c_enable_207), 
            .n8289(n8289), .s_count_r({Open_13, Open_14, Open_15, Open_16, 
            Open_17, Open_18, Open_19, Open_20, s_count_r[1], Open_21}), 
            .n8272(n8272), .mclk_c_enable_231(mclk_c_enable_231), .mclk_c_enable_219(mclk_c_enable_219), 
            .s_rx_mem_addr({s_rx_mem_addr}), .mclk_c_enable_204(mclk_c_enable_204), 
            .\s_count_r[0] (s_count_r[0]), .n8294(n8294), .bit_pos({Open_22, 
            Open_23, bit_pos[1], Open_24}), .\bit_pos[0] (bit_pos[0]), 
            .n8850(n8850), .s_rx_mem_din({s_rx_mem_din}), .s_mtdo(s_mtdo), 
            .n8282(n8282), .\s_desc_data[29] (\s_desc_data[29] ), .GND_net(GND_net), 
            .\s_desc_data[27] (\s_desc_data[27] ), .\s_desc_data[28] (\s_desc_data[28] ), 
            .\s_desc_data[25] (\s_desc_data[25] ), .\s_desc_data[26] (\s_desc_data[26] ), 
            .\s_desc_data[23] (\s_desc_data[23] ), .\s_desc_data[24] (\s_desc_data[24] ), 
            .\s_desc_data[21] (\s_desc_data[21] ), .\s_desc_data[22] (\s_desc_data[22] ), 
            .\s_desc_data[20] (\s_desc_data[20] ), .n8858(n8858), .s_count_r_1__N_897(s_count_r_1__N_897));   // c:/devel/yard-ice/src/fpga/jtag_enh_shifter.vhdl(263[7:25])
    jtag_enh_io io (.mclk_c_enable_115(mclk_c_enable_115), .tckn_stb(tckn_stb), 
            .n8851(n8851), .mclk_c(mclk_c), .mclk_c_enable_91(mclk_c_enable_91), 
            .tp_tms_c(tp_tms_c), .s_mtms(s_mtms), .s_mtdo(s_mtdo), .s_tap_tdo(s_tap_tdo), 
            .s_tap_tdi(s_tap_tdi), .n7090(n7090), .s_so(s_so));   // c:/devel/yard-ice/src/fpga/jtag_enh_shifter.vhdl(216[7:25])
    \jtag_enh_ecu(16,8,8)  exec_ctrl (.mclk_c(mclk_c), .s_tcken(s_tcken), 
            .n8851(n8851), .\s_status[0] (\s_status[0] ), .\s_status[1] (\s_status[1] ), 
            .\s_status[2] (\s_status[2] ), .\s_status[3] (\s_status[3] ), 
            .s_end_state({s_end_state}), .s_tdien(s_tdien), .s_reg_din({s_reg_din}), 
            .s_mtms(s_mtms), .n8264(n8264), .s_en_r0(s_en_r0), .n8228(n8228), 
            .n8212(n8212), .\s_offs_r[1] (s_offs_r[1]), .n2503(n2503), 
            .s_offs_inc(s_offs_inc), .\s_offs_r[0] (s_offs_r[0]), .n2478(n2478), 
            .tckn_stb(tckn_stb), .s_en_r1(s_en_r1), .mclk_c_enable_137(mclk_c_enable_137), 
            .n8279(n8279), .s_tap_busy(s_tap_busy), .\s_cfg[6] (\s_cfg[6] ), 
            .n2(n2), .n8221(n8221), .n8273(n8273), .n8274(n8274), .GND_net(GND_net), 
            .n8317(n8317), .out_get_N_312(out_get_N_312), .n7217(n7217), 
            .n4959(n4959), .s_ptr_addr({s_ptr_addr}), .s_cnt_en_N_1514(s_cnt_en_N_1514), 
            .\s_goto_state[0] (\s_goto_state[0] ), .\s_goto_state[2] (\s_goto_state[2] ), 
            .\s_goto_state[1] (\s_goto_state[1] ), .n1(n1), .n7227(n7227));   // c:/devel/yard-ice/src/fpga/jtag_enh_shifter.vhdl(313[14:33])
    \jtag_enh_ckgen(16)  ckgen (.mclk_c(mclk_c), .GND_net(GND_net), .\s_irq[1] (\s_irq[1] ), 
            .s_ckgen_rtdiv({s_ckgen_rtdiv}), .s_tcken(s_tcken), .\s_cfg[5] (\s_cfg[5] ), 
            .tp_rtck_c(tp_rtck_c), .s_ckgen_div({s_ckgen_div}), .\s_irq_en[1] (\s_irq_en[1] ), 
            .\irq_N_8[1] (\irq_N_8[1] ), .tp_tck_c(tp_tck_c), .mclk_c_enable_115(mclk_c_enable_115), 
            .s_tdien(s_tdien), .n7090(n7090));   // c:/devel/yard-ice/src/fpga/jtag_enh_shifter.vhdl(195[10:31])
    
endmodule
//
// Verilog Description of module \jtag_enh_tx(16,10,10,4) 
//

module \jtag_enh_tx(16,10,10,4)  (n8858, n2598, n2614, n2615, n2597, 
            n2613, n2596, n2612, n2595, n2611, n2594, n2610, n2593, 
            n2609, n2592, n2608, n2591, n2607, n2590, n2606, n2589, 
            n2605, n2588, n2604, n2587, n2603, n2586, n2602, n2585, 
            n2601, n2584, n2600, n2583, n2599, s_msbf, mclk_c, 
            s_addr_0, s_msbf_r, s_tx_shift_stb, GND_net, \s_desc_data[18] , 
            \s_desc_data[19] , \s_desc_data[16] , \s_desc_data[17] , \s_desc_data[14] , 
            \s_desc_data[15] , \s_desc_data[12] , \s_desc_data[13] , \s_desc_data[10] , 
            \s_desc_data[11] , s_en_r1, n8217, s_so, \s_desc_data[8] , 
            \s_desc_data[30] , \s_desc_data[9] , \s_desc_data[6] , \s_desc_data[7] , 
            \s_desc_data[4] , \s_desc_data[5] , s_pos_0_3__N_670);
    input n8858;
    input n2598;
    input n2614;
    input n2615;
    input n2597;
    input n2613;
    input n2596;
    input n2612;
    input n2595;
    input n2611;
    input n2594;
    input n2610;
    input n2593;
    input n2609;
    input n2592;
    input n2608;
    input n2591;
    input n2607;
    input n2590;
    input n2606;
    input n2589;
    input n2605;
    input n2588;
    input n2604;
    input n2587;
    input n2603;
    input n2586;
    input n2602;
    input n2585;
    input n2601;
    input n2584;
    input n2600;
    input n2583;
    input n2599;
    input s_msbf;
    input mclk_c;
    output [9:0]s_addr_0;
    output s_msbf_r;
    input s_tx_shift_stb;
    input GND_net;
    input \s_desc_data[18] ;
    input \s_desc_data[19] ;
    input \s_desc_data[16] ;
    input \s_desc_data[17] ;
    input \s_desc_data[14] ;
    input \s_desc_data[15] ;
    input \s_desc_data[12] ;
    input \s_desc_data[13] ;
    input \s_desc_data[10] ;
    input \s_desc_data[11] ;
    input s_en_r1;
    input n8217;
    output s_so;
    input \s_desc_data[8] ;
    input \s_desc_data[30] ;
    input \s_desc_data[9] ;
    input \s_desc_data[6] ;
    input \s_desc_data[7] ;
    input \s_desc_data[4] ;
    input \s_desc_data[5] ;
    input [3:0]s_pos_0_3__N_670;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    wire s_pos_3__N_687;
    wire [3:0]n1;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(75[9:16])
    wire [3:0]n2031;
    
    wire n7422, n7421, n7420, n7419, n7418, n7417, n7416, n7415, 
        n7414, n7413, n7412, n7411, n7410, n7409, n7408, n7407, 
        n8249;
    wire [9:0]s_addr_r;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(82[9:17])
    
    wire n8202, n8198;
    wire [9:0]s_sum;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(83[9:14])
    
    wire n8190;
    wire [3:0]s_pos;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(79[9:14])
    
    wire n8290, n8260, n8271, n8195, n6498, n7431, n7432, n7435, 
        n7433, n7434, n7436, n6497, n6496;
    wire [5:0]n2221;
    
    wire n6495, n6494, n8191, n8189, n8192, n8193, n7423, n7424, 
        n7425, n7426, n7427, n7428, n7429, n7430;
    
    PFUMX mux_509_i4 (.BLUT(s_pos_3__N_687), .ALUT(n1[3]), .C0(n8858), 
          .Z(n2031[3]));
    LUT4 i5759_3_lut (.A(n2598), .B(n2614), .C(n2615), .Z(n7422)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5759_3_lut.init = 16'hcaca;
    LUT4 i5758_3_lut (.A(n2597), .B(n2613), .C(n2615), .Z(n7421)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5758_3_lut.init = 16'hcaca;
    LUT4 i5757_3_lut (.A(n2596), .B(n2612), .C(n2615), .Z(n7420)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5757_3_lut.init = 16'hcaca;
    LUT4 i5756_3_lut (.A(n2595), .B(n2611), .C(n2615), .Z(n7419)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5756_3_lut.init = 16'hcaca;
    LUT4 i5755_3_lut (.A(n2594), .B(n2610), .C(n2615), .Z(n7418)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5755_3_lut.init = 16'hcaca;
    LUT4 i5754_3_lut (.A(n2593), .B(n2609), .C(n2615), .Z(n7417)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5754_3_lut.init = 16'hcaca;
    LUT4 i5753_3_lut (.A(n2592), .B(n2608), .C(n2615), .Z(n7416)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5753_3_lut.init = 16'hcaca;
    LUT4 i5752_3_lut (.A(n2591), .B(n2607), .C(n2615), .Z(n7415)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5752_3_lut.init = 16'hcaca;
    LUT4 i5751_3_lut (.A(n2590), .B(n2606), .C(n2615), .Z(n7414)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5751_3_lut.init = 16'hcaca;
    LUT4 i5750_3_lut (.A(n2589), .B(n2605), .C(n2615), .Z(n7413)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5750_3_lut.init = 16'hcaca;
    LUT4 i5749_3_lut (.A(n2588), .B(n2604), .C(n2615), .Z(n7412)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5749_3_lut.init = 16'hcaca;
    LUT4 i5748_3_lut (.A(n2587), .B(n2603), .C(n2615), .Z(n7411)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5748_3_lut.init = 16'hcaca;
    LUT4 i5747_3_lut (.A(n2586), .B(n2602), .C(n2615), .Z(n7410)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5747_3_lut.init = 16'hcaca;
    LUT4 i5746_3_lut (.A(n2585), .B(n2601), .C(n2615), .Z(n7409)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5746_3_lut.init = 16'hcaca;
    LUT4 i5745_3_lut (.A(n2584), .B(n2600), .C(n2615), .Z(n7408)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5745_3_lut.init = 16'hcaca;
    LUT4 i5744_3_lut (.A(n2583), .B(n2599), .C(n2615), .Z(n7407)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5744_3_lut.init = 16'hcaca;
    LUT4 s_sum_9__I_73_3_lut_rep_88 (.A(s_msbf), .B(n8249), .C(s_addr_r[0]), 
         .Z(n8202)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B (C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_73_3_lut_rep_88.init = 16'h4848;
    LUT4 s_sum_9__I_72_3_lut_rep_84_4_lut (.A(s_msbf), .B(n8249), .C(s_addr_r[0]), 
         .D(s_addr_r[1]), .Z(n8198)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_72_3_lut_rep_84_4_lut.init = 16'h4008;
    LUT4 s_addr_r_9__I_0_i2_2_lut_4_lut (.A(s_msbf), .B(n8249), .C(s_addr_r[0]), 
         .D(s_addr_r[1]), .Z(s_sum[1])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i2_2_lut_4_lut.init = 16'hb748;
    LUT4 s_addr_r_9__I_0_i9_2_lut_4_lut (.A(s_msbf), .B(n8190), .C(s_addr_r[7]), 
         .D(s_addr_r[8]), .Z(s_sum[8])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i9_2_lut_4_lut.init = 16'hb748;
    FD1S3AX s_pos_0__91 (.D(n2031[0]), .CK(mclk_c), .Q(s_pos[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(126[4] 136[11])
    defparam s_pos_0__91.GSR = "ENABLED";
    FD1S3AX s_pos_1__92 (.D(n2031[1]), .CK(mclk_c), .Q(s_pos[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(126[4] 136[11])
    defparam s_pos_1__92.GSR = "ENABLED";
    FD1S3AX s_pos_2__93 (.D(n2031[2]), .CK(mclk_c), .Q(s_pos[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(126[4] 136[11])
    defparam s_pos_2__93.GSR = "ENABLED";
    FD1S3AX s_pos_3__94 (.D(n2031[3]), .CK(mclk_c), .Q(s_pos[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(126[4] 136[11])
    defparam s_pos_3__94.GSR = "ENABLED";
    FD1S3AX s_addr_r_i0 (.D(s_addr_0[0]), .CK(mclk_c), .Q(s_addr_r[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(161[3] 165[10])
    defparam s_addr_r_i0.GSR = "ENABLED";
    LUT4 s_pos_3__I_62_3_lut_rep_146_4_lut (.A(s_msbf_r), .B(n8290), .C(s_pos[1]), 
         .D(s_pos[2]), .Z(n8260)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(137[10:40])
    defparam s_pos_3__I_62_3_lut_rep_146_4_lut.init = 16'h4008;
    FD1S3AX s_msbf_r_90 (.D(s_msbf), .CK(mclk_c), .Q(s_msbf_r)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=236, LSE_RLINE=236 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(107[3] 111[10])
    defparam s_msbf_r_90.GSR = "ENABLED";
    LUT4 s_pos_2__bdd_4_lut (.A(s_pos[2]), .B(n8271), .C(n1[2]), .D(n8858), 
         .Z(n2031[2])) /* synthesis lut_function=(A (B (C (D))+!B (C+!(D)))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam s_pos_2__bdd_4_lut.init = 16'hf066;
    LUT4 s_pos_1__bdd_4_lut (.A(s_pos[1]), .B(n8290), .C(n1[1]), .D(n8858), 
         .Z(n2031[1])) /* synthesis lut_function=(A (B (C (D))+!B (C+!(D)))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam s_pos_1__bdd_4_lut.init = 16'hf066;
    LUT4 s_pos_0__bdd_4_lut (.A(s_pos[0]), .B(s_tx_shift_stb), .C(n1[0]), 
         .D(n8858), .Z(n2031[0])) /* synthesis lut_function=(A (B (C (D))+!B (C+!(D)))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam s_pos_0__bdd_4_lut.init = 16'hf066;
    LUT4 s_sum_9__I_71_3_lut_rep_81_4_lut (.A(s_msbf), .B(n8202), .C(s_addr_r[1]), 
         .D(s_addr_r[2]), .Z(n8195)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_71_3_lut_rep_81_4_lut.init = 16'h4008;
    LUT4 s_addr_r_9__I_0_i3_2_lut_4_lut (.A(s_msbf), .B(n8202), .C(s_addr_r[1]), 
         .D(s_addr_r[2]), .Z(s_sum[2])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i3_2_lut_4_lut.init = 16'hb748;
    CCU2D add_516_11 (.A0(GND_net), .B0(n8858), .C0(s_sum[8]), .D0(\s_desc_data[18] ), 
          .A1(GND_net), .B1(n8858), .C1(s_sum[9]), .D1(\s_desc_data[19] ), 
          .CIN(n6498), .S0(s_addr_0[8]), .S1(s_addr_0[9]));   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam add_516_11.INIT0 = 16'h569a;
    defparam add_516_11.INIT1 = 16'h569a;
    defparam add_516_11.INJECT1_0 = "NO";
    defparam add_516_11.INJECT1_1 = "NO";
    L6MUX21 i5772 (.D0(n7431), .D1(n7432), .SD(s_pos[2]), .Z(n7435));
    L6MUX21 i5773 (.D0(n7433), .D1(n7434), .SD(s_pos[2]), .Z(n7436));
    LUT4 s_addr_r_9__I_0_i5_2_lut_4_lut (.A(s_msbf), .B(n8195), .C(s_addr_r[3]), 
         .D(s_addr_r[4]), .Z(s_sum[4])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i5_2_lut_4_lut.init = 16'hb748;
    CCU2D add_516_9 (.A0(GND_net), .B0(n8858), .C0(s_sum[6]), .D0(\s_desc_data[16] ), 
          .A1(GND_net), .B1(n8858), .C1(s_sum[7]), .D1(\s_desc_data[17] ), 
          .CIN(n6497), .COUT(n6498), .S0(s_addr_0[6]), .S1(s_addr_0[7]));   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam add_516_9.INIT0 = 16'h569a;
    defparam add_516_9.INIT1 = 16'h569a;
    defparam add_516_9.INJECT1_0 = "NO";
    defparam add_516_9.INJECT1_1 = "NO";
    CCU2D add_516_7 (.A0(n2221[4]), .B0(n8858), .C0(s_sum[4]), .D0(\s_desc_data[14] ), 
          .A1(n2221[5]), .B1(n8858), .C1(s_sum[5]), .D1(\s_desc_data[15] ), 
          .CIN(n6496), .COUT(n6497), .S0(s_addr_0[4]), .S1(s_addr_0[5]));   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam add_516_7.INIT0 = 16'h74b8;
    defparam add_516_7.INIT1 = 16'h74b8;
    defparam add_516_7.INJECT1_0 = "NO";
    defparam add_516_7.INJECT1_1 = "NO";
    CCU2D add_516_5 (.A0(n2221[2]), .B0(n8858), .C0(s_sum[2]), .D0(\s_desc_data[12] ), 
          .A1(n2221[3]), .B1(n8858), .C1(s_sum[3]), .D1(\s_desc_data[13] ), 
          .CIN(n6495), .COUT(n6496), .S0(s_addr_0[2]), .S1(s_addr_0[3]));   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam add_516_5.INIT0 = 16'h74b8;
    defparam add_516_5.INIT1 = 16'h74b8;
    defparam add_516_5.INJECT1_0 = "NO";
    defparam add_516_5.INJECT1_1 = "NO";
    CCU2D add_516_3 (.A0(n2221[0]), .B0(n8858), .C0(s_sum[0]), .D0(\s_desc_data[10] ), 
          .A1(n2221[1]), .B1(n8858), .C1(s_sum[1]), .D1(\s_desc_data[11] ), 
          .CIN(n6494), .COUT(n6495), .S0(s_addr_0[0]), .S1(s_addr_0[1]));   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam add_516_3.INIT0 = 16'h74b8;
    defparam add_516_3.INIT1 = 16'h74b8;
    defparam add_516_3.INJECT1_0 = "NO";
    defparam add_516_3.INJECT1_1 = "NO";
    CCU2D add_516_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_en_r1), .B1(n8217), .C1(GND_net), .D1(GND_net), .COUT(n6494));   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam add_516_1.INIT0 = 16'hF000;
    defparam add_516_1.INIT1 = 16'hffff;
    defparam add_516_1.INJECT1_0 = "NO";
    defparam add_516_1.INJECT1_1 = "NO";
    LUT4 s_sum_9__I_66_3_lut_rep_75_4_lut (.A(s_msbf), .B(n8191), .C(s_addr_r[6]), 
         .D(s_addr_r[7]), .Z(n8189)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_66_3_lut_rep_75_4_lut.init = 16'h4008;
    LUT4 s_pos_3__I_64_3_lut_rep_176 (.A(s_msbf_r), .B(s_tx_shift_stb), 
         .C(s_pos[0]), .Z(n8290)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B (C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(137[10:40])
    defparam s_pos_3__I_64_3_lut_rep_176.init = 16'h4848;
    LUT4 s_pos_3__I_63_3_lut_rep_157_4_lut (.A(s_msbf_r), .B(s_tx_shift_stb), 
         .C(s_pos[0]), .D(s_pos[1]), .Z(n8271)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(137[10:40])
    defparam s_pos_3__I_63_3_lut_rep_157_4_lut.init = 16'h4008;
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
    LUT4 s_sum_9__I_67_3_lut_rep_76_4_lut (.A(s_msbf), .B(n8192), .C(s_addr_r[5]), 
         .D(s_addr_r[6]), .Z(n8190)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_67_3_lut_rep_76_4_lut.init = 16'h4008;
    LUT4 s_addr_r_9__I_0_i7_2_lut_4_lut (.A(s_msbf), .B(n8192), .C(s_addr_r[5]), 
         .D(s_addr_r[6]), .Z(s_sum[6])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i7_2_lut_4_lut.init = 16'hb748;
    LUT4 s_sum_9__I_68_3_lut_rep_77_4_lut (.A(s_msbf), .B(n8193), .C(s_addr_r[4]), 
         .D(s_addr_r[5]), .Z(n8191)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_68_3_lut_rep_77_4_lut.init = 16'h4008;
    LUT4 s_addr_r_9__I_0_i6_2_lut_4_lut (.A(s_msbf), .B(n8193), .C(s_addr_r[4]), 
         .D(s_addr_r[5]), .Z(s_sum[5])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i6_2_lut_4_lut.init = 16'hb748;
    L6MUX21 i5768 (.D0(n7423), .D1(n7424), .SD(s_pos[1]), .Z(n7431));
    L6MUX21 i5769 (.D0(n7425), .D1(n7426), .SD(s_pos[1]), .Z(n7432));
    L6MUX21 i5770 (.D0(n7427), .D1(n7428), .SD(s_pos[1]), .Z(n7433));
    L6MUX21 i5771 (.D0(n7429), .D1(n7430), .SD(s_pos[1]), .Z(n7434));
    LUT4 s_addr_r_9__I_0_i10_4_lut (.A(s_addr_r[9]), .B(s_msbf), .C(n8189), 
         .D(s_addr_r[8]), .Z(s_sum[9])) /* synthesis lut_function=(A (B ((D)+!C)+!B !(C (D)))+!A !(B ((D)+!C)+!B !(C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(149[16:34])
    defparam s_addr_r_9__I_0_i10_4_lut.init = 16'h9a6a;
    LUT4 i5774_3_lut (.A(n7435), .B(n7436), .C(s_pos[3]), .Z(s_so)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5774_3_lut.init = 16'hcaca;
    LUT4 i3377_2_lut (.A(\s_desc_data[8] ), .B(\s_desc_data[30] ), .Z(n2221[4])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam i3377_2_lut.init = 16'h8888;
    LUT4 i3378_2_lut (.A(\s_desc_data[9] ), .B(\s_desc_data[30] ), .Z(n2221[5])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam i3378_2_lut.init = 16'h8888;
    LUT4 i3375_2_lut (.A(\s_desc_data[6] ), .B(\s_desc_data[30] ), .Z(n2221[2])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam i3375_2_lut.init = 16'h8888;
    LUT4 i3376_2_lut (.A(\s_desc_data[7] ), .B(\s_desc_data[30] ), .Z(n2221[3])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam i3376_2_lut.init = 16'h8888;
    LUT4 s_addr_r_9__I_0_i1_2_lut_4_lut (.A(s_msbf_r), .B(n8260), .C(s_pos[3]), 
         .D(s_addr_r[0]), .Z(s_sum[0])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(137[10:40])
    defparam s_addr_r_9__I_0_i1_2_lut_4_lut.init = 16'hb748;
    LUT4 i3260_2_lut (.A(\s_desc_data[4] ), .B(\s_desc_data[30] ), .Z(n2221[0])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam i3260_2_lut.init = 16'h8888;
    LUT4 i3374_2_lut (.A(\s_desc_data[5] ), .B(\s_desc_data[30] ), .Z(n2221[1])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(99[14] 101[53])
    defparam i3374_2_lut.init = 16'h8888;
    LUT4 s_addr_r_9__I_0_i8_2_lut_4_lut (.A(s_msbf), .B(n8191), .C(s_addr_r[6]), 
         .D(s_addr_r[7]), .Z(s_sum[7])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i8_2_lut_4_lut.init = 16'hb748;
    LUT4 s_pos_wrap_I_0_3_lut_rep_135_4_lut (.A(s_msbf_r), .B(n8271), .C(s_pos[2]), 
         .D(s_pos[3]), .Z(n8249)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(137[10:40])
    defparam s_pos_wrap_I_0_3_lut_rep_135_4_lut.init = 16'h4008;
    LUT4 s_pos_3__I_0_112_2_lut_4_lut (.A(s_msbf_r), .B(n8271), .C(s_pos[2]), 
         .D(s_pos[3]), .Z(s_pos_3__N_687)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(137[10:40])
    defparam s_pos_3__I_0_112_2_lut_4_lut.init = 16'hb748;
    LUT4 i3244_2_lut (.A(s_pos_0_3__N_670[0]), .B(\s_desc_data[30] ), .Z(n1[0])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(96[13] 97[63])
    defparam i3244_2_lut.init = 16'h8888;
    LUT4 s_sum_9__I_70_3_lut_rep_79_4_lut (.A(s_msbf), .B(n8198), .C(s_addr_r[2]), 
         .D(s_addr_r[3]), .Z(n8193)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_70_3_lut_rep_79_4_lut.init = 16'h4008;
    LUT4 i3366_2_lut (.A(s_pos_0_3__N_670[1]), .B(\s_desc_data[30] ), .Z(n1[1])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(96[13] 97[63])
    defparam i3366_2_lut.init = 16'h8888;
    LUT4 s_addr_r_9__I_0_i4_2_lut_4_lut (.A(s_msbf), .B(n8198), .C(s_addr_r[2]), 
         .D(s_addr_r[3]), .Z(s_sum[3])) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_addr_r_9__I_0_i4_2_lut_4_lut.init = 16'hb748;
    LUT4 i3367_2_lut (.A(s_pos_0_3__N_670[2]), .B(\s_desc_data[30] ), .Z(n1[2])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(96[13] 97[63])
    defparam i3367_2_lut.init = 16'h8888;
    LUT4 i3368_2_lut (.A(s_pos_0_3__N_670[3]), .B(\s_desc_data[30] ), .Z(n1[3])) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(96[13] 97[63])
    defparam i3368_2_lut.init = 16'h8888;
    PFUMX i5760 (.BLUT(n7407), .ALUT(n7408), .C0(s_pos[0]), .Z(n7423));
    LUT4 s_sum_9__I_69_3_lut_rep_78_4_lut (.A(s_msbf), .B(n8195), .C(s_addr_r[3]), 
         .D(s_addr_r[4]), .Z(n8192)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_tx.vhdl(150[10:41])
    defparam s_sum_9__I_69_3_lut_rep_78_4_lut.init = 16'h4008;
    PFUMX i5761 (.BLUT(n7409), .ALUT(n7410), .C0(s_pos[0]), .Z(n7424));
    PFUMX i5762 (.BLUT(n7411), .ALUT(n7412), .C0(s_pos[0]), .Z(n7425));
    PFUMX i5763 (.BLUT(n7413), .ALUT(n7414), .C0(s_pos[0]), .Z(n7426));
    PFUMX i5764 (.BLUT(n7415), .ALUT(n7416), .C0(s_pos[0]), .Z(n7427));
    PFUMX i5765 (.BLUT(n7417), .ALUT(n7418), .C0(s_pos[0]), .Z(n7428));
    PFUMX i5766 (.BLUT(n7419), .ALUT(n7420), .C0(s_pos[0]), .Z(n7429));
    PFUMX i5767 (.BLUT(n7421), .ALUT(n7422), .C0(s_pos[0]), .Z(n7430));
    
endmodule
//
// Verilog Description of module \jtag_enh_shift_ctrl(10,8) 
//

module \jtag_enh_shift_ctrl(10,8)  (s_ptr_data, GND_net, s_offs_r, mclk_c, 
            n2478, \s_desc_data[4] , n8858, s_pos_0_3__N_670, n8217, 
            s_en_r1, \s_op_fifo.buf_3_.msbf_N_1174 , \s_cfg[6] , mclk_c_enable_243, 
            mclk_c_enable_219, \s_desc_data[30] , s_msbf_r, s_msbf, 
            n8221, mclk_c_enable_207, mclk_c_enable_231, \s_offs_r[1] , 
            tckn_stb, mclk_c_enable_137, n8212, s_en_r0, n8228, mclk_c_enable_118, 
            s_tx_shift_stb, n2503, s_desc_addr, \s_desc_data[9] , \s_desc_data[8] , 
            \s_desc_data[7] , \s_desc_data[6] , \s_desc_data[5] , mclk_c_enable_204, 
            \s_count_r[0] , s_count_r_0__N_893, \bit_pos[0] , bit_pos_0__N_816, 
            \bit_pos[1] , n8282, n8289, \s_count_r[1] , n8272, s_count_r_1__N_897, 
            op_msbf, n8294, s_offs_inc);
    input [15:0]s_ptr_data;
    input GND_net;
    output [7:0]s_offs_r;
    input mclk_c;
    input n2478;
    input \s_desc_data[4] ;
    output n8858;
    input [3:0]s_pos_0_3__N_670;
    output n8217;
    output s_en_r1;
    input [3:0]\s_op_fifo.buf_3_.msbf_N_1174 ;
    input \s_cfg[6] ;
    output mclk_c_enable_243;
    output mclk_c_enable_219;
    input \s_desc_data[30] ;
    input s_msbf_r;
    output s_msbf;
    input n8221;
    output mclk_c_enable_207;
    output mclk_c_enable_231;
    output \s_offs_r[1] ;
    input tckn_stb;
    input mclk_c_enable_137;
    output n8212;
    output s_en_r0;
    input n8228;
    output mclk_c_enable_118;
    output s_tx_shift_stb;
    input n2503;
    output [8:1]s_desc_addr;
    input \s_desc_data[9] ;
    input \s_desc_data[8] ;
    input \s_desc_data[7] ;
    input \s_desc_data[6] ;
    input \s_desc_data[5] ;
    output mclk_c_enable_204;
    input \s_count_r[0] ;
    output s_count_r_0__N_893;
    input \bit_pos[0] ;
    output bit_pos_0__N_816;
    input \bit_pos[1] ;
    output n8282;
    output n8289;
    input \s_count_r[1] ;
    output n8272;
    output s_count_r_1__N_897;
    input op_msbf;
    output n8294;
    output s_offs_inc;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    wire n6503;
    wire [7:0]s_stop_N_1313;
    
    wire n6502, n6501;
    wire [9:0]bit_cnt;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(68[2:9])
    
    wire n8269, bit_cnt_4__N_1226, n6500, n8281, bit_cnt_3__N_1220;
    wire [7:0]s_offs_r_c;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(80[9:17])
    
    wire n2482, n8207, n8196, n2494, bit_cnt_0__N_1203, n8250, s_en_r2, 
        n8229, n2480, bit_cnt_9__N_1189, bit_cnt_8__N_1250, bit_cnt_7__N_1244, 
        n8199, n2486, n8222, n8305, bit_cnt_2__N_1214, s_rx_en_r1, 
        mclk_c_enable_89, n4071, s_stop_N_1312, bit_cnt_6__N_1238, bit_cnt_5__N_1232, 
        n2501, n2490, n8258, n8235, bit_cnt_1__N_1208, n8243, s_cnt_stb, 
        n8194, n8203, n6406, n6405, n6507, n6506, n6505, n13, 
        n14;
    
    CCU2D add_191_9 (.A0(s_ptr_data[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n6503), 
          .S0(s_stop_N_1313[7]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_191_9.INIT0 = 16'h5aaa;
    defparam add_191_9.INIT1 = 16'h0000;
    defparam add_191_9.INJECT1_0 = "NO";
    defparam add_191_9.INJECT1_1 = "NO";
    CCU2D add_191_7 (.A0(s_ptr_data[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_ptr_data[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n6502), .COUT(n6503), .S0(s_stop_N_1313[5]), .S1(s_stop_N_1313[6]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_191_7.INIT0 = 16'h5aaa;
    defparam add_191_7.INIT1 = 16'h5aaa;
    defparam add_191_7.INJECT1_0 = "NO";
    defparam add_191_7.INJECT1_1 = "NO";
    FD1S3AY s_offs_r_0__225 (.D(n2478), .CK(mclk_c), .Q(s_offs_r[0]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_0__225.GSR = "ENABLED";
    CCU2D add_191_5 (.A0(s_ptr_data[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_ptr_data[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n6501), .COUT(n6502), .S0(s_stop_N_1313[3]), .S1(s_stop_N_1313[4]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_191_5.INIT0 = 16'h5aaa;
    defparam add_191_5.INIT1 = 16'h5aaa;
    defparam add_191_5.INJECT1_0 = "NO";
    defparam add_191_5.INJECT1_1 = "NO";
    LUT4 bit_cnt_4__I_129_4_lut (.A(bit_cnt[4]), .B(\s_desc_data[4] ), .C(n8858), 
         .D(n8269), .Z(bit_cnt_4__N_1226)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_4__I_129_4_lut.init = 16'hcac5;
    CCU2D add_191_3 (.A0(s_ptr_data[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_ptr_data[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n6500), .COUT(n6501), .S0(s_stop_N_1313[1]), .S1(s_stop_N_1313[2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_191_3.INIT0 = 16'h5aaa;
    defparam add_191_3.INIT1 = 16'h5aaa;
    defparam add_191_3.INJECT1_0 = "NO";
    defparam add_191_3.INJECT1_1 = "NO";
    LUT4 bit_cnt_3__I_128_4_lut (.A(bit_cnt[3]), .B(s_pos_0_3__N_670[3]), 
         .C(n8858), .D(n8281), .Z(bit_cnt_3__N_1220)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_3__I_128_4_lut.init = 16'hcac5;
    FD1S3AY s_offs_r_6__231 (.D(n2482), .CK(mclk_c), .Q(s_offs_r_c[6]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_6__231.GSR = "ENABLED";
    LUT4 i1_2_lut_3_lut_4_lut (.A(n8217), .B(s_en_r1), .C(\s_op_fifo.buf_3_.msbf_N_1174 [0]), 
         .D(\s_cfg[6] ), .Z(mclk_c_enable_243)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(121[15:49])
    defparam i1_2_lut_3_lut_4_lut.init = 16'h0080;
    LUT4 i1_2_lut_3_lut_4_lut_adj_196 (.A(n8217), .B(s_en_r1), .C(\s_op_fifo.buf_3_.msbf_N_1174 [2]), 
         .D(\s_cfg[6] ), .Z(mclk_c_enable_219)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(121[15:49])
    defparam i1_2_lut_3_lut_4_lut_adj_196.init = 16'h0080;
    LUT4 s_msbf_r_I_0_3_lut_4_lut (.A(n8217), .B(s_en_r1), .C(\s_desc_data[30] ), 
         .D(s_msbf_r), .Z(s_msbf)) /* synthesis lut_function=(A (B (C)+!B (D))+!A (D)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(121[15:49])
    defparam s_msbf_r_I_0_3_lut_4_lut.init = 16'hf780;
    LUT4 s_offs_r_7__I_136_2_lut_rep_82_3_lut_4_lut (.A(n8207), .B(s_offs_r_c[2]), 
         .C(s_offs_r_c[4]), .D(s_offs_r_c[3]), .Z(n8196)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam s_offs_r_7__I_136_2_lut_rep_82_3_lut_4_lut.init = 16'h8000;
    CCU2D add_191_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_ptr_data[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n6500), .S1(s_stop_N_1313[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_191_1.INIT0 = 16'hF000;
    defparam add_191_1.INIT1 = 16'h5555;
    defparam add_191_1.INJECT1_0 = "NO";
    defparam add_191_1.INJECT1_1 = "NO";
    LUT4 i3246_3_lut_4_lut (.A(n8207), .B(s_offs_r_c[2]), .C(n8221), .D(s_offs_r_c[3]), 
         .Z(n2494)) /* synthesis lut_function=(!(A (B (C+(D))+!B (C+!(D)))+!A (C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam i3246_3_lut_4_lut.init = 16'h0708;
    FD1S3IX s_cnt_r_0__214 (.D(bit_cnt_0__N_1203), .CK(mclk_c), .CD(n8221), 
            .Q(bit_cnt[0]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_0__214.GSR = "ENABLED";
    LUT4 i1_2_lut_3_lut_4_lut_adj_197 (.A(n8217), .B(s_en_r1), .C(\s_op_fifo.buf_3_.msbf_N_1174 [3]), 
         .D(\s_cfg[6] ), .Z(mclk_c_enable_207)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(121[15:49])
    defparam i1_2_lut_3_lut_4_lut_adj_197.init = 16'h0080;
    LUT4 i1_2_lut_3_lut_4_lut_adj_198 (.A(n8217), .B(s_en_r1), .C(\s_op_fifo.buf_3_.msbf_N_1174 [1]), 
         .D(\s_cfg[6] ), .Z(mclk_c_enable_231)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(121[15:49])
    defparam i1_2_lut_3_lut_4_lut_adj_198.init = 16'h0080;
    LUT4 i3290_2_lut_rep_136_3_lut_4_lut (.A(n8281), .B(bit_cnt[3]), .C(bit_cnt[5]), 
         .D(bit_cnt[4]), .Z(n8250)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3290_2_lut_rep_136_3_lut_4_lut.init = 16'hfffe;
    LUT4 s_offs_r_7__I_139_2_lut_rep_93_4_lut (.A(s_offs_r[0]), .B(n8217), 
         .C(s_en_r2), .D(\s_offs_r[1] ), .Z(n8207)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam s_offs_r_7__I_139_2_lut_rep_93_4_lut.init = 16'h8000;
    LUT4 i3296_2_lut_rep_115_3_lut_4_lut (.A(n8250), .B(bit_cnt[6]), .C(bit_cnt[8]), 
         .D(bit_cnt[7]), .Z(n8229)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3296_2_lut_rep_115_3_lut_4_lut.init = 16'hfffe;
    FD1P3AX s_en_r2_213 (.D(s_en_r1), .SP(tckn_stb), .CK(mclk_c), .Q(s_en_r2));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(98[3] 112[10])
    defparam s_en_r2_213.GSR = "ENABLED";
    FD1S3AY s_offs_r_7__232 (.D(n2480), .CK(mclk_c), .Q(s_offs_r_c[7]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_7__232.GSR = "ENABLED";
    FD1P3IX s_cnt_r_9__223 (.D(bit_cnt_9__N_1189), .SP(mclk_c_enable_137), 
            .CD(n8221), .CK(mclk_c), .Q(bit_cnt[9]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_9__223.GSR = "ENABLED";
    FD1P3IX s_cnt_r_8__222 (.D(bit_cnt_8__N_1250), .SP(mclk_c_enable_137), 
            .CD(n8221), .CK(mclk_c), .Q(bit_cnt[8]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_8__222.GSR = "ENABLED";
    FD1P3IX s_cnt_r_7__221 (.D(bit_cnt_7__N_1244), .SP(mclk_c_enable_137), 
            .CD(n8221), .CK(mclk_c), .Q(bit_cnt[7]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_7__221.GSR = "ENABLED";
    LUT4 i3243_3_lut_4_lut (.A(n8199), .B(s_offs_r_c[4]), .C(n8221), .D(s_offs_r_c[5]), 
         .Z(n2486)) /* synthesis lut_function=(!(A (B (C+(D))+!B (C+!(D)))+!A (C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam i3243_3_lut_4_lut.init = 16'h0708;
    LUT4 i3_3_lut_rep_98_4_lut (.A(tckn_stb), .B(n8222), .C(s_en_r2), 
         .D(s_offs_r[0]), .Z(n8212)) /* synthesis lut_function=(!((B+!(C (D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam i3_3_lut_rep_98_4_lut.init = 16'h2000;
    LUT4 bit_cnt_2__I_127_4_lut (.A(bit_cnt[2]), .B(s_pos_0_3__N_670[2]), 
         .C(n8858), .D(n8305), .Z(bit_cnt_2__N_1214)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_2__I_127_4_lut.init = 16'hcac5;
    FD1P3AX s_rx_en_r1_234 (.D(s_en_r2), .SP(tckn_stb), .CK(mclk_c), .Q(s_rx_en_r1));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(180[3] 190[10])
    defparam s_rx_en_r1_234.GSR = "ENABLED";
    FD1P3IX s_en_r0_211 (.D(n8228), .SP(mclk_c_enable_89), .CD(n4071), 
            .CK(mclk_c), .Q(s_en_r0)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=15, LSE_RCOL=41, LSE_LLINE=288, LSE_RLINE=288 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(98[3] 112[10])
    defparam s_en_r0_211.GSR = "ENABLED";
    FD1S3AX s_en_r1_212 (.D(s_en_r0), .CK(mclk_c), .Q(s_en_r1)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=15, LSE_RCOL=41, LSE_LLINE=288, LSE_RLINE=288 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(98[3] 112[10])
    defparam s_en_r1_212.GSR = "ENABLED";
    LUT4 i1_2_lut_3_lut_4_lut_adj_199 (.A(n8229), .B(bit_cnt[9]), .C(s_en_r0), 
         .D(s_stop_N_1312), .Z(mclk_c_enable_89)) /* synthesis lut_function=(!(A (C)+!A (B (C)+!B !((D)+!C)))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_199.init = 16'h1f0f;
    LUT4 i2365_2_lut_3_lut_4_lut (.A(n8229), .B(bit_cnt[9]), .C(s_en_r0), 
         .D(s_stop_N_1312), .Z(n4071)) /* synthesis lut_function=(!(A+(B+!(C (D))))) */ ;
    defparam i2365_2_lut_3_lut_4_lut.init = 16'h1000;
    FD1P3IX s_cnt_r_6__220 (.D(bit_cnt_6__N_1238), .SP(mclk_c_enable_137), 
            .CD(n8221), .CK(mclk_c), .Q(bit_cnt[6]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_6__220.GSR = "ENABLED";
    FD1P3IX s_cnt_r_5__219 (.D(bit_cnt_5__N_1232), .SP(mclk_c_enable_137), 
            .CD(n8221), .CK(mclk_c), .Q(bit_cnt[5]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_5__219.GSR = "ENABLED";
    FD1P3IX s_cnt_r_4__218 (.D(bit_cnt_4__N_1226), .SP(mclk_c_enable_137), 
            .CD(n8221), .CK(mclk_c), .Q(bit_cnt[4]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_4__218.GSR = "ENABLED";
    FD1P3IX s_cnt_r_3__217 (.D(bit_cnt_3__N_1220), .SP(mclk_c_enable_137), 
            .CD(n8221), .CK(mclk_c), .Q(bit_cnt[3]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_3__217.GSR = "ENABLED";
    LUT4 desc_rd_stb_I_0_2_lut_rep_97_3_lut_4_lut (.A(n8229), .B(bit_cnt[9]), 
         .C(s_en_r1), .D(tckn_stb), .Z(mclk_c_enable_118)) /* synthesis lut_function=(!(A+(B+!(C (D))))) */ ;
    defparam desc_rd_stb_I_0_2_lut_rep_97_3_lut_4_lut.init = 16'h1000;
    FD1P3IX s_cnt_r_2__216 (.D(bit_cnt_2__N_1214), .SP(mclk_c_enable_137), 
            .CD(n8221), .CK(mclk_c), .Q(bit_cnt[2]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_2__216.GSR = "ENABLED";
    LUT4 tx_shift_stb_I_0_3_lut (.A(s_en_r0), .B(tckn_stb), .C(s_en_r1), 
         .Z(s_tx_shift_stb)) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(194[18:51])
    defparam tx_shift_stb_I_0_3_lut.init = 16'hc8c8;
    FD1S3AY s_offs_r_1__226 (.D(n2503), .CK(mclk_c), .Q(\s_offs_r[1] ));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_1__226.GSR = "ENABLED";
    FD1S3AY s_offs_r_2__227 (.D(n2501), .CK(mclk_c), .Q(s_offs_r_c[2]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_2__227.GSR = "ENABLED";
    FD1S3AY s_offs_r_3__228 (.D(n2494), .CK(mclk_c), .Q(s_offs_r_c[3]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_3__228.GSR = "ENABLED";
    FD1S3AY s_offs_r_4__229 (.D(n2490), .CK(mclk_c), .Q(s_offs_r_c[4]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_4__229.GSR = "ENABLED";
    FD1S3AY s_offs_r_5__230 (.D(n2486), .CK(mclk_c), .Q(s_offs_r_c[5]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam s_offs_r_5__230.GSR = "ENABLED";
    LUT4 i4855_2_lut (.A(s_ptr_data[8]), .B(s_offs_r[0]), .Z(s_desc_addr[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i4855_2_lut.init = 16'h6666;
    LUT4 i3288_2_lut_rep_144_3_lut_4_lut (.A(n8305), .B(bit_cnt[2]), .C(bit_cnt[4]), 
         .D(bit_cnt[3]), .Z(n8258)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3288_2_lut_rep_144_3_lut_4_lut.init = 16'hfffe;
    LUT4 i2_2_lut_rep_103_3_lut_4_lut (.A(n8235), .B(bit_cnt[8]), .C(tckn_stb), 
         .D(bit_cnt[9]), .Z(n8217)) /* synthesis lut_function=(!(A+(B+((D)+!C)))) */ ;
    defparam i2_2_lut_rep_103_3_lut_4_lut.init = 16'h0010;
    FD1P3IX s_cnt_r_1__215 (.D(bit_cnt_1__N_1208), .SP(mclk_c_enable_137), 
            .CD(n8221), .CK(mclk_c), .Q(bit_cnt[1]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(131[4] 141[11])
    defparam s_cnt_r_1__215.GSR = "ENABLED";
    LUT4 i3402_2_lut_rep_108_3_lut_4_lut (.A(n8243), .B(bit_cnt[7]), .C(bit_cnt[9]), 
         .D(bit_cnt[8]), .Z(n8222)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3402_2_lut_rep_108_3_lut_4_lut.init = 16'hfffe;
    LUT4 bit_cnt_0__I_125_4_lut (.A(bit_cnt[0]), .B(s_pos_0_3__N_670[0]), 
         .C(n8858), .D(s_cnt_stb), .Z(bit_cnt_0__N_1203)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_0__I_125_4_lut.init = 16'hc5ca;
    LUT4 tckn_stb_I_0_2_lut (.A(tckn_stb), .B(s_en_r1), .Z(s_cnt_stb)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(123[15:35])
    defparam tckn_stb_I_0_2_lut.init = 16'h8888;
    LUT4 i3239_4_lut (.A(s_offs_r_c[7]), .B(n8221), .C(n8194), .D(s_offs_r_c[6]), 
         .Z(n2480)) /* synthesis lut_function=(!(A (B+(C (D)))+!A (B+!(C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(158[4] 166[11])
    defparam i3239_4_lut.init = 16'h1222;
    LUT4 i3240_3_lut_4_lut (.A(n8196), .B(s_offs_r_c[5]), .C(n8221), .D(s_offs_r_c[6]), 
         .Z(n2482)) /* synthesis lut_function=(!(A (B (C+(D))+!B (C+!(D)))+!A (C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam i3240_3_lut_4_lut.init = 16'h0708;
    LUT4 bit_cnt_9__I_116_4_lut (.A(bit_cnt[9]), .B(\s_desc_data[9] ), .C(n8858), 
         .D(n8229), .Z(bit_cnt_9__N_1189)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_9__I_116_4_lut.init = 16'hcac5;
    LUT4 bit_cnt_8__I_133_4_lut (.A(bit_cnt[8]), .B(\s_desc_data[8] ), .C(n8858), 
         .D(n8235), .Z(bit_cnt_8__N_1250)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_8__I_133_4_lut.init = 16'hcac5;
    LUT4 bit_cnt_7__I_132_4_lut (.A(bit_cnt[7]), .B(\s_desc_data[7] ), .C(n8858), 
         .D(n8243), .Z(bit_cnt_7__N_1244)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_7__I_132_4_lut.init = 16'hcac5;
    LUT4 i3282_2_lut_rep_191 (.A(bit_cnt[0]), .B(bit_cnt[1]), .Z(n8305)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i3282_2_lut_rep_191.init = 16'heeee;
    LUT4 i3284_2_lut_rep_167_3_lut (.A(bit_cnt[0]), .B(bit_cnt[1]), .C(bit_cnt[2]), 
         .Z(n8281)) /* synthesis lut_function=(A+(B+(C))) */ ;
    defparam i3284_2_lut_rep_167_3_lut.init = 16'hfefe;
    LUT4 i3286_2_lut_rep_155_3_lut_4_lut (.A(bit_cnt[0]), .B(bit_cnt[1]), 
         .C(bit_cnt[3]), .D(bit_cnt[2]), .Z(n8269)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3286_2_lut_rep_155_3_lut_4_lut.init = 16'hfffe;
    LUT4 s_offs_r_7__I_138_2_lut_rep_89_3_lut (.A(n8212), .B(\s_offs_r[1] ), 
         .C(s_offs_r_c[2]), .Z(n8203)) /* synthesis lut_function=(A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam s_offs_r_7__I_138_2_lut_rep_89_3_lut.init = 16'h8080;
    LUT4 s_offs_r_7__I_135_2_lut_rep_80_3_lut_4_lut (.A(n8203), .B(s_offs_r_c[3]), 
         .C(s_offs_r_c[5]), .D(s_offs_r_c[4]), .Z(n8194)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam s_offs_r_7__I_135_2_lut_rep_80_3_lut_4_lut.init = 16'h8000;
    LUT4 s_offs_r_7__I_137_2_lut_rep_85_3_lut_4_lut (.A(n8212), .B(\s_offs_r[1] ), 
         .C(s_offs_r_c[3]), .D(s_offs_r_c[2]), .Z(n8199)) /* synthesis lut_function=(A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam s_offs_r_7__I_137_2_lut_rep_85_3_lut_4_lut.init = 16'h8000;
    LUT4 bit_cnt_6__I_131_4_lut (.A(bit_cnt[6]), .B(\s_desc_data[6] ), .C(n8858), 
         .D(n8250), .Z(bit_cnt_6__N_1238)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_6__I_131_4_lut.init = 16'hcac5;
    LUT4 i3256_3_lut_4_lut (.A(n8212), .B(\s_offs_r[1] ), .C(n8221), .D(s_offs_r_c[2]), 
         .Z(n2501)) /* synthesis lut_function=(!(A (B (C+(D))+!B (C+!(D)))+!A (C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam i3256_3_lut_4_lut.init = 16'h0708;
    LUT4 bit_cnt_5__I_130_4_lut (.A(bit_cnt[5]), .B(\s_desc_data[5] ), .C(n8858), 
         .D(n8258), .Z(bit_cnt_5__N_1232)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_5__I_130_4_lut.init = 16'hcac5;
    LUT4 i3245_3_lut_4_lut (.A(n8203), .B(s_offs_r_c[3]), .C(n8221), .D(s_offs_r_c[4]), 
         .Z(n2490)) /* synthesis lut_function=(!(A (B (C+(D))+!B (C+!(D)))+!A (C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(167[10:28])
    defparam i3245_3_lut_4_lut.init = 16'h0708;
    CCU2D s_offs_r_7__I_0_8 (.A0(s_stop_N_1313[1]), .B0(\s_offs_r[1] ), 
          .C0(s_stop_N_1313[0]), .D0(s_offs_r[0]), .A1(GND_net), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n6406), .S1(s_stop_N_1312));
    defparam s_offs_r_7__I_0_8.INIT0 = 16'h9009;
    defparam s_offs_r_7__I_0_8.INIT1 = 16'hFFFF;
    defparam s_offs_r_7__I_0_8.INJECT1_0 = "YES";
    defparam s_offs_r_7__I_0_8.INJECT1_1 = "NO";
    CCU2D s_offs_r_7__I_0_7 (.A0(s_stop_N_1313[5]), .B0(s_offs_r_c[5]), 
          .C0(s_stop_N_1313[4]), .D0(s_offs_r_c[4]), .A1(s_stop_N_1313[3]), 
          .B1(s_offs_r_c[3]), .C1(s_stop_N_1313[2]), .D1(s_offs_r_c[2]), 
          .CIN(n6405), .COUT(n6406));
    defparam s_offs_r_7__I_0_7.INIT0 = 16'h9009;
    defparam s_offs_r_7__I_0_7.INIT1 = 16'h9009;
    defparam s_offs_r_7__I_0_7.INJECT1_0 = "YES";
    defparam s_offs_r_7__I_0_7.INJECT1_1 = "YES";
    CCU2D s_offs_r_7__I_0_0 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_stop_N_1313[7]), .B1(s_offs_r_c[7]), .C1(s_stop_N_1313[6]), 
          .D1(s_offs_r_c[6]), .COUT(n6405));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(174[22:55])
    defparam s_offs_r_7__I_0_0.INIT0 = 16'hF000;
    defparam s_offs_r_7__I_0_0.INIT1 = 16'h9009;
    defparam s_offs_r_7__I_0_0.INJECT1_0 = "NO";
    defparam s_offs_r_7__I_0_0.INJECT1_1 = "YES";
    CCU2D desc_base_7__I_0_8 (.A0(s_ptr_data[14]), .B0(s_offs_r_c[6]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ptr_data[15]), .B1(s_offs_r_c[7]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6507), .S0(s_desc_addr[7]), .S1(s_desc_addr[8]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(202[32:51])
    defparam desc_base_7__I_0_8.INIT0 = 16'h5666;
    defparam desc_base_7__I_0_8.INIT1 = 16'h5666;
    defparam desc_base_7__I_0_8.INJECT1_0 = "NO";
    defparam desc_base_7__I_0_8.INJECT1_1 = "NO";
    LUT4 s_rx_en_r1_I_0_273_2_lut_rep_212 (.A(s_rx_en_r1), .B(tckn_stb), 
         .Z(mclk_c_enable_204)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam s_rx_en_r1_I_0_273_2_lut_rep_212.init = 16'h8888;
    LUT4 s_count_r_0__I_0_502_2_lut_3_lut (.A(s_rx_en_r1), .B(tckn_stb), 
         .C(\s_count_r[0] ), .Z(s_count_r_0__N_893)) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam s_count_r_0__I_0_502_2_lut_3_lut.init = 16'h7878;
    LUT4 bit_pos_0__I_0_2_lut_3_lut (.A(s_rx_en_r1), .B(tckn_stb), .C(\bit_pos[0] ), 
         .Z(bit_pos_0__N_816)) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam bit_pos_0__I_0_2_lut_3_lut.init = 16'h7878;
    CCU2D desc_base_7__I_0_6 (.A0(s_ptr_data[12]), .B0(s_offs_r_c[4]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ptr_data[13]), .B1(s_offs_r_c[5]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6506), .COUT(n6507), .S0(s_desc_addr[5]), 
          .S1(s_desc_addr[6]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(202[32:51])
    defparam desc_base_7__I_0_6.INIT0 = 16'h5666;
    defparam desc_base_7__I_0_6.INIT1 = 16'h5666;
    defparam desc_base_7__I_0_6.INJECT1_0 = "NO";
    defparam desc_base_7__I_0_6.INJECT1_1 = "NO";
    LUT4 i3294_2_lut_rep_121_3_lut_4_lut (.A(n8258), .B(bit_cnt[5]), .C(bit_cnt[7]), 
         .D(bit_cnt[6]), .Z(n8235)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3294_2_lut_rep_121_3_lut_4_lut.init = 16'hfffe;
    LUT4 i2_3_lut_rep_168_4_lut (.A(s_rx_en_r1), .B(tckn_stb), .C(\bit_pos[0] ), 
         .D(\bit_pos[1] ), .Z(n8282)) /* synthesis lut_function=(!(((C+(D))+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam i2_3_lut_rep_168_4_lut.init = 16'h0008;
    LUT4 shift_stb_I_0_2_lut_rep_175_3_lut (.A(s_rx_en_r1), .B(tckn_stb), 
         .C(\s_count_r[0] ), .Z(n8289)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam shift_stb_I_0_2_lut_rep_175_3_lut.init = 16'h0808;
    CCU2D desc_base_7__I_0_4 (.A0(s_ptr_data[10]), .B0(s_offs_r_c[2]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ptr_data[11]), .B1(s_offs_r_c[3]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6505), .COUT(n6506), .S0(s_desc_addr[3]), 
          .S1(s_desc_addr[4]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(202[32:51])
    defparam desc_base_7__I_0_4.INIT0 = 16'h5666;
    defparam desc_base_7__I_0_4.INIT1 = 16'h5666;
    defparam desc_base_7__I_0_4.INJECT1_0 = "NO";
    defparam desc_base_7__I_0_4.INJECT1_1 = "NO";
    LUT4 s_count_r_9__I_110_2_lut_rep_158_3_lut_4_lut (.A(s_rx_en_r1), .B(tckn_stb), 
         .C(\s_count_r[1] ), .D(\s_count_r[0] ), .Z(n8272)) /* synthesis lut_function=(!(((C+(D))+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam s_count_r_9__I_110_2_lut_rep_158_3_lut_4_lut.init = 16'h0008;
    LUT4 s_count_r_1__I_0_504_2_lut_3_lut_4_lut (.A(s_rx_en_r1), .B(tckn_stb), 
         .C(\s_count_r[1] ), .D(\s_count_r[0] ), .Z(s_count_r_1__N_897)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (C))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam s_count_r_1__I_0_504_2_lut_3_lut_4_lut.init = 16'hf078;
    LUT4 bit_pos_3__I_93_3_lut_rep_180_4_lut (.A(s_rx_en_r1), .B(tckn_stb), 
         .C(\bit_pos[0] ), .D(op_msbf), .Z(n8294)) /* synthesis lut_function=(!(((C (D)+!C !(D))+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(199[18:41])
    defparam bit_pos_3__I_93_3_lut_rep_180_4_lut.init = 16'h0880;
    CCU2D desc_base_7__I_0_2 (.A0(s_ptr_data[8]), .B0(s_offs_r[0]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ptr_data[9]), .B1(\s_offs_r[1] ), .C1(GND_net), 
          .D1(GND_net), .COUT(n6505), .S1(s_desc_addr[2]));   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(202[32:51])
    defparam desc_base_7__I_0_2.INIT0 = 16'h7000;
    defparam desc_base_7__I_0_2.INIT1 = 16'h5666;
    defparam desc_base_7__I_0_2.INJECT1_0 = "NO";
    defparam desc_base_7__I_0_2.INJECT1_1 = "NO";
    LUT4 desc_rd_stb_I_0_2_lut_rep_229 (.A(n8229), .B(bit_cnt[9]), .C(s_en_r1), 
         .D(tckn_stb), .Z(n8858)) /* synthesis lut_function=(!(A+(B+!(C (D))))) */ ;
    defparam desc_rd_stb_I_0_2_lut_rep_229.init = 16'h1000;
    LUT4 i3292_2_lut_rep_129_3_lut_4_lut (.A(n8269), .B(bit_cnt[4]), .C(bit_cnt[6]), 
         .D(bit_cnt[5]), .Z(n8243)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3292_2_lut_rep_129_3_lut_4_lut.init = 16'hfffe;
    LUT4 bit_cnt_1__I_126_4_lut (.A(bit_cnt[1]), .B(s_pos_0_3__N_670[1]), 
         .C(n8858), .D(bit_cnt[0]), .Z(bit_cnt_1__N_1208)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(134[5] 140[12])
    defparam bit_cnt_1__I_126_4_lut.init = 16'hcac5;
    LUT4 s_offs_inc_I_141_4_lut (.A(n8217), .B(s_en_r2), .C(n13), .D(n14), 
         .Z(s_offs_inc)) /* synthesis lut_function=(A (B)+!A !((C+(D))+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_shift_ctrl.vhdl(148[16:64])
    defparam s_offs_inc_I_141_4_lut.init = 16'h888c;
    LUT4 i5_4_lut (.A(s_offs_r[0]), .B(s_offs_r_c[6]), .C(s_offs_r_c[4]), 
         .D(s_offs_r_c[7]), .Z(n13)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i5_4_lut.init = 16'hfffe;
    LUT4 i6_4_lut (.A(s_offs_r_c[2]), .B(s_offs_r_c[5]), .C(s_offs_r_c[3]), 
         .D(\s_offs_r[1] ), .Z(n14)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i6_4_lut.init = 16'hfffe;
    
endmodule
//
// Verilog Description of module \jtag_enh_rx(16,10,10,4) 
//

module \jtag_enh_rx(16,10,10,4)  (s_count_r_0__N_893, mclk_c, mclk_c_enable_118, 
            bit_pos_0__N_816, s_pos_0_3__N_670, \s_op_fifo.buf_3_.msbf_N_1174 , 
            \s_desc_data[4] , \s_desc_data[5] , \s_desc_data[6] , \s_desc_data[7] , 
            \s_desc_data[8] , \s_desc_data[9] , op_msbf, s_rx_mem_wr_stb, 
            mclk_c_enable_243, \s_desc_data[31] , mclk_c_enable_207, n8289, 
            s_count_r, n8272, mclk_c_enable_231, mclk_c_enable_219, 
            s_rx_mem_addr, mclk_c_enable_204, \s_count_r[0] , n8294, 
            bit_pos, \bit_pos[0] , n8850, s_rx_mem_din, s_mtdo, n8282, 
            \s_desc_data[29] , GND_net, \s_desc_data[27] , \s_desc_data[28] , 
            \s_desc_data[25] , \s_desc_data[26] , \s_desc_data[23] , \s_desc_data[24] , 
            \s_desc_data[21] , \s_desc_data[22] , \s_desc_data[20] , n8858, 
            s_count_r_1__N_897);
    input s_count_r_0__N_893;
    input mclk_c;
    input mclk_c_enable_118;
    input bit_pos_0__N_816;
    input [3:0]s_pos_0_3__N_670;
    output [3:0]\s_op_fifo.buf_3_.msbf_N_1174 ;
    input \s_desc_data[4] ;
    input \s_desc_data[5] ;
    input \s_desc_data[6] ;
    input \s_desc_data[7] ;
    input \s_desc_data[8] ;
    input \s_desc_data[9] ;
    output op_msbf;
    output s_rx_mem_wr_stb;
    input mclk_c_enable_243;
    input \s_desc_data[31] ;
    input mclk_c_enable_207;
    input n8289;
    output [9:0]s_count_r;
    input n8272;
    input mclk_c_enable_231;
    input mclk_c_enable_219;
    output [9:0]s_rx_mem_addr;
    input mclk_c_enable_204;
    output \s_count_r[0] ;
    input n8294;
    output [3:0]bit_pos;
    output \bit_pos[0] ;
    input n8850;
    output [15:0]s_rx_mem_din;
    input s_mtdo;
    input n8282;
    input \s_desc_data[29] ;
    input GND_net;
    input \s_desc_data[27] ;
    input \s_desc_data[28] ;
    input \s_desc_data[25] ;
    input \s_desc_data[26] ;
    input \s_desc_data[23] ;
    input \s_desc_data[24] ;
    input \s_desc_data[21] ;
    input \s_desc_data[22] ;
    input \s_desc_data[20] ;
    input n8858;
    input s_count_r_1__N_897;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    wire [9:0]\s_op0.addr ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(138[9:14])
    wire [9:0]op_addr_9__N_817;
    
    wire busy;
    wire [9:0]op_addr;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(77[2:9])
    
    wire s_addr_r1_2__N_874, fifo_get;
    wire [9:0]n2033;
    
    wire s_addr_r1_1__N_876, n7466, n7467;
    wire [1:0]\s_op_fifo.tail ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire bit_pos_3__N_804;
    wire [3:0]op_pos;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(79[2:8])
    wire [3:0]n2034;
    
    wire bit_pos_1__N_814, n7472, n7473;
    wire [9:0]op_len;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(78[2:8])
    wire [9:0]n2032;
    
    wire s_addr_r1_0__N_878, start;
    wire [9:0]\s_op_fifo.buf[0].addr ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    wire [9:0]\s_op_fifo.buf[1].addr ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n7463;
    wire [9:0]\s_op_fifo.buf[2].addr ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    wire [9:0]\s_op_fifo.buf[3].addr ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n7451, n7450;
    wire [3:0]\s_op_fifo.buf[2].pos ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    wire [3:0]\s_op_fifo.buf[3].pos ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n7448;
    wire [3:0]\s_op_fifo.buf[0].pos ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    wire [3:0]\s_op_fifo.buf[1].pos ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n7447, n7442, n7441;
    wire [9:0]op_len_9__N_827;
    wire [9:0]\s_op_fifo.buf[2].len ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    wire [9:0]\s_op_fifo.buf[3].len ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n7390;
    wire [9:0]\s_op_fifo.buf[0].len ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    wire [9:0]\s_op_fifo.buf[1].len ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n7389, n7318;
    wire [9:0]s_addr_r1;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(115[9:18])
    
    wire n8205, s_addr_r1_9__N_842, n7372, n7371, \s_op_fifo.buf_2_.msbf , 
        \s_op_fifo.buf_3_.msbf , n7363, \s_op_fifo.buf_0_.msbf , \s_op_fifo.buf_1_.msbf , 
        n7362, n7360, n8309, mem_wr_N_1153, \s_rx_op.msbf , n8219, 
        n8209, s_addr_r1_6__N_866, n8214, \s_op_fifo.empty ;
    wire [9:0]s_count_r_c;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(121[9:18])
    
    wire n8253, n7359, n7357, n7356, n7354;
    wire [1:0]n1;
    
    wire n8324, n7353, n7351, n7350, n7348, n7347, s_count_r_3__N_905, 
        n7317;
    wire [3:0]bit_pos_c;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(76[2:9])
    
    wire n8266, n8225, n8257, n8275, n8226, n8215, s_addr_r1_5__N_868, 
        s_addr_r1_8__N_862, n6, n6644, mclk_c_enable_92, mem_dout_15__N_799, 
        n7345, n7344, n8300, n8296, mclk_c_enable_258, n7342, n7341, 
        n7339, mem_dout_15__N_739, n7338, n8297, mclk_c_enable_254, 
        mem_dout_15__N_755, n8299, mclk_c_enable_250, mem_dout_15__N_771, 
        n8301, mclk_c_enable_257, mem_dout_15__N_743, mclk_c_enable_253, 
        n7336, n7335, n7333, n7332, mem_dout_15__N_759, n7330, n7329, 
        mclk_c_enable_249, mem_dout_15__N_775, n7327, n7326, n7324, 
        n7323, n8232, s_count_r_9__N_880, n8302, mclk_c_enable_256, 
        n8230, mem_dout_15__N_747, mclk_c_enable_252, n7321, s_addr_r1_4__N_870, 
        n7320, mem_dout_15__N_763, mclk_c_enable_248, mem_dout_15__N_779, 
        n8306, mclk_c_enable_246, mem_dout_15__N_787, mclk_c_enable_245, 
        mem_dout_15__N_791, mclk_c_enable_244, mem_dout_15__N_795, n7476, 
        n7475, n8234, s_addr_r1_3__N_872, n8237, s_count_r_8__N_925, 
        n8261, n8241;
    wire [1:0]\s_op_fifo.tail_1__N_1054 ;
    
    wire n8328, n8329, n8304, mclk_c_enable_255, mclk_c_enable_247, 
        mem_dout_15__N_783, mclk_c_enable_251, mem_dout_15__N_767, mem_dout_15__N_751, 
        \s_rx_op.msbf_N_1187 , n8245, s_count_r_7__N_921, n6490, n6489, 
        n6488, n6487, n6486;
    wire [3:0]op_pos_3__N_837;
    wire [1:0]\s_op_fifo.head ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(136[9:18])
    
    wire n4_adj_1591, s_count_r_6__N_917, s_count_r_5__N_913, s_addr_r1_7__N_864, 
        s_count_r_4__N_909, n7464;
    
    LUT4 \s_op0.addr_9__I_0_i4_3_lut  (.A(\s_op0.addr [3]), .B(op_addr_9__N_817[3]), 
         .C(busy), .Z(op_addr[3])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i4_3_lut .init = 16'hcaca;
    PFUMX mux_511_i3 (.BLUT(s_addr_r1_2__N_874), .ALUT(op_addr[2]), .C0(fifo_get), 
          .Z(n2033[2]));
    PFUMX mux_511_i2 (.BLUT(s_addr_r1_1__N_876), .ALUT(op_addr[1]), .C0(fifo_get), 
          .Z(n2033[1]));
    PFUMX i5805 (.BLUT(n7466), .ALUT(n7467), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[7]));
    PFUMX mux_512_i4 (.BLUT(bit_pos_3__N_804), .ALUT(op_pos[3]), .C0(fifo_get), 
          .Z(n2034[3]));
    PFUMX mux_512_i2 (.BLUT(bit_pos_1__N_814), .ALUT(op_pos[1]), .C0(fifo_get), 
          .Z(n2034[1]));
    PFUMX i5811 (.BLUT(n7472), .ALUT(n7473), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[6]));
    PFUMX mux_510_i1 (.BLUT(s_count_r_0__N_893), .ALUT(op_len[0]), .C0(fifo_get), 
          .Z(n2032[0]));
    PFUMX mux_511_i1 (.BLUT(s_addr_r1_0__N_878), .ALUT(op_addr[0]), .C0(fifo_get), 
          .Z(n2033[0]));
    FD1S3IX s_start_r_394 (.D(mclk_c_enable_118), .CK(mclk_c), .CD(busy), 
            .Q(start));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(172[3] 176[10])
    defparam s_start_r_394.GSR = "ENABLED";
    PFUMX mux_512_i1 (.BLUT(bit_pos_0__N_816), .ALUT(op_pos[0]), .C0(fifo_get), 
          .Z(n2034[0]));
    LUT4 i5800_3_lut (.A(\s_op_fifo.buf[0].addr [8]), .B(\s_op_fifo.buf[1].addr [8]), 
         .C(\s_op_fifo.tail [0]), .Z(n7463)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5800_3_lut.init = 16'hcaca;
    LUT4 \s_op0.addr_9__I_0_i5_3_lut  (.A(\s_op0.addr [4]), .B(op_addr_9__N_817[4]), 
         .C(busy), .Z(op_addr[4])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i5_3_lut .init = 16'hcaca;
    LUT4 i5788_3_lut (.A(\s_op_fifo.buf[2].addr [9]), .B(\s_op_fifo.buf[3].addr [9]), 
         .C(\s_op_fifo.tail [0]), .Z(n7451)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5788_3_lut.init = 16'hcaca;
    LUT4 i5787_3_lut (.A(\s_op_fifo.buf[0].addr [9]), .B(\s_op_fifo.buf[1].addr [9]), 
         .C(\s_op_fifo.tail [0]), .Z(n7450)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5787_3_lut.init = 16'hcaca;
    LUT4 i5785_3_lut (.A(\s_op_fifo.buf[2].pos [1]), .B(\s_op_fifo.buf[3].pos [1]), 
         .C(\s_op_fifo.tail [0]), .Z(n7448)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5785_3_lut.init = 16'hcaca;
    LUT4 i5784_3_lut (.A(\s_op_fifo.buf[0].pos [1]), .B(\s_op_fifo.buf[1].pos [1]), 
         .C(\s_op_fifo.tail [0]), .Z(n7447)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5784_3_lut.init = 16'hcaca;
    LUT4 i5779_3_lut (.A(\s_op_fifo.buf[2].pos [2]), .B(\s_op_fifo.buf[3].pos [2]), 
         .C(\s_op_fifo.tail [0]), .Z(n7442)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5779_3_lut.init = 16'hcaca;
    LUT4 \s_op0.addr_9__I_0_i6_3_lut  (.A(\s_op0.addr [5]), .B(op_addr_9__N_817[5]), 
         .C(busy), .Z(op_addr[5])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i6_3_lut .init = 16'hcaca;
    LUT4 \s_op0.addr_9__I_0_i7_3_lut  (.A(\s_op0.addr [6]), .B(op_addr_9__N_817[6]), 
         .C(busy), .Z(op_addr[6])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i7_3_lut .init = 16'hcaca;
    LUT4 i5778_3_lut (.A(\s_op_fifo.buf[0].pos [2]), .B(\s_op_fifo.buf[1].pos [2]), 
         .C(\s_op_fifo.tail [0]), .Z(n7441)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5778_3_lut.init = 16'hcaca;
    LUT4 \s_op0.addr_9__I_0_i8_3_lut  (.A(\s_op0.addr [7]), .B(op_addr_9__N_817[7]), 
         .C(busy), .Z(op_addr[7])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i8_3_lut .init = 16'hcaca;
    LUT4 vec_len_9__I_0_i2_3_lut (.A(s_pos_0_3__N_670[1]), .B(op_len_9__N_827[1]), 
         .C(busy), .Z(op_len[1])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_len_9__I_0_i2_3_lut.init = 16'hcaca;
    FD1P3AY \s_op_fifo.head_FSM_i0_i0  (.D(\s_op_fifo.buf_3_.msbf_N_1174 [3]), 
            .SP(mclk_c_enable_118), .CK(mclk_c), .Q(\s_op_fifo.buf_3_.msbf_N_1174 [0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam \s_op_fifo.head_FSM_i0_i0 .GSR = "ENABLED";
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
    LUT4 i5727_3_lut (.A(\s_op_fifo.buf[2].len [1]), .B(\s_op_fifo.buf[3].len [1]), 
         .C(\s_op_fifo.tail [0]), .Z(n7390)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5727_3_lut.init = 16'hcaca;
    LUT4 i5726_3_lut (.A(\s_op_fifo.buf[0].len [1]), .B(\s_op_fifo.buf[1].len [1]), 
         .C(\s_op_fifo.tail [0]), .Z(n7389)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5726_3_lut.init = 16'hcaca;
    LUT4 i5655_3_lut (.A(\s_op_fifo.buf[2].addr [4]), .B(\s_op_fifo.buf[3].addr [4]), 
         .C(\s_op_fifo.tail [0]), .Z(n7318)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5655_3_lut.init = 16'hcaca;
    LUT4 \s_op0.addr_9__I_0_i10_3_lut  (.A(\s_op0.addr [9]), .B(op_addr_9__N_817[9]), 
         .C(busy), .Z(op_addr[9])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i10_3_lut .init = 16'hcaca;
    LUT4 s_addr_r1_9__I_0_4_lut (.A(s_addr_r1[9]), .B(op_msbf), .C(n8205), 
         .D(s_addr_r1[8]), .Z(s_addr_r1_9__N_842)) /* synthesis lut_function=(A (B ((D)+!C)+!B !(C (D)))+!A !(B ((D)+!C)+!B !(C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(286[22:41])
    defparam s_addr_r1_9__I_0_4_lut.init = 16'h9a6a;
    LUT4 i5709_3_lut (.A(\s_op_fifo.buf[2].pos [3]), .B(\s_op_fifo.buf[3].pos [3]), 
         .C(\s_op_fifo.tail [0]), .Z(n7372)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5709_3_lut.init = 16'hcaca;
    LUT4 i5708_3_lut (.A(\s_op_fifo.buf[0].pos [3]), .B(\s_op_fifo.buf[1].pos [3]), 
         .C(\s_op_fifo.tail [0]), .Z(n7371)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5708_3_lut.init = 16'hcaca;
    LUT4 i5700_3_lut (.A(\s_op_fifo.buf_2_.msbf ), .B(\s_op_fifo.buf_3_.msbf ), 
         .C(\s_op_fifo.tail [0]), .Z(n7363)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5700_3_lut.init = 16'hcaca;
    LUT4 i5699_3_lut (.A(\s_op_fifo.buf_0_.msbf ), .B(\s_op_fifo.buf_1_.msbf ), 
         .C(\s_op_fifo.tail [0]), .Z(n7362)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5699_3_lut.init = 16'hcaca;
    LUT4 i5697_3_lut (.A(\s_op_fifo.buf[2].pos [0]), .B(\s_op_fifo.buf[3].pos [0]), 
         .C(\s_op_fifo.tail [0]), .Z(n7360)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5697_3_lut.init = 16'hcaca;
    FD1S3IX s_store_stb_441 (.D(mem_wr_N_1153), .CK(mclk_c), .CD(n8309), 
            .Q(s_rx_mem_wr_stb));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(317[3] 321[10])
    defparam s_store_stb_441.GSR = "ENABLED";
    FD1P3AX s_rx_msbf_415 (.D(\s_rx_op.msbf ), .SP(fifo_get), .CK(mclk_c), 
            .Q(op_msbf));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(218[3] 222[10])
    defparam s_rx_msbf_415.GSR = "ENABLED";
    FD1P3AX \s_op_fifo.buf_0..msbf_410  (.D(\s_desc_data[31] ), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf_0_.msbf ));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..msbf_410 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i0  (.D(s_pos_0_3__N_670[0]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i0 .GSR = "DISABLED";
    LUT4 s_addr_r1_9__I_96_3_lut_rep_95_4_lut (.A(op_msbf), .B(n8219), .C(s_addr_r1[5]), 
         .D(s_addr_r1[6]), .Z(n8209)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_96_3_lut_rep_95_4_lut.init = 16'h4008;
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i0  (.D(\s_op0.addr [0]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i0 .GSR = "DISABLED";
    LUT4 s_addr_r1_6__I_0_2_lut_4_lut (.A(op_msbf), .B(n8219), .C(s_addr_r1[5]), 
         .D(s_addr_r1[6]), .Z(s_addr_r1_6__N_866)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_6__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 start_I_0_4_lut (.A(start), .B(n8214), .C(\s_op_fifo.empty ), 
         .D(busy), .Z(fifo_get)) /* synthesis lut_function=(A+!((C+!(D))+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(180[16:75])
    defparam start_I_0_4_lut.init = 16'haeaa;
    LUT4 s_count_r_9__I_108_2_lut_rep_139_3_lut_4_lut (.A(n8289), .B(s_count_r[1]), 
         .C(s_count_r_c[3]), .D(s_count_r_c[2]), .Z(n8253)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_108_2_lut_rep_139_3_lut_4_lut.init = 16'h0002;
    LUT4 s_count_r_2__bdd_4_lut (.A(s_count_r_c[2]), .B(n8272), .C(op_len[2]), 
         .D(fifo_get), .Z(n2032[2])) /* synthesis lut_function=(A (B (C (D))+!B (C+!(D)))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam s_count_r_2__bdd_4_lut.init = 16'hf066;
    LUT4 i5696_3_lut (.A(\s_op_fifo.buf[0].pos [0]), .B(\s_op_fifo.buf[1].pos [0]), 
         .C(\s_op_fifo.tail [0]), .Z(n7359)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5696_3_lut.init = 16'hcaca;
    FD1P3AX \s_op_fifo.buf_1..msbf_406  (.D(\s_desc_data[31] ), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf_1_.msbf ));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..msbf_406 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i0  (.D(s_pos_0_3__N_670[0]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i0 .GSR = "DISABLED";
    LUT4 i5694_3_lut (.A(\s_op_fifo.buf[2].addr [0]), .B(\s_op_fifo.buf[3].addr [0]), 
         .C(\s_op_fifo.tail [0]), .Z(n7357)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5694_3_lut.init = 16'hcaca;
    LUT4 i5693_3_lut (.A(\s_op_fifo.buf[0].addr [0]), .B(\s_op_fifo.buf[1].addr [0]), 
         .C(\s_op_fifo.tail [0]), .Z(n7356)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5693_3_lut.init = 16'hcaca;
    FD1P3AX \s_op_fifo.buf_1..len_i0_i0  (.D(s_pos_0_3__N_670[0]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i0  (.D(s_pos_0_3__N_670[0]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i0 .GSR = "DISABLED";
    LUT4 i5691_3_lut (.A(\s_op_fifo.buf[2].len [0]), .B(\s_op_fifo.buf[3].len [0]), 
         .C(\s_op_fifo.tail [0]), .Z(n7354)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5691_3_lut.init = 16'hcaca;
    FD1S3AX \s_op_fifo.tail_i0_i0  (.D(n1[0]), .CK(mclk_c), .Q(\s_op_fifo.tail [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.tail_i0_i0 .GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i0 (.D(s_addr_r1[0]), .SP(mclk_c_enable_204), .CK(mclk_c), 
            .Q(s_rx_mem_addr[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i0.GSR = "ENABLED";
    FD1P3AX \s_op_fifo.buf_3..pos_i0_i0  (.D(n8324), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].pos [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..pos_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..msbf_402  (.D(\s_desc_data[31] ), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf_2_.msbf ));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..msbf_402 .GSR = "DISABLED";
    LUT4 i5690_3_lut (.A(\s_op_fifo.buf[0].len [0]), .B(\s_op_fifo.buf[1].len [0]), 
         .C(\s_op_fifo.tail [0]), .Z(n7353)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5690_3_lut.init = 16'hcaca;
    LUT4 i5688_3_lut (.A(\s_op_fifo.buf[2].len [2]), .B(\s_op_fifo.buf[3].len [2]), 
         .C(\s_op_fifo.tail [0]), .Z(n7351)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5688_3_lut.init = 16'hcaca;
    LUT4 i5687_3_lut (.A(\s_op_fifo.buf[0].len [2]), .B(\s_op_fifo.buf[1].len [2]), 
         .C(\s_op_fifo.tail [0]), .Z(n7350)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5687_3_lut.init = 16'hcaca;
    LUT4 i5685_3_lut (.A(\s_op_fifo.buf[2].len [3]), .B(\s_op_fifo.buf[3].len [3]), 
         .C(\s_op_fifo.tail [0]), .Z(n7348)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5685_3_lut.init = 16'hcaca;
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i0  (.D(\s_op0.addr [0]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i0 .GSR = "DISABLED";
    LUT4 i5684_3_lut (.A(\s_op_fifo.buf[0].len [3]), .B(\s_op_fifo.buf[1].len [3]), 
         .C(\s_op_fifo.tail [0]), .Z(n7347)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5684_3_lut.init = 16'hcaca;
    FD1P3AX \s_op_fifo.buf_2..pos_i0_i0  (.D(n8324), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].pos [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..pos_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i0  (.D(\s_op0.addr [0]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..pos_i0_i0  (.D(n8324), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].pos [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..pos_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i0  (.D(\s_op0.addr [0]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i0 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..pos_i0_i0  (.D(n8324), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].pos [0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..pos_i0_i0 .GSR = "DISABLED";
    FD1S3AX s_count_r_0__416 (.D(n2032[0]), .CK(mclk_c), .Q(\s_count_r[0] )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_0__416.GSR = "ENABLED";
    LUT4 s_count_r_3__I_0_508_2_lut_3_lut_4_lut (.A(n8289), .B(s_count_r[1]), 
         .C(s_count_r_c[3]), .D(s_count_r_c[2]), .Z(s_count_r_3__N_905)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_3__I_0_508_2_lut_3_lut_4_lut.init = 16'hf0d2;
    FD1S3AX s_count_r_1__417 (.D(n2032[1]), .CK(mclk_c), .Q(s_count_r[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_1__417.GSR = "ENABLED";
    FD1S3AX s_count_r_2__418 (.D(n2032[2]), .CK(mclk_c), .Q(s_count_r_c[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_2__418.GSR = "ENABLED";
    LUT4 i5654_3_lut (.A(\s_op_fifo.buf[0].addr [4]), .B(\s_op_fifo.buf[1].addr [4]), 
         .C(\s_op_fifo.tail [0]), .Z(n7317)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5654_3_lut.init = 16'hcaca;
    LUT4 bit_pos_3__I_91_3_lut_rep_152_4_lut (.A(op_msbf), .B(n8294), .C(bit_pos[1]), 
         .D(bit_pos_c[2]), .Z(n8266)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam bit_pos_3__I_91_3_lut_rep_152_4_lut.init = 16'h4008;
    FD1S3AX s_count_r_3__419 (.D(n2032[3]), .CK(mclk_c), .Q(s_count_r_c[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_3__419.GSR = "ENABLED";
    FD1S3AX s_count_r_4__420 (.D(n2032[4]), .CK(mclk_c), .Q(s_count_r_c[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_4__420.GSR = "ENABLED";
    FD1S3AX s_count_r_5__421 (.D(n2032[5]), .CK(mclk_c), .Q(s_count_r_c[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_5__421.GSR = "ENABLED";
    FD1S3AX s_count_r_6__422 (.D(n2032[6]), .CK(mclk_c), .Q(s_count_r_c[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_6__422.GSR = "ENABLED";
    FD1S3AX s_count_r_7__423 (.D(n2032[7]), .CK(mclk_c), .Q(s_count_r_c[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_7__423.GSR = "ENABLED";
    FD1S3AX s_count_r_8__424 (.D(n2032[8]), .CK(mclk_c), .Q(s_count_r_c[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_8__424.GSR = "ENABLED";
    FD1S3AX s_count_r_9__425 (.D(n2032[9]), .CK(mclk_c), .Q(s_count_r_c[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(232[4] 241[11])
    defparam s_count_r_9__425.GSR = "ENABLED";
    FD1S3AX s_pos_r1_0__426 (.D(n2034[0]), .CK(mclk_c), .Q(\bit_pos[0] )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(257[4] 267[11])
    defparam s_pos_r1_0__426.GSR = "ENABLED";
    FD1S3AX s_pos_r1_1__427 (.D(n2034[1]), .CK(mclk_c), .Q(bit_pos[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(257[4] 267[11])
    defparam s_pos_r1_1__427.GSR = "ENABLED";
    FD1S3AX s_pos_r1_2__428 (.D(n2034[2]), .CK(mclk_c), .Q(bit_pos_c[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(257[4] 267[11])
    defparam s_pos_r1_2__428.GSR = "ENABLED";
    FD1S3AX s_pos_r1_3__429 (.D(n2034[3]), .CK(mclk_c), .Q(bit_pos_c[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(257[4] 267[11])
    defparam s_pos_r1_3__429.GSR = "ENABLED";
    FD1S3AX s_addr_r1_0__430 (.D(n2033[0]), .CK(mclk_c), .Q(s_addr_r1[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_0__430.GSR = "ENABLED";
    FD1S3AX s_addr_r1_1__431 (.D(n2033[1]), .CK(mclk_c), .Q(s_addr_r1[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_1__431.GSR = "ENABLED";
    FD1S3AX s_addr_r1_2__432 (.D(n2033[2]), .CK(mclk_c), .Q(s_addr_r1[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_2__432.GSR = "ENABLED";
    FD1S3AX s_addr_r1_3__433 (.D(n2033[3]), .CK(mclk_c), .Q(s_addr_r1[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_3__433.GSR = "ENABLED";
    LUT4 s_wrap_stb_I_0_2_lut_3_lut_4_lut (.A(n8225), .B(s_count_r_c[8]), 
         .C(n8257), .D(s_count_r_c[9]), .Z(mem_wr_N_1153)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_wrap_stb_I_0_2_lut_3_lut_4_lut.init = 16'hf0f2;
    FD1S3AX s_addr_r1_4__434 (.D(n2033[4]), .CK(mclk_c), .Q(s_addr_r1[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_4__434.GSR = "ENABLED";
    LUT4 bit_pos_2__bdd_4_lut (.A(bit_pos_c[2]), .B(n8275), .C(op_pos[2]), 
         .D(fifo_get), .Z(n2034[2])) /* synthesis lut_function=(A (B (C (D))+!B (C+!(D)))+!A (B (C+!(D))+!B (C (D)))) */ ;
    defparam bit_pos_2__bdd_4_lut.init = 16'hf066;
    LUT4 s_addr_r1_9__I_97_3_lut_rep_101_4_lut (.A(op_msbf), .B(n8226), 
         .C(s_addr_r1[4]), .D(s_addr_r1[5]), .Z(n8215)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_97_3_lut_rep_101_4_lut.init = 16'h4008;
    FD1S3AX s_addr_r1_5__435 (.D(n2033[5]), .CK(mclk_c), .Q(s_addr_r1[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_5__435.GSR = "ENABLED";
    FD1S3AX s_addr_r1_6__436 (.D(n2033[6]), .CK(mclk_c), .Q(s_addr_r1[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_6__436.GSR = "ENABLED";
    FD1S3AX s_addr_r1_7__437 (.D(n2033[7]), .CK(mclk_c), .Q(s_addr_r1[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_7__437.GSR = "ENABLED";
    LUT4 s_addr_r1_5__I_0_2_lut_4_lut (.A(op_msbf), .B(n8226), .C(s_addr_r1[4]), 
         .D(s_addr_r1[5]), .Z(s_addr_r1_5__N_868)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_5__I_0_2_lut_4_lut.init = 16'hb748;
    FD1S3AX s_addr_r1_8__438 (.D(n2033[8]), .CK(mclk_c), .Q(s_addr_r1[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_8__438.GSR = "ENABLED";
    FD1S3AX s_addr_r1_9__439 (.D(n2033[9]), .CK(mclk_c), .Q(s_addr_r1[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(280[4] 288[11])
    defparam s_addr_r1_9__439.GSR = "ENABLED";
    FD1P3AX \s_op_fifo.buf_3..msbf_398  (.D(\s_desc_data[31] ), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf_3_.msbf ));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..msbf_398 .GSR = "DISABLED";
    LUT4 s_addr_r1_8__I_0_2_lut_4_lut (.A(op_msbf), .B(n8209), .C(s_addr_r1[7]), 
         .D(s_addr_r1[8]), .Z(s_addr_r1_8__N_862)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_8__I_0_2_lut_4_lut.init = 16'hb748;
    FD1S3AX s_busy_r_393 (.D(n6), .CK(mclk_c), .Q(busy)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(157[3] 165[10])
    defparam s_busy_r_393.GSR = "ENABLED";
    FD1P3JX \s_op_fifo.empty_414  (.D(n8850), .SP(mclk_c_enable_118), .PD(n6644), 
            .CK(mclk_c), .Q(\s_op_fifo.empty )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.empty_414 .GSR = "ENABLED";
    FD1P3AX s_rxbuf_i0 (.D(mem_dout_15__N_799), .SP(mclk_c_enable_92), .CK(mclk_c), 
            .Q(s_rx_mem_din[0])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i0.GSR = "ENABLED";
    LUT4 i5682_3_lut (.A(\s_op_fifo.buf[2].len [4]), .B(\s_op_fifo.buf[3].len [4]), 
         .C(\s_op_fifo.tail [0]), .Z(n7345)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5682_3_lut.init = 16'hcaca;
    LUT4 i5681_3_lut (.A(\s_op_fifo.buf[0].len [4]), .B(\s_op_fifo.buf[1].len [4]), 
         .C(\s_op_fifo.tail [0]), .Z(n7344)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5681_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_3_lut_4_lut (.A(mclk_c_enable_204), .B(n8300), .C(s_rx_mem_wr_stb), 
         .D(n8296), .Z(mclk_c_enable_258)) /* synthesis lut_function=(A (B (C+(D))+!B (C))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut.init = 16'hf8f0;
    LUT4 i5679_3_lut (.A(\s_op_fifo.buf[2].len [5]), .B(\s_op_fifo.buf[3].len [5]), 
         .C(\s_op_fifo.tail [0]), .Z(n7342)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5679_3_lut.init = 16'hcaca;
    LUT4 i5678_3_lut (.A(\s_op_fifo.buf[0].len [5]), .B(\s_op_fifo.buf[1].len [5]), 
         .C(\s_op_fifo.tail [0]), .Z(n7341)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5678_3_lut.init = 16'hcaca;
    LUT4 i5676_3_lut (.A(\s_op_fifo.buf[2].len [6]), .B(\s_op_fifo.buf[3].len [6]), 
         .C(\s_op_fifo.tail [0]), .Z(n7339)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5676_3_lut.init = 16'hcaca;
    LUT4 i3281_2_lut_3_lut_4_lut (.A(mclk_c_enable_204), .B(n8300), .C(s_mtdo), 
         .D(n8296), .Z(mem_dout_15__N_739)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i3281_2_lut_3_lut_4_lut.init = 16'h8000;
    LUT4 i5675_3_lut (.A(\s_op_fifo.buf[0].len [6]), .B(\s_op_fifo.buf[1].len [6]), 
         .C(\s_op_fifo.tail [0]), .Z(n7338)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5675_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_3_lut_4_lut_adj_181 (.A(mclk_c_enable_204), .B(n8300), 
         .C(s_rx_mem_wr_stb), .D(n8297), .Z(mclk_c_enable_254)) /* synthesis lut_function=(A (B (C+!(D))+!B (C))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_181.init = 16'hf0f8;
    LUT4 i3277_2_lut_3_lut_4_lut (.A(mclk_c_enable_204), .B(n8300), .C(s_mtdo), 
         .D(n8297), .Z(mem_dout_15__N_755)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;
    defparam i3277_2_lut_3_lut_4_lut.init = 16'h0080;
    LUT4 i1_2_lut_3_lut_4_lut_adj_182 (.A(mclk_c_enable_204), .B(n8300), 
         .C(s_rx_mem_wr_stb), .D(n8299), .Z(mclk_c_enable_250)) /* synthesis lut_function=(A (B (C+!(D))+!B (C))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_182.init = 16'hf0f8;
    LUT4 i3273_2_lut_3_lut_4_lut (.A(mclk_c_enable_204), .B(n8300), .C(s_mtdo), 
         .D(n8299), .Z(mem_dout_15__N_771)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;
    defparam i3273_2_lut_3_lut_4_lut.init = 16'h0080;
    LUT4 i1_2_lut_3_lut_4_lut_adj_183 (.A(mclk_c_enable_204), .B(n8301), 
         .C(s_rx_mem_wr_stb), .D(n8296), .Z(mclk_c_enable_257)) /* synthesis lut_function=(A (B (C)+!B (C+(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_183.init = 16'hf2f0;
    LUT4 i3280_2_lut_3_lut_4_lut (.A(mclk_c_enable_204), .B(n8301), .C(s_mtdo), 
         .D(n8296), .Z(mem_dout_15__N_743)) /* synthesis lut_function=(!((B+!(C (D)))+!A)) */ ;
    defparam i3280_2_lut_3_lut_4_lut.init = 16'h2000;
    LUT4 i1_2_lut_3_lut_4_lut_adj_184 (.A(mclk_c_enable_204), .B(n8301), 
         .C(s_rx_mem_wr_stb), .D(n8297), .Z(mclk_c_enable_253)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_184.init = 16'hf0f2;
    LUT4 i5673_3_lut (.A(\s_op_fifo.buf[2].len [7]), .B(\s_op_fifo.buf[3].len [7]), 
         .C(\s_op_fifo.tail [0]), .Z(n7336)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5673_3_lut.init = 16'hcaca;
    LUT4 i5672_3_lut (.A(\s_op_fifo.buf[0].len [7]), .B(\s_op_fifo.buf[1].len [7]), 
         .C(\s_op_fifo.tail [0]), .Z(n7335)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5672_3_lut.init = 16'hcaca;
    LUT4 i5670_3_lut (.A(\s_op_fifo.buf[2].len [8]), .B(\s_op_fifo.buf[3].len [8]), 
         .C(\s_op_fifo.tail [0]), .Z(n7333)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5670_3_lut.init = 16'hcaca;
    LUT4 i5669_3_lut (.A(\s_op_fifo.buf[0].len [8]), .B(\s_op_fifo.buf[1].len [8]), 
         .C(\s_op_fifo.tail [0]), .Z(n7332)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5669_3_lut.init = 16'hcaca;
    LUT4 i3276_2_lut_3_lut_4_lut (.A(mclk_c_enable_204), .B(n8301), .C(s_mtdo), 
         .D(n8297), .Z(mem_dout_15__N_759)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i3276_2_lut_3_lut_4_lut.init = 16'h0020;
    LUT4 i5667_3_lut (.A(\s_op_fifo.buf[2].len [9]), .B(\s_op_fifo.buf[3].len [9]), 
         .C(\s_op_fifo.tail [0]), .Z(n7330)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5667_3_lut.init = 16'hcaca;
    LUT4 i5666_3_lut (.A(\s_op_fifo.buf[0].len [9]), .B(\s_op_fifo.buf[1].len [9]), 
         .C(\s_op_fifo.tail [0]), .Z(n7329)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5666_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_3_lut_4_lut_adj_185 (.A(mclk_c_enable_204), .B(n8301), 
         .C(s_rx_mem_wr_stb), .D(n8299), .Z(mclk_c_enable_249)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_185.init = 16'hf0f2;
    LUT4 i3272_2_lut_3_lut_4_lut (.A(mclk_c_enable_204), .B(n8301), .C(s_mtdo), 
         .D(n8299), .Z(mem_dout_15__N_775)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i3272_2_lut_3_lut_4_lut.init = 16'h0020;
    LUT4 i5664_3_lut (.A(\s_op_fifo.buf[2].addr [1]), .B(\s_op_fifo.buf[3].addr [1]), 
         .C(\s_op_fifo.tail [0]), .Z(n7327)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5664_3_lut.init = 16'hcaca;
    LUT4 i5663_3_lut (.A(\s_op_fifo.buf[0].addr [1]), .B(\s_op_fifo.buf[1].addr [1]), 
         .C(\s_op_fifo.tail [0]), .Z(n7326)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5663_3_lut.init = 16'hcaca;
    LUT4 i5661_3_lut (.A(\s_op_fifo.buf[2].addr [2]), .B(\s_op_fifo.buf[3].addr [2]), 
         .C(\s_op_fifo.tail [0]), .Z(n7324)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5661_3_lut.init = 16'hcaca;
    LUT4 i5660_3_lut (.A(\s_op_fifo.buf[0].addr [2]), .B(\s_op_fifo.buf[1].addr [2]), 
         .C(\s_op_fifo.tail [0]), .Z(n7323)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5660_3_lut.init = 16'hcaca;
    LUT4 s_count_r_9__N_881_I_0_2_lut_rep_100_3_lut_4_lut (.A(n8232), .B(s_count_r_c[7]), 
         .C(s_count_r_c[9]), .D(s_count_r_c[8]), .Z(n8214)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__N_881_I_0_2_lut_rep_100_3_lut_4_lut.init = 16'h0002;
    LUT4 s_count_r_9__I_0_529_2_lut_3_lut_4_lut (.A(n8232), .B(s_count_r_c[7]), 
         .C(s_count_r_c[9]), .D(s_count_r_c[8]), .Z(s_count_r_9__N_880)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_0_529_2_lut_3_lut_4_lut.init = 16'hf0d2;
    LUT4 i1_2_lut_3_lut_4_lut_adj_186 (.A(mclk_c_enable_204), .B(n8302), 
         .C(s_rx_mem_wr_stb), .D(n8296), .Z(mclk_c_enable_256)) /* synthesis lut_function=(A (B (C)+!B (C+(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_186.init = 16'hf2f0;
    LUT4 i5809_3_lut (.A(\s_op_fifo.buf[0].addr [6]), .B(\s_op_fifo.buf[1].addr [6]), 
         .C(\s_op_fifo.tail [0]), .Z(n7472)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5809_3_lut.init = 16'hcaca;
    LUT4 s_addr_r1_9__I_98_3_lut_rep_105_4_lut (.A(op_msbf), .B(n8230), 
         .C(s_addr_r1[3]), .D(s_addr_r1[4]), .Z(n8219)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_98_3_lut_rep_105_4_lut.init = 16'h4008;
    LUT4 i3279_2_lut_3_lut_4_lut (.A(mclk_c_enable_204), .B(n8302), .C(s_mtdo), 
         .D(n8296), .Z(mem_dout_15__N_747)) /* synthesis lut_function=(!((B+!(C (D)))+!A)) */ ;
    defparam i3279_2_lut_3_lut_4_lut.init = 16'h2000;
    LUT4 i1_2_lut_3_lut_4_lut_adj_187 (.A(mclk_c_enable_204), .B(n8302), 
         .C(s_rx_mem_wr_stb), .D(n8297), .Z(mclk_c_enable_252)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_187.init = 16'hf0f2;
    LUT4 i5658_3_lut (.A(\s_op_fifo.buf[2].addr [3]), .B(\s_op_fifo.buf[3].addr [3]), 
         .C(\s_op_fifo.tail [0]), .Z(n7321)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5658_3_lut.init = 16'hcaca;
    LUT4 s_addr_r1_4__I_0_2_lut_4_lut (.A(op_msbf), .B(n8230), .C(s_addr_r1[3]), 
         .D(s_addr_r1[4]), .Z(s_addr_r1_4__N_870)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_4__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 i5657_3_lut (.A(\s_op_fifo.buf[0].addr [3]), .B(\s_op_fifo.buf[1].addr [3]), 
         .C(\s_op_fifo.tail [0]), .Z(n7320)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5657_3_lut.init = 16'hcaca;
    LUT4 i3275_2_lut_3_lut_4_lut (.A(mclk_c_enable_204), .B(n8302), .C(s_mtdo), 
         .D(n8297), .Z(mem_dout_15__N_763)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i3275_2_lut_3_lut_4_lut.init = 16'h0020;
    LUT4 i1_2_lut_3_lut_4_lut_adj_188 (.A(mclk_c_enable_204), .B(n8302), 
         .C(s_rx_mem_wr_stb), .D(n8299), .Z(mclk_c_enable_248)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_188.init = 16'hf0f2;
    LUT4 i3271_2_lut_3_lut_4_lut (.A(mclk_c_enable_204), .B(n8302), .C(s_mtdo), 
         .D(n8299), .Z(mem_dout_15__N_779)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i3271_2_lut_3_lut_4_lut.init = 16'h0020;
    LUT4 i1_2_lut_3_lut_4_lut_adj_189 (.A(mclk_c_enable_204), .B(n8306), 
         .C(s_rx_mem_wr_stb), .D(n8300), .Z(mclk_c_enable_246)) /* synthesis lut_function=(A (B (C)+!B (C+(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_189.init = 16'hf2f0;
    LUT4 i3269_2_lut_3_lut_4_lut (.A(mclk_c_enable_204), .B(n8306), .C(s_mtdo), 
         .D(n8300), .Z(mem_dout_15__N_787)) /* synthesis lut_function=(!((B+!(C (D)))+!A)) */ ;
    defparam i3269_2_lut_3_lut_4_lut.init = 16'h2000;
    LUT4 i1_2_lut_3_lut_4_lut_adj_190 (.A(mclk_c_enable_204), .B(n8306), 
         .C(s_rx_mem_wr_stb), .D(n8301), .Z(mclk_c_enable_245)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_190.init = 16'hf0f2;
    LUT4 i3268_2_lut_3_lut_4_lut (.A(mclk_c_enable_204), .B(n8306), .C(s_mtdo), 
         .D(n8301), .Z(mem_dout_15__N_791)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i3268_2_lut_3_lut_4_lut.init = 16'h0020;
    LUT4 i1_2_lut_3_lut_4_lut_adj_191 (.A(mclk_c_enable_204), .B(n8306), 
         .C(s_rx_mem_wr_stb), .D(n8302), .Z(mclk_c_enable_244)) /* synthesis lut_function=(A (B (C)+!B (C+!(D)))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_191.init = 16'hf0f2;
    LUT4 i3267_2_lut_3_lut_4_lut (.A(mclk_c_enable_204), .B(n8306), .C(s_mtdo), 
         .D(n8302), .Z(mem_dout_15__N_795)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i3267_2_lut_3_lut_4_lut.init = 16'h0020;
    FD1P3AX \s_op_fifo.head_FSM_i0_i1  (.D(\s_op_fifo.buf_3_.msbf_N_1174 [0]), 
            .SP(mclk_c_enable_118), .CK(mclk_c), .Q(\s_op_fifo.buf_3_.msbf_N_1174 [1]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam \s_op_fifo.head_FSM_i0_i1 .GSR = "ENABLED";
    FD1P3AX \s_op_fifo.head_FSM_i0_i2  (.D(\s_op_fifo.buf_3_.msbf_N_1174 [1]), 
            .SP(mclk_c_enable_118), .CK(mclk_c), .Q(\s_op_fifo.buf_3_.msbf_N_1174 [2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam \s_op_fifo.head_FSM_i0_i2 .GSR = "ENABLED";
    FD1P3AX \s_op_fifo.head_FSM_i0_i3  (.D(\s_op_fifo.buf_3_.msbf_N_1174 [2]), 
            .SP(mclk_c_enable_118), .CK(mclk_c), .Q(\s_op_fifo.buf_3_.msbf_N_1174 [3]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam \s_op_fifo.head_FSM_i0_i3 .GSR = "ENABLED";
    LUT4 i5813_3_lut (.A(\s_op_fifo.buf[2].addr [5]), .B(\s_op_fifo.buf[3].addr [5]), 
         .C(\s_op_fifo.tail [0]), .Z(n7476)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5813_3_lut.init = 16'hcaca;
    FD1P3AX \s_op_fifo.buf_3..len_i0_i1  (.D(s_pos_0_3__N_670[1]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i1 .GSR = "DISABLED";
    LUT4 i5812_3_lut (.A(\s_op_fifo.buf[0].addr [5]), .B(\s_op_fifo.buf[1].addr [5]), 
         .C(\s_op_fifo.tail [0]), .Z(n7475)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5812_3_lut.init = 16'hcaca;
    LUT4 s_addr_r1_9__I_99_3_lut_rep_112_4_lut (.A(op_msbf), .B(n8234), 
         .C(s_addr_r1[2]), .D(s_addr_r1[3]), .Z(n8226)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_99_3_lut_rep_112_4_lut.init = 16'h4008;
    LUT4 s_addr_r1_3__I_0_2_lut_4_lut (.A(op_msbf), .B(n8234), .C(s_addr_r1[2]), 
         .D(s_addr_r1[3]), .Z(s_addr_r1_3__N_872)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_3__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 s_count_r_8__I_0_518_2_lut_3_lut_4_lut (.A(n8237), .B(s_count_r_c[6]), 
         .C(s_count_r_c[8]), .D(s_count_r_c[7]), .Z(s_count_r_8__N_925)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_8__I_0_518_2_lut_3_lut_4_lut.init = 16'hf0d2;
    LUT4 s_count_r_9__I_109_2_lut_rep_147_3_lut_4_lut (.A(mclk_c_enable_204), 
         .B(\s_count_r[0] ), .C(s_count_r_c[2]), .D(s_count_r[1]), .Z(n8261)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_109_2_lut_rep_147_3_lut_4_lut.init = 16'h0002;
    FD1P3AX \s_op_fifo.buf_3..len_i0_i2  (.D(s_pos_0_3__N_670[2]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i3  (.D(s_pos_0_3__N_670[3]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i4  (.D(\s_desc_data[4] ), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i5  (.D(\s_desc_data[5] ), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i6  (.D(\s_desc_data[6] ), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i7  (.D(\s_desc_data[7] ), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i8  (.D(\s_desc_data[8] ), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..len_i0_i9  (.D(\s_desc_data[9] ), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].len [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..len_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i1  (.D(\s_op0.addr [1]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i1 .GSR = "DISABLED";
    PFUMX i5814 (.BLUT(n7475), .ALUT(n7476), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[5]));
    LUT4 s_addr_r1_9__I_100_3_lut_rep_116_4_lut (.A(op_msbf), .B(n8241), 
         .C(s_addr_r1[1]), .D(s_addr_r1[2]), .Z(n8230)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_100_3_lut_rep_116_4_lut.init = 16'h4008;
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i2  (.D(\s_op0.addr [2]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i3  (.D(\s_op0.addr [3]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i4  (.D(\s_op0.addr [4]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i5  (.D(\s_op0.addr [5]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i6  (.D(\s_op0.addr [6]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i7  (.D(\s_op0.addr [7]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i8  (.D(\s_op0.addr [8]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..addr_i0_i9  (.D(\s_op0.addr [9]), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].addr [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..addr_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i1  (.D(s_pos_0_3__N_670[1]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i1 .GSR = "DISABLED";
    LUT4 s_addr_r1_2__I_0_2_lut_4_lut (.A(op_msbf), .B(n8241), .C(s_addr_r1[1]), 
         .D(s_addr_r1[2]), .Z(s_addr_r1_2__N_874)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_2__I_0_2_lut_4_lut.init = 16'hb748;
    FD1P3AX \s_op_fifo.buf_2..len_i0_i2  (.D(s_pos_0_3__N_670[2]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i3  (.D(s_pos_0_3__N_670[3]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i4  (.D(\s_desc_data[4] ), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i5  (.D(\s_desc_data[5] ), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i6  (.D(\s_desc_data[6] ), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i7  (.D(\s_desc_data[7] ), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i8  (.D(\s_desc_data[8] ), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..len_i0_i9  (.D(\s_desc_data[9] ), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].len [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..len_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i1  (.D(s_pos_0_3__N_670[1]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i2  (.D(s_pos_0_3__N_670[2]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i3  (.D(s_pos_0_3__N_670[3]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i4  (.D(\s_desc_data[4] ), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i5  (.D(\s_desc_data[5] ), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i6  (.D(\s_desc_data[6] ), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i7  (.D(\s_desc_data[7] ), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i8  (.D(\s_desc_data[8] ), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..len_i0_i9  (.D(\s_desc_data[9] ), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].len [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..len_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i1  (.D(s_pos_0_3__N_670[1]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i2  (.D(s_pos_0_3__N_670[2]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i3  (.D(s_pos_0_3__N_670[3]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i4  (.D(\s_desc_data[4] ), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i5  (.D(\s_desc_data[5] ), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i6  (.D(\s_desc_data[6] ), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i7  (.D(\s_desc_data[7] ), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i8  (.D(\s_desc_data[8] ), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..len_i0_i9  (.D(\s_desc_data[9] ), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].len [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..len_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.tail_i0_i1  (.D(\s_op_fifo.tail_1__N_1054 [1]), .SP(fifo_get), 
            .CK(mclk_c), .Q(\s_op_fifo.tail [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.tail_i0_i1 .GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i1 (.D(s_addr_r1[1]), .SP(mclk_c_enable_204), .CK(mclk_c), 
            .Q(s_rx_mem_addr[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i1.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i2 (.D(s_addr_r1[2]), .SP(mclk_c_enable_204), .CK(mclk_c), 
            .Q(s_rx_mem_addr[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i2.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i3 (.D(s_addr_r1[3]), .SP(mclk_c_enable_204), .CK(mclk_c), 
            .Q(s_rx_mem_addr[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i3.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i4 (.D(s_addr_r1[4]), .SP(mclk_c_enable_204), .CK(mclk_c), 
            .Q(s_rx_mem_addr[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i4.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i5 (.D(s_addr_r1[5]), .SP(mclk_c_enable_204), .CK(mclk_c), 
            .Q(s_rx_mem_addr[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i5.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i6 (.D(s_addr_r1[6]), .SP(mclk_c_enable_204), .CK(mclk_c), 
            .Q(s_rx_mem_addr[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i6.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i7 (.D(s_addr_r1[7]), .SP(mclk_c_enable_204), .CK(mclk_c), 
            .Q(s_rx_mem_addr[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i7.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i8 (.D(s_addr_r1[8]), .SP(mclk_c_enable_204), .CK(mclk_c), 
            .Q(s_rx_mem_addr[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i8.GSR = "ENABLED";
    FD1P3AX s_addr_r2_i0_i9 (.D(s_addr_r1[9]), .SP(mclk_c_enable_204), .CK(mclk_c), 
            .Q(s_rx_mem_addr[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(328[3] 332[10])
    defparam s_addr_r2_i0_i9.GSR = "ENABLED";
    FD1P3AX \s_op_fifo.buf_3..pos_i0_i1  (.D(n8328), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].pos [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..pos_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..pos_i0_i2  (.D(n8329), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].pos [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..pos_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_3..pos_i0_i3  (.D(n8304), .SP(mclk_c_enable_207), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[3].pos [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_3..pos_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i1  (.D(\s_op0.addr [1]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i2  (.D(\s_op0.addr [2]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i3  (.D(\s_op0.addr [3]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i4  (.D(\s_op0.addr [4]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i5  (.D(\s_op0.addr [5]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i6  (.D(\s_op0.addr [6]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i7  (.D(\s_op0.addr [7]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i8  (.D(\s_op0.addr [8]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..addr_i0_i9  (.D(\s_op0.addr [9]), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].addr [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..addr_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..pos_i0_i1  (.D(n8328), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].pos [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..pos_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..pos_i0_i2  (.D(n8329), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].pos [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..pos_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_2..pos_i0_i3  (.D(n8304), .SP(mclk_c_enable_219), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[2].pos [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_2..pos_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i1  (.D(\s_op0.addr [1]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i2  (.D(\s_op0.addr [2]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i3  (.D(\s_op0.addr [3]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i4  (.D(\s_op0.addr [4]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i5  (.D(\s_op0.addr [5]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i6  (.D(\s_op0.addr [6]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i7  (.D(\s_op0.addr [7]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i8  (.D(\s_op0.addr [8]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..addr_i0_i9  (.D(\s_op0.addr [9]), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].addr [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..addr_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..pos_i0_i1  (.D(n8328), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].pos [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..pos_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..pos_i0_i2  (.D(n8329), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].pos [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..pos_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_1..pos_i0_i3  (.D(n8304), .SP(mclk_c_enable_231), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[1].pos [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_1..pos_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i1  (.D(\s_op0.addr [1]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i2  (.D(\s_op0.addr [2]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i3  (.D(\s_op0.addr [3]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i3 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i4  (.D(\s_op0.addr [4]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i4 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i5  (.D(\s_op0.addr [5]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i5 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i6  (.D(\s_op0.addr [6]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i6 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i7  (.D(\s_op0.addr [7]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i7 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i8  (.D(\s_op0.addr [8]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i8 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..addr_i0_i9  (.D(\s_op0.addr [9]), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].addr [9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..addr_i0_i9 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..pos_i0_i1  (.D(n8328), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].pos [1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..pos_i0_i1 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..pos_i0_i2  (.D(n8329), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].pos [2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..pos_i0_i2 .GSR = "DISABLED";
    FD1P3AX \s_op_fifo.buf_0..pos_i0_i3  (.D(n8304), .SP(mclk_c_enable_243), 
            .CK(mclk_c), .Q(\s_op_fifo.buf[0].pos [3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam \s_op_fifo.buf_0..pos_i0_i3 .GSR = "DISABLED";
    FD1P3AX s_rxbuf_i1 (.D(mem_dout_15__N_795), .SP(mclk_c_enable_244), 
            .CK(mclk_c), .Q(s_rx_mem_din[1])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i1.GSR = "ENABLED";
    LUT4 bit_pos_3__I_92_3_lut_rep_161_4_lut (.A(op_msbf), .B(mclk_c_enable_204), 
         .C(\bit_pos[0] ), .D(bit_pos[1]), .Z(n8275)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam bit_pos_3__I_92_3_lut_rep_161_4_lut.init = 16'h4008;
    LUT4 bit_pos_1__I_0_2_lut_4_lut (.A(op_msbf), .B(mclk_c_enable_204), 
         .C(\bit_pos[0] ), .D(bit_pos[1]), .Z(bit_pos_1__N_814)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam bit_pos_1__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 i3241_2_lut_rep_182 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), .Z(n8296)) /* synthesis lut_function=(A (B)) */ ;
    defparam i3241_2_lut_rep_182.init = 16'h8888;
    LUT4 i1_2_lut_3_lut_4_lut_adj_192 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .C(s_rx_mem_wr_stb), .D(n8282), .Z(mclk_c_enable_255)) /* synthesis lut_function=(A (B (C+(D))+!B (C))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_192.init = 16'hf8f0;
    FD1P3AX s_rxbuf_i2 (.D(mem_dout_15__N_791), .SP(mclk_c_enable_245), 
            .CK(mclk_c), .Q(s_rx_mem_din[2])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i2.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i3 (.D(mem_dout_15__N_787), .SP(mclk_c_enable_246), 
            .CK(mclk_c), .Q(s_rx_mem_din[3])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i3.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i4 (.D(mem_dout_15__N_783), .SP(mclk_c_enable_247), 
            .CK(mclk_c), .Q(s_rx_mem_din[4])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i4.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i5 (.D(mem_dout_15__N_779), .SP(mclk_c_enable_248), 
            .CK(mclk_c), .Q(s_rx_mem_din[5])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i5.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i6 (.D(mem_dout_15__N_775), .SP(mclk_c_enable_249), 
            .CK(mclk_c), .Q(s_rx_mem_din[6])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i6.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i7 (.D(mem_dout_15__N_771), .SP(mclk_c_enable_250), 
            .CK(mclk_c), .Q(s_rx_mem_din[7])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i7.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i8 (.D(mem_dout_15__N_767), .SP(mclk_c_enable_251), 
            .CK(mclk_c), .Q(s_rx_mem_din[8])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i8.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i9 (.D(mem_dout_15__N_763), .SP(mclk_c_enable_252), 
            .CK(mclk_c), .Q(s_rx_mem_din[9])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i9.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i10 (.D(mem_dout_15__N_759), .SP(mclk_c_enable_253), 
            .CK(mclk_c), .Q(s_rx_mem_din[10])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i10.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i11 (.D(mem_dout_15__N_755), .SP(mclk_c_enable_254), 
            .CK(mclk_c), .Q(s_rx_mem_din[11])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i11.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i12 (.D(mem_dout_15__N_751), .SP(mclk_c_enable_255), 
            .CK(mclk_c), .Q(s_rx_mem_din[12])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i12.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i13 (.D(mem_dout_15__N_747), .SP(mclk_c_enable_256), 
            .CK(mclk_c), .Q(s_rx_mem_din[13])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i13.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i14 (.D(mem_dout_15__N_743), .SP(mclk_c_enable_257), 
            .CK(mclk_c), .Q(s_rx_mem_din[14])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i14.GSR = "ENABLED";
    FD1P3AX s_rxbuf_i15 (.D(mem_dout_15__N_739), .SP(mclk_c_enable_258), 
            .CK(mclk_c), .Q(s_rx_mem_din[15])) /* synthesis lse_init_val=0, LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=263, LSE_RLINE=263 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(299[4] 307[11])
    defparam s_rxbuf_i15.GSR = "ENABLED";
    LUT4 i3278_2_lut_3_lut_4_lut (.A(bit_pos_c[2]), .B(bit_pos_c[3]), .C(s_mtdo), 
         .D(n8282), .Z(mem_dout_15__N_751)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i3278_2_lut_3_lut_4_lut.init = 16'h8000;
    LUT4 bit_pos_3__I_0_457_i6_2_lut_rep_183 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .Z(n8297)) /* synthesis lut_function=(A+!(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam bit_pos_3__I_0_457_i6_2_lut_rep_183.init = 16'hbbbb;
    LUT4 vec_msbf_I_0_3_lut (.A(\s_desc_data[31] ), .B(\s_rx_op.msbf_N_1187 ), 
         .C(busy), .Z(\s_rx_op.msbf )) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_msbf_I_0_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_3_lut_4_lut_adj_193 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .C(s_rx_mem_wr_stb), .D(n8282), .Z(mclk_c_enable_251)) /* synthesis lut_function=(A (C)+!A (B (C+(D))+!B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam i1_2_lut_3_lut_4_lut_adj_193.init = 16'hf4f0;
    LUT4 i3274_2_lut_3_lut_4_lut (.A(bit_pos_c[2]), .B(bit_pos_c[3]), .C(s_mtdo), 
         .D(n8282), .Z(mem_dout_15__N_767)) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam i3274_2_lut_3_lut_4_lut.init = 16'h4000;
    LUT4 s_count_r_9__I_104_2_lut_rep_111_3_lut_4_lut (.A(n8245), .B(s_count_r_c[5]), 
         .C(s_count_r_c[7]), .D(s_count_r_c[6]), .Z(n8225)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_104_2_lut_rep_111_3_lut_4_lut.init = 16'h0002;
    LUT4 s_count_r_7__I_0_516_2_lut_3_lut_4_lut (.A(n8245), .B(s_count_r_c[5]), 
         .C(s_count_r_c[7]), .D(s_count_r_c[6]), .Z(s_count_r_7__N_921)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_7__I_0_516_2_lut_3_lut_4_lut.init = 16'hf0d2;
    PFUMX i5656 (.BLUT(n7317), .ALUT(n7318), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[4]));
    LUT4 bit_pos_3__I_0_465_i6_2_lut_rep_185 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .Z(n8299)) /* synthesis lut_function=((B)+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam bit_pos_3__I_0_465_i6_2_lut_rep_185.init = 16'hdddd;
    LUT4 i1_2_lut_3_lut_4_lut_adj_194 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .C(s_rx_mem_wr_stb), .D(n8282), .Z(mclk_c_enable_247)) /* synthesis lut_function=(A (B (C)+!B (C+(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam i1_2_lut_3_lut_4_lut_adj_194.init = 16'hf2f0;
    LUT4 i3270_2_lut_3_lut_4_lut (.A(bit_pos_c[2]), .B(bit_pos_c[3]), .C(s_mtdo), 
         .D(n8282), .Z(mem_dout_15__N_783)) /* synthesis lut_function=(!((B+!(C (D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam i3270_2_lut_3_lut_4_lut.init = 16'h2000;
    CCU2D add_518_11 (.A0(\s_desc_data[29] ), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n6490), .S0(\s_op0.addr [9]));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(146[16] 148[53])
    defparam add_518_11.INIT0 = 16'h5aaa;
    defparam add_518_11.INIT1 = 16'h0000;
    defparam add_518_11.INJECT1_0 = "NO";
    defparam add_518_11.INJECT1_1 = "NO";
    CCU2D add_518_9 (.A0(\s_desc_data[27] ), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(\s_desc_data[28] ), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6489), .COUT(n6490), .S0(\s_op0.addr [7]), 
          .S1(\s_op0.addr [8]));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(146[16] 148[53])
    defparam add_518_9.INIT0 = 16'h5aaa;
    defparam add_518_9.INIT1 = 16'h5aaa;
    defparam add_518_9.INJECT1_0 = "NO";
    defparam add_518_9.INJECT1_1 = "NO";
    LUT4 i1_2_lut_rep_186 (.A(bit_pos[1]), .B(\bit_pos[0] ), .Z(n8300)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_rep_186.init = 16'h8888;
    LUT4 bit_pos_3__I_0_471_i5_2_lut_rep_187 (.A(\bit_pos[0] ), .B(bit_pos[1]), 
         .Z(n8301)) /* synthesis lut_function=(A+!(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam bit_pos_3__I_0_471_i5_2_lut_rep_187.init = 16'hbbbb;
    LUT4 i1170_2_lut (.A(\s_op_fifo.tail [0]), .B(fifo_get), .Z(n1[0])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(191[3] 207[10])
    defparam i1170_2_lut.init = 16'h6666;
    CCU2D add_518_7 (.A0(\s_desc_data[9] ), .B0(\s_desc_data[31] ), .C0(\s_desc_data[25] ), 
          .D0(GND_net), .A1(\s_desc_data[26] ), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6488), .COUT(n6489), .S0(\s_op0.addr [5]), 
          .S1(\s_op0.addr [6]));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(146[16] 148[53])
    defparam add_518_7.INIT0 = 16'h7878;
    defparam add_518_7.INIT1 = 16'h5aaa;
    defparam add_518_7.INJECT1_0 = "NO";
    defparam add_518_7.INJECT1_1 = "NO";
    CCU2D add_518_5 (.A0(\s_desc_data[7] ), .B0(\s_desc_data[31] ), .C0(\s_desc_data[23] ), 
          .D0(GND_net), .A1(\s_desc_data[8] ), .B1(\s_desc_data[31] ), 
          .C1(\s_desc_data[24] ), .D1(GND_net), .CIN(n6487), .COUT(n6488), 
          .S0(\s_op0.addr [3]), .S1(\s_op0.addr [4]));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(146[16] 148[53])
    defparam add_518_5.INIT0 = 16'h7878;
    defparam add_518_5.INIT1 = 16'h7878;
    defparam add_518_5.INJECT1_0 = "NO";
    defparam add_518_5.INJECT1_1 = "NO";
    CCU2D add_518_3 (.A0(\s_desc_data[5] ), .B0(\s_desc_data[31] ), .C0(\s_desc_data[21] ), 
          .D0(GND_net), .A1(\s_desc_data[6] ), .B1(\s_desc_data[31] ), 
          .C1(\s_desc_data[22] ), .D1(GND_net), .CIN(n6486), .COUT(n6487), 
          .S0(\s_op0.addr [1]), .S1(\s_op0.addr [2]));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(146[16] 148[53])
    defparam add_518_3.INIT0 = 16'h7878;
    defparam add_518_3.INIT1 = 16'h7878;
    defparam add_518_3.INJECT1_0 = "NO";
    defparam add_518_3.INJECT1_1 = "NO";
    CCU2D add_518_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(\s_desc_data[4] ), .B1(\s_desc_data[31] ), .C1(\s_desc_data[20] ), 
          .D1(GND_net), .COUT(n6486), .S1(\s_op0.addr [0]));   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(146[16] 148[53])
    defparam add_518_1.INIT0 = 16'hF000;
    defparam add_518_1.INIT1 = 16'h7878;
    defparam add_518_1.INJECT1_0 = "NO";
    defparam add_518_1.INJECT1_1 = "NO";
    LUT4 bit_pos_3__I_0_465_i5_2_lut_rep_188 (.A(\bit_pos[0] ), .B(bit_pos[1]), 
         .Z(n8302)) /* synthesis lut_function=((B)+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam bit_pos_3__I_0_465_i5_2_lut_rep_188.init = 16'hdddd;
    PFUMX i5659 (.BLUT(n7320), .ALUT(n7321), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[3]));
    LUT4 i3373_2_lut_rep_190 (.A(s_pos_0_3__N_670[3]), .B(\s_desc_data[31] ), 
         .Z(n8304)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam i3373_2_lut_rep_190.init = 16'h8888;
    LUT4 \s_op0.pos_3__I_0_i4_3_lut_4_lut  (.A(s_pos_0_3__N_670[3]), .B(\s_desc_data[31] ), 
         .C(busy), .D(op_pos_3__N_837[3]), .Z(op_pos[3])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A (C (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam \s_op0.pos_3__I_0_i4_3_lut_4_lut .init = 16'hf808;
    LUT4 bit_pos_3__I_0_474_i6_2_lut_rep_192 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .Z(n8306)) /* synthesis lut_function=(A+(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam bit_pos_3__I_0_474_i6_2_lut_rep_192.init = 16'heeee;
    LUT4 i3266_2_lut_3_lut_4_lut (.A(bit_pos_c[2]), .B(bit_pos_c[3]), .C(s_mtdo), 
         .D(n8282), .Z(mem_dout_15__N_799)) /* synthesis lut_function=(!(A+(B+!(C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam i3266_2_lut_3_lut_4_lut.init = 16'h1000;
    LUT4 i1_2_lut_3_lut_4_lut_adj_195 (.A(bit_pos_c[2]), .B(bit_pos_c[3]), 
         .C(s_rx_mem_wr_stb), .D(n8282), .Z(mclk_c_enable_92)) /* synthesis lut_function=(A (C)+!A (B (C)+!B (C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(302[31:43])
    defparam i1_2_lut_3_lut_4_lut_adj_195.init = 16'hf1f0;
    PFUMX i5662 (.BLUT(n7323), .ALUT(n7324), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[2]));
    LUT4 i6101_4_lut (.A(fifo_get), .B(\s_op_fifo.tail [0]), .C(\s_op_fifo.head [0]), 
         .D(n4_adj_1591), .Z(n6644)) /* synthesis lut_function=(!((B (C+!(D))+!B ((D)+!C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(201[4] 206[11])
    defparam i6101_4_lut.init = 16'h0820;
    PFUMX i5665 (.BLUT(n7326), .ALUT(n7327), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[1]));
    LUT4 i1_2_lut (.A(\s_op_fifo.buf_3_.msbf_N_1174 [1]), .B(\s_op_fifo.buf_3_.msbf_N_1174 [3]), 
         .Z(\s_op_fifo.head [0])) /* synthesis lut_function=(A+(B)) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1_2_lut.init = 16'heeee;
    LUT4 i1_3_lut (.A(\s_op_fifo.tail [1]), .B(\s_op_fifo.buf_3_.msbf_N_1174 [2]), 
         .C(\s_op_fifo.buf_3_.msbf_N_1174 [3]), .Z(n4_adj_1591)) /* synthesis lut_function=(!(A (B+(C))+!A !(B+(C)))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1_3_lut.init = 16'h5656;
    PFUMX i5668 (.BLUT(n7329), .ALUT(n7330), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[9]));
    PFUMX i5671 (.BLUT(n7332), .ALUT(n7333), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[8]));
    LUT4 i2369_1_lut_rep_195 (.A(busy), .Z(n8309)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(157[3] 165[10])
    defparam i2369_1_lut_rep_195.init = 16'h5555;
    PFUMX i5674 (.BLUT(n7335), .ALUT(n7336), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[7]));
    LUT4 i6114_4_lut_4_lut (.A(busy), .B(n8214), .C(\s_op_fifo.empty ), 
         .D(n8858), .Z(n6)) /* synthesis lut_function=(A (((D)+!C)+!B)+!A (D)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(157[3] 165[10])
    defparam i6114_4_lut_4_lut.init = 16'hff2a;
    LUT4 s_addr_r1_9__I_101_3_lut_rep_120_4_lut (.A(op_msbf), .B(n8257), 
         .C(s_addr_r1[0]), .D(s_addr_r1[1]), .Z(n8234)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_101_3_lut_rep_120_4_lut.init = 16'h4008;
    LUT4 s_addr_r1_1__I_0_2_lut_4_lut (.A(op_msbf), .B(n8257), .C(s_addr_r1[0]), 
         .D(s_addr_r1[1]), .Z(s_addr_r1_1__N_876)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_1__I_0_2_lut_4_lut.init = 16'hb748;
    PFUMX i5677 (.BLUT(n7338), .ALUT(n7339), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[6]));
    PFUMX i5680 (.BLUT(n7341), .ALUT(n7342), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[5]));
    PFUMX i5683 (.BLUT(n7344), .ALUT(n7345), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[4]));
    LUT4 s_count_r_9__I_105_2_lut_rep_118_3_lut_4_lut (.A(n8253), .B(s_count_r_c[4]), 
         .C(s_count_r_c[6]), .D(s_count_r_c[5]), .Z(n8232)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_105_2_lut_rep_118_3_lut_4_lut.init = 16'h0002;
    LUT4 s_count_r_6__I_0_514_2_lut_3_lut_4_lut (.A(n8253), .B(s_count_r_c[4]), 
         .C(s_count_r_c[6]), .D(s_count_r_c[5]), .Z(s_count_r_6__N_917)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_6__I_0_514_2_lut_3_lut_4_lut.init = 16'hf0d2;
    LUT4 i3251_2_lut_rep_210 (.A(s_pos_0_3__N_670[0]), .B(\s_desc_data[31] ), 
         .Z(n8324)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam i3251_2_lut_rep_210.init = 16'h8888;
    LUT4 \s_op0.pos_3__I_0_i1_3_lut_4_lut  (.A(s_pos_0_3__N_670[0]), .B(\s_desc_data[31] ), 
         .C(busy), .D(op_pos_3__N_837[0]), .Z(op_pos[0])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A (C (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam \s_op0.pos_3__I_0_i1_3_lut_4_lut .init = 16'hf808;
    LUT4 i3371_2_lut_rep_214 (.A(s_pos_0_3__N_670[1]), .B(\s_desc_data[31] ), 
         .Z(n8328)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam i3371_2_lut_rep_214.init = 16'h8888;
    PFUMX i5686 (.BLUT(n7347), .ALUT(n7348), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[3]));
    LUT4 \s_op0.pos_3__I_0_i2_3_lut_4_lut  (.A(s_pos_0_3__N_670[1]), .B(\s_desc_data[31] ), 
         .C(busy), .D(op_pos_3__N_837[1]), .Z(op_pos[1])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A (C (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam \s_op0.pos_3__I_0_i2_3_lut_4_lut .init = 16'hf808;
    LUT4 i3372_2_lut_rep_215 (.A(s_pos_0_3__N_670[2]), .B(\s_desc_data[31] ), 
         .Z(n8329)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam i3372_2_lut_rep_215.init = 16'h8888;
    PFUMX i5689 (.BLUT(n7350), .ALUT(n7351), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[2]));
    LUT4 \s_op0.pos_3__I_0_i3_3_lut_4_lut  (.A(s_pos_0_3__N_670[2]), .B(\s_desc_data[31] ), 
         .C(busy), .D(op_pos_3__N_837[2]), .Z(op_pos[2])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A (C (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(143[15] 144[63])
    defparam \s_op0.pos_3__I_0_i3_3_lut_4_lut .init = 16'hf808;
    PFUMX i5692 (.BLUT(n7353), .ALUT(n7354), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[0]));
    LUT4 s_addr_r1_9__I_102_3_lut_rep_127_4_lut (.A(op_msbf), .B(n8266), 
         .C(bit_pos_c[3]), .D(s_addr_r1[0]), .Z(n8241)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam s_addr_r1_9__I_102_3_lut_rep_127_4_lut.init = 16'h4008;
    LUT4 s_addr_r1_0__I_0_2_lut_4_lut (.A(op_msbf), .B(n8266), .C(bit_pos_c[3]), 
         .D(s_addr_r1[0]), .Z(s_addr_r1_0__N_878)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam s_addr_r1_0__I_0_2_lut_4_lut.init = 16'hb748;
    PFUMX i5695 (.BLUT(n7356), .ALUT(n7357), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[0]));
    PFUMX i5698 (.BLUT(n7359), .ALUT(n7360), .C0(\s_op_fifo.tail [1]), 
          .Z(op_pos_3__N_837[0]));
    LUT4 s_count_r_9__I_106_2_lut_rep_123_3_lut_4_lut (.A(n8261), .B(s_count_r_c[3]), 
         .C(s_count_r_c[5]), .D(s_count_r_c[4]), .Z(n8237)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_106_2_lut_rep_123_3_lut_4_lut.init = 16'h0002;
    LUT4 s_count_r_5__I_0_512_2_lut_3_lut_4_lut (.A(n8261), .B(s_count_r_c[3]), 
         .C(s_count_r_c[5]), .D(s_count_r_c[4]), .Z(s_count_r_5__N_913)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_5__I_0_512_2_lut_3_lut_4_lut.init = 16'hf0d2;
    LUT4 s_addr_r1_9__I_95_3_lut_rep_91_4_lut (.A(op_msbf), .B(n8215), .C(s_addr_r1[6]), 
         .D(s_addr_r1[7]), .Z(n8205)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_9__I_95_3_lut_rep_91_4_lut.init = 16'h4008;
    LUT4 s_addr_r1_7__I_0_2_lut_4_lut (.A(op_msbf), .B(n8215), .C(s_addr_r1[6]), 
         .D(s_addr_r1[7]), .Z(s_addr_r1_7__N_864)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(289[10:45])
    defparam s_addr_r1_7__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 i1024_2_lut (.A(\s_op_fifo.tail [1]), .B(\s_op_fifo.tail [0]), 
         .Z(\s_op_fifo.tail_1__N_1054 [1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam i1024_2_lut.init = 16'h6666;
    LUT4 s_count_r_9__I_107_2_lut_rep_131_3_lut_4_lut (.A(n8272), .B(s_count_r_c[2]), 
         .C(s_count_r_c[4]), .D(s_count_r_c[3]), .Z(n8245)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_9__I_107_2_lut_rep_131_3_lut_4_lut.init = 16'h0002;
    LUT4 s_count_r_4__I_0_510_2_lut_3_lut_4_lut (.A(n8272), .B(s_count_r_c[2]), 
         .C(s_count_r_c[4]), .D(s_count_r_c[3]), .Z(s_count_r_4__N_909)) /* synthesis lut_function=(A (B (C)+!B (C (D)+!C !(D)))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(242[10:33])
    defparam s_count_r_4__I_0_510_2_lut_3_lut_4_lut.init = 16'hf0d2;
    LUT4 i5810_3_lut (.A(\s_op_fifo.buf[2].addr [6]), .B(\s_op_fifo.buf[3].addr [6]), 
         .C(\s_op_fifo.tail [0]), .Z(n7473)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5810_3_lut.init = 16'hcaca;
    PFUMX i5701 (.BLUT(n7362), .ALUT(n7363), .C0(\s_op_fifo.tail [1]), 
          .Z(\s_rx_op.msbf_N_1187 ));
    PFUMX i5710 (.BLUT(n7371), .ALUT(n7372), .C0(\s_op_fifo.tail [1]), 
          .Z(op_pos_3__N_837[3]));
    LUT4 \s_op0.addr_9__I_0_i1_3_lut  (.A(\s_op0.addr [0]), .B(op_addr_9__N_817[0]), 
         .C(busy), .Z(op_addr[0])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i1_3_lut .init = 16'hcaca;
    PFUMX mux_511_i10 (.BLUT(s_addr_r1_9__N_842), .ALUT(op_addr[9]), .C0(fifo_get), 
          .Z(n2033[9]));
    PFUMX mux_511_i9 (.BLUT(s_addr_r1_8__N_862), .ALUT(op_addr[8]), .C0(fifo_get), 
          .Z(n2033[8]));
    LUT4 vec_len_9__I_0_i1_3_lut (.A(s_pos_0_3__N_670[0]), .B(op_len_9__N_827[0]), 
         .C(busy), .Z(op_len[0])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam vec_len_9__I_0_i1_3_lut.init = 16'hcaca;
    LUT4 s_wrap_stb_I_115_3_lut_rep_143_4_lut (.A(op_msbf), .B(n8275), .C(bit_pos_c[2]), 
         .D(bit_pos_c[3]), .Z(n8257)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam s_wrap_stb_I_115_3_lut_rep_143_4_lut.init = 16'h4008;
    LUT4 bit_pos_3__I_0_2_lut_4_lut (.A(op_msbf), .B(n8275), .C(bit_pos_c[2]), 
         .D(bit_pos_c[3]), .Z(bit_pos_3__N_804)) /* synthesis lut_function=(A (B (C (D)+!C !(D))+!B (D))+!A !(B (C (D)+!C !(D))+!B !(D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(268[10:44])
    defparam bit_pos_3__I_0_2_lut_4_lut.init = 16'hb748;
    LUT4 i5804_3_lut (.A(\s_op_fifo.buf[2].addr [7]), .B(\s_op_fifo.buf[3].addr [7]), 
         .C(\s_op_fifo.tail [0]), .Z(n7467)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5804_3_lut.init = 16'hcaca;
    PFUMX i5728 (.BLUT(n7389), .ALUT(n7390), .C0(\s_op_fifo.tail [1]), 
          .Z(op_len_9__N_827[1]));
    PFUMX mux_510_i10 (.BLUT(s_count_r_9__N_880), .ALUT(op_len[9]), .C0(fifo_get), 
          .Z(n2032[9]));
    PFUMX mux_510_i9 (.BLUT(s_count_r_8__N_925), .ALUT(op_len[8]), .C0(fifo_get), 
          .Z(n2032[8]));
    PFUMX mux_510_i8 (.BLUT(s_count_r_7__N_921), .ALUT(op_len[7]), .C0(fifo_get), 
          .Z(n2032[7]));
    PFUMX mux_510_i7 (.BLUT(s_count_r_6__N_917), .ALUT(op_len[6]), .C0(fifo_get), 
          .Z(n2032[6]));
    PFUMX mux_510_i6 (.BLUT(s_count_r_5__N_913), .ALUT(op_len[5]), .C0(fifo_get), 
          .Z(n2032[5]));
    LUT4 i5803_3_lut (.A(\s_op_fifo.buf[0].addr [7]), .B(\s_op_fifo.buf[1].addr [7]), 
         .C(\s_op_fifo.tail [0]), .Z(n7466)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5803_3_lut.init = 16'hcaca;
    PFUMX mux_510_i5 (.BLUT(s_count_r_4__N_909), .ALUT(op_len[4]), .C0(fifo_get), 
          .Z(n2032[4]));
    PFUMX mux_510_i4 (.BLUT(s_count_r_3__N_905), .ALUT(op_len[3]), .C0(fifo_get), 
          .Z(n2032[3]));
    LUT4 \s_op0.addr_9__I_0_i2_3_lut  (.A(\s_op0.addr [1]), .B(op_addr_9__N_817[1]), 
         .C(busy), .Z(op_addr[1])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i2_3_lut .init = 16'hcaca;
    LUT4 i5801_3_lut (.A(\s_op_fifo.buf[2].addr [8]), .B(\s_op_fifo.buf[3].addr [8]), 
         .C(\s_op_fifo.tail [0]), .Z(n7464)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i5801_3_lut.init = 16'hcaca;
    PFUMX mux_510_i2 (.BLUT(s_count_r_1__N_897), .ALUT(op_len[1]), .C0(fifo_get), 
          .Z(n2032[1]));
    LUT4 \s_op0.addr_9__I_0_i3_3_lut  (.A(\s_op0.addr [2]), .B(op_addr_9__N_817[2]), 
         .C(busy), .Z(op_addr[2])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_rx.vhdl(211[13] 212[30])
    defparam \s_op0.addr_9__I_0_i3_3_lut .init = 16'hcaca;
    PFUMX mux_511_i8 (.BLUT(s_addr_r1_7__N_864), .ALUT(op_addr[7]), .C0(fifo_get), 
          .Z(n2033[7]));
    PFUMX mux_511_i7 (.BLUT(s_addr_r1_6__N_866), .ALUT(op_addr[6]), .C0(fifo_get), 
          .Z(n2033[6]));
    PFUMX i5780 (.BLUT(n7441), .ALUT(n7442), .C0(\s_op_fifo.tail [1]), 
          .Z(op_pos_3__N_837[2]));
    PFUMX mux_511_i6 (.BLUT(s_addr_r1_5__N_868), .ALUT(op_addr[5]), .C0(fifo_get), 
          .Z(n2033[5]));
    PFUMX mux_511_i5 (.BLUT(s_addr_r1_4__N_870), .ALUT(op_addr[4]), .C0(fifo_get), 
          .Z(n2033[4]));
    PFUMX i5786 (.BLUT(n7447), .ALUT(n7448), .C0(\s_op_fifo.tail [1]), 
          .Z(op_pos_3__N_837[1]));
    PFUMX i5789 (.BLUT(n7450), .ALUT(n7451), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[9]));
    PFUMX i5802 (.BLUT(n7463), .ALUT(n7464), .C0(\s_op_fifo.tail [1]), 
          .Z(op_addr_9__N_817[8]));
    PFUMX mux_511_i4 (.BLUT(s_addr_r1_3__N_872), .ALUT(op_addr[3]), .C0(fifo_get), 
          .Z(n2033[3]));
    
endmodule
//
// Verilog Description of module jtag_enh_io
//

module jtag_enh_io (mclk_c_enable_115, tckn_stb, n8851, mclk_c, mclk_c_enable_91, 
            tp_tms_c, s_mtms, s_mtdo, s_tap_tdo, s_tap_tdi, n7090, 
            s_so);
    input mclk_c_enable_115;
    output tckn_stb;
    input n8851;
    input mclk_c;
    input mclk_c_enable_91;
    output tp_tms_c;
    input s_mtms;
    output s_mtdo;
    input s_tap_tdo;
    output s_tap_tdi;
    input n7090;
    input s_so;
    
    wire mclk_c_enable_115 /* synthesis is_clock=1, SET_AS_NETWORK=mclk_c_enable_115 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
    wire s_tckn_r0_N_669;
    
    FD1S3DX s_tckn_r0_36 (.D(n8851), .CK(mclk_c_enable_115), .CD(tckn_stb), 
            .Q(s_tckn_r0_N_669)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=7, LSE_RCOL=25, LSE_LLINE=216, LSE_RLINE=216 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_io.vhdl(74[3] 78[10])
    defparam s_tckn_r0_36.GSR = "DISABLED";
    FD1P3AX s_tckn_r1_37 (.D(s_tckn_r0_N_669), .SP(mclk_c_enable_91), .CK(mclk_c), 
            .Q(tckn_stb));   // c:/devel/yard-ice/src/fpga/jtag_enh_io.vhdl(80[3] 84[10])
    defparam s_tckn_r1_37.GSR = "DISABLED";
    FD1P3AX s_tms_r0_41 (.D(s_mtms), .SP(mclk_c_enable_115), .CK(mclk_c), 
            .Q(tp_tms_c));   // c:/devel/yard-ice/src/fpga/jtag_enh_io.vhdl(112[3] 118[10])
    defparam s_tms_r0_41.GSR = "ENABLED";
    FD1P3AX s_tdo_r0_42 (.D(s_tap_tdo), .SP(mclk_c_enable_115), .CK(mclk_c), 
            .Q(s_mtdo));   // c:/devel/yard-ice/src/fpga/jtag_enh_io.vhdl(124[3] 128[10])
    defparam s_tdo_r0_42.GSR = "ENABLED";
    FD1P3IX s_tdi_r0_40 (.D(s_so), .SP(mclk_c_enable_115), .CD(n7090), 
            .CK(mclk_c), .Q(s_tap_tdi));   // c:/devel/yard-ice/src/fpga/jtag_enh_io.vhdl(112[3] 118[10])
    defparam s_tdi_r0_40.GSR = "ENABLED";
    
endmodule
//
// Verilog Description of module \jtag_enh_ecu(16,8,8) 
//

module \jtag_enh_ecu(16,8,8)  (mclk_c, s_tcken, n8851, \s_status[0] , 
            \s_status[1] , \s_status[2] , \s_status[3] , s_end_state, 
            s_tdien, s_reg_din, s_mtms, n8264, s_en_r0, n8228, n8212, 
            \s_offs_r[1] , n2503, s_offs_inc, \s_offs_r[0] , n2478, 
            tckn_stb, s_en_r1, mclk_c_enable_137, n8279, s_tap_busy, 
            \s_cfg[6] , n2, n8221, n8273, n8274, GND_net, n8317, 
            out_get_N_312, n7217, n4959, s_ptr_addr, s_cnt_en_N_1514, 
            \s_goto_state[0] , \s_goto_state[2] , \s_goto_state[1] , n1, 
            n7227);
    input mclk_c;
    output s_tcken;
    input n8851;
    output \s_status[0] ;
    output \s_status[1] ;
    output \s_status[2] ;
    output \s_status[3] ;
    output [3:0]s_end_state;
    output s_tdien;
    input [15:0]s_reg_din;
    output s_mtms;
    output n8264;
    input s_en_r0;
    output n8228;
    input n8212;
    input \s_offs_r[1] ;
    output n2503;
    input s_offs_inc;
    input \s_offs_r[0] ;
    output n2478;
    input tckn_stb;
    input s_en_r1;
    output mclk_c_enable_137;
    output n8279;
    output s_tap_busy;
    input \s_cfg[6] ;
    input n2;
    output n8221;
    output n8273;
    output n8274;
    input GND_net;
    output n8317;
    input out_get_N_312;
    input n7217;
    input n4959;
    output [8:1]s_ptr_addr;
    output s_cnt_en_N_1514;
    input \s_goto_state[0] ;
    input \s_goto_state[2] ;
    input \s_goto_state[1] ;
    output n1;
    output n7227;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    wire [7:0]s_cnt_reg;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(169[9:18])
    
    wire mclk_c_enable_114, busy_N_1509;
    wire [7:0]n333;
    wire [3:0]n514;
    wire [3:0]s_tap_state_next_3__N_1438;
    
    wire n7241;
    wire [3:0]s_tap_state_next;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(229[9:25])
    wire [3:0]n519;
    
    wire n7256;
    wire [3:0]s_tap_state_next_3__N_1461;
    
    wire n7245;
    wire [3:0]n504;
    wire [3:0]n509;
    
    wire n7258, mclk_c_enable_25, n2463;
    wire [3:0]s_irpause_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(212[9:24])
    wire [3:0]s_drpause_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(204[9:24])
    
    wire n8265, n8845, n3540;
    wire [15:0]mtms_N_1486;
    wire [15:0]s_insn_r0;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(188[9:18])
    
    wire n8262, n8342, n8341, n7550, n19, n20, n182;
    wire [3:0]s_drscan_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(200[9:23])
    
    wire n5106, n8038, s_cnt_ovf, n6615, n8045, n6648, n2159, 
        s_pos_goto_en, tdi_en_N_1474, n4856, s_pos_goto_en_N_1480, n8331, 
        n8330, tdi_en_N_1472, n8849, mclk_c_enable_45, n8254, n7736, 
        insn_goto_N_1512;
    wire [3:0]s_irscan_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(208[9:23])
    
    wire n8844, n4016, n5152, n7189, n8734, n8735, n8733, n8732, 
        n8054, n8053, n8268, n8044, n8041, n2099;
    wire [3:0]s_reset_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(220[9:22])
    
    wire s_exec_en, s_shift_en_r0, s_shift_en, n8263, n7191, n7161, 
        n8252, n4029, n3068, n7105, n8298, n126;
    wire [3:0]s_runtest_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(216[9:24])
    wire [0:0]n2108;
    
    wire n8332, n8239, n8246, n7250, n8240, n5, n4012, n8291, 
        n4, n7231, n8040, n8042, n6408;
    wire [7:0]s_cnt_top;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(168[9:18])
    
    wire n6407, n7167, n8052, n4_adj_1590;
    wire [10:0]n1655;
    
    wire n8224, n5022, n6512, n6511, n6510, n6509, n3000, n7738, 
        n7739, n7737, n8343, n2979;
    wire [3:0]s_goto_state;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(196[9:21])
    
    FD1P3IX s_cnt_reg__i0 (.D(n333[0]), .SP(mclk_c_enable_114), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i0.GSR = "ENABLED";
    L6MUX21 s_tap_state_next_3__I_0_225_i1 (.D0(n514[0]), .D1(s_tap_state_next_3__N_1438[0]), 
            .SD(n7241), .Z(s_tap_state_next[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    PFUMX s_tap_state_next_3__I_0_225_i2 (.BLUT(n519[1]), .ALUT(s_tap_state_next_3__N_1438[1]), 
          .C0(n7256), .Z(s_tap_state_next[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    PFUMX s_tap_state_next_3__I_154_i1 (.BLUT(n519[0]), .ALUT(s_tap_state_next_3__N_1461[0]), 
          .C0(n7245), .Z(s_tap_state_next_3__N_1438[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    PFUMX mux_159_i2 (.BLUT(n504[1]), .ALUT(n509[1]), .C0(n7258), .Z(s_tap_state_next_3__N_1461[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    FD1P3IX s_tck_en_194 (.D(n8851), .SP(mclk_c_enable_25), .CD(n2463), 
            .CK(mclk_c), .Q(s_tcken));   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_tck_en_194.GSR = "DISABLED";
    LUT4 tap_state_3__I_0_201_Mux_0_i15_3_lut_4_lut (.A(\s_status[0] ), .B(\s_status[1] ), 
         .C(\s_status[2] ), .D(\s_status[3] ), .Z(s_irpause_state[0])) /* synthesis lut_function=(!(A (B (C+!(D)))+!A !(B+((D)+!C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1243[3] 1308[12])
    defparam tap_state_3__I_0_201_Mux_0_i15_3_lut_4_lut.init = 16'h7f67;
    PFUMX mux_157_i1 (.BLUT(n509[0]), .ALUT(s_drpause_state[0]), .C0(n8265), 
          .Z(n514[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    LUT4 i1272_3_lut_4_lut_4_lut (.A(\s_status[2] ), .B(\s_status[3] ), 
         .C(\s_status[0] ), .D(\s_status[1] ), .Z(s_drpause_state[0])) /* synthesis lut_function=(!(A (C (D))+!A (B (C (D))+!B (C (D)+!C !(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1097[3] 1162[12])
    defparam i1272_3_lut_4_lut_4_lut.init = 16'h0ffe;
    LUT4 i1840_3_lut_3_lut_4_lut_4_lut (.A(\s_status[0] ), .B(\s_status[1] ), 
         .C(\s_status[2] ), .D(n8845), .Z(n3540)) /* synthesis lut_function=(A ((C+!(D))+!B)+!A (B+((D)+!C))) */ ;
    defparam i1840_3_lut_3_lut_4_lut_4_lut.init = 16'hf7ef;
    LUT4 mux_102_Mux_8_i15_4_lut_4_lut (.A(\s_status[0] ), .B(\s_status[1] ), 
         .C(\s_status[2] ), .D(\s_status[3] ), .Z(mtms_N_1486[8])) /* synthesis lut_function=(!(A (B+!(C))+!A (B+!(C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(411[22:33])
    defparam mux_102_Mux_8_i15_4_lut_4_lut.init = 16'h3020;
    LUT4 n182_bdd_4_lut_6813 (.A(s_insn_r0[12]), .B(s_insn_r0[14]), .C(s_insn_r0[13]), 
         .D(s_insn_r0[15]), .Z(n8262)) /* synthesis lut_function=(A+((C+!(D))+!B)) */ ;
    defparam n182_bdd_4_lut_6813.init = 16'hfbff;
    LUT4 mux_177_Mux_3_i221_4_lut_then_1_lut (.A(\s_status[3] ), .Z(n8342)) /* synthesis lut_function=(A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1316[3] 1381[12])
    defparam mux_177_Mux_3_i221_4_lut_then_1_lut.init = 16'haaaa;
    LUT4 mux_177_Mux_3_i221_4_lut_else_1_lut (.A(s_end_state[0]), .B(\s_status[3] ), 
         .C(\s_status[2] ), .D(\s_status[0] ), .Z(n8341)) /* synthesis lut_function=(A (B ((D)+!C)+!B !((D)+!C))+!A (B+(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1316[3] 1381[12])
    defparam mux_177_Mux_3_i221_4_lut_else_1_lut.init = 16'hdc7c;
    LUT4 i6072_3_lut (.A(n7550), .B(n19), .C(n20), .Z(n182)) /* synthesis lut_function=(!((B+(C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(321[22:55])
    defparam i6072_3_lut.init = 16'h0202;
    LUT4 i1751_4_lut (.A(s_drscan_state[3]), .B(n5106), .C(n8038), .D(\s_status[3] ), 
         .Z(s_tap_state_next_3__N_1438[3])) /* synthesis lut_function=(A (B+((D)+!C))+!A (B (C)+!B (C (D)))) */ ;
    defparam i1751_4_lut.init = 16'hfaca;
    LUT4 i6070_4_lut (.A(s_insn_r0[8]), .B(s_insn_r0[5]), .C(s_insn_r0[11]), 
         .D(s_insn_r0[6]), .Z(n7550)) /* synthesis lut_function=(!(A+(B+(C+(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(321[22:55])
    defparam i6070_4_lut.init = 16'h0001;
    LUT4 i7_4_lut (.A(s_insn_r0[14]), .B(s_insn_r0[9]), .C(s_insn_r0[4]), 
         .D(s_insn_r0[12]), .Z(n19)) /* synthesis lut_function=(A+(B+(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(321[22:55])
    defparam i7_4_lut.init = 16'hfeff;
    LUT4 i8_4_lut (.A(s_insn_r0[13]), .B(s_insn_r0[10]), .C(s_insn_r0[7]), 
         .D(s_insn_r0[15]), .Z(n20)) /* synthesis lut_function=(A+(B+(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(321[22:55])
    defparam i8_4_lut.init = 16'hfeff;
    LUT4 i2_3_lut (.A(\s_status[3] ), .B(s_cnt_ovf), .C(n3540), .Z(n6615)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;
    defparam i2_3_lut.init = 16'h0808;
    LUT4 i1_2_lut (.A(\s_status[1] ), .B(n8045), .Z(n6648)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut.init = 16'h8888;
    LUT4 i1_3_lut (.A(s_insn_r0[15]), .B(s_insn_r0[14]), .C(s_insn_r0[13]), 
         .Z(n2159)) /* synthesis lut_function=(A (B+(C))) */ ;
    defparam i1_3_lut.init = 16'ha8a8;
    FD1P3IX s_pos_goto_en_193 (.D(s_pos_goto_en_N_1480), .SP(tdi_en_N_1474), 
            .CD(n4856), .CK(mclk_c), .Q(s_pos_goto_en));   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_pos_goto_en_193.GSR = "ENABLED";
    LUT4 i6105_3_lut_3_lut_4_lut_4_lut_4_lut_3_lut_4_lut_1_lut_2_lut_4_lut_3_lut (.A(n8331), 
         .B(n2159), .C(n8330), .Z(tdi_en_N_1472)) /* synthesis lut_function=(!(A+!(B (C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam i6105_3_lut_3_lut_4_lut_4_lut_4_lut_3_lut_4_lut_1_lut_2_lut_4_lut_3_lut.init = 16'h4040;
    LUT4 mux_562_i1_3_lut_rep_222 (.A(s_insn_r0[12]), .B(s_insn_r0[13]), 
         .C(s_insn_r0[14]), .Z(n8849)) /* synthesis lut_function=(A (B+!(C))+!A !(B+!(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam mux_562_i1_3_lut_rep_222.init = 16'h9a9a;
    FD1P3AX s_tdi_en_195 (.D(tdi_en_N_1472), .SP(mclk_c_enable_45), .CK(mclk_c), 
            .Q(s_tdien));   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_tdi_en_195.GSR = "DISABLED";
    LUT4 insn_irscan_bdd_4_lut_6382 (.A(n8254), .B(n8262), .C(s_irpause_state[2]), 
         .D(\s_status[2] ), .Z(n7736)) /* synthesis lut_function=(A (B (D))+!A (C)) */ ;
    defparam insn_irscan_bdd_4_lut_6382.init = 16'hd850;
    FD1P3AX s_insn_r0_i0_i0 (.D(s_reg_din[0]), .SP(busy_N_1509), .CK(mclk_c), 
            .Q(s_insn_r0[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(287[3] 291[10])
    defparam s_insn_r0_i0_i0.GSR = "ENABLED";
    LUT4 n182_bdd_4_lut (.A(n182), .B(s_insn_r0[14]), .C(s_insn_r0[13]), 
         .D(s_insn_r0[15]), .Z(insn_goto_N_1512)) /* synthesis lut_function=(!((B (D)+!B (C (D)))+!A)) */ ;
    defparam n182_bdd_4_lut.init = 16'h02aa;
    LUT4 tap_state_3__I_0_200_Mux_1_i15_3_lut_4_lut_4_lut (.A(\s_status[2] ), 
         .B(\s_status[1] ), .C(\s_status[0] ), .D(\s_status[3] ), .Z(s_irscan_state[1])) /* synthesis lut_function=(!(A (B (C+!(D)))+!A (B (C+!(D))+!B !(C+(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1024[3] 1089[12])
    defparam tap_state_3__I_0_200_Mux_1_i15_3_lut_4_lut_4_lut.init = 16'h3f32;
    LUT4 mux_564_i2_3_lut_rep_218 (.A(n182), .B(n8330), .C(n2159), .Z(n8844)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam mux_564_i2_3_lut_rep_218.init = 16'hcaca;
    LUT4 i6077_2_lut_rep_151_4_lut_3_lut_4_lut_3_lut (.A(s_insn_r0[12]), .B(s_insn_r0[14]), 
         .C(n2159), .Z(n8265)) /* synthesis lut_function=(!(A+(B+!(C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam i6077_2_lut_rep_151_4_lut_3_lut_4_lut_3_lut.init = 16'h1010;
    LUT4 i2311_3_lut (.A(s_end_state[1]), .B(s_end_state[2]), .C(s_end_state[3]), 
         .Z(n4016)) /* synthesis lut_function=(A ((C)+!B)+!A !(B)) */ ;
    defparam i2311_3_lut.init = 16'hb3b3;
    LUT4 s_end_state_3__I_151_i4_4_lut (.A(insn_goto_N_1512), .B(s_insn_r0[11]), 
         .C(n5152), .D(s_insn_r0[3]), .Z(s_end_state[3])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A !((C)+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[3] 351[10])
    defparam s_end_state_3__I_151_i4_4_lut.init = 16'hac0c;
    LUT4 s_end_state_3__I_151_i2_4_lut (.A(s_insn_r0[1]), .B(s_insn_r0[9]), 
         .C(n5152), .D(insn_goto_N_1512), .Z(s_end_state[1])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A !((C)+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[3] 351[10])
    defparam s_end_state_3__I_151_i2_4_lut.init = 16'hac0c;
    LUT4 s_end_state_3__I_151_i3_4_lut (.A(s_insn_r0[2]), .B(s_insn_r0[10]), 
         .C(n5152), .D(insn_goto_N_1512), .Z(s_end_state[2])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A !((C)+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[3] 351[10])
    defparam s_end_state_3__I_151_i3_4_lut.init = 16'hac0c;
    LUT4 i5529_2_lut (.A(s_end_state[0]), .B(s_end_state[1]), .Z(n7189)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i5529_2_lut.init = 16'heeee;
    LUT4 s_end_state_3__I_151_i1_4_lut (.A(s_insn_r0[0]), .B(s_insn_r0[8]), 
         .C(n5152), .D(insn_goto_N_1512), .Z(s_end_state[0])) /* synthesis lut_function=(A (B ((D)+!C)+!B (C (D)))+!A !((C)+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[3] 351[10])
    defparam s_end_state_3__I_151_i1_4_lut.init = 16'hac0c;
    LUT4 n344_bdd_2_lut (.A(n8734), .B(s_tap_state_next[0]), .Z(n8735)) /* synthesis lut_function=(A (B)) */ ;
    defparam n344_bdd_2_lut.init = 16'h8888;
    LUT4 tap_state_3__I_0_199_Mux_1_i15_3_lut_4_lut (.A(\s_status[2] ), .B(\s_status[1] ), 
         .C(\s_status[0] ), .D(\s_status[3] ), .Z(s_drpause_state[1])) /* synthesis lut_function=(!(A (B ((D)+!C))+!A (B+!(C+!(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1097[3] 1162[12])
    defparam tap_state_3__I_0_199_Mux_1_i15_3_lut_4_lut.init = 16'h32b3;
    LUT4 s_tap_state_next_1__bdd_3_lut (.A(\s_status[1] ), .B(\s_status[3] ), 
         .C(\s_status[2] ), .Z(n8733)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;
    defparam s_tap_state_next_1__bdd_3_lut.init = 16'h0404;
    LUT4 s_tap_state_next_1__bdd_4_lut (.A(\s_status[1] ), .B(\s_status[3] ), 
         .C(\s_status[2] ), .D(\s_status[0] ), .Z(n8732)) /* synthesis lut_function=(A (B (C (D)))+!A !(B+((D)+!C))) */ ;
    defparam s_tap_state_next_1__bdd_4_lut.init = 16'h8010;
    LUT4 s_status_3__bdd_4_lut_6188 (.A(\s_status[3] ), .B(\s_status[2] ), 
         .C(\s_status[1] ), .D(\s_status[0] ), .Z(s_irscan_state[2])) /* synthesis lut_function=(A (B ((D)+!C))+!A (B ((D)+!C)+!B !(C+(D)))) */ ;
    defparam s_status_3__bdd_4_lut_6188.init = 16'hcc0d;
    PFUMX i6405 (.BLUT(n8054), .ALUT(n8053), .C0(s_tap_state_next[2]), 
          .Z(s_mtms));
    LUT4 s_opc_2__I_0_235_i4_2_lut_rep_150_4_lut_3_lut (.A(n8330), .B(n2159), 
         .C(n8331), .Z(n8264)) /* synthesis lut_function=(A+!(B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam s_opc_2__I_0_235_i4_2_lut_rep_150_4_lut_3_lut.init = 16'hbfbf;
    LUT4 s_opc_2__I_0_232_i4_2_lut_rep_154_4_lut_3_lut (.A(n8330), .B(n2159), 
         .C(n8331), .Z(n8268)) /* synthesis lut_function=(((C)+!B)+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam s_opc_2__I_0_232_i4_2_lut_rep_154_4_lut_3_lut.init = 16'hf7f7;
    LUT4 i3259_3_lut_4_lut (.A(s_en_r0), .B(n8228), .C(n8212), .D(\s_offs_r[1] ), 
         .Z(n2503)) /* synthesis lut_function=(!(A (C (D)+!C !(D))+!A (B+(C (D)+!C !(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1472[16] 1473[55])
    defparam i3259_3_lut_4_lut.init = 16'h0bb0;
    LUT4 i3236_3_lut_4_lut (.A(s_en_r0), .B(n8228), .C(s_offs_inc), .D(\s_offs_r[0] ), 
         .Z(n2478)) /* synthesis lut_function=(!(A (C (D)+!C !(D))+!A (B+(C (D)+!C !(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1472[16] 1473[55])
    defparam i3236_3_lut_4_lut.init = 16'h0bb0;
    LUT4 s_status_2__bdd_4_lut_6425 (.A(\s_status[2] ), .B(\s_status[3] ), 
         .C(\s_status[1] ), .D(\s_status[0] ), .Z(s_drscan_state[1])) /* synthesis lut_function=(!(A (B (C))+!A (B (C+!(D))+!B (C (D))))) */ ;
    defparam s_status_2__bdd_4_lut_6425.init = 16'h2f3b;
    LUT4 s_status_3__bdd_4_lut_6381 (.A(\s_status[3] ), .B(\s_status[2] ), 
         .C(\s_status[1] ), .D(\s_status[0] ), .Z(s_irpause_state[2])) /* synthesis lut_function=(A (B ((D)+!C)+!B !(C+(D)))+!A (B ((D)+!C)+!B !(C))) */ ;
    defparam s_status_3__bdd_4_lut_6381.init = 16'hcd0f;
    LUT4 i2_3_lut_4_lut (.A(s_en_r0), .B(n8228), .C(tckn_stb), .D(s_en_r1), 
         .Z(mclk_c_enable_137)) /* synthesis lut_function=(A (C (D))+!A (B+(C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1472[16] 1473[55])
    defparam i2_3_lut_4_lut.init = 16'hf444;
    PFUMX i6396 (.BLUT(n8044), .ALUT(n8041), .C0(n8845), .Z(n8045));
    LUT4 mux_564_i1_3_lut_rep_165 (.A(n182), .B(n8849), .C(n2159), .Z(n8279)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam mux_564_i1_3_lut_rep_165.init = 16'hcaca;
    LUT4 i1_2_lut_4_lut_3_lut (.A(n8849), .B(n2159), .C(n8330), .Z(n2099)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam i1_2_lut_4_lut_3_lut.init = 16'h0808;
    LUT4 i3438_3_lut (.A(\s_status[2] ), .B(\s_status[1] ), .C(\s_status[0] ), 
         .Z(s_reset_state[2])) /* synthesis lut_function=(A ((C)+!B)+!A !(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1243[3] 1308[12])
    defparam i3438_3_lut.init = 16'hb3b3;
    FD1S3AX s_exec_r1_189 (.D(s_tap_busy), .CK(mclk_c), .Q(s_exec_en)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(259[3] 269[10])
    defparam s_exec_r1_189.GSR = "ENABLED";
    FD1P3AY s_tap_state__i1 (.D(s_tap_state_next[0]), .SP(tckn_stb), .CK(mclk_c), 
            .Q(\s_status[0] )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_tap_state__i1.GSR = "ENABLED";
    FD1P3IX s_cnt_reg__i7 (.D(n333[7]), .SP(mclk_c_enable_114), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i7.GSR = "ENABLED";
    FD1P3IX s_cnt_reg__i6 (.D(n333[6]), .SP(mclk_c_enable_114), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i6.GSR = "ENABLED";
    FD1S3AX s_shift_en_r0_197 (.D(s_shift_en), .CK(mclk_c), .Q(s_shift_en_r0)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1477[3] 1481[10])
    defparam s_shift_en_r0_197.GSR = "ENABLED";
    LUT4 i3158_2_lut_4_lut (.A(n8263), .B(n7191), .C(n7161), .D(n8252), 
         .Z(n4856)) /* synthesis lut_function=(A (B (D)+!B ((D)+!C))+!A (D)) */ ;
    defparam i3158_2_lut_4_lut.init = 16'hff02;
    LUT4 i871_2_lut_4_lut (.A(n8263), .B(n7191), .C(n7161), .D(\s_cfg[6] ), 
         .Z(n2463)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;
    defparam i871_2_lut_4_lut.init = 16'h0002;
    FD1P3IX s_cnt_reg__i5 (.D(n333[5]), .SP(mclk_c_enable_114), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i5.GSR = "ENABLED";
    FD1P3IX s_cnt_reg__i4 (.D(n333[4]), .SP(mclk_c_enable_114), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i4.GSR = "ENABLED";
    FD1P3IX s_cnt_reg__i3 (.D(n333[3]), .SP(mclk_c_enable_114), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i3.GSR = "ENABLED";
    FD1P3IX s_cnt_reg__i2 (.D(n333[2]), .SP(mclk_c_enable_114), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i2.GSR = "ENABLED";
    FD1P3IX s_cnt_reg__i1 (.D(n333[1]), .SP(mclk_c_enable_114), .CD(busy_N_1509), 
            .CK(mclk_c), .Q(s_cnt_reg[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(417[3] 425[10])
    defparam s_cnt_reg__i1.GSR = "ENABLED";
    FD1S3AX s_exec_r0_188 (.D(n4029), .CK(mclk_c), .Q(s_tap_busy)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(259[3] 269[10])
    defparam s_exec_r0_188.GSR = "ENABLED";
    LUT4 i5531_4_lut (.A(s_end_state[0]), .B(s_end_state[1]), .C(\s_status[0] ), 
         .D(\s_status[1] ), .Z(n7191)) /* synthesis lut_function=(!(A (B (C (D))+!B !((D)+!C))+!A !(B (C+!(D))+!B (C+(D))))) */ ;
    defparam i5531_4_lut.init = 16'h7bde;
    LUT4 i5504_4_lut (.A(s_end_state[2]), .B(s_end_state[3]), .C(\s_status[2] ), 
         .D(\s_status[3] ), .Z(n7161)) /* synthesis lut_function=(!(A (B (C (D))+!B !((D)+!C))+!A !(B (C+!(D))+!B (C+(D))))) */ ;
    defparam i5504_4_lut.init = 16'h7bde;
    LUT4 i1_4_lut (.A(s_tap_busy), .B(n182), .C(n3068), .D(n2159), .Z(n7105)) /* synthesis lut_function=(A (B (C+!(D))+!B (C (D)))) */ ;
    defparam i1_4_lut.init = 16'ha088;
    LUT4 i1_3_lut_adj_173 (.A(busy_N_1509), .B(tckn_stb), .C(n2), .Z(mclk_c_enable_114)) /* synthesis lut_function=(A+(B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(263[7:45])
    defparam i1_3_lut_adj_173.init = 16'heaea;
    LUT4 i1_2_lut_rep_114 (.A(s_shift_en_r0), .B(s_shift_en), .Z(n8228)) /* synthesis lut_function=(!(A+!(B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1472[16] 1473[55])
    defparam i1_2_lut_rep_114.init = 16'h4444;
    LUT4 i1_2_lut_rep_107_3_lut (.A(s_shift_en_r0), .B(s_shift_en), .C(s_en_r0), 
         .Z(n8221)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1472[16] 1473[55])
    defparam i1_2_lut_rep_107_3_lut.init = 16'h0404;
    LUT4 s_status_0__bdd_4_lut_6495 (.A(\s_status[0] ), .B(\s_status[3] ), 
         .C(\s_status[2] ), .D(\s_status[1] ), .Z(s_drscan_state[2])) /* synthesis lut_function=(A (C)+!A !(B (D)+!B ((D)+!C))) */ ;
    defparam s_status_0__bdd_4_lut_6495.init = 16'ha0f4;
    LUT4 i1328_4_lut_4_lut (.A(\s_status[0] ), .B(n8298), .C(\s_status[3] ), 
         .D(n7189), .Z(n126)) /* synthesis lut_function=(A (B (C))+!A (B (C)+!B !(C+!(D)))) */ ;
    defparam i1328_4_lut_4_lut.init = 16'hc1c0;
    LUT4 i1317_3_lut_rep_159_4_lut (.A(n8330), .B(n8331), .C(n2159), .D(n182), 
         .Z(n8273)) /* synthesis lut_function=(A (C+(D))+!A (B (C+(D))+!B !(C+!(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(325[26:45])
    defparam i1317_3_lut_rep_159_4_lut.init = 16'hefe0;
    LUT4 s_status_3__bdd_4_lut_6390 (.A(\s_status[3] ), .B(\s_status[0] ), 
         .C(\s_status[2] ), .D(\s_status[1] ), .Z(s_runtest_state[0])) /* synthesis lut_function=(!(A (B (C+(D))+!B !((D)+!C))+!A (B (C+(D))))) */ ;
    defparam s_status_3__bdd_4_lut_6390.init = 16'h331f;
    LUT4 i1370_2_lut_3_lut (.A(n8330), .B(n8331), .C(n8849), .Z(n3068)) /* synthesis lut_function=(A+(B+(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(325[26:45])
    defparam i1370_2_lut_3_lut.init = 16'hfefe;
    LUT4 i3459_3_lut_rep_160_4_lut (.A(n8330), .B(n8331), .C(n2159), .D(n182), 
         .Z(n8274)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A !(C+!(D))) */ ;
    defparam i3459_3_lut_rep_160_4_lut.init = 16'h8f80;
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
    LUT4 tdi_en_I_159_2_lut (.A(s_pos_goto_en), .B(s_exec_en), .Z(tdi_en_N_1474)) /* synthesis lut_function=(!(A+!(B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(372[10:53])
    defparam tdi_en_I_159_2_lut.init = 16'h4444;
    LUT4 i3248_2_lut (.A(n2108[0]), .B(n8332), .Z(s_pos_goto_en_N_1480)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(374[5] 395[12])
    defparam i3248_2_lut.init = 16'h8888;
    LUT4 i6102_4_lut_4_lut (.A(n8239), .B(n8262), .C(n8246), .D(n8263), 
         .Z(n7245)) /* synthesis lut_function=(A (D)+!A (B (D)+!B ((D)+!C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6102_4_lut_4_lut.init = 16'hff01;
    LUT4 i3_4_lut (.A(n4856), .B(\s_cfg[6] ), .C(s_pos_goto_en), .D(s_exec_en), 
         .Z(mclk_c_enable_45)) /* synthesis lut_function=(!(A+(B+(C+!(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(372[10:53])
    defparam i3_4_lut.init = 16'h0100;
    LUT4 i6112_4_lut_4_lut (.A(n8239), .B(n7250), .C(n8246), .D(n8263), 
         .Z(n7256)) /* synthesis lut_function=(A (C+(D))+!A (B+(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6112_4_lut_4_lut.init = 16'hfff4;
    FD1P3AY s_tap_state__i2 (.D(s_tap_state_next[1]), .SP(tckn_stb), .CK(mclk_c), 
            .Q(\s_status[1] )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_tap_state__i2.GSR = "ENABLED";
    LUT4 i2307_4_lut_4_lut (.A(n8240), .B(n8262), .C(n8246), .D(n5), 
         .Z(n4012)) /* synthesis lut_function=(A (C)+!A (B (C)+!B (C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[8:27])
    defparam i2307_4_lut_4_lut.init = 16'hf1f0;
    LUT4 i2168_2_lut_rep_184 (.A(\s_status[1] ), .B(\s_status[2] ), .Z(n8298)) /* synthesis lut_function=(A+!(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1024[3] 1089[12])
    defparam i2168_2_lut_rep_184.init = 16'hbbbb;
    LUT4 mux_177_Mux_3_i236_3_lut_3_lut_4_lut (.A(\s_status[1] ), .B(\s_status[2] ), 
         .C(\s_status[3] ), .D(\s_status[0] ), .Z(s_drscan_state[3])) /* synthesis lut_function=(A (C)+!A !(B (C+(D))+!B !(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1024[3] 1089[12])
    defparam mux_177_Mux_3_i236_3_lut_3_lut_4_lut.init = 16'hb0b4;
    LUT4 i1_2_lut_rep_177_3_lut (.A(\s_status[1] ), .B(\s_status[2] ), .C(\s_status[0] ), 
         .Z(n8291)) /* synthesis lut_function=(A+((C)+!B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1024[3] 1089[12])
    defparam i1_2_lut_rep_177_3_lut.init = 16'hfbfb;
    LUT4 i1_2_lut_3_lut (.A(\s_status[1] ), .B(\s_status[2] ), .C(\s_status[0] ), 
         .Z(n4)) /* synthesis lut_function=(!(A (C)+!A (B+(C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1024[3] 1089[12])
    defparam i1_2_lut_3_lut.init = 16'h0b0b;
    LUT4 i6108_4_lut_4_lut (.A(n8239), .B(n7231), .C(n8246), .D(n8263), 
         .Z(n7241)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6108_4_lut_4_lut.init = 16'hfffe;
    LUT4 s_opc_1__bdd_4_lut (.A(n8844), .B(\s_status[0] ), .C(\s_status[3] ), 
         .D(\s_status[2] ), .Z(n8040)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (C (D))))) */ ;
    defparam s_opc_1__bdd_4_lut.init = 16'h4008;
    LUT4 n344_bdd_2_lut_6401 (.A(n8040), .B(s_cnt_ovf), .Z(n8041)) /* synthesis lut_function=(A (B)) */ ;
    defparam n344_bdd_2_lut_6401.init = 16'h8888;
    FD1P3AY s_tap_state__i3 (.D(s_tap_state_next[2]), .SP(tckn_stb), .CK(mclk_c), 
            .Q(\s_status[2] )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_tap_state__i3.GSR = "ENABLED";
    FD1P3AY s_tap_state__i4 (.D(s_tap_state_next[3]), .SP(tckn_stb), .CK(mclk_c), 
            .Q(\s_status[3] )) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam s_tap_state__i4.GSR = "ENABLED";
    LUT4 s_opc_0__bdd_4_lut_6411 (.A(n8279), .B(\s_status[3] ), .C(\s_status[2] ), 
         .D(s_en_r1), .Z(n8042)) /* synthesis lut_function=(!(A ((C+!(D))+!B)+!A (B+(C+!(D))))) */ ;
    defparam s_opc_0__bdd_4_lut_6411.init = 16'h0900;
    CCU2D s_cnt_reg_7__I_0_8 (.A0(s_cnt_top[1]), .B0(s_cnt_reg[1]), .C0(s_cnt_top[0]), 
          .D0(s_cnt_reg[0]), .A1(GND_net), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6408), .S1(s_cnt_ovf));
    defparam s_cnt_reg_7__I_0_8.INIT0 = 16'h9009;
    defparam s_cnt_reg_7__I_0_8.INIT1 = 16'hFFFF;
    defparam s_cnt_reg_7__I_0_8.INJECT1_0 = "YES";
    defparam s_cnt_reg_7__I_0_8.INJECT1_1 = "NO";
    CCU2D s_cnt_reg_7__I_0_7 (.A0(s_cnt_top[5]), .B0(s_cnt_reg[5]), .C0(s_cnt_top[4]), 
          .D0(s_cnt_reg[4]), .A1(s_cnt_top[3]), .B1(s_cnt_reg[3]), .C1(s_cnt_top[2]), 
          .D1(s_cnt_reg[2]), .CIN(n6407), .COUT(n6408));
    defparam s_cnt_reg_7__I_0_7.INIT0 = 16'h9009;
    defparam s_cnt_reg_7__I_0_7.INIT1 = 16'h9009;
    defparam s_cnt_reg_7__I_0_7.INJECT1_0 = "YES";
    defparam s_cnt_reg_7__I_0_7.INJECT1_1 = "YES";
    CCU2D s_cnt_reg_7__I_0_0 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_top[7]), .B1(s_cnt_reg[7]), .C1(s_cnt_top[6]), 
          .D1(s_cnt_reg[6]), .COUT(n6407));   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(428[24:45])
    defparam s_cnt_reg_7__I_0_0.INIT0 = 16'hF000;
    defparam s_cnt_reg_7__I_0_0.INIT1 = 16'h9009;
    defparam s_cnt_reg_7__I_0_0.INJECT1_0 = "NO";
    defparam s_cnt_reg_7__I_0_0.INJECT1_1 = "YES";
    LUT4 n4_bdd_4_lut (.A(s_tap_state_next[3]), .B(n7167), .C(s_tap_state_next[1]), 
         .D(\s_status[3] ), .Z(n8054)) /* synthesis lut_function=(!(A ((C+!(D))+!B)+!A ((C+(D))+!B))) */ ;
    defparam n4_bdd_4_lut.init = 16'h0804;
    LUT4 n8052_bdd_3_lut (.A(n8052), .B(n8735), .C(s_tap_state_next[3]), 
         .Z(n8053)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam n8052_bdd_3_lut.init = 16'hcaca;
    LUT4 n4_bdd_4_lut_6404 (.A(n4_adj_1590), .B(s_tap_state_next[1]), .C(s_tap_state_next[0]), 
         .D(mtms_N_1486[8]), .Z(n8052)) /* synthesis lut_function=(A ((C (D))+!B)+!A (B (C (D)))) */ ;
    defparam n4_bdd_4_lut_6404.init = 16'he222;
    LUT4 n182_bdd_4_lut_6500 (.A(n8330), .B(n2159), .C(n8331), .D(n8849), 
         .Z(n8332)) /* synthesis lut_function=(!(A ((C (D))+!B)+!A !(B (C+(D))))) */ ;
    defparam n182_bdd_4_lut_6500.init = 16'h4cc8;
    LUT4 i1_2_lut_4_lut (.A(\s_status[0] ), .B(\s_status[1] ), .C(\s_status[2] ), 
         .D(\s_status[3] ), .Z(mtms_N_1486[7])) /* synthesis lut_function=(!(((C+!(D))+!B)+!A)) */ ;
    defparam i1_2_lut_4_lut.init = 16'h0800;
    LUT4 i2843_4_lut_4_lut (.A(\s_status[0] ), .B(\s_status[1] ), .C(\s_status[2] ), 
         .D(s_tap_state_next[0]), .Z(n7167)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A !(B (D)))) */ ;
    defparam i2843_4_lut_4_lut.init = 16'h4408;
    LUT4 mux_564_i3_3_lut_rep_219 (.A(n182), .B(n8331), .C(n2159), .Z(n8845)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam mux_564_i3_3_lut_rep_219.init = 16'hcaca;
    LUT4 s_tap_busy_I_0_1_lut_rep_203 (.A(s_tap_busy), .Z(n8317)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(890[16:30])
    defparam s_tap_busy_I_0_1_lut_rep_203.init = 16'h5555;
    LUT4 i2_3_lut_3_lut (.A(s_tap_busy), .B(out_get_N_312), .C(n7217), 
         .Z(busy_N_1509)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(890[16:30])
    defparam i2_3_lut_3_lut.init = 16'h0404;
    LUT4 i3184_3_lut_3_lut_4_lut (.A(\s_status[2] ), .B(\s_status[3] ), 
         .C(\s_status[0] ), .D(\s_status[1] ), .Z(n1655[8])) /* synthesis lut_function=(!(A+(B+!(C (D)+!C !(D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1097[3] 1162[12])
    defparam i3184_3_lut_3_lut_4_lut.init = 16'h1001;
    LUT4 i3173_2_lut_3_lut (.A(\s_status[2] ), .B(\s_status[3] ), .C(\s_status[1] ), 
         .Z(mtms_N_1486[10])) /* synthesis lut_function=(!(A+(B+(C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1097[3] 1162[12])
    defparam i3173_2_lut_3_lut.init = 16'h0101;
    LUT4 s_status_0__bdd_4_lut_6664 (.A(\s_status[0] ), .B(\s_status[3] ), 
         .C(\s_status[1] ), .D(\s_status[2] ), .Z(s_irscan_state[0])) /* synthesis lut_function=(!(A (C)+!A (B (C+!(D))+!B !(C+!(D))))) */ ;
    defparam s_status_0__bdd_4_lut_6664.init = 16'h1e1b;
    LUT4 i3386_3_lut_4_lut (.A(\s_status[3] ), .B(\s_status[2] ), .C(\s_status[0] ), 
         .D(\s_status[1] ), .Z(s_reset_state[0])) /* synthesis lut_function=(A (B+!(C (D)))+!A !(C (D))) */ ;
    defparam i3386_3_lut_4_lut.init = 16'h8fff;
    LUT4 i6016_3_lut_4_lut_4_lut (.A(n8268), .B(n514[1]), .C(s_drscan_state[1]), 
         .D(n8279), .Z(n519[1])) /* synthesis lut_function=(A (B)+!A (B (C+(D))+!B !((D)+!C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6016_3_lut_4_lut_4_lut.init = 16'hccd8;
    LUT4 i2323_3_lut (.A(n8224), .B(busy_N_1509), .C(n7105), .Z(n4029)) /* synthesis lut_function=(A (B)+!A (B+(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(259[3] 269[10])
    defparam i2323_3_lut.init = 16'hdcdc;
    LUT4 mux_155_i1_3_lut_4_lut_4_lut (.A(n8268), .B(\s_status[0] ), .C(s_irscan_state[1]), 
         .D(n8279), .Z(n504[0])) /* synthesis lut_function=(A (B)+!A (B (C+(D))+!B !((D)+!C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam mux_155_i1_3_lut_4_lut_4_lut.init = 16'hccd8;
    LUT4 i3323_2_lut_3_lut (.A(\s_status[3] ), .B(\s_status[0] ), .C(\s_status[1] ), 
         .Z(n5022)) /* synthesis lut_function=(A (B+!(C))+!A !(C)) */ ;
    defparam i3323_2_lut_3_lut.init = 16'h8f8f;
    CCU2D add_108_9 (.A0(s_cnt_reg[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n6512), 
          .S0(n333[7]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_108_9.INIT0 = 16'h5aaa;
    defparam add_108_9.INIT1 = 16'h0000;
    defparam add_108_9.INJECT1_0 = "NO";
    defparam add_108_9.INJECT1_1 = "NO";
    CCU2D add_108_7 (.A0(s_cnt_reg[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_cnt_reg[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n6511), .COUT(n6512), .S0(n333[5]), .S1(n333[6]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_108_7.INIT0 = 16'h5aaa;
    defparam add_108_7.INIT1 = 16'h5aaa;
    defparam add_108_7.INJECT1_0 = "NO";
    defparam add_108_7.INJECT1_1 = "NO";
    CCU2D add_108_5 (.A0(s_cnt_reg[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_cnt_reg[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n6510), .COUT(n6511), .S0(n333[3]), .S1(n333[4]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_108_5.INIT0 = 16'h5aaa;
    defparam add_108_5.INIT1 = 16'h5aaa;
    defparam add_108_5.INJECT1_0 = "NO";
    defparam add_108_5.INJECT1_1 = "NO";
    CCU2D add_108_3 (.A0(s_cnt_reg[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_cnt_reg[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n6509), .COUT(n6510), .S0(n333[1]), .S1(n333[2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_108_3.INIT0 = 16'h5aaa;
    defparam add_108_3.INIT1 = 16'h5aaa;
    defparam add_108_3.INJECT1_0 = "NO";
    defparam add_108_3.INJECT1_1 = "NO";
    LUT4 i1231_3_lut_3_lut_4_lut (.A(\s_status[3] ), .B(\s_status[0] ), 
         .C(\s_status[2] ), .D(\s_status[1] ), .Z(s_irpause_state[1])) /* synthesis lut_function=(!(A (B (C (D))+!B ((D)+!C))+!A ((D)+!C))) */ ;
    defparam i1231_3_lut_3_lut_4_lut.init = 16'h08f8;
    CCU2D add_108_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_cnt_reg[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n6509), .S1(n333[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam add_108_1.INIT0 = 16'hF000;
    defparam add_108_1.INIT1 = 16'h5555;
    defparam add_108_1.INJECT1_0 = "NO";
    defparam add_108_1.INJECT1_1 = "NO";
    LUT4 i1302_4_lut_3_lut (.A(\s_status[0] ), .B(\s_status[1] ), .C(\s_status[3] ), 
         .Z(n3000)) /* synthesis lut_function=(A ((C)+!B)+!A !(B+!(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1389[3] 1454[12])
    defparam i1302_4_lut_3_lut.init = 16'hb2b2;
    PFUMX i6742 (.BLUT(n8733), .ALUT(n8732), .C0(s_tap_state_next[1]), 
          .Z(n8734));
    LUT4 i6005_3_lut_4_lut_4_lut (.A(n8268), .B(s_irpause_state[0]), .C(s_irscan_state[0]), 
         .D(n8279), .Z(n509[0])) /* synthesis lut_function=(A (B)+!A (B (C+!(D))+!B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[8:27])
    defparam i6005_3_lut_4_lut_4_lut.init = 16'hd8cc;
    LUT4 i3407_2_lut_3_lut_4_lut (.A(\s_status[1] ), .B(\s_status[2] ), 
         .C(n8273), .D(n8279), .Z(n5106)) /* synthesis lut_function=(!(A+((C+!(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1316[3] 1381[12])
    defparam i3407_2_lut_3_lut_4_lut.init = 16'h0400;
    LUT4 i2_2_lut_4_lut_4_lut (.A(\s_status[3] ), .B(\s_status[2] ), .C(\s_status[1] ), 
         .D(\s_status[0] ), .Z(mtms_N_1486[11])) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(362[3] 401[10])
    defparam i2_2_lut_4_lut_4_lut.init = 16'h4000;
    LUT4 n8043_bdd_2_lut_3_lut (.A(n8042), .B(\s_status[0] ), .C(s_en_r0), 
         .Z(n8044)) /* synthesis lut_function=(!((B+(C))+!A)) */ ;
    defparam n8043_bdd_2_lut_3_lut.init = 16'h0202;
    LUT4 i1_2_lut_rep_138 (.A(s_exec_en), .B(n7105), .Z(n8252)) /* synthesis lut_function=(!(A+!(B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(273[16:59])
    defparam i1_2_lut_rep_138.init = 16'h4444;
    PFUMX i6177 (.BLUT(n7738), .ALUT(s_reset_state[2]), .C0(n4012), .Z(n7739));
    LUT4 i1273_2_lut_3_lut (.A(s_exec_en), .B(n7105), .C(\s_cfg[6] ), 
         .Z(mclk_c_enable_25)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(273[16:59])
    defparam i1273_2_lut_3_lut.init = 16'h0404;
    LUT4 mux_562_i2_3_lut_rep_216 (.A(s_insn_r0[12]), .B(s_insn_r0[13]), 
         .C(s_insn_r0[14]), .Z(n8330)) /* synthesis lut_function=(A (B (C))+!A (B+!(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam mux_562_i2_3_lut_rep_216.init = 16'hc5c5;
    LUT4 i3350_3_lut_rep_217 (.A(s_insn_r0[13]), .B(s_insn_r0[14]), .C(s_insn_r0[12]), 
         .Z(n8331)) /* synthesis lut_function=(!(A (B)+!A !((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(315[3] 322[14])
    defparam i3350_3_lut_rep_217.init = 16'h7373;
    LUT4 i3303_2_lut_3_lut_4_lut (.A(n8845), .B(n8844), .C(s_insn_r0[0]), 
         .D(n4959), .Z(s_cnt_top[0])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i3303_2_lut_3_lut_4_lut.init = 16'h00b0;
    LUT4 i1_2_lut_3_lut_4_lut (.A(n8845), .B(n8844), .C(s_insn_r0[3]), 
         .D(n4959), .Z(s_cnt_top[3])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i1_2_lut_3_lut_4_lut.init = 16'h00b0;
    LUT4 i1_2_lut_3_lut_4_lut_adj_174 (.A(n8845), .B(n8844), .C(s_insn_r0[5]), 
         .D(n4959), .Z(s_cnt_top[5])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_174.init = 16'h00b0;
    LUT4 i1_2_lut_3_lut_4_lut_adj_175 (.A(n8845), .B(n8844), .C(s_insn_r0[4]), 
         .D(n4959), .Z(s_cnt_top[4])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_175.init = 16'h00b0;
    LUT4 i1_2_lut_3_lut_adj_176 (.A(n8845), .B(n8844), .C(s_insn_r0[4]), 
         .Z(s_ptr_addr[5])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i1_2_lut_3_lut_adj_176.init = 16'h4040;
    LUT4 i1_2_lut_3_lut_adj_177 (.A(n8845), .B(n8844), .C(s_insn_r0[3]), 
         .Z(s_ptr_addr[4])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i1_2_lut_3_lut_adj_177.init = 16'h4040;
    LUT4 i3396_2_lut_3_lut (.A(n8845), .B(n8844), .C(s_insn_r0[2]), .Z(s_ptr_addr[3])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i3396_2_lut_3_lut.init = 16'h4040;
    LUT4 i3392_2_lut_3_lut_4_lut (.A(n8845), .B(n8844), .C(s_insn_r0[1]), 
         .D(n4959), .Z(s_cnt_top[1])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i3392_2_lut_3_lut_4_lut.init = 16'h00b0;
    LUT4 i3304_2_lut_3_lut (.A(n8845), .B(n8844), .C(s_insn_r0[0]), .Z(s_ptr_addr[1])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i3304_2_lut_3_lut.init = 16'h4040;
    LUT4 i1_2_lut_3_lut_adj_178 (.A(n8845), .B(n8844), .C(s_insn_r0[7]), 
         .Z(s_ptr_addr[8])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i1_2_lut_3_lut_adj_178.init = 16'h4040;
    LUT4 i3397_2_lut_3_lut (.A(n8845), .B(n8844), .C(s_insn_r0[6]), .Z(s_ptr_addr[7])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i3397_2_lut_3_lut.init = 16'h4040;
    LUT4 i1_2_lut_3_lut_adj_179 (.A(n8845), .B(n8844), .C(s_insn_r0[5]), 
         .Z(s_ptr_addr[6])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i1_2_lut_3_lut_adj_179.init = 16'h4040;
    LUT4 i3393_2_lut_3_lut_4_lut (.A(n8845), .B(n8844), .C(s_insn_r0[2]), 
         .D(n4959), .Z(s_cnt_top[2])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i3393_2_lut_3_lut_4_lut.init = 16'h00b0;
    LUT4 insn_reset_I_0_4_lut (.A(n3000), .B(n8246), .C(n8262), .D(\s_status[2] ), 
         .Z(s_cnt_en_N_1514)) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1467[57] 1469[46])
    defparam insn_reset_I_0_4_lut.init = 16'hcac0;
    LUT4 i1_2_lut_3_lut_4_lut_adj_180 (.A(n8845), .B(n8844), .C(s_insn_r0[7]), 
         .D(n4959), .Z(s_cnt_top[7])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_180.init = 16'h00b0;
    LUT4 i3394_2_lut_3_lut_4_lut (.A(n8845), .B(n8844), .C(s_insn_r0[6]), 
         .D(n4959), .Z(s_cnt_top[6])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B+((D)+!C)))) */ ;
    defparam i3394_2_lut_3_lut_4_lut.init = 16'h00b0;
    LUT4 i3395_2_lut_3_lut (.A(n8845), .B(n8844), .C(s_insn_r0[1]), .Z(s_ptr_addr[2])) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i3395_2_lut_3_lut.init = 16'h4040;
    LUT4 mux_156_i2_4_lut (.A(s_irscan_state[1]), .B(n5022), .C(n8246), 
         .D(\s_status[2] ), .Z(n509[1])) /* synthesis lut_function=(A (B ((D)+!C)+!B !(C))+!A (B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1458[62] 1463[45])
    defparam mux_156_i2_4_lut.init = 16'hca0a;
    LUT4 mux_155_i2_4_lut (.A(\s_status[3] ), .B(\s_status[1] ), .C(n8262), 
         .D(n8291), .Z(n504[1])) /* synthesis lut_function=(A (B (C))+!A (B (C+!(D))+!B !(C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1458[62] 1464[45])
    defparam mux_155_i2_4_lut.init = 16'hc0c5;
    LUT4 i2_4_lut (.A(\s_status[3] ), .B(n4), .C(n8240), .D(n8239), 
         .Z(s_shift_en)) /* synthesis lut_function=(!(A (((D)+!C)+!B)+!A !(B (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1472[16] 1473[55])
    defparam i2_4_lut.init = 16'h4480;
    LUT4 i6014_3_lut_4_lut_4_lut (.A(n8264), .B(n504[0]), .C(s_reset_state[0]), 
         .D(n8279), .Z(n519[0])) /* synthesis lut_function=(A (B)+!A (B (C+(D))+!B !((D)+!C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(338[11:28])
    defparam i6014_3_lut_4_lut_4_lut.init = 16'hccd8;
    LUT4 s_drscan_state_2__bdd_3_lut_4_lut (.A(n8279), .B(n8844), .C(n7737), 
         .D(s_drscan_state[2]), .Z(n7738)) /* synthesis lut_function=(A (C)+!A (B (D)+!B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1458[62] 1459[40])
    defparam s_drscan_state_2__bdd_3_lut_4_lut.init = 16'hf4b0;
    LUT4 insn_goto_I_0_238_2_lut_rep_149 (.A(insn_goto_N_1512), .B(s_pos_goto_en), 
         .Z(n8263)) /* synthesis lut_function=(A+(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(404[30] 405[24])
    defparam insn_goto_I_0_238_2_lut_rep_149.init = 16'heeee;
    LUT4 i1281_4_lut (.A(n8343), .B(s_drscan_state[3]), .C(n4016), .D(s_end_state[1]), 
         .Z(n2979)) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1316[3] 1381[12])
    defparam i1281_4_lut.init = 16'hc0ca;
    PFUMX mux_537_i1 (.BLUT(n6648), .ALUT(n6615), .C0(n2099), .Z(n2108[0]));
    LUT4 i6045_4_lut (.A(n126), .B(n2979), .C(n4016), .D(s_end_state[3]), 
         .Z(s_goto_state[3])) /* synthesis lut_function=(A (B+!(C+(D)))+!A (B (C+(D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(1458[22] 1465[15])
    defparam i6045_4_lut.init = 16'hccca;
    LUT4 mux_159_i1_3_lut_4_lut (.A(insn_goto_N_1512), .B(s_pos_goto_en), 
         .C(\s_goto_state[0] ), .D(s_runtest_state[0]), .Z(s_tap_state_next_3__N_1461[0])) /* synthesis lut_function=(A (C)+!A (B (C)+!B (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(404[30] 405[24])
    defparam mux_159_i1_3_lut_4_lut.init = 16'hf1e0;
    LUT4 i1_3_lut_rep_110_4_lut (.A(insn_goto_N_1512), .B(s_pos_goto_en), 
         .C(n7161), .D(n7191), .Z(n8224)) /* synthesis lut_function=(!(A (C+(D))+!A ((C+(D))+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(404[30] 405[24])
    defparam i1_3_lut_rep_110_4_lut.init = 16'h000e;
    LUT4 n7739_bdd_3_lut_4_lut (.A(insn_goto_N_1512), .B(s_pos_goto_en), 
         .C(\s_goto_state[2] ), .D(n7739), .Z(s_tap_state_next[2])) /* synthesis lut_function=(A (C)+!A (B (C)+!B (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(404[30] 405[24])
    defparam n7739_bdd_3_lut_4_lut.init = 16'hf1e0;
    LUT4 s_tap_state_next_3__I_154_i2_3_lut_4_lut (.A(insn_goto_N_1512), .B(s_pos_goto_en), 
         .C(\s_goto_state[1] ), .D(s_tap_state_next_3__N_1461[1]), .Z(s_tap_state_next_3__N_1438[1])) /* synthesis lut_function=(A (C)+!A (B (C)+!B (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(404[30] 405[24])
    defparam s_tap_state_next_3__I_154_i2_3_lut_4_lut.init = 16'hf1e0;
    LUT4 s_opc_2__I_0_235_i5_2_lut_rep_140_3_lut (.A(n8844), .B(n8845), 
         .C(n8279), .Z(n8254)) /* synthesis lut_function=(A+!(B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[8:28])
    defparam s_opc_2__I_0_235_i5_2_lut_rep_140_3_lut.init = 16'hbfbf;
    LUT4 i6088_2_lut_rep_132_2_lut_3_lut (.A(n8844), .B(n8845), .C(n8279), 
         .Z(n8246)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[8:28])
    defparam i6088_2_lut_rep_132_2_lut_3_lut.init = 16'h0404;
    LUT4 i1_2_lut_3_lut_3_lut_3_lut (.A(n8844), .B(n8845), .C(n8279), 
         .Z(n5)) /* synthesis lut_function=(A (C)+!A !(B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[8:28])
    defparam i1_2_lut_3_lut_3_lut_3_lut.init = 16'hb5b5;
    LUT4 n8037_bdd_2_lut_3_lut_3_lut_4_lut_3_lut (.A(n8844), .B(n8845), 
         .C(n8279), .Z(n8038)) /* synthesis lut_function=(A (B (C))+!A !(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[8:28])
    defparam n8037_bdd_2_lut_3_lut_3_lut_4_lut_3_lut.init = 16'h9191;
    PFUMX i6175 (.BLUT(n7736), .ALUT(s_irscan_state[2]), .C0(n8240), .Z(n7737));
    LUT4 i6001_3_lut_4_lut_4_lut (.A(n8279), .B(s_irpause_state[1]), .C(s_drpause_state[1]), 
         .D(n8274), .Z(n514[1])) /* synthesis lut_function=(A (B)+!A (B (C+!(D))+!B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[34:54])
    defparam i6001_3_lut_4_lut_4_lut.init = 16'hd8cc;
    LUT4 i6003_3_lut_4_lut_4_lut (.A(n8279), .B(mtms_N_1486[7]), .C(n1655[8]), 
         .D(n8274), .Z(n1)) /* synthesis lut_function=(A (B)+!A (B (C+!(D))+!B (C (D)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[34:54])
    defparam i6003_3_lut_4_lut_4_lut.init = 16'hd8cc;
    LUT4 i6130_2_lut_2_lut_3_lut_4_lut_3_lut (.A(n8279), .B(n8264), .C(n8274), 
         .Z(n7227)) /* synthesis lut_function=(!(A (B)+!A !(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[34:54])
    defparam i6130_2_lut_2_lut_3_lut_4_lut_3_lut.init = 16'h7272;
    LUT4 i5587_3_lut_3_lut_4_lut_4_lut_4_lut_4_lut (.A(n8279), .B(n8274), 
         .C(n8268), .D(n8264), .Z(n7250)) /* synthesis lut_function=(A ((D)+!C)+!A !(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[34:54])
    defparam i5587_3_lut_3_lut_4_lut_4_lut_4_lut_4_lut.init = 16'hbb1b;
    LUT4 i5568_3_lut_3_lut_4_lut_4_lut_4_lut_4_lut (.A(n8279), .B(n8274), 
         .C(n8268), .D(n8264), .Z(n7231)) /* synthesis lut_function=(A (C (D))+!A !(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(339[34:54])
    defparam i5568_3_lut_3_lut_4_lut_4_lut_4_lut_4_lut.init = 16'hb111;
    PFUMX s_tap_state_next_3__I_0_225_i4 (.BLUT(s_tap_state_next_3__N_1438[3]), 
          .ALUT(s_goto_state[3]), .C0(n8263), .Z(s_tap_state_next[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    PFUMX s_tap_state_next_3__I_0_i4 (.BLUT(mtms_N_1486[11]), .ALUT(mtms_N_1486[10]), 
          .C0(s_tap_state_next[0]), .Z(n4_adj_1590)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=14, LSE_RCOL=33, LSE_LLINE=313, LSE_RLINE=313 */ ;
    LUT4 i3452_2_lut_3_lut (.A(n8844), .B(n8845), .C(n4959), .Z(n5152)) /* synthesis lut_function=(A (B (C))+!A (C)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i3452_2_lut_3_lut.init = 16'hd0d0;
    LUT4 i6096_2_lut_rep_126_2_lut_3_lut (.A(n8844), .B(n8845), .C(n8279), 
         .Z(n8240)) /* synthesis lut_function=(!((B+!(C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6096_2_lut_rep_126_2_lut_3_lut.init = 16'h2020;
    LUT4 i6109_2_lut_3_lut_3_lut_3_lut_3_lut_4_lut_3_lut (.A(n8844), .B(n8845), 
         .C(n8279), .Z(n7258)) /* synthesis lut_function=(!(A (B+!(C))+!A ((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6109_2_lut_3_lut_3_lut_3_lut_3_lut_4_lut_3_lut.init = 16'h2424;
    LUT4 i6128_2_lut_rep_125_2_lut_3_lut (.A(n8844), .B(n8845), .C(n8279), 
         .Z(n8239)) /* synthesis lut_function=(!((B+(C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ecu.vhdl(334[33:52])
    defparam i6128_2_lut_rep_125_2_lut_3_lut.init = 16'h0202;
    PFUMX i6501 (.BLUT(n8341), .ALUT(n8342), .C0(\s_status[1] ), .Z(n8343));
    
endmodule
//
// Verilog Description of module \jtag_enh_ckgen(16) 
//

module \jtag_enh_ckgen(16)  (mclk_c, GND_net, \s_irq[1] , s_ckgen_rtdiv, 
            s_tcken, \s_cfg[5] , tp_rtck_c, s_ckgen_div, \s_irq_en[1] , 
            \irq_N_8[1] , tp_tck_c, mclk_c_enable_115, s_tdien, n7090);
    input mclk_c;
    input GND_net;
    output \s_irq[1] ;
    input [15:0]s_ckgen_rtdiv;
    input s_tcken;
    input \s_cfg[5] ;
    input tp_rtck_c;
    input [15:0]s_ckgen_div;
    input \s_irq_en[1] ;
    output \irq_N_8[1] ;
    output tp_tck_c;
    output mclk_c_enable_115;
    input s_tdien;
    output n7090;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    wire mclk_c_enable_115 /* synthesis is_clock=1, SET_AS_NETWORK=mclk_c_enable_115 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    wire [14:0]s_cnt_lo_r;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(71[9:19])
    
    wire mclk_c_enable_261, n4036;
    wire [14:0]n65;
    wire [14:0]s_cnt_hi_r;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(72[9:19])
    
    wire mclk_c_enable_135, n4077;
    wire [14:0]n65_adj_1589;
    
    wire n6404, rtcktmo_N_629, n6398;
    wire [14:0]s_cmp_lo;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(74[9:17])
    
    wire n6399, n6464, n6465, n6403;
    wire [14:0]s_cmp_hi;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(73[9:17])
    wire [14:0]n194;
    
    wire n6463, mclk_c_enable_41, n8310, s_rtcken_reg, s_tck_r0, s_enabled;
    wire [15:0]s_div_lo_r0;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(62[9:20])
    wire [16:0]s_div_lo_r0_14__N_475;
    wire [15:0]s_rtdiv_hi_r0;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(65[9:22])
    wire [15:0]s_rtdiv_lo_r0;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(64[9:22])
    wire [16:0]s_rtdiv_lo_r0_14__N_490;
    wire [14:0]s_div_hi;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(67[9:17])
    wire [15:0]s_div_hi_r0;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(63[9:20])
    wire [14:0]s_div_lo;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(66[9:17])
    wire [14:0]s_rtdiv_hi;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(69[9:19])
    wire [14:0]s_rtdiv_lo;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(68[9:19])
    
    wire s_tcken_r0, s_tcken_r1, s_rtcken_r0, s_rtcken_r1, s_rtck_r0, 
        n6455, n6456, n6454, n6402, mclk_c_enable_159;
    wire [14:0]s_cmp_lo_14__N_550;
    
    wire n8334, n8335, mclk_c_enable_106, s_tck_r0_N_638, s_rtck_r1, 
        s_rtck_r2, n8307, n6397, mclk_c_enable_95, s_tcki_r0, s_tcki_r0_N_639, 
        s_rtck_r1_N_660, n6337, s_tcki_r1, n6462, n6460, n6459, 
        n6458, n1830, n6457, n6484, n6483, n6482, n6481, n6480, 
        n6401, n6479, n6478, n6476, n6475, n6474, n6473, n6472, 
        n6471, n8248, n6470, n6400, n6468, n6467, n6466;
    
    FD1P3IX s_cnt_lo_r_755__i5 (.D(n65[5]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[5])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i5.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_756__i4 (.D(n65_adj_1589[4]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[4])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i4.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_756__i5 (.D(n65_adj_1589[5]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[5])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i5.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_755__i6 (.D(n65[6]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[6])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i6.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_755__i4 (.D(n65[4]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[4])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i4.GSR = "ENABLED";
    CCU2D s_cnt_hi_r_14__I_0_15 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n6404), .S0(rtcktmo_N_629));
    defparam s_cnt_hi_r_14__I_0_15.INIT0 = 16'hFFFF;
    defparam s_cnt_hi_r_14__I_0_15.INIT1 = 16'h0000;
    defparam s_cnt_hi_r_14__I_0_15.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_14__I_0_15.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_14__I_0_13 (.A0(s_cmp_lo[7]), .B0(s_cnt_lo_r[7]), .C0(s_cmp_lo[6]), 
          .D0(s_cnt_lo_r[6]), .A1(s_cmp_lo[5]), .B1(s_cnt_lo_r[5]), .C1(s_cmp_lo[4]), 
          .D1(s_cnt_lo_r[4]), .CIN(n6398), .COUT(n6399));
    defparam s_cnt_lo_r_14__I_0_13.INIT0 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_13.INIT1 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_13.INJECT1_0 = "YES";
    defparam s_cnt_lo_r_14__I_0_13.INJECT1_1 = "YES";
    CCU2D s_cnt_hi_r_756_add_4_7 (.A0(s_cnt_hi_r[5]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[6]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6464), .COUT(n6465), .S0(n65_adj_1589[5]), 
          .S1(n65_adj_1589[6]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756_add_4_7.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_7.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_7.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_756_add_4_7.INJECT1_1 = "NO";
    CCU2D s_cnt_hi_r_14__I_0_15_4815 (.A0(s_cmp_hi[3]), .B0(s_cnt_hi_r[3]), 
          .C0(s_cmp_hi[2]), .D0(s_cnt_hi_r[2]), .A1(s_cmp_hi[1]), .B1(s_cnt_hi_r[1]), 
          .C1(s_cmp_hi[0]), .D1(s_cnt_hi_r[0]), .CIN(n6403), .COUT(n6404));
    defparam s_cnt_hi_r_14__I_0_15_4815.INIT0 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_15_4815.INIT1 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_15_4815.INJECT1_0 = "YES";
    defparam s_cnt_hi_r_14__I_0_15_4815.INJECT1_1 = "YES";
    FD1P3IX s_cnt_hi_r_756__i3 (.D(n65_adj_1589[3]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[3])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i3.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_755__i7 (.D(n65[7]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[7])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i7.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_755__i8 (.D(n65[8]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[8])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i8.GSR = "ENABLED";
    FD1P3AX s_cmp_hi_i0_i0 (.D(n194[0]), .SP(mclk_c_enable_261), .CK(mclk_c), 
            .Q(s_cmp_hi[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_hi_i0_i0.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_755__i9 (.D(n65[9]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[9])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i9.GSR = "ENABLED";
    CCU2D s_cnt_hi_r_756_add_4_5 (.A0(s_cnt_hi_r[3]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[4]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6463), .COUT(n6464), .S0(n65_adj_1589[3]), 
          .S1(n65_adj_1589[4]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756_add_4_5.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_5.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_5.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_756_add_4_5.INJECT1_1 = "NO";
    FD1P3IX s_cnt_hi_r_756__i2 (.D(n65_adj_1589[2]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[2])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i2.GSR = "ENABLED";
    FD1P3IX s_rtck_tmo_117 (.D(s_rtcken_reg), .SP(mclk_c_enable_41), .CD(n8310), 
            .CK(mclk_c), .Q(\s_irq[1] ));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_rtck_tmo_117.GSR = "ENABLED";
    LUT4 i1_2_lut (.A(s_tck_r0), .B(s_enabled), .Z(mclk_c_enable_135)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut.init = 16'h8888;
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
    CCU2D s_cnt_lo_r_755_add_4_5 (.A0(s_cnt_lo_r[3]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[4]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6455), .COUT(n6456), .S0(n65[3]), .S1(n65[4]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755_add_4_5.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_5.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_5.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_755_add_4_5.INJECT1_1 = "NO";
    FD1P3IX s_cnt_hi_r_756__i1 (.D(n65_adj_1589[1]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[1])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i1.GSR = "ENABLED";
    CCU2D s_cnt_lo_r_755_add_4_3 (.A0(s_cnt_lo_r[1]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[2]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6454), .COUT(n6455), .S0(n65[1]), .S1(n65[2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755_add_4_3.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_3.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_3.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_755_add_4_3.INJECT1_1 = "NO";
    CCU2D s_cnt_hi_r_14__I_0_13 (.A0(s_cmp_hi[7]), .B0(s_cnt_hi_r[7]), .C0(s_cmp_hi[6]), 
          .D0(s_cnt_hi_r[6]), .A1(s_cmp_hi[5]), .B1(s_cnt_hi_r[5]), .C1(s_cmp_hi[4]), 
          .D1(s_cnt_hi_r[4]), .CIN(n6402), .COUT(n6403));
    defparam s_cnt_hi_r_14__I_0_13.INIT0 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_13.INIT1 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_13.INJECT1_0 = "YES";
    defparam s_cnt_hi_r_14__I_0_13.INJECT1_1 = "YES";
    FD1P3AX s_cmp_lo_i0 (.D(s_cmp_lo_14__N_550[0]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i0.GSR = "ENABLED";
    FD1S3AX s_div_hi_r0_i1 (.D(s_ckgen_div[1]), .CK(mclk_c), .Q(s_div_hi_r0[0])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(101[3] 119[10])
    defparam s_div_hi_r0_i1.GSR = "ENABLED";
    CCU2D s_cnt_lo_r_755_add_4_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[0]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .COUT(n6454), .S1(n65[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755_add_4_1.INIT0 = 16'hF000;
    defparam s_cnt_lo_r_755_add_4_1.INIT1 = 16'h0555;
    defparam s_cnt_lo_r_755_add_4_1.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_755_add_4_1.INJECT1_1 = "NO";
    PFUMX i6496 (.BLUT(n8334), .ALUT(n8335), .C0(s_tck_r0), .Z(mclk_c_enable_106));
    LUT4 i2_4_lut_4_lut_4_lut_else_4_lut (.A(s_enabled), .B(s_tck_r0_N_638), 
         .C(s_rtck_r1), .D(s_rtck_r2), .Z(n8334)) /* synthesis lut_function=((B+!(C+!(D)))+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i2_4_lut_4_lut_4_lut_else_4_lut.init = 16'hdfdd;
    LUT4 i1_2_lut_adj_171 (.A(\s_irq[1] ), .B(\s_irq_en[1] ), .Z(\irq_N_8[1] )) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut_adj_171.init = 16'h8888;
    FD1P3IX s_cnt_hi_r_756__i14 (.D(n65_adj_1589[14]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[14])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i14.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_756__i13 (.D(n65_adj_1589[13]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[13])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i13.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_756__i12 (.D(n65_adj_1589[12]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[12])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i12.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_756__i11 (.D(n65_adj_1589[11]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[11])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i11.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_756__i10 (.D(n65_adj_1589[10]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[10])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i10.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_756__i9 (.D(n65_adj_1589[9]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[9])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i9.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_756__i8 (.D(n65_adj_1589[8]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[8])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i8.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_756__i7 (.D(n65_adj_1589[7]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[7])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i7.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_756__i6 (.D(n65_adj_1589[6]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[6])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i6.GSR = "ENABLED";
    FD1S3IX s_rtck_r1_111 (.D(s_rtck_r0), .CK(mclk_c), .CD(n8307), .Q(s_rtck_r1));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(144[3] 157[10])
    defparam s_rtck_r1_111.GSR = "ENABLED";
    CCU2D s_cnt_lo_r_14__I_0_11 (.A0(s_cmp_lo[11]), .B0(s_cnt_lo_r[11]), 
          .C0(s_cmp_lo[10]), .D0(s_cnt_lo_r[10]), .A1(s_cmp_lo[9]), .B1(s_cnt_lo_r[9]), 
          .C1(s_cmp_lo[8]), .D1(s_cnt_lo_r[8]), .CIN(n6397), .COUT(n6398));
    defparam s_cnt_lo_r_14__I_0_11.INIT0 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_11.INIT1 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_11.INJECT1_0 = "YES";
    defparam s_cnt_lo_r_14__I_0_11.INJECT1_1 = "YES";
    FD1P3AX s_enabled_115 (.D(s_tcken_r1), .SP(mclk_c_enable_95), .CK(mclk_c), 
            .Q(s_enabled));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_enabled_115.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_755__i11 (.D(n65[11]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[11])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i11.GSR = "ENABLED";
    FD1P3AX s_tcki_r0_118 (.D(s_tcki_r0_N_639), .SP(mclk_c_enable_106), 
            .CK(mclk_c), .Q(s_tcki_r0)) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_tcki_r0_118.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_755__i10 (.D(n65[10]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[10])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i10.GSR = "ENABLED";
    FD1S3IX s_rtck_r2_112 (.D(n6337), .CK(mclk_c), .CD(s_rtck_r1_N_660), 
            .Q(s_rtck_r2));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(144[3] 157[10])
    defparam s_rtck_r2_112.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_755__i12 (.D(n65[12]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[12])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i12.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_755__i13 (.D(n65[13]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[13])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i13.GSR = "ENABLED";
    FD1P3AX s_tck_r0_113 (.D(mclk_c_enable_261), .SP(mclk_c_enable_106), 
            .CK(mclk_c), .Q(s_tck_r0));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_tck_r0_113.GSR = "ENABLED";
    FD1S3IX s_tck_r1_114 (.D(s_tck_r0), .CK(mclk_c), .CD(n8310), .Q(tp_tck_c));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_tck_r1_114.GSR = "ENABLED";
    FD1S3IX s_rtcken_reg_116 (.D(s_rtcken_r1), .CK(mclk_c), .CD(n8310), 
            .Q(s_rtcken_reg));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_rtcken_reg_116.GSR = "ENABLED";
    FD1S3IX s_tcki_r1_119 (.D(s_tcki_r0), .CK(mclk_c), .CD(n8310), .Q(s_tcki_r1));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_tcki_r1_119.GSR = "ENABLED";
    CCU2D s_cnt_hi_r_756_add_4_3 (.A0(s_cnt_hi_r[1]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[2]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6462), .COUT(n6463), .S0(n65_adj_1589[1]), 
          .S1(n65_adj_1589[2]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756_add_4_3.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_3.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_3.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_756_add_4_3.INJECT1_1 = "NO";
    CCU2D s_cnt_hi_r_756_add_4_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[0]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .COUT(n6462), .S1(n65_adj_1589[0]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756_add_4_1.INIT0 = 16'hF000;
    defparam s_cnt_hi_r_756_add_4_1.INIT1 = 16'h0555;
    defparam s_cnt_hi_r_756_add_4_1.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_756_add_4_1.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_755_add_4_15 (.A0(s_cnt_lo_r[13]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[14]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6460), .S0(n65[13]), .S1(n65[14]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755_add_4_15.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_15.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_15.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_755_add_4_15.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_755_add_4_13 (.A0(s_cnt_lo_r[11]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[12]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6459), .COUT(n6460), .S0(n65[11]), .S1(n65[12]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755_add_4_13.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_13.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_13.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_755_add_4_13.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_755_add_4_11 (.A0(s_cnt_lo_r[9]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[10]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6458), .COUT(n6459), .S0(n65[9]), .S1(n65[10]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755_add_4_11.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_11.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_11.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_755_add_4_11.INJECT1_1 = "NO";
    LUT4 i2358_2_lut_4_lut_4_lut (.A(mclk_c_enable_159), .B(s_rtck_r2), 
         .C(s_tck_r0_N_638), .D(s_rtck_r1), .Z(n4036)) /* synthesis lut_function=(!(A+!(B (C+!(D))+!B (C)))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i2358_2_lut_4_lut_4_lut.init = 16'h5054;
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
    FD1P3IX s_cnt_lo_r_755__i14 (.D(n65[14]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[14])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i14.GSR = "ENABLED";
    FD1P3IX s_cnt_hi_r_756__i0 (.D(n65_adj_1589[0]), .SP(mclk_c_enable_135), 
            .CD(n4077), .CK(mclk_c), .Q(s_cnt_hi_r[0])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756__i0.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_755__i0 (.D(n65[0]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[0])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i0.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i1 (.D(s_cmp_lo_14__N_550[1]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[1])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i1.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i2 (.D(s_cmp_lo_14__N_550[2]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[2])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i2.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i3 (.D(s_cmp_lo_14__N_550[3]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[3])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i3.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i4 (.D(s_cmp_lo_14__N_550[4]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[4])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i4.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i5 (.D(s_cmp_lo_14__N_550[5]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[5])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i5.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i6 (.D(s_cmp_lo_14__N_550[6]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[6])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i6.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i7 (.D(s_cmp_lo_14__N_550[7]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[7])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i7.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i8 (.D(s_cmp_lo_14__N_550[8]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[8])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i8.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i9 (.D(s_cmp_lo_14__N_550[9]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[9])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i9.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i10 (.D(s_cmp_lo_14__N_550[10]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[10])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i10.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i11 (.D(s_cmp_lo_14__N_550[11]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[11])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i11.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i12 (.D(s_cmp_lo_14__N_550[12]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[12])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i12.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i13 (.D(s_cmp_lo_14__N_550[13]), .SP(mclk_c_enable_159), 
            .CK(mclk_c), .Q(s_cmp_lo[13])) /* synthesis LSE_LINE_FILE_ID=32, LSE_LCOL=10, LSE_RCOL=31, LSE_LLINE=195, LSE_RLINE=195 */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam s_cmp_lo_i13.GSR = "ENABLED";
    FD1P3AX s_cmp_lo_i14 (.D(s_cmp_lo_14__N_550[14]), .SP(mclk_c_enable_159), 
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
    LUT4 s_div_lo_14__I_0_i1_4_lut (.A(s_rtdiv_lo[0]), .B(s_div_lo[0]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[0])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i1_4_lut.init = 16'hcac0;
    LUT4 i1_2_lut_rep_189 (.A(s_tck_r0), .B(s_enabled), .Z(mclk_c_enable_159)) /* synthesis lut_function=(A+!(B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut_rep_189.init = 16'hbbbb;
    CCU2D s_cnt_lo_r_755_add_4_9 (.A0(s_cnt_lo_r[7]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[8]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6457), .COUT(n6458), .S0(n65[7]), .S1(n65[8]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755_add_4_9.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_9.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_9.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_755_add_4_9.INJECT1_1 = "NO";
    LUT4 i4750_1_lut_rep_166_2_lut (.A(s_tck_r0), .B(s_enabled), .Z(mclk_c_enable_261)) /* synthesis lut_function=(!(A+!(B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i4750_1_lut_rep_166_2_lut.init = 16'h4444;
    LUT4 i1_2_lut_3_lut (.A(s_tck_r0), .B(s_enabled), .C(rtcktmo_N_629), 
         .Z(mclk_c_enable_41)) /* synthesis lut_function=(A (C)+!A !(B+!(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut_3_lut.init = 16'hb0b0;
    LUT4 i1_2_lut_3_lut_adj_172 (.A(s_tck_r0), .B(s_enabled), .C(s_tcken_r1), 
         .Z(s_tcki_r0_N_639)) /* synthesis lut_function=(A (C)+!A !(B+!(C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut_3_lut_adj_172.init = 16'hb0b0;
    CCU2D s_cnt_lo_r_755_add_4_7 (.A0(s_cnt_lo_r[5]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_lo_r[6]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6456), .COUT(n6457), .S0(n65[5]), .S1(n65[6]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755_add_4_7.INIT0 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_7.INIT1 = 16'hfaaa;
    defparam s_cnt_lo_r_755_add_4_7.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_755_add_4_7.INJECT1_1 = "NO";
    CCU2D sub_92_add_2_15 (.A0(s_ckgen_rtdiv[13]), .B0(s_rtdiv_hi[13]), 
          .C0(GND_net), .D0(GND_net), .A1(s_ckgen_rtdiv[14]), .B1(s_rtdiv_hi[14]), 
          .C1(GND_net), .D1(GND_net), .CIN(n6484), .S0(s_rtdiv_lo_r0_14__N_490[13]), 
          .S1(s_rtdiv_lo_r0_14__N_490[14]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_92_add_2_15.INIT0 = 16'h5999;
    defparam sub_92_add_2_15.INIT1 = 16'h5999;
    defparam sub_92_add_2_15.INJECT1_0 = "NO";
    defparam sub_92_add_2_15.INJECT1_1 = "NO";
    CCU2D sub_92_add_2_13 (.A0(s_ckgen_rtdiv[11]), .B0(s_rtdiv_hi[11]), 
          .C0(GND_net), .D0(GND_net), .A1(s_ckgen_rtdiv[12]), .B1(s_rtdiv_hi[12]), 
          .C1(GND_net), .D1(GND_net), .CIN(n6483), .COUT(n6484), .S0(s_rtdiv_lo_r0_14__N_490[11]), 
          .S1(s_rtdiv_lo_r0_14__N_490[12]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_92_add_2_13.INIT0 = 16'h5999;
    defparam sub_92_add_2_13.INIT1 = 16'h5999;
    defparam sub_92_add_2_13.INJECT1_0 = "NO";
    defparam sub_92_add_2_13.INJECT1_1 = "NO";
    CCU2D sub_92_add_2_11 (.A0(s_ckgen_rtdiv[9]), .B0(s_rtdiv_hi[9]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_rtdiv[10]), .B1(s_rtdiv_hi[10]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6482), .COUT(n6483), .S0(s_rtdiv_lo_r0_14__N_490[9]), 
          .S1(s_rtdiv_lo_r0_14__N_490[10]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_92_add_2_11.INIT0 = 16'h5999;
    defparam sub_92_add_2_11.INIT1 = 16'h5999;
    defparam sub_92_add_2_11.INJECT1_0 = "NO";
    defparam sub_92_add_2_11.INJECT1_1 = "NO";
    CCU2D sub_92_add_2_9 (.A0(s_ckgen_rtdiv[7]), .B0(s_rtdiv_hi[7]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_rtdiv[8]), .B1(s_rtdiv_hi[8]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6481), .COUT(n6482), .S0(s_rtdiv_lo_r0_14__N_490[7]), 
          .S1(s_rtdiv_lo_r0_14__N_490[8]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_92_add_2_9.INIT0 = 16'h5999;
    defparam sub_92_add_2_9.INIT1 = 16'h5999;
    defparam sub_92_add_2_9.INJECT1_0 = "NO";
    defparam sub_92_add_2_9.INJECT1_1 = "NO";
    CCU2D sub_92_add_2_7 (.A0(s_ckgen_rtdiv[5]), .B0(s_rtdiv_hi[5]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_rtdiv[6]), .B1(s_rtdiv_hi[6]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6480), .COUT(n6481), .S0(s_rtdiv_lo_r0_14__N_490[5]), 
          .S1(s_rtdiv_lo_r0_14__N_490[6]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_92_add_2_7.INIT0 = 16'h5999;
    defparam sub_92_add_2_7.INIT1 = 16'h5999;
    defparam sub_92_add_2_7.INJECT1_0 = "NO";
    defparam sub_92_add_2_7.INJECT1_1 = "NO";
    CCU2D s_cnt_hi_r_14__I_0_11 (.A0(s_cmp_hi[11]), .B0(s_cnt_hi_r[11]), 
          .C0(s_cmp_hi[10]), .D0(s_cnt_hi_r[10]), .A1(s_cmp_hi[9]), .B1(s_cnt_hi_r[9]), 
          .C1(s_cmp_hi[8]), .D1(s_cnt_hi_r[8]), .CIN(n6401), .COUT(n6402));
    defparam s_cnt_hi_r_14__I_0_11.INIT0 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_11.INIT1 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_11.INJECT1_0 = "YES";
    defparam s_cnt_hi_r_14__I_0_11.INJECT1_1 = "YES";
    CCU2D sub_92_add_2_5 (.A0(s_ckgen_rtdiv[3]), .B0(s_rtdiv_hi[3]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_rtdiv[4]), .B1(s_rtdiv_hi[4]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6479), .COUT(n6480), .S0(s_rtdiv_lo_r0_14__N_490[3]), 
          .S1(s_rtdiv_lo_r0_14__N_490[4]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_92_add_2_5.INIT0 = 16'h5999;
    defparam sub_92_add_2_5.INIT1 = 16'h5999;
    defparam sub_92_add_2_5.INJECT1_0 = "NO";
    defparam sub_92_add_2_5.INJECT1_1 = "NO";
    CCU2D sub_92_add_2_3 (.A0(s_ckgen_rtdiv[1]), .B0(s_rtdiv_hi[1]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_rtdiv[2]), .B1(s_rtdiv_hi[2]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6478), .COUT(n6479), .S0(s_rtdiv_lo_r0_14__N_490[1]), 
          .S1(s_rtdiv_lo_r0_14__N_490[2]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_92_add_2_3.INIT0 = 16'h5999;
    defparam sub_92_add_2_3.INIT1 = 16'h5999;
    defparam sub_92_add_2_3.INJECT1_0 = "NO";
    defparam sub_92_add_2_3.INJECT1_1 = "NO";
    CCU2D sub_92_add_2_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_ckgen_rtdiv[0]), .B1(s_rtdiv_hi[0]), .C1(GND_net), .D1(GND_net), 
          .COUT(n6478), .S1(s_rtdiv_lo_r0_14__N_490[0]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(114[22:37])
    defparam sub_92_add_2_1.INIT0 = 16'h0000;
    defparam sub_92_add_2_1.INIT1 = 16'h5999;
    defparam sub_92_add_2_1.INJECT1_0 = "NO";
    defparam sub_92_add_2_1.INJECT1_1 = "NO";
    CCU2D sub_91_add_2_15 (.A0(s_ckgen_div[13]), .B0(s_div_hi[13]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[14]), .B1(s_div_hi[14]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6476), .S0(s_div_lo_r0_14__N_475[13]), .S1(s_div_lo_r0_14__N_475[14]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_91_add_2_15.INIT0 = 16'h5999;
    defparam sub_91_add_2_15.INIT1 = 16'h5999;
    defparam sub_91_add_2_15.INJECT1_0 = "NO";
    defparam sub_91_add_2_15.INJECT1_1 = "NO";
    CCU2D s_cnt_hi_r_14__I_0_0 (.A0(s_cmp_hi[14]), .B0(s_cnt_hi_r[14]), 
          .C0(GND_net), .D0(GND_net), .A1(s_cmp_hi[13]), .B1(s_cnt_hi_r[13]), 
          .C1(s_cmp_hi[12]), .D1(s_cnt_hi_r[12]), .COUT(n6401));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(205[11:32])
    defparam s_cnt_hi_r_14__I_0_0.INIT0 = 16'h9000;
    defparam s_cnt_hi_r_14__I_0_0.INIT1 = 16'h9009;
    defparam s_cnt_hi_r_14__I_0_0.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_14__I_0_0.INJECT1_1 = "YES";
    CCU2D sub_91_add_2_13 (.A0(s_ckgen_div[11]), .B0(s_div_hi[11]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[12]), .B1(s_div_hi[12]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6475), .COUT(n6476), .S0(s_div_lo_r0_14__N_475[11]), 
          .S1(s_div_lo_r0_14__N_475[12]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_91_add_2_13.INIT0 = 16'h5999;
    defparam sub_91_add_2_13.INIT1 = 16'h5999;
    defparam sub_91_add_2_13.INJECT1_0 = "NO";
    defparam sub_91_add_2_13.INJECT1_1 = "NO";
    CCU2D sub_91_add_2_11 (.A0(s_ckgen_div[9]), .B0(s_div_hi[9]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[10]), .B1(s_div_hi[10]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6474), .COUT(n6475), .S0(s_div_lo_r0_14__N_475[9]), 
          .S1(s_div_lo_r0_14__N_475[10]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_91_add_2_11.INIT0 = 16'h5999;
    defparam sub_91_add_2_11.INIT1 = 16'h5999;
    defparam sub_91_add_2_11.INJECT1_0 = "NO";
    defparam sub_91_add_2_11.INJECT1_1 = "NO";
    CCU2D sub_91_add_2_9 (.A0(s_ckgen_div[7]), .B0(s_div_hi[7]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[8]), .B1(s_div_hi[8]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6473), .COUT(n6474), .S0(s_div_lo_r0_14__N_475[7]), 
          .S1(s_div_lo_r0_14__N_475[8]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_91_add_2_9.INIT0 = 16'h5999;
    defparam sub_91_add_2_9.INIT1 = 16'h5999;
    defparam sub_91_add_2_9.INJECT1_0 = "NO";
    defparam sub_91_add_2_9.INJECT1_1 = "NO";
    CCU2D sub_91_add_2_7 (.A0(s_ckgen_div[5]), .B0(s_div_hi[5]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[6]), .B1(s_div_hi[6]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6472), .COUT(n6473), .S0(s_div_lo_r0_14__N_475[5]), 
          .S1(s_div_lo_r0_14__N_475[6]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_91_add_2_7.INIT0 = 16'h5999;
    defparam sub_91_add_2_7.INIT1 = 16'h5999;
    defparam sub_91_add_2_7.INJECT1_0 = "NO";
    defparam sub_91_add_2_7.INJECT1_1 = "NO";
    CCU2D sub_91_add_2_5 (.A0(s_ckgen_div[3]), .B0(s_div_hi[3]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[4]), .B1(s_div_hi[4]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6471), .COUT(n6472), .S0(s_div_lo_r0_14__N_475[3]), 
          .S1(s_div_lo_r0_14__N_475[4]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_91_add_2_5.INIT0 = 16'h5999;
    defparam sub_91_add_2_5.INIT1 = 16'h5999;
    defparam sub_91_add_2_5.INJECT1_0 = "NO";
    defparam sub_91_add_2_5.INJECT1_1 = "NO";
    LUT4 i1_2_lut_rep_193 (.A(s_rtcken_reg), .B(\s_irq[1] ), .Z(n8307)) /* synthesis lut_function=((B)+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut_rep_193.init = 16'hdddd;
    LUT4 i4752_1_lut_2_lut (.A(s_rtcken_reg), .B(\s_irq[1] ), .Z(n6337)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i4752_1_lut_2_lut.init = 16'h2222;
    LUT4 i2_3_lut_rep_194 (.A(s_tcki_r0), .B(s_tcki_r1), .C(s_enabled), 
         .Z(mclk_c_enable_115)) /* synthesis lut_function=(!((B+!(C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(255[12:36])
    defparam i2_3_lut_rep_194.init = 16'h2020;
    LUT4 i1_2_lut_4_lut (.A(s_tcki_r0), .B(s_tcki_r1), .C(s_enabled), 
         .D(s_tdien), .Z(n7090)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(255[12:36])
    defparam i1_2_lut_4_lut.init = 16'h0020;
    LUT4 i285_1_lut_rep_196 (.A(s_enabled), .Z(n8310)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i285_1_lut_rep_196.init = 16'h5555;
    LUT4 i1_2_lut_3_lut_3_lut (.A(s_enabled), .B(n8248), .C(s_tck_r0), 
         .Z(mclk_c_enable_95)) /* synthesis lut_function=((B (C))+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_2_lut_3_lut_3_lut.init = 16'hd5d5;
    LUT4 i1_3_lut_3_lut (.A(s_enabled), .B(s_tck_r0), .C(s_rtcken_reg), 
         .Z(n1830)) /* synthesis lut_function=(!(A ((C)+!B))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i1_3_lut_3_lut.init = 16'h5d5d;
    LUT4 s_rtck_r1_I_0_140_1_lut (.A(s_rtck_r1), .Z(s_rtck_r1_N_660)) /* synthesis lut_function=(!(A)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(247[17:30])
    defparam s_rtck_r1_I_0_140_1_lut.init = 16'h5555;
    CCU2D sub_91_add_2_3 (.A0(s_ckgen_div[1]), .B0(s_div_hi[1]), .C0(GND_net), 
          .D0(GND_net), .A1(s_ckgen_div[2]), .B1(s_div_hi[2]), .C1(GND_net), 
          .D1(GND_net), .CIN(n6470), .COUT(n6471), .S0(s_div_lo_r0_14__N_475[1]), 
          .S1(s_div_lo_r0_14__N_475[2]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_91_add_2_3.INIT0 = 16'h5999;
    defparam sub_91_add_2_3.INIT1 = 16'h5999;
    defparam sub_91_add_2_3.INJECT1_0 = "NO";
    defparam sub_91_add_2_3.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_14__I_0_15 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n6400), .S0(s_tck_r0_N_638));
    defparam s_cnt_lo_r_14__I_0_15.INIT0 = 16'hFFFF;
    defparam s_cnt_lo_r_14__I_0_15.INIT1 = 16'h0000;
    defparam s_cnt_lo_r_14__I_0_15.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_14__I_0_15.INJECT1_1 = "NO";
    CCU2D sub_91_add_2_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(s_ckgen_div[0]), .B1(s_div_hi[0]), .C1(GND_net), .D1(GND_net), 
          .COUT(n6470), .S1(s_div_lo_r0_14__N_475[0]));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(112[20:33])
    defparam sub_91_add_2_1.INIT0 = 16'h0000;
    defparam sub_91_add_2_1.INIT1 = 16'h5999;
    defparam sub_91_add_2_1.INJECT1_0 = "NO";
    defparam sub_91_add_2_1.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_14__I_0_0 (.A0(s_cmp_lo[14]), .B0(s_cnt_lo_r[14]), 
          .C0(GND_net), .D0(GND_net), .A1(s_cmp_lo[13]), .B1(s_cnt_lo_r[13]), 
          .C1(s_cmp_lo[12]), .D1(s_cnt_lo_r[12]), .COUT(n6397));   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(188[10:31])
    defparam s_cnt_lo_r_14__I_0_0.INIT0 = 16'h9000;
    defparam s_cnt_lo_r_14__I_0_0.INIT1 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_0.INJECT1_0 = "NO";
    defparam s_cnt_lo_r_14__I_0_0.INJECT1_1 = "YES";
    CCU2D s_cnt_hi_r_756_add_4_15 (.A0(s_cnt_hi_r[13]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[14]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6468), .S0(n65_adj_1589[13]), .S1(n65_adj_1589[14]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756_add_4_15.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_15.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_15.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_756_add_4_15.INJECT1_1 = "NO";
    CCU2D s_cnt_hi_r_756_add_4_13 (.A0(s_cnt_hi_r[11]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[12]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6467), .COUT(n6468), .S0(n65_adj_1589[11]), 
          .S1(n65_adj_1589[12]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756_add_4_13.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_13.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_13.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_756_add_4_13.INJECT1_1 = "NO";
    LUT4 i1_3_lut_rep_134 (.A(s_rtck_r1), .B(rtcktmo_N_629), .C(s_rtck_r2), 
         .Z(n8248)) /* synthesis lut_function=(A (B+!(C))+!A (B)) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(91[9:18])
    defparam i1_3_lut_rep_134.init = 16'hcece;
    LUT4 i2383_2_lut_4_lut (.A(s_rtck_r1), .B(rtcktmo_N_629), .C(s_rtck_r2), 
         .D(mclk_c_enable_135), .Z(n4077)) /* synthesis lut_function=(A (B (D)+!B !(C+!(D)))+!A (B (D))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(91[9:18])
    defparam i2383_2_lut_4_lut.init = 16'hce00;
    FD1P3IX s_cnt_lo_r_755__i1 (.D(n65[1]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[1])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i1.GSR = "ENABLED";
    CCU2D s_cnt_hi_r_756_add_4_11 (.A0(s_cnt_hi_r[9]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[10]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6466), .COUT(n6467), .S0(n65_adj_1589[9]), 
          .S1(n65_adj_1589[10]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756_add_4_11.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_11.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_11.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_756_add_4_11.INJECT1_1 = "NO";
    FD1P3IX s_cnt_lo_r_755__i2 (.D(n65[2]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[2])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i2.GSR = "ENABLED";
    FD1P3IX s_cnt_lo_r_755__i3 (.D(n65[3]), .SP(mclk_c_enable_261), .CD(n4036), 
            .CK(mclk_c), .Q(s_cnt_lo_r[3])) /* synthesis syn_use_carry_chain=1 */ ;   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_lo_r_755__i3.GSR = "ENABLED";
    LUT4 i2_4_lut_4_lut_4_lut_then_4_lut (.A(s_enabled), .B(s_rtck_r1), 
         .C(s_rtck_r2), .D(rtcktmo_N_629), .Z(n8335)) /* synthesis lut_function=((B ((D)+!C)+!B (D))+!A) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(162[3] 244[10])
    defparam i2_4_lut_4_lut_4_lut_then_4_lut.init = 16'hff5d;
    CCU2D s_cnt_hi_r_756_add_4_9 (.A0(s_cnt_hi_r[7]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(s_cnt_hi_r[8]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n6465), .COUT(n6466), .S0(n65_adj_1589[7]), 
          .S1(n65_adj_1589[8]));   // C:/lscc/diamond/3.10_x64/ispfpga/vhdl_packages/numeric_std.vhd(1241[12:13])
    defparam s_cnt_hi_r_756_add_4_9.INIT0 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_9.INIT1 = 16'hfaaa;
    defparam s_cnt_hi_r_756_add_4_9.INJECT1_0 = "NO";
    defparam s_cnt_hi_r_756_add_4_9.INJECT1_1 = "NO";
    CCU2D s_cnt_lo_r_14__I_0_15_4814 (.A0(s_cmp_lo[3]), .B0(s_cnt_lo_r[3]), 
          .C0(s_cmp_lo[2]), .D0(s_cnt_lo_r[2]), .A1(s_cmp_lo[1]), .B1(s_cnt_lo_r[1]), 
          .C1(s_cmp_lo[0]), .D1(s_cnt_lo_r[0]), .CIN(n6399), .COUT(n6400));
    defparam s_cnt_lo_r_14__I_0_15_4814.INIT0 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_15_4814.INIT1 = 16'h9009;
    defparam s_cnt_lo_r_14__I_0_15_4814.INJECT1_0 = "YES";
    defparam s_cnt_lo_r_14__I_0_15_4814.INJECT1_1 = "YES";
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
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[1])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i2_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i3_4_lut (.A(s_rtdiv_lo[2]), .B(s_div_lo[2]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[2])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i3_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i4_4_lut (.A(s_rtdiv_lo[3]), .B(s_div_lo[3]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[3])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i4_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i5_4_lut (.A(s_rtdiv_lo[4]), .B(s_div_lo[4]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[4])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i5_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i6_4_lut (.A(s_rtdiv_lo[5]), .B(s_div_lo[5]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[5])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i6_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i7_4_lut (.A(s_rtdiv_lo[6]), .B(s_div_lo[6]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[6])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i7_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i8_4_lut (.A(s_rtdiv_lo[7]), .B(s_div_lo[7]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[7])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i8_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i9_4_lut (.A(s_rtdiv_lo[8]), .B(s_div_lo[8]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[8])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i9_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i10_4_lut (.A(s_rtdiv_lo[9]), .B(s_div_lo[9]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[9])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i10_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i11_4_lut (.A(s_rtdiv_lo[10]), .B(s_div_lo[10]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[10])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i11_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i12_4_lut (.A(s_rtdiv_lo[11]), .B(s_div_lo[11]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[11])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i12_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i13_4_lut (.A(s_rtdiv_lo[12]), .B(s_div_lo[12]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[12])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i13_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i14_4_lut (.A(s_rtdiv_lo[13]), .B(s_div_lo[13]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[13])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i14_4_lut.init = 16'hcac0;
    LUT4 s_div_lo_14__I_0_i15_4_lut (.A(s_rtdiv_lo[14]), .B(s_div_lo[14]), 
         .C(n1830), .D(s_rtcken_reg), .Z(s_cmp_lo_14__N_550[14])) /* synthesis lut_function=(A (B (C+(D))+!B !(C+!(D)))+!A (B (C))) */ ;   // c:/devel/yard-ice/src/fpga/jtag_enh_ckgen.vhdl(175[4] 243[11])
    defparam s_div_lo_14__I_0_i15_4_lut.init = 16'hcac0;
    
endmodule
//
// Verilog Description of module \dpram(16,10)_U1 
//

module \dpram(16,10)_U1  (mclk_c, fsmc_clk_c, VCC_net, GND_net, s_rx_mem_addr, 
            q_15__N_82, q_15__N_80, q_15__N_78, q_15__N_76, q_15__N_74, 
            q_15__N_72, q_15__N_70, q_15__N_68, q_15__N_66, s_rx_mem_din, 
            n2562, n2563, n2564, n2565, n2566, n2567, n2568, n2569, 
            n2570, n2571, n2572, n2573, n2574, n2575, n2576, n2577, 
            n2546, n2547, n2548, n2549, n2550, n2551, n2552, n2553, 
            n2554, n2555, n2556, n2557, n2558, n2559, n2560, n2561, 
            n2578, q_15__N_64, s_rx_mem_wr_stb);
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
    output n2562;
    output n2563;
    output n2564;
    output n2565;
    output n2566;
    output n2567;
    output n2568;
    output n2569;
    output n2570;
    output n2571;
    output n2572;
    output n2573;
    output n2574;
    output n2575;
    output n2576;
    output n2577;
    output n2546;
    output n2547;
    output n2548;
    output n2549;
    output n2550;
    output n2551;
    output n2552;
    output n2553;
    output n2554;
    output n2555;
    output n2556;
    output n2557;
    output n2558;
    output n2559;
    output n2560;
    output n2561;
    output n2578;
    input q_15__N_64;
    input s_rx_mem_wr_stb;
    
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(40[2:10])
    
    wire mclk_c_enable_28, mclk_c_enable_34;
    
    PDPW8KC mem1 (.DI0(s_rx_mem_din[0]), .DI1(s_rx_mem_din[1]), .DI2(s_rx_mem_din[2]), 
            .DI3(s_rx_mem_din[3]), .DI4(s_rx_mem_din[4]), .DI5(s_rx_mem_din[5]), 
            .DI6(s_rx_mem_din[6]), .DI7(s_rx_mem_din[7]), .DI8(s_rx_mem_din[8]), 
            .DI9(s_rx_mem_din[9]), .DI10(s_rx_mem_din[10]), .DI11(s_rx_mem_din[11]), 
            .DI12(s_rx_mem_din[12]), .DI13(s_rx_mem_din[13]), .DI14(s_rx_mem_din[14]), 
            .DI15(s_rx_mem_din[15]), .DI16(GND_net), .DI17(GND_net), .ADW0(s_rx_mem_addr[0]), 
            .ADW1(s_rx_mem_addr[1]), .ADW2(s_rx_mem_addr[2]), .ADW3(s_rx_mem_addr[3]), 
            .ADW4(s_rx_mem_addr[4]), .ADW5(s_rx_mem_addr[5]), .ADW6(s_rx_mem_addr[6]), 
            .ADW7(s_rx_mem_addr[7]), .ADW8(s_rx_mem_addr[8]), .BE0(VCC_net), 
            .BE1(VCC_net), .CEW(mclk_c_enable_28), .CLKW(mclk_c), .CSW0(GND_net), 
            .CSW1(GND_net), .CSW2(GND_net), .ADR0(GND_net), .ADR1(GND_net), 
            .ADR2(GND_net), .ADR3(GND_net), .ADR4(q_15__N_82), .ADR5(q_15__N_80), 
            .ADR6(q_15__N_78), .ADR7(q_15__N_76), .ADR8(q_15__N_74), .ADR9(q_15__N_72), 
            .ADR10(q_15__N_70), .ADR11(q_15__N_68), .ADR12(q_15__N_66), 
            .CER(VCC_net), .OCER(VCC_net), .CLKR(fsmc_clk_c), .CSR0(GND_net), 
            .CSR1(GND_net), .CSR2(GND_net), .RST(GND_net), .DO0(n2571), 
            .DO1(n2572), .DO2(n2573), .DO3(n2574), .DO4(n2575), .DO5(n2576), 
            .DO6(n2577), .DO9(n2562), .DO10(n2563), .DO11(n2564), .DO12(n2565), 
            .DO13(n2566), .DO14(n2567), .DO15(n2568), .DO16(n2569), 
            .DO17(n2570));
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
            .BE1(VCC_net), .CEW(mclk_c_enable_34), .CLKW(mclk_c), .CSW0(GND_net), 
            .CSW1(GND_net), .CSW2(GND_net), .ADR0(GND_net), .ADR1(GND_net), 
            .ADR2(GND_net), .ADR3(GND_net), .ADR4(q_15__N_82), .ADR5(q_15__N_80), 
            .ADR6(q_15__N_78), .ADR7(q_15__N_76), .ADR8(q_15__N_74), .ADR9(q_15__N_72), 
            .ADR10(q_15__N_70), .ADR11(q_15__N_68), .ADR12(q_15__N_66), 
            .CER(VCC_net), .OCER(VCC_net), .CLKR(fsmc_clk_c), .CSR0(GND_net), 
            .CSR1(GND_net), .CSR2(GND_net), .RST(GND_net), .DO0(n2555), 
            .DO1(n2556), .DO2(n2557), .DO3(n2558), .DO4(n2559), .DO5(n2560), 
            .DO6(n2561), .DO9(n2546), .DO10(n2547), .DO11(n2548), .DO12(n2549), 
            .DO13(n2550), .DO14(n2551), .DO15(n2552), .DO16(n2553), 
            .DO17(n2554));
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
    FD1S3AX _931 (.D(q_15__N_64), .CK(fsmc_clk_c), .Q(n2578));
    defparam _931.GSR = "DISABLED";
    LUT4 i927_2_lut (.A(s_rx_mem_wr_stb), .B(s_rx_mem_addr[9]), .Z(mclk_c_enable_28)) /* synthesis lut_function=(A (B)) */ ;   // c:/devel/yard-ice/src/fpga/dpram.vhdl(65[9:16])
    defparam i927_2_lut.init = 16'h8888;
    LUT4 i926_2_lut (.A(s_rx_mem_wr_stb), .B(s_rx_mem_addr[9]), .Z(mclk_c_enable_34)) /* synthesis lut_function=(!((B)+!A)) */ ;   // c:/devel/yard-ice/src/fpga/dpram.vhdl(65[9:16])
    defparam i926_2_lut.init = 16'h2222;
    
endmodule
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
            n2583, n2584, n2585, n2586, n2587, n2588, n2589, n2590, 
            n2591, n2592, n2593, n2594, n2595, n2596, n2597, n2598, 
            fsmc_clk_c_enable_2, n8853, n8857, n2599, n2600, n2601, 
            n2602, n2603, n2604, n2605, n2606, n2607, n2608, n2609, 
            n2610, n2611, n2612, n2613, n2614, n2615);
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
    output n2583;
    output n2584;
    output n2585;
    output n2586;
    output n2587;
    output n2588;
    output n2589;
    output n2590;
    output n2591;
    output n2592;
    output n2593;
    output n2594;
    output n2595;
    output n2596;
    output n2597;
    output n2598;
    input fsmc_clk_c_enable_2;
    input n8853;
    input n8857;
    output n2599;
    output n2600;
    output n2601;
    output n2602;
    output n2603;
    output n2604;
    output n2605;
    output n2606;
    output n2607;
    output n2608;
    output n2609;
    output n2610;
    output n2611;
    output n2612;
    output n2613;
    output n2614;
    output n2615;
    
    wire fsmc_clk_c /* synthesis SET_AS_NETWORK=fsmc_clk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(40[2:10])
    wire mclk_c /* synthesis SET_AS_NETWORK=mclk_c, is_clock=1 */ ;   // c:/devel/yard-ice/src/fpga/yard_ice.vhdl(36[2:6])
    
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
            .RST(GND_net), .DO0(n2592), .DO1(n2593), .DO2(n2594), .DO3(n2595), 
            .DO4(n2596), .DO5(n2597), .DO6(n2598), .DO9(n2583), .DO10(n2584), 
            .DO11(n2585), .DO12(n2586), .DO13(n2587), .DO14(n2588), 
            .DO15(n2589), .DO16(n2590), .DO17(n2591));
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
            .DI15(fsmc_d_out_15), .DI16(GND_net), .DI17(GND_net), .ADW0(n8853), 
            .ADW1(n8857), .ADW2(q_15__N_78), .ADW3(q_15__N_76), .ADW4(q_15__N_74), 
            .ADW5(q_15__N_72), .ADW6(q_15__N_70), .ADW7(q_15__N_68), .ADW8(q_15__N_66), 
            .BE0(VCC_net), .BE1(VCC_net), .CEW(fsmc_clk_c_enable_2), .CLKW(fsmc_clk_c), 
            .CSW0(GND_net), .CSW1(GND_net), .CSW2(GND_net), .ADR0(GND_net), 
            .ADR1(GND_net), .ADR2(GND_net), .ADR3(GND_net), .ADR4(s_addr_0[0]), 
            .ADR5(s_addr_0[1]), .ADR6(s_addr_0[2]), .ADR7(s_addr_0[3]), 
            .ADR8(s_addr_0[4]), .ADR9(s_addr_0[5]), .ADR10(s_addr_0[6]), 
            .ADR11(s_addr_0[7]), .ADR12(s_addr_0[8]), .CER(VCC_net), .OCER(VCC_net), 
            .CLKR(mclk_c), .CSR0(GND_net), .CSR1(GND_net), .CSR2(GND_net), 
            .RST(GND_net), .DO0(n2608), .DO1(n2609), .DO2(n2610), .DO3(n2611), 
            .DO4(n2612), .DO5(n2613), .DO6(n2614), .DO9(n2599), .DO10(n2600), 
            .DO11(n2601), .DO12(n2602), .DO13(n2603), .DO14(n2604), 
            .DO15(n2605), .DO16(n2606), .DO17(n2607));
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
    FD1S3AX _1103 (.D(s_addr_0[9]), .CK(mclk_c), .Q(n2615));
    defparam _1103.GSR = "DISABLED";
    
endmodule
