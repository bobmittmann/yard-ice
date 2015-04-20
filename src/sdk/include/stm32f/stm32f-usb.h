/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the libstm32f.
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
 * @file stm32f-usb.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __STM32F_USB_H__
#define __STM32F_USB_H__

/* USB control register */
#define STM32F_USB_CNTR 0x40

/* Bit 15 - Correct transfer interrupt mask */
#define USB_CTRM (1 << 15)
/* 0: Correct Transfer (CTR) Interrupt disabled. 1: CTR Interrupt enabled, an
   interrupt request is generated when the corresponding bit in the USB_ISTR
   register is set. */

/* Bit 14 - Packet memory area over / underrun interrupt mask */
#define USB_PMAOVRM (1 << 14)
/* 0: PMAOVR Interrupt disabled. 1: PMAOVR Interrupt enabled, an interrupt
   request is generated when the corresponding bit in the USB_ISTR register is
   set. */

/* Bit 13 - Error interrupt mask */
#define USB_ERRM (1 << 13)
/* 0: ERR Interrupt disabled. 1: ERR Interrupt enabled, an interrupt request is 
   generated when the corresponding bit in the USB_ISTR register is set. */

/* Bit 12 - Wakeup interrupt mask */
#define USB_WKUPM (1 << 12)
/* 0: WKUP Interrupt disabled. 1: WKUP Interrupt enabled, an interrupt request
   is generated when the corresponding bit in the USB_ISTR register is set. */

/* Bit 11 - Suspend mode interrupt mask */
#define USB_SUSPM (1 << 11)
/* 0: Suspend Mode Request (SUSP) Interrupt disabled. 1: SUSP Interrupt
   enabled, an interrupt request is generated when the corresponding bit in the 
   USB_ISTR register is set. */

/* Bit 10 - USB reset interrupt mask */
#define USB_RESETM (1 << 10)
/* 0: RESET Interrupt disabled. 1: RESET Interrupt enabled, an interrupt
   request is generated when the corresponding bit in the USB_ISTR register is
   set. */

/* Bit 9 - Start of frame interrupt mask */
#define USB_SOFM (1 << 9)
/* 0: SOF Interrupt disabled. 1: SOF Interrupt enabled, an interrupt request is 
   generated when the corresponding bit in the USB_ISTR register is set. */

/* Bit 8 - Expected start of frame interrupt mask */
#define USB_ESOFM (1 << 8)
/* 0: Expected Start of Frame (ESOF) Interrupt disabled. 1: ESOF Interrupt
   enabled, an interrupt request is generated when the corresponding bit in the 
   USB_ISTR register is set. */

/* [7..5] Reserved. */

/* Bit 4 - Resume request */
#define USB_RESUME (1 << 4)
/* The microcontroller can set this bit to send a Resume signal to the host. It 
   must be activated, according to USB specifications, for no less than 1mS and 
   no more than 15mS after which the Host PC is ready to drive the resume
   sequence up to its end. */

/* Bit 3 - Force suspend */
#define USB_FSUSP (1 << 3)
/* Software must set this bit when the SUSP interrupt is received, which is
   issued when no traffic is received by the USB peripheral for 3 mS.

   0: No effect.

   1: Enter suspend mode. Clocks and static power dissipation in the analog
   transceiver are left unaffected. If suspend power consumption is a
   requirement (bus-powered device), the application software should set the
   LP_MODE bit after FSUSP as explained below. */

/* Bit 2 - Low-power mode */
#define USB_LP_MODE (1 << 2)
/* This mode is used when the suspend-mode power constraints require that all
   static power dissipation is avoided, except the one required to supply the
   external pull-up resistor. This condition should be entered when the
   application is ready to stop all system clocks, or reduce their frequency in 
   order to meet the power consumption requirements of the USB suspend
   condition. The USB activity during the suspend mode (WKUP event)
   asynchronously resets this bit (it can also be reset by software). 
   
   0: No Low-power mode. 

   1: Enter Low-power mode. */

