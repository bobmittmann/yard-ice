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

#ifndef __AT91_US_H__
#define __AT91_US_H__

/* DBGU_CR - Debug Unit Control Register */
#define US_RSTRX            (0x1 <<  2) /* Reset Receiver */
#define US_RSTTX            (0x1 <<  3) /* Reset Transmitter */
#define US_RXEN             (0x1 <<  4) /* Receiver Enable */
#define US_RXDIS            (0x1 <<  5) /* Receiver Disable */
#define US_TXEN             (0x1 <<  6) /* Transmitter Enable */
#define US_TXDIS            (0x1 <<  7) /* Transmitter Disable */
#define US_RSTSTA           (0x1 <<  8) /* Reset Status Bits */

/* DBGU_MR - Debug Unit Mode Register */
#define US_PAR              (0x7 <<  9) /* Parity type */
#define US_PAR_EVEN         (0x0 <<  9) /* Even Parity */
#define US_PAR_ODD          (0x1 <<  9) /* Odd Parity */
#define US_PAR_SPACE        (0x2 <<  9) /* Parity forced to 0 (Space) */
#define US_PAR_MARK         (0x3 <<  9) /* Parity forced to 1 (Mark) */
#define US_PAR_NONE         (0x4 <<  9) /* No Parity */
#define US_PAR_MULTI_DROP   (0x6 <<  9) /* Multi-drop mode */

/* DBGU_IER - Debug Unit Interrupt Enable Register */
#define US_RXRDY            (0x1 <<  0) /* RXRDY Interrupt */
#define US_TXRDY            (0x1 <<  1) /* TXRDY Interrupt */
#define US_ENDRX            (0x1 <<  3) /* End of Receive Transfer Interrupt */
#define US_ENDTX            (0x1 <<  4) /* End of Transmit Interrupt */
#define US_OVRE             (0x1 <<  5) /* Overrun Interrupt */
#define US_FRAME            (0x1 <<  6) /* Framing Error Interrupt */
#define US_PARE             (0x1 <<  7) /* Parity Error Interrupt */
#define US_TXEMPTY          (0x1 <<  9) /* TXEMPTY Interrupt */
#define US_TXBUFE           (0x1 << 11) /* TXBUFE Interrupt */
#define US_RXBUFF           (0x1 << 12) /* RXBUFF Interrupt */
#define US_COMM_TX          (0x1 << 30) /* COMM_TX Interrupt */
#define US_COMM_RX          (0x1 << 31) /* COMM_RX Interrupt */

/* DBGU_IDR - Debug Unit Interrupt Disable Register */

/* DBGU_IMR - Debug Unit Interrupt Mask Register */

/* DBGU_CSR - Debug Unit Channel Status Register */

/* DBGU_FNTR - Debug Unit FORCE_NTRST Register */
#define US_FORCE_NTRST      (0x1 <<  0) /* Force NTRST in JTAG */


/* Usart */

#define US_CR           ( 0) /* Control Register */
#define US_MR           ( 4) /* Mode Register */
#define US_IER          ( 8) /* Interrupt Enable Register */
#define US_IDR          (12) /* Interrupt Disable Register */
#define US_IMR          (16) /* Interrupt Mask Register */
#define US_CSR          (20) /* Channel Status Register */
#define US_RHR          (24) /* Receiver Holding Register */
#define US_THR          (28) /* Transmitter Holding Register */
#define US_BRGR         (32) /* Baud Rate Generator Register */
#define US_RTOR         (36) /* Receiver Time-out Register */
#define US_TTGR         (40) /* Transmitter Time-guard Register */

#define US_FIDI         (64) /* FI_DI_Ratio Register */
#define US_NER          (68) /* Nb Errors Register */

#define US_IF           (76) /* IRDA_FILTER Register */

#define US_RPR          (256) /* Receive Pointer Register */
#define US_RCR          (260) /* Receive Counter Register */
#define US_TPR          (264) /* Transmit Pointer Register */
#define US_TCR          (268) /* Transmit Counter Register */
#define US_RNPR         (272) /* Receive Next Pointer Register */
#define US_RNCR         (276) /* Receive Next Counter Register */
#define US_TNPR         (280) /* Transmit Next Pointer Register */
#define US_TNCR         (284) /* Transmit Next Counter Register */
#define US_PTCR         (288) /* PDC Transfer Control Register */
#define US_PTSR         (292) /* PDC Transfer Status Register */

/* US_CR - Debug Unit Control Register */
#define US_STTBRK           (0x1 <<  9) /* Start Break */
#define US_STPBRK           (0x1 << 10) /* Stop Break */
#define US_STTTO            (0x1 << 11) /* Start Time-out */
#define US_SENDA            (0x1 << 12) /* Send Address */
#define US_RSTIT            (0x1 << 13) /* Reset Iterations */
#define US_RSTNACK          (0x1 << 14) /* Reset Non Acknowledge */
#define US_RETTO            (0x1 << 15) /* Rearm Time-out */
#define US_DTREN            (0x1 << 16) /* Data Terminal ready Enable */
#define US_DTRDIS           (0x1 << 17) /* Data Terminal ready Disable */
#define US_RTSEN            (0x1 << 18) /* Request to Send enable */
#define US_RTSDIS           (0x1 << 19) /* Request to Send Disable */

