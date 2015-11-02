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

#ifndef __AT91_TWI_H__
#define __AT91_TWI_H__

/* Two-wire Interface */

#define TWI_CR          ( 0) /* Control Register */
#define TWI_MMR         ( 4) /* Master Mode Register */
#define TWI_IADR        (12) /* Internal Address Register */
#define TWI_CWGR        (16) /* Clock Waveform Generator Register */
#define TWI_SR          (32) /* Status Register */
#define TWI_IER         (36) /* Interrupt Enable Register */
#define TWI_IDR         (40) /* Interrupt Disable Register */
#define TWI_IMR         (44) /* Interrupt Mask Register */
#define TWI_RHR         (48) /* Receive Holding Register */
#define TWI_THR         (52) /* Transmit Holding Register */

/* TWI_CR - TWI Control Register */
#define TWI_START         (0x1 <<  0) /* (TWI) Send a START Condition */
#define TWI_STOP          (0x1 <<  1) /* (TWI) Send a STOP Condition */
#define TWI_MSEN          (0x1 <<  2) /* (TWI) TWI Master Transfer Enabled */
#define TWI_MSDIS         (0x1 <<  3) /* (TWI) TWI Master Transfer Disabled */
#define TWI_SWRST         (0x1 <<  7) /* (TWI) Software Reset */

/* TWI_MMR - TWI Master Mode Register */
#define TWI_IADRSZ        (0x3 <<  8) /* (TWI) Internal Device Address Size */
#define TWI_IADRSZ_NO     (0x0 <<  8) /* (TWI) No internal device address */
#define TWI_IADRSZ_1_BYTE (0x1 <<  8) /* (TWI) 1-byte internal device address */
#define TWI_IADRSZ_2_BYTE (0x2 <<  8) /* (TWI) 2-byte internal device address */
#define TWI_IADRSZ_3_BYTE (0x3 <<  8) /* (TWI) 3-byte internal device address */
#define TWI_MREAD         (0x1 << 12) /* (TWI) Master Read Direction */
#define TWI_MWRITE        (0x0 << 12) /* (TWI) Master Write Direction */
#define TWI_DADR(X)       (((X) & 0x7f) << 16) /* (TWI) Device Address */

/* TWI_CWGR - TWI Clock Waveform Generator Register */
#define TWI_CLDIV(X)    (((X) & 0xff) << 0) /* (TWI) Clock Low Divider */
#define TWI_CHDIV(X)    (((X) & 0xff) << 8) /* (TWI) Clock High Divider */
#define TWI_CKDIV(X)    (((X) & 0x7) << 16) /* (TWI) Clock Divider */

/* TWI_SR - TWI Status Register */
#define TWI_TXCOMP      (0x1 <<  0) /* (TWI) Transmission Completed */
#define TWI_RXRDY       (0x1 <<  1) /* (TWI) Receive holding register ReaDY */
#define TWI_TXRDY       (0x1 <<  2) /* (TWI) Transmit holding register ReaDY */
#define TWI_OVRE        (0x1 <<  6) /* (TWI) Overrun Error */
#define TWI_UNRE        (0x1 <<  7) /* (TWI) Underrun Error */
#define TWI_NACK        (0x1 <<  8) /* (TWI) Not Acknowledged */

/* TWI_IER - TWI Interrupt Enable Register */

/* TWI_IDR - TWI Interrupt Disable Register */

/* TWI_IMR - TWI Interrupt Mask Register */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_twi {
	volatile uint32_t cr; /* Control Register */
	volatile uint32_t mmr; /* Master Mode Register */
	volatile uint32_t smr; /* Slave Mode Register */
	volatile uint32_t iadr; /* Internal Address Register */
	volatile uint32_t cwgr; /* Clock Waveform Generator Register */
	uint32_t res1[3];
	volatile uint32_t sr; /* Status Register */
	volatile uint32_t ier; /* Interrupt Enable Register */
	volatile uint32_t idr; /* Interrupt Disable Register */
	volatile uint32_t imr; /* Interrupt Mask Register */
	volatile uint32_t rhr; /* Receive Holding Register */
	volatile uint32_t thr; /* Transmit Holding Register */
	uint32_t res2[50];
	volatile uint32_t rpr; /* Receive Pointer Register */
	volatile uint32_t rcr; /* Receive Counter Register */
	volatile uint32_t tpr; /* Transmit Pointer Register */
	volatile uint32_t tcr; /* Transmit Counter Register */
	volatile uint32_t rnpr; /* Receive Next Pointer Register */
	volatile uint32_t rncr; /* Receive Next Counter Register */
	volatile uint32_t tnpr; /* Transmit Next Pointer Register */
	volatile uint32_t tncr; /* Transmit Next Counter Register */
	volatile uint32_t ptcr; /* PDC Transfer Control Register */
	volatile uint32_t ptsr; /* PDC Transfer Status Register */
};

#endif /* __ASSEMBLER__ */


#endif /* __AT91_TWI_H__ */
