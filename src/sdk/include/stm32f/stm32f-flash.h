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
 * @file stm32f-flash.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_FLASH_H__
#define __STM32F_FLASH_H__

/*-------------------------------------------------------------------------
  Flash Interface
  -------------------------------------------------------------------------*/

/* Flash access control register */
#define STM32F_FLASH_ACR 0x00
/*	The Flash access control register is used to enable/disable 
	the acceleration features and control the Flash memory access 
	time according to CPU frequency. */

#if defined(STM32F2X) || defined(STM32F4X)

#define FLASH_DCRST (1 << 12)
/* Data cache reset
0: Data cache is not reset
1: Data cache is reset
This bit can be written only when the D cache is disabled.*/

#define FLASH_ICRST (1 << 11)
/* Instruction cache reset
0: Instruction cache is not reset
1: Instruction cache is reset
This bit can be written only when the I cache is disabled.*/

#define FLASH_DCEN (1 << 10)
/* Data cache enable
0: Data cache is disabled
1: Data cache is enabled*/

#define FLASH_ICEN (1 << 9)
/* Instruction cache enable
0: Instruction cache is disabled
1: Instruction cache is enabled*/

#define FLASH_PRFTEN (1 << 8)
/* Prefetch enable
0: Prefetch is disabled
1: Prefetch is enabled */

#endif /* STM32F2X */

#if defined(STM32F1X) || defined(STM32F3X)

#define FLASH_PRFTBS (1 << 5)
#define FLASH_PRFTBE (1 << 4)
#define FLASH_HLFCYA (1 << 3)

#endif /* defined(STM32F1X) || defined(STM32F3X) */


#define FLASH_LATENCY(WS) (((WS) & 0x7) << 0)
/* Latency. These bits represent the ratio of the CPU clock period to the Flash memory access time. 
000: Zero wait state
001: One wait state
010: Two wait states
011: Three wait states
100: Four wait states
101: Five wait states
110: Six wait states
111: Seven wait states */


/* Flash key register */
#define STM32F_FLASH_KEYR 0x04
/* The Flash key register is used to allow access to the Flash control 
   register and so, to allow program and erase operations. */
#define FLASH_KEY1 0x45670123
#define FLASH_KEY2 0xcdef89ab

/* Flash option key register */
#define FLASH_OPTKEYR 0x08
/* The Flash option key register is used to allow program and erase 
   operations in the user configuration sector. */
#define FLASH_OPTKEY1 0x08192a3b
#define FLASH_OPTKEY2 0x4c5d6e7f




/* Flash status register */
#define STM32F_FLASH_SR 0x0c
/* The Flash status register gives information on ongoing program and 
   erase operations. */

#if defined(STM32F1X) || defined(STM32F3X)

#define FLASH_EOP      (1 << 5)
#define FLASH_WRPRTERR (1 << 4)
#define FLASH_PGPERR   (1 << 2)
#define FLASH_BSY      (1 << 0)

#else /* STM32F1X */


#define FLASH_BSY (1 << 16)
/* Busy. This bit indicates that a Flash memory operation is in progress. 
   It is set at the beginning of a Flash memory operation and cleared 
   when the operation finishes or an error occurs.
	0: no Flash memory operation ongoing
	1: Flash memory operation ongoing */

#define FLASH_PGSERR (1 << 7)
/* Programming sequence error. Set by hardware when a write access to 
   the Flash memory is performed by the code while the control register 
   has not been correctly configured.
   Cleared by writing 1.*/

#define FLASH_PGPERR (1 << 6)
/* Programming parallelism error. Set by hardware when the size of the 
   access (byte, half-word, word, double word) during the program 
   sequence does not correspond to the parallelism configuration 
   PSIZE (x8, x16, x32, x64).
   Cleared by writing 1.*/

#define FLASH_PGAERR (1 << 5)
/* Programming alignment error. Set by hardware when the data to program 
   cannot be contained in the same 128-bit Flash memory row.
   Cleared by writing 1.*/

#define FLASH_WRPERR (1 << 4)
/* Write protection error. Set by hardware when an address to be 
   erased/programmed belongs to a write-protected part of the Flash memory.
   Cleared by writing 1.*/

#define FLASH_OPERR (1 << 1)
/* Operation error. Set by hardware when a flash operation 
   (programming / erase) request is detected and can not be run because 
   of parallelism, alignment, sequence or write protection error. This bit 
   is set only if error interrupts are enabled (ERRIE = 1).*/

#define FLASH_EOP (1 << 0)
/* End of operation. Set by hardware when one or more Flash memory 
   operations (program/erase) has/have completed successfully. 
   It is set only if the end of operation interrupts are 
   enabled (EOPIE = 1).
   Cleared by writing a 1. */

#endif /* STM32F1X */

/* Flash control register */
#define STM32F_FLASH_CR 0x10
/* The Flash control register is used to configure and start 
   Flash memory operations. */

#if defined(STM32F1X) || defined(STM32F3X)

