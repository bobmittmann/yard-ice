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

/*****************************************************************************
 USART: Universal Synchronous/Asynchronous Receiver/Transmitter
 *****************************************************************************/

/* Control Register US_CR - Write Only */
#define US_CR                  0x00 
/* Reset Receiver */
#define US_RSTRX                    (1 << 2)
/* Reset Transmitter */
#define US_RSTTX                    (1 << 3)
/* Receiver Enable */
#define US_RXEN                     (1 << 4)
/* Receiver Disable */
#define US_RXDIS                    (1 << 5)
/* Transmitter Enable */
#define US_TXEN                     (1 << 6)
/* Transmitter Disabled */
#define US_TXDIS                    (1 << 7)
/* Reset Status Bit */
#define US_RSTSTA                   (1 << 8)
/* Start Break */
#define US_STTBRK                   (1 << 9)
/* Stop Break */
#define US_STPBRK                   (1 << 10)
/* Start Time-Out */
#define US_STTTO                    (1 << 11)
/* Send Address */
#define US_SENDA                    (1 << 12)

/* Mode Register US_MR Read/Write */ 
#define US_MR                  0x04
/* Clock Selection */
//#define US_CLKS_MCK                 0
//#define US_CLKS_SCK                 2
#define US_CLKS_MASK                (~(3 << 4))
#define US_CLKS_MCK                 (0 << 4)
#define US_CLKS_MCK8                (1 << 4)
#define US_CLKS_SCK                 (2 << 4)
/* Character Length */
#define US_CHRL_MASK                (~(3 << 6))
#define US_CHRL_5                   (0 << 6)
#define US_CHRL_6                   (1 << 6)
#define US_CHRL_7                   (2 << 6)
#define US_CHRL_8                   (3 << 6)
/* Synchronous Mode Selection */
#define US_SYNC                     (0 << 8)
/* Parity Type */
#define US_PAR_MASK                 (~(7 << 9))
#define US_PAR_EVEN                 (0 << 9)
#define US_PAR_ODD                  (1 << 9)
#define US_PAR_SPACE                (2 << 9)
#define US_PAR_MARK                 (3 << 9)
#define US_PAR_NO                   (4 << 9)
#define US_PAR_NONE                 US_PAR_NO
#define US_PAR_MULTIDROP            (6 << 9)
/* Number of Stop Bits */
#define US_NBSTOP_MASK              (~(3 << 12))
#define US_NBSTOP_1                 (0 << 12)
#define US_NBSTOP_1_BIT             US_NBSTOP_1
#define US_NBSTOP_1_5               (1 << 12)
#define US_NBSTOP_2                 (2 << 12)
/* Channel Mode */
#define US_CHMODE_MASK              (~(3 << 14))
#define US_CHMODE_NORMAL            (0 << 14)
#define US_CHMODE_AUTOMATIC_ECHO    (1 << 14)
#define US_CHMODE_LOCAL_LOOPBACK    (2 << 14)
#define US_CHMODE_REMOTE_LOOPBACK   (3 << 14)
/* 9-bit Character Length */
#define US_MODE9                    (1 << 17)
/* Clock Output select */
#define US_CLKO                     (1 << 18)

/* Interrupt Enable Register US_IER - Write Only */
#define US_IER                 0x08 
/* Interrupt Disable Register US_IDR - Write Only */
#define US_IDR                 0x0c 
/* Interrupt Mask Register US_IMR - Read Only */
#define US_IMR                 0x10 
/* Channel Status Register US_CSR - Read Only */
#define US_CSR                 0x14
/* Enable/Disable/Mask RXRDY Interrupt */
#define US_RXRDY                    (1 << 0)
/* Enable/Disable/Mask TXRDY Interrupt */
#define US_TXRDY                    (1 << 1)
/* Enable/Disable/Mask Receiver Break Interrupt */
#define US_RXBRK                    (1 << 2)
/* Enable/Disable/Mask End Of Receiver Transfer Interrupt */
#define US_ENDRX                    (1 << 3)
/* Enable/Disable/Mask End Of Transmit Interrupt */
#define US_ENDTX                    (1 << 4)
/* Enable/Disable/Mask Overrun Error Interrupt */
#define US_OVRE                     (1 << 5)
/* Enable/Disable/Mask Framming Error Interrupt */
#define US_FRAME                    (1 << 6)
/* Enable/Disable/Mask Parity Error Interrupt */
#define US_PARE                     (1 << 7)
/* Enable/Disable/Mask Time-Out Interrupt */
#define US_TIMEOUT                  (1 << 8)
/* Enable/Disable/Mask TXEMPTY Interrupt */
#define US_TXEMPTY                  (1 << 9)

/* Receiver Holding Register US_RHR - Read Only */
#define US_RHR                 0x18
/* Transmitter Holding Register US_THR - Write Only */
#define US_THR                 0x1c
/* Baud Rate Generator Register US_BRGR - Read/Write */
#define US_BRGR                0x20
/* Receiver Time-out Register US_RTOR - Read/Write */
#define US_RTOR                0x24
/* Transmitter Time-guard Register US_TTGR - Read/Write */
#define US_TTGR                0x30
/* Receive Pointer Register US_RPR - Read/Write */
#define US_RPR                 0x30
/* Receive Counter Register US_RCR - Read/Write */
#define US_RCR                 0x34
/* Transmit Pointer Register US_TPR - Read/Write */ 
#define US_TPR                 0x38
/* Transmit Counter Register US_TCR - Read/Write */
#define US_TCR                 0x3c

#ifndef __ASSEMBLER__

struct at91_us {
	volatile uint32_t cr;
	uint32_t mr;
	volatile uint32_t ier;
	volatile uint32_t idr;
	volatile uint32_t imr;
	volatile uint32_t csr;
	volatile uint32_t rhr;
	uint32_t thr;
	uint32_t brgr;
	volatile uint32_t rtor;
	uint32_t ttgr;
	uint32_t res0;
	volatile void * rpr;
	volatile uint32_t rcr;
	volatile void * tpr;
	volatile uint32_t tcr;
	uint32_t unused[4080];
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_US_H__ */

