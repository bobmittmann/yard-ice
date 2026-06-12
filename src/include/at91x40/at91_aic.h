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


#ifndef __AT91_AIC_H__
#define __AT91_AIC_H__

/*****************************************************************************
 AIC: Advanced Interrupt Controller
 *****************************************************************************/

/* AIC Source Mode Register 0 - Read/Write */
#define AIC_SMR0               0x0000
#define AT91_AIC_SMR_FIQ            AIC_SMR0

/* AIC Source Mode Register 1 - Read/Write */
#define AIC_SMR1               0x0004
#define AIC_SMR_SWI            AIC_SMR1
#define AT91_AIC_SMR_SWIRQ          AIC_SMR1

/* AIC Source Mode Register 2 - Read/Write */
#define AIC_SMR2               0x0008
#define AT91_AIC_SMR_US0IRQ         AIC_SMR2

/* AIC Source Mode Register 3 - Read/Write */
#define AIC_SMR3               0x000c
#define AT91_AIC_SMR_US1IRQ         AIC_SMR3

/* AIC Source Mode Register 4 - Read/Write */
#define AIC_SMR4               0x0010
#define AT91_AIC_SMR_TC0IRQ         AIC_SMR4

/* AIC Source Mode Register 5 - Read/Write */
#define AIC_SMR5               0x0014
#define AT91_AIC_SMR_TC1IRQ         AIC_SMR5

/* AIC Source Mode Register 6 - Read/Write */
#define AIC_SMR6               0x0018
#define AT91_AIC_SMR_TC2IRQ         AIC_SMR6

/* AIC Source Mode Register 7 - Read/Write */
#define AIC_SMR7               0x001c
#define AIC_SMR_WD             AIC_SMR7
#define AT91_AIC_SMR_WDIRQ          AIC_SMR7

/* AIC Source Mode Register 8 - Read/Write */
#define AIC_SMR8               0x0020
#define AT91_AIC_SMR_PIOIRQ         AIC_SMR8

/* AIC Source Mode Register 9 - Read/Write */
#define AIC_SMR9               0x0024
/* AIC Source Mode Register 11 - Read/Write */
#define AIC_SMR10              0x0028
/* AIC Source Mode Register 11 - Read/Write */
#define AIC_SMR11              0x002c
/* AIC Source Mode Register 12 - Read/Write */
#define AIC_SMR12              0x0030
/* AIC Source Mode Register 13 - Read/Write */
#define AIC_SMR13              0x0034
/* AIC Source Mode Register 14 - Read/Write */
#define AIC_SMR14              0x0038
/* AIC Source Mode Register 15 - Read/Write */
#define AIC_SMR15              0x003c
/* AIC Source Mode Register 16 - Read/Write */
#define AIC_SMR16              0x0040
/* AIC Source Mode Register 17 - Read/Write */
#define AIC_SMR17              0x0044
/* AIC Source Mode Register 18 - Read/Write */
#define AIC_SMR18              0x0048
/* AIC Source Mode Register 19 - Read/Write */
#define AIC_SMR19              0x004c
/* AIC Source Mode Register 20 - Read/Write */
#define AIC_SMR20              0x0050
/* AIC Source Mode Register 21 - Read/Write */
#define AIC_SMR21              0x0054
/* AIC Source Mode Register 22 - Read/Write */
#define AIC_SMR22              0x0058
/* AIC Source Mode Register 23 - Read/Write */
#define AIC_SMR23              0x005c
/* AIC Source Mode Register 24 - Read/Write */
#define AIC_SMR24              0x0060
/* AIC Source Mode Register 25 - Read/Write */
#define AIC_SMR25              0x0064
/* AIC Source Mode Register 26 - Read/Write */
#define AIC_SMR26              0x0068
/* AIC Source Mode Register 27 - Read/Write */
#define AIC_SMR27              0x006c
/* AIC Source Mode Register 28 - Read/Write */
#define AIC_SMR28              0x0070
/* AIC Source Mode Register 29 - Read/Write */
#define AIC_SMR29              0x0074
/* AIC Source Mode Register 30 - Read/Write */
#define AIC_SMR30              0x0078
/* AIC Source Mode Register 31 - Read/Write */
#define AIC_SMR31              0x007c