/* Bit 1 - Power down */
#define USB_PDWN (1 << 1)
/* This bit is used to completely switch off all USB-related analog parts if it 
   is required to completely disable the USB peripheral for any reason. When
   this bit is set, the USB peripheral is disconnected from the transceivers
   and it cannot be used.

   0: Exit Power Down.

   1: Enter Power down mode. */

/* Bit 0 - Force USB Reset */
#define USB_FRES (1 << 0)
/* 0: Clear USB reset.

   1: Force a reset of the USB peripheral, exactly like a
   RESET signalling on the USB. The USB peripheral is held in RESET state until 
   software clears this bit. A “USB-RESET” interrupt is generated, if
   enabled. */

/* USB interrupt status register */
#define STM32F_USB_ISTR 0x44

/* Bit 15 - Correct transfer */
#define USB_CTR (1 << 15)
/* This bit is set by the hardware to indicate that an endpoint has
   successfully completed a transaction; using DIR and EP_ID bits software can
   determine which endpoint requested the interrupt. This bit is read-only. */

/* Bit 14 - Packet memory area over / underrun */
#define USB_PMAOVR (1 << 14)
/* This bit is set if the microcontroller has not been able to respond in time
   to an USB memory request. The USB peripheral handles this event in the
   following way: During reception an ACK handshake packet is not sent, during
   transmission a bit-stuff error is forced on the transmitted stream; in both
   cases the host will retry the transaction. The PMAOVR interrupt should never 
   occur during normal operations. Since the failed transaction is retried by
   the host, the application software has the chance to speed-up device
   operations during this interrupt handling, to be ready for the next
   transaction retry; however this does not happen during Isochronous transfers 
   (no isochronous transaction is anyway retried) leading to a loss of data in
   this case. This bit is read/write but only ‘0 can be written and writing
   ‘1 has no effect. */

/* Bit 13 - Error */
#define USB_ERR (1 << 13)
/* This flag is set whenever one of the errors listed below has occurred: NANS: 
   No ANSwer. The timeout for a host response has expired. CRC: Cyclic
   Redundancy Check error. One of the received CRCs, either in the token or in
   the data, was wrong. BST: Bit Stuffing error. A bit stuffing error was
   detected anywhere in the PID, data, and/or CRC. FVIO: Framing format
   Violation. A non-standard frame was received (EOP not in the right place,
   wrong token sequence, etc.). The USB software can usually ignore errors,
   since the USB peripheral and the PC host manage retransmission in case of
   errors in a fully transparent way. This interrupt can be useful during the
   software development phase, or to monitor the quality of transmission over
   the USB bus, to flag possible problems to the user (e.g. loose connector,
   too noisy environment, broken conductor in the USB cable and so on). This
   bit is read/write but only ‘0 can be written and writing ‘1 has no
   effect. */

/* Bit 12 - Wakeup */
#define USB_WKUP (1 << 12)
/* This bit is set to 1 by the hardware when, during suspend mode, activity is
   detected that wakes up the USB peripheral. This event asynchronously clears
   the LP_MODE bit in the CTLR register and activates the USB_WAKEUP line,
   which can be used to notify the rest of the device (e.g. wakeup unit) about
   the start of the resume process. This bit is read/write but only ‘0 can be 
   written and writing ‘1 has no effect. */

/* Bit 11 - Suspend mode request */
#define USB_SUSP (1 << 11)
/* This bit is set by the hardware when no traffic has been received for 3mS,
   indicating a suspend mode request from the USB bus. The suspend condition
   check is enabled immediately after any USB reset and it is disabled by the
   hardware when the suspend mode is active (FSUSP=1) until the end of resume
   sequence. This bit is read/write but only ‘0 can be written and writing
   ‘1 has no effect. */

/* Bit 10 - USB reset request */
#define USB_RESET (1 << 10)
/* Set when the USB peripheral detects an active USB RESET signal at its
   inputs. The USB peripheral, in response to a RESET, just resets its internal 
   protocol state machine, generating an interrupt if RESETM enable bit in the
   USB_CNTR register is set. Reception and transmission are disabled until the
   RESET bit is cleared. All configuration registers do not reset: the
   microcontroller must explicitly clear these registers (this is to ensure
   that the RESET interrupt can be safely delivered, and any transaction
   immediately followed by a RESET can be completed). The function address and
   endpoint registers are reset by an USB reset event. This bit is read/write
   but only ‘0 can be written and writing ‘1 has no effect. */

