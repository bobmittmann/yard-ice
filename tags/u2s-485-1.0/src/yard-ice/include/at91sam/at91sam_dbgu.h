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

#ifndef __AT91_DBGU_H__
#define __AT91_DBGU_H__

#include <at91sam/at91sam_us.h>

/* Debug Unit */

#define DBGU_CR         ( 0) /* Control Register */
#define DBGU_MR         ( 4) /* Mode Register */
#define DBGU_IER        ( 8) /* Interrupt Enable Register */
#define DBGU_IDR        (12) /* Interrupt Disable Register */
#define DBGU_IMR        (16) /* Interrupt Mask Register */
#define DBGU_CSR        (20) /* Channel Status Register */
#define DBGU_RHR        (24) /* Receiver Holding Register */
#define DBGU_THR        (28) /* Transmitter Holding Register */
#define DBGU_BRGR       (32) /* Baud Rate Generator Register */

#define DBGU_CIDR       (64) /* Chip ID Register */

#define CIDR_VER(CIDR) (((CIDR) >> 0) & 0x1f)
#define CIDR_EPROC(CIDR) (((CIDR) >> 5) & 0x03)
#define CIDR_NVPSIZ(CIDR) (((CIDR) >> 8) & 0x0f)
#define CIDR_NVPSIZ2(CIDR) (((CIDR) >> 12) & 0x0f)
#define CIDR_SRAMSIZ(CIDR) (((CIDR) >> 16) & 0x0f)
#define CIDR_ARCH(CIDR) (((CIDR) >> 20) & 0xff)
#define CIDR_NVPTYP(CIDR) (((CIDR) >> 28) & 0x03)
#define CIDR_EXT(CIDR) (((CIDR) >> 31) & 0x01)

#define DBGU_EXID       (68) /* Chip ID Extension Register */
#define DBGU_FNTR       (72) /* Force NTRST Register */

#define DBGU_RPR        (256) /* Receive Pointer Register */
#define DBGU_RCR        (260) /* Receive Counter Register */
#define DBGU_TPR        (264) /* Transmit Pointer Register */
#define DBGU_TCR        (268) /* Transmit Counter Register */
#define DBGU_RNPR       (272) /* Receive Next Pointer Register */
#define DBGU_RNCR       (276) /* Receive Next Counter Register */
#define DBGU_TNPR       (280) /* Transmit Next Pointer Register */
#define DBGU_TNCR       (284) /* Transmit Next Counter Register */
#define DBGU_PTCR       (288) /* PDC Transfer Control Register */
#define DBGU_PTSR       (292) /* PDC Transfer Status Register */


/* DBGU_IDR - Debug Unit Interrupt Disable Register */

/* DBGU_IMR - Debug Unit Interrupt Mask Register */

/* DBGU_CSR - Debug Unit Channel Status Register */

/* DBGU_FNTR - Debug Unit FORCE_NTRST Register */
#define AT91C_US_FORCE_NTRST      (0x1 <<  0) /* (DBGU) Force NTRST in JTAG */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_dbgu {
	volatile uint32_t cr; /* Control Register */
	uint32_t mr; /* Mode Register */
	volatile uint32_t ier; /* Interrupt Enable Register */
	volatile uint32_t idr; /* Interrupt Disable Register */
	volatile uint32_t imr; /* Interrupt Mask Register */
	volatile uint32_t csr; /* Channel Status Register */
	volatile uint32_t rhr; /* Receiver Holding Register */
	volatile uint32_t thr; /* Transmitter Holding Register */
	volatile uint32_t brgr; /* Baud Rate Generator Register */
	uint32_t res0[7]; 
	volatile uint32_t cidr; /* Chip ID Register */
	volatile uint32_t exid; /* Chip ID Extension Register */
	volatile uint32_t fntr; /* Force NTRST Register */
	uint32_t res1[45]; 
	volatile void * rpr; /* Receive Pointer Register */
	volatile uint32_t rcr; /* Receive Counter Register */
	volatile void * tpr; /* Transmit Pointer Register */
	volatile uint32_t tcr; /* Transmit Counter Register */
	volatile void * rnpr; /* Receive Next Pointer Register */
	volatile uint32_t rncr; /* Receive Next Counter Register */
	volatile void * tnpr; /* Transmit Next Pointer Register */
	volatile uint32_t tncr; /* Transmit Next Counter Register */
	volatile uint32_t ptcr; /* PDC Transfer Control Register */
	volatile uint32_t ptsr; /* PDC Transfer Status Register */
};
#endif /* __ASSEMBLER__ */

#endif /* __AT91C_DBGU_H__ */