#define AIC_SMR_TAB                 ((volatile uint32_t *)(AIC_SMR0))

/* Priority Level */
#define AIC_PRIOR_MASK              (~(7 << 0))
#define AIC_PRIOR(P)                (P << 0)
#define AIC_PRIORITY(P)             ((P & 0x07) << 0)
/* Interrupt Source Type / External Source */
#define AIC_SRCTYPE_EXT_LOW_LEVEL       (0 << 5)
#define AIC_SRCTYPE_EXT_NEGATIVE_EDGE   (1 << 5)
#define AIC_SRCTYPE_EXT_HIGH_LEVEL      (2 << 5)
#define AIC_SRCTYPE_EXT_POSITIVE_EDGE   (3 << 5)
/* Interrupt Source Type / Internal Source */
#define AIC_SRCTYPE_INT_LEVEL           (0 << 5)
#define AIC_SRCTYPE_INT_EDGE            (1 << 5)

/* AIC Source Vector Register 0 - Read/Write */
#define AIC_SVR0               0x0080
#define AIC_SVR_FIQ            AIC_SVR0
/* AIC Source Vector Register 1 - Read/Write */
#define AIC_SVR1               0x0084
#define AIC_SVR_SWIRQ          AIC_SVR1
/* AIC Source Vector Register 2 - Read/Write */
#define AIC_SVR2               0x0088
#define AIC_SVR_US0            AIC_SVR2
/* AIC Source Vector Register 3 - Read/Write */
#define AIC_SVR3               0x008c
#define AIC_SVR_US1IRQ              AIC_SVR3
/* AIC Source Vector Register 4 - Read/Write */
#define AIC_SVR4               0x0090
#define AIC_SVR_TC0IRQ              AIC_SVR4
/* AIC Source Vector Register 5 - Read/Write */
#define AIC_SVR5               0x0094
#define AIC_SVR_TC1IRQ              AIC_SVR5
/* AIC Source Vector Register 6 - Read/Write */
#define AIC_SVR6               0x0098
#define AIC_SVR_TC2IRQ              AIC_SVR6
/* AIC Source Vector Register 7 - Read/Write */
#define AIC_SVR7               0x009c
#define AIC_SVR_WD             AIC_SVR7
#define AIC_SVR_WDIRQ               AIC_SVR7
/* AIC Source Vector Register 8 - Read/Write */
#define AIC_SVR8               0x00a0
#define AIC_SVR_PIOIRQ              AIC_SVR8
/* AIC Source Vector Register 9 - Read/Write */
#define AIC_SVR9               0x00a4
/* AIC Source Vector Register 10 - Read/Write */
#define AIC_SVR10              0x00a8
/* AIC Source Vector Register 11 - Read/Write */
#define AIC_SVR11              0x00ac
/* AIC Source Vector Register 12 - Read/Write */
#define AIC_SVR12              0x00b0
/* AIC Source Vector Register 13 - Read/Write */
#define AIC_SVR13              0x00b4
/* AIC Source Vector Register 14 - Read/Write */
#define AIC_SVR14              0x00b8
/* AIC Source Vector Register 15 - Read/Write */
#define AIC_SVR15              0x00bc
/* AIC Source Vector Register 16 - Read/Write */
#define AIC_SVR16              0x00c0
#define AIC_SVR_IRQ0                AIC_SVR16
/* AIC Source Vector Register 17 - Read/Write */
#define AIC_SVR17              0x00c4
#define AIC_SVR_IRQ1                AIC_SVR17
/* AIC Source Vector Register 18 - Read/Write */
#define AIC_SVR18              0x00c8
#define AIC_SVR_IRQ2                AIC_SVR18
/* AIC Source Vector Register 19 - Read/Write */
#define AIC_SVR19              0x00cc
/* AIC Source Vector Register 20 - Read/Write */
#define AIC_SVR20              0x00d0
/* AIC Source Vector Register 21 - Read/Write */
#define AIC_SVR21              0x00d4
/* AIC Source Vector Register 22 - Read/Write */
#define AIC_SVR22              0x00d8
/* AIC Source Vector Register 23 - Read/Write */
#define AIC_SVR23              0x00dc
/* AIC Source Vector Register 24 - Read/Write */
#define AIC_SVR24              0x00e0
/* AIC Source Vector Register 25 - Read/Write */
#define AIC_SVR25              0x00e4
/* AIC Source Vector Register 26 - Read/Write */
#define AIC_SVR26              0x00e8
/* AIC Source Vector Register 27 - Read/Write */
#define AIC_SVR27              0x00ec
/* AIC Source Vector Register 28 - Read/Write */
#define AIC_SVR28              0x00f0
/* AIC Source Vector Register 29 - Read/Write */
#define AIC_SVR29              0x00f4
/* AIC Source Vector Register 30 - Read/Write */
#define AIC_SVR30              0x00f8
/* AIC Source Vector Register 31 - Read/Write */
#define AIC_SVR31              0x00fc

