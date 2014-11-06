/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the libstm32f.
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
 * @file stm32f-dac.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_DAC_H__
#define __STM32F_DAC_H__

/*-------------------------------------------------------------------------
  DAC
  -------------------------------------------------------------------------*/

/* DAC control register (DAC_CR) */
#define STM32F_DAC_CR 0x00

/* [31..30] Reserved. */

/* Bits 29 - DAC channel2 DMA underrun interrupt enable */
#define DAC_DMAUDRIE2 (1 << 29)
/* This bit is set and cleared by software.
	0: DAC channel2 DMA underrun interrupt disabled
	1: DAC channel2 DMA underrun interrupt enabled */

/* Bits 28 - DAC channel2 DMA enable */
#define DAC_DMAEN2 (1 << 28)
/* This bit is set and cleared by software.
	0: DAC channel2 DMA mode disabled
	1: DAC channel2 DMA mode enabled */

/* Bits [27..24] - DAC channel2 mask/amplitude selector */
#define DAC_MAMP2_MSK (((1 << (3 + 1)) - 1) << 24)
#define DAC_MAMP2_SET(VAL) (((VAL) << 24) & DAC_MAMP2_MSK)
#define DAC_MAMP2_GET(REG) (((REG) & DAC_MAMP2_MSK) >> 24)
/* These bits are written by software to select mask in wave generation mode or amplitude in triangle generation mode.
	0000: Unmask bit0 of LFSR/ triangle amplitude equal to 1
	0001: Unmask bits[1:0] of LFSR/ triangle amplitude equal to 3
	0010: Unmask bits[2:0] of LFSR/ triangle amplitude equal to 7
	0011: Unmask bits[3:0] of LFSR/ triangle amplitude equal to 15
	0100: Unmask bits[4:0] of LFSR/ triangle amplitude equal to 31
	0101: Unmask bits[5:0] of LFSR/ triangle amplitude equal to 63
	0110: Unmask bits[6:0] of LFSR/ triangle amplitude equal to 127
	0111: Unmask bits[7:0] of LFSR/ triangle amplitude equal to 255
	1000: Unmask bits[8:0] of LFSR/ triangle amplitude equal to 511
	1001: Unmask bits[9:0] of LFSR/ triangle amplitude equal to 1023
	1010: Unmask bits[10:0] of LFSR/ triangle amplitude equal to 2047
	≥ 1011: Unmask bits[11:0] of LFSR/ triangle amplitude equal to 4095
*/

/* Bits [23..22] - DAC channel2 noise/triangle wave generation enable */
#define DAC_WAVE2_MSK (((1 << (1 + 1)) - 1) << 22)
#define DAC_WAVE2_SET(VAL) (((VAL) << 22) & DAC_WAVE2_MSK)
#define DAC_WAVE2_GET(REG) (((REG) & DAC_WAVE2_MSK) >> 22)

#define DAC_WAVE2_NOISE (1 << 22)
#define DAC_WAVE2_TRIANG (2 << 22)
/* These bits are set/reset by software.
	00: wave generation disabled
	01: Noise wave generation enabled
	1x: Triangle wave generation enabled
	Note: Only used if bit TEN2 = 1 (DAC channel2 trigger enabled) */

/* Bits [21..19] - DAC channel2 trigger selection */
#define DAC_TSEL2_MSK (((1 << (2 + 1)) - 1) << 19) 
#define DAC_TSEL2_SET(VAL) (((VAL) << 19) & DAC_TSEL2_MSK)
#define DAC_TSEL2_GET(REG) (((REG) & DAC_TSEL2_MSK) >> 19)
/* These bits select the external event used to trigger DAC channel2
	000: Timer 6 TRGO event
	001: Timer 8 TRGO event
	010: Timer 7 TRGO event
	011: Timer 5 TRGO event
	100: Timer 2 TRGO event
	101: Timer 4 TRGO event
	110: External line9
	111: Software trigger
	Note: Only used if bit TEN2 = 1 (DAC channel2 trigger enabled). */
#define DAC_TSEL2_TIMER6 (0 << 19)

#if defined(STM32FX2)
#define DAC_TSEL2_TIMER8 (1 << 19)
#elif defined(STM32FX1)
#define DAC_TSEL2_TIMER3 (1 << 19)
#define DAC_TSEL2_TIMER15 (3 << 19)
#endif

#define DAC_TSEL2_TIMER7 (2 << 19)
#define DAC_TSEL2_TIMER5 (3 << 19)
#define DAC_TSEL2_TIMER2 (4 << 19)
#define DAC_TSEL2_TIMER4 (5 << 19)
#define DAC_TSEL2_EXT9   (6 << 19)
#define DAC_TSEL2_SOFT   (7 << 19)

/* Bits 18 - DAC channel2 trigger enable */
#define DAC_TEN2 (1 << 18)
/* This bit is set and cleared by software to enable/disable DAC channel2 trigger
	0: DAC channel2 trigger disabled and data written into the DAC_DHRx register are
	transferred one APB1 clock cycle later to the DAC_DOR2 register
	1: DAC channel2 trigger enabled and data from the DAC_DHRx register are transferred
	three APB1 clock cycles later to the DAC_DOR2 register
	Note: When software trigger is selected, the transfer from the DAC_DHRx register to the
	DAC_DOR2 register takes only one APB1 clock cycle. */