/* Bit 9 - Start of frame */
#define USB_SOF (1 << 9)
/* This bit signals the beginning of a new USB frame and it is set when a SOF
   packet arrives through the USB bus. The interrupt service routine may
   monitor the SOF events to have a 1mS synchronization event to the USB host
   and to safely read the USB_FNR register which is updated at the SOF packet
   reception (this could be useful for isochronous applications). This bit is
   read/write but only ‘0 can be written and writing ‘1 has no effect. */

/* Bit 8 - Expected start of frame */
#define USB_ESOF (1 << 8)
/* This bit is set by the hardware when an SOF packet is expected but not
   received. The host sends an SOF packet each mS, but if the hub does not
   receive it properly, the Suspend Timer issues this interrupt. If three
   consecutive ESOF interrupts are generated (i.e. three SOF packets are lost)
   without any traffic occurring in between, a SUSP interrupt is generated.
   This bit is set even when the missing SOF packets occur while the Suspend
   Timer is not yet locked. This bit is read/write but only ‘0 can be written 
   and writing ‘1 has no effect. */

/* [7..5] Reserved. */

/* Bit 4 - Direction of transaction */
#define USB_DIR (1 << 4)
/* This bit is written by the hardware according to the direction of the
   successful transaction, which generated the interrupt request. If DIR bit=0, 
   CTR_TX bit is set in the USB_EPnR register related to the interrupting
   endpoint. The interrupting transaction is of IN type (data transmitted by
   the USB peripheral to the host PC). If DIR bit=1, CTR_RX bit or both
   CTR_TX/CTR_RX are set in the USB_EPnR register related to the interrupting
   endpoint. The interrupting transaction is of OUT type (data received by the
   USB peripheral from the host PC) or two pending transactions are waiting to
   be processed. This information can be used by the application software to
   access the USB_EPnR bits related to the triggering transaction since it
   represents the direction having the interrupt pending. This bit is
   read-only. */

/* Bits [3..0] - Endpoint Identifier */
#define USB_EP_ID_MSK (((1 << (3 + 1)) - 1) << 0)
#define USB_EP_ID_SET(VAL) (((VAL) << 0) & USB_EP_ID_MSK)
#define USB_EP_ID_GET(REG) (((REG) & USB_EP_ID_MSK) >> 0)
/* These bits are written by the hardware according to the endpoint number,
   which generated the interrupt request. If several endpoint transactions are
   pending, the hardware writes the endpoint identifier related to the endpoint 
   having the highest priority defined in the following way: Two endpoint sets
   are defined, in order of priority: Isochronous and double-buffered bulk
   endpoints are considered first and then the other endpoints are examined. If 
   more than one endpoint from the same set is requesting an interrupt, the
   EP_ID bits in USB_ISTR register are assigned according to the lowest
   requesting endpoint register, EP0R having the highest priority followed by
   EP1R and so on. The application software can assign a register to each
   endpoint according to this priority scheme, so as to order the concurring
   endpoint requests in a suitable way. These bits are read only. */

/* USB frame number register */
#define STM32F_USB_FNR 0x48

/* Bit 15 - Receive data + line status */
#define USB_RXDP (1 << 15)
/* This bit can be used to observe the status of received data plus upstream
   port data line. It can be used during end-of-suspend routines to help
   determining the wakeup event. */

/* Bit 14 - Receive data - line status */
#define USB_RXDM (1 << 14)
/* This bit can be used to observe the status of received data minus upstream
   port data line. It can be used during end-of-suspend routines to help
   determining the wakeup event. */

/* Bit 13 - Locked */
#define USB_LCK (1 << 13)
/* This bit is set by the hardware when at least two consecutive SOF packets
   have been received after the end of an USB reset condition or after the end
   of an USB resume sequence. Once locked, the frame timer remains in this
   state until an USB reset or USB suspend event occurs. */

