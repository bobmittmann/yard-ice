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
 * @file stm32f-fsmc.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_FSMC_H__
#define __STM32F_FSMC_H__

/*-------------------------------------------------------------------------
  Flexible static memory controller (FSMC)
  -------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------- */
/* NOR/PSRAM controller registers */

/* ------------------------------------------------------------------------- */
/* SRAM/NOR-Flash chip-select control registers 1..4 (FSMC_BCR1..4)
   Address offset: 0xA000 0000 + 8 * (x – 1), x = 1...4 */
#define STM32F_FSMC_BCR1 (0xa0000000 + 8 * (1 – 1))
#define STM32F_FSMC_BCR2 (0xa0000000 + 8 * (2 – 1))
#define STM32F_FSMC_BCR3 (0xa0000000 + 8 * (3 – 1))
#define STM32F_FSMC_BCR4 (0xa0000000 + 8 * (4 – 1))

/* Bit 19 - Write burst enable. */
#define FSMC_CBURSTRW (1 << 19)
/* For Cellular RAM, the bit enables synchronous burst protocol during 
   write operations. For Flash memory access in burst mode, this bit 
   enables/disables the wait state insertion via the NWAIT signal. The 
   enable bit for the synchronous burst protocol during read access is 
   the BURSTEN bit in the FSMC_BCRx register.
   0: Write operations are always performed in asynchronous mode
   1: Write operations are performed in synchronous mode. */

/* Bit 15 - Wait signal during asynchronous transfers */
#define FSMC_ASYNCWAIT (1 << 15)
/* This bit enables the FSMC to use the wait signal even during an 
   asynchronous protocol.
   0: NWAIT signal is not taken in to account when running an 
      asynchronous protocol (default after reset)
   1: NWAIT signal is taken in to account when running an 
      asynchronous protocol */

/* Bit 14 - Extended mode enable. */
#define FSMC_EXTMOD (1 << 14)
/* This bit enables the FSMC to program inside the FSMC_BWTR register, so 
   it allows different timings for read and write.
   0: values inside FSMC_BWTR register are not taken into account 
   (default after reset)
   1: values inside FSMC_BWTR register are taken into account */

/* Bit 13 - Wait enable bit. */
#define FSMC_WAITEN (1 << 13)
/* For Flash memory access in burst mode, this bit enables/disables 
   wait-state insertion via the NWAIT signal:
   0: NWAIT signal is disabled (its level not taken into account, no 
   wait state inserted after the programmed Flash latency period)
   1: NWAIT signal is enabled (its level is taken into account after the 
   programmed Flash latency period to insert wait states if 
   asserted) (default after reset) */

/* Bit 12 - Write enable bit. */
#define FSMC_WREN (1 << 12)
/* This bit indicates whether write operations are enabled/disabled in the 
   bank by the FSMC:
   0: Write operations are disabled in the bank by the FSMC, an AHB 
   error is reported,
   1: Write operations are enabled for the bank by the FSMC (default 
   after reset). */

/* Bit 11 - Wait timing configuration. */
#define FSMC_WAITCFG (1 << 11)
/* For memory access in burst mode, the NWAIT signal indicates whether 
   the data from the memory are valid or if a wait state must be 
   inserted. This configuration bit determines if NWAIT is asserted
   by the memory one clock cycle before the wait state or during the 
   wait state:
   0: NWAIT signal is active one data cycle before wait state (default 
   after reset),
   1: NWAIT signal is active during wait state (not for Cellular RAM). */

/* Bit 10 - Wrapped burst mode support. */
#define FSMC_WRAPMOD (1 << 10)
/* Defines whether the controller will or not split an AHB burst wrap 
   access into two linear accesses.
   Valid only when accessing memories in burst mode
   0: Direct wrapped burst is not enabled (default after reset),
   1: Direct wrapped burst is enabled.
   Note: This bit has no effect as the CPU and DMA cannot 
   generate wrapping burst transfers. */

/* Bit 9 - Wait signal polarity bit. */
#define FSMC_WAITPOL (1 << 9)
/* Defines the polarity of the wait signal from memory. Valid only when 
   accessing the memory in burst mode:
   0: NWAIT active low (default after reset),
   1: NWAIT active high. */

/* Bit 8 - Burst enable bit. */
#define FSMC_BURSTEN (1 << 8)
/* Enables the burst access mode for the memory. Valid only with 
   synchronous burst memories:
   0: Burst access mode disabled (default after reset)
   1: Burst access mode enable */

