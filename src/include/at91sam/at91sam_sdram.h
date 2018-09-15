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
 * @file arch/at91_.h
 * @brief ARM CPU macros and definitions 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __AT91_SDRAM_H__
#define __AT91_SDRAM_H__

/* SDRAM Controller Interface */

#define SDRAMC_MR       ( 0) /* SDRAM Controller Mode Register */
#define SDRAMC_TR       ( 4) /* SDRAM Controller Refresh Timer Register */
#define SDRAMC_CR       ( 8) /* SDRAM Controller Configuration Register */
#define SDRAMC_HSR      (12) /* SDRAM Controller High Speed Register */
#define SDRAMC_LPR      (16) /* SDRAM Controller Low Power Register */
#define SDRAMC_IER      (20) /* SDRAM Controller Interrupt Enable Register */
#define SDRAMC_IDR      (24) /* SDRAM Controller Interrupt Disable Register */
#define SDRAMC_IMR      (28) /* SDRAM Controller Interrupt Mask Register */
#define SDRAMC_ISR      (32) /* SDRAM Controller Interrupt Mask Register */
#define SDRAMC_MDR      (36) /* SDRAM Memory Device Register */

/* SDRAMC_MR - SDRAM Controller Mode Register */
#define SDRAMC_MODE         (0xf <<  0) /* (SDRAMC) Mode */
#define SDRAMC_MODE_NORMAL_CMD           (0x0) /* (SDRAMC) Normal Mode */
#define SDRAMC_MODE_NOP_CMD              (0x1) /* (SDRAMC) Issue a NOP Command at every access */
#define SDRAMC_MODE_PRCGALL_CMD          (0x2) /* (SDRAMC) Issue a All Banks Precharge Command at every access */
#define SDRAMC_MODE_LMR_CMD              (0x3) /* (SDRAMC) Issue a Load Mode Register at every access */
#define SDRAMC_MODE_RFSH_CMD             (0x4) /* (SDRAMC) Issue a Refresh */
#define SDRAMC_MODE_EXT_LMR_CMD          (0x5) /* (SDRAMC) Issue an Extended Load Mode Register */
#define SDRAMC_MODE_DEEP_CMD             (0x6) /* (SDRAMC) Enter Deep Power Mode */

/* SDRAMC_TR - SDRAMC Refresh Timer Register */
#define SDRAMC_COUNT(VAL)        (((VAL) & 0xfff) <<  0) /* (SDRAMC) Refresh Counter */

