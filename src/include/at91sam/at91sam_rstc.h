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

#ifndef __AT91_RSTC_H__
#define __AT91_RSTC_H__

/* Reset Controller Interface */

#define RSTC_CR        (0) /* Reset Control Register */
#define RSTC_SR        (4) /* Reset Status Register */
#define RSTC_MR        (8) /* Reset Mode Register */

/* RSTC_RCR - Reset Control Register */
#define RSTC_PROCRST        (0x1 <<  0) /* (RSTC) Processor Reset */
#define RSTC_ICERST         (0x1 <<  1) /* (RSTC) ICE Interface Reset */
#define RSTC_PERRST         (0x1 <<  2) /* (RSTC) Peripheral Reset */
#define RSTC_EXTRST         (0x1 <<  3) /* (RSTC) External Reset */
#define RSTC_KEY(KEY)       (((KEY) & 0xff) << 24) /* (RSTC) Password */

/* RSTC_RSR - Reset Status Register */
#define RSTC_URSTS          (0x1 <<  0) /* (RSTC) User Reset Status */
#define RSTC_BODSTS         (0x1 <<  1) /* (RSTC) Brownout Detection Status */
#define RSTC_RSTTYP         (0x7 <<  8) /* (RSTC) Reset Type */
#define 	RSTC_RSTTYP_GENERAL              (0x0 <<  8) /* (RSTC) General reset. Both VDDCORE and VDDBU rising. */
#define 	RSTC_RSTTYP_WAKEUP               (0x1 <<  8) /* (RSTC) WakeUp Reset. VDDCORE rising. */
#define 	RSTC_RSTTYP_WATCHDOG             (0x2 <<  8) /* (RSTC) Watchdog Reset. Watchdog overflow occured. */
#define 	RSTC_RSTTYP_SOFTWARE             (0x3 <<  8) /* (RSTC) Software Reset. Processor reset required by the software. */
#define 	RSTC_RSTTYP_USER                 (0x4 <<  8) /* (RSTC) User Reset. NRST pin detected low. */
#define RSTC_NRSTL          (0x1 << 16) /* (RSTC) NRST pin level */
#define RSTC_SRCMP          (0x1 << 17) /* (RSTC) Software Reset Command in Progress. */

/* RSTC_RMR - Reset Mode Register */
#define RSTC_URSTEN         (0x1 <<  0) /* (RSTC) User Reset Enable */
#define RSTC_URSTIEN        (0x1 <<  4) /* (RSTC) User Reset Interrupt Enable */
#define RSTC_ERSTL(X)       (((X) & 0xf) <<  8) /* (RSTC) User Reset Length */

#define STL_60_US 0
#define STL_120_US 1
#define STL_240_US 2
#define STL_480_US 3
#define STL_960_US 4
#define STL_2_MS 5
#define STL_4_MS 6
#define STL_8_MS 7
#define STL_16_MS 8
#define STL_31_MS 9
#define STL_62_MS 10
#define STL_125_MS 11
#define STL_250_MS 12
#define STL_500_MS 13
#define STL_1_S 14
#define STL_2_S 15


#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_rstc {
	volatile uint32_t cr; /* Reset Control Register */
	volatile uint32_t sr; /* Reset Status Register */
	volatile uint32_t mr; /* Reset Mode Register */
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_RSTC_H__ */

