/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file at91sam7.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <at91sam/at91sam_ckgr.h>
#include <at91sam/at91sam_pmc.h>
#include <at91sam/at91sam_rstc.h>
#include <at91sam/at91sam_wdtc.h>
#include <at91sam/at91sam_mc.h>
#include <at91sam/at91sam_dbgu.h>

#include <sys/os.h>

#include "armice.h"
#include "target/at91sam7.h"
#include "script.h"
#include "dbglog.h"

#define AT91_BASE_DBGU      0xfffff200 /* (DBGU) Base Address */
#define AT91_BASE_PIOA      0xfffff400 /* (PIOA) Base Address */
#define AT91_BASE_PIOB      0xfffff600 /* (PIOB) Base Address */
#define AT91_BASE_CKGR      0xfffffc20 /* (CKGR) Base Address */
#define AT91_BASE_PMC       0xfffffc00 /* (PMC) Base Address */
#define AT91_BASE_RSTC      0xfffffd00 /* (RSTC) Base Address */
#define AT91_BASE_RTTC      0xfffffd20 /* (RTTC) Base Address */
#define AT91_BASE_PITC      0xfffffd30 /* (PITC) Base Address */
#define AT91_BASE_WDTC      0xfffffd40 /* (WDTC) Base Address */
#define AT91_BASE_VREG      0xfffffd60 /* (VREG) Base Address */
#define AT91_BASE_MC        0xffffff00 /* (MC) Base Address */

/*
 * XXX: the at91sam7s256 reports a wrong chip id 
 * of 0x270D0940 instead of 0x270B0940. */

#define AT91SAM7S16_A_ID 0x27050240
#define AT9SAM7S161_A_ID 0x27050241

#define AT91SAM7S321_A_ID 0x27080342
#define AT91SAM7S32_A_ID 0x27080340
#define AT91SAM7S32_B_ID 0x27080341

#define AT91SAM7S64_A_ID 0x27090540
#define AT91SAM7S64_B_ID 0x27090543

#define AT91SAM7S128_A_ID 0x270A0740
#define AT91SAM7S128_B_ID 0x270A0741

#define AT91SAM7S256_A_ID 0x270D0940
#define AT91SAM7S256_A_FIX_ID 0x270b0940
#define AT91SAM7S256_B_ID 0x270B0941

#define AT91SAM7S512_A_ID 0x270B0A40

#define AT91SAM7X256_ID 0x275b0940

#define AT91SAM7XC128_ID 0x271a0740
#define AT91SAM7XC256_ID 0x271b0940
#define AT91SAM7XC512_ID 0x271c0a40