/* US_MR - Debug Unit Mode Register */
#define US_USMODE               (0xf <<  0) /* Usart mode */
#define US_USMODE_NORMAL    (0x0) /* Normal */
#define US_USMODE_RS485     (0x1) /* RS485 */
#define US_USMODE_HWHSH     (0x2) /* Hardware Handshaking */
#define US_USMODE_MODEM     (0x3) /* Modem */
#define US_USMODE_ISO7816_0 (0x4) /* ISO7816 protocol: T = 0 */
#define US_USMODE_ISO7816_1 (0x6) /* ISO7816 protocol: T = 1 */
#define US_USMODE_IRDA      (0x8) /* IrDA */
#define US_USMODE_SWHSH     (0xc) /* Software Handshaking */

/* Clock Selection (Baud Rate Generator Input Clock) */
#define US_CLKS             (0x3 <<  4) 
#define US_CLKS_CLOCK   (0x0 <<  4) /* Clock */
#define US_CLKS_FDIV1   (0x1 <<  4) /* fdiv1 */
#define US_CLKS_SLOW    (0x2 <<  4) /* slow_clock (ARM) */
#define US_CLKS_EXT     (0x3 <<  4) /* External (SCK) */
 
/* Character Length Selection */
#define US_CHRL(N)      ((((N) - 5) & 0x3) <<  6) 
#define US_CHRL_5_BITS  (0x0 <<  6) /* Character Length: 5 bits */
#define US_CHRL_6_BITS  (0x1 <<  6) /* Character Length: 6 bits */
#define US_CHRL_7_BITS  (0x2 <<  6) /* Character Length: 7 bits */
#define US_CHRL_8_BITS  (0x3 <<  6) /* Character Length: 8 bits */

#define US_CHRL_5 US_CHRL_5_BITS
#define US_CHRL_6 US_CHRL_6_BITS
#define US_CHRL_7 US_CHRL_7_BITS
#define US_CHRL_8 US_CHRL_8_BITS

#define US_SYNC             (0x1 <<  8) /* Synchronous Mode Select */
#define US_NBSTOP           (0x3 << 12) /* Number of Stop bits */
#define US_NBSTOP_1_BIT     (0x0 << 12) /* 1 stop bit */
#define US_NBSTOP_15_BIT    (0x1 << 12) /* Asynchronous (SYNC=0) 2 stop bits Synchronous (SYNC=1) 2 stop bits */
#define US_NBSTOP_2_BIT     (0x2 << 12) /* 2 stop bits */
#define US_MSBF             (0x1 << 16) /* Bit Order */
#define US_MODE9            (0x1 << 17) /* 9-bit Character length */
#define US_CKLO             (0x1 << 18) /* Clock Output Select */
#define US_OVER             (0x1 << 19) /* Over Sampling Mode */
#define US_INACK            (0x1 << 20) /* Inhibit Non Acknowledge */
#define US_DSNACK           (0x1 << 21) /* Disable Successive NACK */
#define US_MAX_ITER         (0x1 << 24) /* Number of Repetitions */
#define US_FILTER           (0x1 << 28) /* Receive Line Filter */

/* US_IER - Debug Unit Interrupt Enable Register */
#define US_RXBRK            (0x1 <<  2) /* Break Received/End of Break */
#define US_TIMEOUT          (0x1 <<  8) /* Receiver Time-out */
#define US_ITERATION        (0x1 << 10) /* Max number of Repetitions Reached */
#define US_NACK             (0x1 << 13) /* Non Acknowledge */
#define US_RIIC             (0x1 << 16) /* Ring INdicator Input Change Flag */
#define US_DSRIC            (0x1 << 17) /* Data Set Ready Input Change Flag */
#define US_DCDIC            (0x1 << 18) /* Data Carrier Flag */
#define US_CTSIC            (0x1 << 19) /* Clear To Send Input Change Flag */

/* US_IDR - Debug Unit Interrupt Disable Register */

/* US_IMR - Debug Unit Interrupt Mask Register */

/* US_CSR - Debug Unit Channel Status Register */
#define US_RI               (0x1 << 20) /* Image of RI Input */
#define US_DSR              (0x1 << 21) /* Image of DSR Input */
#define US_DCD              (0x1 << 22) /* Image of DCD Input */
#define US_CTS              (0x1 << 23) /* Image of CTS Input */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_us {
	volatile uint32_t cr; /* Control Register */
	uint32_t mr; /* Mode Register */
	volatile uint32_t ier; /* Interrupt Enable Register */
	volatile uint32_t idr; /* Interrupt Disable Register */
	volatile uint32_t imr; /* Interrupt Mask Register */
	volatile uint32_t csr; /* Channel Status Register */
	volatile uint32_t rhr; /* Receiver Holding Register */
	volatile uint32_t thr; /* Transmitter Holding Register */
	volatile uint32_t brgr; /* Baud Rate Generator Register */
	volatile uint32_t rtor; /* Receiver Time-out Register */
	volatile uint32_t ttgr; /* Transmitter Time-guard Register */
	uint32_t res0[5];
	volatile uint32_t fidi; /* FI_DI_Ratio Register */
	volatile uint32_t ner; /* Nb Errors Register */
	uint32_t res1;
	volatile uint32_t ifr; /* IRDA_FILTER Register */
	uint32_t res2[44];
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

#endif /* __AT91_US_H__ */
