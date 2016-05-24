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
 * @file arm-dwt.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __ARM_DWT_H__
#define __ARM_DWT_H__

/****************************************************************************
  CM3 DWT (Data Watchpoint and Trace Unit)
 ****************************************************************************/

/* DWT Control Register */

#define DWT_NUMCOMP(CTRL) (((CTRL) >> 28) & 0x0f) 
/* Number of comparators implemented. 
   A value of zero indicates no comparator support. */

#define DWT_NOTRCPKT (1 << 27) 
/* Shows whether the implementation supports trace sampling 
   and exception tracing:
   0 = Trace sampling and exception tracing supported.
   1 = Trace sampling and exception tracing not supported.
   If this bit is RAZ, the NOCYCCNT bit must also RAZ. */

#define DWT_NOEXTTRIG (1 << 26)
/* Shows whether the implementation includes external match signals, 
   CMPMATCH[N]:
   0 = CMPMATCH[N] supported.
   1 = CMPMATCH[N] not supported. */

#define DWT_NOCYCCNT (1 << 25) 
/* Shows whether the implementation supports a cycle counter:
   0 = Cycle counter supported.
   1 = Cycle counter not supported. */

#define DWT_NOPRFCNT (1 << 24) 
/* Shows whether the implementation supports the profiling counters:
   0 = Supported.
   1 = Not supported. */

#define DWT_CYCEVTENA (1 << 22) 
/* Enables POSTCNT underflow Event counter packets generation:
   0 = No POSTCNT underflow packets generated.
   1 = POSTCNT underflow packets generated, if PCSAMPLENA set to 0. 
   This bit is UNK/SBZP if the NOTRCPKT bit is RAO or the
   NOCYCCNT bit is RAO. */

#define DWT_FOLDEVTENA  (1 << 21) 
/* Enables generation of the Folded-instruction counter overflow event:
   0 = Disabled.
   1 = Enabled.
   This bit is UNK/SBZP if the NOPRFCNT bit is RAO. */

#define DWT_LSUEVTENA  (1 << 20) 
/* Enables generation of the LSU counter overflow event.
   0 = Disabled.
   1 = Enabled.
   This bit is UNK/SBZP if the NOPRFCNT bit is RAO. */

#define DWT_SLEEPEVTENA  (1 << 19) 
/* Enables generation of the Sleep counter overflow event.
   0 = Disabled.
   1 = Enabled.
   This bit is UNK/SBZP if the NOPRFCNT bit is RAO. */

#define DWT_EXCEVTENA  (1 << 18) 
/* Enables generation of the Exception overhead counter overflow event:
   0 = Disabled.
   1 = Enabled.
   This bit is UNK/SBZP if the NOPRFCNT bit is RAO. */

#define DWT_CPIEVTENA (1 << 17) 
/* Enables generation of the CPI counter overflow event:
   0 = Disabled.
   1 = Enabled.
   This bit is UNK/SBZP if the NOPRFCNT bit is RAO. */

#define DWT_EXCTRCENA (1 << 16) 
/* Enables generation of exception trace:
   0 = Disabled.
   1 = Enabled.
   This bit is UNK/SBZP if the NOTRCPKT bit is RAO. */

#define DWT_PCSAMPLENA (1 << 12)
/* Enables use of POSTCNT counter as a timer for Periodic PC sample 
   packet generation:
   0 = No Periodic PC sample packets generated.
   1 = Periodic PC sample packets generated.
   This bit is UNK/SBZP if the NOTRCPKT bit is RAO or 
   the NOCYCCNT bit is RAO. */

#define DWT_SYNCTAP (0x03 << 10) 
/* Selects the position of the synchronization packet counter tap
   on the CYCCNT counter. This determines the
   Synchronization packet rate:
   00 = Disabled. No Synchronization packets.
   01 = Synchronization counter tap at CYCCNT[24]
   10 = Synchronization counter tap at CYCCNT[26]
   11 = Synchronization counter tap at CYCCNT[28]
   This field is UNK/SBZP if the NOCYCCNT bit is RAO. */

#define DWT_CYCTAP (1 << 9) 
/*  Selects the position of the POSTCNT tap on the CYCCNT counter:
	0 = POSTCNT tap at CYCCNT[6]
	1 = POSTCNT tap at CYCCNT[10]
	This bit is UNK/SBZP if the NOCYCCNT bit is RAO. */

#define DWT_POSTINIT (0x0f << 5)
/* Initial value for the POSTCNT counter. 
   This field is UNK/SBZP if the NOCYCCNT bit is RAO. */

#define DWT_POSTPRESET (0x07 << 1)
/* Reload value for the POSTCNT counter. 
   This field is UNK/SBZP if the NOCYCCNT bit is RAO. */

#define DWT_CYCCNTENA (1 << 0) 
/* Enables CYCCNT:
   0 = Disabled
   1 = Enabled
   This bit is UNK/SBZP if the NOCYCCNT bit is RAO. */


#define DWT_CTRL_OFFS    0
#define DWT_CYCCNT_OFFS  4
#define DWT_COMP_OFFS   32

#ifdef __ASSEMBLER__

#else /* __ASSEMBLER__ */

#include "jtag_adi.h"

struct arm_dwt {
	volatile uint32_t ctrl; /* RW See a Control Register */
	volatile uint32_t cyccnt; /* RW 0x00000000 Cycle Count Register */
	volatile uint32_t cpicnt; /* RW - CPI Count Register */
	volatile uint32_t exccnt; /* RW - Exception Overhead Count Register */

	volatile uint32_t sleepcnt; /* RW - Sleep Count Register */
	volatile uint32_t lsucnt; /* RW - LSU Count Register */
	volatile uint32_t foldcnt; /* RW - Folded-instruction Count Register */
	volatile uint32_t pcsr; /* RO - Program Counter Sample Register */

	struct {
		volatile uint32_t comp; /* RW - Comparator Register0 */
		volatile uint32_t mask; /* RW - Mask Register0 */
		volatile uint32_t function; /* RW 0x00000000 Function Register0 */
		uint32_t reserved0; 
	} wp[4]; 

	uint32_t reserved3[(0xfd0 - 0x05c) / 4]; 
	volatile uint32_t pid4; /* Peripheral Identification Register #4 */
	volatile uint32_t pid5; /* Peripheral Identification Register #5 */
	volatile uint32_t pid6; /* Peripheral Identification Register #6 */
	volatile uint32_t pid7; /* Peripheral Identification Register #7 */
	volatile uint32_t pid0; /* Peripheral Identification Register #0 */
	volatile uint32_t pid1; /* Peripheral Identification Register #1 */
	volatile uint32_t pid2; /* Peripheral Identification Register #2 */
	volatile uint32_t pid3; /* Peripheral Identification Register #3 */
	volatile uint32_t cid0; /* Component Identification Register #0 */
	volatile uint32_t cid1; /* Component Identification Register #1 */
	volatile uint32_t cid2; /* Component Identification Register #2 */
	volatile uint32_t cid3; /* Component Identification Register #3 */
}; 


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __ARM_DWT_H__ */


