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


/* ------------------------------------------------------------------------- */
/* Program/erase control register */
#define STM32_FLASH_PECR 0x04

/* Bit 18 OBL_LAUNCH: Launch the option byte loading */
#define FLASH_OBL_LAUNCH (1 << 18)
/* This bit is set by software to launch the option byte loading. 
   This bit is cleared only when the option byte loading has completed. 
   It cannot be written if OPTLOCK is set. 
When this bit is set, a reset is generated.
0: Option byte loading complete
1: Option byte has to be loaded */

/* Bit 17 ERRIE: Error interrupt enable
0: interrupt disabled
1: interrupt enabled */

/* Bit 16 EOPIE: End of programming interrupt enable
0: interrupt disabled
1: interrupt enabled */

/* Bit 15 PARALLBANK: Parallel bank mode.
This bit can be set and cleared by software when no program or erase process is on-going.
When this bit is set, 2 half pages can be programmed or 2 pages can be erased in parallel
(the first one in the first bank and the second one in the second bank).
0: Parallel bank mode disabled
1: Parallel bank mode enabled
Note: This bit is available in Cat.4 and Cat.5 devices only. */

/* Bit 10 FPRG: Half Page/Double Word programming mode */
#define FLASH_FPRG (1 << 10)
/* This bit can be written by software when no program or erase 
   process is ongoing.
   It is used to enable/disable Half Page Programming to the program 
   memory or Double Word Programming to the data EEPROM.
   32 loadings are required to program half a page to the program memory.
   2 loadings are required to program a double word to the data EEPROM.
   This bit is cleared when PELOCK is set.
0: Half Page/Double Word programming disabled
1: Half Page/Double Word programming enabled */

/* Bit 9 ERASE: Page or Double Word erase mode */
#define FLASH_ERASE (1 << 9)
/* This bit can be written by software when no program or erase process is 
   on going. It is used to enable/disable Page Erase on the program memory or 
   Double Word Erase on the data EEPROM and the option byte block.
   This bit is cleared when PELOCK is set.
0: Page or Double Word Erase disabled
1: Page or Double Word Erase enabled */

/* Bit 8 FTDW: Fixed time data write for Byte, Half Word and Word programming */
#define FLASH_FTDW (1 << 8)
/* This bit is writable by software when no program or erase process is 
   ongoing. This bit is used for the data EEPROM only.
   It is cleared when PELOCK is set.
   0: Programming of a Byte, Half Word or word is performed without any 
      previous erase operation. This is possible if the word being written 
	  to is 0x0000 0000.
   1: Before the programming of a Byte, Half Word and word an erase phase 
      is automatically performed. So the time of programming is fixed and 
	  lasts two t prog */

/* Bit 4 DATA: Data EEPROM selection */
#define FLASH_DATA (1 << 4)
/* This bit is writable by software when no program or erase process is 
   ongoing. This bit has to be set prior to data EEPROM double word 
   erase/programming. 
   This bit is cleared when PELOCK is set.
0: Data EEPROM not selected
1: Data EEPROM selected */

/* Bit 3 PROG: Program memory selection */
#define FLASH_PROG (1 << 3)
/* This bit is writable by software when no program or erase process is 
   ongoing. This bit has to be set to gain write access to the program 
   memory, except in the case of word programming.
This bit is cleared when PELOCK is set.
0: Program memory not selected
1: Program memory selected */

/* Bit 2 OPTLOCK: Option bytes block lock */
#define FLASH_OPTLOCK (1 << 2)

/* This bit can only be written to 1. When it is set, it indicates that 
   the option byte block is locked.
   It is cleared by hardware after detecting the unlock sequence. In the 
   event of an unsuccessful unlock operation or a third access to 
   OPTKEYR, a bus error (Cortex ® -M3 hardfault or Busfault) is generated 
   and this bit remains set until the next reset. This bit is set when 
   PELOCK is set.
0: option unlocked
1: option locked */

/* Bit 1 PRGLOCK: Program memory lock */
#define FLASH_PRGLOCK (1 << 1)
/* This bit can only be written to 1. When it is set, it indicates that 
   the program memory cannot be written. It is cleared by hardware after 
   detecting the unlock sequence. In the event of an unsuccessful unlock 
   operation or a third access to PRGKEYR, a bus error (Cortex ® -M3 
   hardfault or Busfault) is generated and this bit remains set until 
   the next reset.
This bit is set when PELOCK is set.
0: program memory unlocked
1: program memory locked */

/* Bit 0 PELOCK: FLASH_PECR and data EEPROM lock */
#define FLASH_PELOCK (1 << 0)
/* This bit can only be written to 1. When it is set, it indicates that 
   the FLASH_PECR register and data EEPROM are locked. It is cleared by 
   hardware after detecting the unlock sequence. In the event of unsuccessful 
   unlock operation or a third access to PEKEYR, a bus error (Cortex ® -M3 
   hardfault or Busfault) is generated and this bit remains set until 
   the next reset. When this bit is cleared, write access to the 
   data EEPROM is allowed.
0: FLASH_PECR is unlocked
1: FLASH_PECR is locked */


