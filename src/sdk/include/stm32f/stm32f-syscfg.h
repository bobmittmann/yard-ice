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
   1: RMII interface is selected 
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
   0101: PF[x] pin
   0110: PG[x] pin
   0111: PH[x] pin
   1000: PI[x] pin  */

#define SYSCFG_EXTI0_MSK (0xf << 0)
#define SYSCFG_EXTI0_PA  (0x0 << 0)
#define SYSCFG_EXTI0_PB  (0x1 << 0)
#define SYSCFG_EXTI0_PC  (0x2 << 0)
#define SYSCFG_EXTI0_PD  (0x3 << 0)
#define SYSCFG_EXTI0_PE  (0x4 << 0)
#define SYSCFG_EXTI0_PF  (0x5 << 0)
#define SYSCFG_EXTI0_PG  (0x6 << 0)
#define SYSCFG_EXTI0_PH  (0x7 << 0)
#define SYSCFG_EXTI0_PI  (0x8 << 0)

#define SYSCFG_EXTI1_MSK (0xf << 4)
#define SYSCFG_EXTI1_PA (0x0 << 4)
#define SYSCFG_EXTI1_PB (0x1 << 4)
#define SYSCFG_EXTI1_PC (0x2 << 4)
#define SYSCFG_EXTI1_PD (0x3 << 4)
#define SYSCFG_EXTI1_PE (0x4 << 4)
#define SYSCFG_EXTI1_PF (0x5 << 4)
#define SYSCFG_EXTI1_PG (0x6 << 4)
#define SYSCFG_EXTI1_PH (0x7 << 4)
#define SYSCFG_EXTI1_PI (0x8 << 4)

#define SYSCFG_EXTI2_MSK (0xf << 8)
#define SYSCFG_EXTI2_PA (0x0 << 8)
#define SYSCFG_EXTI2_PB (0x1 << 8)
#define SYSCFG_EXTI2_PC (0x2 << 8)
#define SYSCFG_EXTI2_PD (0x3 << 8)
#define SYSCFG_EXTI2_PE (0x4 << 8)
#define SYSCFG_EXTI2_PF (0x5 << 8)
#define SYSCFG_EXTI2_PG (0x6 << 8)
#define SYSCFG_EXTI2_PH (0x7 << 8)
#define SYSCFG_EXTI2_PI (0x8 << 8)

#define SYSCFG_EXTI3_MSK (0xf << 12)
#define SYSCFG_EXTI3_PA (0x0 << 12)
#define SYSCFG_EXTI3_PB (0x1 << 12)
#define SYSCFG_EXTI3_PC (0x2 << 12)
#define SYSCFG_EXTI3_PD (0x3 << 12)
#define SYSCFG_EXTI3_PE (0x4 << 12)
#define SYSCFG_EXTI3_PF (0x5 << 12)
#define SYSCFG_EXTI3_PG (0x6 << 12)
#define SYSCFG_EXTI3_PH (0x7 << 12)
#define SYSCFG_EXTI3_PI (0x8 << 12)


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

#define SYSCFG_EXTI4_MSK (0xf << 0)
#define SYSCFG_EXTI4_PA (0x0 << 0)
#define SYSCFG_EXTI4_PB (0x1 << 0)
#define SYSCFG_EXTI4_PC (0x2 << 0)
#define SYSCFG_EXTI4_PD (0x3 << 0)
#define SYSCFG_EXTI4_PE (0x4 << 0)
#define SYSCFG_EXTI4_PF (0x5 << 0)
#define SYSCFG_EXTI4_PG (0x6 << 0)
#define SYSCFG_EXTI4_PH (0x7 << 0)
#define SYSCFG_EXTI4_PI (0x8 << 0)

#define SYSCFG_EXTI5_MSK (0xf << 4)
#define SYSCFG_EXTI5_PA (0x0 << 4)
#define SYSCFG_EXTI5_PB (0x1 << 4)
#define SYSCFG_EXTI5_PC (0x2 << 4)
#define SYSCFG_EXTI5_PD (0x3 << 4)
#define SYSCFG_EXTI5_PE (0x4 << 4)
#define SYSCFG_EXTI5_PF (0x5 << 4)
#define SYSCFG_EXTI5_PG (0x6 << 4)
#define SYSCFG_EXTI5_PH (0x7 << 4)
#define SYSCFG_EXTI5_PI (0x8 << 4)

