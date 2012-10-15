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
 * @file stm32f-syscfg.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_SYSCFG_H__
#define __STM32F_SYSCFG_H__

/*-------------------------------------------------------------------------
  System configuration controller (SYSCFG)
  -------------------------------------------------------------------------*/

/* SYSCFG memory remap register */
#define STM32F_SYSCFG_MEMRMP 0x00

/* [31..2] Reserved */

/* Bits [1..0] - Memory mapping selection */
#define SYSCFG_MEM_MODE ((1 - 0) << 0)
#define SYSCFG_MEM_MODE_MFLASH (0 << 0)
#define SYSCFG_MEM_MODE_SLASH (1 << 0)
#define SYSCFG_MEM_MODE_FSMC (2 << 0)
#define SYSCFG_MEM_MODE_SRAM (3 << 0)
/* Set and cleared by software. This bit controls the memory internal 
   mapping at address 0x0000 0000. After reset these bits take on the 
   memory mapping selected by the BOOT pins. 
   00: Main Flash memory mapped at 0x0000 0000
   01: System Flash memory mapped at 0x0000 0000
   10: FSMC Bank1 (NOR/PSRAM 1 and 2) mapped at 0x0000 0000
   11: Embedded SRAM (112kB) mapped at 0x0000 0000  */

/* SYSCFG peripheral mode configuration register */
#define STM32F_SYSCFG_PMC 0x04


/* [31..24] Reserved */


/* Bit 23 - Ethernet PHY interface selection */
#define SYSCFG_MII_RMII_SEL (1 << 23)
/* Set and Cleared by software.These bits control the PHY interface for 
   the Ethernet MAC.
   0: MII interface is selected
   1: RMII Why interface is selected 
   Note: This configuration must be done while the MAC is under reset and 
   before enabling the MAC clocks.  */

/* [22..0] Reserved */


/* SYSCFG external interrupt configuration register 1 */
#define STM32F_SYSCFG_EXTICR1 0x08


/* [31..16] Reserved */


/* Bits [15..0] - EXTI x configuration (x = 0 to 3) */
#define SYSCFG_EXTIX_MSK (((1 << (3 + 1)) - 1) << 0)
#define SYSCFG_EXTIX_SET(VAL) (((VAL) << 0) & SYSCFG_EXTIX_MSK)
#define SYSCFG_EXTIX_GET(REG) (((REG) & SYSCFG_EXTIX_MSK) >> 0)
/* These bits are written by software to select the source input for the 
   EXTIx external interrupt.
   0000: PA[x] pin
   0001: PB[x] pin
   0010: PC[x] pin
   0011: PD[x] pin
   0100: PE[x] pin
   0101: PF[C] pin
   0110: PG[x] pin
   0111: PH[x] pin
   1000: PI[x] pin  */

/* SYSCFG external interrupt configuration register 2 */
#define STM32F_SYSCFG_EXTICR2 0x0C

/* [31..16] Reserved */

/* Bits [15..0] - EXTI x configuration (x = 4 to 7) */
#define SYSCFG_EXTIX_MSK (((1 << (3 + 1)) - 1) << 0)
#define SYSCFG_EXTIX_SET(VAL) (((VAL) << 0) & SYSCFG_EXTIX_MSK)
#define SYSCFG_EXTIX_GET(REG) (((REG) & SYSCFG_EXTIX_MSK) >> 0)
/* These bits are written by software to select the source input for the 
   EXTIx external interrupt.
   0000: PA[x] pin
   0001: PB[x] pin
   0010: PC[x] pin
   0011: PD[x] pin
   0100: PE[x] pin
   0101: PF[x] pin
   0110: PG[x] pin
   0111: PH[x] pin
   1000: PI[x] pin */

/* SYSCFG external interrupt configuration register 3 */
#define STM32F_SYSCFG_EXTICR3 0x10


/* [31..16] Reserved */


/* Bits [15..0] - EXTI x configuration (x = 8 to 11) */
#define SYSCFG_EXTIX_MSK (((1 << (3 + 1)) - 1) << 0)
#define SYSCFG_EXTIX_SET(VAL) (((VAL) << 0) & SYSCFG_EXTIX_MSK)
#define SYSCFG_EXTIX_GET(REG) (((REG) & SYSCFG_EXTIX_MSK) >> 0)
/* These bits are written by software to select the source input for the 
   EXTIx external interrupt.
   0000: PA[x] pin
   0001: PB[x] pin
   0010: PC[x] pin
   0011: PD[x] pin
   0100: PE[x] pin
   0101: PF[x] pin
   0110: PG[x] pin
   0111: PH[x] pin
   1000: PI[x] pin */

/* SYSCFG external interrupt configuration register 4 */
#define STM32F_SYSCFG_EXTICR4 0x14


/* [31..16] Reserved */


/* Bits [15..0] - EXTI x configuration (x = 12 to 15) */
#define SYSCFG_EXTIX_MSK (((1 << (3 + 1)) - 1) << 0)
#define SYSCFG_EXTIX_SET(VAL) (((VAL) << 0) & SYSCFG_EXTIX_MSK)
#define SYSCFG_EXTIX_GET(REG) (((REG) & SYSCFG_EXTIX_MSK) >> 0)
/* These bits are written by software to select the source input for the 
   EXTIx external interrupt.
   0000: PA[x] pin
   0001: PB[x] pin
   0010: PC[x] pin
   0011: PD[x] pin
   0100: PE[x] pin
   0101: PF[x] pin
   0110: PG[x] pin
   0111: PH[x] pin
   Note: PI[15:12] are not used. */

/* Compensation cell control register */
#define STM32F_SYSCFG_CMPCR 0x20


/* [31..9] Reserved */


/* Bit 8 - Compensation cell ready flag */
#define SYSCFG_READY (1 << 8)
/* 0: I/O compensation cell not ready
   1: O compensation cell ready  */

/* [7..2] Reserved */


/* Bit 0 - Compensation cell power-down */
#define SYSCFG_CMP_PD (1 << 0)
/* 0: I/O compensation cell power-down mode
   1: I/O compensation cell enabled  */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32f_syscfg {
	volatile uint32_t memrmp;
	volatile uint32_t pmc;
	volatile uint32_t exticr1;
	volatile uint32_t exticr2;
	volatile uint32_t exticr3;
	volatile uint32_t exticr4;
	volatile uint32_t res[2];
	volatile uint32_t cmpcr; /* Compensation cell control register */
};

#endif /* __ASSEMBLER__ */

#endif /* __STM32F_SYSCFG_H__ */
