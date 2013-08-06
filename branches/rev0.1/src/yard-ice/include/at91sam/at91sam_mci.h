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

#ifndef __AT91_MCI_H__
#define __AT91_MCI_H__

/* Multimedia Card Interface */

#define MCI_CR          ( 0) /* MCI Control Register */
#define MCI_MR          ( 4) /* MCI Mode Register */
#define MCI_DTOR        ( 8) /* MCI Data Timeout Register */
#define MCI_SDCR        (12) /* MCI SD Card Register */
#define MCI_ARGR        (16) /* MCI Argument Register */
#define MCI_CMDR        (20) /* MCI Command Register */
#define MCI_BLKR        (24) /* MCI Block Register */
#define MCI_RSPR        (32) /* MCI Response Register */
#define MCI_RDR         (48) /* MCI Receive Data Register */
#define MCI_TDR         (52) /* MCI Transmit Data Register */
#define MCI_SR          (64) /* MCI Status Register */
#define MCI_IER         (68) /* MCI Interrupt Enable Register */
#define MCI_IDR         (72) /* MCI Interrupt Disable Register */
#define MCI_IMR         (76) /* MCI Interrupt Mask Register */
#define MCI_VR          (252) /* MCI Version Register */
#define MCI_RPR         (256) /* Receive Pointer Register */
#define MCI_RCR         (260) /* Receive Counter Register */
#define MCI_TPR         (264) /* Transmit Pointer Register */
#define MCI_TCR         (268) /* Transmit Counter Register */
#define MCI_RNPR        (272) /* Receive Next Pointer Register */
#define MCI_RNCR        (276) /* Receive Next Counter Register */
#define MCI_TNPR        (280) /* Transmit Next Pointer Register */
#define MCI_TNCR        (284) /* Transmit Next Counter Register */
#define MCI_PTCR        (288) /* PDC Transfer Control Register */
#define MCI_PTSR        (292) /* PDC Transfer Status Register */

/* MCI_CR - MCI Control Register */
#define MCI_MCIEN           (0x1 <<  0) /* (MCI) Multimedia Interface Enable */
#define MCI_MCIDIS          (0x1 <<  1) /* (MCI) Multimedia Interface Disable */
#define MCI_PWSEN           (0x1 <<  2) /* (MCI) Power Save Mode Enable */
#define MCI_PWSDIS          (0x1 <<  3) /* (MCI) Power Save Mode Disable */
#define MCI_SWRST           (0x1 <<  7) /* (MCI) MCI Software reset */

/* MCI_MR - MCI Mode Register */
#define MCI_CLKDIV          (0xff <<  0) /* (MCI) Clock Divider */
#define MCI_PWSDIV          (0x7 <<  8) /* (MCI) Power Saving Divider */
#define MCI_RDPROOF         (0x1 << 11) /* (MCI) Read Proof Enable */
#define MCI_WRPROOF         (0x1 << 12) /* (MCI) Write Proof Enable */
#define MCI_PDCFBYTE        (0x1 << 13) /* (MCI) PDC Force Byte Transfer */
#define MCI_PDCPADV         (0x1 << 14) /* (MCI) PDC Padding Value */
#define MCI_PDCMODE         (0x1 << 15) /* (MCI) PDC Oriented Mode */
#define MCI_BLKLEN          (0xffff << 16) /* (MCI) Data Block Length */