const struct target_sym at91sam7_sym[] = {
#if 0
	{0xfffa0000, "base_tc0", 4, _RW_},
	{0xfffa0000, "base_tcb", 4, _RW_},
	{0xfffa0000, "tc0_ccr", 4, _RW_},
	{0xfffa0004, "tc0_cmr", 4, _RW_},
	{0xfffa0010, "tc0_cv", 4, _RW_},
	{0xfffa0014, "tc0_ra", 4, _RW_},
	{0xfffa0018, "tc0_rb", 4, _RW_},
	{0xfffa001c, "tc0_rc", 4, _RW_},
	{0xfffa0020, "tc0_sr", 4, _RW_},
	{0xfffa0024, "tc0_ier", 4, _RW_},
	{0xfffa0028, "tc0_idr", 4, _RW_},
	{0xfffa002c, "tc0_imr", 4, _RW_},
	{0xfffa0040, "base_tc1", 4, _RW_},
	{0xfffa0040, "tc1_ccr", 4, _RW_},
	{0xfffa0044, "tc1_cmr", 4, _RW_},
	{0xfffa0050, "tc1_cv", 4, _RW_},
	{0xfffa0054, "tc1_ra", 4, _RW_},
	{0xfffa0058, "tc1_rb", 4, _RW_},
	{0xfffa005c, "tc1_rc", 4, _RW_},
	{0xfffa0060, "tc1_sr", 4, _RW_},
	{0xfffa0064, "tc1_ier", 4, _RW_},
	{0xfffa0068, "tc1_idr", 4, _RW_},
	{0xfffa006c, "tc1_imr", 4, _RW_},
	{0xfffa0080, "base_tc2", 4, _RW_},
	{0xfffa0080, "tc2_ccr", 4, _RW_},
	{0xfffa0084, "tc2_cmr", 4, _RW_},
	{0xfffa0090, "tc2_cv", 4, _RW_},
	{0xfffa0094, "tc2_ra", 4, _RW_},
	{0xfffa0098, "tc2_rb", 4, _RW_},
	{0xfffa009c, "tc2_rc", 4, _RW_},
	{0xfffa00a0, "tc2_sr", 4, _RW_},
	{0xfffa00a4, "tc2_ier", 4, _RW_},
	{0xfffa00a8, "tc2_idr", 4, _RW_},
	{0xfffa00ac, "tc2_imr", 4, _RW_},
	{0xfffa00c0, "tcb_bcr", 4, _RW_},
	{0xfffa00c4, "tcb_bmr", 4, _RW_},
	{0xfffa4000, "aes_cr", 4, _RW_},
	{0xfffa4000, "base_aes", 4, _RW_},
	{0xfffa4004, "aes_mr", 4, _RW_},
	{0xfffa4010, "aes_ier", 4, _RW_},
	{0xfffa4014, "aes_idr", 4, _RW_},
	{0xfffa4018, "aes_imr", 4, _RW_},
	{0xfffa401c, "aes_isr", 4, _RW_},
	{0xfffa40fc, "aes_vr", 4, _RW_},
	{0xfffa4100, "aes_rpr", 4, _RW_},
	{0xfffa4100, "base_pdc_aes", 4, _RW_},
	{0xfffa4104, "aes_rcr", 4, _RW_},
	{0xfffa4108, "aes_tpr", 4, _RW_},
	{0xfffa410c, "aes_tcr", 4, _RW_},
	{0xfffa4110, "aes_rnpr", 4, _RW_},
	{0xfffa4114, "aes_rncr", 4, _RW_},
	{0xfffa4118, "aes_tnpr", 4, _RW_},
	{0xfffa411c, "aes_tncr", 4, _RW_},
	{0xfffa4120, "aes_ptcr", 4, _RW_},
	{0xfffa4124, "aes_ptsr", 4, _RW_},
	{0xfffa8000, "base_tdes", 4, _RW_},
	{0xfffa8000, "tdes_cr", 4, _RW_},
	{0xfffa8004, "tdes_mr", 4, _RW_},
	{0xfffa8010, "tdes_ier", 4, _RW_},
	{0xfffa8014, "tdes_idr", 4, _RW_},
	{0xfffa8018, "tdes_imr", 4, _RW_},
	{0xfffa801c, "tdes_isr", 4, _RW_},
	{0xfffa80fc, "tdes_vr", 4, _RW_},
	{0xfffa8100, "base_pdc_tdes", 4, _RW_},
	{0xfffa8100, "tdes_rpr", 4, _RW_},
	{0xfffa8104, "tdes_rcr", 4, _RW_},
	{0xfffa8108, "tdes_tpr", 4, _RW_},
	{0xfffa810c, "tdes_tcr", 4, _RW_},
	{0xfffa8110, "tdes_rnpr", 4, _RW_},
	{0xfffa8114, "tdes_rncr", 4, _RW_},
	{0xfffa8118, "tdes_tnpr", 4, _RW_},
	{0xfffa811c, "tdes_tncr", 4, _RW_},
	{0xfffa8120, "tdes_ptcr", 4, _RW_},
	{0xfffa8124, "tdes_ptsr", 4, _RW_},
	{0xfffb0000, "base_udp", 4, _RW_},
	{0xfffb0000, "udp_num", 4, _RW_},
	{0xfffb0004, "udp_glbstate", 4, _RW_},
	{0xfffb0008, "udp_faddr", 4, _RW_},
	{0xfffb0010, "udp_ier", 4, _RW_},
	{0xfffb0014, "udp_idr", 4, _RW_},
	{0xfffb0018, "udp_imr", 4, _RW_},
	{0xfffb001c, "udp_isr", 4, _RW_},
	{0xfffb0020, "udp_icr", 4, _RW_},
	{0xfffb0028, "udp_rstep", 4, _RW_},
	{0xfffb0030, "udp_csr", 4, _RW_},
	{0xfffb0050, "udp_fdr", 4, _RW_},
	{0xfffb0074, "udp_txvc", 4, _RW_},
	{0xfffb8000, "base_twi", 4, _RW_},
	{0xfffb8000, "twi_cr", 4, _RW_},
	{0xfffb8004, "twi_mmr", 4, _RW_},
	{0xfffb800c, "twi_iadr", 4, _RW_},
	{0xfffb8010, "twi_cwgr", 4, _RW_},
	{0xfffb8020, "twi_sr", 4, _RW_},
	{0xfffb8024, "twi_ier", 4, _RW_},
	{0xfffb8028, "twi_idr", 4, _RW_},
	{0xfffb802c, "twi_imr", 4, _RW_},
	{0xfffb8030, "twi_rhr", 4, _RW_},
	{0xfffb8034, "twi_thr", 4, _RW_},
	{0xfffc0000, "base_us0", 4, _RW_},
	{0xfffc0000, "us0_cr", 4, _RW_},
	{0xfffc0004, "us0_mr", 4, _RW_},
	{0xfffc0008, "us0_ier", 4, _RW_},
	{0xfffc000c, "us0_idr", 4, _RW_},
	{0xfffc0010, "us0_imr", 4, _RW_},
	{0xfffc0014, "us0_csr", 4, _RW_},
	{0xfffc0018, "us0_rhr", 4, _RW_},
	{0xfffc001c, "us0_thr", 4, _RW_},
	{0xfffc0020, "us0_brgr", 4, _RW_},
	{0xfffc0024, "us0_rtor", 4, _RW_},
	{0xfffc0028, "us0_ttgr", 4, _RW_},
	{0xfffc0040, "us0_fidi", 4, _RW_},
	{0xfffc0044, "us0_ner", 4, _RW_},
	{0xfffc004c, "us0_if", 4, _RW_},
	{0xfffc0100, "base_pdc_us0", 4, _RW_},
	{0xfffc0100, "us0_rpr", 4, _RW_},
	{0xfffc0104, "us0_rcr", 4, _RW_},
	{0xfffc0108, "us0_tpr", 4, _RW_},
	{0xfffc010c, "us0_tcr", 4, _RW_},
	{0xfffc0110, "us0_rnpr", 4, _RW_},
	{0xfffc0114, "us0_rncr", 4, _RW_},
	{0xfffc0118, "us0_tnpr", 4, _RW_},
	{0xfffc011c, "us0_tncr", 4, _RW_},
	{0xfffc0120, "us0_ptcr", 4, _RW_},
	{0xfffc0124, "us0_ptsr", 4, _RW_},
	{0xfffc4000, "base_us1", 4, _RW_},
	{0xfffc4000, "us1_cr", 4, _RW_},
	{0xfffc4004, "us1_mr", 4, _RW_},
	{0xfffc4008, "us1_ier", 4, _RW_},
	{0xfffc400c, "us1_idr", 4, _RW_},
	{0xfffc4010, "us1_imr", 4, _RW_},
	{0xfffc4014, "us1_csr", 4, _RW_},
	{0xfffc4018, "us1_rhr", 4, _RW_},
	{0xfffc401c, "us1_thr", 4, _RW_},
	{0xfffc4020, "us1_brgr", 4, _RW_},
	{0xfffc4024, "us1_rtor", 4, _RW_},
	{0xfffc4028, "us1_ttgr", 4, _RW_},
	{0xfffc4040, "us1_fidi", 4, _RW_},
	{0xfffc4044, "us1_ner", 4, _RW_},
	{0xfffc404c, "us1_if", 4, _RW_},
	{0xfffc4100, "base_pdc_us1", 4, _RW_},
	{0xfffc4100, "us1_rpr", 4, _RW_},
	{0xfffc4104, "us1_rcr", 4, _RW_},
	{0xfffc4108, "us1_tpr", 4, _RW_},
	{0xfffc410c, "us1_tcr", 4, _RW_},
	{0xfffc4110, "us1_rnpr", 4, _RW_},
	{0xfffc4114, "us1_rncr", 4, _RW_},
	{0xfffc4118, "us1_tnpr", 4, _RW_},
	{0xfffc411c, "us1_tncr", 4, _RW_},
	{0xfffc4120, "us1_ptcr", 4, _RW_},
	{0xfffc4124, "us1_ptsr", 4, _RW_},
	{0xfffcc000, "base_pwmc", 4, _RW_},
	{0xfffcc000, "pwmc_mr", 4, _RW_},
	{0xfffcc004, "pwmc_ena", 4, _RW_},
	{0xfffcc008, "pwmc_dis", 4, _RW_},
	{0xfffcc00c, "pwmc_sr", 4, _RW_},
	{0xfffcc010, "pwmc_ier", 4, _RW_},
	{0xfffcc014, "pwmc_idr", 4, _RW_},
	{0xfffcc018, "pwmc_imr", 4, _RW_},
	{0xfffcc01c, "pwmc_isr", 4, _RW_},
	{0xfffcc0fc, "pwmc_vr", 4, _RW_},
	{0xfffcc200, "base_pwmc_ch0", 4, _RW_},
	{0xfffcc200, "pwmc_ch0_cmr", 4, _RW_},
	{0xfffcc204, "pwmc_ch0_cdtyr", 4, _RW_},
	{0xfffcc208, "pwmc_ch0_cprdr", 4, _RW_},
	{0xfffcc20c, "pwmc_ch0_ccntr", 4, _RW_},
	{0xfffcc210, "pwmc_ch0_cupdr", 4, _RW_},
	{0xfffcc220, "base_pwmc_ch1", 4, _RW_},
	{0xfffcc220, "pwmc_ch1_cmr", 4, _RW_},
	{0xfffcc224, "pwmc_ch1_cdtyr", 4, _RW_},
	{0xfffcc228, "pwmc_ch1_cprdr", 4, _RW_},
	{0xfffcc22c, "pwmc_ch1_ccntr", 4, _RW_},
	{0xfffcc230, "pwmc_ch1_cupdr", 4, _RW_},
	{0xfffcc240, "base_pwmc_ch2", 4, _RW_},
	{0xfffcc240, "pwmc_ch2_cmr", 4, _RW_},
	{0xfffcc244, "pwmc_ch2_cdtyr", 4, _RW_},
	{0xfffcc248, "pwmc_ch2_cprdr", 4, _RW_},
	{0xfffcc24c, "pwmc_ch2_ccntr", 4, _RW_},
	{0xfffcc250, "pwmc_ch2_cupdr", 4, _RW_},
	{0xfffcc260, "base_pwmc_ch3", 4, _RW_},
	{0xfffcc260, "pwmc_ch3_cmr", 4, _RW_},
	{0xfffcc264, "pwmc_ch3_cdtyr", 4, _RW_},
	{0xfffcc268, "pwmc_ch3_cprdr", 4, _RW_},
	{0xfffcc26c, "pwmc_ch3_ccntr", 4, _RW_},
	{0xfffcc270, "pwmc_ch3_cupdr", 4, _RW_},
	{0xfffd0000, "base_can", 4, _RW_},
	{0xfffd0000, "can_mr", 4, _RW_},
	{0xfffd0004, "can_ier", 4, _RW_},
	{0xfffd0008, "can_idr", 4, _RW_},
	{0xfffd000c, "can_imr", 4, _RW_},
	{0xfffd0010, "can_sr", 4, _RW_},
	{0xfffd0014, "can_br", 4, _RW_},
	{0xfffd0018, "can_tim", 4, _RW_},
	{0xfffd001c, "can_timestp", 4, _RW_},
	{0xfffd0020, "can_ecr", 4, _RW_},
	{0xfffd0024, "can_tcr", 4, _RW_},
	{0xfffd0028, "can_acr", 4, _RW_},
	{0xfffd00fc, "can_vr", 4, _RW_},
	{0xfffd0200, "base_can_mb0", 4, _RW_},
	{0xfffd0200, "can_mb0_mmr", 4, _RW_},
	{0xfffd0204, "can_mb0_mam", 4, _RW_},
	{0xfffd0208, "can_mb0_mid", 4, _RW_},
	{0xfffd020c, "can_mb0_mfid", 4, _RW_},
	{0xfffd0210, "can_mb0_msr", 4, _RW_},
	{0xfffd0214, "can_mb0_mdl", 4, _RW_},
	{0xfffd0218, "can_mb0_mdh", 4, _RW_},
	{0xfffd021c, "can_mb0_mcr", 4, _RW_},
	{0xfffd0220, "base_can_mb1", 4, _RW_},
	{0xfffd0220, "can_mb1_mmr", 4, _RW_},
	{0xfffd0224, "can_mb1_mam", 4, _RW_},
	{0xfffd0228, "can_mb1_mid", 4, _RW_},
	{0xfffd022c, "can_mb1_mfid", 4, _RW_},
	{0xfffd0230, "can_mb1_msr", 4, _RW_},
	{0xfffd0234, "can_mb1_mdl", 4, _RW_},
	{0xfffd0238, "can_mb1_mdh", 4, _RW_},
	{0xfffd023c, "can_mb1_mcr", 4, _RW_},
	{0xfffd0240, "base_can_mb2", 4, _RW_},
	{0xfffd0240, "can_mb2_mmr", 4, _RW_},
	{0xfffd0244, "can_mb2_mam", 4, _RW_},
	{0xfffd0248, "can_mb2_mid", 4, _RW_},
	{0xfffd024c, "can_mb2_mfid", 4, _RW_},
	{0xfffd0250, "can_mb2_msr", 4, _RW_},
	{0xfffd0254, "can_mb2_mdl", 4, _RW_},
	{0xfffd0258, "can_mb2_mdh", 4, _RW_},
	{0xfffd025c, "can_mb2_mcr", 4, _RW_},
	{0xfffd0260, "base_can_mb3", 4, _RW_},
	{0xfffd0260, "can_mb3_mmr", 4, _RW_},
	{0xfffd0264, "can_mb3_mam", 4, _RW_},
	{0xfffd0268, "can_mb3_mid", 4, _RW_},
	{0xfffd026c, "can_mb3_mfid", 4, _RW_},
	{0xfffd0270, "can_mb3_msr", 4, _RW_},
	{0xfffd0274, "can_mb3_mdl", 4, _RW_},
	{0xfffd0278, "can_mb3_mdh", 4, _RW_},
	{0xfffd027c, "can_mb3_mcr", 4, _RW_},
	{0xfffd0280, "base_can_mb4", 4, _RW_},
	{0xfffd0280, "can_mb4_mmr", 4, _RW_},
	{0xfffd0284, "can_mb4_mam", 4, _RW_},
	{0xfffd0288, "can_mb4_mid", 4, _RW_},
	{0xfffd028c, "can_mb4_mfid", 4, _RW_},
	{0xfffd0290, "can_mb4_msr", 4, _RW_},
	{0xfffd0294, "can_mb4_mdl", 4, _RW_},
	{0xfffd0298, "can_mb4_mdh", 4, _RW_},
	{0xfffd029c, "can_mb4_mcr", 4, _RW_},
	{0xfffd02a0, "base_can_mb5", 4, _RW_},
	{0xfffd02a0, "can_mb5_mmr", 4, _RW_},
	{0xfffd02a4, "can_mb5_mam", 4, _RW_},
	{0xfffd02a8, "can_mb5_mid", 4, _RW_},
	{0xfffd02ac, "can_mb5_mfid", 4, _RW_},
	{0xfffd02b0, "can_mb5_msr", 4, _RW_},
	{0xfffd02b4, "can_mb5_mdl", 4, _RW_},
	{0xfffd02b8, "can_mb5_mdh", 4, _RW_},
	{0xfffd02bc, "can_mb5_mcr", 4, _RW_},
	{0xfffd02c0, "base_can_mb6", 4, _RW_},
	{0xfffd02c0, "can_mb6_mmr", 4, _RW_},
	{0xfffd02c4, "can_mb6_mam", 4, _RW_},
	{0xfffd02c8, "can_mb6_mid", 4, _RW_},
	{0xfffd02cc, "can_mb6_mfid", 4, _RW_},
	{0xfffd02d0, "can_mb6_msr", 4, _RW_},
	{0xfffd02d4, "can_mb6_mdl", 4, _RW_},
	{0xfffd02d8, "can_mb6_mdh", 4, _RW_},
	{0xfffd02dc, "can_mb6_mcr", 4, _RW_},
	{0xfffd02e0, "base_can_mb7", 4, _RW_},
	{0xfffd02e0, "can_mb7_mmr", 4, _RW_},
	{0xfffd02e4, "can_mb7_mam", 4, _RW_},
	{0xfffd02e8, "can_mb7_mid", 4, _RW_},
	{0xfffd02ec, "can_mb7_mfid", 4, _RW_},
	{0xfffd02f0, "can_mb7_msr", 4, _RW_},
	{0xfffd02f4, "can_mb7_mdl", 4, _RW_},
	{0xfffd02f8, "can_mb7_mdh", 4, _RW_},
	{0xfffd02fc, "can_mb7_mcr", 4, _RW_},
	{0xfffd4000, "base_ssc", 4, _RW_},
	{0xfffd4000, "ssc_cr", 4, _RW_},
	{0xfffd4004, "ssc_cmr", 4, _RW_},
	{0xfffd4010, "ssc_rcmr", 4, _RW_},
	{0xfffd4014, "ssc_rfmr", 4, _RW_},
	{0xfffd4018, "ssc_tcmr", 4, _RW_},
	{0xfffd401c, "ssc_tfmr", 4, _RW_},
	{0xfffd4020, "ssc_rhr", 4, _RW_},
	{0xfffd4024, "ssc_thr", 4, _RW_},
	{0xfffd4030, "ssc_rshr", 4, _RW_},
	{0xfffd4034, "ssc_tshr", 4, _RW_},
	{0xfffd4040, "ssc_sr", 4, _RW_},
	{0xfffd4044, "ssc_ier", 4, _RW_},
	{0xfffd4048, "ssc_idr", 4, _RW_},
	{0xfffd404c, "ssc_imr", 4, _RW_},
	{0xfffd4100, "base_pdc_ssc", 4, _RW_},
	{0xfffd4100, "ssc_rpr", 4, _RW_},
	{0xfffd4104, "ssc_rcr", 4, _RW_},
	{0xfffd4108, "ssc_tpr", 4, _RW_},
	{0xfffd410c, "ssc_tcr", 4, _RW_},
	{0xfffd4110, "ssc_rnpr", 4, _RW_},
	{0xfffd4114, "ssc_rncr", 4, _RW_},
	{0xfffd4118, "ssc_tnpr", 4, _RW_},
	{0xfffd411c, "ssc_tncr", 4, _RW_},
	{0xfffd4120, "ssc_ptcr", 4, _RW_},
	{0xfffd4124, "ssc_ptsr", 4, _RW_},
	{0xfffd8000, "adc_cr", 4, _RW_},
	{0xfffd8000, "base_adc", 4, _RW_},
	{0xfffd8004, "adc_mr", 4, _RW_},
	{0xfffd8010, "adc_cher", 4, _RW_},
	{0xfffd8014, "adc_chdr", 4, _RW_},
	{0xfffd8018, "adc_chsr", 4, _RW_},
	{0xfffd801c, "adc_sr", 4, _RW_},
	{0xfffd8020, "adc_lcdr", 4, _RW_},
	{0xfffd8024, "adc_ier", 4, _RW_},
	{0xfffd8028, "adc_idr", 4, _RW_},
	{0xfffd802c, "adc_imr", 4, _RW_},
	{0xfffd8030, "adc_cdr0", 4, _RW_},
	{0xfffd8034, "adc_cdr1", 4, _RW_},
	{0xfffd8038, "adc_cdr2", 4, _RW_},
	{0xfffd803c, "adc_cdr3", 4, _RW_},
	{0xfffd8040, "adc_cdr4", 4, _RW_},
	{0xfffd8044, "adc_cdr5", 4, _RW_},
	{0xfffd8048, "adc_cdr6", 4, _RW_},
	{0xfffd804c, "adc_cdr7", 4, _RW_},
	{0xfffd8100, "adc_rpr", 4, _RW_},
	{0xfffd8100, "base_pdc_adc", 4, _RW_},
	{0xfffd8104, "adc_rcr", 4, _RW_},
	{0xfffd8108, "adc_tpr", 4, _RW_},
	{0xfffd810c, "adc_tcr", 4, _RW_},
	{0xfffd8110, "adc_rnpr", 4, _RW_},
	{0xfffd8114, "adc_rncr", 4, _RW_},
	{0xfffd8118, "adc_tnpr", 4, _RW_},
	{0xfffd811c, "adc_tncr", 4, _RW_},
	{0xfffd8120, "adc_ptcr", 4, _RW_},
	{0xfffd8124, "adc_ptsr", 4, _RW_},
	{0xfffdc000, "base_emac", 4, _RW_},
	{0xfffdc000, "emac_ncr", 4, _RW_},
	{0xfffdc004, "emac_ncfgr", 4, _RW_},
	{0xfffdc008, "emac_nsr", 4, _RW_},
	{0xfffdc014, "emac_tsr", 4, _RW_},
	{0xfffdc018, "emac_rbqp", 4, _RW_},
	{0xfffdc01c, "emac_tbqp", 4, _RW_},
	{0xfffdc020, "emac_rsr", 4, _RW_},
	{0xfffdc024, "emac_isr", 4, _RW_},
	{0xfffdc028, "emac_ier", 4, _RW_},
	{0xfffdc02c, "emac_idr", 4, _RW_},
	{0xfffdc030, "emac_imr", 4, _RW_},
	{0xfffdc034, "emac_man", 4, _RW_},
	{0xfffdc038, "emac_ptr", 4, _RW_},
	{0xfffdc03c, "emac_pfr", 4, _RW_},
	{0xfffdc040, "emac_fto", 4, _RW_},
	{0xfffdc044, "emac_scf", 4, _RW_},
	{0xfffdc048, "emac_mcf", 4, _RW_},
	{0xfffdc04c, "emac_fro", 4, _RW_},
	{0xfffdc050, "emac_fcse", 4, _RW_},
	{0xfffdc054, "emac_ale", 4, _RW_},
	{0xfffdc058, "emac_dtf", 4, _RW_},
	{0xfffdc05c, "emac_lcol", 4, _RW_},
	{0xfffdc060, "emac_ecol", 4, _RW_},
	{0xfffdc064, "emac_tund", 4, _RW_},
	{0xfffdc068, "emac_cse", 4, _RW_},
	{0xfffdc06c, "emac_rre", 4, _RW_},
	{0xfffdc070, "emac_rov", 4, _RW_},
	{0xfffdc074, "emac_rse", 4, _RW_},
	{0xfffdc078, "emac_ele", 4, _RW_},
	{0xfffdc07c, "emac_rja", 4, _RW_},
	{0xfffdc080, "emac_usf", 4, _RW_},
	{0xfffdc084, "emac_ste", 4, _RW_},
	{0xfffdc088, "emac_rle", 4, _RW_},
	{0xfffdc08c, "emac_tpf", 4, _RW_},
	{0xfffdc090, "emac_hrb", 4, _RW_},
	{0xfffdc094, "emac_hrt", 4, _RW_},
	{0xfffdc098, "emac_sa1l", 4, _RW_},
	{0xfffdc09c, "emac_sa1h", 4, _RW_},
	{0xfffdc0a0, "emac_sa2l", 4, _RW_},
	{0xfffdc0a4, "emac_sa2h", 4, _RW_},
	{0xfffdc0a8, "emac_sa3l", 4, _RW_},
	{0xfffdc0ac, "emac_sa3h", 4, _RW_},
	{0xfffdc0b0, "emac_sa4l", 4, _RW_},
	{0xfffdc0b4, "emac_sa4h", 4, _RW_},
	{0xfffdc0b8, "emac_tid", 4, _RW_},
	{0xfffdc0bc, "emac_tpq", 4, _RW_},
	{0xfffdc0c0, "emac_usrio", 4, _RW_},
	{0xfffdc0c4, "emac_wol", 4, _RW_},
	{0xfffdc0fc, "emac_rev", 4, _RW_},
	{0xfffe0000, "base_spi0", 4, _RW_},
	{0xfffe0000, "spi0_cr", 4, _RW_},
	{0xfffe0004, "spi0_mr", 4, _RW_},
	{0xfffe0008, "spi0_rdr", 4, _RW_},
	{0xfffe000c, "spi0_tdr", 4, _RW_},
	{0xfffe0010, "spi0_sr", 4, _RW_},
	{0xfffe0014, "spi0_ier", 4, _RW_},
	{0xfffe0018, "spi0_idr", 4, _RW_},
	{0xfffe001c, "spi0_imr", 4, _RW_},
	{0xfffe0030, "spi0_csr", 4, _RW_},
	{0xfffe0100, "base_pdc_spi0", 4, _RW_},
	{0xfffe0100, "spi0_rpr", 4, _RW_},
	{0xfffe0104, "spi0_rcr", 4, _RW_},
	{0xfffe0108, "spi0_tpr", 4, _RW_},
	{0xfffe010c, "spi0_tcr", 4, _RW_},
	{0xfffe0110, "spi0_rnpr", 4, _RW_},
	{0xfffe0114, "spi0_rncr", 4, _RW_},
	{0xfffe0118, "spi0_tnpr", 4, _RW_},
	{0xfffe011c, "spi0_tncr", 4, _RW_},
	{0xfffe0120, "spi0_ptcr", 4, _RW_},
	{0xfffe0124, "spi0_ptsr", 4, _RW_},
	{0xfffe4000, "base_spi1", 4, _RW_},
	{0xfffe4000, "spi1_cr", 4, _RW_},
	{0xfffe4004, "spi1_mr", 4, _RW_},
	{0xfffe4008, "spi1_rdr", 4, _RW_},
	{0xfffe400c, "spi1_tdr", 4, _RW_},
	{0xfffe4010, "spi1_sr", 4, _RW_},
	{0xfffe4014, "spi1_ier", 4, _RW_},
	{0xfffe4018, "spi1_idr", 4, _RW_},
	{0xfffe401c, "spi1_imr", 4, _RW_},
	{0xfffe4030, "spi1_csr", 4, _RW_},
	{0xfffe4100, "base_pdc_spi1", 4, _RW_},
	{0xfffe4100, "spi1_rpr", 4, _RW_},
	{0xfffe4104, "spi1_rcr", 4, _RW_},
	{0xfffe4108, "spi1_tpr", 4, _RW_},
	{0xfffe410c, "spi1_tcr", 4, _RW_},
	{0xfffe4110, "spi1_rnpr", 4, _RW_},
	{0xfffe4114, "spi1_rncr", 4, _RW_},
	{0xfffe4118, "spi1_tnpr", 4, _RW_},
	{0xfffe411c, "spi1_tncr", 4, _RW_},
	{0xfffe4120, "spi1_ptcr", 4, _RW_},
	{0xfffe4124, "spi1_ptsr", 4, _RW_},
	{0xfffff000, "aic_smr", 4, _RW_},
	{0xfffff000, "base_aic", 4, _RW_},
	{0xfffff000, "base_sys", 4, _RW_},
	{0xfffff080, "aic_svr", 4, _RW_},
	{0xfffff100, "aic_ivr", 4, _RW_},
	{0xfffff104, "aic_fvr", 4, _RW_},
	{0xfffff108, "aic_isr", 4, _RW_},
	{0xfffff10c, "aic_ipr", 4, _RW_},
	{0xfffff110, "aic_imr", 4, _RW_},
	{0xfffff114, "aic_cisr", 4, _RW_},
	{0xfffff120, "aic_iecr", 4, _RW_},
	{0xfffff124, "aic_idcr", 4, _RW_},
	{0xfffff128, "aic_iccr", 4, _RW_},
	{0xfffff12c, "aic_iscr", 4, _RW_},
	{0xfffff130, "aic_eoicr", 4, _RW_},
	{0xfffff134, "aic_spu", 4, _RW_},
	{0xfffff138, "aic_dcr", 4, _RW_},
	{0xfffff140, "aic_ffer", 4, _RW_},
	{0xfffff144, "aic_ffdr", 4, _RW_},
	{0xfffff148, "aic_ffsr", 4, _RW_},
	{0xfffff200, "base_dbgu", 4, _RW_},
	{0xfffff200, "dbgu_cr", 4, _RW_},
	{0xfffff204, "dbgu_mr", 4, _RW_},
	{0xfffff208, "dbgu_ier", 4, _RW_},
	{0xfffff20c, "dbgu_idr", 4, _RW_},
	{0xfffff210, "dbgu_imr", 4, _RW_},
	{0xfffff214, "dbgu_csr", 4, _RW_},
	{0xfffff218, "dbgu_rhr", 4, _RW_},
	{0xfffff21c, "dbgu_thr", 4, _RW_},
	{0xfffff220, "dbgu_brgr", 4, _RW_},
	{0xfffff240, "dbgu_cidr", 4, _RW_},
	{0xfffff244, "dbgu_exid", 4, _RW_},
	{0xfffff248, "dbgu_fntr", 4, _RW_},
	{0xfffff300, "base_pdc_dbgu", 4, _RW_},
	{0xfffff300, "dbgu_rpr", 4, _RW_},
	{0xfffff304, "dbgu_rcr", 4, _RW_},
	{0xfffff308, "dbgu_tpr", 4, _RW_},
	{0xfffff30c, "dbgu_tcr", 4, _RW_},
	{0xfffff310, "dbgu_rnpr", 4, _RW_},
	{0xfffff314, "dbgu_rncr", 4, _RW_},
	{0xfffff318, "dbgu_tnpr", 4, _RW_},
	{0xfffff31c, "dbgu_tncr", 4, _RW_},
	{0xfffff320, "dbgu_ptcr", 4, _RW_},
	{0xfffff324, "dbgu_ptsr", 4, _RW_},
	{0xfffff400, "base_pioa", 4, _RW_},
	{0xfffff400, "pioa_per", 4, _RW_},
	{0xfffff404, "pioa_pdr", 4, _RW_},
	{0xfffff408, "pioa_psr", 4, _RW_},
	{0xfffff410, "pioa_oer", 4, _RW_},
	{0xfffff414, "pioa_odr", 4, _RW_},
	{0xfffff418, "pioa_osr", 4, _RW_},
	{0xfffff420, "pioa_ifer", 4, _RW_},
	{0xfffff424, "pioa_ifdr", 4, _RW_},
	{0xfffff428, "pioa_ifsr", 4, _RW_},
	{0xfffff430, "pioa_sodr", 4, _RW_},
	{0xfffff434, "pioa_codr", 4, _RW_},
	{0xfffff438, "pioa_odsr", 4, _RW_},
	{0xfffff43c, "pioa_pdsr", 4, _RW_},
	{0xfffff440, "pioa_ier", 4, _RW_},
	{0xfffff444, "pioa_idr", 4, _RW_},
	{0xfffff448, "pioa_imr", 4, _RW_},
	{0xfffff44c, "pioa_isr", 4, _RW_},
	{0xfffff450, "pioa_mder", 4, _RW_},
	{0xfffff454, "pioa_mddr", 4, _RW_},
	{0xfffff458, "pioa_mdsr", 4, _RW_},
	{0xfffff460, "pioa_ppudr", 4, _RW_},
	{0xfffff464, "pioa_ppuer", 4, _RW_},
	{0xfffff468, "pioa_ppusr", 4, _RW_},
	{0xfffff470, "pioa_asr", 4, _RW_},
	{0xfffff474, "pioa_bsr", 4, _RW_},
	{0xfffff478, "pioa_absr", 4, _RW_},
	{0xfffff4a0, "pioa_ower", 4, _RW_},
	{0xfffff4a4, "pioa_owdr", 4, _RW_},
	{0xfffff4a8, "pioa_owsr", 4, _RW_},
	{0xfffff600, "base_piob", 4, _RW_},
	{0xfffff600, "piob_per", 4, _RW_},
	{0xfffff604, "piob_pdr", 4, _RW_},
	{0xfffff608, "piob_psr", 4, _RW_},
	{0xfffff610, "piob_oer", 4, _RW_},
	{0xfffff614, "piob_odr", 4, _RW_},
	{0xfffff618, "piob_osr", 4, _RW_},
	{0xfffff620, "piob_ifer", 4, _RW_},
	{0xfffff624, "piob_ifdr", 4, _RW_},
	{0xfffff628, "piob_ifsr", 4, _RW_},
	{0xfffff630, "piob_sodr", 4, _RW_},
	{0xfffff634, "piob_codr", 4, _RW_},
	{0xfffff638, "piob_odsr", 4, _RW_},
	{0xfffff63c, "piob_pdsr", 4, _RW_},
	{0xfffff640, "piob_ier", 4, _RW_},
	{0xfffff644, "piob_idr", 4, _RW_},
	{0xfffff648, "piob_imr", 4, _RW_},
	{0xfffff64c, "piob_isr", 4, _RW_},
	{0xfffff650, "piob_mder", 4, _RW_},
	{0xfffff654, "piob_mddr", 4, _RW_},
	{0xfffff658, "piob_mdsr", 4, _RW_},
	{0xfffff660, "piob_ppudr", 4, _RW_},
	{0xfffff664, "piob_ppuer", 4, _RW_},
	{0xfffff668, "piob_ppusr", 4, _RW_},
	{0xfffff670, "piob_asr", 4, _RW_},
	{0xfffff674, "piob_bsr", 4, _RW_},
	{0xfffff678, "piob_absr", 4, _RW_},
	{0xfffff6a0, "piob_ower", 4, _RW_},
	{0xfffff6a4, "piob_owdr", 4, _RW_},
	{0xfffff6a8, "piob_owsr", 4, _RW_},
	{0xfffffc00, "base_pmc", 4, _RW_},
	{0xfffffc00, "pmc_scer", 4, _RW_},
	{0xfffffc04, "pmc_scdr", 4, _RW_},
	{0xfffffc08, "pmc_scsr", 4, _RW_},
	{0xfffffc10, "pmc_pcer", 4, _RW_},
	{0xfffffc14, "pmc_pcdr", 4, _RW_},
	{0xfffffc18, "pmc_pcsr", 4, _RW_},
	{0xfffffc20, "base_ckgr", 4, _RW_},
	{0xfffffc20, "ckgr_mor", 4, _RW_},
	{0xfffffc20, "pmc_mor", 4, _RW_},
	{0xfffffc24, "ckgr_mcfr", 4, _RW_},
	{0xfffffc24, "pmc_mcfr", 4, _RW_},
	{0xfffffc2c, "ckgr_pllr", 4, _RW_},
	{0xfffffc2c, "pmc_pllr", 4, _RW_},
	{0xfffffc30, "pmc_mckr", 4, _RW_},
	{0xfffffc40, "pmc_pckr", 4, _RW_},
	{0xfffffc60, "pmc_ier", 4, _RW_},
	{0xfffffc64, "pmc_idr", 4, _RW_},
	{0xfffffc68, "pmc_sr", 4, _RW_},
	{0xfffffc6c, "pmc_imr", 4, _RW_},
	{0xfffffd00, "base_rstc", 4, _RW_},
	{0xfffffd00, "rstc_rcr", 4, _RW_},
	{0xfffffd04, "rstc_rsr", 4, _RW_},
	{0xfffffd08, "rstc_rmr", 4, _RW_},
	{0xfffffd20, "base_rttc", 4, _RW_},
	{0xfffffd20, "rttc_rtmr", 4, _RW_},
	{0xfffffd24, "rttc_rtar", 4, _RW_},
	{0xfffffd28, "rttc_rtvr", 4, _RW_},
	{0xfffffd2c, "rttc_rtsr", 4, _RW_},
	{0xfffffd30, "base_pitc", 4, _RW_},
	{0xfffffd30, "pitc_pimr", 4, _RW_},
	{0xfffffd34, "pitc_pisr", 4, _RW_},
	{0xfffffd38, "pitc_pivr", 4, _RW_},
	{0xfffffd3c, "pitc_piir", 4, _RW_},
	{0xfffffd40, "base_wdtc", 4, _RW_},
	{0xfffffd40, "wdtc_wdcr", 4, _RW_},
	{0xfffffd44, "wdtc_wdmr", 4, _RW_},
	{0xfffffd48, "wdtc_wdsr", 4, _RW_},
	{0xfffffd60, "base_vreg", 4, _RW_},
	{0xfffffd60, "vreg_mr", 4, _RW_},
	{0xffffff00, "base_mc", 4, _RW_},
	{0xffffff00, "mc_rcr", 4, _WO_},
	{0xffffff04, "mc_asr", 4, _RO_}, 
	{0xffffff08, "mc_aasr", 4, _RO_}, 
	{0xffffff60, "mc_fmr", 4, _RW_}, 
	{0xffffff64, "mc_fcr", 4, _WO_}, 
	{0xffffff68, "mc_fsr", 4, _RO_}, 
#endif
	{0, "", 0} 
};