/* Bit 7 Reserved. */

/* Bit 6 - Flash access enable */
#define FSMC_FACCEN (1 << 6)
/* Enables NOR Flash memory access operations.
   0: Corresponding NOR Flash memory access is disabled
   1: Corresponding NOR Flash memory access is enabled (default after reset) */

/* Bits [5..4] - Memory databus width. */
#define FSMC_MWID ((5 - 4) << 4)
#define FSMC_MWID_8  ((0) << 4)
#define FSMC_MWID_16 ((1) << 4)
/* Defines the external memory device width, valid for all type of memories.
   00: 8 bits,
   01: 16 bits (default after reset),
   10: reserved, do not use,
   11: reserved, do not use. */

/* Bits [3..2] - Memory type. */
#define FSMC_MTYP ((3 - 2) << 2)
#define FSMC_MTYP_SRAM  ((0) << 2)
#define FSMC_MTYP_PSRAM ((1) << 2)
#define FSMC_MTYP_FLASH ((2) << 2)
/* Defines the type of external memory attached to the 
   corresponding memory bank:
   00: SRAM, ROM (default after reset for Bank 2...4)
   01: PSRAM (Cellular RAM: CRAM)
   10: NOR Flash/OneNAND Flash (default after reset for Bank 1)
   11: reserved */

/* Bit 1 - Address/data multiplexing enable bit. */
#define FSMC_MUXEN (1 << 1)
/* When this bit is set, the address and data values are multiplexed on 
   the databus, valid only with NOR and PSRAM memories:
   0: Address/Data nonmultiplexed
   1: Address/Data multiplexed on databus (default after reset) */

/* Bit 0 - Memory bank enable bit. */
#define FSMC_MBKEN (1 << 0)
/* Enables the memory bank. After reset Bank1 is enabled, all others 
   are disabled. Accessing a disabled bank causes an ERROR on AHB bus.
   0: Corresponding memory bank is disabled
   1: Corresponding memory bank is enabled */


/* ------------------------------------------------------------------------- */
/* SRAM/NOR-Flash chip-select timing registers 1..4 (FSMC_BTR1..4)
   Address offset: 0xA000 0000 + 0x04 + 8 * (x – 1), x = 1..4 */
#define STM32F_FSMC_BTR1 (0xa0000000 + 0x04 + 8 * (1 – 1))
#define STM32F_FSMC_BTR2 (0xa0000000 + 0x04 + 8 * (2 – 1))
#define STM32F_FSMC_BTR3 (0xa0000000 + 0x04 + 8 * (3 – 1))
#define STM32F_FSMC_BTR4 (0xa0000000 + 0x04 + 8 * (4 – 1))

/* Bits [29..28] - Access mode */
#define FSMC_ACCMOD ((29 - 28) << 28)
#define FSMC_ACCMOD_A ((0) << 28)
#define FSMC_ACCMOD_B ((1) << 28)
#define FSMC_ACCMOD_C ((2) << 28)
#define FSMC_ACCMOD_D ((3) << 28)
/* Specifies the asynchronous access modes as shown in the timing diagrams. 
   These bits are taken into account only when the EXTMOD bit in 
   the FSMC_BCRx register is 1.
   00: access mode A
   01: access mode B
   10: access mode C
   11: access mode D */

/* Bits [27..24] - Data latency for synchronous burst NOR Flash memory */
#define FSMC_DATLAT ((27 - 24) << 24)
#define FSMC_DATLAT_MSK (((1 << (27 - 24 + 1)) - 1) << 24)
#define FSMC_DATLAT_SET(VAL) (((VAL) << 24) & FSMC_DATLAT_MSK)
#define FSMC_DATLAT_GET(REG) (((REG) & FSMC_DATLAT_MSK) >> 24)
/* For NOR Flash with synchronous burst mode enabled, defines the number 
   of memory clock cycles (+2) to issue to the memory before getting the 
   first data:
   0000: Data latency of 2 CLK clock cycles for first burst access
   1111: Data latency of 17 CLK clock cycles for first burst access 
   (default value after reset)
   Note: This timing parameter is not expressed in HCLK periods, but 
   in Flash clock (CLK) periods. In asynchronous NOR Flash, SRAM or ROM 
   accesses, this value is don't care. 
   In the case of CRAM, this field must be set to ‘0’. */