/* MCI_DTOR - MCI Data Timeout Register */
#define MCI_DTOCYC          (0xf <<  0) /* (MCI) Data Timeout Cycle Number */
#define MCI_DTOMUL          (0x7 <<  4) /* (MCI) Data Timeout Multiplier */
#define 	MCI_DTOMUL_1                    (0x0 <<  4) /* (MCI) DTOCYC x 1 */
#define 	MCI_DTOMUL_16                   (0x1 <<  4) /* (MCI) DTOCYC x 16 */
#define 	MCI_DTOMUL_128                  (0x2 <<  4) /* (MCI) DTOCYC x 128 */
#define 	MCI_DTOMUL_256                  (0x3 <<  4) /* (MCI) DTOCYC x 256 */
#define 	MCI_DTOMUL_1024                 (0x4 <<  4) /* (MCI) DTOCYC x 1024 */
#define 	MCI_DTOMUL_4096                 (0x5 <<  4) /* (MCI) DTOCYC x 4096 */
#define 	MCI_DTOMUL_65536                (0x6 <<  4) /* (MCI) DTOCYC x 65536 */
#define 	MCI_DTOMUL_1048576              (0x7 <<  4) /* (MCI) DTOCYC x 1048576 */

/* MCI_SDCR - MCI SD Card Register */
#define MCI_SCDSEL          (0x3 <<  0) /* (MCI) SD Card Selector */
#define MCI_SCDBUS          (0x1 <<  7) /* (MCI) SDCard/SDIO Bus Width */

/* MCI_CMDR - MCI Command Register */
#define MCI_CMDNB           (0x3f <<  0) /* (MCI) Command Number */
#define MCI_RSPTYP          (0x3 <<  6) /* (MCI) Response Type */
#define 	MCI_RSPTYP_NO                   (0x0 <<  6) /* (MCI) No response */
#define 	MCI_RSPTYP_48                   (0x1 <<  6) /* (MCI) 48-bit response */
#define 	MCI_RSPTYP_136                  (0x2 <<  6) /* (MCI) 136-bit response */
#define MCI_SPCMD           (0x7 <<  8) /* (MCI) Special CMD */
#define 	MCI_SPCMD_NONE                 (0x0 <<  8) /* (MCI) Not a special CMD */
#define 	MCI_SPCMD_INIT                 (0x1 <<  8) /* (MCI) Initialization CMD */
#define 	MCI_SPCMD_SYNC                 (0x2 <<  8) /* (MCI) Synchronized CMD */
#define 	MCI_SPCMD_IT_CMD               (0x4 <<  8) /* (MCI) Interrupt command */
#define 	MCI_SPCMD_IT_REP               (0x5 <<  8) /* (MCI) Interrupt response */
#define MCI_OPDCMD          (0x1 << 11) /* (MCI) Open Drain Command */
#define MCI_MAXLAT          (0x1 << 12) /* (MCI) Maximum Latency for Command to respond */
#define MCI_TRCMD           (0x3 << 16) /* (MCI) Transfer CMD */
#define 	MCI_TRCMD_NO                   (0x0 << 16) /* (MCI) No transfer */
#define 	MCI_TRCMD_START                (0x1 << 16) /* (MCI) Start transfer */
#define 	MCI_TRCMD_STOP                 (0x2 << 16) /* (MCI) Stop transfer */
#define MCI_TRDIR           (0x1 << 18) /* (MCI) Transfer Direction */
#define MCI_TRTYP           (0x7 << 19) /* (MCI) Transfer Type */
#define 	MCI_TRTYP_BLOCK                (0x0 << 19) /* (MCI) MMC/SDCard Single Block Transfer type */
#define 	MCI_TRTYP_MULTIPLE             (0x1 << 19) /* (MCI) MMC/SDCard Multiple Block transfer type */
#define 	MCI_TRTYP_STREAM               (0x2 << 19) /* (MCI) MMC Stream transfer type */
#define 	MCI_TRTYP_SDIO_BYTE            (0x4 << 19) /* (MCI) SDIO Byte transfer type */
#define 	MCI_TRTYP_SDIO_BLOCK           (0x5 << 19) /* (MCI) SDIO Block transfer type */
#define MCI_IOSPCMD         (0x3 << 24) /* (MCI) SDIO Special Command */
#define 	MCI_IOSPCMD_NONE                 (0x0 << 24) /* (MCI) NOT a special command */
#define 	MCI_IOSPCMD_SUSPEND              (0x1 << 24) /* (MCI) SDIO Suspend Command */
#define 	MCI_IOSPCMD_RESUME               (0x2 << 24) /* (MCI) SDIO Resume Command */

