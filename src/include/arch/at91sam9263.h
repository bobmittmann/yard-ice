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
 * @file arch/at91sam9263.h
 * @brief ARM CPU macros and definitions 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __AT91SAM9263_H__
#define __AT91SAM9263_H__

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif /* __ASSEMBLER__ */

#include <at91sam/at91sam_ebi.h>
#include <at91sam/at91sam_ecc.h>
#include <at91sam/at91sam_sdram.h>
#include <at91sam/at91sam_smc.h>
#include <at91sam/at91sam_matrix.h>
#include <at91sam/at91sam_ccfg.h>
#include <at91sam/at91sam_pdc.h>
#include <at91sam/at91sam_dbgu.h>
#include <at91sam/at91sam_aic.h>
#include <at91sam/at91sam_pio.h>
#include <at91sam/at91sam_ckgr.h>
#include <at91sam/at91sam_pmc.h>
#include <at91sam/at91sam_rstc.h>
#include <at91sam/at91sam_shdwc.h>
#include <at91sam/at91sam_rttc.h>
#include <at91sam/at91sam_pitc.h>
#include <at91sam/at91sam_wdtc.h>
#include <at91sam/at91sam_tc.h>
#include <at91sam/at91sam_mci.h>
#include <at91sam/at91sam_twi.h>
#include <at91sam/at91sam_us.h>
#include <at91sam/at91sam_ssc.h>
#include <at91sam/at91sam_spi.h>
#include <at91sam/at91sam_adc.h>
#include <at91sam/at91sam_emac.h>
#include <at91sam/at91sam_udp.h>
#include <at91sam/at91sam_uhp.h>
#include <at91sam/at91sam_isi.h>

/* REGISTER ADDRESS DEFINITION FOR AT91SAM9260 */
/* Register definition for SYS peripheral */
#define SYS_GPBR3           (0xffffffff) /* (SYS) General Purpose Register 3 */
#define SYS_GPBR1           (0xffffffff) /* (SYS) General Purpose Register 1 */
#define SYS_GPBR2           (0xffffffff) /* (SYS) General Purpose Register 2 */
#define SYS_GPBR0           (0xffffffff) /* (SYS) General Purpose Register 0 */

/* Hardware register definition */

/* GPBR : (SYS Offset: 0x1350) GPBR General Purpose Register */
/* GPBR : (SYS Offset: 0x1354) GPBR General Purpose Register */
/* GPBR : (SYS Offset: 0x1358) GPBR General Purpose Register */
/* GPBR : (SYS Offset: 0x135c) GPBR General Purpose Register */