/* Bits [23..20] - Clock divide ratio (for CLK signal) */
#define FSMC_CLKDIV ((23 - 20) << 20)
#define FSMC_CLKDIV_MSK (((1 << (23 - 20 + 1)) - 1) << 20)
#define FSMC_CLKDIV_SET(VAL) (((VAL) << 20) & FSMC_CLKDIV_MSK)
#define FSMC_CLKDIV_GET(REG) (((REG) & FSMC_CLKDIV_MSK) >> 20)
/* Defines the period of CLK clock output signal, expressed in number 
   of HCLK cycles:
   0000: Reserved
   0001: CLK period = 2 × HCLK periods
   0010: CLK period = 3 × HCLK periods
   1111: CLK period = 16 × HCLK periods (default value after reset)
   In asynchronous NOR Flash, SRAM or ROM accesses, this value is don’t care. */

/* Bits [19..16] - Bus turnaround phase duration */
#define FSMC_BUSTURN ((19 - 16) << 16)
#define FSMC_BUSTURN_MSK (((1 << (17 - 16 + 1)) - 1) << 16)
#define FSMC_BUSTURN_SET(VAL) (((VAL) << 16) & FSMC_BUSTURN_MSK)
#define FSMC_BUSTURN_GET(REG) (((REG) & FSMC_BUSTURN_MSK) >> 16)
/* These bits are written by software to insert the bus turnaround delay 
   after a read access only from multiplexed NOR Flash memory to avoid bus 
   contention if the controller needs to drive addresses on the databus 
   for the next side-by-side transaction. BUSTURN can be set to the minimum 
   if the slowest memory does not take more than 6 HCLK clock cycles to 
   put the databus in Hi-Z state. These bits are written by software to 
   add a delay at the end of a write/read transaction. This delay allows 
   to match the minimum time between consecutive transactions (tEHEL from 
   NEx high to NEx low) and the maximum time needed by the memory to free 
   the data bus after a read access (tEHQZ):
   (BUSTRUN + 1)HCLK period ≥ tEHELmin and 
   (BUSTRUN + 2)HCLK period ≥ tEHQZmax if EXTMOD = ‘0’
   (BUSTRUN + 2)HCLK period ≥ max (tEHELmin, tEHQZmax) if EXTMOD = ‘1’.
   0000: BUSTURN phase duration = 0 HCLK clock cycle added
   ...
   1111: BUSTURN phase duration = 15 × HCLK clock cycles (default 
   value after reset) */

/* Bits [15..8] - Data-phase duration */
#define FSMC_DATAST ((15 - 8) << 8)
#define FSMC_DATAST_MSK (((1 << (15 - 8 + 1)) - 1) << 8)
#define FSMC_DATAST_SET(VAL) (((VAL) << 8) & FSMC_DATAST_MSK)
#define FSMC_DATAST_GET(REG) (((REG) & FSMC_DATAST_MSK) >> 8)
/* These bits are written by software to define the duration of the 
   data phase (refer to Figure 396 to Figure 408), used in SRAMs, ROMs 
   and asynchronous NOR Flash accesses:
   0000 0000: Reserved
   0000 0001: DATAST phase duration = 1 × HCLK clock cycles
   0000 0010: DATAST phase duration = 2 × HCLK clock cycles
   ...
   1111 1111: DATAST phase duration = 255 × HCLK clock cycles (default 
   value after reset)
   For each memory type and access mode data-phase duration, please refer 
   to the respective figure (Figure 396 to Figure 408).
   Example: Mode1, write access, DATAST=1: 
   Data-phase duration= DATAST+1 = 2 HCLK clock cycles.
   Note: In synchronous accesses, this value is don't care. */

/* Bits [7..4] - Address-hold phase duration */
#define FSMC_ADDHLD ((7 - 4) << 4)
#define FSMC_ADDHDL_MSK (((1 << (7 - 4 + 1)) - 1) << 4)
#define FSMC_ADDHDL_SET(VAL) (((VAL) << 4) & FSMC_ADDHDL_MSK)
#define FSMC_ADDHDL_GET(REG) (((REG) & FSMC_ADDHDL_MSK) >> 4)
/* These bits are written by software to define the duration of the 
   address hold phase (refer to Figure 405 to Figure 408), used in mode D 
   and multiplexed accesses:
   0000: Reserved
   0001: ADDHLD phase duration =1 × HCLK clock cycle
   0010: ADDHLD phase duration = 2 × HCLK clock cycle
   ...
   1111: ADDHLD phase duration = 15 × HCLK clock cycles (default value 
   after reset)
   For each access mode address-hold phase duration, please refer to the 
   respective figure (Figure 405 to Figure 408).
   Note: In synchronous accesses, this value is not used, the address 
   hold phase is always 1 memory clock period duration. */

