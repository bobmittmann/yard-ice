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

#ifndef __AT91_EMAC_H__
#define __AT91_EMAC_H__

/* Ethernet MAC 10/100 */

#define EMAC_NCR        ( 0) /* Network Control Register */
#define EMAC_NCFGR      ( 4) /* Network Configuration Register */
#define EMAC_NSR        ( 8) /* Network Status Register */
#define EMAC_TSR        (20) /* Transmit Status Register */
#define EMAC_RBQP       (24) /* Receive Buffer Queue Pointer */
#define EMAC_TBQP       (28) /* Transmit Buffer Queue Pointer */
#define EMAC_RSR        (32) /* Receive Status Register */
#define EMAC_ISR        (36) /* Interrupt Status Register */
#define EMAC_IER        (40) /* Interrupt Enable Register */
#define EMAC_IDR        (44) /* Interrupt Disable Register */
#define EMAC_IMR        (48) /* Interrupt Mask Register */
#define EMAC_MAN        (52) /* PHY Maintenance Register */
#define EMAC_PTR        (56) /* Pause Time Register */
#define EMAC_PFR        (60) /* Pause Frames received Register */
#define EMAC_FTO        (64) /* Frames Transmitted OK Register */
#define EMAC_SCF        (68) /* Single Collision Frame Register */
#define EMAC_MCF        (72) /* Multiple Collision Frame Register */
#define EMAC_FRO        (76) /* Frames Received OK Register */
#define EMAC_FCSE       (80) /* Frame Check Sequence Error Register */
#define EMAC_ALE        (84) /* Alignment Error Register */
#define EMAC_DTF        (88) /* Deferred Transmission Frame Register */
#define EMAC_LCOL       (92) /* Late Collision Register */
#define EMAC_ECOL       (96) /* Excessive Collision Register */
#define EMAC_TUND       (100) /* Transmit Underrun Error Register */
#define EMAC_CSE        (104) /* Carrier Sense Error Register */
#define EMAC_RRE        (108) /* Receive Ressource Error Register */
#define EMAC_ROV        (112) /* Receive Overrun Errors Register */
#define EMAC_RSE        (116) /* Receive Symbol Errors Register */
#define EMAC_ELE        (120) /* Excessive Length Errors Register */
#define EMAC_RJA        (124) /* Receive Jabbers Register */
#define EMAC_USF        (128) /* Undersize Frames Register */
#define EMAC_STE        (132) /* SQE Test Error Register */
#define EMAC_RLE        (136) /* Receive Length Field Mismatch Register */
#define EMAC_TPF        (140) /* Transmitted Pause Frames Register */
#define EMAC_HRB        (144) /* Hash Address Bottom[31:0] */
#define EMAC_HRT        (148) /* Hash Address Top[63:32] */
#define EMAC_SA1L       (152) /* Specific Address 1 Bottom, First 4 bytes */
#define EMAC_SA1H       (156) /* Specific Address 1 Top, Last 2 bytes */
#define EMAC_SA2L       (160) /* Specific Address 2 Bottom, First 4 bytes */
#define EMAC_SA2H       (164) /* Specific Address 2 Top, Last 2 bytes */
#define EMAC_SA3L       (168) /* Specific Address 3 Bottom, First 4 bytes */
#define EMAC_SA3H       (172) /* Specific Address 3 Top, Last 2 bytes */
#define EMAC_SA4L       (176) /* Specific Address 4 Bottom, First 4 bytes */
#define EMAC_SA4H       (180) /* Specific Address 4 Top, Last 2 bytes */
#define EMAC_TID        (184) /* Type ID Checking Register */
#define EMAC_TPQ        (188) /* Transmit Pause Quantum Register */
#define EMAC_USRIO      (192) /* USER Input/Output Register */
#define EMAC_WOL        (196) /* Wake On LAN Register */

#define EMAC_REV        (252) /* Revision Register */

/* EMAC_NCR -  */
#define EMAC_LB             (0x1 <<  0) /* Loopback. Optional. When set, loopback signal is at high level. */
#define EMAC_LLB            (0x1 <<  1) /* Loopback local. */
#define EMAC_RE             (0x1 <<  2) /* Receive enable. */
#define EMAC_TE             (0x1 <<  3) /* Transmit enable. */
#define EMAC_MPE            (0x1 <<  4) /* Management port enable. */
#define EMAC_CLRSTAT        (0x1 <<  5) /* Clear statistics registers. */
#define EMAC_INCSTAT        (0x1 <<  6) /* Increment statistics registers. */
#define EMAC_WESTAT         (0x1 <<  7) /* Write enable for statistics registers. */
#define EMAC_BP             (0x1 <<  8) /* Back pressure. */
#define EMAC_TSTART         (0x1 <<  9) /* Start Transmission. */
#define EMAC_THALT          (0x1 << 10) /* Transmission Halt. */
#define EMAC_TPFR           (0x1 << 11) /* Transmit pause frame */
#define EMAC_TZQ            (0x1 << 12) /* Transmit zero quantum pause frame */