/* Bits [12..11] - Lost SOF */
#define USB_LSOF_MSK (((1 << (1 + 1)) - 1) << 11)
#define USB_LSOF_SET(VAL) (((VAL) << 11) & USB_LSOF_MSK)
#define USB_LSOF_GET(REG) (((REG) & USB_LSOF_MSK) >> 11)
/* These bits are written by the hardware when an ESOF interrupt is generated,
   counting the number of consecutive SOF packets lost. At the reception of an
   SOF packet, these bits are cleared. */

/* Bits [10..0] - Frame number */
#define USB_FN_MSK (((1 << (10 + 1)) - 1) << 0)
#define USB_FN_SET(VAL) (((VAL) << 0) & USB_FN_MSK)
#define USB_FN_GET(REG) (((REG) & USB_FN_MSK) >> 0)
/* This bit field contains the 11-bits frame number contained in the last
   Received SOF packet. The frame number is incremented for every frame sent by 
   the host and it is useful for Isochronous transfers. This bit field is
   updated on the generation of an SOF interrupt. */

/* USB device address */
#define STM32F_USB_DADDR 0x4C

/* [15..8] Reserved */

/* Bit 7 - Enable function */
#define USB_EF (1 << 7)
/* This bit is set by the software to enable the USB device. The address of
   this device is contained in the following ADD[6:0] bits. If this bit is at
   ‘0 no transactions are handled, irrespective of the settings of USB_EPnR
   registers. */

/* Bits [6..0] - Device address */
#define USB_ADD_MSK (((1 << (6 + 1)) - 1) << 0)
#define USB_ADD_SET(VAL) (((VAL) << 0) & USB_ADD_MSK)
#define USB_ADD_GET(REG) (((REG) & USB_ADD_MSK) >> 0)
/* These bits contain the USB function address assigned by the host PC during
   the enumeration process. Both this field and the Endpoint Address (EA) field 
   in the associated USB_EPnR register must match with the information co */

/* Buffer table address */
#define STM32F_USB_BTABLE 0x50

/* Bits [15..3] - Buffer table */
#define USB_BTABLE_MSK (((1 << (15 + 1)) - 1) << 3)
#define USB_BTABLE_SET(VAL) (((VAL) << 3) & USB_BTABLE_MSK)
#define USB_BTABLE_GET(REG) (((REG) & USB_BTABLE_MSK) >> 3)
/* These bits contain the start address of the buffer allocation table inside
   the dedicated packet memory. This table describes each endpoint buffer
   location and size and it must be aligned to an 8 byte boundary (the 3 least
   significant bits are always ‘0). At the beginning of every transaction
   addressed to this device, the USP peripheral reads the element of this table
   related to the addressed endpoint, to get its buffer start location and the
   buffer size (Refer to Structure and usage of packet buffers on page 601). */

/* [2..0] Reserved, forced by hardware to 0. */

/* USB endpoint n register */
#define STM32F_USB_EPNR 0x00

/* Bit 15 - Correct Transfer for reception */
#define USB_CTR_RX (1 << 15)
/* This bit is set by the hardware when an OUT/SETUP transaction is
   successfully completed on this endpoint; the software can only clear this
   bit. If the CTRM bit in USB_CNTR register is set accordingly, a generic
   interrupt condition is generated together with the endpoint related
   interrupt condition, which is always activated. The type of occurred
   transaction, OUT or SETUP, can be determined from the SETUP bit described
   below. A transaction ended with a NAK or STALL handshake does not set this
   bit, since no data is actually transferred, as in the case of protocol
   errors or data toggle mismatches. This bit is read/write but only ‘0 can
   be written, writing 1 has no effect. */