/* Bits [3..0] - Address setup phase duration */
#define FSMC_ADDSET ((3 - 0) << 0)
#define FSMC_ADDSET_MSK (((1 << (3 - 0 + 1)) - 1) << 0)
#define FSMC_ADDSET_SET(VAL) (((VAL) << 0) & FSMC_ADDSET_MSK)
#define FSMC_ADDSET_GET(REG) (((REG) & FSMC_ADDSET_MSK) >> 0)
/* These bits are written by software to define the duration of the address 
   setup phase (refer to Figure 396 to Figure 408), used in SRAMs, ROMs and 
   asynchronous NOR Flash accesses:
   0000: ADDSET phase duration = 0 × HCLK clock cycle
   ...
   1111: ADDSET phase duration = 1615 × HCLK clock cycles (default 
   value after reset)
   For each access mode address setup phase duration, please refer to the 
   respective figure (refer to Figure 396 to Figure 408).
   Note: In synchronous accesses, this value is don’t care.
   Note: PSRAMs (CRAMs) have a variable latency due to internal 
   refresh. Therefore these memories issue the NWAIT signal during the 
   whole latency phase to prolong the latency as needed.
   With PSRAMs (CRAMs) the filed DATLAT must be set to 0, so that the 
   FSMC exits its latency phase soon and starts sampling NWAIT from 
   memory, then starts to read or write when the memory is ready.
   This method can be used also with the latest generation of synchronous 
   Flash memories that issue the NWAIT signal, unlike older Flash 
   memories (check the datasheet of the specific Flash memory being used). */


/* ------------------------------------------------------------------------- */
/* SRAM/NOR-Flash write timing registers 1..4 (FSMC_BWTR1..4)
   Address offset: 0xA000 0000 + 0x104 + 8 * (x – 1), x = 1...4 */
#define STM32F_FSMC_BWTR1 (0xa0000000 + 0x104 + 8 * (1 – 1))
#define STM32F_FSMC_BWTR2 (0xa0000000 + 0x104 + 8 * (2 – 1))
#define STM32F_FSMC_BWTR3 (0xa0000000 + 0x104 + 8 * (3 – 1))
#define STM32F_FSMC_BWTR4 (0xa0000000 + 0x104 + 8 * (4 – 1))
/* This register contains the control information of each memory bank, 
   used for SRAMs, ROMs and NOR Flash memories. When the EXTMOD bit is 
   set in the FSMC_BCRx register, then this register is active for 
   write access. */

/* Bits [29..28] - Access mode. */
#define FSMC_ACCMOD ((29 - 28) << 28)
/* Specifies the asynchronous access modes as shown in the next timing 
   diagrams.These bits are taken into account only when the EXTMOD bit in 
   the FSMC_BCRx register is 1.
   00: access mode A
   01: access mode B
   10: access mode C
   11: access mode D */

/* Bits [27..24] - Data latency (for synchronous burst NOR Flash). */
#define FSMC_DATLAT ((27 - 24) << 24)
/* For NOR Flash with Synchronous burst mode enabled, defines the number 
   of memory clock cycles (+2) to issue to the memory before getting the 
   first data:
   0000: (0x0) Data latency of 2 CLK clock cycles for first burst access
   ...
   1111: (0xF) Data latency of 17 CLK clock cycles for first burst 
   access (default value after reset)
   Note: This timing parameter is not expressed in HCLK periods, but in 
   Flash clock (CLK) periods. In asynchronous NOR Flash, SRAM or ROM 
   accesses, this value is don’t care. In case of CRAM, this field 
   must be set to 0 */

/* Bits [23..20] - Clock divide ratio (for CLK signal). */
#define FSMC_CLKDIV ((23 - 20) << 20)
/* Defines the period of CLK clock output signal, expressed in number of 
   HCLK cycles:
   0000: Reserved
   0001 CLK period = 2 × HCLK periods
   0010 CLK period = 3 × HCLK periods
   1111: CLK period = 16 × HCLK periods (default value after reset)
   In asynchronous NOR Flash, SRAM or ROM accesses, 
   this value is don’t care. */

/* Bits [19..16] - Bus turnaround phase duration */
#define FSMC_BUSTURN ((19 - 16) << 16)
/* These bits are written by software to add a delay at the end of a 
   write transaction to match the minimum time between consecutive 
   transactions (tEHEL from ENx high to ENx low): 
   (BUSTRUN + 1) HCLK period ≥ tEHELmin.
   0000: BUSTURN phase duration = 0 HCLK clock cycle added
   ...
   1111: BUSTURN phase duration = 15 HCLK clock cycles added (default 
   value after reset) */