/* Bits 17 - DAC channel2 output buffer disable */
#define DAC_BOFF2 (1 << 17)
/* This bit is set and cleared by software to enable/disable DAC channel2 output buffer.
	0: DAC channel2 output buffer enabled
	1: DAC channel2 output buffer disabled */

/* Bits 16 - DAC channel2 enable */
#define DAC_EN2 (1 << 16)
/* This bit is set and cleared by software to enable/disable DAC channel2.
	0: DAC channel2 disabled
	1: DAC channel2 enabled */

/* [15..14] Reserved. */

/* Bit 13 - DAC channel1 DMA Underrun Interrupt enable */
#define DAC_DMAUDRIE1 (1 << 13) 
/*This bit is set and cleared by software.
	0: DAC channel1 DMA Underrun Interrupt disabled
	1: DAC channel1 DMA Underrun Interrupt enabled */

/* Bit 12 - DAC channel1 DMA enable */
#define DAC_DMAEN1 (1 << 12)
/* This bit is set and cleared by software.
0: DAC channel1 DMA mode disabled
1: DAC channel1 DMA mode enabled */

/* Bits [11..8] - DAC channel1 mask/amplitude selector */
#define DAC_MAMP1_MSK (((1 << (3 + 1)) - 1) << 8)
#define DAC_MAMP1_SET(VAL) (((VAL) << 8) & DAC_MAMP1_MSK)
#define DAC_MAMP1_GET(REG) (((REG) & DAC_MAMP1_MSK) >> 8)
/* These bits are written by software to select mask in wave generation mode or amplitude in triangle generation mode.
	0000: Unmask bit0 of LFSR/ triangle amplitude equal to 1
	0001: Unmask bits[1:0] of LFSR/ triangle amplitude equal to 3
	0010: Unmask bits[2:0] of LFSR/ triangle amplitude equal to 7
	0011: Unmask bits[3:0] of LFSR/ triangle amplitude equal to 15
	0100: Unmask bits[4:0] of LFSR/ triangle amplitude equal to 31
	0101: Unmask bits[5:0] of LFSR/ triangle amplitude equal to 63
	0110: Unmask bits[6:0] of LFSR/ triangle amplitude equal to 127
	0111: Unmask bits[7:0] of LFSR/ triangle amplitude equal to 255
	1000: Unmask bits[8:0] of LFSR/ triangle amplitude equal to 511
	1001: Unmask bits[9:0] of LFSR/ triangle amplitude equal to 1023
	1010: Unmask bits[10:0] of LFSR/ triangle amplitude equal to 2047
	≥ 1011: Unmask bits[11:0] of LFSR/ triangle amplitude equal to 4095 */

/* Bits [7..6] - DAC channel1 noise/triangle wave generation enable */
#define DAC_WAVE1_MSK (((1 << (1 + 1)) - 1) << 6)
#define DAC_WAVE1_SET(VAL) (((VAL) << 6) & DAC_WAVE1_MSK)
#define DAC_WAVE1_GET(REG) (((REG) & DAC_WAVE1_MSK) >> 6)
#define DAC_WAVE1_NOISE (1 << 6)
#define DAC_WAVE1_TRIANG (2 << 6)
/* These bits are set and cleared by software.
	00: wave generation disabled
	01: Noise wave generation enabled
	1x: Triangle wave generation enabled
	Note: Only used if bit TEN1 = 1 (DAC channel1 trigger enabled). */

/* Bits [5..3] - DAC channel1 trigger selection */
#define DAC_TSEL1_MSK (((1 << (2 + 1)) - 1) << 3)
#define DAC_TSEL1_SET(VAL) (((VAL) << 3) & DAC_TSEL1_MSK)
#define DAC_TSEL1_GET(REG) (((REG) & DAC_TSEL1_MSK) >> 3)
/* These bits select the external event used to trigger DAC channel1.
	000: Timer 6 TRGO event
	001: Timer 8 TRGO event
	010: Timer 7 TRGO event
	011: Timer 5 TRGO event
	100: Timer 2 TRGO event
	101: Timer 4 TRGO event
	110: External line9
	111: Software trigger
	Note: Only used if bit TEN1 = 1 (DAC channel1 trigger enabled). */
#define DAC_TSEL1_TIMER6 (0 << 3)

#if defined(STM32FX2)
#define DAC_TSEL1_TIMER8 (1 << 3)
#elif defined(STM32FX1)
#define DAC_TSEL1_TIMER3 (1 << 3)
#define DAC_TSEL1_TIMER15 (3 << 3)
#endif

#define DAC_TSEL1_TIMER7 (2 << 3)
#define DAC_TSEL1_TIMER5 (3 << 3)
#define DAC_TSEL1_TIMER2 (4 << 3)
#define DAC_TSEL1_TIMER4 (5 << 3)
#define DAC_TSEL1_EXT9   (6 << 3)
#define DAC_TSEL1_SOFT   (7 << 3)

