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
 * @file stm32l-flash.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32L_FLASH_H__
#define __STM32L_FLASH_H__

/*-------------------------------------------------------------------------
  Flash Interface
  -------------------------------------------------------------------------*/

/* Flash access control register */
#define STM32_FLASH_ACR 0x00
/*	The Flash access control register is used to enable/disable 
	the acceleration features and control the Flash memory access 
	time according to CPU frequency. */

#define FLASH_RUN_PD (1 << 4)
/* Power saving mode during Run
   This bit can be written only when it is unlocked by writing to FLASH_PDKEYR.
   This bit determines whether the Flash memory module is in 
   Power down mode or Idle mode when the STM32L1xxxx is in Run mode.
   The Flash memory module can be placed in Power down mode only 
   when the code is executed from SRAM).
0: Flash module in Idle mode
1: Flash modulein Power down mode */

#define FLASH_SLEEP_PD (1 << 3)
/* Power saving mode during Sleep
   This bit is used to put the Flash memory module in Power down mode 
   or Idle mode when the STM32L1xxxx is in Sleep mode.
0: Flash module in Idle mode
1: Flash module in Power down mode 
 */

#define FLASH_ACC64 (1 << 2)
/* 64-bit access
   This bit is used to read data from the memory 64 bits or 32 bits at a time. 
   32-bit access is used to decreases the memory consumption. 
   On the contrary, 64-bit access is used to improve the performance. 
   In this case it is useful to enable prefetch.
0: 32-bit access
1: 64-bit access
Note: 32-bit access is a low power mode. It is used only at low 
frequencies, that is with 0 wait state of latency and prefetch off.
Note: This bit cannot be written at the same time as 
the LATENCY and PRFTEN bits */

#define FLASH_PRFTEN (1 << 1)
/* Prefetch enable
0: Prefetch is disabled
1: Prefetch is enabled 
Note: Prefetch can be enabled only when ACC64 is set.
This bit can be set or cleared only if ACC64 is set. */

#define FLASH_LATENCY (1 << 0)
/* Latency. This bit represent the ratio of the CPU clock period to the Flash memory access time. 
0: Zero wait state
1: One wait state 
Note: Latency can be set only when ACC64 is set.
This bit can be set or cleared only if ACC64 is set. */


/* Flash option key register */
#define FLASH_OPTKEYR 0x08

/* Flash status register */
#define STM32_FLASH_SR 0x18
/* The Flash status register gives information on ongoing program and 
   erase operations. */

#define FLASH_BSY (1 << 16)
/* Busy. This bit indicates that a Flash memory operation is in progress. 
   It is set at the beginning of a Flash memory operation and cleared 
   when the operation finishes or an error occurs.
	0: no Flash memory operation ongoing
	1: Flash memory operation ongoing */


#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32_flash {
	volatile uint32_t acr;      
	volatile uint32_t pecr;   
	volatile uint32_t pdkeyr;
	volatile uint32_t pekeyr;
	volatile uint32_t prgkeyr;
	volatile uint32_t optkeyr;
	volatile uint32_t sr;
	volatile uint32_t obr;
};

#endif /* __ASSEMBLER__ */

#endif /* __STM32L_FLASH_H__ */