/* Bits [15..8] - Data-phase duration. */
#define FSMC_DATAST ((15 - 8) << 8)
/* These bits are written by software to define the duration of the data 
   phase (refer to Figure 396 to Figure 408), used in SRAMs, ROMs and 
   asynchronous NOR Flash accesses:
   0000 0000: Reserved
   0000 0001: DATAST phase duration = 1 × HCLK clock cycles
   0000 0010: DATAST phase duration = 2 × HCLK clock cycles
   ...
   1111 1111: DATAST phase duration = 255 × HCLK clock cycles (default 
   value after reset)
   Note: In synchronous accesses, this value is don't care. */

/* Bits [7..4] - Address-hold phase duration. */
#define FSMC_ADDHLD ((7 - 4) << 4)
/* These bits are written by software to define the duration of the 
   address hold phase (refer to Figure 405 to Figure 408), used in SRAMs, 
   ROMs and asynchronous multiplexed NOR Flash accesses:
   0000: Reserved
   0001: ADDHLD phase duration = 1 × HCLK clock cycle
   0010: ADDHLD phase duration = 2 × HCLK clock cycle
   ...
   1111: ADDHLD phase duration = 15 × HCLK clock cycles (default 
   value after reset)
   Note: In synchronous NOR Flash accesses, this value is not used, the 
   address hold phase is always 1 Flash clock period duration. */

/* Bits [3..0] - Address setup phase duration. */
#define FSMC_ADDSET ((3 - 0) << 0)
/* These bits are written by software to define the duration of the 
   address setup phase in HCLK cycles (refer to Figure 405 to Figure 408),
   used in SRAMs, ROMs and asynchronous NOR Flash accessed:
   0000: ADDSET phase duration = 0 × HCLK clock cycle
   ...
   1111: ADDSET phase duration = 15 × HCLK clock cycles (default value 
   after reset)
   Note: In synchronous NOR Flash accesses, this value is don’t care. */


/* ------------------------------------------------------------------------- */
/*  NAND Flash/PC Card controller registers */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* PC Card/NAND Flash control registers 2..4 (FSMC_PCR2..4)
Address offset: 0xA0000000 + 0x40 + 0x20 * (x – 1), x = 2..4 */
#define STM32F_FSMC_PCR2 (0xa0000000 + 0x40 + 0x20 * (2 – 1))
#define STM32F_FSMC_PCR3 (0xa0000000 + 0x40 + 0x20 * (3 – 1))
#define STM32F_FSMC_PCR4 (0xa0000000 + 0x40 + 0x20 * (4 – 1))

/* Bits [19..17] - ECC page size. */
#define FSMC_ECCPS ((19 - 17) << 17)
/* Defines the page size for the extended ECC:
000: 256 bytes
001: 512 bytes
010: 1024 bytes
011: 2048 bytes
100: 4096 bytes
101: 8192 bytes */

/* Bits [16..13] - ALE to RE delay. */
#define FSMC_TAR ((16 - 13) << 13)
/* Sets time from ALE low to RE low in number of AHB clock cycles (HCLK).
Time is: t_ar = (TAR + SET + 2) × THCLK where THCLK is the HCLK clock period
0000: 1 HCLK cycle (default)
1111: 16 HCLK cycles
Note: SET is MEMSET or ATTSET according to the addressed space. */

/* Bits [12..9] - CLE to RE delay. */
#define FSMC_TCLR ((12 - 9) << 9)
/* Sets time from CLE low to RE low in number of AHB clock cycles (HCLK).
Time is t_clr = (TCLR + SET + 2) × THCLK where THCLK is the HCLK clock period
0000: 1 HCLK cycle (default)
1111: 16 HCLK cycles
Note: SET is MEMSET or ATTSET according to the addressed space.
Bits 8:7
Reserved. */

/* Bit 6 - ECC computation logic enable bit */
#define FSMC_ECCEN (1 << 6)
/* 0: ECC logic is disabled and reset (default after reset),
1: ECC logic is enabled. */

/* Bits [5..4] - Databus width. */
#define FSMC_PWID ((5 - 4) << 4)
/* Defines the external memory device width.
00: 8 bits (default after reset)
01: 16 bits (mandatory for PC Card)
10: reserved, do not use
11: reserved, do not use */