/* PIO Definitions FOR AT91SAM9263 */
#define PIO_PA0        (1 << 0) /* Pin Controlled by PA0 */
#define PA0_MCI0_DA0 (PIO_PA0)
#define PA0_SPI0_MISO (PIO_PA0)
#define PIO_PA1        (1 << 1) /* Pin Controlled by PA1 */
#define PA1_MCI0_CDA (PIO_PA1)
#define PA1_SPI0_MOSI (PIO_PA1)
#define PIO_PA10       (1 << 10) /* Pin Controlled by PA10 */
#define PA10_MCI1_DA2 (PIO_PA10)
#define PIO_PA11       (1 << 11) /* Pin Controlled by PA11 */
#define PA11_MCI1_DA3 (PIO_PA11)
#define PIO_PA12       (1 << 12) /* Pin Controlled by PA12 */
#define PA12_MCI0_CK  (PIO_PA12)
#define PIO_PA13       (1 << 13) /* Pin Controlled by PA13 */
#define PA13_CANTX    (PIO_PA13)
#define PA13_PCK0     (PIO_PA13)
#define PIO_PA14       (1 << 14) /* Pin Controlled by PA14 */
#define PA14_CANRX    (PIO_PA14)
#define PA14_IRQ0     (PIO_PA14)
#define PIO_PA15       (1 << 15) /* Pin Controlled by PA15 */
#define PA15_TCLK2    (PIO_PA15)
#define PA15_IRQ1     (PIO_PA15)
#define PIO_PA16       (1 << 16) /* Pin Controlled by PA16 */
#define PA16_MCI0_CDB (PIO_PA16)
#define PA16_EBI1_D16 (PIO_PA16)
#define PIO_PA17       (1 << 17) /* Pin Controlled by PA17 */
#define PA17_MCI0_DB0 (PIO_PA17)
#define PA17_EBI1_D17 (PIO_PA17)
#define PIO_PA18       (1 << 18) /* Pin Controlled by PA18 */
#define PA18_MCI0_DB1 (PIO_PA18)
#define PA18_EBI1_D18 (PIO_PA18)
#define PIO_PA19       (1 << 19) /* Pin Controlled by PA19 */
#define PA19_MCI0_DB2 (PIO_PA19)
#define PA19_EBI1_D19 (PIO_PA19)
#define PIO_PA2        (1 << 2) /* Pin Controlled by PA2 */
#define PA2_UNCONNECTED_PA2_A (PIO_PA2)
#define PA2_SPI0_SPCK (PIO_PA2)
#define PIO_PA20       (1 << 20) /* Pin Controlled by PA20 */
#define PA20_MCI0_DB3 (PIO_PA20)
#define PA20_EBI1_D20 (PIO_PA20)
#define PIO_PA21       (1 << 21) /* Pin Controlled by PA21 */
#define PA21_MCI1_CDB (PIO_PA21)
#define PA21_EBI1_D21 (PIO_PA21)
#define PIO_PA22       (1 << 22) /* Pin Controlled by PA22 */
#define PA22_MCI1_DB0 (PIO_PA22)
#define PA22_EBI1_D22 (PIO_PA22)
#define PIO_PA23       (1 << 23) /* Pin Controlled by PA23 */
#define PA23_MCI1_DB1 (PIO_PA23)
#define PA23_EBI1_D23 (PIO_PA23)
#define PIO_PA24       (1 << 24) /* Pin Controlled by PA24 */
#define PA24_MCI1_DB2 (PIO_PA24)
#define PA24_EBI1_D24 (PIO_PA24)
#define PIO_PA25       (1 << 25) /* Pin Controlled by PA25 */
#define PA25_MCI1_DB3 (PIO_PA25)
#define PA25_EBI1_D25 (PIO_PA25)
#define PIO_PA26       (1 << 26) /* Pin Controlled by PA26 */
#define PA26_TXD0     (PIO_PA26)
#define PA26_EBI1_D26 (PIO_PA26)
#define PIO_PA27       (1 << 27) /* Pin Controlled by PA27 */
#define PA27_RXD0     (PIO_PA27)
#define PA27_EBI1_D27 (PIO_PA27)
#define PIO_PA28       (1 << 28) /* Pin Controlled by PA28 */
#define PA28_RTS0     (PIO_PA28)
#define PA28_EBI1_D28 (PIO_PA28)
#define PIO_PA29       (1 << 29) /* Pin Controlled by PA29 */
#define PA29_CTS0     (PIO_PA29)
#define PA29_EBI1_D29 (PIO_PA29)
#define PIO_PA3        (1 << 3) /* Pin Controlled by PA3 */
#define PA3_MCI0_DA1 (PIO_PA3)
#define PA3_SPI0_NPCS1 (PIO_PA3)
#define PIO_PA30       (1 << 30) /* Pin Controlled by PA30 */
#define PA30_SCK0     (PIO_PA30)
#define PA30_EBI1_D30 (PIO_PA30)
#define PIO_PA31       (1 << 31) /* Pin Controlled by PA31 */
#define PA31_DMARQ0   (PIO_PA31)
#define PA31_EBI1_D31 (PIO_PA31)
#define PIO_PA4        (1 << 4) /* Pin Controlled by PA4 */
#define PA4_MCI0_DA2 (PIO_PA4)
#define PA4_SPI0_NPCS2A (PIO_PA4)
#define PIO_PA5        (1 << 5) /* Pin Controlled by PA5 */
#define PA5_MCI0_DA3 (PIO_PA5)
#define PA5_SPI0_NPCS0 (PIO_PA5)
#define PIO_PA6        (1 << 6) /* Pin Controlled by PA6 */
#define PA6_MCI1_CK  (PIO_PA6)
#define PA6_PCK2     (PIO_PA6)
#define PIO_PA7        (1 << 7) /* Pin Controlled by PA7 */
#define PA7_MCI1_CDA (PIO_PA7)
#define PIO_PA8        (1 << 8) /* Pin Controlled by PA8 */
#define PA8_MCI1_DA0 (PIO_PA8)
#define PIO_PA9        (1 << 9) /* Pin Controlled by PA9 */
#define PA9_MCI1_DA1 (PIO_PA9)
#define PIO_PB0        (1 << 0) /* Pin Controlled by PB0 */
#define PB0_AC97FS   (PIO_PB0)
#define PB0_TF0      (PIO_PB0)
#define PIO_PB1        (1 << 1) /* Pin Controlled by PB1 */
#define PB1_AC97CK   (PIO_PB1)
#define PB1_TK0      (PIO_PB1)
#define PIO_PB10       (1 << 10) /* Pin Controlled by PB10 */
#define PB10_RK1      (PIO_PB10)
#define PB10_PCK1     (PIO_PB10)
#define PIO_PB11       (1 << 11) /* Pin Controlled by PB11 */
#define PB11_RF1      (PIO_PB11)
#define PB11_SPI0_NPCS3B (PIO_PB11)
#define PIO_PB12       (1 << 12) /* Pin Controlled by PB12 */
#define PB12_SPI1_MISO (PIO_PB12)
#define PIO_PB13       (1 << 13) /* Pin Controlled by PB13 */
#define PB13_SPI1_MOSI (PIO_PB13)
#define PIO_PB14       (1 << 14) /* Pin Controlled by PB14 */
#define PB14_SPI1_SPCK (PIO_PB14)
#define PIO_PB15       (1 << 15) /* Pin Controlled by PB15 */
#define PB15_SPI1_NPCS0 (PIO_PB15)
#define PIO_PB16       (1 << 16) /* Pin Controlled by PB16 */
#define PB16_SPI1_NPCS1 (PIO_PB16)
#define PB16_PCK1     (PIO_PB16)
#define PIO_PB17       (1 << 17) /* Pin Controlled by PB17 */
#define PB17_SPI1_NPCS2B (PIO_PB17)
#define PB17_TIOA2    (PIO_PB17)
#define PIO_PB18       (1 << 18) /* Pin Controlled by PB18 */
#define PB18_SPI1_NPCS3B (PIO_PB18)
#define PB18_TIOB2    (PIO_PB18)
#define PIO_PB19       (1 << 19) /* Pin Controlled by PB19 */
#define PIO_PB2        (1 << 2) /* Pin Controlled by PB2 */
#define PB2_AC97TX   (PIO_PB2)
#define PB2_TD0      (PIO_PB2)
#define PIO_PB20       (1 << 20) /* Pin Controlled by PB20 */
#define PIO_PB21       (1 << 21) /* Pin Controlled by PB21 */
#define PIO_PB22       (1 << 22) /* Pin Controlled by PB22 */
#define PIO_PB23       (1 << 23) /* Pin Controlled by PB23 */
#define PIO_PB24       (1 << 24) /* Pin Controlled by PB24 */
#define PB24_UNCONNECTED_PB24_A (PIO_PB24)
#define PB24_DMARQ3   (PIO_PB24)
#define PIO_PB25       (1 << 25) /* Pin Controlled by PB25 */
#define PIO_PB26       (1 << 26) /* Pin Controlled by PB26 */
#define PIO_PB27       (1 << 27) /* Pin Controlled by PB27 */
#define PB27_UNCONNECTED_PB27_A (PIO_PB27)
#define PB27_PWM2     (PIO_PB27)
#define PIO_PB28       (1 << 28) /* Pin Controlled by PB28 */
#define PB28_UNCONNECTED_PB28_A (PIO_PB28)
#define PB28_TCLK0    (PIO_PB28)
#define PIO_PB29       (1 << 29) /* Pin Controlled by PB29 */
#define PB29_UNCONNECTED_PB29_A (PIO_PB29)
#define PB29_PWM3     (PIO_PB29)
#define PIO_PB3        (1 << 3) /* Pin Controlled by PB3 */
#define PB3_AC97RX   (PIO_PB3)
#define PB3_RD0      (PIO_PB3)
#define PIO_PB30       (1 << 30) /* Pin Controlled by PB30 */
#define PIO_PB31       (1 << 31) /* Pin Controlled by PB31 */
#define PIO_PB4        (1 << 4) /* Pin Controlled by PB4 */
#define PB4_TWD      (PIO_PB4)
#define PB4_RK0      (PIO_PB4)
#define PIO_PB5        (1 << 5) /* Pin Controlled by PB5 */
#define PB5_TWCK     (PIO_PB5)
#define PB5_RF0      (PIO_PB5)
#define PIO_PB6        (1 << 6) /* Pin Controlled by PB6 */
#define PB6_TF1      (PIO_PB6)
#define PB6_DMARQ1   (PIO_PB6)
#define PIO_PB7        (1 << 7) /* Pin Controlled by PB7 */
#define PB7_TK1      (PIO_PB7)
#define PB7_PWM0     (PIO_PB7)
#define PIO_PB8        (1 << 8) /* Pin Controlled by PB8 */
#define PB8_TD1      (PIO_PB8)
#define PB8_PWM1     (PIO_PB8)
#define PIO_PB9        (1 << 9) /* Pin Controlled by PB9 */
#define PB9_RD1      (PIO_PB9)
#define PB9_LCDCC    (PIO_PB9)
#define PIO_PC0        (1 << 0) /* Pin Controlled by PC0 */
#define PC0_LCDVSYNC (PIO_PC0)
#define PIO_PC1        (1 << 1) /* Pin Controlled by PC1 */
#define PC1_LCDHSYNC (PIO_PC1)
#define PIO_PC10       (1 << 10) /* Pin Controlled by PC10 */
#define PC10_LCDD6    (PIO_PC10)
#define PC10_LCDD11B  (PIO_PC10)
#define PIO_PC11       (1 << 11) /* Pin Controlled by PC11 */
#define PC11_LCDD7    (PIO_PC11)
#define PC11_LCDD12B  (PIO_PC11)
#define PIO_PC12       (1 << 12) /* Pin Controlled by PC12 */
#define PC12_LCDD8    (PIO_PC12)
#define PC12_LCDD13B  (PIO_PC12)
#define PIO_PC13       (1 << 13) /* Pin Controlled by PC13 */
#define PC13_LCDD9    (PIO_PC13)
#define PC13_LCDD14B  (PIO_PC13)
#define PIO_PC14       (1 << 14) /* Pin Controlled by PC14 */
#define PC14_LCDD10   (PIO_PC14)
#define PC14_LCDD15B  (PIO_PC14)
#define PIO_PC15       (1 << 15) /* Pin Controlled by PC15 */
#define PC15_LCDD11   (PIO_PC15)
#define PC15_LCDD19B  (PIO_PC15)
#define PIO_PC16       (1 << 16) /* Pin Controlled by PC16 */
#define PC16_LCDD12   (PIO_PC16)
#define PC16_LCDD20B  (PIO_PC16)
#define PIO_PC17       (1 << 17) /* Pin Controlled by PC17 */
#define PC17_LCDD13   (PIO_PC17)
#define PC17_LCDD21B  (PIO_PC17)
#define PIO_PC18       (1 << 18) /* Pin Controlled by PC18 */
#define PC18_LCDD14   (PIO_PC18)
#define PC18_LCDD22B  (PIO_PC18)
#define PIO_PC19       (1 << 19) /* Pin Controlled by PC19 */
#define PC19_LCDD15   (PIO_PC19)
#define PC19_LCDD23B  (PIO_PC19)
#define PIO_PC2        (1 << 2) /* Pin Controlled by PC2 */
#define PC2_LCDDOTCK (PIO_PC2)
#define PIO_PC20       (1 << 20) /* Pin Controlled by PC20 */
#define PC20_LCDD16   (PIO_PC20)
#define PC20_ETX2     (PIO_PC20)
#define PIO_PC21       (1 << 21) /* Pin Controlled by PC21 */
#define PC21_LCDD17   (PIO_PC21)
#define PC21_ETX3     (PIO_PC21)
#define PIO_PC22       (1 << 22) /* Pin Controlled by PC22 */
#define PC22_LCDD18   (PIO_PC22)
#define PC22_ERX2     (PIO_PC22)
#define PIO_PC23       (1 << 23) /* Pin Controlled by PC23 */
#define PC23_LCDD19   (PIO_PC23)
#define PC23_ERX3     (PIO_PC23)
#define PIO_PC24       (1 << 24) /* Pin Controlled by PC24 */
#define PC24_LCDD20   (PIO_PC24)
#define PC24_ETXER    (PIO_PC24)
#define PIO_PC25       (1 << 25) /* Pin Controlled by PC25 */
#define PC25_LCDD21   (PIO_PC25)
#define PC25_ERXDV    (PIO_PC25)
#define PIO_PC26       (1 << 26) /* Pin Controlled by PC26 */
#define PC26_LCDD22   (PIO_PC26)
#define PC26_ECOL     (PIO_PC26)
#define PIO_PC27       (1 << 27) /* Pin Controlled by PC27 */
#define PC27_LCDD23   (PIO_PC27)
#define PC27_ERXCK    (PIO_PC27)
#define PIO_PC28       (1 << 28) /* Pin Controlled by PC28 */
#define PC28_PWM0     (PIO_PC28)
#define PC28_TCLK1    (PIO_PC28)
#define PIO_PC29       (1 << 29) /* Pin Controlled by PC29 */
#define PC29_PCK0     (PIO_PC29)
#define PC29_PWM2     (PIO_PC29)
#define PIO_PC3        (1 << 3) /* Pin Controlled by PC3 */
#define PC3_LCDEN    (PIO_PC3)
#define PC3_PWM1     (PIO_PC3)
#define PIO_PC30       (1 << 30) /* Pin Controlled by PC30 */
#define PC30_DRXD     (PIO_PC30)
#define PIO_PC31       (1 << 31) /* Pin Controlled by PC31 */
#define PC31_DTXD     (PIO_PC31)
#define PIO_PC4        (1 << 4) /* Pin Controlled by PC4 */
#define PC4_LCDD0    (PIO_PC4)
#define PC4_LCDD3B   (PIO_PC4)
#define PIO_PC5        (1 << 5) /* Pin Controlled by PC5 */
#define PC5_LCDD1    (PIO_PC5)
#define PC5_LCDD4B   (PIO_PC5)
#define PIO_PC6        (1 << 6) /* Pin Controlled by PC6 */
#define PC6_LCDD2    (PIO_PC6)
#define PC6_LCDD5B   (PIO_PC6)
#define PIO_PC7        (1 << 7) /* Pin Controlled by PC7 */
#define PC7_LCDD3    (PIO_PC7)
#define PC7_LCDD6B   (PIO_PC7)
#define PIO_PC8        (1 << 8) /* Pin Controlled by PC8 */
#define PC8_LCDD4    (PIO_PC8)
#define PC8_LCDD7B   (PIO_PC8)
#define PIO_PC9        (1 << 9) /* Pin Controlled by PC9 */
#define PC9_LCDD5    (PIO_PC9)
#define PC9_LCDD10B  (PIO_PC9)
#define PIO_PD0        (1 << 0) /* Pin Controlled by PD0 */
#define PD0_TXD1     (PIO_PD0)
#define PD0_SPI0_NPCS2D (PIO_PD0)
#define PIO_PD1        (1 << 1) /* Pin Controlled by PD1 */
#define PD1_RXD1     (PIO_PD1)
#define PD1_SPI0_NPCS3D (PIO_PD1)
#define PIO_PD10       (1 << 10) /* Pin Controlled by PD10 */
#define PD10_UNCONNECTED_PD10_A (PIO_PD10)
#define PD10_SCK1     (PIO_PD10)
#define PIO_PD11       (1 << 11) /* Pin Controlled by PD11 */
#define PD11_EBI0_NCS2 (PIO_PD11)
#define PD11_TSYNC    (PIO_PD11)
#define PIO_PD12       (1 << 12) /* Pin Controlled by PD12 */
#define PD12_EBI0_A23 (PIO_PD12)
#define PD12_TCLK     (PIO_PD12)
#define PIO_PD13       (1 << 13) /* Pin Controlled by PD13 */
#define PD13_EBI0_A24 (PIO_PD13)
#define PD13_TPS0     (PIO_PD13)
#define PIO_PD14       (1 << 14) /* Pin Controlled by PD14 */
#define PD14_EBI0_A25_CFNRW (PIO_PD14)
#define PD14_TPS1     (PIO_PD14)
#define PIO_PD15       (1 << 15) /* Pin Controlled by PD15 */
#define PD15_EBI0_NCS3_NANDCS (PIO_PD15)
#define PD15_TPS2     (PIO_PD15)
#define PIO_PD16       (1 << 16) /* Pin Controlled by PD16 */
#define PD16_EBI0_D16 (PIO_PD16)
#define PD16_TPK0     (PIO_PD16)
#define PIO_PD17       (1 << 17) /* Pin Controlled by PD17 */
#define PD17_EBI0_D17 (PIO_PD17)
#define PD17_TPK1     (PIO_PD17)
#define PIO_PD18       (1 << 18) /* Pin Controlled by PD18 */
#define PD18_EBI0_D18 (PIO_PD18)
#define PD18_TPK2     (PIO_PD18)
#define PIO_PD19       (1 << 19) /* Pin Controlled by PD19 */
#define PD19_EBI0_D19 (PIO_PD19)
#define PD19_TPK3     (PIO_PD19)
#define PIO_PD2        (1 << 2) /* Pin Controlled by PD2 */
#define PD2_TXD2     (PIO_PD2)
#define PD2_SPI1_NPCS2D (PIO_PD2)
#define PIO_PD20       (1 << 20) /* Pin Controlled by PD20 */
#define PD20_EBI0_D20 (PIO_PD20)
#define PD20_TPK4     (PIO_PD20)
#define PIO_PD21       (1 << 21) /* Pin Controlled by PD21 */
#define PD21_EBI0_D21 (PIO_PD21)
#define PD21_TPK5     (PIO_PD21)
#define PIO_PD22       (1 << 22) /* Pin Controlled by PD22 */
#define PD22_EBI0_D22 (PIO_PD22)
#define PD22_TPK6     (PIO_PD22)
#define PIO_PD23       (1 << 23) /* Pin Controlled by PD23 */
#define PD23_EBI0_D23 (PIO_PD23)
#define PD23_TPK7     (PIO_PD23)
#define PIO_PD24       (1 << 24) /* Pin Controlled by PD24 */
#define PD24_EBI0_D24 (PIO_PD24)
#define PD24_TPK8     (PIO_PD24)
#define PIO_PD25       (1 << 25) /* Pin Controlled by PD25 */
#define PD25_EBI0_D25 (PIO_PD25)
#define PD25_TPK9     (PIO_PD25)
#define PIO_PD26       (1 << 26) /* Pin Controlled by PD26 */
#define PD26_EBI0_D26 (PIO_PD26)
#define PD26_TPK10    (PIO_PD26)
#define PIO_PD27       (1 << 27) /* Pin Controlled by PD27 */
#define PD27_EBI0_D27 (PIO_PD27)
#define PD27_TPK11    (PIO_PD27)
#define PIO_PD28       (1 << 28) /* Pin Controlled by PD28 */
#define PD28_EBI0_D28 (PIO_PD28)
#define PD28_TPK12    (PIO_PD28)
#define PIO_PD29       (1 << 29) /* Pin Controlled by PD29 */
#define PD29_EBI0_D29 (PIO_PD29)
#define PD29_TPK13    (PIO_PD29)
#define PIO_PD3        (1 << 3) /* Pin Controlled by PD3 */
#define PD3_RXD2     (PIO_PD3)
#define PD3_SPI1_NPCS3D (PIO_PD3)
#define PIO_PD30       (1 << 30) /* Pin Controlled by PD30 */
#define PD30_EBI0_D30 (PIO_PD30)
#define PD30_TPK14    (PIO_PD30)
#define PIO_PD31       (1 << 31) /* Pin Controlled by PD31 */
#define PD31_EBI0_D31 (PIO_PD31)
#define PD31_TPK15    (PIO_PD31)
#define PIO_PD4        (1 << 4) /* Pin Controlled by PD4 */
#define PD4_FIQ      (PIO_PD4)
#define PD4_DMARQ2   (PIO_PD4)
#define PIO_PD5        (1 << 5) /* Pin Controlled by PD5 */
#define PD5_EBI0_NWAIT (PIO_PD5)
#define PD5_RTS2     (PIO_PD5)
#define PIO_PD6        (1 << 6) /* Pin Controlled by PD6 */
#define PD6_EBI0_NCS4_CFCS0 (PIO_PD6)
#define PD6_CTS2     (PIO_PD6)
#define PIO_PD7        (1 << 7) /* Pin Controlled by PD7 */
#define PD7_EBI0_NCS5_CFCS1 (PIO_PD7)
#define PD7_RTS1     (PIO_PD7)
#define PIO_PD8        (1 << 8) /* Pin Controlled by PD8 */
#define PD8_EBI0_CFCE1 (PIO_PD8)
#define PD8_CTS1     (PIO_PD8)
#define PIO_PD9        (1 << 9) /* Pin Controlled by PD9 */
#define PD9_EBI0_CFCE2 (PIO_PD9)
#define PD9_SCK2     (PIO_PD9)
#define PIO_PE0        (1 << 0) /* Pin Controlled by PE0 */
#define PE0_ISI_D0   (PIO_PE0)
#define PIO_PE1        (1 << 1) /* Pin Controlled by PE1 */
#define PE1_ISI_D1   (PIO_PE1)
#define PIO_PE10       (1 << 10) /* Pin Controlled by PE10 */
#define PE10_ISI_VSYNC (PIO_PE10)
#define PE10_PWM3     (PIO_PE10)
#define PIO_PE11       (1 << 11) /* Pin Controlled by PE11 */
#define PE11_ISI_MCK  (PIO_PE11)
#define PE11_PCK3     (PIO_PE11)
#define PIO_PE12       (1 << 12) /* Pin Controlled by PE12 */
#define PE12_KBDR0    (PIO_PE12)
#define PE12_ISI_D8   (PIO_PE12)
#define PIO_PE13       (1 << 13) /* Pin Controlled by PE13 */
#define PE13_KBDR1    (PIO_PE13)
#define PE13_ISI_D9   (PIO_PE13)
#define PIO_PE14       (1 << 14) /* Pin Controlled by PE14 */
#define PE14_KBDR2    (PIO_PE14)
#define PE14_ISI_D10  (PIO_PE14)
#define PIO_PE15       (1 << 15) /* Pin Controlled by PE15 */
#define PE15_KBDR3    (PIO_PE15)
#define PE15_ISI_D11  (PIO_PE15)
#define PIO_PE16       (1 << 16) /* Pin Controlled by PE16 */
#define PE16_KBDR4    (PIO_PE16)
#define PIO_PE17       (1 << 17) /* Pin Controlled by PE17 */
#define PE17_KBDC0    (PIO_PE17)
#define PIO_PE18       (1 << 18) /* Pin Controlled by PE18 */
#define PE18_KBDC1    (PIO_PE18)
#define PE18_TIOA0    (PIO_PE18)
#define PIO_PE19       (1 << 19) /* Pin Controlled by PE19 */
#define PE19_KBDC2    (PIO_PE19)
#define PE19_TIOB0    (PIO_PE19)
#define PIO_PE2        (1 << 2) /* Pin Controlled by PE2 */
#define PE2_ISI_D2   (PIO_PE2)
#define PIO_PE20       (1 << 20) /* Pin Controlled by PE20 */
#define PE20_KBDC3    (PIO_PE20)
#define PE20_EBI1_NWAIT (PIO_PE20)
#define PIO_PE21       (1 << 21) /* Pin Controlled by PE21 */
#define PE21_ETXCK    (PIO_PE21)
#define PE21_EBI1_NANDWE (PIO_PE21)
#define PIO_PE22       (1 << 22) /* Pin Controlled by PE22 */
#define PE22_ECRS     (PIO_PE22)
#define PE22_EBI1_NCS2_NANDCS (PIO_PE22)
#define PIO_PE23       (1 << 23) /* Pin Controlled by PE23 */
#define PE23_ETX0     (PIO_PE23)
#define PE23_EBI1_NANDOE (PIO_PE23)
#define PIO_PE24       (1 << 24) /* Pin Controlled by PE24 */
#define PE24_ETX1     (PIO_PE24)
#define PE24_EBI1_NWR3_NBS3 (PIO_PE24)
#define PIO_PE25       (1 << 25) /* Pin Controlled by PE25 */
#define PE25_ERX0     (PIO_PE25)
#define PE25_EBI1_NCS1_SDCS (PIO_PE25)
#define PIO_PE26       (1 << 26) /* Pin Controlled by PE26 */
#define PE26_ERX1     (PIO_PE26)
#define PIO_PE27       (1 << 27) /* Pin Controlled by PE27 */
#define PE27_ERXER    (PIO_PE27)
#define PE27_EBI1_SDCKE (PIO_PE27)
#define PIO_PE28       (1 << 28) /* Pin Controlled by PE28 */
#define PE28_ETXEN    (PIO_PE28)
#define PE28_EBI1_RAS (PIO_PE28)
#define PIO_PE29       (1 << 29) /* Pin Controlled by PE29 */
#define PE29_EMDC     (PIO_PE29)
#define PE29_EBI1_CAS (PIO_PE29)
#define PIO_PE3        (1 << 3) /* Pin Controlled by PE3 */
#define PE3_ISI_D3   (PIO_PE3)
#define PIO_PE30       (1 << 30) /* Pin Controlled by PE30 */
#define PE30_EMDIO    (PIO_PE30)
#define PE30_EBI1_SDWE (PIO_PE30)
#define PIO_PE31       (1 << 31) /* Pin Controlled by PE31 */
#define PE31_EF100    (PIO_PE31)
#define PE31_EBI1_SDA10 (PIO_PE31)
#define PIO_PE4        (1 << 4) /* Pin Controlled by PE4 */
#define PE4_ISI_D4   (PIO_PE4)
#define PIO_PE5        (1 << 5) /* Pin Controlled by PE5 */
#define PE5_ISI_D5   (PIO_PE5)
#define PIO_PE6        (1 << 6) /* Pin Controlled by PE6 */
#define PE6_ISI_D6   (PIO_PE6)
#define PIO_PE7        (1 << 7) /* Pin Controlled by PE7 */
#define PE7_ISI_D7   (PIO_PE7)
#define PIO_PE8        (1 << 8) /* Pin Controlled by PE8 */
#define PE8_ISI_PCK  (PIO_PE8)
#define PE8_TIOA1    (PIO_PE8)
#define PIO_PE9        (1 << 9) /* Pin Controlled by PE9 */
#define PE9_ISI_HSYNC (PIO_PE9)
#define PE9_TIOB1    (PIO_PE9)

