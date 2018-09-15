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

#ifndef __AT91_PDC_H__
#define __AT91_PDC_H__

/* Peripheral DMA Controller */

#define PDC_RPR         ( 0) /* Receive Pointer Register */
#define PDC_RCR         ( 4) /* Receive Counter Register */
#define PDC_TPR         ( 8) /* Transmit Pointer Register */
#define PDC_TCR         (12) /* Transmit Counter Register */
#define PDC_RNPR        (16) /* Receive Next Pointer Register */
#define PDC_RNCR        (20) /* Receive Next Counter Register */
#define PDC_TNPR        (24) /* Transmit Next Pointer Register */
#define PDC_TNCR        (28) /* Transmit Next Counter Register */
#define PDC_PTCR        (32) /* PDC Transfer Control Register */
#define PDC_PTSR        (36) /* PDC Transfer Status Register */

/* PDC_PTCR - PDC Transfer Control Register */
#define PDC_RXTEN           (0x1 <<  0) /* (PDC) Receiver Transfer Enable */
#define PDC_RXTDIS          (0x1 <<  1) /* (PDC) Receiver Transfer Disable */
#define PDC_TXTEN           (0x1 <<  8) /* (PDC) Transmitter Transfer Enable */
#define PDC_TXTDIS          (0x1 <<  9) /* (PDC) Transmitter Transfer Disable */

/* PDC_PTSR - PDC Transfer Status Register */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91s_pdc {
	volatile uint32_t pdc_rpr; /* Receive Pointer Register */
	volatile uint32_t pdc_rcr; /* Receive Counter Register */
	volatile uint32_t pdc_tpr; /* Transmit Pointer Register */
	volatile uint32_t pdc_tcr; /* Transmit Counter Register */
	volatile uint32_t pdc_rnpr; /* Receive Next Pointer Register */
	volatile uint32_t pdc_rncr; /* Receive Next Counter Register */
	volatile uint32_t pdc_tnpr; /* Transmit Next Pointer Register */
	volatile uint32_t pdc_tncr; /* Transmit Next Counter Register */
	volatile uint32_t pdc_ptcr; /* PDC Transfer Control Register */
	volatile uint32_t pdc_ptsr; /* PDC Transfer Status Register */
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_PDC_H__ */