/* Bit 3 - Memory type. */
#define FSMC_PTYP (1 << 3)
/* Defines the type of device attached to the corresponding memory bank:
0: PC Card, CompactFlash, CF+ or PCMCIA
1: NAND Flash (default after reset) */

/* Bit 2 - PC Card/NAND Flash memory bank enable bit. */
#define FSMC_PBKEN (1 << 2)
/* Enables the memory bank. Accessing a disabled memory bank causes an ERROR on AHB
bus
0: Corresponding memory bank is disabled (default after reset)
1: Corresponding memory bank is enabled */

/* Bit 1 - Wait feature enable bit. */
#define FSMC_PWAITEN (1 << 1)
/* Enables the Wait feature for the PC Card/NAND Flash memory bank:
0: disabled
1: enabled
Note: For a PC Card, when the wait feature is enabled, the MEMWAITx/ATTWAITx/IOWAITx
bits must be programmed to a value as follows:
xxWAITx ≥ 4 + max_wait_assertion_time/HCLK
Where max_wait_assertion_time is the maximum time taken by NWAIT to go low once
nOE/nWE or nIORD/nIOWR is low.
Bit 0
Reserved. */

/* FIFO status and interrupt register 2..4 (FSMC_SR2..4)
Address offset: 0xA000 0000 + 0x44 + 0x20 * (x-1), x = 2..4 */

/* Bit 6 - FIFO empty. */
#define FSMC_FEMPT (1 << 6)
/* Read-only bit that provides the status of the FIFO
0: FIFO not empty
1: FIFO empty */

/* Bit 5 - Interrupt falling edge detection enable bit */
#define FSMC_IFEN (1 << 5)
/* 
0: Interrupt falling edge detection request disabled
1: Interrupt falling edge detection request enabled */

/* Bit 4 - Interrupt high-level detection enable bit */
#define FSMC_ILEN (1 << 4)
/* 
0: Interrupt high-level detection request disabled
1: Interrupt high-level detection request enabled */

/* Bit 3 - Interrupt rising edge detection enable bit */
#define FSMC_IREN (1 << 3)
/* 
0: Interrupt rising edge detection request disabled
1: Interrupt rising edge detection request enabled */

/* Bit 2 - Interrupt falling edge status */
#define FSMC_IFS (1 << 2)
/* 
The flag is set by hardware and reset by software.
0: No interrupt falling edge occurred
1: Interrupt falling edge occurred */

/* Bit 1 - Interrupt high-level status */
#define FSMC_ILS (1 << 1)
/* 
The flag is set by hardware and reset by software.
0: No Interrupt high-level occurred
1: Interrupt high-level occurred */

/* Bit 0 - Interrupt rising edge status */
#define FSMC_IRS (1 << 0)
/* 
The flag is set by hardware and reset by software.
0: No interrupt rising edge occurred
1: Interrupt rising edge occurred
Common memory space timing register 2..4 (FSMC_PMEM2..4)
Address offset: Address: 0xA000 0000 + 0x48 + 0x20 * (x – 1), x = 2..4
Each FSMC_PMEMx (x = 2..4) read/write register contains the timing information for PC
Card or NAND Flash memory bank x, used for access to the common memory space of the
16-bit PC Card/CompactFlash, or to access the NAND Flash for command, address write
access and data read/write access. */

/* Bits [31..24] - Common memory x databus HiZ time */
#define FSMC_MEMHIZX ((31 - 24) << 24)
/* 
Defines the number of HCLK clock cycles during which the databus is kept in HiZ after the
start of a PC Card/NAND Flash write access to common memory space on socket x. Only
valid for write transaction:
0000 0000: (0x00) 0 HCLK cycle (for PC Card)
1111 1111: (0xFF) 255 HCLK cycles (for PC Card) - (default value after reset) */

/* Bits [23..16] - Common memory x hold time */
#define FSMC_MEMHOLDX ((23 - 16) << 16)
/* 
Defines the number of HCLK clock cycles to hold address (and data for write access) after
the command deassertion (NWE, NOE), for PC Card/NAND Flash read or write access to
common memory space on socket x:
   0000 0000: reserved
   0000 0001: 1 HCLK cycle
   1111 1111: 255 HCLK cycles (default value after reset) */