/* Peripheral ID Definitions for AT91SAM9263 */
#define AT91_ID_FIQ    ( 0) /* Advanced Interrupt Controller (FIQ) */
#define AT91_ID_SYS    ( 1) /* System Controller */
#define AT91_ID_PIOA   ( 2) /* Parallel IO Controller A */
#define AT91_ID_PIOB   ( 3) /* Parallel IO Controller B */
#define AT91_ID_PIOCDE ( 4) /* Parallel IO Controller C, 
							   Parallel IO Controller D, 
							   Parallel IO Controller E */
#define AT91_ID_US0    ( 7) /* USART 0 */
#define AT91_ID_US1    ( 8) /* USART 1 */
#define AT91_ID_US2    ( 9) /* USART 2 */
#define AT91_ID_MCI0   (10) /* Multimedia Card Interface 0 */
#define AT91_ID_MCI1   (11) /* Multimedia Card Interface 1 */
#define AT91_ID_CAN    (12) /* CAN Controller */
#define AT91_ID_TWI    (13) /* Two-Wire Interface */
#define AT91_ID_SPI0   (14) /* Serial Peripheral Interface 0 */
#define AT91_ID_SPI1   (15) /* Serial Peripheral Interface 1 */
#define AT91_ID_SSC0   (16) /* Serial Synchronous Controller 0 */
#define AT91_ID_SSC1   (17) /* Serial Synchronous Controller 1 */
#define AT91_ID_AC97C  (18) /* AC97 Controller */
#define AT91_ID_TC012  (19) /* Timer Counter 0, Timer Counter 1, Timer Counter 2 */
#define AT91_ID_PWMC   (20) /* PWM Controller */
#define AT91_ID_EMAC   (21) /* Ethernet Mac */
#define AT91_ID_UDP    (24) /* USB Device Port */
#define AT91_ID_ISI    (25) /* Image Sensor Interface */
#define AT91_ID_LCDC   (26) /* LCD Controller */
#define AT91_ID_DMA    (27) /* DMA Controller */
#define AT91_ID_UHP    (29) /* USB Host Port */
#define AT91_ID_IRQ0   (30) /* Advanced Interrupt Controller (IRQ0) */
#define AT91_ID_IRQ1   (31) /* Advanced Interrupt Controller (IRQ1) */
#define AT91_ALL_INT   (0xef3fff9f) /* ALL VALID INTERRUPTS */