/* Bit 14 - Data Toggle, for reception transfers */
#define USB_DTOG_RX (1 << 14)
#define USB_SWBUF_TX (1 << 14)
/* If the endpoint is not Isochronous, this bit contains the expected value of
   the data toggle bit (0=DATA0, 1=DATA1) for the next data packet to be
   received. Hardware toggles this bit, when the ACK handshake is sent to the
   USB host, following a data packet reception having a matching data PID
   value; if the endpoint is defined as a control one, hardware clears this bit 
   at the reception of a SETUP PID addressed to this endpoint. If the endpoint
   is using the double-buffering feature this bit is used to support packet
   buffer swapping too (Refer to Section 23.4.3: Double-buffered endpoints). If 
   the endpoint is Isochronous, this bit is used only to support packet buffer
   swapping since no data toggling is used for this sort of endpoints and only
   DATA0 packet are transmitted (Refer to Section 23.4.4: Isochronous
   transfers). Hardware toggles this bit just after the end of data packet
   reception, since no handshake is used for isochronous transfers. This bit
   can also be toggled by the software to initialize its value (mandatory when
   the endpoint is not a control one) or to force specific data toggle/packet
   buffer usage. When the application software writes ‘0, the value of
   DTOG_RX remains unchanged, while writing ‘1 makes the bit value toggle.
   This bit is read/write but it can be only toggled by writing 1. */

/* Bits [13..12] - Status bits for reception transfers */
#define USB_STAT_RX     (3 << 12) 
#define USB_STAT_RX_MSK (3 << 12) 
/* These bits contain
   information about the endpoint status, which are listed in Table 173:
   Reception status encoding on page 622.These bits can be toggled by software
   to initialize their value. When the application software writes ‘0, the
   value remains unchanged, while writing ‘1 makes the bit value toggle.
   Hardware sets the STAT_RX bits to NAK when a correct transfer has occurred
   (CTR_RX=1) corresponding to a OUT or SETUP (control only) transaction
   addressed to this endpoint, so the software has the time to elaborate the
   received data before it acknowledge a new transaction Double-buffered bulk
   endpoints implement a special transaction flow control, which control the
   status based upon buffer availability condition (Refer to Section 23.4.3:
   Double-buffered endpoints). If the endpoint is defined as Isochronous, its
   status can be only “VALID” or “DISABLED”, so that the hardware
   cannot change the status of the endpoint after a successful transaction. If
   the software sets the STAT_RX bits to ‘STALL’ or ‘NAK’ for an
   Isochronous endpoint, the USB peripheral behavior is not defined. These bits 
   are read/write but they can be only toggled by writing ‘1. */

#define USB_RX_DISABLED (0 << 12)
#define USB_RX_STALL    (1 << 12)
#define USB_RX_NAK      (2 << 12)
#define USB_RX_VALID    (3 << 12)

/* Bit 11 - Setup transaction completed */
#define USB_SETUP (1 << 11)
/* This bit is read-only and it is set by the hardware when the last completed
   transaction is a SETUP. This bit changes its value only for control
   endpoints. It must be examined, in the case of a successful receive
   transaction (CTR_RX event), to determine the type of transaction occurred.
   To protect the interrupt service routine from the changes in SETUP bits due
   to next incoming tokens, this bit is kept frozen while CTR_RX bit is at 1;
   its state changes when CTR_RX is at 0. This bit is read-only. */

/* Bits [10..9] - Endpoint type */
#define USB_EP_TYPE_MSK (((1 << (1 + 1)) - 1) << 9)
#define USB_EP_TYPE_SET(VAL) (((VAL) << 9) & USB_EP_TYPE_MSK)
#define USB_EP_TYPE_GET(REG) (((REG) & USB_EP_TYPE_MSK) >> 9)
/* These bits configure the behavior of this endpoint as described in Table
   174: Endpoint type encoding on page 622. Endpoint 0 must always be a control 
   endpoint and each USB function must have at least one control endpoint which 
   has address 0, but there may be other control endpoints if required. Only
   control endpoints handle SETUP transactions, which are ignored by endpoints
   of other kinds. SETUP transactions cannot be answered with NAK or STALL. If
   a control endpoint is defined as NAK, the USB peripheral will not answer,
   simulating a receive error, in the receive direction when a SETUP
   transaction is received. If the control endpoint is defined as STALL in the
   receive direction, then the SETUP packet will be accepted anyway,
   transferring data and issuing the CTR interrupt. The reception of OUT
   transactions is handled in the normal way, even if the endpoint is a control
   one. Bulk and interrupt endpoints have very similar behavior and they differ
   only in the special feature available using the EP_KIND configuration bit.
   The usage of Isochronous endpoints is explained in Section 23.4.4:
   Isochronous transfers */