/* Bits [15..8] - Common memory x wait time */
#define FSMC_MEMWAITX ((15 - 8) << 8)
/* 
Defines the minimum number of HCLK (+1) clock cycles to assert the command (NWE,
NOE), for PC Card/NAND Flash read or write access to common memory space on socket
x. The duration for command assertion is extended if the wait signal (NWAIT) is active (low)
at the end of the programmed value of HCLK:
   0000 0000: reserved
   0000 0001: 2HCLK cycles (+ wait cycle introduced by deasserting NWAIT)
   1111 1111: 256 HCLK cycles (+ wait cycle introduced by the Card deasserting NWAIT)
(default value after reset) */

/* Bits [7..0] - Common memory x setup time */
#define FSMC_MEMSETX ((7 - 0) << 0)
/* Defines the number of HCLK (+1) clock cycles to set up the address 
   before the command assertion (NWE, NOE), for PC Card/NAND Flash read 
   or write access to common memory space on socket x:
   0000 0000: 1 HCLK cycle (for PC Card) / HCLK cycles (for NAND Flash)
   1111 1111: 256 HCLK cycles (for PC Card) / 257 HCLK cycles (for NAND 
   Flash) - (default value after reset) */

/* Attribute memory space timing registers 2..4 (FSMC_PATT2..4)
Address offset: 0xA000 0000 + 0x4C + 0x20 * (x – 1), x = 2..4 */

/* Each FSMC_PATTx (x = 2..4) read/write register contains the timing 
   information for PC Card/CompactFlash or NAND Flash memory bank x. It 
   is used for 8-bit accesses to the attribute memory space of the PC 
   Card/CompactFlash or to access the NAND Flash for the last address write 
   access if the timing must differ from that of previous accesses (for 
   Ready/Busy management, refer to Section 31.6.5: NAND Flash pre-wait 
   functionality). */

/* Bits [31..24] - Attribute memory x databus HiZ time */
#define FSMC_ATTHIZX ((31 - 24) << 24)
/* Defines the number of HCLK clock cycles during which the databus is 
   kept in HiZ after the start of a PC CARD/NAND Flash write access to 
   attribute memory space on socket x. Only valid for write transaction:
   0000 0000: 0 HCLK cycle
   1111 1111: 255 HCLK cycles (default value after reset) */

/* Bits [23..16] - Attribute memory x hold time */
#define FSMC_ATTHOLDX ((23 - 16) << 16)
/* Defines the number of HCLK clock cycles to hold address (and data for 
   write access) after the command deassertion (NWE, NOE), for PC 
   Card/NAND Flash read or write access to attribute memory space on socket x
   0000 0000: reserved
   0000 0001: 1 HCLK cycle
   1111 1111: 255 HCLK cycles (default value after reset) */

/* Bits [15..8] - Attribute memory x wait time */
#define FSMC_ATTWAITX ((15 - 8) << 8)
/* Defines the minimum number of HCLK (+1) clock cycles to assert the 
   command (NWE, NOE), for PC Card/NAND Flash read or write access to 
   attribute memory space on socket x.
   The duration for command assertion is extended if the wait signal 
   (NWAIT) is active (low) at the end of the programmed value of HCLK:
   0000 0000: reserved
   0000 0001: 2 HCLK cycles (+ wait cycle introduced by deassertion of NWAIT)
   1111 1111: 256 HCLK cycles (+ wait cycle introduced by the card 
   deasserting NWAIT) (default value after reset) */

/* Bits [7..0] - Attribute memory x setup time */
#define FSMC_ATTSETX ((7 - 0) << 0)
/* Defines the number of HCLK (+1) clock cycles to set up address before 
   the command assertion (NWE, NOE), for PC CARD/NAND Flash read or write 
   access to attribute memory space on socket x:
   0000 0000: 1 HCLK cycle
   1111 1111: 256 HCLK cycles (default value after reset) */

/* I/O space timing register 4 */
#define STM32F_FSMC_PIO4 0xA000
/* The FSMC_PIO4 read/write registers contain the timing information used to 
   gain access to the I/O space of the 16-bit PC Card/CompactFlash. */

/* Bits [31..24] - I/O x databus HiZ time */
#define FSMC_IOHIZX ((31 - 24) << 24)
/* Defines the number of HCLK clock cycles during which the databus is 
   kept in HiZ after the start of a PC Card write access to I/O space 
   on socket x. Only valid for write transaction:
   0000 0000: 0 HCLK cycle
   1111 1111: 255 HCLK cycles (default value after reset) */