/* SDRAMC_CR - SDRAM Configuration Register */
/* Number of Column Bits */
#define SDRAMC_NC(X)        ((((X) - 8) & 0x3) << 0) 
#define SDRAMC_NC_GET(CR)   ((((CR) >> 0) & 0x3) + 8) 
#define SDRAMC_NC_8         (0x0) /*  8 Bits */
#define SDRAMC_NC_9         (0x1) /*  9 Bits */
#define SDRAMC_NC_10        (0x2) /*  10 Bits */
#define SDRAMC_NC_11        (0x3) /*  11 Bits */
/* Number of Row Bits */
#define SDRAMC_NR(X)        ((((X) - 11) & 0x3) << 2) 
#define SDRAMC_NR_GET(CR)   ((((CR) >> 2) & 0x3) + 11) 
#define SDRAMC_NR_11        (0x0 <<  2) /*  11 Bits */
#define SDRAMC_NR_12        (0x1 <<  2) /*  12 Bits */
#define SDRAMC_NR_13        (0x2 <<  2) /*  13 Bits */
/*  Number of Banks */
#define SDRAMC_NB(X)        ((((X) >> 2) & 0x1) << 4) 
#define SDRAMC_NB_GET(CR)   ((((CR) >> 4) & 0x1) << 2) 
#define SDRAMC_NB_2_BANKS   (0x0 <<  4) /*  2 banks */
#define SDRAMC_NB_4_BANKS   (0x1 <<  4) /*  4 banks */
/*  CAS Latency */
#define SDRAMC_CAS(X)       ((((X) - 0) & 0x3) << 5) 
#define SDRAMC_CAS_GET(CR)  ((((CR) >> 5) & 0x3) + 0) 
#define SDRAMC_CAS_2        (0x2 <<  5) /*  2 cycles */
#define SDRAMC_CAS_3        (0x3 <<  5) /*  3 cycles */
#define SDRAMC_DBW          (0x1 <<  7) /*  Data Bus Width */
#define SDRAMC_DBW_32_BITS  (0x0 <<  7) /* (SDRAMC) 32 Bits datas bus */
#define SDRAMC_DBW_16_BITS  (0x1 <<  7) /* (SDRAMC) 16 Bits datas bus */
/* (SDRAMC) Number of Write Recovery Time Cycles */
#define SDRAMC_TWR(X)       (((X) & 0xf) <<  8) 
#define SDRAMC_TWR_GET(CR)  (((CR) >> 8) & 0xf) 
#define SDRAMC_TWR_0        (0x0 <<  8) /* (SDRAMC) Value :  0 */
#define SDRAMC_TWR_1        (0x1 <<  8) /* (SDRAMC) Value :  1 */
#define SDRAMC_TWR_2        (0x2 <<  8) /* (SDRAMC) Value :  2 */
#define SDRAMC_TWR_3        (0x3 <<  8) /* (SDRAMC) Value :  3 */
#define SDRAMC_TWR_4        (0x4 <<  8) /* (SDRAMC) Value :  4 */
#define SDRAMC_TWR_5        (0x5 <<  8) /* (SDRAMC) Value :  5 */
#define SDRAMC_TWR_6        (0x6 <<  8) /* (SDRAMC) Value :  6 */
#define SDRAMC_TWR_7        (0x7 <<  8) /* (SDRAMC) Value :  7 */
#define SDRAMC_TWR_8        (0x8 <<  8) /* (SDRAMC) Value :  8 */
#define SDRAMC_TWR_9        (0x9 <<  8) /* (SDRAMC) Value :  9 */
#define SDRAMC_TWR_10       (0xa <<  8) /* (SDRAMC) Value : 10 */
#define SDRAMC_TWR_11       (0xb <<  8) /* (SDRAMC) Value : 11 */
#define SDRAMC_TWR_12       (0xc <<  8) /* (SDRAMC) Value : 12 */
#define SDRAMC_TWR_13       (0xd <<  8) /* (SDRAMC) Value : 13 */
#define SDRAMC_TWR_14       (0xe <<  8) /* (SDRAMC) Value : 14 */
#define SDRAMC_TWR_15       (0xf <<  8) /* (SDRAMC) Value : 15 */
/* (SDRAMC) Number of RAS Cycle Time Cycles */
#define SDRAMC_TRC(X)       (((X) & 0xf) << 12) 
#define SDRAMC_TRC_GET(CR)   (((CR) >> 12) & 0xf) 
#define SDRAMC_TRC_0        (0x0 << 12) /* (SDRAMC) Value :  0 */
#define SDRAMC_TRC_1        (0x1 << 12) /* (SDRAMC) Value :  1 */
#define SDRAMC_TRC_2        (0x2 << 12) /* (SDRAMC) Value :  2 */
#define SDRAMC_TRC_3        (0x3 << 12) /* (SDRAMC) Value :  3 */
#define SDRAMC_TRC_4        (0x4 << 12) /* (SDRAMC) Value :  4 */
#define SDRAMC_TRC_5        (0x5 << 12) /* (SDRAMC) Value :  5 */
#define SDRAMC_TRC_6        (0x6 << 12) /* (SDRAMC) Value :  6 */
#define SDRAMC_TRC_7        (0x7 << 12) /* (SDRAMC) Value :  7 */
#define SDRAMC_TRC_8        (0x8 << 12) /* (SDRAMC) Value :  8 */
#define SDRAMC_TRC_9        (0x9 << 12) /* (SDRAMC) Value :  9 */
#define SDRAMC_TRC_10       (0xa << 12) /* (SDRAMC) Value : 10 */
#define SDRAMC_TRC_11       (0xb << 12) /* (SDRAMC) Value : 11 */
#define SDRAMC_TRC_12       (0xc << 12) /* (SDRAMC) Value : 12 */
#define SDRAMC_TRC_13       (0xd << 12) /* (SDRAMC) Value : 13 */
#define SDRAMC_TRC_14       (0xe << 12) /* (SDRAMC) Value : 14 */
#define SDRAMC_TRC_15       (0xf << 12) /* (SDRAMC) Value : 15 */
/* (SDRAMC) Number of RAS Precharge Time Cycles */
#define SDRAMC_TRP(X)       (((X) & 0xf) << 16) 
#define SDRAMC_TRP_GET(CR)  (((CR) >> 16) & 0xf) 
#define SDRAMC_TRP_0        (0x0 << 16) /* (SDRAMC) Value :  0 */
#define SDRAMC_TRP_1        (0x1 << 16) /* (SDRAMC) Value :  1 */
#define SDRAMC_TRP_2        (0x2 << 16) /* (SDRAMC) Value :  2 */
#define SDRAMC_TRP_3        (0x3 << 16) /* (SDRAMC) Value :  3 */
#define SDRAMC_TRP_4        (0x4 << 16) /* (SDRAMC) Value :  4 */
#define SDRAMC_TRP_5        (0x5 << 16) /* (SDRAMC) Value :  5 */
#define SDRAMC_TRP_6        (0x6 << 16) /* (SDRAMC) Value :  6 */
#define SDRAMC_TRP_7        (0x7 << 16) /* (SDRAMC) Value :  7 */
#define SDRAMC_TRP_8        (0x8 << 16) /* (SDRAMC) Value :  8 */
#define SDRAMC_TRP_9        (0x9 << 16) /* (SDRAMC) Value :  9 */
#define SDRAMC_TRP_10       (0xa << 16) /* (SDRAMC) Value : 10 */
#define SDRAMC_TRP_11       (0xb << 16) /* (SDRAMC) Value : 11 */
#define SDRAMC_TRP_12       (0xc << 16) /* (SDRAMC) Value : 12 */
#define SDRAMC_TRP_13       (0xd << 16) /* (SDRAMC) Value : 13 */
#define SDRAMC_TRP_14       (0xe << 16) /* (SDRAMC) Value : 14 */
#define SDRAMC_TRP_15       (0xf << 16) /* (SDRAMC) Value : 15 */
/* (SDRAMC) Number of RAS to CAS Delay Cycles */
#define SDRAMC_TRCD(X)      (((X) & 0xf) << 20)
#define SDRAMC_TRCD_GET(CR) (((CR) >> 20) & 0xf) 
#define SDRAMC_TRCD_0       (0x0 << 20) /* (SDRAMC) Value :  0 */
#define SDRAMC_TRCD_1       (0x1 << 20) /* (SDRAMC) Value :  1 */
#define SDRAMC_TRCD_2       (0x2 << 20) /* (SDRAMC) Value :  2 */
#define SDRAMC_TRCD_3       (0x3 << 20) /* (SDRAMC) Value :  3 */
#define SDRAMC_TRCD_4       (0x4 << 20) /* (SDRAMC) Value :  4 */
#define SDRAMC_TRCD_5       (0x5 << 20) /* (SDRAMC) Value :  5 */
#define SDRAMC_TRCD_6       (0x6 << 20) /* (SDRAMC) Value :  6 */
#define SDRAMC_TRCD_7       (0x7 << 20) /* (SDRAMC) Value :  7 */
#define SDRAMC_TRCD_8       (0x8 << 20) /* (SDRAMC) Value :  8 */
#define SDRAMC_TRCD_9       (0x9 << 20) /* (SDRAMC) Value :  9 */
#define SDRAMC_TRCD_10      (0xa << 20) /* (SDRAMC) Value : 10 */
#define SDRAMC_TRCD_11      (0xb << 20) /* (SDRAMC) Value : 11 */
#define SDRAMC_TRCD_12      (0xc << 20) /* (SDRAMC) Value : 12 */
#define SDRAMC_TRCD_13      (0xd << 20) /* (SDRAMC) Value : 13 */
#define SDRAMC_TRCD_14      (0xe << 20) /* (SDRAMC) Value : 14 */
#define SDRAMC_TRCD_15      (0xf << 20) /* (SDRAMC) Value : 15 */
/* (SDRAMC) Number of RAS Active Time Cycles */
#define SDRAMC_TRAS(X)      (((X) & 0xf) << 24)
#define SDRAMC_TRAS_GET(CR) (((CR) >> 24) & 0xf)
#define SDRAMC_TRAS_0       (0x0 << 24) /* (SDRAMC) Value :  0 */
#define SDRAMC_TRAS_1       (0x1 << 24) /* (SDRAMC) Value :  1 */
#define SDRAMC_TRAS_2       (0x2 << 24) /* (SDRAMC) Value :  2 */
#define SDRAMC_TRAS_3       (0x3 << 24) /* (SDRAMC) Value :  3 */
#define SDRAMC_TRAS_4       (0x4 << 24) /* (SDRAMC) Value :  4 */
#define SDRAMC_TRAS_5       (0x5 << 24) /* (SDRAMC) Value :  5 */
#define SDRAMC_TRAS_6       (0x6 << 24) /* (SDRAMC) Value :  6 */
#define SDRAMC_TRAS_7       (0x7 << 24) /* (SDRAMC) Value :  7 */
#define SDRAMC_TRAS_8       (0x8 << 24) /* (SDRAMC) Value :  8 */
#define SDRAMC_TRAS_9       (0x9 << 24) /* (SDRAMC) Value :  9 */
#define SDRAMC_TRAS_10      (0xa << 24) /* (SDRAMC) Value : 10 */
#define SDRAMC_TRAS_11      (0xb << 24) /* (SDRAMC) Value : 11 */
#define SDRAMC_TRAS_12      (0xc << 24) /* (SDRAMC) Value : 12 */
#define SDRAMC_TRAS_13      (0xd << 24) /* (SDRAMC) Value : 13 */
#define SDRAMC_TRAS_14      (0xe << 24) /* (SDRAMC) Value : 14 */
#define SDRAMC_TRAS_15      (0xf << 24) /* (SDRAMC) Value : 15 */
/* (SDRAMC) Number of Command Recovery Time Cycles */
#define SDRAMC_TXSR(X)      (((X) & 0xf) << 28) 
#define SDRAMC_TXSR_GET(CR) (((CR) >> 28) & 0xf)
#define SDRAMC_TXSR_0       (0x0 << 28) /* (SDRAMC) Value :  0 */
#define SDRAMC_TXSR_1       (0x1 << 28) /* (SDRAMC) Value :  1 */
#define SDRAMC_TXSR_2       (0x2 << 28) /* (SDRAMC) Value :  2 */
#define SDRAMC_TXSR_3       (0x3 << 28) /* (SDRAMC) Value :  3 */
#define SDRAMC_TXSR_4       (0x4 << 28) /* (SDRAMC) Value :  4 */
#define SDRAMC_TXSR_5       (0x5 << 28) /* (SDRAMC) Value :  5 */
#define SDRAMC_TXSR_6       (0x6 << 28) /* (SDRAMC) Value :  6 */
#define SDRAMC_TXSR_7       (0x7 << 28) /* (SDRAMC) Value :  7 */
#define SDRAMC_TXSR_8       (0x8 << 28) /* (SDRAMC) Value :  8 */
#define SDRAMC_TXSR_9       (0x9 << 28) /* (SDRAMC) Value :  9 */
#define SDRAMC_TXSR_10      (0xa << 28) /* (SDRAMC) Value : 10 */
#define SDRAMC_TXSR_11      (0xb << 28) /* (SDRAMC) Value : 11 */
#define SDRAMC_TXSR_12      (0xc << 28) /* (SDRAMC) Value : 12 */
#define SDRAMC_TXSR_13      (0xd << 28) /* (SDRAMC) Value : 13 */
#define SDRAMC_TXSR_14      (0xe << 28) /* (SDRAMC) Value : 14 */
#define SDRAMC_TXSR_15      (0xf << 28) /* (SDRAMC) Value : 15 */