/* Bit 2 - DAC channel1 trigger enable */
#define DAC_TEN1 (1 << 2)
/* This bit is set and cleared by software to enable/disable DAC channel1 
   trigger.
   0: DAC channel1 trigger disabled and data written into the DAC_DHRx 
   register are transferred one APB1 clock cycle later to the DAC_DOR1 
   register
   1: DAC channel1 trigger enabled and data from the DAC_DHRx register 
   are transferred three APB1 clock cycles later to the DAC_DOR1 register
   Note: When software trigger is selected, the transfer from the DAC_DHRx 
   register to the DAC_DOR1 register takes only one APB1 clock cycle. */

/* Bit 1 - DAC channel1 output buffer disable */
#define DAC_BOFF1 (1 << 1)
/* This bit is set and cleared by software to enable/disable DAC channel1 output buffer.
	0: DAC channel1 output buffer enabled
	1: DAC channel1 output buffer disabled */

/* Bit 0 - DAC channel1 enable */
#define DAC_EN1 (1 << 0)
/* This bit is set and cleared by software to enable/disable DAC channel1.
	0: DAC channel1 disabled
	1: DAC channel1 enabled */

/* DAC software trigger register (DAC_SWTRIGR) */
#define STM32F_DAC_SWTRIGR 0x04

/* [31..2] Reserved. */

/* Bit 1 - DAC channel2 software trigger */
#define DAC_SWTRIG2 (1 << 1)
/* This bit is set and cleared by software to enable/disable the 
   software trigger.
	0: Software trigger disabled
	1: Software trigger enabled
	Note: This bit is cleared by hardware (one APB1 clock cycle later) once the DAC_DHR2 register value has been loaded into the DAC_DOR2 register. */

/* Bit 0 - DAC channel1 software trigger */
#define DAC_SWTRIG1 (1 << 0)
/* This bit is set and cleared by software to enable/disable the 
   software trigger.
	0: Software trigger disabled
	1: Software trigger enabled
	Note: This bit is cleared by hardware (one APB1 clock cycle later) once the DAC_DHR1 register value has been loaded into the DAC_DOR1 register. */


/* DAC channel1 12-bit right-aligned data holding register */
#define STM32F_DAC_DHR12R1 0x08


/* DAC channel1 12-bit left aligned data holding register */
#define STM32F_DAC_DHR12L1 0x0C


/* DAC channel1 8-bit right aligned data holding register */
#define STM32F_DAC_DHR8R1 0x10


/* DAC channel2 12-bit right aligned data holding register */
#define STM32F_DAC_DHR12R2 0x14


/* DAC channel2 12-bit left aligned data holding register */
#define STM32F_DAC_DHR12L2 0x18


/* DAC channel2 8-bit right-aligned data holding register */
#define STM32F_DAC_DHR8R2 0x1C


/* Dual DAC 12-bit right-aligned data holding register */
#define STM32F_DAC_DHR12RD 0x20


/* DUAL DAC 12-bit left aligned data holding register */
#define STM32F_DAC_DHR12LD 0x24


/* DUAL DAC 8-bit right aligned data holding register */
#define STM32F_DAC_DHR8RD 0x28


/* DAC channel1 data output register */
#define STM32F_DAC_DOR1 0x2C


/* DAC channel2 data output register */
#define STM32F_DAC_DOR2 0x30

/* DAC status register */
#define STM32F_DAC_SR 0x34 

/* [31..30] Reserved. */

#define DAC_DMAUDR2 (1 << 29) /* Bits 29 - DAC channel2 DMA underrun flag */
/* This bit is set by hardware and cleared by software (by writing it to 1).
	0: No DMA underrun error condition occurred for DAC channel2
	1: DMA underrun error condition occurred for DAC channel2 (the currently selected trigger is driving DAC channel2 conversion at a frequency higher than the DMA service capability rate) */

/* [28..14] Reserved. */

#define DAC_DMAUDR1 (1 << 13) /* Bits 13 - DAC channel1 DMA underrun flag */
/* This bit is set by hardware and cleared by software (by writing it to 1).
	0: No DMA underrun error condition occurred for DAC channel1
	1: DMA underrun error condition occurred for DAC channel1 (the currently selected trigger is driving DAC channel1 conversion at a frequency higher than the DMA service capability rate) */

/* [12..0] Reserved. */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32f_dac {
	volatile uint32_t cr;
	volatile uint32_t swtrigr;
	volatile uint32_t dhr12r1;
	volatile uint32_t dhr12l1;
	volatile uint32_t dhr8r1;
	volatile uint32_t dhr12r2;
	volatile uint32_t dhr12l2;
	volatile uint32_t dhr8r2;
	volatile uint32_t dhr12rd;
	volatile uint32_t dhr12ld;
	volatile uint32_t dhr8rd;
	volatile uint32_t dor1;
	volatile uint32_t dor2;
	volatile uint32_t sr;
};

#endif /* __ASSEMBLER__ */


#endif /* __STM32F_DAC_H__ */