/* Base Address Definitions for AT91SAM9263 */
#define AT91_BASE_SYS       (0xffffe000) /* (SYS) Base Address */
#define AT91_BASE_EBI0      (0xffffe200) /* (EBI0) Base Address */
#define AT91_BASE_SDRAMC0   (0xffffe200) /* (SDRAMC0) Base Address */
#define AT91_BASE_SMC0      (0xffffe400) /* (SMC0) Base Address */
#define AT91_BASE_EBI1      (0xffffe800) /* (EBI1) Base Address */
#define AT91_BASE_SDRAMC1   (0xffffe800) /* (SDRAMC1) Base Address */
#define AT91_BASE_SMC1      (0xffffea00) /* (SMC1) Base Address */
#define AT91_BASE_MATRIX    (0xffffec00) /* (MATRIX) Base Address */
#define AT91_BASE_CCFG      (0xffffed10) /* (CCFG) Base Address */
#define AT91_BASE_PDC_DBGU  (0xffffef00) /* (PDC_DBGU) Base Address */
#define AT91_BASE_DBGU      (0xffffee00) /* (DBGU) Base Address */
#define AT91_BASE_AIC       (0xfffff000) /* (AIC) Base Address */
#define AT91_BASE_PIOA      (0xfffff200) /* (PIOA) Base Address */
#define AT91_BASE_PIOB      (0xfffff400) /* (PIOB) Base Address */
#define AT91_BASE_PIOC      (0xfffff600) /* (PIOC) Base Address */
#define AT91_BASE_PIOD      (0xfffff800) /* (PIOD) Base Address */
#define AT91_BASE_PIOE      (0xfffffa00) /* (PIOE) Base Address */
#define AT91_BASE_CKGR      (0xfffffc20) /* (CKGR) Base Address */
#define AT91_BASE_PMC       (0xfffffc00) /* (PMC) Base Address */
#define AT91_BASE_RSTC      (0xfffffd00) /* (RSTC) Base Address */
#define AT91_BASE_SHDWC     (0xfffffd10) /* (SHDWC) Base Address */
#define AT91_BASE_RTTC0     (0xfffffd20) /* (RTTC0) Base Address */
#define AT91_BASE_RTTC1     (0xfffffd50) /* (RTTC1) Base Address */
#define AT91_BASE_PITC      (0xfffffd30) /* (PITC) Base Address */
#define AT91_BASE_WDTC      (0xfffffd40) /* (WDTC) Base Address */
#define AT91_BASE_TC0       (0xfff7c000) /* (TC0) Base Address */
#define AT91_BASE_TC1       (0xfff7c040) /* (TC1) Base Address */
#define AT91_BASE_TC2       (0xfff7c080) /* (TC2) Base Address */
#define AT91_BASE_TCB0      (0xfff7c000) /* (TCB0) Base Address */
#define AT91_BASE_TCB1      (0xfff7c040) /* (TCB1) Base Address */
#define AT91_BASE_TCB2      (0xfff7c080) /* (TCB2) Base Address */
#define AT91_BASE_PDC_MCI0  (0xfff80100) /* (PDC_MCI0) Base Address */
#define AT91_BASE_MCI0      (0xfff80000) /* (MCI0) Base Address */
#define AT91_BASE_PDC_MCI1  (0xfff84100) /* (PDC_MCI1) Base Address */
#define AT91_BASE_MCI1      (0xfff84000) /* (MCI1) Base Address */
#define AT91_BASE_TWI       (0xfff88000) /* (TWI) Base Address */
#define AT91_BASE_PDC_US0   (0xfff8c100) /* (PDC_US0) Base Address */
#define AT91_BASE_US0       (0xfff8c000) /* (US0) Base Address */
#define AT91_BASE_PDC_US1   (0xfff90100) /* (PDC_US1) Base Address */
#define AT91_BASE_US1       (0xfff90000) /* (US1) Base Address */
#define AT91_BASE_PDC_US2   (0xfff94100) /* (PDC_US2) Base Address */
#define AT91_BASE_US2       (0xfff94000) /* (US2) Base Address */
#define AT91_BASE_PDC_SSC0  (0xfff98100) /* (PDC_SSC0) Base Address */
#define AT91_BASE_SSC0      (0xfff98000) /* (SSC0) Base Address */
#define AT91_BASE_PDC_SSC1  (0xfff9c100) /* (PDC_SSC1) Base Address */
#define AT91_BASE_SSC1      (0xfff9c000) /* (SSC1) Base Address */
#define AT91_BASE_PDC_AC97C (0xfffa0100) /* (PDC_AC97C) Base Address */
#define AT91_BASE_AC97C     (0xfffa0000) /* (AC97C) Base Address */
#define AT91_BASE_PDC_SPI0  (0xfffa4100) /* (PDC_SPI0) Base Address */
#define AT91_BASE_SPI0      (0xfffa4000) /* (SPI0) Base Address */
#define AT91_BASE_PDC_SPI1  (0xfffa8100) /* (PDC_SPI1) Base Address */
#define AT91_BASE_SPI1      (0xfffa8000) /* (SPI1) Base Address */
#define AT91_BASE_CAN_MB0   (0xfffac200) /* (CAN_MB0) Base Address */
#define AT91_BASE_CAN_MB1   (0xfffac220) /* (CAN_MB1) Base Address */
#define AT91_BASE_CAN_MB2   (0xfffac240) /* (CAN_MB2) Base Address */
#define AT91_BASE_CAN_MB3   (0xfffac260) /* (CAN_MB3) Base Address */
#define AT91_BASE_CAN_MB4   (0xfffac280) /* (CAN_MB4) Base Address */
#define AT91_BASE_CAN_MB5   (0xfffac2a0) /* (CAN_MB5) Base Address */
#define AT91_BASE_CAN_MB6   (0xfffac2c0) /* (CAN_MB6) Base Address */
#define AT91_BASE_CAN_MB7   (0xfffac2e0) /* (CAN_MB7) Base Address */
#define AT91_BASE_CAN_MB8   (0xfffac300) /* (CAN_MB8) Base Address */
#define AT91_BASE_CAN_MB9   (0xfffac320) /* (CAN_MB9) Base Address */
#define AT91_BASE_CAN_MB10  (0xfffac340) /* (CAN_MB10) Base Address */
#define AT91_BASE_CAN_MB11  (0xfffac360) /* (CAN_MB11) Base Address */
#define AT91_BASE_CAN_MB12  (0xfffac380) /* (CAN_MB12) Base Address */
#define AT91_BASE_CAN_MB13  (0xfffac3a0) /* (CAN_MB13) Base Address */
#define AT91_BASE_CAN_MB14  (0xfffac3c0) /* (CAN_MB14) Base Address */
#define AT91_BASE_CAN_MB15  (0xfffac3e0) /* (CAN_MB15) Base Address */
#define AT91_BASE_CAN       (0xfffac000) /* (CAN) Base Address */
#define AT91_BASE_PWMC_CH0  (0xfffb8200) /* (PWMC_CH0) Base Address */
#define AT91_BASE_PWMC_CH1  (0xfffb8220) /* (PWMC_CH1) Base Address */
#define AT91_BASE_PWMC_CH2  (0xfffb8240) /* (PWMC_CH2) Base Address */
#define AT91_BASE_PWMC_CH3  (0xfffb8260) /* (PWMC_CH3) Base Address */
#define AT91_BASE_PWMC      (0xfffb8000) /* (PWMC) Base Address */
#define AT91_BASE_MACB      (0xfffbc000) /* (MACB) Base Address */
#define AT91_BASE_LCDC      (0x00700000) /* (LCDC) Base Address */
#define AT91_BASE_DMA       (0x00800000) /* (DMA) Base Address */
#define AT91_BASE_UDP       (0xfff78000) /* (UDP) Base Address */
#define AT91_BASE_UHP       (0x00a00000) /* (UHP) Base Address */
#define AT91_BASE_TBOX      (0x70000000) /* (TBOX) Base Address */
#define AT91_BASE_HECC0     (0xffffe000) /* (HECC0) Base Address */
#define AT91_BASE_HECC1     (0xffffe600) /* (HECC1) Base Address */
#define AT91_BASE_ISI       (0xfffc4000) /* (ISI) Base Address */