#define AIC_SVR_TAB                 ((volatile void **)(AIC_SVR0))

/* IRQ Vector Register - Read Only */
#define AIC_IVR                0x0100
 
/* FIQ Vector Register - Read Only */
#define AIC_FVR                0x104
 
/* Interrupt Status Register - Read Only */
#define AIC_ISR                0x108
/* Current IRQ Identifier */
#define AIC_IRQID_MASK              (~(0x001f))

/* Interrupt Pending Register - Read Only */
#define AIC_IPR                0x10c
/* Interrupt Mask Register - Read Only */
#define AIC_IMR                0x110

/* Core Interrupt Status Register - Read Only */
#define AIC_CISR               0x114
/* NFIQ Status */
#define AIC_NFIQ                    1
/* NIRQ Status */
#define AIC_NIRQ                    2

/* AIC Interrupt Enable Command Register - Write Only */
#define AIC_IECR               0x120
/* AIC Interrupt Disable Command Register - Write Only */
#define AIC_IDCR               0x124
/* Interrupt Clear Command Register - Write Only */
#define AIC_ICCR               0x128
/* Interrupt Set Command Register - Write Only */
#define AIC_ISCR               0x12c

#define FIQ                         0
#define SWIRQ                       1
#define US0IRQ                      2
#define US1IRQ                      3
#define TC0IRQ                      4
#define TC1IRQ                      5
#define TC2IRQ                      6
#define WDIRQ                       7
#define PIOIRQ                      8
#define IRQ1                        17
#define IRQ0                        16
#define IRQ2                        18

#define AIC_IRQ2                    (1 << IRQ2)
#define AIC_IRQ1                    (1 << IRQ1)
#define AIC_IRQ0                    (1 << IRQ0)
#define AIC_PIOIRQ                  (1 << PIOIRQ)
#define AIC_WDIRQ                   (1 << WDIRQ)
#define AIC_TC2IRQ                  (1 << TC2IRQ)
#define AIC_TC1IRQ                  (1 << TC1IRQ)
#define AIC_TC0IRQ                  (1 << TC0IRQ)
#define AIC_US1IRQ                  (1 << US1IRQ)
#define AIC_US0IRQ                  (1 << US0IRQ)
#define AIC_SWIRQ                   (1 << SWIRQ)
#define AIC_FIQ                     (1 << FIQ)

/* End of Interrupt Command Register - Write Only */
#define AIC_EOICR              0x130

/* Spurious Vector Register - Read/Write */
#define AIC_SPU                0x134

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_aic {
	uint32_t smr[32];
	void * volatile svr[32];
	void * volatile ivr;
	void * volatile fvr;
	volatile uint32_t isr;
	volatile uint32_t ipr;
	volatile uint32_t imr;
	volatile uint32_t cisr;
	uint32_t res[2];
	volatile uint32_t iecr;
	volatile uint32_t idcr;
	uint32_t iccr;
	volatile uint32_t iscr;
	volatile uint32_t eoicr;
	void * volatile spu;
	uint32_t res2[946];
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_AIC_H__ */

