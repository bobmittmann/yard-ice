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

#ifndef __AT91_SSC_H__
#define __AT91_SSC_H__


/* Synchronous Serial Controller Interface */

#define SSC_CR          ( 0) /* Control Register */
#define SSC_CMR         ( 4) /* Clock Mode Register */
#define SSC_RCMR        (16) /* Receive Clock ModeRegister */
#define SSC_RFMR        (20) /* Receive Frame Mode Register */
#define SSC_TCMR        (24) /* Transmit Clock Mode Register */
#define SSC_TFMR        (28) /* Transmit Frame Mode Register */
#define SSC_RHR         (32) /* Receive Holding Register */
#define SSC_THR         (36) /* Transmit Holding Register */
#define SSC_RSHR        (48) /* Receive Sync Holding Register */
#define SSC_TSHR        (52) /* Transmit Sync Holding Register */
#define SSC_SR          (64) /* Status Register */
#define SSC_IER         (68) /* Interrupt Enable Register */
#define SSC_IDR         (72) /* Interrupt Disable Register */
#define SSC_IMR         (76) /* Interrupt Mask Register */
#define SSC_RPR         (256) /* Receive Pointer Register */
#define SSC_RCR         (260) /* Receive Counter Register */
#define SSC_TPR         (264) /* Transmit Pointer Register */
#define SSC_TCR         (268) /* Transmit Counter Register */
#define SSC_RNPR        (272) /* Receive Next Pointer Register */
#define SSC_RNCR        (276) /* Receive Next Counter Register */
#define SSC_TNPR        (280) /* Transmit Next Pointer Register */
#define SSC_TNCR        (284) /* Transmit Next Counter Register */
#define SSC_PTCR        (288) /* PDC Transfer Control Register */
#define SSC_PTSR        (292) /* PDC Transfer Status Register */

/* SSC_CR - SSC Control Register */
#define SSC_RXEN            (0x1 <<  0) /* (SSC) Receive Enable */
#define SSC_RXDIS           (0x1 <<  1) /* (SSC) Receive Disable */
#define SSC_TXEN            (0x1 <<  8) /* (SSC) Transmit Enable */
#define SSC_TXDIS           (0x1 <<  9) /* (SSC) Transmit Disable */
#define SSC_SWRST           (0x1 << 15) /* (SSC) Software Reset */

/* SSC_RCMR - SSC Receive Clock Mode Register */
#define SSC_CKS             (0x3 <<  0) /* (SSC) Receive/Transmit Clock Selection */
#define 	SSC_CKS_DIV                  (0x0) /* (SSC) Divided Clock */
#define 	SSC_CKS_TK                   (0x1) /* (SSC) TK Clock signal */
#define 	SSC_CKS_RK                   (0x2) /* (SSC) RK pin */
#define SSC_CKO             (0x7 <<  2) /* (SSC) Receive/Transmit Clock Output Mode Selection */
#define 	SSC_CKO_NONE                 (0x0 <<  2) /* (SSC) Receive/Transmit Clock Output Mode: None RK pin: Input-only */
#define 	SSC_CKO_CONTINOUS            (0x1 <<  2) /* (SSC) Continuous Receive/Transmit Clock RK pin: Output */
#define 	SSC_CKO_DATA_TX              (0x2 <<  2) /* (SSC) Receive/Transmit Clock only during data transfers RK pin: Output */
#define SSC_CKI             (0x1 <<  5) /* (SSC) Receive/Transmit Clock Inversion */
#define SSC_START           (0xf <<  8) /* (SSC) Receive/Transmit Start Selection */
#define 	SSC_START_CONTINOUS            (0x0 <<  8) /* (SSC) Continuous, as soon as the receiver is enabled, and immediately after the end of transfer of the previous data. */
#define 	SSC_START_TX                   (0x1 <<  8) /* (SSC) Transmit/Receive start */
#define 	SSC_START_LOW_RF               (0x2 <<  8) /* (SSC) Detection of a low level on RF input */
#define 	SSC_START_HIGH_RF              (0x3 <<  8) /* (SSC) Detection of a high level on RF input */
#define 	SSC_START_FALL_RF              (0x4 <<  8) /* (SSC) Detection of a falling edge on RF input */
#define 	SSC_START_RISE_RF              (0x5 <<  8) /* (SSC) Detection of a rising edge on RF input */
#define 	SSC_START_LEVEL_RF             (0x6 <<  8) /* (SSC) Detection of any level change on RF input */
#define 	SSC_START_EDGE_RF              (0x7 <<  8) /* (SSC) Detection of any edge on RF input */
#define 	SSC_START_0                    (0x8 <<  8) /* (SSC) Compare 0 */
#define SSC_STTDLY          (0xff << 16) /* (SSC) Receive/Transmit Start Delay */
#define SSC_STTDLY_SET(X)   (((X) & 0xff) << 16)
#define SSC_STTDLY_GET(X)   (((RCMR >> 16) & 0xff)

#define SSC_PERIOD           (0xff << 24) /* (SSC) Receive/Transmit Period Divider Selection */
#define SSC_PERIOD_GET(RCMR) ((((RCMR) >> 24) & 0xff) + 1)
#define SSC_PERIOD_SET(X)    ((((X) - 1) & 0xff) << 24)

/* SSC_RFMR - SSC Receive Frame Mode Register */
#define SSC_DATLEN          (0x1f <<  0) /* (SSC) Data Length */
#define SSC_DATLEN_SET(X)  ((((X) - 1) & 0x1f) <<  0)
#define SSC_DATLEN_GET(X)  ((((RFMR) >> 0) & 0x1f) + 1)

