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

#ifndef __AT91_UDP_H__
#define __AT91_UDP_H__

/* USB Device Interface */

#define UDP_NUM         ( 0) /* Frame Number Register */
#define UDP_GLBSTATE    ( 4) /* Global State Register */
#define UDP_FADDR       ( 8) /* Function Address Register */
#define UDP_IER         (16) /* Interrupt Enable Register */
#define UDP_IDR         (20) /* Interrupt Disable Register */
#define UDP_IMR         (24) /* Interrupt Mask Register */
#define UDP_ISR         (28) /* Interrupt Status Register */
#define UDP_ICR         (32) /* Interrupt Clear Register */
#define UDP_RSTEP       (40) /* Reset Endpoint Register */
#define UDP_CSR         (48) /* Endpoint Control and Status Register */
#define UDP_FDR         (80) /* Endpoint FIFO Data Register */
#define UDP_TXVC        (116) /* Transceiver Control Register */

/* UDP_FRM_NUM - USB Frame Number Register */
#define UDP_FRM_NUM         (0x7ff <<  0) /* (UDP) Frame Number as Defined in the Packet Field Formats */
#define UDP_FRM_ERR         (0x1 << 16) /* (UDP) Frame Error */
#define UDP_FRM_OK          (0x1 << 17) /* (UDP) Frame OK */

/* UDP_GLB_STATE - USB Global State Register */
#define UDP_FADDEN          (0x1 <<  0) /* (UDP) Function Address Enable */
#define UDP_CONFG           (0x1 <<  1) /* (UDP) Configured */
#define UDP_ESR             (0x1 <<  2) /* (UDP) Enable Send Resume */
#define UDP_RSMINPR         (0x1 <<  3) /* (UDP) A Resume Has Been Sent to the Host */
#define UDP_RMWUPE          (0x1 <<  4) /* (UDP) Remote Wake Up Enable */

/* UDP_FADDR - USB Function Address Register */
#define UDP_FADD            (0xff <<  0) /* (UDP) Function Address Value */
#define UDP_FEN             (0x1 <<  8) /* (UDP) Function Enable */

/* UDP_IER - USB Interrupt Enable Register */
#define UDP_EPINT0          (0x1 <<  0) /* (UDP) Endpoint 0 Interrupt */
#define UDP_EPINT1          (0x1 <<  1) /* (UDP) Endpoint 0 Interrupt */
#define UDP_EPINT2          (0x1 <<  2) /* (UDP) Endpoint 2 Interrupt */
#define UDP_EPINT3          (0x1 <<  3) /* (UDP) Endpoint 3 Interrupt */
#define UDP_EPINT4          (0x1 <<  4) /* (UDP) Endpoint 4 Interrupt */
#define UDP_EPINT5          (0x1 <<  5) /* (UDP) Endpoint 5 Interrupt */
#define UDP_RXSUSP          (0x1 <<  8) /* (UDP) USB Suspend Interrupt */
#define UDP_RXRSM           (0x1 <<  9) /* (UDP) USB Resume Interrupt */
#define UDP_EXTRSM          (0x1 << 10) /* (UDP) USB External Resume Interrupt */
#define UDP_SOFINT          (0x1 << 11) /* (UDP) USB Start Of frame Interrupt */
#define UDP_WAKEUP          (0x1 << 13) /* (UDP) USB Resume Interrupt */

/* UDP_IDR - USB Interrupt Disable Register */

/* UDP_IMR - USB Interrupt Mask Register */

/* UDP_ISR - USB Interrupt Status Register */
#define UDP_ENDBUSRES       (0x1 << 12) /* (UDP) USB End Of Bus Reset Interrupt */

/* UDP_ICR - USB Interrupt Clear Register */