/* Memory Mapping Definitions for AT91SAM9263 */
/* ITCM */
/* Maximum ITCM Area base address */
#define AT91_ITCM (0x00100000)
/* Maximum ITCM Area size in byte (64 Kbytes) */
#define AT91_ITCM_SIZE (0x00010000)
/* DTCM */
/* Maximum DTCM Area base address */
#define AT91_DTCM (0x00200000)
/* Maximum DTCM Area size in byte (64 Kbytes) */
#define AT91_DTCM_SIZE (0x00010000)
/* IRAM */
/* Maximum Internal SRAM base address */
#define AT91_IRAM (0x00300000)
/* Maximum Internal SRAM size in byte (80 Kbytes) */
#define AT91_IRAM_SIZE (0x00014000)
/* IRAM_MIN */
/* Minimum Internal RAM base address */
#define AT91_IRAM_MIN (0x00300000)
/* Minimum Internal RAM size in byte (16 Kbytes) */
#define AT91_IRAM_MIN_SIZE (0x00004000)
/* IROM */
/* Internal ROM base address */
#define AT91_IROM (0x00400000)
/* Internal ROM size in byte (128 Kbytes) */
#define AT91_IROM_SIZE (0x00020000)
/* IRAM2 */
/* IRAM2 base address */
#define AT91_IRAM2 (0x00500000)
/* IRAM2 size in byte (16 Kbytes) */
#define AT91_IRAM2_SIZE (0x00004000)
/* EBI0_CS0 */
/* EBI0 Chip Select 0 base address */
#define AT91_EBI0_CS0 (0x10000000)
/* EBI0 Chip Select 0 size in byte (262144 Kbytes) */
#define AT91_EBI0_CS0_SIZE (0x10000000)
/* EBI0_CS1 */
/* EBI0 Chip Select 1 base address */
#define AT91_EBI0_CS1 (0x20000000)
/* EBI0 Chip Select 1 size in byte (262144 Kbytes) */
#define AT91_EBI0_CS1_SIZE (0x10000000)
/* EBI0_SDRAM */
/* SDRAM on EBI0 Chip Select 1 base address */
#define AT91_EBI0_SDRAM (0x20000000)
/* SDRAM on EBI0 Chip Select 1 size in byte (262144 Kbytes) */
#define AT91_EBI0_SDRAM_SIZE (0x10000000)
/* EBI0_SDRAM_16BIT */
/* SDRAM on EBI0 Chip Select 1 base address */
#define AT91_EBI0_SDRAM_16BIT (0x20000000)
/* SDRAM on EBI0 Chip Select 1 size in byte (32768 Kbytes) */
#define AT91_EBI0_SDRAM_16BIT_SIZE (0x02000000)
/* EBI0_SDRAM_32BIT */
/* SDRAM on EBI0 Chip Select 1 base address */
#define AT91_EBI0_SDRAM_32BIT (0x20000000)
/* SDRAM on EBI0 Chip Select 1 size in byte (65536 Kbytes) */
#define AT91_EBI0_SDRAM_32BIT_SIZE (0x04000000)
/* EBI0_CS2 */
/* EBI0 Chip Select 2 base address */
#define AT91_EBI0_CS2 (0x30000000)
/* EBI0 Chip Select 2 size in byte (262144 Kbytes) */
#define AT91_EBI0_CS2_SIZE (0x10000000)
/* EBI0_CS3 */
/* EBI0 Chip Select 3 base address */
#define AT91_EBI0_CS3 (0x40000000)
/* EBI0 Chip Select 3 size in byte (262144 Kbytes) */
#define AT91_EBI0_CS3_SIZE (0x10000000)
/* EBI0_SM */
/* SmartMedia on EBI0 Chip Select 3 base address */
#define AT91_EBI0_SM (0x40000000)
/* SmartMedia on EBI0 Chip Select 3 size in byte (262144 Kbytes) */
#define AT91_EBI0_SM_SIZE (0x10000000)
/* EBI0_CS4 */
/* EBI0 Chip Select 4 base address */
#define AT91_EBI0_CS4 (0x50000000)
/* EBI0 Chip Select 4 size in byte (262144 Kbytes) */
#define AT91_EBI0_CS4_SIZE (0x10000000)
/* EBI0_CF0 */
/* CompactFlash 0 on EBI0 Chip Select 4 base address */
#define AT91_EBI0_CF0 (0x50000000)
/* CompactFlash 0 on EBI0 Chip Select 4 size in byte (262144 Kbytes) */
#define AT91_EBI0_CF0_SIZE (0x10000000)
/* EBI0_CS5 */
/* EBI0 Chip Select 5 base address */
#define AT91_EBI0_CS5 (0x60000000)
/* EBI0 Chip Select 5 size in byte (262144 Kbytes) */
#define AT91_EBI0_CS5_SIZE (0x10000000)
/* EBI0_CF1 */
/* CompactFlash 1 on EBI0Chip Select 5 base address */
#define AT91_EBI0_CF1 (0x60000000)
/* CompactFlash 1 on EBI0Chip Select 5 size in byte (262144 Kbytes) */
#define AT91_EBI0_CF1_SIZE (0x10000000)
/* EBI1_CS0 */
/* EBI1 Chip Select 0 base address */
#define AT91_EBI1_CS0 (0x70000000)
/* EBI1 Chip Select 0 size in byte (262144 Kbytes) */
#define AT91_EBI1_CS0_SIZE (0x10000000)
/* EBI1_CS1 */
/* EBI1 Chip Select 1 base address */
#define AT91_EBI1_CS1 (0x80000000)
/* EBI1 Chip Select 1 size in byte (262144 Kbytes) */
#define AT91_EBI1_CS1_SIZE (0x10000000)
/* EBI1_SDRAM_16BIT */
/* SDRAM on EBI1 Chip Select 1 base address */
#define AT91_EBI1_SDRAM_16BIT (0x80000000)
/* SDRAM on EBI1 Chip Select 1 size in byte (32768 Kbytes) */
#define AT91_EBI1_SDRAM_16BIT_SIZE (0x02000000)
/* EBI1_SDRAM_32BIT */
/* SDRAM on EBI1 Chip Select 1 base address */
#define AT91_EBI1_SDRAM_32BIT (0x80000000)
/* SDRAM on EBI1 Chip Select 1 size in byte (65536 Kbytes) */
#define AT91_EBI1_SDRAM_32BIT_SIZE (0x04000000)
/* EBI1_CS2 */
/* EBI1 Chip Select 2 base address */
#define AT91_EBI1_CS2 (0x90000000)
/* EBI1 Chip Select 2 size in byte (262144 Kbytes) */
#define AT91_EBI1_CS2_SIZE (0x10000000)
/* EBI1_SM */
/* SmartMedia on EBI1 Chip Select 2 base address */
#define AT91_EBI1_SM (0x90000000)
/* SmartMedia on EBI1 Chip Select 2 size in byte (262144 Kbytes) */
#define AT91_EBI1_SM_SIZE (0x10000000)