/* SDRAMC_HSR - SDRAM Controller High Speed Register */
#define SDRAMC_DA           (0x1 <<  0) /* (SDRAMC) Decode Cycle Enable Bit */
#define SDRAMC_DA_DISABLE   (0x0) /* (SDRAMC) Disable Decode Cycle */
#define SDRAMC_DA_ENABLE    (0x1) /* (SDRAMC) Enable Decode Cycle */

/* SDRAMC_LPR - SDRAM Controller Low-power Register */
#define SDRAMC_LPCB         (0x3 <<  0) /* (SDRAMC) Low-power Configurations */
#define SDRAMC_LPCB_DISABLE   (0x0) /* (SDRAMC) Disable Low Power Features */
#define SDRAMC_LPCB_SELF_REFRESH  (0x1) /* (SDRAMC) Enable SELF_REFRESH */
#define SDRAMC_LPCB_POWER_DOWN      (0x2) /* (SDRAMC) Enable POWER_DOWN */
#define SDRAMC_LPCB_DEEP_POWER_DOWN (0x3) /* (SDRAMC) Enable DEEP_POWER_DOWN */
#define SDRAMC_PASR         (0x7 <<  4) /* (SDRAMC) Partial Array Self Refresh (only for Low Power SDRAM) */
#define SDRAMC_TCSR         (0x3 <<  8) /* (SDRAMC) Temperature Compensated Self Refresh (only for Low Power SDRAM) */
#define SDRAMC_DS           (0x3 << 10) /* (SDRAMC) Drive Strenght (only for Low Power SDRAM) */
#define SDRAMC_TIMEOUT      (0x3 << 12) /* (SDRAMC) Time to define when Low Power Mode is enabled */
#define SDRAMC_TIMEOUT_0_CLK_CYCLES         (0x0 << 12) /* (SDRAMC) Activate SDRAM Low Power Mode Immediately */
#define SDRAMC_TIMEOUT_64_CLK_CYCLES        (0x1 << 12) /* (SDRAMC) Activate SDRAM Low Power Mode after 64 clock cycles after the end of the last transfer */
#define SDRAMC_TIMEOUT_128_CLK_CYCLES       (0x2 << 12) /* (SDRAMC) Activate SDRAM Low Power Mode after 64 clock cycles after the end of the last transfer */