static const uint16_t sramsize[16] = {
	0, 1, 2, 0, 112, 4, 80, 160, 8, 16, 32, 64, 128, 256, 96, 512 };

static const uint16_t nvpsize[16] = {
	0, 8, 16, 32, 0, 64, 0, 128, 0, 256, 512, 0, 1024, 0, 2048, 0 };

#define AT91SAM7_MCLK_HZ 48000000

int at91sam7_on_init(FILE * f, const ice_drv_t * ice, 
					 ice_mem_entry_t * mem)
{
	uint32_t data;
	uint32_t fmain;
	uint32_t fpll;
	uint32_t fsr;
	uint32_t cidr;
	int again;
	int pll_mul;
	int pll_div;
/*	int err;
	int min;
	int x;
	int x_mul;
	int x_min;
	int mul;
	int div; */

	/* ATTENTION: all clock frequencies are in KiloHertz to avoid
	 * multiplication overflow ... */
	/* main clock (KHz) configure 
	 *
	 * We select the main clock as 48MHz to enable the use of 
	 * USB device port 
	 */
	fpll = (AT91SAM7_MCLK_HZ * 2) / 1000;

	DCC_LOG(LOG_INFO, "reset peripherals ...");
	/* reset the peripherals */
	ice_wr32(ice, AT91_BASE_RSTC + RSTC_CR, 
			  RSTC_KEY(0xa5) | RSTC_PERRST);

	DCC_LOG(LOG_INFO, "configure nRST ...");
	/* configure the NRST pint to generate a reset */
	ice_wr32(ice, AT91_BASE_RSTC + RSTC_MR, 
			  RSTC_KEY(0xa5) | RSTC_URSTEN);

	DCC_LOG(LOG_INFO, "watchdog disable ...");
	/* Disable the watchdog */
	ice_wr32(ice,  AT91_BASE_WDTC + WDTC_WDMR, WDTC_WDDIS);

	DCC_LOG(LOG_INFO, "set master clock to slow clock ...");
	/* 0. Set the master clock to the slow clock : */
	ice_wr32(ice, AT91_BASE_PMC + PMC_MCKR, PMC_CSS_SLOW_CLK);


	/* disable the PLL */
	DCC_LOG(LOG_INFO, "PLL disable ...");
	ice_wr32(ice, AT91_BASE_CKGR + CKGR_PLLR, 0x00000000);

	DCC_LOG(LOG_INFO, "main oscillator disable ...");
	/* disable the Main Oscillator */
	ice_wr32(ice, AT91_BASE_CKGR + CKGR_MOR, 0x00000000);

	DCC_LOG(LOG_INFO, "wait for slow clock to be ready ...");

	for (again = 256; ; again--) {
		ice_rd32(ice, AT91_BASE_PMC + PMC_SR, &data);
		if (data & PMC_MCKRDY)
			break;
		if (again == 0) {
			fprintf(f, " %s: slow clock fail!\n", __func__);
			DCC_LOG(LOG_WARNING, "slow clock fail!");
			return -1;
		}
	}

	DCC_LOG(LOG_INFO, "main oscillator enable ...");
	/* 1. Enabling the Main Oscillator: */
	ice_wr32(ice, AT91_BASE_CKGR + CKGR_MOR, 0x00000701);

	/* wait for main startup time expiration */
	for (again = 256; ; again--) {
		ice_rd32(ice, AT91_BASE_PMC + PMC_SR, &data);
		if (data & PMC_MOSCS)
			break;
		if (again == 0) {
			fprintf(f, " %s: main oscillator startup fail!\n", __func__);
			DCC_LOG(LOG_WARNING, "main oscillator startup fail!");
			return -1;
		}
	}

	/* 2. Checking the Main Oscillator Frequency: */
	for (again = 256; ; again--) {
		ice_rd32(ice, AT91_BASE_CKGR + CKGR_MCFR, &data);
		if (data & CKGR_MAINRDY)
			break;
		if (again == 0) {
			fprintf(f, " %s: main oscillator ready fail!\n", __func__);
			DCC_LOG(LOG_WARNING, "main oscillator ready fail!");
			return -1;
		}
	}

	fmain = (data & CKGR_MAINF) * 32 / 16;

	DCC_LOG2(LOG_TRACE, "main oscillator frequency: %d.%d MHz", 
		(fmain / 100) / 10, (fmain / 100) % 10);



	/*
     * fpll = (fmain * pll_mul) / pll_div
	 */
/*
	pll_div = 0;
	pll_mul = 0;
	min = (fpll * 256);
	for (div = 2; div < 256; div++) {
		x_min = (fpll * div);
		x_mul = 0;
		for (x = 11; x >= 0; x--) {
			mul = x_mul + (1 << x);
			err = (fpll * div) - (fmain * mul);
			if (err < 0)
				err = -err;
			if (err < x_min) {
				x_mul = mul;
				x_min = err;
			}
			if (err < min) {
				pll_mul = mul;
				pll_div = div;
				min = err;
			}
		}
	}
*/
	if (fmain > 19500) {
		fmain = 20000;
		pll_mul = 48;
		pll_div = 10;
	} else {
		if (fmain > 18000) {
			fmain = 18432;
			pll_mul = 125;
			pll_div = 24;
		} else {
			fmain = 12000;
			pll_mul = 16;
			pll_div = 2;
		}
	}

    fpll = (fmain * pll_mul) / pll_div;
	fprintf(f, " %s: fmain=%u fpll=%u mul=%d div=%d\n", __func__, 
			fmain, fpll, pll_mul, pll_div);

	/*
     * fpll = (fmain * pll_mul) / pll_div
	 *
	 * fusb = fpll / n; (n = 1, 2, 4)
	 *
	 * fusb = fmain * (pll_mul / (pll_div * n))
	 * 
	 * pll_mul = (fusb * n * pll_div) / fmain
	 *
	 */

	/* 3. Setting PLL and divider: */
	DCC_LOG(LOG_INFO, "PLL and divider setting...");

	ice_wr32(ice, AT91_BASE_CKGR + CKGR_PLLR, 
			  CKGR_PLLMUL(pll_mul) | CKGR_PLLCOUNT(16) | 
			  CKGR_PLLDIV(pll_div) | CKGR_USBDIV(2));

	/* Wait the pll lock (until PMC Status register LOCK bit is set) */
	for (again = 256; ; again--) {
		ice_rd32(ice, AT91_BASE_PMC + PMC_SR, &data);
		if (data & PMC_LOCK)
			break;
		if (again == 0) {
			fprintf(f, " %s: pll lock fail!\n", __func__);
			DCC_LOG(LOG_WARNING, "pll lock fail!");
			return -1;
		}
	}

	/* 4. Selection of Master Clock and Processor Clock */
/*
   If a new value for CSS field corresponds to PLL Clock,
     – Program the PRES field in the PMC_MCKR register.
     – Wait for the MCKRDY bit to be set in the PMC_SR register.
     – Program the CSS field in the PMC_MCKR register.
     – Wait for the MCKRDY bit to be set in the PMC_SR register.
   If a new value for CSS field corresponds to Main Clock or Slow Clock,
     – Program the CSS field in the PMC_MCKR register.
     – Wait for the MCKRDY bit to be set in the PMC_SR register.
     – Program the PRES field in the PMC_MCKR register.
     – Wait for the MCKRDY bit to be set in the PMC_SR register
	 */

	DCC_LOG(LOG_INFO, "master and processor clock clock selection...");

	/* Master clock = Main Clock */
	ice_wr32(ice, AT91_BASE_PMC + PMC_MCKR, PMC_CSS_MAIN_CLK);
	
	for (again = 256; ; again--) {
		ice_rd32(ice, AT91_BASE_PMC + PMC_SR, &data);
		if (data & PMC_MCKRDY)
			break;
		if (again == 0) {
			fprintf(f, " %s: master clock set fail!\n", __func__);
			DCC_LOG(LOG_WARNING, "master clock set fail!");
			return -1;
		}
	}

	/* Chip identification.... */
	ice_rd32(ice, AT91_BASE_DBGU + DBGU_CIDR, &cidr);
	DCC_LOG1(LOG_TRACE, "cidr=0x%08x", cidr);

	/* Wrong chip id fix up */
	if (cidr == AT91SAM7S256_A_ID) {
		cidr = AT91SAM7S256_A_FIX_ID;
	}

	fprintf(f, " %s: ver=%d proc=%d nvpm=%dKiB,%dKiB sram=%dKiB\n", 
			__func__,
			CIDR_VER(cidr), CIDR_EPROC(cidr), 
			nvpsize[CIDR_NVPSIZ(cidr)], nvpsize[CIDR_NVPSIZ2(cidr)],
			sramsize[CIDR_SRAMSIZ(cidr)]);

	fprintf(f, " %s: arch=0x%02x nvtp=%d ext=%d\n",  __func__,
			CIDR_ARCH(cidr), CIDR_NVPTYP(cidr), CIDR_EXT(cidr));

	DCC_LOG(LOG_INFO, "flash configuration...");
	/* Flash configuration: waitstates and the number of Master 
	 * Clock cycles in 1.5 microseconds. 
	 */
	/* No Erase Before Programming */
	ice_wr32(ice, AT91_BASE_MC + MC_FMR, 
			MC_NEBP | MC_FWS(1) | MC_FMCN(AT91SAM7_MCLK_HZ / 666667));

	switch (CIDR_ARCH(cidr)) {
	case 0x70:
		DCC_LOG(LOG_TRACE, "AT91SAM7Sxx...");
		/* AT91SAM7Sxx Series*/
		break;
	case 0x75:
		/* AT91SAM7Xxx Series*/
		DCC_LOG(LOG_TRACE, "AT91SAM7Xxx...");
		fprintf(f, " %s: setting GPNVM bit 2\n", __func__);

		/* Select boot from flash: GPNVM bit 2 set to 1 */
		ice_wr32(ice, AT91_BASE_MC + MC_FCR, 
				 MC_KEY(0x5a) | MC_FCMD_SET_GP_NVM | MC_PAGEN(2));

		for (again = 4096; ; again--) {
			ice_rd32(ice, AT91_BASE_MC + MC_FSR, &data);
			if (data & MC_FRDY)
				break;
			if (again == 0) {
				fprintf(f, " %s: flash write fail!\n", __func__);
				return -1;
			}
		}
		break;
	default:
		DCC_LOG(LOG_TRACE, "Unknown ATMEL series...");
	}

	ice_rd32(ice, AT91_BASE_MC + MC_FSR, &fsr);
	fprintf(f, " %s: fsr=0x%08x locks=%06x gpnvm=%02x\n", __func__, 
			fsr, fsr >> 16, (fsr >> 8) & 0xff);


	/* Memory remap */
	DCC_LOG(LOG_INFO, "memory remap...");
	ice_wr32(ice, AT91_BASE_MC + MC_RCR, MC_RCB);

/*	ice_wr32(ice, PIOA_PER, 0xffffffff);
	ice_wr32(ice, PIOA_OER, 0xffffffff);
	ice_wr32(ice, PIOA_CODR, 0xffffffff); 
	ice_wr32(ice, PIOA_SODR, 0xffffffff); */

	return 0;
}

