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

/* Advanced Interrupt Controller */

#define AIC_SMR         ( 0) /* Source Mode Register */
#define AIC_SVR         (128) /* Source Vector Register */
#define AIC_IVR         (256) /* IRQ Vector Register */
#define AIC_FVR         (260) /* FIQ Vector Register */
#define AIC_ISR         (264) /* Interrupt Status Register */
#define AIC_IPR         (268) /* Interrupt Pending Register */
#define AIC_IMR         (272) /* Interrupt Mask Register */
#define AIC_CISR        (276) /* Core Interrupt Status Register */
#define AIC_IECR        (288) /* Interrupt Enable Command Register */
#define AIC_IDCR        (292) /* Interrupt Disable Command Register */
#define AIC_ICCR        (296) /* Interrupt Clear Command Register */
#define AIC_ISCR        (300) /* Interrupt Set Command Register */
#define AIC_EOICR       (304) /* End of Interrupt Command Register */
#define AIC_SPU         (308) /* Spurious Vector Register */
#define AIC_DCR         (312) /* Debug Control Register (Protect) */
#define AIC_FFER        (320) /* Fast Forcing Enable Register */
#define AIC_FFDR        (324) /* Fast Forcing Disable Register */
#define AIC_FFSR        (328) /* Fast Forcing Status Register */

/* AIC_SMR - Control Register */
/* (AIC) Priority Level */
#define AIC_PRIOR(P)                ((P & 0x07) << 0)
#define AIC_PRIORITY(P)             ((P & 0x07) << 0)
#define AIC_PRIOR_MASK              (~(7 << 0))
/* (AIC) Highest priority level */
#define AIC_PRIOR_HIGHEST              (0x7) 
/* (AIC) Lowest priority level */
#define AIC_PRIOR_LOWEST               (0x0) 

/* (AIC) Interrupt Source Type */
#define AIC_SRCTYPE_MASK         (0x3 <<  5)
/* Interrupt Source Type / External Source */
#define AIC_SRCTYPE_EXT_LOW_LEVEL       (0 << 5)
#define AIC_SRCTYPE_EXT_NEGATIVE_EDGE   (1 << 5)
/* (AIC) External Sources High-level Sensitive */
#define AIC_SRCTYPE_EXT_HIGH_LEVEL      (2 << 5)
/* (AIC) External Sources Positive Edge triggered */
#define AIC_SRCTYPE_EXT_POSITIVE_EDGE   (3 << 5)
/* Interrupt Source Type / Internal Source */
#define AIC_SRCTYPE_INT_LEVEL           (0 << 5)
#define AIC_SRCTYPE_INT_EDGE            (1 << 5)
/* (AIC) Internal Sources Code Label Level Sensitive */
#define AIC_SRCTYPE_INT_LEVEL_SENSITIVE  (0x0 <<  5) 
/* (AIC) Internal Sources Code Label Edge triggered */
#define AIC_SRCTYPE_INT_EDGE_TRIGGERED   (0x1 <<  5) 

#define AIC_SRCTYPE_INT_HIGH_LEVEL AIC_SRCTYPE_INT_LEVEL

/* AIC_CISR - AIC Core Interrupt Status Register */
#define AIC_NFIQ            (0x1 <<  0) /* (AIC) NFIQ Status */
#define AIC_NIRQ            (0x1 <<  1) /* (AIC) NIRQ Status */

/* AIC_DCR -  */
#define AIC_DCR_PROT        (0x1 <<  0) /* (AIC) Protection Mode */
#define AIC_DCR_GMSK        (0x1 <<  1) /* (AIC) General Mask */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_aic {
	uint32_t smr[32]; /* Source Mode Register */
	void * volatile svr[32]; /* Source Vector Register */
	void * volatile ivr; /* IRQ Vector Register */
	void * volatile fvr; /* FIQ Vector Register */
	volatile uint32_t isr; /* Interrupt Status Register */
	volatile uint32_t ipr; /* Interrupt Pending Register */
	volatile uint32_t imr; /* Interrupt Mask Register */
	volatile uint32_t cisr; /* Core Interrupt Status Register */
	uint32_t res0[2];
	volatile uint32_t iecr; /* Interrupt Enable Command Register */
	volatile uint32_t idcr; /* Interrupt Disable Command Register */
	uint32_t iccr; /* Interrupt Clear Command Register */
	volatile uint32_t iscr; /* Interrupt Set Command Register */
	volatile uint32_t eoicr; /* End of Interrupt Command Register */
	void * volatile spu; /* Spurious Vector Register */
	uint32_t dcr; /* Debug Control Register (Protect) */
	uint32_t res1;
	uint32_t ffer; /* Fast Forcing Enable Register */
	uint32_t ffdr; /* Fast Forcing Disable Register */
	uint32_t ffsr; /* Fast Forcing Status Register */
	uint32_t res2[941];
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_AIC_H__ */