#define USB_EP_BULK      (0 << 9)
#define USB_EP_CONTROL   (1 << 9)
#define USB_EP_ISO       (2 << 9)
#define USB_EP_INTERRUPT (3 << 9)
   
/* Bit 8 - Endpoint kind */
#define USB_EP_KIND (1 << 8)
#define USB_EP_DBL_BUF (1 << 8)
#define USB_EP_STATUS_OUT (1 << 8)
/* The meaning of this bit depends on the endpoint type configured by the
   EP_TYPE bits. Table 175 summarizes the different meanings. 

   DBL_BUF: This bit 
   is set by the software to enable the double-buffering feature for this bulk
   endpoint. The usage of double-buffered bulk endpoints is explained in
   Section 23.4.3: Double-buffered endpoints.

   STATUS_OUT: This bit is set by
   the software to indicate that a status out transaction is expected: in this
   case all OUT transactions containing more than zero data bytes are answered
   ‘STALL’ instead of ‘ACK’. This bit may be used to improve the
   robustness of the application to protocol errors during control transfers
   and its usage is intended for control endpoints only. When STATUS_OUT is
   reset, OUT transactions can have any number of bytes, as required. */

/* Bit 7 - Correct Transfer for transmission */
#define USB_CTR_TX (1 << 7)
/* This bit is set by the hardware when an IN transaction is successfully
   completed on this endpoint; the software can only clear this bit. If the
   CTRM bit in the USB_CNTR register is set accordingly, a generic interrupt
   condition is generated together with the endpoint related interrupt
   condition, which is always activated. A transaction ended with a NAK or
   STALL handshake does not set this bit, since no data is actually
   transferred, as in the case of protocol errors or data toggle mismatches.
   This bit is read/write but only ‘0 can be written. */

/* Bit 6 - Data Toggle, for transmission transfers */
#define USB_DTOG_TX (1 << 6)
#define USB_SWBUF_RX (1 << 6)
/* If the endpoint is non-isochronous, this bit contains the required value of
   the data toggle bit (0=DATA0, 1=DATA1) for the next data packet to be
   transmitted. Hardware toggles this bit when the ACK handshake is received
   from the USB host, following a data packet transmission. If the endpoint is
   defined as a control one, hardware sets this bit to 1 at the reception of a
   SETUP PID addressed to this endpoint. If the endpoint is using the double
   buffer feature, this bit is used to support packet buffer swapping too
   (Refer to Section 23.4.3: Double-buffered endpoints) If the endpoint is
   Isochronous, this bit is used to support packet buffer swapping since no
   data toggling is used for this sort of endpoints and only DATA0 packet are
   transmitted (Refer to Section 23.4.4: Isochronous transfers). Hardware
   toggles this bit just after the end of data packet transmission, since no
   handshake is used for Isochronous transfers. This bit can also be toggled by 
   the software to initialize its value (mandatory when the endpoint is not a
   control one) or to force a specific data toggle/packet buffer usage. When
   the application software writes ‘0, the value of DTOG_TX remains
   unchanged, while writing ‘1 makes the bit value toggle. This bit is
   read/write but it can only be toggled by writing 1. */

/* Bits [5..4] - Status bits, for transmission transfers */
#define USB_STAT_TX     (0x3 << 4)
#define USB_STAT_TX_MSK (0x3 << 4)
/* These bits contain the information
   about the endpoint status, listed in Table 176. These bits can be toggled by 
   the software to initialize their value. When the application software writes
   ‘0, the value remains unchanged, while writing ‘1 makes the bit value
   toggle. Hardware sets the STAT_TX bits to NAK, when a correct transfer has
   occurred (CTR_TX=1) corresponding to a IN or SETUP (control only) transaction 
   addressed to this endpoint. It then waits for the software to prepare the
   next set of data to be transmitted. Double-buffered bulk endpoints implement
   a special transaction flow control, which controls the status based on buffer 
   availability condition (Refer to Section 23.4.3: Double-buffered endpoints).
   If the endpoint is defined as Isochronous, its status can only be “VALID” 
   or “DISABLED”. Therefore, the hardware cannot change the status of the
   endpoint after a successful transaction. If the software sets the STAT_TX
   bits to ‘STALL’ or ‘NAK’ for an Isochronous endpoint, the USB
   peripheral behavior is not defined. These bits are read/write but they can be 
   only toggled by writing ‘1. */