/* MCI_BLKR - MCI Block Register */
#define MCI_BCNT            (0xffff <<  0) /* (MCI) MMC/SDIO Block Count / SDIO Byte Count */

/* MCI_SR - MCI Status Register */
#define MCI_CMDRDY          (0x1 <<  0) /* (MCI) Command Ready flag */
#define MCI_RXRDY           (0x1 <<  1) /* (MCI) RX Ready flag */
#define MCI_TXRDY           (0x1 <<  2) /* (MCI) TX Ready flag */
#define MCI_BLKE            (0x1 <<  3) /* (MCI) Data Block Transfer Ended flag */
#define MCI_DTIP            (0x1 <<  4) /* (MCI) Data Transfer in Progress flag */
#define MCI_NOTBUSY         (0x1 <<  5) /* (MCI) Data Line Not Busy flag */
#define MCI_ENDRX           (0x1 <<  6) /* (MCI) End of RX Buffer flag */
#define MCI_ENDTX           (0x1 <<  7) /* (MCI) End of TX Buffer flag */
#define MCI_SDIOIRQA        (0x1 <<  8) /* (MCI) SDIO Interrupt for Slot A */
#define MCI_SDIOIRQB        (0x1 <<  9) /* (MCI) SDIO Interrupt for Slot B */
#define MCI_SDIOIRQC        (0x1 << 10) /* (MCI) SDIO Interrupt for Slot C */
#define MCI_SDIOIRQD        (0x1 << 11) /* (MCI) SDIO Interrupt for Slot D */
#define MCI_RXBUFF          (0x1 << 14) /* (MCI) RX Buffer Full flag */
#define MCI_TXBUFE          (0x1 << 15) /* (MCI) TX Buffer Empty flag */
#define MCI_RINDE           (0x1 << 16) /* (MCI) Response Index Error flag */
#define MCI_RDIRE           (0x1 << 17) /* (MCI) Response Direction Error flag */
#define MCI_RCRCE           (0x1 << 18) /* (MCI) Response CRC Error flag */
#define MCI_RENDE           (0x1 << 19) /* (MCI) Response End Bit Error flag */
#define MCI_RTOE            (0x1 << 20) /* (MCI) Response Time-out Error flag */
#define MCI_DCRCE           (0x1 << 21) /* (MCI) data CRC Error flag */
#define MCI_DTOE            (0x1 << 22) /* (MCI) Data timeout Error flag */
#define MCI_OVRE            (0x1 << 30) /* (MCI) Overrun flag */
#define MCI_UNRE            (0x1 << 31) /* (MCI) Underrun flag */

/* MCI_IER - MCI Interrupt Enable Register */

/* MCI_IDR - MCI Interrupt Disable Register */

/* MCI_IMR - MCI Interrupt Mask Register */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_mci {
	volatile uint32_t cr; /* MCI Control Register */
	volatile uint32_t mr; /* MCI Mode Register */
	volatile uint32_t dtor; /* MCI Data Timeout Register */
	volatile uint32_t sdcr; /* MCI SD Card Register */
	volatile uint32_t argr; /* MCI Argument Register */
	volatile uint32_t cmdr; /* MCI Command Register */
	volatile uint32_t blkr; /* MCI Block Register */
	volatile uint32_t rspr; /* MCI Response Register */
	volatile uint32_t rdr; /* MCI Receive Data Register */
	volatile uint32_t tdr; /* MCI Transmit Data Register */
	volatile uint32_t sr; /* MCI Status Register */
	volatile uint32_t ier; /* MCI Interrupt Enable Register */
	volatile uint32_t idr; /* MCI Interrupt Disable Register */
	volatile uint32_t imr; /* MCI Interrupt Mask Register */
	volatile uint32_t vr; /* MCI Version Register */
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


#endif /* __AT91_MCI_H__ */