#define SSC_LOOP            (0x1 <<  5) /* (SSC) Loop Mode */
#define SSC_MSBF            (0x1 <<  7) /* (SSC) Most Significant Bit First */

#define SSC_DATNB           (0xf <<  8) /* (SSC) Data Number per Frame */
#define SSC_DATNB_SET(X)    ((((X) - 1) & 0xf) <<  8)
#define SSC_DATNB_GET(X)    ((((RFMR) >> 8) & 0xf) + 1)

#define SSC_FSLEN           (0xf << 16) /* (SSC) Receive/Transmit Frame Sync length */
#define SSC_FSLEN_SET(X)  (((X) & 0xf) <<  16)
#define SSC_FSLEN_GET(X)  (((RFMR) >> 16) & 0xf)

#define SSC_FSOS            (0x7 << 20) /* (SSC) Receive/Transmit Frame Sync Output Selection */
#define 	SSC_FSOS_NONE                 (0x0 << 20) /* (SSC) Selected Receive/Transmit Frame Sync Signal: None RK pin Input-only */
#define 	SSC_FSOS_NEGATIVE             (0x1 << 20) /* (SSC) Selected Receive/Transmit Frame Sync Signal: Negative Pulse */
#define 	SSC_FSOS_POSITIVE             (0x2 << 20) /* (SSC) Selected Receive/Transmit Frame Sync Signal: Positive Pulse */
#define 	SSC_FSOS_LOW                  (0x3 << 20) /* (SSC) Selected Receive/Transmit Frame Sync Signal: Driver Low during data transfer */
#define 	SSC_FSOS_HIGH                 (0x4 << 20) /* (SSC) Selected Receive/Transmit Frame Sync Signal: Driver High during data transfer */
#define 	SSC_FSOS_TOGGLE               (0x5 << 20) /* (SSC) Selected Receive/Transmit Frame Sync Signal: Toggling at each start of data transfer */
#define SSC_FSEDGE          (0x1 << 24) /* (SSC) Frame Sync Edge Detection */

/* SSC_TCMR - SSC Transmit Clock Mode Register */
#define 	SSC_START_LOW_TF               (0x2 <<  8) /* (SSC) Detection of a low level on TF input */
#define 	SSC_START_HIGH_TF              (0x3 <<  8) /* (SSC) Detection of a high level on TF input */
#define 	SSC_START_FALL_TF              (0x4 <<  8) /* (SSC) Detection of a falling edge on TF input */
#define 	SSC_START_RISE_TF              (0x5 <<  8) /* (SSC) Detection of a rising edge on TF input */
#define 	SSC_START_LEVEL_TF             (0x6 <<  8) /* (SSC) Detection of any level change on TF input */
#define 	SSC_START_EDGE_TF              (0x7 <<  8) /* (SSC) Detection of any edge on TF input */

/* SSC_TFMR - SSC Transmit Frame Mode Register */
#define SSC_DATDEF          (0x1 <<  5) /* (SSC) Data Default Value */
#define SSC_FSDEN           (0x1 << 23) /* (SSC) Frame Sync Data Enable */

/* SSC_SR - SSC Status Register */
#define SSC_TXRDY           (0x1 <<  0) /* (SSC) Transmit Ready */
#define SSC_TXEMPTY         (0x1 <<  1) /* (SSC) Transmit Empty */
#define SSC_ENDTX           (0x1 <<  2) /* (SSC) End Of Transmission */
#define SSC_TXBUFE          (0x1 <<  3) /* (SSC) Transmit Buffer Empty */
#define SSC_RXRDY           (0x1 <<  4) /* (SSC) Receive Ready */
#define SSC_OVRUN           (0x1 <<  5) /* (SSC) Receive Overrun */
#define SSC_ENDRX           (0x1 <<  6) /* (SSC) End of Reception */
#define SSC_RXBUFF          (0x1 <<  7) /* (SSC) Receive Buffer Full */
#define SSC_TXSYN           (0x1 << 10) /* (SSC) Transmit Sync */
#define SSC_RXSYN           (0x1 << 11) /* (SSC) Receive Sync */
#define SSC_TXENA           (0x1 << 16) /* (SSC) Transmit Enable */
#define SSC_RXENA           (0x1 << 17) /* (SSC) Receive Enable */

/* SSC_IER - SSC Interrupt Enable Register */

/* SSC_IDR - SSC Interrupt Disable Register */

/* SSC_IMR - SSC Interrupt Mask Register */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_ssc {
	volatile uint32_t cr; /* Control Register */
	volatile uint32_t cmr; /* Clock Mode Register */
	uint32_t res0[2];
	volatile uint32_t rcmr; /* Receive Clock ModeRegister */
	volatile uint32_t rfmr; /* Receive Frame Mode Register */
	volatile uint32_t tcmr; /* Transmit Clock Mode Register */
	volatile uint32_t tfmr; /* Transmit Frame Mode Register */
	volatile uint32_t rhr; /* Receive Holding Register */
	volatile uint32_t thr; /* Transmit Holding Register */
	uint32_t res1[2];
	volatile uint32_t rshr; /* Receive Sync Holding Register */
	volatile uint32_t tshr; /* Transmit Sync Holding Register */
#if 0
	volatile uint32_t rc0r; /* Receive Compare 0 Register */
	volatile uint32_t rc1r; /* Receive Compare 1 Register */
#else
	uint32_t res2[2];
#endif
	volatile uint32_t sr; /* Status Register */
	volatile uint32_t ier; /* Interrupt Enable Register */
	volatile uint32_t idr; /* Interrupt Disable Register */
	volatile uint32_t imr; /* Interrupt Mask Register */
	uint32_t res3[44];
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


#endif /* __AT91_SSC_H__ */