#define SYSCFG_EXTI6_MSK (0xf << 8)
#define SYSCFG_EXTI6_PA (0x0 << 8)
#define SYSCFG_EXTI6_PB (0x1 << 8)
#define SYSCFG_EXTI6_PC (0x2 << 8)
#define SYSCFG_EXTI6_PD (0x3 << 8)
#define SYSCFG_EXTI6_PE (0x4 << 8)
#define SYSCFG_EXTI6_PF (0x5 << 8)
#define SYSCFG_EXTI6_PG (0x6 << 8)
#define SYSCFG_EXTI6_PH (0x7 << 8)
#define SYSCFG_EXTI6_PI (0x8 << 8)

#define SYSCFG_EXTI7_MSK (0xf << 12)
#define SYSCFG_EXTI7_PA (0x0 << 12)
#define SYSCFG_EXTI7_PB (0x1 << 12)
#define SYSCFG_EXTI7_PC (0x2 << 12)
#define SYSCFG_EXTI7_PD (0x3 << 12)
#define SYSCFG_EXTI7_PE (0x4 << 12)
#define SYSCFG_EXTI7_PF (0x5 << 12)
#define SYSCFG_EXTI7_PG (0x6 << 12)
#define SYSCFG_EXTI7_PH (0x7 << 12)
#define SYSCFG_EXTI7_PI (0x8 << 12)

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

#define SYSCFG_EXTI8_MSK (0xf << 0)
#define SYSCFG_EXTI8_PA (0x0 << 0)
#define SYSCFG_EXTI8_PB (0x1 << 0)
#define SYSCFG_EXTI8_PC (0x2 << 0)
#define SYSCFG_EXTI8_PD (0x3 << 0)
#define SYSCFG_EXTI8_PE (0x4 << 0)
#define SYSCFG_EXTI8_PF (0x5 << 0)
#define SYSCFG_EXTI8_PG (0x6 << 0)
#define SYSCFG_EXTI8_PH (0x7 << 0)
#define SYSCFG_EXTI8_PI (0x8 << 0)

#define SYSCFG_EXTI9_MSK (0xf << 4)
#define SYSCFG_EXTI9_PA (0x0 << 4)
#define SYSCFG_EXTI9_PB (0x1 << 4)
#define SYSCFG_EXTI9_PC (0x2 << 4)
#define SYSCFG_EXTI9_PD (0x3 << 4)
#define SYSCFG_EXTI9_PE (0x4 << 4)
#define SYSCFG_EXTI9_PF (0x5 << 4)
#define SYSCFG_EXTI9_PG (0x6 << 4)
#define SYSCFG_EXTI9_PH (0x7 << 4)
#define SYSCFG_EXTI9_PI (0x8 << 4)

#define SYSCFG_EXTI10_MSK (0xf << 8)
#define SYSCFG_EXTI10_PA (0x0 << 8)
#define SYSCFG_EXTI10_PB (0x1 << 8)
#define SYSCFG_EXTI10_PC (0x2 << 8)
#define SYSCFG_EXTI10_PD (0x3 << 8)
#define SYSCFG_EXTI10_PE (0x4 << 8)
#define SYSCFG_EXTI10_PF (0x5 << 8)
#define SYSCFG_EXTI10_PG (0x6 << 8)
#define SYSCFG_EXTI10_PH (0x7 << 8)
#define SYSCFG_EXTI10_PI (0x8 << 8)

#define SYSCFG_EXTI11_MSK (0xf << 12)
#define SYSCFG_EXTI11_PA (0x0 << 12)
#define SYSCFG_EXTI11_PB (0x1 << 12)
#define SYSCFG_EXTI11_PC (0x2 << 12)
#define SYSCFG_EXTI11_PD (0x3 << 12)
#define SYSCFG_EXTI11_PE (0x4 << 12)
#define SYSCFG_EXTI11_PF (0x5 << 12)
#define SYSCFG_EXTI11_PG (0x6 << 12)
#define SYSCFG_EXTI11_PH (0x7 << 12)
#define SYSCFG_EXTI11_PI (0x8 << 12)

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