#ifndef __ASSEMBLER__

struct at91_sysc {
	uint32_t res0[2560];
	struct at91_ecc ecc;
	struct at91_sdramc sdramc;
	struct at91_smc smc;
	struct at91_matrix matrix;
	struct at91_ccfg ccfg;
	struct at91_aic aic;
	struct at91_dbgu dbgu;
	struct at91_pio pioa;
	struct at91_pio piob;
	struct at91_pio pioc;
	struct at91_pio piod;
	struct at91_pmc pmc;
	struct at91_rstc rstc;
	struct at91_shdwc shdwc;
	struct at91_rttc rttc;
	struct at91_pitc pitc;
	struct at91_wdtc wdtc;
/*	struct at91_gpbr gpbr; */
};

/*
#define AT91_PIO(N) ((struct at91_pio *)((AT91_BASE_PIOA + (N) * \
										 (AT91_BASE_PIOB - AT91_BASE_PIOA))
*/
#define AT91_PMC ((struct at91_pmc *)AT91_BASE_PMC)
#define AT91_CKGR ((struct at91_ckgr *)AT91_BASE_CKGR)

struct at91sam9260_peripheral {
	uint32_t res0[67010560];
	struct at91_tcb tcb1;
	struct at91_udp udp;
	struct at91_mci mci;
	struct at91_twi twi;
	struct at91_us us0;
	struct at91_us us1;
	struct at91_us us2;
	struct at91_ssc ssc;
	struct at91_isi isi;
	struct at91_emac emac;
	struct at91_spi spi0;
	struct at91_spi spi1;
	struct at91_us us3;
	struct at91_us us4;
	struct at91_us us5;
	struct at91_tcb tcb2;
	struct at91_adc adc;
	uint32_t res1[24576];
	struct at91_sysc sysc;
};