/* Bits [23..16] - I/O x hold time */
#define FSMC_IOHOLDX ((23 - 16) << 16)
/* Defines the number of HCLK clock cycles to hold address (and data for 
   write access) after the command deassertion (NWE, NOE), for PC Card 
   read or write access to I/O space on socket x:
   0000 0000: reserved
   0000 0001: 1 HCLK cycle
   1111 1111: 255 HCLK cycles (default value after reset) */

/* Bits [15..8] - I/O x wait time */
#define FSMC_IOWAITX ((15 - 8) << 8)
/* Defines the minimum number of HCLK (+1) clock cycles to assert the 
   command (SMNWE, SMNOE), for PC Card read or write access to I/O space 
   on socket x. The duration for command assertion is extended if the wait 
   signal (NWAIT) is active (low) at the end of the programmed value of HCLK:
   0000 0000: reserved, do not use this value
   0000 0001: 2 HCLK cycles (+ wait cycle introduced by deassertion of NWAIT)
   1111 1111: 256 HCLK cycles (+ wait cycle introduced by the Card 
   deasserting NWAIT) */

/* Bits [7..0] - I/O x setup time */
#define FSMC_IOSETX ((7 - 0) << 0)
/* Defines the number of HCLK (+1) clock cycles to set up the address 
   before the command assertion (NWE, NOE), for PC Card read or write 
   access to I/O space on socket x:
   0000 0000: 1 HCLK cycle
   1111 1111: 256 HCLK cycles (default value after reset) */


/* ------------------------------------------------------------------------- */
/* ECC result registers 2/3 (FSMC_ECCR2/3)
   Address offset: 0xA000 0000 + 0x54 + 0x20 * (x – 1), x = 2 or 3 */
#define STM32F_FSMC_ECCR2 (0xa0000000 + 0x54 + 0x20 * (2 – 1))
#define STM32F_FSMC_ECCR3 (0xa0000000 + 0x54 + 0x20 * (3 – 1))
/* These registers contain the current error correction code value computed 
   by the ECC computation modules of the FSMC controller (one module per 
   NAND Flash memory bank).
   When the CPU reads the data from a NAND Flash memory page at the correct 
   address (refer to Section 31.6.6: Error correction code computation 
   ECC (NAND Flash)), the data read from or written to the NAND Flash are 
   processed automatically by ECC computation module. At the end of X bytes 
   read (according to the ECCPS field in the FSMC_PCRx registers), the CPU 
   must read the computed ECC value from the FSMC_ECCx registers, and then 
   verify whether these computed parity data are the same as the parity 
   value recorded in the spare area, to determine whether a page is valid, 
   and, to correct it if applicable. The FSMC_ECCRx registers should be 
   cleared after being read by setting the ECCEN bit to zero. For computing 
   a new data block, the ECCEN bit must be set to one. */

/* Bits [31..0] - ECC result */
#define FSMC_ECCX ((31 - 0) << 0)
/* This field provides the value computed by the ECC computation logic. 
   Table 198 hereafter describes the contents of these bit fields. */


#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32f_fsmc {
	volatile uint32_t bcr1;
	volatile uint32_t btr1;
	volatile uint32_t bcr2;
	volatile uint32_t btr2;
	volatile uint32_t bcr3;
	volatile uint32_t btr3;
	volatile uint32_t bcr4;
	volatile uint32_t btr4;
	/* 0x020 */
	uint32_t res1[(0x060 - 0x020) / 4]; 

	/* 0x060 */
	volatile uint32_t pcr2;
	volatile uint32_t sr2;
	volatile uint32_t pmem2;
	volatile uint32_t patt2;
	uint32_t res2;
	volatile uint32_t eccr2;
	uint32_t res3[2];

	/* 0x080 */
	volatile uint32_t pcr3;
	volatile uint32_t sr3;
	volatile uint32_t pmem3;
	volatile uint32_t patt3;
	uint32_t res4;
	volatile uint32_t eccr3;
	uint32_t res5[2];

	/* 0x0a0 */
	volatile uint32_t pcr4;
	volatile uint32_t sr4;
	volatile uint32_t pmem4;
	volatile uint32_t patt4;
	volatile uint32_t pio4;
	/* 0x0b4 */
	uint32_t res6[(0x104 - 0x0b4) / 4]; 

	/* 0x104 */
	volatile uint32_t bwtr1;
	uint32_t res7;
	volatile uint32_t bwtr2;
	uint32_t res8;
	volatile uint32_t bwtr3;
	uint32_t res9;
	volatile uint32_t bwtr4;
};

#endif /* __ASSEMBLER__ */


#endif /* __STM32F_RCC_H__ */