#define FLASH_EOPIE  (1 << 12)
#define FLASH_ERRIE  (1 << 10)
#define FLASH_OPTWRE (1 << 9)
#define FLASH_LOCK   (1 << 7)
#define FLASH_STRT   (1 << 6)
#define FLASH_OPTER  (1 << 5)
#define FLASH_OPTPG  (1 << 4)

#else /* STM32F10X */

#define FLASH_LOCK (1 << 31)
/* Lock. Write to 1 only. When it is set, this bit indicates that the 
   FLASH_CR register is locked. It is cleared by hardware after detecting 
   the unlock sequence. In the event of an unsuccessful unlock operation, 
   this bit remains set until the next reset. */

#define FLASH_ERRIE (1 << 25)
/* Error interrupt enable. This bit enables the interrupt generation when 
   the OPERR bit in the FLASH_SR register is set to 1.
0: Error interrupt generation disabled
1: Error interrupt generation enabled */

#define FLASH_EOPIE (1 << 24)
/* End of operation interrupt enable. This bit enables the interrupt 
   generation when the EOP bit in the FLASH_SR register goes to 1.
0: Interrupt generation disabled
1: Interrupt generation enabled */

#define FLASH_STRT (1 << 16)
/* Start. This bit triggers an erase operation when set. It is set only by 
   software and cleared when the BSY bit is cleared. */

#define FLASH_PSIZE (0x3 << 8)
#define FLASH_PSIZE_8 (0x0 << 8)
#define FLASH_PSIZE_16 (0x1 << 8)
#define FLASH_PSIZE_32 (0x2 << 8)
#define FLASH_PSIZE_64 (0x3 << 8)
/* Program size. These bits select the program parallelism.
00 program x8
01 program x16
10 program x32
11 program x64 */

#define FLASH_SNB(SECT) (((SECT) & 0xf) << 3)
/* Sector number. These bits select the sector to erase.
0000 sector 0
0001 sector 1
...
1011 sector 11
Others not allowed*/

#endif /* STM32F10X */

#define FLASH_MER (1 << 2)
/* Mass Erase. Erase activated for all user sectors.*/

#define FLASH_SER (1 << 1)
/* Sector Erase. Sector Erase activated.*/

#define FLASH_PG (1 << 0)
/* Programming. Flash programming activated. */


/* Flash option control register */
#define STM32F_FLASH_OPTCR 0x14
/* The FLASH_OPTCR register is used to modify the user option bytes. */

#define FLASH_NWRP (1 << 16)
/* Not write protect
These bits contain the value of the write-protection option bytes after reset. They can be
written to program a new write protect value into Flash memory.
0: Write protection active
1: Write protection not active */

#define FLASH_RDP (1 << 15)
/* Read protect
These bits contain the value of the read-protection option level after reset. They can be
written to program a new read protection value into Flash memory.
0xAA: Level 0, read protection not active
0xCC: Level 2, chip read protection active
Others: Level 1, read protection of memories active */

#define FLASH_USER (1 << 7)
/* User option bytes
These bits contain the value of the user option byte after reset. They can be written to
program a new user option byte value into Flash memory.
Bit 7: nRST_STDBY
Bit 6: nRST_STOP
Bit 5: WDG_SW
Note: When changing the WDG mode from hardware to software or from software to
hardware, a system reset is required to make the change effective.
Bit 4 Reserved, must be kept cleared. */

#define FLASH_BOR_LEV (1 << 3)
/* BOR reset Level. These bits contain the supply level threshold that 
   activates/releases the reset. They can be written to program a new BOR 
   level. By default, BOR is off. When the supply voltage (VDD) drops below 
   the selected BOR level, a device reset is generated.
00: BOR Level 3 (VBOR3), reset threshold level for 2.70 to 3.60 V voltage range
01: BOR Level 2 (VBOR2), reset threshold level for 2.40 to 2.70 V voltage range
10: BOR Level 1 (VBOR1), reset threshold level for 2.10 to 2.40 V voltage range
11: BOR off (VBOR0), reset threshold level for 1.80 to 2.10 V voltage range
Note: For full details about BOR characteristics, refer to the "Electrical characteristics" section
in the device datasheet. */

#define FLASH_OPTSTRT (1 << 1)
/* Option start. This bit triggers a user option operation when set. 
   It is set only by software and cleared when the BSY bit is cleared. */

#define FLASH_OPTLOCK (1 << 0)
/* Option lock. Write to 1 only. When this bit is set, it indicates that 
   the FLASH_OPTCR register is locked. This bit is cleared by hardware 
   after detecting the unlock sequence. In the event of an unsuccessful 
   unlock operation, this bit remains set until the next reset. */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32_flash {
	volatile uint32_t acr;      /* access control register */
	volatile uint32_t keyr;     /* key register */
	volatile uint32_t optkeyr;  /* option key register */
	volatile uint32_t sr;       /* status register */
	volatile uint32_t cr;       /* control register */
#if defined(STM32F1X) || defined(STM32F3X)
	volatile uint32_t ar;    /* flash address register */
#else
	volatile uint32_t optcr;    /* option control register */
#endif
};

#endif /* __ASSEMBLER__ */

#endif /* __STM32F_FLASH_H__ */