/* SDRAMC_IER - SDRAM Controller Interrupt Enable Register */
#define SDRAMC_RES          (0x1 <<  0) /* (SDRAMC) Refresh Error Status */

/* SDRAMC_IDR - SDRAM Controller Interrupt Disable Register */

/* SDRAMC_IMR - SDRAM Controller Interrupt Mask Register */

/* SDRAMC_ISR - SDRAM Controller Interrupt Status Register */

/* SDRAMC_MDR - SDRAM Controller Memory Device Register */
#define SDRAMC_MD           (0x3 <<  0) /* (SDRAMC) Memory Device Type */
#define SDRAMC_MD_SDRAM                (0x0) /* (SDRAMC) SDRAM Mode */
#define SDRAMC_MD_LOW_POWER_SDRAM      (0x1) /* (SDRAMC) SDRAM Low Power Mode */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_sdramc {
	volatile uint32_t mr; /* SDRAM Controller Mode Register */
	volatile uint32_t tr; /* SDRAM Controller Refresh Timer Register */
	volatile uint32_t cr; /* SDRAM Controller Configuration Register */
	volatile uint32_t hsr; /* SDRAM Controller High Speed Register */
	volatile uint32_t lpr; /* SDRAM Controller Low Power Register */
	volatile uint32_t ier; /* SDRAM Controller Interrupt Enable Register */
	volatile uint32_t idr; /* SDRAM Controller Interrupt Disable Register */
	volatile uint32_t imr; /* SDRAM Controller Interrupt Mask Register */
	volatile uint32_t isr; /* SDRAM Controller Interrupt Mask Register */
	volatile uint32_t mdr; /* SDRAM Memory Device Register */
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_SDRAM_H__ */