/* EMAC_NCFGR - Network Configuration Register */
#define EMAC_SPD            (0x1 <<  0) /* Speed. */
#define EMAC_FD             (0x1 <<  1) /* Full duplex. */
#define EMAC_JFRAME         (0x1 <<  3) /* Jumbo Frames. */
#define EMAC_CAF            (0x1 <<  4) /* Copy all frames. */
#define EMAC_NBC            (0x1 <<  5) /* No broadcast. */
#define EMAC_MTI            (0x1 <<  6) /* Multicast hash event enable */
#define EMAC_UNI            (0x1 <<  7) /* Unicast hash enable. */
#define EMAC_BIG            (0x1 <<  8) /* Receive 1522 bytes. */
#define EMAC_EAE            (0x1 <<  9) /* External address match enable. */
#define EMAC_CLK            (0x3 << 10) /* */
#define 	EMAC_CLK_HCLK_8               (0x0 << 10) /* HCLK divided by 8 */
#define 	EMAC_CLK_HCLK_16              (0x1 << 10) /* HCLK divided by 16 */
#define 	EMAC_CLK_HCLK_32              (0x2 << 10) /* HCLK divided by 32 */
#define 	EMAC_CLK_HCLK_64              (0x3 << 10) /* HCLK divided by 64 */
#define EMAC_RTY            (0x1 << 12) /* Retry test */
#define EMAC_PAE            (0x1 << 13) /* Pause Enable */
#define EMAC_RBOF           (0x3 << 14) /* */
#define 	EMAC_RBOF_OFFSET_0             (0x0 << 14) /* no offset from start of receive buffer */
#define 	EMAC_RBOF_OFFSET_1             (0x1 << 14) /* one byte offset from start of receive buffer */
#define 	EMAC_RBOF_OFFSET_2             (0x2 << 14) /* two bytes offset from start of receive buffer */
#define 	EMAC_RBOF_OFFSET_3             (0x3 << 14) /* three bytes offset from start of receive buffer */
#define EMAC_RLCE           (0x1 << 16) /* Receive Length field Checking Enable */
#define EMAC_DRFCS          (0x1 << 17) /* Discard Receive FCS */
#define EMAC_EFRHD          (0x1 << 18) /* */
#define EMAC_IRXFCS         (0x1 << 19) /* Ignore RX FCS */

/* EMAC_NSR - Network Status Register */
#define EMAC_LINKR          (0x1 <<  0) /* */
#define EMAC_MDIO           (0x1 <<  1) /* */
#define EMAC_IDLE           (0x1 <<  2) /* */

/* EMAC_TSR - Transmit Status Register */
#define EMAC_UBR            (0x1 <<  0) /* */
#define EMAC_COL            (0x1 <<  1) /* */
#define EMAC_RLES           (0x1 <<  2) /* */
#define EMAC_TGO            (0x1 <<  3) /* Transmit Go */
#define EMAC_BEX            (0x1 <<  4) /* Buffers exhausted mid frame */
#define EMAC_COMP           (0x1 <<  5) /* */
#define EMAC_UND            (0x1 <<  6) /* */

/* EMAC_RSR - Receive Status Register */
#define EMAC_BNA            (0x1 <<  0) /* */
#define EMAC_REC            (0x1 <<  1) /* */
#define EMAC_OVR            (0x1 <<  2) /* */

/* EMAC_ISR - Interrupt Status Register */
#define EMAC_MFD            (0x1 <<  0) /* */
#define EMAC_RCOMP          (0x1 <<  1) /* */
#define EMAC_RXUBR          (0x1 <<  2) /* */
#define EMAC_TXUBR          (0x1 <<  3) /* */
#define EMAC_TUNDR          (0x1 <<  4) /* */
#define EMAC_RLEX           (0x1 <<  5) /* */
#define EMAC_TXERR          (0x1 <<  6) /* */
#define EMAC_TCOMP          (0x1 <<  7) /* */
#define EMAC_LINK           (0x1 <<  9) /* */
#define EMAC_ROVR           (0x1 << 10) /* */
#define EMAC_HRESP          (0x1 << 11) /* */
#define EMAC_PFRE           (0x1 << 12) /* */
#define EMAC_PTZ            (0x1 << 13) /* */