extern struct at91sam9263_peripheral at91sam9263;

#define AT91_SYS ((struct at91_sys *)AT91_BASE_SYS)
#define AT91_EBI0 ((struct at91_ebi *)AT91_BASE_EBI0)
#define AT91_SDRAMC0 ((struct at91_sdramc *)AT91_BASE_SDRAMC0)
#define AT91_SMC0 ((struct at91_smc0 *)AT91_BASE_SMC0)
#define AT91_EBI1 ((struct at91_ebi1 *)AT91_BASE_EBI1)
#define AT91_SDRAMC1 ((struct at91_sdramc1 *)AT91_BASE_SDRAMC1)
#define AT91_SMC1 ((struct at91_smc1 *)AT91_BASE_SMC1)
#define AT91_MATRIX ((struct at91_matrix *)AT91_BASE_MATRIX)
#define AT91_CCFG ((struct at91_ccfg *)AT91_BASE_CCFG)
#define AT91_PDC_DBGU ((struct at91_pdc_dbgu *)AT91_BASE_PDC_DBGU)
#define AT91_DBGU ((struct at91_dbgu *)AT91_BASE_DBGU)
#define AT91_AIC ((struct at91_aic *)AT91_BASE_AIC)
#define AT91_PIOA ((struct at91_pio *)AT91_BASE_PIOA)
#define AT91_PIOB ((struct at91_pio *)AT91_BASE_PIOB)
#define AT91_PIOC ((struct at91_pio *)AT91_BASE_PIOC)
#define AT91_PIOD ((struct at91_pio *)AT91_BASE_PIOD)
#define AT91_PIOE ((struct at91_pio *)AT91_BASE_PIOE)
#define AT91_CKGR ((struct at91_ckgr *)AT91_BASE_CKGR)
#define AT91_PMC ((struct at91_pmc *)AT91_BASE_PMC)
#define AT91_RSTC ((struct at91_rstc *)AT91_BASE_RSTC)
#define AT91_SHDWC ((struct at91_shdwc *)AT91_BASE_SHDWC)
#define AT91_RTTC0 ((struct at91_rttc *)AT91_BASE_RTTC0)
#define AT91_RTTC1 ((struct at91_rttc *)AT91_BASE_RTTC1)
#define AT91_PITC ((struct at91_pitc *)AT91_BASE_PITC)
#define AT91_WDTC ((struct at91_wdtc *)AT91_BASE_WDTC)
#define AT91_TC0 ((struct at91_tc *)AT91_BASE_TC0)
#define AT91_TC1 ((struct at91_tc *)AT91_BASE_TC1)
#define AT91_TC2 ((struct at91_tc *)AT91_BASE_TC2)
#define AT91_TCB0 ((struct at91_tcb *)AT91_BASE_TCB0)
#define AT91_TCB1 ((struct at91_tcb *)AT91_BASE_TCB1)
#define AT91_TCB2 ((struct at91_tcb *)AT91_BASE_TCB2)
#define AT91_PDC_MCI0 ((struct at91_pdc *)AT91_BASE_PDC_MCI0)
#define AT91_MCI0 ((struct at91_mci *)AT91_BASE_MCI0)
#define AT91_PDC_MCI1 ((struct at91_pdc *)AT91_BASE_PDC_MCI1)
#define AT91_MCI1 ((struct at91_mci *)AT91_BASE_MCI1)
#define AT91_TWI ((struct at91_twi *)AT91_BASE_TWI)
#define AT91_PDC_US0 ((struct at91_pdc *)AT91_BASE_PDC_US0)
#define AT91_US0 ((struct at91_us *)AT91_BASE_US0)
#define AT91_PDC_US1 ((struct at91_pdc *)AT91_BASE_PDC_US1)
#define AT91_US1 ((struct at91_us *)AT91_BASE_US1)
#define AT91_PDC_US2 ((struct at91_pdc *)AT91_BASE_PDC_US2)
#define AT91_US2 ((struct at91_us *)AT91_BASE_US2)
#define AT91_PDC_SSC0 ((struct at91_pdc *)AT91_BASE_PDC_SSC0)
#define AT91_SSC0 ((struct at91_ssc *)AT91_BASE_SSC0)
#define AT91_PDC_SSC1 ((struct at91_pdc *)AT91_BASE_PDC_SSC1)
#define AT91_SSC1 ((struct at91_ssc *)AT91_BASE_SSC1)
#define AT91_PDC_AC97C ((struct at91_pdc *)AT91_BASE_PDC_AC97C)
#define AT91_AC97C ((struct at91_ac97c *)AT91_BASE_AC97C)
#define AT91_PDC_SPI0 ((struct at91_pdc *)AT91_BASE_PDC_SPI0)
#define AT91_SPI0 ((struct at91_spi *)AT91_BASE_SPI0)
#define AT91_PDC_SPI1 ((struct at91_pdc *)AT91_BASE_PDC_SPI1)
#define AT91_SPI1 ((struct at91_spi *)AT91_BASE_SPI1)
#define AT91_CAN_MB0 ((struct at91_can_mb *)AT91_BASE_CAN_MB0)
#define AT91_CAN_MB1 ((struct at91_can_mb *)AT91_BASE_CAN_MB1)
#define AT91_CAN_MB2 ((struct at91_can_mb *)AT91_BASE_CAN_MB2)
#define AT91_CAN_MB3 ((struct at91_can_mb *)AT91_BASE_CAN_MB3)
#define AT91_CAN_MB4 ((struct at91_can_mb *)AT91_BASE_CAN_MB4)
#define AT91_CAN_MB5 ((struct at91_can_mb *)AT91_BASE_CAN_MB5)
#define AT91_CAN_MB6 ((struct at91_can_mb *)AT91_BASE_CAN_MB6)
#define AT91_CAN_MB7 ((struct at91_can_mb *)AT91_BASE_CAN_MB7)
#define AT91_CAN_MB8 ((struct at91_can_mb *)AT91_BASE_CAN_MB8)
#define AT91_CAN_MB9 ((struct at91_can_mb *)AT91_BASE_CAN_MB9)
#define AT91_CAN_MB10 ((struct at91_can_mb *)AT91_BASE_CAN_MB10)
#define AT91_CAN_MB11 ((struct at91_can_mb *)AT91_BASE_CAN_MB11)
#define AT91_CAN_MB12 ((struct at91_can_mb *)AT91_BASE_CAN_MB12)
#define AT91_CAN_MB13 ((struct at91_can_mb *)AT91_BASE_CAN_MB13)
#define AT91_CAN_MB14 ((struct at91_can_mb *)AT91_BASE_CAN_MB14)
#define AT91_CAN_MB15 ((struct at91_can_mb *)AT91_BASE_CAN_MB15)
#define AT91_CAN ((struct at91_can *)AT91_BASE_CAN)
#define AT91_PWMC_CH0 ((struct at91_pwmc_ch *)AT91_BASE_PWMC_CH0)
#define AT91_PWMC_CH1 ((struct at91_pwmc_ch *)AT91_BASE_PWMC_CH1)
#define AT91_PWMC_CH2 ((struct at91_pwmc_ch *)AT91_BASE_PWMC_CH2)
#define AT91_PWMC_CH3 ((struct at91_pwmc_ch *)AT91_BASE_PWMC_CH3)
#define AT91_PWMC ((struct at91_pwm *)AT91_BASE_PWMC)
//#define AT91_MACB ((struct at91_macb *)AT91_BASE_MACB)
#define AT91_EMAC ((struct at91_emac *)AT91_BASE_MACB)

#define AT91_LCDC ((struct at91_lcdc *)AT91_BASE_LCDC)
#define AT91_DMA ((struct at91_dma *)AT91_BASE_DMA)
#define AT91_UDP ((struct at91_udp *)AT91_BASE_UDP)
#define AT91_UHP ((struct at91_uhp *)AT91_BASE_UHP)
#define AT91_TBOX ((struct at91_tbox *)AT91_BASE_TBOX)
#define AT91_HECC0 ((struct at91_hecc *)AT91_BASE_HECC0)
#define AT91_HECC1 ((struct at91_hecc *)AT91_BASE_HECC1)
#define AT91_ISI ((struct at91_isi *)AT91_BASE_ISI)
#endif /* __ASSEMBLER__ */

#endif /* __AT91SAM9263_H__ */