int at91sam7_on_halt(FILE * f, const ice_drv_t * ice, ice_mem_entry_t * mem)
{
	/* Disable the watchdog */
	return ice_wr32(ice, AT91_BASE_WDTC + WDTC_WDMR, WDTC_WDDIS);
}

int at91sam7_reset(FILE * f, const ice_drv_t * ice, ice_mem_entry_t * mem)
{
	uint32_t cr_addr;
	uint32_t cr_data;
	uint32_t data;
	int again;
	uint32_t addr;
	uint32_t r[2];

	fprintf(f, " - AT91SAM RSTC reset.\n");

	DCC_LOG(LOG_INFO, "master clock set to slow clock...");
	/* 0. Set the master clock to the slow clock : */
	ice_wr32(ice, AT91_BASE_PMC + PMC_MCKR, 0x00000000);

	for (again = 4096; ; again--) {
		ice_rd32(ice, AT91_BASE_PMC + PMC_SR, &data);
		if (data & PMC_MCKRDY)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "master clock not ready!");
			return -1;
		}
	}

	cr_addr = AT91_BASE_RSTC + RSTC_CR;
	cr_data = RSTC_KEY(0xa5) | RSTC_EXTRST | RSTC_PROCRST | RSTC_PERRST;

	DCC_LOG2(LOG_INFO, "PROCRST CR(0x%08x)=0x%08x", cr_addr, cr_data);

	/* write some code into the memory */
	addr = 0x00200000;
	ice_wr32(ice, addr, ARM_STR(ARM_R0, ARM_R1, 0));
	ice_wr32(ice, addr + 4, ARM_B(-8)); /* loop at the same place */
	/* set the registers */
	r[0] = cr_data;
	r[1] = cr_addr;
	/* execute */
	ice_exec(ice,  addr, r, 2);

	/* wait 10ms for the reset to complete. */
	__os_sleep(10);

	return 0;
}