/* EMAC_IER - Interrupt Enable Register */

/* EMAC_IDR - Interrupt Disable Register */

/* EMAC_IMR - Interrupt Mask Register */

/* EMAC_MAN - PHY Maintenance Register */
#define EMAC_DATA(X)        (((X) & 0xffff) << 0) /* Data */
#define EMAC_CODE(X)        (((X) & 0x3) << 16) /* */
#define EMAC_REGA(X)        (((X) & 0x1f) << 18) /* Register Address */
#define EMAC_PHYA(X)        (((X) & 0x1f) << 23) /* PHY Address */
#define EMAC_RW(X)          (((X) & 0x3) << 28) /* Read-write */
#define EMAC_SOF(X)         (((X) & 0x3) << 30) /* Start of frame*/

/* EMAC_USRIO - USER Input Output Register */
#define EMAC_RMII           (0x1 <<  0) /* Reduce MII */
#define EMAC_CLKEN          (0x1 <<  1) /* Clock Enable */

/* EMAC_WOL - Wake On LAN Register */
#define EMAC_IP             (0xffff <<  0) /* ARP request IP address */
#define EMAC_MAG            (0x1 << 16) /* Magic packet event enable */
#define EMAC_ARP            (0x1 << 17) /* ARP request event enable */
#define EMAC_SA1            (0x1 << 18) /* Specific address register 1 event enable */

/* EMAC_REV - Revision Register */
#define EMAC_REVREF         (0xffff <<  0) /* */
#define EMAC_PARTREF        (0xffff << 16) /* */

/* Receive Buffer Descriptor Entry
 *
   31:2     Address of beginning of buffer

      1     Wrap - marks last descriptor in receive buffer descriptor list.

      0     Ownership - needs to be zero for the EMAC to write data to 
			 the receive buffer. The EMAC sets this to one once it has
             successfully written a frame to memory.
             Software has to clear this bit before the buffer can be used again.

     31     Global all ones broadcast address detected

     30     Multicast hash match

     29     Unicast hash match

     28     External address match

     27     Reserved for future use

     26     Specific address register 1 match

     25     Specific address register 2 match

     24     Specific address register 3 match

     23     Specific address register 4 match

     22     Type ID match

     21     VLAN tag detected (i.e., type id of 0x8100)

     20     Priority tag detected (i.e., type id of 0x8100 and null 
            VLAN identifier)
			
  19:17     VLAN priority (only valid if bit 21 is set)

     16     Concatenation format indicator (CFI) bit 
	        (only valid if bit 21 is set)

     15     End of frame - when set the buffer contains the end of a frame. 
	        If end of frame is not set, then the only other valid status
            are bits 12, 13 and 14.

     14     Start of frame - when set the buffer contains the start of a 
	        frame. If both bits 15 and 14 are set, then the buffer contains a
            whole frame.

  13:12     Receive buffer offset - indicates the number of bytes by which 
            the data in the first buffer is offset from the word address.
            Updated with the current values of the network configuration 
			register. If jumbo frame mode is enabled through bit 3 of the
            network configuration register, then bits 13:12 of the receive 
			buffer descriptor entry are used to indicate bits 13:12 of the
            frame length.
   11:0     Length of frame including FCS (if selected). Bits 13:12 are also 
            used if jumbo frame mode is selected.
*/

#define EMAC_RX_WRAP (1 << 1)
#define EMAC_RX_OWNER (1 << 0)

#define EMAC_RX_BROADCAST (1 << 31)
#define EMAC_RX_MULTICAST (1 << 30)
#define EMAC_RX_UNICAST (1 << 29)
#define EMAC_RX_EXTADDR (1 << 28)
#define EMAC_RX_EOF (1 << 15)
#define EMAC_RX_SOF (1 << 14)
#define EMAC_RX_OFFS(WORD) (((WORD) >> 13) & 0x3)
#define EMAC_RX_LEN(WORD) (((WORD) >> 0) & 0x0fff)
#define EMAC_RX_JUMBO_LEN(WORD) (((WORD) >> 0) & 0x03fff)