#define SYSCFG_EXTI12_MSK (0xf << 0)
#define SYSCFG_EXTI12_PA (0x0 << 0)
#define SYSCFG_EXTI12_PB (0x1 << 0)
#define SYSCFG_EXTI12_PC (0x2 << 0)
#define SYSCFG_EXTI12_PD (0x3 << 0)
#define SYSCFG_EXTI12_PE (0x4 << 0)
#define SYSCFG_EXTI12_PF (0x5 << 0)
#define SYSCFG_EXTI12_PG (0x6 << 0)
#define SYSCFG_EXTI12_PH (0x7 << 0)
#define SYSCFG_EXTI12_PI (0x8 << 0)

#define SYSCFG_EXTI13_MSK (0xf << 4)
#define SYSCFG_EXTI13_PA (0x0 << 4)
#define SYSCFG_EXTI13_PB (0x1 << 4)
#define SYSCFG_EXTI13_PC (0x2 << 4)
#define SYSCFG_EXTI13_PD (0x3 << 4)
#define SYSCFG_EXTI13_PE (0x4 << 4)
#define SYSCFG_EXTI13_PF (0x5 << 4)
#define SYSCFG_EXTI13_PG (0x6 << 4)
#define SYSCFG_EXTI13_PH (0x7 << 4)
#define SYSCFG_EXTI13_PI (0x8 << 4)

#define SYSCFG_EXTI14_MSK (0xf << 8)
#define SYSCFG_EXTI14_PA (0x0 << 8)
#define SYSCFG_EXTI14_PB (0x1 << 8)
#define SYSCFG_EXTI14_PC (0x2 << 8)
#define SYSCFG_EXTI14_PD (0x3 << 8)
#define SYSCFG_EXTI14_PE (0x4 << 8)
#define SYSCFG_EXTI14_PF (0x5 << 8)
#define SYSCFG_EXTI14_PG (0x6 << 8)
#define SYSCFG_EXTI14_PH (0x7 << 8)
#define SYSCFG_EXTI14_PI (0x8 << 8)

#define SYSCFG_EXTI15_MSK (0xf << 12)
#define SYSCFG_EXTI15_PA (0x0 << 12)
#define SYSCFG_EXTI15_PB (0x1 << 12)
#define SYSCFG_EXTI15_PC (0x2 << 12)
#define SYSCFG_EXTI15_PD (0x3 << 12)
#define SYSCFG_EXTI15_PE (0x4 << 12)
#define SYSCFG_EXTI15_PF (0x5 << 12)
#define SYSCFG_EXTI15_PG (0x6 << 12)
#define SYSCFG_EXTI15_PH (0x7 << 12)
#define SYSCFG_EXTI15_PI (0x8 << 12)

/* Compensation cell control register */
#define STM32F_SYSCFG_CMPCR 0x20


/* [31..9] Reserved */


/* Bit 8 - Compensation cell ready flag */
#define SYSCFG_READY (1 << 8)
/* 0: I/O compensation cell not ready
   1: O compensation cell ready  */

/* [7..2] Reserved */


/* Bit 0 - Compensation cell power-down */
#define SYSCFG_CMP_PD (0 << 0)
#define SYSCFG_CMP_EN (1 << 0)
/* 0: I/O compensation cell power-down mode
   1: I/O compensation cell enabled  */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32f_syscfg {
	volatile uint32_t memrmp;
	volatile uint32_t pmc;
	union {
		struct {
			volatile uint32_t exticr1;
			volatile uint32_t exticr2;
			volatile uint32_t exticr3;
			volatile uint32_t exticr4;
		};
		volatile uint32_t exticr[4];
	};
	volatile uint32_t res[2];
	volatile uint32_t cmpcr; /* Compensation cell control register */
};

#endif /* __ASSEMBLER__ */

#endif /* __STM32F_SYSCFG_H__ */