/* UDP_RST_EP - USB Reset Endpoint Register */
#define UDP_EP0             (0x1 <<  0) /* (UDP) Reset Endpoint 0 */
#define UDP_EP1             (0x1 <<  1) /* (UDP) Reset Endpoint 1 */
#define UDP_EP2             (0x1 <<  2) /* (UDP) Reset Endpoint 2 */
#define UDP_EP3             (0x1 <<  3) /* (UDP) Reset Endpoint 3 */
#define UDP_EP4             (0x1 <<  4) /* (UDP) Reset Endpoint 4 */
#define UDP_EP5             (0x1 <<  5) /* (UDP) Reset Endpoint 5 */

/* UDP_CSR - USB Endpoint Control and Status Register */
#define UDP_TXCOMP          (0x1 <<  0) /* (UDP) Generates an IN packet with data previously written in the DPR */
#define UDP_RX_DATA_BK0     (0x1 <<  1) /* (UDP) Receive Data Bank 0 */
#define UDP_RXSETUP         (0x1 <<  2) /* (UDP) Sends STALL to the Host (Control endpoints) */
#define UDP_ISOERROR        (0x1 <<  3) /* (UDP) Isochronous error (Isochronous endpoints) */
#define UDP_TXPKTRDY        (0x1 <<  4) /* (UDP) Transmit Packet Ready */
#define UDP_FORCESTALL      (0x1 <<  5) /* (UDP) Force Stall (used by Control, Bulk and Isochronous endpoints). */
#define UDP_RX_DATA_BK1     (0x1 <<  6) /* (UDP) Receive Data Bank 1 (only used by endpoints with ping-pong attributes). */
#define UDP_DIR             (0x1 <<  7) /* (UDP) Transfer Direction */
#define UDP_EPTYPE          (0x7 <<  8) /* (UDP) Endpoint type */
#define 	UDP_EPTYPE_CTRL                 (0x0 <<  8) /* (UDP) Control */
#define 	UDP_EPTYPE_ISO_OUT              (0x1 <<  8) /* (UDP) Isochronous OUT */
#define 	UDP_EPTYPE_BULK_OUT             (0x2 <<  8) /* (UDP) Bulk OUT */
#define 	UDP_EPTYPE_INT_OUT              (0x3 <<  8) /* (UDP) Interrupt OUT */
#define 	UDP_EPTYPE_ISO_IN               (0x5 <<  8) /* (UDP) Isochronous IN */
#define 	UDP_EPTYPE_BULK_IN              (0x6 <<  8) /* (UDP) Bulk IN */
#define 	UDP_EPTYPE_INT_IN               (0x7 <<  8) /* (UDP) Interrupt IN */
#define UDP_DTGLE           (0x1 << 11) /* (UDP) Data Toggle */
#define UDP_EPEDS           (0x1 << 15) /* (UDP) Endpoint Enable Disable */

#define UDP_RXBYTECNT(CSR)  ((CSR) >> 16) /* (UDP) Number Of Bytes Available in the FIFO */

/* UDP_TXVC - Transceiver Control Register */
#define UDP_TXVDIS          (0x1 <<  8) /* (UDP) */
#define UDP_PUON            (0x1 <<  9) /* (UDP) Pull-up ON */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_udp {
	volatile uint32_t frm_num; /* Frame Number Register */
	volatile uint32_t glb_stat; /* Global State Register */
	volatile uint32_t faddr; /* Function Address Register */
	uint32_t res0[1];
	volatile uint32_t ier; /* Interrupt Enable Register */
	volatile uint32_t idr; /* Interrupt Disable Register */
	volatile uint32_t imr; /* Interrupt Mask Register */
	volatile uint32_t isr; /* Interrupt Status Register */
	volatile uint32_t icr; /* Interrupt Clear Register */
	uint32_t res1[1]; 
	volatile uint32_t rst_ep; /* Reset Endpoint Register */
	uint32_t res2[1]; 
	volatile uint32_t csr[8]; /* Endpoint Control and Status Register */
	volatile uint32_t fdr[8]; /* Endpoint FIFO Data Register */
	uint32_t res3[1]; 
	volatile uint32_t txvc; /* Transceiver Control Register */
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_UDP_H__ */