/* Transmit Buffer Descriptor Entry
 
  31:0      Byte Address of buffer

    31      Used. Needs to be zero for the EMAC to read data from the 
			transmit buffer. The EMAC sets this to one for the first buffer
            of a frame once it has been successfully transmitted.
            Software has to clear this bit before the buffer can be used again.
            Note:    This bit is only set for the first buffer in a frame 
			unlike receive where all buffers have the Used bit set once used.

    30      Wrap. Marks last descriptor in transmit buffer descriptor list.

    29      Retry limit exceeded, transmit error detected

    28      Transmit underrun, occurs either when hresp is not OK (bus error) 
			or the transmit data could not be fetched in time or
            when buffers are exhausted in mid frame.

    27      Buffers exhausted in mid frame

 26:17      Reserved

    16      No CRC. When set, no CRC is appended to the current frame. This 
	        bit only needs to be set for the last buffer of a frame.

    15      Last buffer. When set, this bit indicates the last buffer in the 
	        current frame has been reached.

 14:11      Reserved

  10:0      Length of buffer */

#define EMAC_TX_SEND (1 << 31)
#define EMAC_TX_WRAP (1 << 30)
#define EMAC_TX_LAST (1 << 15)



#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_emac {
	volatile uint32_t ncr; /* Network Control Register */
	volatile uint32_t ncfgr; /* Network Configuration Register */
	volatile uint32_t nsr; /* Network Status Register */
	uint32_t res0[2];
	volatile uint32_t tsr; /* Transmit Status Register */
	void * volatile rbqp; /* Receive Buffer Queue Pointer */
	void * volatile tbqp; /* Transmit Buffer Queue Pointer */

	volatile uint32_t rsr; /* Receive Status Register */
	volatile uint32_t isr; /* Interrupt Status Register */
	volatile uint32_t ier; /* Interrupt Enable Register */
	volatile uint32_t idr; /* Interrupt Disable Register */
	volatile uint32_t imr; /* Interrupt Mask Register */
	volatile uint32_t man; /* PHY Maintenance Register */
	volatile uint32_t ptr; /* Pause Time Register */
	volatile uint32_t pfr; /* Pause Frames received Register */

	volatile uint32_t fto; /* Frames Transmitted OK Register */
	volatile uint32_t scf; /* Single Collision Frame Register */
	volatile uint32_t mcf; /* Multiple Collision Frame Register */
	volatile uint32_t fro; /* Frames Received OK Register */
	volatile uint32_t fcse; /* Frame Check Sequence Error Register */
	volatile uint32_t ale; /* Alignment Error Register */
	volatile uint32_t dtf; /* Deferred Transmission Frame Register */
	volatile uint32_t lcol; /* Late Collision Register */

	volatile uint32_t ecol; /* Excessive Collision Register */
	volatile uint32_t tund; /* Transmit Underrun Error Register */
	volatile uint32_t cse; /* Carrier Sense Error Register */
	volatile uint32_t rre; /* Receive Ressource Error Register */
	volatile uint32_t rov; /* Receive Overrun Errors Register */
	volatile uint32_t rse; /* Receive Symbol Errors Register */
	volatile uint32_t ele; /* Excessive Length Errors Register */
	volatile uint32_t rja; /* Receive Jabbers Register */

	volatile uint32_t usf; /* Undersize Frames Register */
	volatile uint32_t ste; /* SQE Test Error Register */
	volatile uint32_t rle; /* Receive Length Field Mismatch Register */
	volatile uint32_t tpf; /* Transmitted Pause Frames Register */
	volatile uint32_t hrb; /* Hash Address Bottom[31:0] */
	volatile uint32_t hrt; /* Hash Address Top[63:32] */
	volatile uint32_t sa1b; /* Specific Address 1 Bottom, First 4 bytes */
	volatile uint32_t sa1t; /* Specific Address 1 Top, Last 2 bytes */

	volatile uint32_t sa2b; /* Specific Address 2 Bottom, First 4 bytes */
	volatile uint32_t sa2t; /* Specific Address 2 Top, Last 2 bytes */
	volatile uint32_t sa3b; /* Specific Address 3 Bottom, First 4 bytes */
	volatile uint32_t sa3t; /* Specific Address 3 Top, Last 2 bytes */
	volatile uint32_t sa4b; /* Specific Address 4 Bottom, First 4 bytes */
	volatile uint32_t sa4t; /* Specific Address 4 Top, Last 2 bytes */
	volatile uint32_t tid; /* Type ID Checking Register */
	volatile uint32_t tpq; /* Transmit Pause Quantum Register */

	volatile uint32_t usrio; /* USER Input/Output Register */
	volatile uint32_t wol; /* Wake On LAN Register */

	uint32_t res1[13];
	volatile uint32_t rev; /* Revision Register */
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_EMAC_H__ */