/* ------------------------------------------------------------------------- */
/* Power down key register (FLASH_PDKEYR) */
#define STM32_FLASH_PDKEYR 0x08
/* The Power down key register is used to unlock 
   the RUN_PD bit in FLASH_ACR. */
#define FLASH_PDKEY1 0x04152637
#define FLASH_PDKEY2 0xfafbfcfd

/* ------------------------------------------------------------------------- */
/* Program/erase key register (FLASH_PEKEYR) */
#define STM32_FLASH_PEKEYR 0x0c
/* The Program/erase key register is used to allow access to FLASH_PECR 
   and so, to allow program and erase operations in the data EEPROM. */
#define FLASH_PEKEY1 0x89abcdef
#define FLASH_PEKEY2 0x02030405

/* ------------------------------------------------------------------------- */
/* Program memory key register (FLASH_PRGKEYR) */
#define STM32_FLASH_PRGKEYR 0x10
/*	The Program memory key register is used to allow program and erase 
	operations in the Program memory. It is write accessible only after 
	a correct write sequence has been executed in FLASH_PEKER */
#define FLASH_PRGKEYR1 0x8c9daebf
#define FLASH_PRGKEYR2 0x13141516

/* ------------------------------------------------------------------------- */
/* Option byte key register (FLASH_OPTKEYR) */
#define STM32_FLASH_OPTKEYR 0x14
/* The Option key register is used to allow program and erase operations 
   in the option byte block. It is write accessible only after the good 
   write sequence has been executed in FLASH_PEKEYR. */
#define FLASH_OPTKEY1 0xfbead9c8
#define FLASH_OPTKEY2 0x24252627

/* ------------------------------------------------------------------------- */
/* Flash status register */
#define STM32_FLASH_SR 0x18
/* The Flash status register gives information on ongoing program and 
   erase operations. */

/* Bit 13 RDERR: Read protected error */
#define FLASH_RDERR (1 << 13)
/* Set by hardware when an address to be read through the Dbus belongs 
   to a read-protected (PCROP) part of the memory.
   Cleared by writing 1. */

/* Bit 12 OPTVERRUSR: Option UserValidity Error. */
#define FLASH_OPTVERRUSR (1 << 12)
/* Set by hardware when the user options read may not be the ones 
   configured by the user.
   Cleared by writing 1.
   If option have not been properly loaded, each time there is a system 
   reset, OPTVERRUSR is set again. Consequently, an interrupt is 
   generated as soon as ERRIE is set.
   Note: This bit is available in Cat.3, Cat.4 and Cat.5 devices only. */

/* Bit 11 OPTVERR: Option validity error */
#define FLASH_OPTVERR (1 << 11)
/* Set by hardware when the options read may not be the ones configured by 
   the software.
   Cleared by writing 1.
   If the options have not been properly loaded, each time a system reset 
   occurs, OPTVERR reverts to logical level 1. Consequently, an 
   interrupt is generated whenever ERRIE is set. */

/* Bit 10 SIZERR: Size error */
#define FLASH_SIZERR (1 << 10)
/* Set by hardware when the size of the data to program is prohibited.
Cleared by writing it to 1. */

/* Bit 9 PGAERR: Programming alignment error */
#define FLASH_PGAERR (1 << 9)
/* Set by hardware when the data to program cannot be contained in a 
   given half page or double word.
   Cleared by writing it to 1. */

/* Bit 8 WRPERR: Write protected error */
#define FLASH_WRPERR (1 << 8)
/* Set by hardware when an address to be erased/programmed belongs to 
   a write-protected part of the memory.
   Cleared by writing it to 1. */

/* Bit 3 READY: Flash memory module ready after low power mode */
#define FLASH_READY (1 << 3)
/* This bit is set and cleared by hardware.
0: Flash memory module is not ready
1: Flash memory module is ready */

/* Bit 2 ENDHV: End of high voltage */
#define FLASH_ENDHV (1 << 2)
/* This bit is set and cleared by hardware.
0: High voltage still applied during write/erase operations
1: End of high voltage */

/* Bit 1 EOP: End of operation */
#define FLASH_EOP (1 << 1)
/* This bit is set by hardware if the high voltage stops being applied 
   and programming has not been aborted. It is cleared by software 
   (by writing it to 1).
0: No EOP event occurred
1: An EOP event occured. An interrupt is generated if EOPIE is set */

/* Bit 0 BSY: Write/erase operations in progress */
#define FLASH_BSY (1 << 0)
/* 0: Write/erase operation not in progress
1: Write/erase operation in progress */


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