#define USB_TX_DISABLED (0 << 4)
#define USB_TX_STALL    (1 << 4)
#define USB_TX_NAK      (2 << 4)
#define USB_TX_VALID    (3 << 4)


/* Bits [3..0] - Endpoint address */
#define USB_EA_MSK (((1 << (3 + 1)) - 1) << 0)
#define USB_EA_SET(VAL) (((VAL) << 0) & USB_EA_MSK)
#define USB_EA_GET(REG) (((REG) & USB_EA_MSK) >> 0)
/* Software must write in this field the 4-bit address used to identify the
   transactions directed to this endpoint. A value must be written before
   enabling the corresponding endpoint. Transmission buffer address n
   (USB_ADDRn_TX) Address offset: [USB_BTABLE] + n*16 USB local address:
   [USB_BTABLE] + n*8 */




/* Bits [15..1] - Transmission buffer address */
#define USB_ADDRN_TX_MSK (((1 << (15 + 1)) - 1) << 1)
#define USB_ADDRN_TX_SET(VAL) (((VAL) << 1) & USB_ADDRN_TX_MSK)
#define USB_ADDRN_TX_GET(REG) (((REG) & USB_ADDRN_TX_MSK) >> 1)
/* These bits point to the starting address of the packet buffer containing
   data to be transmitted by the endpoint associated with the USB_EPnR register 
   at the next IN token addressed to it. Bit 0 Must always be written as ‘0
   since packet memory is word-wide and all packet buffers must be
   word-aligned. */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32f_usb {
       /* 0x000 */
	volatile uint32_t epr[8];
       /* 0x020 */
	uint32_t res1[(0x040 - 0x020) / 4];
       /* 0x040 */
	volatile uint32_t cntr;
	volatile uint32_t istr;
	volatile uint32_t fnr;
	volatile uint32_t daddr;
       /* 0x050 */
	volatile uint32_t * btable;
};

/* TX packet buffer descriptor */
struct stm32f_usb_tx_pktbuf {
	uint32_t addr;
	uint32_t count;
};

/* RX packet buffer descriptor */
struct stm32f_usb_rx_pktbuf {
	uint32_t addr;
	volatile uint32_t count: 10;
	uint32_t num_block: 5;
	uint32_t blsize: 1;
	uint32_t res: 16;
};

/* Generic packet buffer descriptor */
struct stm32f_usb_pktbuf {
	union {
		struct {
			/* single buffer entry */
			struct stm32f_usb_tx_pktbuf tx;
			struct stm32f_usb_rx_pktbuf rx;
		};
		/* double buffer TX */
		struct stm32f_usb_tx_pktbuf dbtx[2];
		/* double buffer RX */
		struct stm32f_usb_rx_pktbuf dbrx[2];
	};
};


/* EndPoint no toggle MASK (no toggle fields) */
#define USB_EPREG_MASK (USB_CTR_RX | USB_SETUP | USB_EP_TYPE_MSK | \
						USB_EP_KIND| USB_CTR_TX | USB_EA_MSK)

#define EPTX_DTOGMASK  (USB_STAT_TX_MSK | USB_EPREG_MASK)
#define EPRX_DTOGMASK  (USB_STAT_RX_MSK | USB_EPREG_MASK)

static inline void __set_ep_txstat(struct stm32f_usb * usb, 
								 unsigned int ep_id, uint32_t stat) {
	uint32_t epr;
	epr = usb->epr[ep_id] & EPTX_DTOGMASK;
	usb->epr[ep_id] = epr ^ (stat & USB_STAT_TX_MSK);
};

