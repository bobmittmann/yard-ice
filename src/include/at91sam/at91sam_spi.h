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

#ifndef __AT91_SPI_H__
#define __AT91_SPI_H__

/* Serial Parallel Interface */

#define SPI_CR          ( 0) /* Control Register */
#define SPI_MR          ( 4) /* Mode Register */
#define SPI_RDR         ( 8) /* Receive Data Register */
#define SPI_TDR         (12) /* Transmit Data Register */
#define SPI_SR          (16) /* Status Register */
#define SPI_IER         (20) /* Interrupt Enable Register */
#define SPI_IDR         (24) /* Interrupt Disable Register */
#define SPI_IMR         (28) /* Interrupt Mask Register */
#define SPI_CSR         (48) /* Chip Select Register */
#define SPI_RPR         (256) /* Receive Pointer Register */
#define SPI_RCR         (260) /* Receive Counter Register */
#define SPI_TPR         (264) /* Transmit Pointer Register */
#define SPI_TCR         (268) /* Transmit Counter Register */
#define SPI_RNPR        (272) /* Receive Next Pointer Register */
#define SPI_RNCR        (276) /* Receive Next Counter Register */
#define SPI_TNPR        (280) /* Transmit Next Pointer Register */
#define SPI_TNCR        (284) /* Transmit Next Counter Register */
#define SPI_PTCR        (288) /* PDC Transfer Control Register */
#define SPI_PTSR        (292) /* PDC Transfer Status Register */

/* SPI_CR - SPI Control Register */
#define SPI_SPIEN           (0x1 <<  0) /* (SPI) SPI Enable */
#define SPI_SPIDIS          (0x1 <<  1) /* (SPI) SPI Disable */
#define SPI_SWRST           (0x1 <<  7) /* (SPI) SPI Software reset */
#define SPI_LASTXFER        (0x1 << 24) /* (SPI) SPI Last Transfer */

/* SPI_MR - SPI Mode Register */
#define SPI_MSTR            (0x1 <<  0) /* (SPI) Master/Slave Mode */
#define SPI_PS              (0x1 <<  1) /* (SPI) Peripheral Select */
#define 	SPI_PS_FIXED                (0x0 <<  1) /* (SPI) Fixed Peripheral Select */
#define 	SPI_PS_VARIABLE             (0x1 <<  1) /* (SPI) Variable Peripheral Select */
#define SPI_PCSDEC          (0x1 <<  2) /* (SPI) Chip Select Decode */
#define SPI_FDIV            (0x1 <<  3) /* (SPI) Clock Selection */
#define SPI_MODFDIS         (0x1 <<  4) /* (SPI) Mode Fault Detection */
#define SPI_LLB             (0x1 <<  7) /* (SPI) Clock Selection */
#define SPI_PCS             (0xf << 16) /* (SPI) Peripheral Chip Select */
#define SPI_DLYBCS          (0xff << 24) /* (SPI) Delay Between Chip Selects */

/* SPI_RDR - Receive Data Register */
#define SPI_RD              (0xffff <<  0) /* (SPI) Receive Data */
#define SPI_RPCS            (0xf << 16) /* (SPI) Peripheral Chip Select Status */

/* SPI_TDR - Transmit Data Register */
#define SPI_TD              (0xffff <<  0) /* (SPI) Transmit Data */
#define SPI_TPCS            (0xf << 16) /* (SPI) Peripheral Chip Select Status */

/* SPI_SR - Status Register */
#define SPI_RDRF            (0x1 <<  0) /* (SPI) Receive Data Register Full */
#define SPI_TDRE            (0x1 <<  1) /* (SPI) Transmit Data Register Empty */
#define SPI_MODF            (0x1 <<  2) /* (SPI) Mode Fault Error */
#define SPI_OVRES           (0x1 <<  3) /* (SPI) Overrun Error Status */
#define SPI_ENDRX           (0x1 <<  4) /* (SPI) End of Receiver Transfer */
#define SPI_ENDTX           (0x1 <<  5) /* (SPI) End of Receiver Transfer */
#define SPI_RXBUFF          (0x1 <<  6) /* (SPI) RXBUFF Interrupt */
#define SPI_TXBUFE          (0x1 <<  7) /* (SPI) TXBUFE Interrupt */
#define SPI_NSSR            (0x1 <<  8) /* (SPI) NSSR Interrupt */
#define SPI_TXEMPTY         (0x1 <<  9) /* (SPI) TXEMPTY Interrupt */
#define SPI_SPIENS          (0x1 << 16) /* (SPI) Enable Status */

/* SPI_IER - Interrupt Enable Register */

/* SPI_IDR - Interrupt Disable Register */

/* SPI_IMR - Interrupt Mask Register */

/* SPI_CSR - Chip Select Register */
#define SPI_CPOL            (0x1 <<  0) /* (SPI) Clock Polarity */
#define SPI_NCPHA           (0x1 <<  1) /* (SPI) Clock Phase */
#define SPI_CSAAT           (0x1 <<  3) /* (SPI) Chip Select Active After Transfer */
#define SPI_BITS            (0xf <<  4) /* (SPI) Bits Per Transfer */
#define 	SPI_BITS_8                    (0x0 <<  4) /* (SPI) 8 Bits Per transfer */
#define 	SPI_BITS_9                    (0x1 <<  4) /* (SPI) 9 Bits Per transfer */
#define 	SPI_BITS_10                   (0x2 <<  4) /* (SPI) 10 Bits Per transfer */
#define 	SPI_BITS_11                   (0x3 <<  4) /* (SPI) 11 Bits Per transfer */
#define 	SPI_BITS_12                   (0x4 <<  4) /* (SPI) 12 Bits Per transfer */
#define 	SPI_BITS_13                   (0x5 <<  4) /* (SPI) 13 Bits Per transfer */
#define 	SPI_BITS_14                   (0x6 <<  4) /* (SPI) 14 Bits Per transfer */
#define 	SPI_BITS_15                   (0x7 <<  4) /* (SPI) 15 Bits Per transfer */
#define 	SPI_BITS_16                   (0x8 <<  4) /* (SPI) 16 Bits Per transfer */
#define SPI_SCBR            (0xff <<  8) /* (SPI) Serial Clock Baud Rate */
#define SPI_DLYBS           (0xff << 16) /* (SPI) Delay Before SPCK */
#define SPI_DLYBCT          (0xff << 24) /* (SPI) Delay Between Consecutive Transfers */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_spi {
	volatile uint32_t cr; /* Control Register */
	volatile uint32_t mr; /* Mode Register */
	volatile uint32_t rdr; /* Receive Data Register */
	volatile uint32_t tdr; /* Transmit Data Register */
	volatile uint32_t sr; /* Status Register */
	volatile uint32_t ier; /* Interrupt Enable Register */
	volatile uint32_t idr; /* Interrupt Disable Register */
	volatile uint32_t imr; /* Interrupt Mask Register */
	volatile uint32_t csr; /* Chip Select Register */
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

#endif /* __AT91_SPI_H__ */