int at91sam7s_flash256_erase(armice_ctrl_t * ctrl, ice_mem_ref_t * addr,
							 ice_size_t len)
{
	unsigned int data;
	int again;
	int page;

	for (again = 4096; ; again--) {
		armice_rd32(ctrl, AT91_BASE_MC + MC_FSR, &data);
		if (data & MC_FRDY)
		break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	/* Erase Before Programming */
	armice_wr32(ctrl, AT91_BASE_MC + MC_FMR, 
			 MC_FWS(1) | MC_FMCN(AT91SAM7_MCLK_HZ / 666667));

	page = addr->offs >> 8;

	DCC_LOG4(LOG_INFO, "%08x:%06x len=%d pg=%d", 
			 addr->base, addr->offs, len, page);

	armice_wr32(ctrl, AT91_BASE_MC + MC_FCR, MC_KEY(0x5a) | 
				MC_FCMD_START_PROG | (page << 8));

	return 256;
}

int at91sam7s_flash256_write(armice_ctrl_t * ctrl, ice_mem_ref_t * addr,
							 const void * ptr, ice_size_t len)
{
	unsigned int data;
	int again;
	int page;
	int n;

	for (again = 4096; ; again--) {
		armice_rd32(ctrl, AT91_BASE_MC + MC_FSR, &data);
		if (data & MC_FRDY)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	/* No Erase Before Programming */
	armice_wr32(ctrl, AT91_BASE_MC + MC_FMR, 
			MC_NEBP | MC_FWS(1) | MC_FMCN(AT91SAM7_MCLK_HZ / 666667));


	DCC_LOG1(LOG_MSG, "tries=%d", 4096 - again);

	page = addr->offs >> 8;

	DCC_LOG4(LOG_INFO, "%08x:%06x len=%d pg=%d", 
			 addr->base, addr->offs, len, page);

	if ((n = armice_mem_write(ctrl, addr, ptr, len)) < 0) {
		DCC_LOG(LOG_WARNING, "armice_mem_write() fail!");
		return -1;
	}

	if (armice_wr32(ctrl, AT91_BASE_MC + MC_FCR, MC_KEY(0x5a) | 
				 MC_FCMD_START_PROG | (page << 8)) < 0) {
		DCC_LOG(LOG_WARNING, "arm_wr() fail!");
		return -1;
	}
	
	return n;
}

int at91sam7s_flash128_erase(armice_ctrl_t * ctrl, ice_mem_ref_t * addr,
							 ice_size_t len)
{
	unsigned int data;
	int again;
	int page;

	for (again = 4096; ; again--) {
		armice_rd32(ctrl, AT91_BASE_MC + MC_FSR, &data);
		if (data & MC_FRDY)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	page = addr->offs >> 7;

	DCC_LOG4(LOG_INFO, "%08x:%06x len=%d pg=%d", 
			 addr->base, addr->offs, len, page);

	armice_wr32(ctrl, AT91_BASE_MC + MC_FCR, 0x5A000000 | 
				MC_FCMD_START_PROG | (page << 8));

	return 128;
}

int at91sam7s_flash128_write(armice_ctrl_t * ctrl, ice_mem_ref_t * addr,
							 const void * ptr, ice_size_t len)
{
	uint32_t data;
	int again;
	int page;
	int n;

	addr->offs &= 0xfffffffe;

	for (again = 1024; ; again--) {
		armice_rd32(ctrl, AT91_BASE_MC + MC_FSR, &data);
		if (data & MC_FRDY)
			break;
		if (again == 0) {
			DCC_LOG(LOG_WARNING, "flash not ready!");
			return -1;
		}
	}

	page = addr->offs >> 7;

	DCC_LOG4(LOG_INFO, "%08x:%06x len=%d pg=%d", addr->base, 
			 addr->offs, len, page);

	if ((n = armice_mem_write(ctrl, addr, ptr, len)) < 0) {
		DCC_LOG(LOG_WARNING, "armice_mem_write() fail!");
		return -1;
	}

	if (armice_wr32(ctrl, AT91_BASE_MC + MC_FCR, 
				  0x5A000000 | MC_FCMD_START_PROG | (page << 8)) < 0) {
		DCC_LOG(LOG_WARNING, "arm_wr() fail!");
		return -1;
	}
	
	return n;
}

int at9sam7x256_probe(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem)
{
	uint32_t idr;

	ice_rd32(ice, AT91_BASE_DBGU + DBGU_CIDR, &idr);

	DCC_LOG1(LOG_TRACE, "CIDR=0x%08x", idr);
	fprintf(f, "cidr=0x%08x ", idr);

	return (idr == AT91SAM7X256_ID) ? 1 : 0;
}

int at9sam7s256_probe(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem)
{
	uint32_t idr;

	ice_rd32(ice, AT91_BASE_DBGU + DBGU_CIDR, &idr);

	DCC_LOG1(LOG_TRACE, "CIDR=0x%08x", idr);
	fprintf(f, "cidr=0x%08x ", idr);

	return ((idr == AT91SAM7S256_A_ID) || 
			(idr == AT91SAM7S256_B_ID)) ? 1 : 0;
}

int at9sam7s64_probe(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem)
{
	uint32_t idr;

	ice_rd32(ice, AT91_BASE_DBGU + DBGU_CIDR, &idr);

	DCC_LOG1(LOG_TRACE, "CIDR=0x%08x", idr);
	fprintf(f, "cidr=0x%08x ", idr);

	return ((idr == AT91SAM7S64_A_ID) || 
			(idr == AT91SAM7S64_B_ID)) ? 1 : 0;
}

int at9sam7xc256_probe(FILE * f, const struct ice_drv * ice, 
					   ice_mem_entry_t * mem)
{
	uint32_t idr;

	ice_rd32(ice, AT91_BASE_DBGU + DBGU_CIDR, &idr);

	DCC_LOG1(LOG_TRACE, "CIDR=0x%08x", idr);
	fprintf(f, "cidr=0x%08x ", idr);

	return (idr == AT91SAM7XC256_ID) ? 1 : 0;
}



int at9sam7_id_test(FILE * f, const struct ice_drv * ice)
{
	uint32_t cidr;

	/* Chip identification.... */
	ice_rd32(ice, AT91_BASE_DBGU + DBGU_CIDR, &cidr);
	fprintf(f, " - cidr=0x%08x\n", cidr);

	/* Wrong chip id fix up */
	if (cidr == AT91SAM7S256_A_ID) {
		cidr = AT91SAM7S256_A_FIX_ID;
	}

	fprintf(f, " - %s: ver=%d proc=%d nvpm=%dKiB,%dKiB sram=%dKiB\n", 
			__func__,
			CIDR_VER(cidr), CIDR_EPROC(cidr), 
			nvpsize[CIDR_NVPSIZ(cidr)], nvpsize[CIDR_NVPSIZ2(cidr)],
			sramsize[CIDR_SRAMSIZ(cidr)]);

	fprintf(f, " - %s: arch=0x%02x nvtp=%d ext=%d\n",  __func__,
			CIDR_ARCH(cidr), CIDR_NVPTYP(cidr), CIDR_EXT(cidr));
	return 0;
}

int at9sam7_test(FILE * f, const struct ice_drv * ice, 
				  ice_mem_entry_t * mem, uint32_t val)
{
	fprintf(f, "\n");
	switch (val) {
	case 1:
		return at9sam7_id_test(f, ice);
	default:
		fprintf(f, "# Valid test numbers:\n");
		fprintf(f, "  1 - AT91SAM7 ID test\n");
		return -1;
	};

	return 0;
}



/*
 * at91sam7 flash
 */
const struct ice_mem_oper flash_at91sam7_128_oper = {
	.read = (ice_mem_read_t)armice_mem_read,
	.write = (ice_mem_write_t)at91sam7s_flash128_write, 
	.erase = (ice_mem_erase_t)at91sam7s_flash128_erase
};

const struct ice_mem_oper flash_at91sam7_256_oper = {
	.read = (ice_mem_read_t)armice_mem_read,
	.write = (ice_mem_write_t)at91sam7s_flash256_write, 
	.erase = (ice_mem_erase_t)at91sam7s_flash256_erase
};

const struct target_cpu at91sam7_cpu = {
	.family = "AT91SAM7",
	.model = "---",
	.vendor = "ATMEL",
	.idmask = 0xffffffff,
	.idcomp = 0x3f0f0f0f,
	.sym = at91sam7_sym
};

const struct armice_cfg at91sam7_cfg = {
	.endianness = LITTLE_ENDIAN,
	.work_addr = 0x00200000,
	.work_size = MEM_KiB(8)
};