static inline void __set_ep_rxstat(struct stm32f_usb * usb, 
								 unsigned int ep_id, uint32_t stat) {
	uint32_t epr;
	epr = usb->epr[ep_id] & EPRX_DTOGMASK;
	usb->epr[ep_id] = epr ^ (stat & USB_STAT_RX_MSK);
};

static inline uint32_t __get_ep_txstat(struct stm32f_usb * usb, 
									 unsigned int ep_id) {
	return usb->epr[ep_id] & USB_STAT_TX_MSK;
};

static inline uint32_t __get_ep_rxstat(struct stm32f_usb * usb, 
									 unsigned int ep_id) {
	return usb->epr[ep_id] & USB_STAT_RX_MSK;
};


static inline void __clr_ep_flag(struct stm32f_usb * usb, 
							   int ep_id, uint32_t flag) {
	uint32_t epr;
	epr = usb->epr[ep_id];
	usb->epr[ep_id] = (epr & ~flag) & USB_EPREG_MASK;
}

static inline void __set_ep_flag(struct stm32f_usb * usb, 
							   int ep_id, uint32_t flag) {
	uint32_t epr;
	epr = usb->epr[ep_id];
	usb->epr[ep_id] = (epr | flag) & USB_EPREG_MASK;
}

static inline void __toggle_ep_flag(struct stm32f_usb * usb, 
									int ep_id, uint32_t flag) {
	uint32_t epr;
	epr = usb->epr[ep_id];
	usb->epr[ep_id] = (epr & USB_EPREG_MASK) | flag;
}


/*
static inline void __set_ep_rxvalid(struct stm32f_usb * usb, int ep_id) {
	__set_ep_rxstat(usb, ep_id, USB_RX_VALID);
} 

static inline void __clr_ep_kind(struct stm32f_usb * usb, int ep_id) {
	__clr_ep_flag(usb, ep_id, USB_EP_KIND);
}

static inline void __set_ep_kind(struct stm32f_usb * usb, int ep_id) {
	__set_ep_flag(usb, ep_id, USB_EP_KIND);
}

static inline void __set_ep_dbl_buf(struct stm32f_usb * usb, int ep_id) {
	__set_ep_flag(usb, ep_id, USB_EP_DBL_BUF);
}

static inline void __clr_status_out(struct stm32f_usb * usb, int ep_id) {
	__clr_ep_flag(usb, ep_id, USB_EP_STATUS_OUT);
}
*/

static inline void __set_ep_type(struct stm32f_usb * usb, int ep_id, int type) {
	uint32_t epr;
	epr = usb->epr[ep_id] & USB_EPREG_MASK & ~USB_EP_TYPE_MSK;
	usb->epr[ep_id] = epr | type;
}

static inline void __set_ep_addr(struct stm32f_usb * usb, int ep_id, int addr) {
	uint32_t epr;
	epr = usb->epr[ep_id] & USB_EPREG_MASK & ~USB_EA_MSK;
	usb->epr[ep_id] = epr | addr;
}

/* configure a RX descriptor */
static inline int __pktbuf_rx_cfg(struct stm32f_usb_rx_pktbuf * rx,
		int addr, int mxpktsz) {
	int sz = mxpktsz;
	if (sz < 63) {
		sz = (sz + 1) & ~0x01;
		rx->num_block = sz >> 1;
		rx->blsize = 0;
	} else {
		/* round up to a multiple of 32 */
		sz = (sz + 0x1f) & ~0x1f;
		rx->num_block = (sz >> 5) - 1;
		rx->blsize = 1;
	}
	rx->addr = addr;
	rx->count = 0;
	return sz;
}

/* configure a TX descriptor */
static inline int __pktbuf_tx_cfg(struct stm32f_usb_tx_pktbuf * tx,
		int addr, int mxpktsz) {
	tx->addr = addr;
	tx->count = 0;
	return mxpktsz;
}

/* return the max packet size for the RX packet buffer */
static inline int __pktbuf_rx_mxpktsz(struct stm32f_usb_rx_pktbuf * rx) {
	return (rx->blsize) ? (rx->num_block + 1) * 32 : rx->num_block * 2;
}

#endif /* __ASSEMBLER__ */

#endif /* __STM32F_USB_H__ */
