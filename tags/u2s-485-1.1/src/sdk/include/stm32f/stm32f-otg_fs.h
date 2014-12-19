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
 * @file stm32f-otg_fs.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_OTG_FS_H__
#define __STM32F_OTG_FS_H__

/* -------------------------------------------------------------------------
 * OTG_FS control and status register */
#define STM32F_OTG_FS_GOTGCTL 0x000

/* Bit 19 - B-session valid */
#define OTG_FS_BSVLD (1 << 19)
/* Indicates the device mode transceiver status.
	0: B-session is not valid.
	1: B-session is valid.
	In OTG mode, you can use this bit to determine if the device is 
	connected or disconnected. Note: Only accessible in device mode. */

/* Bit 18 - A-session valid */
#define OTG_FS_ASVLD (1 << 18)
/* Indicates the host mode transceiver status.
	0: A-session is not valid
	1: A-session is valid
	Note: Only accessible in host mode. */

/* Bit 17 - Long/short debounce time */
#define OTG_FS_DBCT (1 << 17)
/* Indicates the debounce time of a detected connection.
	0: Long debounce time, used for physical connections (100 ms + 2.5 μs)
	1: Short debounce time, used for soft connections (2.5 μs)
	Note: Only accessible in host mode. */

/* Bit 16 - Connector ID status */
#define OTG_FS_CIDSTS (1 << 16)
/* Indicates the connector ID status on a connect event.
	0: The OTG_FS controller is in A-device mode
	1: The OTG_FS controller is in B-device mode
	Note: Accessible in both device and host modes. */

/* [15..12] Reserved */

/* Bit 11 - Device HNP enabled */
#define OTG_FS_DHNPEN (1 << 11)
/* The application sets this bit when it successfully receives 
   a SetFeature.SetHNPEnable command from the connected USB host.
	0: HNP is not enabled in the application
	1: HNP is enabled in the application
	Note: Only accessible in device mode. */

/* Bit 10 - host set HNP enable */
#define OTG_FS_HSHNPEN (1 << 10)
/* The application sets this bit when it has successfully enabled 
   HNP (using the SetFeature.SetHNPEnable command) on the connected device.
	0: Host Set HNP is not enabled
	1: Host Set HNP is enabled
	Note: Only accessible in host mode. */

/* Bit 9 - HNP request */
#define OTG_FS_HNPRQ (1 << 9)
/* The application sets this bit to initiate an HNP request to the 
   connected USB host. The application can clear this bit by writing a 0 
   when the host negotiation success status change bit in the OTG_FS_GOTGINT 
   register (HNSSCHG bit in OTG_FS_GOTGINT) is set. The core clears this 
   bit when the HNSSCHG bit is cleared.
	0: No HNP request
	1: HNP request
	Note: Only accessible in device mode. */

/* Bit 8 - Host negotiation success */
#define OTG_FS_HNGSCS (1 << 8)
/* The core sets this bit when host negotiation is successful. The core 
   clears this bit when the HNP Request (HNPRQ) bit in this register is set.
	0: Host negotiation failure
	1: Host negotiation success
	Note: Only accessible in device mode. */

/* [7..2] Reserved */

/* Bit 1 - Session request */
#define OTG_FS_SRQ (1 << 1)
/* The application sets this bit to initiate a session request on the USB. 
   The application can clear this bit by writing a 0 when the host 
   negotiation success status change bit in the OTG_FS_GOTGINT register 
   (HNSSCHG bit in OTG_FS_GOTGINT) is set. The core clears this bit when 
   the HNSSCHG bit is cleared.
   If you use the USB 1.1 full-speed serial transceiver interface to 
   initiate the session request, the application must wait until VBUS 
   discharges to 0.2 V, after the B-Session Valid bit in this register
   (BSVLD bit in OTG_FS_GOTGCTL) is cleared. This discharge time varies 
   between different PHYs and can be obtained from the PHY vendor.
	0: No session request
	1: Session request
	Note: Only accessible in device mode. */

/* Bit 0 - Session request success */
#define OTG_FS_SRQSCS (1 << 0)
/* The core sets this bit when a session request initiation is successful.
	0: Session request failure
	1: Session request success
	Note: Only accessible in device mode. */


/* -------------------------------------------------------------------------
 * OTG_FS interrupt register 
 */
#define STM32F_OTG_FS_GOTGINT 0x004

/* [31..20] Reserved. */

/* Bit 19 - Debounce done */
#define OTG_FS_DBCDNE (1 << 19)
/* The core sets this bit when the debounce is completed after the device 
   connect. The application can start driving USB reset after seeing this 
   interrupt. This bit is only valid when the HNP Capable or SRP Capable bit 
   is set in the OTG_FS_GUSBCFG register (HNPCAP bit or SRPCAP bit in 
   OTG_FS_GUSBCFG, respectively).
	Note: Only accessible in host mode. */

/* Bit 18 - A-device timeout change */
#define OTG_FS_ADTOCHG (1 << 18)
/* The core sets this bit to indicate that the A-device has timed out while 
   waiting for the B-device to connect.
	Note: Accessible in both device and host modes. */

/* Bit 17 - Host negotiation detected */
#define OTG_FS_HNGDET (1 << 17)
/* The core sets this bit when it detects a host negotiation request on 
 * the USB. 
 * Note: Accessible in both device and host modes. */

/* [16..10] Reserved. */

/* Bit 9 - Host negotiation success status change */
#define OTG_FS_HNSSCHG (1 << 9)
/* The core sets this bit on the success or failure of a USB host negotiation 
 * request. The application must read the host negotiation success bit of the 
 * OTG_FS_GOTGCTL register (HNGSCS in OTG_FS_GOTGCTL) to check for success or 
 * failure.
 * Note: Accessible in both device and host modes. */

/* [7..3] Reserved. */

/* Bit 8 - Session request success status change */
#define OTG_FS_SRSSCHG (1 << 8)
/* The core sets this bit on the success or failure of a session request. The 
   application must read the session request success bit in the OTG_FS_GOTGCTL 
   register (SRQSCS bit in OTG_FS_GOTGCTL) to check for success or failure. 
	Note: Accessible in both device and host modes. */

/* Bit 2 - Session end detected */
#define OTG_FS_SEDET (1 << 2)
/* The core sets this bit to indicate that the level of the voltage on VBUS 
   is no longer valid for a B-Peripheral session when VBUS < 0.8 V. */

/* [1..0] Reserved. */


/* -------------------------------------------------------------------------
 * OTG_FS AHB configuration register */
#define STM32F_OTG_FS_GAHBCFG 0x008

/* [31..20] Reserved. */

/* Bit 8 - Periodic TxFIFO empty level */
#define OTG_FS_PTXFELVL (1 << 8)
/* Indicates when the periodic TxFIFO empty interrupt bit in the 
 * OTG_FS_GINTSTS register (PTXFE bit in OTG_FS_GINTSTS) is triggered.
 * 0: PTXFE (in OTG_FS_GINTSTS) interrupt indicates that the Periodic 
 * TxFIFO is half empty
 * 1: PTXFE (in OTG_FS_GINTSTS) interrupt indicates that the Periodic 
 * TxFIFO is completely empty
 * Note: Only accessible in host mode. */

/* Bit 7 - TxFIFO empty level */
#define OTG_FS_TXFELVL (1 << 7)
/* In device mode, this bit indicates when IN endpoint Transmit FIFO empty 
 *    interrupt (TXFE in OTG_FS_DIEPINTx.) is triggered.
 * 0: the TXFE (in OTG_FS_DIEPINTx) interrupt indicates that the IN Endpoint 
 *    TxFIFO is half empty
 * 1: the TXFE (in OTG_FS_DIEPINTx) interrupt indicates that the IN Endpoint 
 *    TxFIFO is completely empty
 * In host mode, this bit indicates when the nonperiodic Tx FIFO empty 
 *    interrupt (NPTXFE bit in OTG_FS_GINTSTS) is triggered:
 * 0: the NPTXFE (in OTG_FS_GINTSTS) interrupt indicates that the 
 *    nonperiodic Tx FIFO is half empty
 * 1: the NPTXFE (in OTG_FS_GINTSTS) interrupt indicates that the 
 *    nonperiodic Tx FIFO is completely empty */

/* [6..1] Reserved. */


/* Bit 0 - Global interrupt mask */
#define OTG_FS_GINTMSK (1 << 0)
/* The application uses this bit to mask or unmask the interrupt line 
   assertion to itself. Irrespective of this bit’s setting, the interrupt 
   status registers are updated by the core.
   0: Mask the interrupt assertion to the application.
   1: Unmask the interrupt assertion to the application.
   Note: Accessible in both device and host modes. */


/* -------------------------------------------------------------------------
 * OTG_FS USB configuration register */
#define STM32F_OTG_FS_GUSBCFG 0x00C


/* [31..20] Reserved. */


/* Bit 31 - Corrupt Tx packet */
#define OTG_FS_CTXPKT (1 << 31)
/* This bit is for debug purposes only. Never set this bit to 1.
   Note: Accessible in both device and host modes. */

/* Bit 30 - Force device mode */
#define OTG_FS_FDMOD (1 << 30)
/* Writing a 1 to this bit forces the core to device mode irrespective 
   of the OTG_FS_ID input pin.
   0: Normal mode
   1: Force device mode
   After setting the force bit, the application must wait at least 25 ms 
   before the change takes effect.
   Note: Accessible in both device and host modes. */

/* Bit 29 - Force host mode */
#define OTG_FS_FHMOD (1 << 29)
/* Writing a 1 to this bit forces the core to host mode irrespective of the 
   OTG_FS_ID input pin.
   0: Normal mode
   1: Force host mode
   After setting the force bit, the application must wait at least 25 ms 
   before the change takes effect.
   Note: Accessible in both device and host modes. */

/* [28..14] Reserved */

/* Bits [13..10] - USB turnaround time */
#define OTG_FS_TRDT ((13 - 10) << 10)
#define OTG_FS_TRDT_MSK (((1 << (13 - 10 + 1)) - 1) << 10)
#define OTG_FS_TRDT_SET(VAL) (((VAL) << 10) & OTG_FS_TRDT_MSK)
#define OTG_FS_TRDT_GET(REG) (((REG) & OTG_FS_TRDT_MSK) >> 10)
/* Sets the turnaround time in PHY clocks.
   To calculate the value of TRDT, use the following formula:
   TRDT = 4 × AHB clock + 1 PHY clock
   Examples:
   1. if AHB clock = 72 MHz (PHY Clock is 48), the TRDT is set to 9.
   2. if AHB clock = 48 MHz (PHY Clock is 48), the TRDT is set to 5.
   Note: Only accessible in device mode. */

/* Bit 9 - HNP-capable */
#define OTG_FS_HNPCAP (1 << 9)
/* The application uses this bit to control the OTG_FS controller’s HNP 
   capabilities.
   0: HNP capability is not enabled.
   1: HNP capability is enabled.
   Note: Accessible in both device and host modes. */

/* Bit 8 - SRP-capable */
#define OTG_FS_SRPCAP (1 << 8)
/* The application uses this bit to control the OTG_FS controller’s SRP 
   capabilities. If the core operates as a non-SRP-capable B-device, it 
   cannot request the connected A-device (host) to activate VBUS and start 
   a session.
   0: SRP capability is not enabled.
   1: SRP capability is enabled.
   Note: Accessible in both device and host modes. */

/* Bit 7 - Full Speed serial transceiver select */
#define OTG_FS_PHYSEL (1 << 7)
/* This bit is always 1 with write-only access. */

/*   Bits [6:3] Reserved */

/* Bits [2:0] : FS timeout calibration */
#define OTG_FS_TOCAL ((2 - 0) << 0)
#define OTG_FS_TOCAL_MSK (((1 << (2 - 0 + 1)) - 1) << 0)
#define OTG_FS_TOCAL_SET(VAL) (((VAL) << 0) & OTG_FS_TOCAL_MSK)
#define OTG_FS_TOCAL_GET(REG) (((REG) & OTG_FS_TOCAL_MSK) >> 0)
/* The number of PHY clocks that the application programs in this field 
   is added to the full-speed interpacket timeout duration in the core to 
   account for any additional delays introduced by the PHY. This can be 
   required, because the delay introduced by the PHY in generating the line
   state condition can vary from one PHY to another.
   The USB standard timeout value for full-speed operation is 16 to 18 
   (inclusive) bit times. The application must program this field based on 
   the speed of enumeration. The number of bit times added per PHY 
   clock is 0.25 bit times. */


/* -------------------------------------------------------------------------
 * OTG_FS reset register */
#define STM32F_OTG_FS_GRSTCTL 0x10

/* Bit 31 - AHB master idle */
#define OTG_FS_AHBIDL (1 << 31)
/* Indicates that the AHB master state machine is in the Idle condition.
   Note: Accessible in both device and host modes. */

/* [30..11] Reserved */

/* Bits [10..6] - TxFIFO number */
#define OTG_FS_TXFIFO ((10 - 6) << 6)
#define OTG_FS_TXFIFO_MSK (((1 << (10 - 6 + 1)) - 1) << 6)
#define OTG_FS_TXFIFO_SET(VAL) (((VAL) << 6) & OTG_FS_TXFIFO_MSK)
#define OTG_FS_TXFIFO_GET(REG) (((REG) & OTG_FS_TXFIFO_MSK) >> 6)
#define OTG_FS_TXFIFO_0 (0x00 << 6)
#define OTG_FS_TXFIFO_1 (0x01 << 6)
#define OTG_FS_TXFIFO_2 (0x02 << 6)
#define OTG_FS_TXFIFO_3 (0x03 << 6)
#define OTG_FS_TXFIFO_ALL (0x10 << 6)
#define TXFIFO_ALL 0x10
/* This is the FIFO number that must be flushed using the TxFIFO Flush bit. 
   This field must not be changed until the core clears the TxFIFO Flush bit.
   ● 00000: 
   – Non-periodic TxFIFO flush in host mode
   – Tx FIFO 0 flush in device mode
   ● 00001:
   – Periodic TxFIFO flush in host mode
   – TXFIFO 1 flush in device mode
   ● 00010: TXFIFO 2 flush in device mode
   ...
   ● 00101: TXFIFO 15 flush in device mode
   ● 10000: Flush all the transmit FIFOs in device or host mode.
   Note: Accessible in both device and host modes. */

/* Bit 5 - TxFIFO flush */
#define OTG_FS_TXFFLSH (1 << 5)
/* This bit selectively flushes a single or all transmit FIFOs, but cannot 
   do so if the core is in the midst of a transaction.
   The application must write this bit only after checking that the core 
   is neither writing to the TxFIFO nor reading from the TxFIFO. Verify 
   using these registers:
   Read—NAK Effective Interrupt ensures the core is not reading from the 
   FIFO Write—AHBIDL bit in OTG_FS_GRSTCTL ensures the core is not 
   writing anything to the FIFO.
   Note: Accessible in both device and host modes. */

/* Bit 4 - RxFIFO flush */
#define OTG_FS_RXFFLSH (1 << 4)
/* The application can flush the entire RxFIFO using this bit, but must 
   first ensure that the core is not in the middle of a transaction.
   The application must only write to this bit after checking that the 
   core is neither reading from the RxFIFO nor writing to the RxFIFO.
   The application must wait until the bit is cleared before performing 
   any other operations. This bit requires 8 clocks (slowest of PHY or 
   AHB clock) to clear.
   Note: Accessible in both device and host modes. */

/* Bit 3 Reserved */

/* Bit 2 - Host frame counter reset */
#define OTG_FS_FCRST (1 << 2)
/* The application writes this bit to reset the frame number counter 
   inside the core. When the frame counter is reset, the subsequent SOF 
   sent out by the core has a frame number of 0.
   Note: Only accessible in host mode. */

/* Bit 1 - HCLK soft reset */
#define OTG_FS_HSRST (1 << 1)
/* The application uses this bit to flush the control logic in the AHB 
   Clock domain. Only AHB Clock
   Domain pipelines are reset.
   FIFOs are not flushed with this bit.
   All state machines in the AHB clock domain are reset to the Idle state 
   after terminating the transactions on the AHB, following the protocol.
   CSR control bits used by the AHB clock domain state machines are cleared.
   To clear this interrupt, status mask bits that control the interrupt 
   status and are generated by the AHB clock domain state machine are cleared.
   Because interrupt status bits are not cleared, the application can get 
   the status of any core events that occurred after it set this bit.
   This is a self-clearing bit that the core clears after all necessary 
   logic is reset in the core. This can take several clocks, depending on 
   the core’s current state.
   Note: Accessible in both device and host modes. */

/* Bit 0 - Core soft reset */
#define OTG_FS_CSRST (1 << 0)
/* Resets the HCLK and PCLK domains as follows: 
   Clears the interrupts and all the CSR register bits except 
   for the following bits:
   – RSTPDMODL bit in OTG_FS_PCGCCTL
   – GAYEHCLK bit in OTG_FS_PCGCCTL
   – PWRCLMP bit in OTG_FS_PCGCCTL
   – STPPCLK bit in OTG_FS_PCGCCTL
   – FSLSPCS bit in OTG_FS_HCFG
   – DSPD bit in OTG_FS_DCFG
   All module state machines (except for the AHB slave unit) are reset 
   to the Idle state, and all the transmit FIFOs and the receive FIFO 
   are flushed.
   Any transactions on the AHB Master are terminated as soon as possible, 
   after completing the last data phase of an AHB transfer. Any 
   transactions on the USB are terminated immediately.
   The application can write to this bit any time it wants to reset the 
   core. This is a self-clearing bit and the core clears this bit after 
   all the necessary logic is reset in the core, which can take several 
   clocks, depending on the current state of the core. Once this bit has 
   been cleared, the software must wait at least 3 PHY clocks before 
   accessing the PHY domain (synchronization delay). The software must 
   also check that bit 31 in this register is set to 1 (AHB Master is Idle)
   before starting any operation.
   Typically, the software reset is used during software development and 
   also when you dynamically change the PHY selection bits in the above 
   listed USB configuration registers.
   When you change the PHY, the corresponding clock for the PHY is 
   selected and used in the PHY domain. Once a new clock is selected, the 
   PHY domain has to be reset for proper operation.
   Note: Accessible in both device and host modes. */


/* -------------------------------------------------------------------------
 * OTG_FS core interrupt register */
#define STM32F_OTG_FS_GINTSTS 0x014


/* Bit 31 - Resume/remote wakeup detected interrupt */
#define OTG_FS_WKUPINT (1 << 31)
/* In device mode, this interrupt is asserted when a resume is detected 
   on the USB. In host mode, this interrupt is asserted when a remote 
   wakeup is detected on the USB.
   Note: Accessible in both device and host modes. */

/* Bit 30 - Session request/new session detected interrupt */
#define OTG_FS_SRQINT (1 << 30)
/* In host mode, this interrupt is asserted when a session request is 
   detected from the device. In device mode, this interrupt is asserted 
   when VBUS is in the valid range for a B-peripheral device. 
   Accessible in both device and host modes. */

/* Bit 29 - Disconnect detected interrupt */
#define OTG_FS_DISCINT (1 << 29)
/* Asserted when a device disconnect is detected.
   Note: Only accessible in host mode. */

/* Bit 28 - Connector ID status change */
#define OTG_FS_CIDSCHG (1 << 28)
/* The core sets this bit when there is a change in connector ID status.
   Note: Accessible in both device and host modes. */

/* Bit 27 Reserved */


/* Bit 26 - Periodic TxFIFO empty */
#define OTG_FS_PTXFE (1 << 26)
/* Asserted when the periodic transmit FIFO is either half or completely 
   empty and there is space for at least one entry to be written in 
   the periodic request queue. The half or completely empty
   status is determined by the periodic TxFIFO empty level bit in the 
   OTG_FS_GAHBCFG register (PTXFELVL bit in OTG_FS_GAHBCFG).
   Note: Only accessible in host mode. */

/* Bit 25 - Host channels interrupt */
#define OTG_FS_HCINT (1 << 25)
/* The core sets this bit to indicate that an interrupt is pending on 
   one of the channels of the core (in host mode). The application must 
   read the OTG_FS_HAINT register to determine the exact number of the 
   channel on which the interrupt occurred, and then read the 
   corresponding OTG_FS_HCINTx register to determine the exact cause of 
   the interrupt. The application must clear the appropriate status bit 
   in the OTG_FS_HCINTx register to clear this bit.
   Note: Only accessible in host mode. */

/* Bit 24 - Host port interrupt */
#define OTG_FS_HPRTINT (1 << 24)
/* The core sets this bit to indicate a change in port status of one of 
   the OTG_FS controller ports in host mode. The application must read 
   the OTG_FS_HPRT register to determine the exact event that caused 
   this interrupt. The application must clear the appropriate status bit 
   in the OTG_FS_HPRT register to clear this bit.
   Note: Only accessible in host mode. */

/* [23..22] Reserved */


/* Bit 21 - Incomplete periodic transfer */
#define OTG_FS_IPXFR (1 << 21)
/* In host mode, the core sets this interrupt bit when there are 
   incomplete periodic transactions still pending, which are scheduled 
   for the current frame. */

/* Bit 21 - Incomplete isochronous OUT transfer */
#define OTG_FS_INCOMPISOOUT (1 << 21)
/* In device mode, the core sets this interrupt to indicate that there 
   is at least one isochronous OUT endpoint on which the transfer is 
   not completed in the current frame. This interrupt is asserted along 
   with the End of periodic frame interrupt (EOPF) bit in this register. */

/* Bit 20 - Incomplete isochronous IN transfer */
#define OTG_FS_IISOIXFR (1 << 20)
/* The core sets this interrupt to indicate that there is at least one 
   isochronous IN endpoint on which the transfer is not completed in the 
   current frame. This interrupt is asserted along with the End of 
   periodic frame interrupt (EOPF) bit in this register.
   Note: Only accessible in device mode. */

/* Bit 19 - OUT endpoint interrupt */
#define OTG_FS_OEPINT (1 << 19)
/* The core sets this bit to indicate that an interrupt is pending on 
   one of the OUT endpoints of the core (in device mode). The 
   application must read the OTG_FS_DAINT register to determine the 
   exact number of the OUT endpoint on which the interrupt occurred, 
   and then read the corresponding OTG_FS_DOEPINTx register to 
   determine the exact cause of the interrupt. The application must 
   clear the appropriate status bit in the corresponding OTG_FS_DOEPINTx 
   register to clear this bit.
   Note: Only accessible in device mode. */

/* Bit 18 - IN endpoint interrupt */
#define OTG_FS_IEPINT (1 << 18)
/* The core sets this bit to indicate that an interrupt is pending on 
   one of the IN endpoints of the core (in device mode). The application 
   must read the OTG_FS_DAINT register to determine the exact number of 
   the IN endpoint on which the interrupt occurred, and then read the
   corresponding OTG_FS_DIEPINTx register to determine the exact cause of 
   the interrupt. The application must clear the appropriate status bit 
   in the corresponding OTG_FS_DIEPINTx register to clear this bit.
   Note: Only accessible in device mode. */

/* [17..16] Reserved */


/* Bit 15 - End of periodic frame interrupt */
#define OTG_FS_EOPF (1 << 15)
/* Indicates that the period specified in the periodic frame interval 
   field of the OTG_FS_DCFG register (PFIVL bit in OTG_FS_DCFG) has 
   been reached in the current frame.
   Note: Only accessible in device mode. */

/* Bit 14 - Isochronous OUT packet dropped interrupt */
#define OTG_FS_ISOODRP (1 << 14)
/* The core sets this bit when it fails to write an isochronous OUT 
   packet into the RxFIFO because the RxFIFO does not have enough 
   space to accommodate a maximum size packet for the isochronous 
   OUT endpoint.
   Note: Only accessible in device mode. */

/* Bit 13 - Enumeration done */
#define OTG_FS_ENUMDNE (1 << 13)
/* The core sets this bit to indicate that speed enumeration is 
   complete. The application must read the OTG_FS_DSTS register to 
   obtain the enumerated speed.
   Note: Only accessible in device mode. */

/* Bit 12 - USB reset */
#define OTG_FS_USBRST (1 << 12)
/* The core sets this bit to indicate that a reset is detected on the USB.
   Note: Only accessible in device mode. */

/* Bit 11 - USB suspend */
#define OTG_FS_USBSUSP (1 << 11)
/* The core sets this bit to indicate that a suspend was detected on the 
   USB. The core enters the Suspended state when there is no activity on 
   the data lines for a period of 3 ms.
   Note: Only accessible in device mode. */

/* Bit 10 - Early suspend */
#define OTG_FS_ESUSP (1 << 10)
/* The core sets this bit to indicate that an Idle state has been 
   detected on the USB for 3 ms.
   Note: Only accessible in device mode. */

/* [9..8] Reserved */


/* Bit 7 - Global OUT NAK effective */
#define OTG_FS_GONAKEFF (1 << 7)
/* Indicates that the Set global OUT NAK bit in the OTG_FS_DCTL register 
   (SGONAK bit in OTG_FS_DCTL), set by the application, has taken effect 
   in the core. This bit can be cleared by writing the Clear global OUT 
   NAK bit in the OTG_FS_DCTL register (CGONAK bit in OTG_FS_DCTL).
   Note: Only accessible in device mode. */

/* Bit 6 - Global IN non-periodic NAK effective */
#define OTG_FS_GINAKEFF (1 << 6)
/* Indicates that the Set global non-periodic IN NAK bit in the 
   OTG_FS_DCTL register (SGINAK bit in OTG_FS_DCTL), set by the 
   application, has taken effect in the core. That is, the core has 
   sampled the Global IN NAK bit set by the application. This bit can 
   be cleared by clearing the Clear global non-periodic IN NAK bit in 
   the OTG_FS_DCTL register (CGINAK bit in OTG_FS_DCTL).
   This interrupt does not necessarily mean that a NAK handshake is sent 
   out on the USB. The STALL bit takes precedence over the NAK bit.
   Note: Only accessible in device mode. */

/* Bit 5 - Non-periodic TxFIFO empty */
#define OTG_FS_NPTXFE (1 << 5)
/* This interrupt is asserted when the non-periodic TxFIFO is either 
   half or completely empty, and there is space for at least one entry 
   to be written to the non-periodic transmit request queue. The half or 
   completely empty status is determined by the non-periodic TxFIFO 
   empty level bit in the OTG_FS_GAHBCFG register (TXFELVL bit 
   in OTG_FS_GAHBCFG).
   Note: Accessible in host mode only. */

/* Bit 4 - RxFIFO non-empty */
#define OTG_FS_RXFLVL (1 << 4)
/* Indicates that there is at least one packet pending to be read from 
   the RxFIFO.
   Note: Accessible in both host and device modes. */

/* Bit 3 - Start of frame */
#define OTG_FS_SOF (1 << 3)
/* In host mode, the core sets this bit to indicate that an SOF (FS), 
   or Keep-Alive (LS) is transmitted on the USB. The application must 
   write a 1 to this bit to clear the interrupt. In device mode, in the 
   core sets this bit to indicate that an SOF token has been received on 
   the USB. The application can read the Device Status register to get 
   the current frame number.
   This interrupt is seen only when the core is operating in FS.
   Note: Accessible in both host and device modes. */

/* Bit 2 - OTG interrupt */
#define OTG_FS_OTGINT (1 << 2)
/* The core sets this bit to indicate an OTG protocol event. The 
   application must read the OTG Interrupt Status (OTG_FS_GOTGINT) 
   register to determine the exact event that caused this interrupt. 
   The application must clear the appropriate status bit in the 
   OTG_FS_GOTGINT register to clear this bit.
   Note: Accessible in both host and device modes. */

/* Bit 1 - Mode mismatch interrupt */
#define OTG_FS_MMIS (1 << 1)
/* The core sets this bit when the application is trying to access:
   A host mode register, when the core is operating in device mode
   A device mode register, when the core is operating in host mode
   The register access is completed on the AHB with an OKAY response, 
   but is ignored by the core internally and does not affect the 
   operation of the core.
   Note: Accessible in both host and device modes. */

/* Bit 0 - Current mode of operation */
#define OTG_FS_CMOD (1 << 0)
/* Indicates the current mode.
   0: Device mode
   1  : Host mode
   Note: Accessible in both host and device modes. */



/* -------------------------------------------------------------------------
 * OTG_FS interrupt mask register */
#define STM32F_OTG_FS_GINTMSK 0x018


/* Bit 31 - Resume/remote wakeup detected interrupt mask */
#define OTG_FS_WUIM (1 << 31)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Accessible in both host and device modes. */

/* Bit 30 - Session request/new session detected interrupt mask */
#define OTG_FS_SRQIM (1 << 30)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Accessible in both host and device modes. */

/* Bit 29 - Disconnect detected interrupt mask */
#define OTG_FS_DISCINT (1 << 29)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* Bit 28 - Connector ID status change mask */
#define OTG_FS_CIDSCHGM (1 << 28)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Accessible in both host and device modes. */

/* Bit 27 Reserved */

/* Bit 26 - Periodic TxFIFO empty mask */
#define OTG_FS_PTXFEM (1 << 26)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in host mode. */

/* Bit 25 - Host channels interrupt mask */
#define OTG_FS_HCIM (1 << 25)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in host mode. */

/* Bit 24 - Host port interrupt mask */
#define OTG_FS_PRTIM (1 << 24)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in host mode. */

/* [23..22] Reserved */

/* Bit 21 - Incomplete periodic transfer mask */
#define OTG_FS_IPXFRM (1 << 21)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in host mode. */

/* Bit 21 - Incomplete isochronous OUT transfer mask */
#define OTG_FS_IISOOXFRM (1 << 21)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* Bit 20 - Incomplete isochronous IN transfer mask */
#define OTG_FS_IISOIXFRM (1 << 20)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* Bit 19 - OUT endpoints interrupt mask */
#define OTG_FS_OEPINTM (1 << 19)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* Bit 18 - IN endpoints interrupt mask */
#define OTG_FS_IEPINTM (1 << 18)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* Bit 17 - Endpoint mismatch interrupt mask */
#define OTG_FS_EPMISM (1 << 17)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* Bit 16 Reserved */

/* Bit 15 - End of periodic frame interrupt mask */
#define OTG_FS_EOPFM (1 << 15)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* Bit 14 - Isochronous OUT packet dropped interrupt mask */
#define OTG_FS_ISOODRPM (1 << 14)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* Bit 13 - Enumeration done mask */
#define OTG_FS_ENUMDNEM (1 << 13)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* Bit 12 - USB reset mask */
#define OTG_FS_USBRSTM (1 << 12)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* Bit 11 - USB suspend mask */
#define OTG_FS_USBSUSPM (1 << 11)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* Bit 10 - Early suspend mask */
#define OTG_FS_ESUSPM (1 << 10)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* [9..8] Reserved. */

/* Bit 7 - Global OUT NAK effective mask */
#define OTG_FS_GONAKEFFM (1 << 7)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* Bit 6 - Global non-periodic IN NAK effective mask */
#define OTG_FS_GINAKEFFM (1 << 6)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in device mode. */

/* Bit 5 - Non-periodic TxFIFO empty mask */
#define OTG_FS_NPTXFEM (1 << 5)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Only accessible in Host mode. */

/* Bit 4 - Receive FIFO non-empty mask */
#define OTG_FS_RXFLVLM (1 << 4)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Accessible in both device and host modes. */

/* Bit 3 - Start of frame mask */
#define OTG_FS_SOFM (1 << 3)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Accessible in both device and host modes. */

/* Bit 2 - OTG interrupt mask */
#define OTG_FS_OTGINT (1 << 2)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Accessible in both device and host modes. */

/* Bit 1 - Mode mismatch interrupt mask */
#define OTG_FS_MMISM (1 << 1)
/* 0: Masked interrupt
   1: Unmasked interrupt
   Note: Accessible in both device and host modes. */

/* Bit 0 Reserved */

/* -------------------------------------------------------------------------
 * OTG_FS Receive status debug read/OTG status read and pop registers
 * (OTG_FS_GRXSTSR/OTG_FS_GRXSTSP)
 * Address offset for Read: 0x01C
 * Address offset for Pop: 0x020 */
#define STM32F_OTG_FS_GRXSTSR 0x01c
#define STM32F_OTG_FS_GRXSTSP 0x020

/* [31..21] Reserved */

/* Bits [20..17] - Packet status */
#define OTG_FS_PKTSTS ((20 - 17) << 17)
/* Indicates the status of the received packet
   0010: IN data packet received
   0011: IN transfer completed (triggers an interrupt)
   0101: Data toggle error (triggers an interrupt)
   0111: Channel halted (triggers an interrupt)
   Others: Reserved */

/* Bits [16..15] - Data PID */
#define OTG_FS_DPIDX ((16 - 15) << 15)
#define OTG_FS_DPID_MSK (((1 << (16 - 15 + 1)) - 1) << 15)
#define OTG_FS_DPID_GET(REG) (((REG) & OTG_FS_DPID_MSK) >> 15)
#define OTG_FS_DPID_DATA0 (0 << 15)
#define OTG_FS_DPID_DATA1 (1 << 15)
#define OTG_FS_DPID_DATA2 (2 << 15)
#define OTG_FS_DPID_MDATA (3 << 15)
/* Indicates the Data PID of the received packet
   00: DATA0
   10: DATA1
   01: DATA2
   11: MDATA */

/* Bits [14..4] - Byte count */
#define OTG_FS_BCNT ((14 - 4) << 4)
#define OTG_FS_BCNT_MSK (((1 << (14 - 4 + 1)) - 1) << 4)
#define OTG_FS_BCNT_SET(VAL) (((VAL) << 4) & OTG_FS_BCNT_MSK)
#define OTG_FS_BCNT_GET(REG) (((REG) & OTG_FS_BCNT_MSK) >> 4)
/* Indicates the byte count of the received IN data packet. */

/* Bits [3..0] - Channel number */
#define OTG_FS_CHNUM ((3 - 0) << 0)
#define OTG_FS_CHNUM_MSK (((1 << (3 - 0 + 1)) - 1) << 0)
#define OTG_FS_CHNUM_SET(VAL) (((VAL) << 0) & OTG_FS_CHNUM_MSK)
#define OTG_FS_CHNUM_GET(REG) (((REG) & OTG_FS_CHNUM_MSK) >> 0)
/* Indicates the channel number to which the current received 
   packet belongs. */



/* [31..25] Reserved */

/* Bits [24..21] - Frame number */
#define OTG_FS_FRMNUM ((24 - 21) << 21)
/* This is the least significant 4 bits of the frame number in which the 
   packet is received on the USB. This field is supported only when 
   isochronous OUT endpoints are supported. */

/* Bits [20..17] - Packet status */
#define OTG_FS_PKTSTS ((20 - 17) << 17)
#define OTG_FS_PKTSTS_MSK (((1 << (20 - 17 + 1)) - 1) << 17)
#define OTG_FS_PKTSTS_SET(VAL) (((VAL) << 17) & OTG_FS_PKTSTS_MSK)
#define OTG_FS_PKTSTS_GET(REG) (((REG) & OTG_FS_PKTSTS_MSK) >> 17)

#define OTG_FS_PKTSTS_GOUT_NACK  (0x1 << 17)
#define OTG_FS_PKTSTS_OUT_DATA_UPDT  (0x2 << 17)
#define OTG_FS_PKTSTS_OUT_XFER_COMP  (0x3 << 17)
#define OTG_FS_PKTSTS_SETUP_COMP (0x4 << 17)
#define OTG_FS_PKTSTS_SETUP_UPDT (0x6 << 17)
/* Indicates the status of the received packet
   0001: Global OUT NAK (triggers an interrupt)
   0010: OUT data packet received
   0011: OUT transfer completed (triggers an interrupt)
   0100: SETUP transaction completed (triggers an interrupt)
   0110: SETUP data packet received
   Others: Reserved */

/* Bits [16..15] - Data PID */
#define OTG_FS_DPID_STAT ((16 - 15) << 15)
/* Indicates the Data PID of the received OUT data packet
   00: DATA0
   10: DATA1
   01: DATA2
   11: MDATA */

/* Bits [14..4] - Byte count */
#define OTG_FS_BCNT ((14 - 4) << 4)
/* Indicates the byte count of the received data packet. */

/* Bits [3..0] - Endpoint number */
#define _OTG_FS_EPNUM ((3 - 0) << 0)
#define OTG_FS_EPNUM_MSK (((1 << (3 - 0 + 1)) - 1) << 0)
#define OTG_FS_EPNUM_SET(VAL) (((VAL) << 0) & OTG_FS_EPNUM_MSK)
#define OTG_FS_EPNUM_GET(REG) (((REG) & OTG_FS_EPNUM_MSK) >> 0)
/* Indicates the endpoint number to which the current received 
   packet belongs. */


/* -------------------------------------------------------------------------
 * OTG_FS Receive FIFO size register */
#define STM32F_OTG_FS_GRXFSIZ 0x024

/* [31..16] Reserved */

/* Bits [15..0] - RxFIFO depth */
#define OTG_FS_RXFD ((15 - 0) << 0)
/* This value is in terms of 32-bit words.
   Minimum value is 16
   Maximum value is 256
   The power-on reset value of this register is specified as the largest 
   Rx data FIFO depth. */

/* -------------------------------------------------------------------------
 * OTG_FS Host non-periodic transmit FIFO size register */
#define STM32F_OTG_FS_HNPTXFSIZ 0x028

/* Bits [31..16] - Non-periodic TxFIFO depth */
#define OTG_FS_NPTXFD ((31 - 16) << 16)
/* This value is in terms of 32-bit words.
   Minimum value is 16
   Maximum value is 256 */

/* Bits [15..0] - Non-periodic transmit RAM start address */
#define OTG_FS_NPTXFSA ((15 - 0) << 0)
/* This field contains the memory start address for non-periodic 
   transmit FIFO RAM.
   Device mode */

/* -------------------------------------------------------------------------
 * Endpoint 0 Transmit FIFO size register */
#define STM32F_OTG_FS_DIEPTXF0 0x028 

/* Bits [31..16] - Endpoint 0 TxFIFO depth */
#define OTG_FS_TX0FD ((31 - 16) << 16)
#define OTG_FS_TX0FD_MSK (((1 << (31 - 16 + 1)) - 1) << 16)
#define OTG_FS_TX0FD_SET(VAL) (((VAL) << 16) & OTG_FS_TX0FD_MSK)
#define OTG_FS_TX0FD_GET(REG) (((REG) & OTG_FS_TX0FD_MSK) >> 16)
/* This value is in terms of 32-bit words.
   Minimum value is 16
   Maximum value is 256 */

/* Bits [15..0] - Endpoint 0 transmit RAM start address */
#define OTG_FS_TX0FSA ((15 - 0) << 0)
#define OTG_FS_TX0FSA_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_FS_TX0FSA_SET(VAL) (((VAL) << 0) & OTG_FS_TX0FSA_MSK)
#define OTG_FS_TX0FSA_GET(REG) (((REG) & OTG_FS_TX0FSA_MSK) >> 0)
/* This field contains the memory start address for the endpoint 0 
   transmit FIFO RAM. */


/* OTG_FS non-periodic transmit FIFO/queue status register */
#define STM32F_OTG_FS_HNPTXSTS 0x02C

/* Bits [30..24] - Top of the non-periodic transmit request queue */
#define OTG_FS_NPTXQTOP ((30 - 24) << 24)
/* Entry in the non-periodic Tx request queue that is currently being 
   processed by the MAC.
   Bits [30:27]: Channel/endpoint number
   Bits [26:25]:
   – 00: IN/OUT token
   – 01: Zero-length transmit packet (device IN/host OUT)
   – 11: Channel halt command
   Bit [24]: Terminate (last entry for selected channel/endpoint) */

/* Bits [23..16] - Non-periodic transmit request queue space available */
#define OTG_FS_NPTQXSAV ((23 - 16) << 16)
/* Indicates the amount of free space available in the non-periodic 
   transmit request queue. This queue holds both IN and OUT requests 
   in host mode. Device mode has only IN requests.
   00: Non-periodic transmit request queue is full
   01: dx1 location available
   10: dx2 locations available
   bxn: dxn locations available (0 ≤ n ≤ dx8) Others: Reserved */

/* Bits [15..0] - Non-periodic TxFIFO space available */
#define OTG_FS_NPTXFSAV ((15 - 0) << 0)
/* Indicates the amount of free space available in the non-periodic TxFIFO.
   Values are in terms of 32-bit words.
   00: Non-periodic TxFIFO is full
   01: dx1 word available
   10: dx2 words available
   0xn: dxn words available (where 0 ≤ n ≤ dx256)
   Others: Reserved */


/* -------------------------------------------------------------------------
 * OTG_FS general core configuration register */
#define STM32F_OTG_FS_GCCFG 0x038


/* [31..22] Reserved */


/* Bit 21 - VBUS sensing disable option */
#define OTG_FS_NOVBUSSENS (1 << 21)
/* When this bit is set, VBUS is considered internally to be always at 
   VBUS valid level (5 V). This option removes the need for a dedicated 
   VBUS pad, and leave this pad free to be used for other purposes such as 
   a shared functionality. VBUS connection can be remapped on another 
   general purpose input pad and monitored by software.
   This option is only suitable for host-only or device-only applications.
   0: VBUS sensing available by hardware
   1: VBUS sensing not available by hardware. */

/* Bit 20 - SOF output enable */
#define OTG_FS_SOFOUTEN (1 << 20)
/* 0: SOF pulse not available on PAD
   1: SOF pulse available on PAD */

/* Bit 19 - Enable the VBUS sensing “B” device */
#define OTG_FS_VBUSBSEN (1 << 19)
/* 0: VBUS sensing “B” disabled
   1: VBUS sensing “B” enabled */

/* Bit 18 - Enable the VBUS sensing “A” device */
#define OTG_FS_VBUSASEN (1 << 18)
/* 0: VBUS sensing “A” disabled
   1: VBUS sensing “A” enabled */

/* Bit 17 Reserved */


/* Bit 16 - Power down */
#define OTG_FS_PWRDWN (1 << 16)
/* Used to activate the transceiver in transmission/reception
  0: Power down active
  1: Power down deactivated (“Transceiver active”) */

/* [15..0] Reserved. */


/* -------------------------------------------------------------------------
 * OTG_FS core ID register */
#define STM32F_OTG_FS_CID 0x03C

/* Bits [31..0] - Product ID field */
#define OTG_FS_PRODUCT_ID ((31 - 0) << 0)
/* Application-programmable ID field. */


/* -------------------------------------------------------------------------
 * OTG_FS Host periodic transmit FIFO size register */
#define STM32F_OTG_FS_HPTXFSIZ 0x100


/* Bits [31..16] - Host periodic TxFIFO depth */
#define OTG_FS_PTXFD ((31 - 16) << 16)
#define OTG_FS_PTXFD ((31 - 16) << 16)
#define OTG_FS_PTXFD_MSK (((1 << (31 - 16 + 1)) - 1) << 16)
#define OTG_FS_PTXFD_SET(VAL) (((VAL) << 16) & OTG_FS_PTXFD_MSK)
#define OTG_FS_PTXFD_GET(REG) (((REG) & OTG_FS_PTXFD_MSK) >> 16)
/* This value is in terms of 32-bit words.
   Minimum value is 16 */

/* Bits [15..0] - Host periodic TxFIFO start address */
#define OTG_FS_PTXSA ((15 - 0) << 0)
#define OTG_FS_PTXFSA_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_FS_PTXFSA_SET(VAL) (((VAL) << 0) & OTG_FS_PTXFSA_MSK)
#define OTG_FS_PTXFSA_GET(REG) (((REG) & OTG_FS_PTXFSA_MSK) >> 0)
/* The power-on reset value of this register is the sum of the largest 
   Rx data FIFO depth and largest non-periodic Tx data FIFO depth. */


/* -------------------------------------------------------------------------
 * OTG_FS device IN endpoint transmit FIFO size register (OTG_FS_DIEPTXFx)
 * (x = 1..3, where x is the FIFO_number)
 * Address offset: 0x104 + (FIFO_number – 1) × 0x04 */
#define OTG_FS_DIEPTXF1 (0x104 + (1 – 1) × 0x04)
#define OTG_FS_DIEPTXF2 (0x104 + (2 – 1) × 0x04)
#define OTG_FS_DIEPTXF3 (0x104 + (3 – 1) × 0x04)

/* Bits [31..16] - IN endpoint TxFIFO depth */
#define OTG_FS_INEPTXFD ((31 - 16) << 16)
#define OTG_FS_INEPTXFD_MSK (((1 << (31 - 16 + 1)) - 1) << 16)
#define OTG_FS_INEPTXFD_SET(VAL) (((VAL) << 16) & OTG_FS_INEPTXFD_MSK)
#define OTG_FS_INEPTXFD_GET(REG) (((REG) & OTG_FS_INEPTXFD_MSK) >> 16)
/* This value is in terms of 32-bit words.
   Minimum value is 16
   The power-on reset value of this register is specified as the 
   largest IN endpoint FIFO number depth. */

/* Bits [15..0] - IN endpoint FIFOx transmit RAM start address */
#define OTG_FS_INEPTXSA ((15 - 0) << 0)
#define OTG_FS_INEPTXSA_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_FS_INEPTXSA_SET(VAL) (((VAL) << 0) & OTG_FS_INEPTXSA_MSK)
#define OTG_FS_INEPTXSA_GET(REG) (((REG) & OTG_FS_INEPTXSA_MSK) >> 0)
/* This field contains the memory start address for IN endpoint 
   transmit FIFOx. */


/* -------------------------------------------------------------------------
 * OTG_FS Host configuration register */
#define STM32F_OTG_FS_HCFG 0x400
/* This register configures the core after power-on. Do not make changes 
   to this register after initializing the host. */

/* [31..3] Reserved */


/* Bit 2 - FS- and LS-only support */
#define OTG_FS_FSLSS (1 << 2)
/* 
The application uses this bit to control the core’s enumeration speed. Using this bit, the
application can make the core enumerate as an FS host, even if the connected device
supports HS traffic. Do not make changes to this field after initial programming.
1: FS/LS-only, even if the connected device can support HS (read-only) */

/* Bits [1..0] - FS/LS PHY clock select */
#define OTG_FS_FSLSPCS ((1 - 0) << 0)
/* 
When the core is in FS host mode
01: PHY clock is running at 48 MHz
Others: Reserved
When the core is in LS host mode
00: Reserved
01: Select 48 MHz PHY clock frequency
10: Select 6 MHz PHY clock frequency
11: Reserved
Note: The FSLSPCS must be set on a connection event according to the speed of the
connected device (after changing this bit, a software reset must be performed). */

/* OTG_FS Host frame interval register */
#define STM32F_OTG_FS_HFIR 0x404



/* [31..16] Reserved */


/* Bits [15..0] - Frame interval */
#define OTG_FS_FRIVL ((15 - 0) << 0)
/* 
The value that the application programs to this field specifies the interval between two
consecutive SOFs (FS) or Keep-Alive tokens (LS). This field contains the number of PHY
clocks that constitute the required frame interval. The application can write a value to this
register only after the Port enable bit of the host port control and status register (PENA bit in
OTG_FS_HPRT) has been set. If no value is programmed, the core calculates the value based
on the PHY clock specified in the FS/LS PHY Clock Select field of the host configuration
register (FSLSPCS in OTG_FS_HCFG). Do not change the value of this field after the initial
configuration.
1 ms × (PHY clock frequency) */

/* OTG_FS Host frame number/frame time remaining register */
#define STM32F_OTG_FS_HFNUM 0x408



/* Bits [31..16] - Frame time remaining */
#define OTG_FS_FTREM ((31 - 16) << 16)
/* 
Indicates the amount of time remaining in the current frame, in terms of PHY clocks. This field
decrements on each PHY clock. When it reaches zero, this field is reloaded with the value in
the Frame interval register and a new SOF is transmitted on the USB. */

/* Bits [15..0] - Frame number */
#define OTG_FS_FRNUM ((15 - 0) << 0)
/* 
This field increments when a new SOF is transmitted on the USB, and is cleared to 0 when it
reaches 0x3FFF. */

/* OTG_FS_Host periodic transmit FIFO/queue status register */
#define OTG_FS_HPTXSTS  0x410


/* Bits [31..24] - Top of the periodic transmit request queue */
#define OTG_FS_PTXQTOP ((31 - 24) << 24)
/* 
This indicates the entry in the periodic Tx request queue that is currently being processed by
the MAC.
This register is used for debugging.
Bit [31]: Odd/Even frame
–
0: send in even frame
–
1: send in odd frame
Bits [30:27]: Channel/endpoint number
Bits [26:25]: Type
–
00: IN/OUT
–
01: Zero-length packet
–
11: Disable channel command
Bit [24]: Terminate (last entry for the selected channel/endpoint) */

/* Bits [23..16] - Periodic transmit request queue space available */
#define OTG_FS_PTXQSAV ((23 - 16) << 16)
/* 
Indicates the number of free locations available to be written in the periodic transmit request
queue. This queue holds both IN and OUT requests.
00: Periodic transmit request queue is full
01: dx1 location available
10: dx2 locations available
bxn: dxn locations available (0 ≤ dxn ≤ 8)
Others: Reserved */

/* Bits [15..0] - Periodic transmit data FIFO space available */
#define OTG_FS_PTXFSAVL ((15 - 0) << 0)
/* 
Indicates the number of free locations available to be written to in the periodic TxFIFO.
Values are in terms of 32-bit words
0000: Periodic TxFIFO is full
0001: dx1 word available
0010: dx2 words available
bxn: dxn words available (where 0 ≤ dxn ≤ PTXFD)
Others: Reserved */

/* OTG_FS Host all channels interrupt register */
#define STM32F_OTG_FS_HAINT 0x414



/* [31..16] Reserved */


/* Bits [15..0] - Channel interrupts */
#define OTG_FS_HAINT ((15 - 0) << 0)
/* 
One bit per channel: Bit 0 for Channel 0, bit 15 for Channel 15 */

/* OTG_FS Host all channels interrupt mask register */
#define STM32F_OTG_FS_HAINTMSK 0x418



/* [31..16] Reserved */


/* Bits [15..0] - Channel interrupt mask */
#define OTG_FS_HAINTM ((15 - 0) << 0)
/* 
0: Masked interrupt
1: Unmasked interrupt
One bit per channel: Bit 0 for channel 0, bit 15 for channel 15 */

/* OTG_FS Host port control and status register */
#define STM32F_OTG_FS_HPRT 0x440



/* [31..19] Reserved */


/* Bits [18..17] - Port speed */
#define OTG_FS_PSPD ((18 - 17) << 17)
/* 
Indicates the speed of the device attached to this port.
01: Full speed
10: Low speed
11: Reserved */

/* Bits [16..13] - Port test control */
#define OTG_FS_PTCTL ((16 - 13) << 13)
/* 
The application writes a nonzero value to this field to put the port into a Test mode, and the
corresponding pattern is signaled on the port.
0000: Test mode disabled
0001: Test_J mode
0010: Test_K mode
0011: Test_SE0_NAK mode
0100: Test_Packet mode
0101: Test_Force_Enable
Others: Reserved */

/* Bit 12 - Port power */
#define OTG_FS_PPWR (1 << 12)
/* 
The application uses this field to control power to this port, and the core clears this bit on an
overcurrent condition.
0: Power off
1: Power on */

/* Bits [11..10] - Port line status */
#define OTG_FS_PLSTS ((11 - 10) << 10)
/* 
Indicates the current logic level USB data lines
Bit [10]: Logic level of OTG_FS_FS_DP
Bit [11]: Logic level of OTG_FS_FS_DM */

/* Bit 9 Reserved */


/* Bit 8 - Port reset */
#define OTG_FS_PRST (1 << 8)
/* 
When the application sets this bit, a reset sequence is started on this port. The application
must time the reset period and clear this bit after the reset sequence is complete.
0: Port not in reset
1: Port in reset
The application must leave this bit set for a minimum duration of at least 10 ms to start a reset
on the port. The application can leave it set for another 10 ms in addition to the required
minimum duration, before clearing the bit, even though there is no maximum limit set by the
USB standard. */

/* Bit 7 - Port suspend */
#define OTG_FS_PSUSP (1 << 7)
/* 
The application sets this bit to put this port in Suspend mode. The core only stops sending
SOFs when this is set. To stop the PHY clock, the application must set the Port clock stop bit,
which asserts the suspend input pin of the PHY.
The read value of this bit reflects the current suspend status of the port. This bit is cleared by
the core after a remote wakeup signal is detected or the application sets the Port reset bit or
Port resume bit in this register or the Resume/remote wakeup detected interrupt bit or
Disconnect detected interrupt bit in the Core interrupt register (WKUINT or DISCINT in
OTG_FS_GINTSTS, respectively).
0: Port not in Suspend mode
1: Port in Suspend mode */

/* Bit 6 - Port resume */
#define OTG_FS_PRES (1 << 6)
/* 
The application sets this bit to drive resume signaling on the port. The core continues to drive
the resume signal until the application clears this bit.
If the core detects a USB remote wakeup sequence, as indicated by the Port resume/remote
wakeup detected interrupt bit of the Core interrupt register (WKUINT bit in
OTG_FS_GINTSTS), the core starts driving resume signaling without application intervention
and clears this bit when it detects a disconnect condition. The read value of this bit indicates
whether the core is currently driving resume signaling.
0: No resume driven
1: Resume driven */

/* Bit 5 - Port overcurrent change */
#define OTG_FS_POCCHNG (1 << 5)
/* 
The core sets this bit when the status of the Port overcurrent active bit (bit 4) in this register
changes. */

/* Bit 4 - Port overcurrent active */
#define OTG_FS_POCA (1 << 4)
/* 
Indicates the overcurrent condition of the port.
0: No overcurrent condition
1: Overcurrent condition */

/* Bit 3 - Port enable/disable change */
#define OTG_FS_PENCHNG (1 << 3)
/* 
The core sets this bit when the status of the Port enable bit [2] in this register changes. */

/* Bit 2 - Port enable */
#define OTG_FS_PENA (1 << 2)
/* 
A port is enabled only by the core after a reset sequence, and is disabled by an overcurrent
condition, a disconnect condition, or by the application clearing this bit. The application cannot
set this bit by a register write. It can only clear it to disable the port. This bit does not trigger
any interrupt to the application.
0: Port disabled
1: Port enabled */

/* Bit 1 - Port connect detected */
#define OTG_FS_PCDET (1 << 1)
/* 
The core sets this bit when a device connection is detected to trigger an interrupt to the
application using the host port interrupt bit in the Core interrupt register (HPRTINT bit in
OTG_FS_GINTSTS). The application must write a 1 to this bit to clear the interrupt. */

/* Bit 0 - Port connect status */
#define OTG_FS_PCSTS (1 << 0)
/* 
0: No device is attached to the port
1: A device is attached to the port
OTG_FS Host channel-x characteristics register (OTG_FS_HCCHARx)
(x = 0..7, where x = Channel_number)
Address offset: 0x500 + (Channel_number × 0x20) */


/* Bit 31 - Channel enable */
#define OTG_FS_CHENA (1 << 31)
/* 
This field is set by the application and cleared by the OTG host.
0: Channel disabled
1: Channel enabled */

/* Bit 30 - Channel disable */
#define OTG_FS_CHDIS (1 << 30)
/* 
The application sets this bit to stop transmitting/receiving data on a channel, even before the
transfer for that channel is complete. The application must wait for the Channel disabled
interrupt before treating the channel as disabled. */

/* Bit 29 - Odd frame */
#define OTG_FS_ODDFRM (1 << 29)
/* 
This field is set (reset) by the application to indicate that the OTG host must perform a transfer
in an odd frame. This field is applicable for only periodic (isochronous and interrupt)
transactions.
0: Even frame
1: Odd frame */

/* Bits [28..22] - Device address */
#define _OTG_FS_DAD ((28 - 22) << 22)
/* 
This field selects the specific device serving as the data source or sink. */

/* Bits [21..20] - Multicount */
#define _OTG_FS_MCNT ((21 - 20) << 20)
/* 
This field indicates to the host the number of transactions that must be executed per frame for
this periodic endpoint. For non-periodic transfers, this field is not used
00: Reserved. This field yields undefined results
01: 1 transaction
10: 2 transactions per frame to be issued for this endpoint
11: 3 transactions per frame to be issued for this endpoint
Note: This field must be set to at least 01. */

/* Bits [19..18] - Endpoint type */
#define OTG_FS_EPTYP ((19 - 18) << 18)
/* 
Indicates the transfer type selected.
00: Control
01: Isochronous
10: Bulk
11: Interrupt */

/* Bit 17 - Low-speed device */
#define OTG_FS_LSDEV (1 << 17)
/* 
This field is set by the application to indicate that this channel is communicating to a low-
speed device. */

/* Bit 16 Reserved */


/* Bit 15 - Endpoint direction */
#define OTG_FS_EPDIR (1 << 15)
/* 
Indicates whether the transaction is IN or OUT.
0: OUT
1: IN */

/* Bits [14..11] - Endpoint number */
#define OTG_FS_EPNUM ((14 - 11) << 11)
/* Indicates the endpoint number on the device serving as the data 
   source or sink. */

/* Bits [10..0] - Maximum packet size */
#define OTG_FS_MPSIZ ((10 - 0) << 0)
/* Indicates the maximum packet size of the associated endpoint. */


/* OTG_FS Host channel-x interrupt register (OTG_FS_HCINTx) 
   (x = 0..7, where x = Channel_number)
   Address offset: 0x508 + (Channel_number × 0x20) */

/* [31..11] Reserved */

/* Bit 10 - Data toggle error */
#define OTG_FS_DTERR (1 << 10)

/* Bit 9 - Frame overrun */
#define OTG_FS_FRMOR (1 << 9)

/* Bit 8 - Babble error */
#define OTG_FS_BBERR (1 << 8)

/* Bit 7 - Transaction error */
#define OTG_FS_TXERR (1 << 7)
/* 
Indicates one of the following errors occurred on the USB.
CRC check failure
Timeout
Bit stuff error
False EOP */

/* Bit 6 Reserved */


/* Bit 5 - ACK response received/transmitted interrupt */
#define OTG_FS_ACK (1 << 5)

/* Bit 4 - NAK response received interrupt */
#define OTG_FS_NAK (1 << 4)

/* Bit 3 - STALL response received interrupt */
#define _OTG_FS_STALL (1 << 3)

/* Bit 2 Reserved */


/* Bit 1 - Channel halted */
#define OTG_FS_CHH (1 << 1)
/* 
Indicates the transfer completed abnormally either because of any USB transaction error or in
response to disable request by the application. */

/* Bit 0 - Transfer completed */
#define OTG_FS_XFRC (1 << 0)
/* 
Transfer completed normally without any errors.
OTG_FS Host channel-x interrupt mask register (OTG_FS_HCINTMSKx)
(x = 0..7, where x = Channel_number)
Address offset: 0x50C + (Channel_number × 0x20) */


/* [31..11] Reserved */


/* Bit 10 - Data toggle error mask */
#define OTG_FS_DTERRM (1 << 10)
/* 
0: Masked interrupt
1: Unmasked interrupt */

/* Bit 9 - Frame overrun mask */
#define OTG_FS_FRMORM (1 << 9)
/* 
0: Masked interrupt
1: Unmasked interrupt */

/* Bit 8 - Babble error mask */
#define OTG_FS_BBERRM (1 << 8)
/* 
0: Masked interrupt
1: Unmasked interrupt */

/* Bit 7 - Transaction error mask */
#define OTG_FS_TXERRM (1 << 7)
/* 
0: Masked interrupt
1: Unmasked interrupt */

/* Bit 6 - response received interrupt mask */
#define OTG_FS_NYET (1 << 6)
/* 
0: Masked interrupt
1: Unmasked interrupt */

/* Bit 5 - ACK response received/transmitted interrupt mask */
#define OTG_FS_ACKM (1 << 5)
/* 
0: Masked interrupt
1: Unmasked interrupt */

/* Bit 4 - NAK response received interrupt mask */
#define OTG_FS_NAKM (1 << 4)
/*
0: Masked interrupt
1: Unmasked interrupt */

/* Bit 3 - STALL response received interrupt mask */
#define OTG_FS_STALLM (1 << 3)
/* 
0: Masked interrupt
1: Unmasked interrupt */

/* Bit 2 Reserved */


/* Bit 1 - Channel halted mask */
#define OTG_FS_CHHM (1 << 1)
/* 
0: Masked interrupt
1: Unmasked interrupt */

/* Bit 0 - Transfer completed mask */
#define OTG_FS_XFRCM (1 << 0)
/* 
0: Masked interrupt
1: Unmasked interrupt
OTG_FS Host channel-x transfer size register (OTG_FS_HCTSIZx) (x = 0..7,
where x = Channel_number)
Address offset: 0x510 + (Channel_number × 0x20) */


/* Bit 31 Reserved */


/* Bits [30..29] - Data PID */
#define _OTG_FS_DPID ((30 - 29) << 29)
/* 
The application programs this field with the type of PID to use for the initial transaction. The
host maintains this field for the rest of the transfer.
00: DATA0
01: DATA2
10: DATA1
11: MDATA (non-control)/SETUP (control) */

/* Bits [28..19] - Packet count */
#define OTG_FS_PKTCNT ((28 - 19) << 19)
/* 
This field is programmed by the application with the expected number of packets to be
transmitted (OUT) or received (IN).
The host decrements this count on every successful transmission or reception of an OUT/IN
packet. Once this count reaches zero, the application is interrupted to indicate normal
completion. */

/* Bits [18..0] - Transfer size */
#define OTG_FS_XFRSIZ ((18 - 0) << 0)
/* For an OUT, this field is the number of data bytes the host sends 
   during the transfer.
   For an IN, this field is the buffer size that the application has 
   reserved for the transfer. The application is expected to program this 
   field as an integer multiple of the maximum packet size for IN 
   transactions (periodic and non-periodic). */


/* ------------------------------------------------------------------------- */
/* OTG_FS device configuration register */
#define STM32F_OTG_FS_DCFG 0x800

/* [31..13] Reserved */

/* Bits [12..11] - Periodic frame interval */
#define OTG_FS_PFIVL ((12 - 11) << 11)
#define OTG_FS_PFIVL_MSK (((1 << (12 - 11 + 1)) - 1) << 11)
#define OTG_FS_PFIVL_SET(VAL) (((VAL) << 11) & OTG_FS_PFIVL_MSK)
#define OTG_FS_PFIVL_GET(REG) (((REG) & OTG_FS_PFIVL_MSK) >> 11)
#define OTG_FS_PFIVL_80 (0 << 11)
#define OTG_FS_PFIVL_85 (1 << 11)
#define OTG_FS_PFIVL_90 (2 << 11)
#define OTG_FS_PFIVL_95 (3 << 11)
/* Indicates the time within a frame at which the application must be 
   notified using the end of periodic frame interrupt. This can be used 
   to determine if all the isochronous traffic for that frame is complete.
   00: 80% of the frame interval
   01: 85% of the frame interval
   10: 90% of the frame interval
   11: 95% of the frame interval */

/* Bits [10..4] - Device address */
#define OTG_FS_DAD ((10 - 4) << 4)
#define OTG_FS_DAD_MSK (((1 << (10 - 4 + 1)) - 1) << 4)
#define OTG_FS_DAD_SET(VAL) (((VAL) << 4) & OTG_FS_DAD_MSK)
#define OTG_FS_DAD_GET(REG) (((REG) & OTG_FS_DAD_MSK) >> 4)
/* The application must program this field after every SetAddress 
   control command. */

/* Bit 3 Reserved */

/* Bit 2 - Non-zero-length status OUT handshake */
#define OTG_FS_NZLSOHSK (1 << 2)
/* The application can use this field to select the handshake the core 
   sends on receiving a nonzero-length data packet during the OUT 
   transaction of a control transfer’s Status stage.
   1: Send a STALL handshake on a nonzero-length status OUT transaction 
   and do not send the received OUT packet to the application.
   0: Send the received OUT packet to the application (zero-length or 
   nonzero-length) and send a handshake based on the NAK and STALL bits 
   for the endpoint in the Device endpoint control register. */

/* Bits [1..0] - Device speed */
#define OTG_FS_DSPD ((1 - 0) << 0)
#define OTG_FS_DSPD_FULL (0x3 << 0)
/* Indicates the speed at which the application requires the core to 
   enumerate, or the maximum speed the application can support. However, 
   the actual bus speed is determined only after the chirp sequence is 
   completed, and is based on the speed of the USB host to which the core 
   is connected.
   00: Reserved
   01: Reserved
   10: Reserved
   11: Full speed (USB 1.1 transceiver clock is 48 MHz) */


/* ------------------------------------------------------------------------- */
/* OTG_FS device control register */
#define STM32F_OTG_FS_DCTL 0x804

/* [31..12] Reserved */

/* Bit 11 - Power-on programming done */
#define OTG_FS_POPRGDNE (1 << 11)
/* The application uses this bit to indicate that register programming is 
   completed after a wakeup from power down mode. */

/* Bit 10 - Clear global OUT NAK */
#define OTG_FS_CGONAK (1 << 10)
/* A write to this field clears the Global OUT NAK. */

/* Bit 9 - Set global OUT NAK */
#define OTG_FS_SGONAK (1 << 9)
/* A write to this field sets the Global OUT NAK.
   The application uses this bit to send a NAK handshake on all 
   OUT endpoints.
   The application must set the this bit only after making sure that 
   the Global OUT NAK effective bit in the Core interrupt register 
   (GONAKEFF bit in OTG_FS_GINTSTS) is cleared. */

/* Bit 8 - Clear global IN NAK */
#define OTG_FS_CGINAK (1 << 8)
/* A write to this field clears the Global IN NAK. */

/* Bit 7 - Set global IN NAK */
#define OTG_FS_SGINAK (1 << 7)
/* A write to this field sets the Global non-periodic IN NAK.The 
   application uses this bit to send a NAK handshake on all non-periodic 
   IN endpoints.
   The application must set this bit only after making sure that the 
   Global IN NAK effective bit in the Core interrupt register 
   (GINAKEFF bit in OTG_FS_GINTSTS) is cleared. */

/* Bits [6..4] - Test control */
#define OTG_FS_TCTL ((6 - 4) << 4)
/* 000: Test mode disabled
   001: Test_J mode
   010: Test_K mode
   011: Test_SE0_NAK mode
   100: Test_Packet mode
   101: Test_Force_Enable
   Others: Reserved */

/* Bit 3 - Global OUT NAK status */
#define OTG_FS_GONSTS (1 << 3)
/* 0: A handshake is sent based on the FIFO Status and the NAK and STALL 
   bit settings.
   1: No data is written to the RxFIFO, irrespective of space availability. 
   Sends a NAK handshake on all packets, except on SETUP transactions. 
   All isochronous OUT packets are dropped. */

/* Bit 2 - Global IN NAK status */
#define OTG_FS_GINSTS (1 << 2)
/* 0: A handshake is sent out based on the data availability in 
   the transmit FIFO.
   1: A NAK handshake is sent out on all non-periodic IN endpoints, 
   irrespective of the data availability in the transmit FIFO. */

/* Bit 1 - Soft disconnect */
#define OTG_FS_SDIS (1 << 1)
/* The application uses this bit to signal the USB OTG core to perform a 
   soft disconnect. As long as this bit is set, the host does not see 
   that the device is connected, and the device does not receive signals 
   on the USB. The core stays in the disconnected state until the 
   application clears this bit.
   0: Normal operation. When this bit is cleared after a soft disconnect, 
   the core generates a device connect event to the USB host. When the 
   device is reconnected, the USB host restarts device enumeration.
   1: The core generates a device disconnect event to the USB host. */

/* Bit 0 - Remote wakeup signaling */
#define OTG_FS_RWUSIG (1 << 0)
/* When the application sets this bit, the core initiates remote signaling 
   to wake up the USB host. The application must set this bit to instruct 
   the core to exit the Suspend state. As specified in the USB 2.
   0 specification, the application must clear this bit 1 ms to 15 ms after 
   setting it. */


/* -------------------------------------------------------------------------
 * OTG_FS device status register */
#define STM32F_OTG_FS_DSTS 0x808

/* [31..22] Reserved */

/* Bits [21..8] - Frame number of the received SOF */
#define OTG_FS_FNSOF ((21 - 8) << 8)

/* [7..4] Reserved */

/* Bit 3 - Erratic error */
#define OTG_FS_EERR (1 << 3)
/* The core sets this bit to report any erratic errors.
   Due to erratic errors, the OTG_FS controller goes into Suspended 
   state and an interrupt is generated to the application with Early 
   suspend bit of the OTG_FS_GINTSTS register (ESUSP bit in 
   OTG_FS_GINTSTS). If the early suspend is asserted due to an erratic 
   error, the application can only perform a soft disconnect recover. */

/* Bits [2..1] - Enumerated speed */
#define OTG_FS_ENUMSPD ((2 - 1) << 1)
#define OTG_FS_ENUMSPD_MSK (((1 << (2 - 1 + 1)) - 1) << 1)
#define OTG_FS_ENUMSPD_SET(VAL) (((VAL) << 1) & OTG_FS_ENUMSPD_MSK)
#define OTG_FS_ENUMSPD_GET(REG) (((REG) & OTG_FS_ENUMSPD_MSK) >> 1)
#define OTG_FS_ENUMSPD_FULL (3 << 1)
/* Indicates the speed at which the OTG_FS controller has come up after 
   speed detection through a chirp sequence.
   01: Reserved
   10: Reserved
   11: Full speed (PHY clock is running at 48 MHz)
   Others: reserved */

/* Bit 0 - Suspend status */
#define OTG_FS_SUSPSTS (1 << 0)
/* In device mode, this bit is set as long as a Suspend condition is 
   detected on the USB. The core enters the Suspended state when there is 
   no activity on the USB data lines for a period of 3 ms. The core comes 
   out of the suspend:
   – When there is an activity on the USB data lines
   – When the application writes to the Remote wakeup signaling bit in the 
   OTG_FS_DCTL register (RWUSIG bit in OTG_FS_DCTL). */


/* -------------------------------------------------------------------------
 * OTG_FS device IN endpoint common interrupt mask register */
#define STM32F_OTG_FS_DIEPMSK 0x810

/* [31..7] Reserved */

/* Bit 6 - IN endpoint NAK effective mask */
#define OTG_FS_INEPNEM (1 << 6)
/* 0: Masked interrupt
   1: Unmasked interrupt */

/* Bit 5 - IN token received with EP mismatch mask */
#define OTG_FS_INEPNMM (1 << 5)
/* 0: Masked interrupt
   1: Unmasked interrupt */

/* Bit 4 - IN token received when TxFIFO empty mask */
#define OTG_FS_ITTXFEMSK (1 << 4)
/* 0: Masked interrupt
   1: Unmasked interrupt */

/* Bit 3 - Timeout condition mask (Non-isochronous endpoints) */
#define OTG_FS_TOM (1 << 3)
/* 0: Masked interrupt
   1: Unmasked interrupt */

/* Bit 2 Reserved */

/* Bit 1 - Endpoint disabled interrupt mask */
#define OTG_FS_EPDM (1 << 1)
/* 0: Masked interrupt
   1: Unmasked interrupt */

/* Bit 0 - Transfer completed interrupt mask */
#define OTG_FS_XFRCM (1 << 0)
/* 0: Masked interrupt
   1: Unmasked interrupt */


/* -------------------------------------------------------------------------
 * OTG_FS device OUT endpoint common interrupt mask register */
#define STM32F_OTG_FS_DOEPMSK 0x814

/* [31..5] Reserved */

/* Bit 4 - OUT token received when endpoint disabled mask */
#define OTG_FS_OTEPDM (1 << 4)
/* Applies to control OUT endpoints only.
   0: Masked interrupt
   1: Unmasked interrupt */

/* Bit 3 - SETUP phase done mask */
#define OTG_FS_STUPM (1 << 3)
/* Applies to control endpoints only.
   0: Masked interrupt
   1: Unmasked interrupt */

/* Bit 2 Reserved */

/* Bit 1 - Endpoint disabled interrupt mask */
#define OTG_FS_EPDM (1 << 1)
/* 0: Masked interrupt
   1: Unmasked interrupt */

/* Bit 0 - Transfer completed interrupt mask */
#define OTG_FS_XFRCM (1 << 0)
/* 0: Masked interrupt
   1: Unmasked interrupt */


/* -------------------------------------------------------------------------
 * OTG_FS device all endpoints interrupt register */
#define STM32F_OTG_FS_DAINT 0x818

/* Bits [31..16] - OUT endpoint interrupt bits */
#define OTG_FS_OEPINT_MSK ((31 - 16) << 16)
#define OTG_FS_OEPINT0 (1 << 16)
#define OTG_FS_OEPINT1 (1 << 17)
#define OTG_FS_OEPINT2 (1 << 18)
#define OTG_FS_OEPINT3 (1 << 19)
/* One bit per OUT endpoint:
   Bit 16 for OUT endpoint 0, bit 18 for OUT endpoint 3. */

/* Bits [15..0] - IN endpoint interrupt bits */
#define OTG_FS_IEPINT_MSK ((15 - 0) << 0)
#define OTG_FS_IEPINT0 (1 << 0)
#define OTG_FS_IEPINT1 (1 << 1)
#define OTG_FS_IEPINT2 (1 << 2)
#define OTG_FS_IEPINT3 (1 << 3)
/* One bit per IN endpoint:
   Bit 0 for IN endpoint 0, bit 3 for endpoint 3. */


/* -------------------------------------------------------------------------
 * OTG_FS all endpoints interrupt mask register */
#define STM32F_OTG_FS_DAINTMSK 0x81C
/* The OTG_FS_DAINTMSK register works with the Device endpoint 
   interrupt register to interrupt the application when an event occurs 
   on a device endpoint. However, the OTG_FS_DAINT register bit 
   corresponding to that interrupt is still set. */

/* Bits [31..16] - OUT EP interrupt mask bits */
#define OTG_FS_OEPM_MSK ((31 - 16) << 16)
#define OTG_FS_OEPM(N) (1 << (16 + (N)))
#define OTG_FS_OEPM0 (1 << 16)
#define OTG_FS_OEPM1 (1 << 17)
#define OTG_FS_OEPM2 (1 << 18)
#define OTG_FS_OEPM3 (1 << 19)
/* One per OUT endpoint:
   Bit 16 for OUT EP 0, bit 18 for OUT EP 3
   0: Masked interrupt
   1: Unmasked interrupt */

/* Bits [15..0] - IN EP interrupt mask bits */
#define OTG_FS_IEPM_MSK ((15 - 0) << 0)
#define OTG_FS_IEPM(N) (1 << (N))
#define OTG_FS_IEPM0 (1 << 0)
#define OTG_FS_IEPM1 (1 << 1)
#define OTG_FS_IEPM2 (1 << 2)
#define OTG_FS_IEPM3 (1 << 3)
/* One bit per IN endpoint:
   Bit 0 for IN EP 0, bit 3 for IN EP 3
   0: Masked interrupt
   1: Unmasked interrupt */


/* -------------------------------------------------------------------------
 * OTG_FS device VBUS discharge time register */
#define STM32F_OTG_FS_DVBUSDIS 0x0828
/* This register specifies the VBUS discharge time after VBUS 
   pulsing during SRP. */

/* [31..16] Reserved */

/* Bits [15..0] - Device VBUS discharge time */
#define OTG_FS_VBUSDT ((15 - 0) << 0)
/* Specifies the VBUS discharge time after VBUS pulsing during SRP. This 
   value equals: VBUS discharge time in PHY clocks / 1 024
   Depending on your VBUS load, this value may need adjusting. */


/* OTG_FS device VBUS pulsing time register */
#define STM32F_OTG_FS_DVBUSPULSE 0x082C

/* This register specifies the VBUS pulsing time during SRP. */

/* [31..12] Reserved */

/* Bits [11..0] - Device VBUS pulsing time */
#define OTG_FS_DVBUSP ((11 - 0) << 0)
/* Specifies the VBUS pulsing time during SRP. This value equals: 
   VBUS pulsing time in PHY clocks / 1024 */


/* -------------------------------------------------------------------------
 * OTG_FS device IN endpoint FIFO empty interrupt mask register */
#define STM32F_OTG_FS_DIEPEMPMSK 0x834
/* This register is used to control the IN endpoint FIFO empty 
   interrupt generation (TXFE_OTG_FS_DIEPINTx). */

/* [31..16] Reserved */

/* Bits [15..0] - IN EP Tx FIFO empty interrupt mask bits */
#define OTG_FS_INEPTXFEM ((15 - 0) << 0)
/* These bits act as mask bits for OTG_FS_DIEPINTx.
   TXFE interrupt one bit per IN endpoint:
   Bit 0 for IN endpoint 0, bit 3 for IN endpoint 3
   0: Masked interrupt
   1: Unmasked interrupt */


/* -------------------------------------------------------------------------
 * OTG_FS device control IN endpoint 0 control register */
#define STM32F_OTG_FS_DIEPCTL0 0x900

/* Bit 31 - Endpoint enable */
#define OTG_FS_EPENA (1 << 31)
/* The application sets this bit to start transmitting data on the endpoint 0.
   The core clears this bit before setting any of the following interrupts 
   on this endpoint:
   – Endpoint disabled
   – Transfer completed */

/* Bit 30 - Endpoint disable */
#define OTG_FS_EPDIS (1 << 30)
/* The application sets this bit to stop transmitting data on an endpoint, 
   even before the transfer for that endpoint is complete. The application 
   must wait for the Endpoint disabled interrupt before treating the 
   endpoint as disabled. The core clears this bit before setting the 
   Endpoint disabled interrupt. The application must set this bit only if 
   Endpoint enable is already set for this endpoint. */

/* [29..28] Reserved */

/* Bit 27 - Set NAK */
#define OTG_FS_SNAK (1 << 27)
/* A write to this bit sets the NAK bit for the endpoint.
   Using this bit, the application can control the transmission of NAK 
   handshakes on an endpoint.
   The core can also set this bit for an endpoint after a SETUP packet 
   is received on that endpoint. */

/* Bit 26 - Clear NAK */
#define OTG_FS_CNAK (1 << 26)
/* A write to this bit clears the NAK bit for the endpoint. */

/* Bits [25..22] - TxFIFO number */
#define OTG_FS_TXFNUM ((25 - 22) << 22)
#define OTG_FS_TXFNUM_MSK (((1 << (25 - 22 + 1)) - 1) << 22)
#define OTG_FS_TXFNUM_SET(VAL) (((VAL) << 22) & OTG_FS_TXFNUM_MSK)
#define OTG_FS_TXFNUM_GET(REG) (((REG) & OTG_FS_TXFNUM_MSK) >> 22)
/* This value is set to the FIFO number that is assigned to IN endpoint 0. */

/* Bit 21 - STALL handshake */
#define OTG_FS_STALL (1 << 21)
/* The application can only set this bit, and the core clears it when a 
   SETUP token is received for this endpoint. If a NAK bit, a Global IN NAK 
   or Global OUT NAK is set along with this bit, the STALL 
   bit takes priority. */

/* Bit 20 Reserved */

/* Bits [19..18] - Endpoint type */
#define OTG_FS_EPTYP ((19 - 18) << 18)
/* Hardcoded to ‘00’ for control. */

/* Bit 17 - NAK status */
#define OTG_FS_NAKSTS (1 << 17)
/* Indicates the following:
   0: The core is transmitting non-NAK handshakes based on the FIFO status
   1: The core is transmitting NAK handshakes on this endpoint.
   When this bit is set, either by the application or core, the core stops 
   transmitting data, even if there are data available in the TxFIFO. 
   Irrespective of this bit’s setting, the core always responds to SETUP 
   data packets with an ACK handshake. */

/* Bit 16 Reserved */

/* Bit 15 - USB active endpoint */
#define OTG_FS_USBAEP (1 << 15)
/* This bit is always set to 1, indicating that control endpoint 0 is 
   always active in all configurations and interfaces. */

/* [14..2] Reserved */

/* Bits [1..0] - Maximum packet size */
#define OTG_FS_MPSIZ ((10 - 0) << 0)
#define OTG_FS_MPSIZ_64 (0 << 0)
#define OTG_FS_MPSIZ_32 (1 << 0)
#define OTG_FS_MPSIZ_16 (2 << 0)
#define OTG_FS_MPSIZ_8  (3 << 0)
/* The application must program this field with the maximum packet size for 
   the current logical endpoint.
   00: 64 bytes
   01: 32 bytes
   10: 16 bytes
   11: 8 bytes */


/*---------------------------------------------------------------------------
   OTG device endpoint-x control register 
   (OTG_FS_DIEPCTLx) (x = 1..3, where x = Endpoint_number)
   Address offset: 0x900 + (Endpoint_number × 0x20) */
#define OTG_FS_DIEPCTL(N) (0x900 + (N * 0x20))
#define OTG_FS_DIEPCTL1 (0x900 + (1 * 0x20))
#define OTG_FS_DIEPCTL2 (0x900 + (2 * 0x20))
#define OTG_FS_DIEPCTL3 (0x900 + (3 * 0x20))

/* Bit 31 - Endpoint enable */
#define OTG_FS_EPENA (1 << 31)
/* The application sets this bit to start transmitting data on an 
   endpoint. The core clears this bit before setting any of the 
   following interrupts on this endpoint:
   – SETUP phase done
   – Endpoint disabled
   – Transfer completed */

/* Bit 30 - Endpoint disable */
#define OTG_FS_EPDIS (1 << 30)
/* The application sets this bit to stop transmitting/receiving data on 
   an endpoint, even before the transfer for that endpoint is complete. 
   The application must wait for the Endpoint disabled interrupt before 
   treating the endpoint as disabled. The core clears this bit before 
   setting the Endpoint disabled interrupt. The application must set this 
   bit only if Endpoint enable is already set for this endpoint. */

/* Bit 29 - Set odd frame */
#define OTG_FS_SODDFRM (1 << 29)
/* Applies to isochronous IN and OUT endpoints only.
   Writing to this field sets the Even/Odd frame (EONUM) field to odd frame. */

/* Bit 28 - Set DATA0 PID */
#define OTG_FS_SD0PID (1 << 28)
/* Applies to interrupt/bulk IN endpoints only.
   Writing to this field sets the endpoint data PID (DPID) field in this 
   register to DATA0. */

/* Bit 28 - Set even frame */
#define OTG_FS_SEVNFRM (1 << 28)
/* Applies to isochronous IN endpoints only.
   Writing to this field sets the Even/Odd frame (EONUM) field to even frame. */

/* Bit 27 - Set NAK */
#define OTG_FS_SNAK (1 << 27)
/* A write to this bit sets the NAK bit for the endpoint.
   Using this bit, the application can control the transmission of 
   NAK handshakes on an endpoint.
   The core can also set this bit for OUT endpoints on a Transfer 
   completed interrupt, or after a SETUP is received on the endpoint. */

/* Bit 26 - Clear NAK */
#define OTG_FS_CNAK (1 << 26)
/* A write to this bit clears the NAK bit for the endpoint. */

/* Bits [25..22] - TxFIFO number */
#define OTG_FS_TXFNUM ((25 - 22) << 22)
/* These bits specify the FIFO number associated with this endpoint. 
   Each active IN endpoint must be programmed to a separate FIFO number.
   This field is valid only for IN endpoints. */

/* Bit 21 - STALL handshake */
#define OTG_FS_STALL (1 << 21)
/* Applies to non-control, non-isochronous IN endpoints only (access type 
   is rw). The application sets this bit to stall all tokens from the USB 
   host to this endpoint. If a NAK bit, Global IN NAK, or Global OUT NAK 
   is set along with this bit, the STALL bit takes priority.
   Only the application can clear this bit, never the core.
   Applies to control endpoints only (access type is rs).
   The application can only set this bit, and the core clears it, when a 
   SETUP token is received for this endpoint. If a NAK bit, Global IN NAK, 
   or Global OUT NAK is set along with this bit, the STALL bit takes 
   priority. Irrespective of this bit’s setting, the core always responds to
   SETUP data packets with an ACK handshake. */

/* Bit 20 Reserved */

/* Bits [19..18] - Endpoint type */
#define OTG_FS_EPTYP ((19 - 18) << 18)
#define OTG_FS_EPTYP_MSK (((1 << (19 - 18 + 1)) - 1) << 18)
#define OTG_FS_EPTYP_SET(VAL) (((VAL) << 18) & OTG_FS_EPTYP_MSK)
#define OTG_FS_EPTYP_GET(REG) (((REG) & OTG_FS_EPTYP_MSK) >> 18)
/* This is the transfer type supported by this logical endpoint.
 * 00: Control
 * 01: Isochronous
 * 10: Bulk
 * 11: Interrupt */
#define OTG_FS_EPTYP_CTRL (0)
#define OTG_FS_EPTYP_ISOC (1)
#define OTG_FS_EPTYP_BULK (2)
#define OTG_FS_EPTYP_INT (3)

/* Bit 17 - NAK status */
#define OTG_FS_NAKSTS (1 << 17)
/* It indicates the following:
   0: The core is transmitting non-NAK handshakes based on the FIFO status.
   1: The core is transmitting NAK handshakes on this endpoint.
   When either the application or the core sets this bit:
   For non-isochronous IN endpoints: The core stops transmitting any data 
   on an IN endpoint, even if there are data available in the TxFIFO.
   For isochronous IN endpoints: The core sends out a zero-length data 
   packet, even if there are data available in the TxFIFO.
   Irrespective of this bit’s setting, the core always responds to SETUP 
   data packets with an ACK handshake. */

/* Bit 16 - Even/odd frame */
#define OTG_FS_EONUM (1 << 16)
/* Applies to isochronous IN endpoints only.
   Indicates the frame number in which the core transmits/receives 
   isochronous data for this endpoint. The application must program the 
   even/odd frame number in which it intends to transmit/receive isochronous 
   data for this endpoint using the SEVNFRM and SODDFRM fields in 
   this register.
   0: Even frame
   1: Odd frame */

/* Bit 16 - Endpoint data PID */
#define OTG_FS_DPID (1 << 16)
#define OTG_FS_DPID0 (0 << 16)
#define OTG_FS_DPID1 (1 << 16)
/* Applies to interrupt/bulk IN endpoints only.
   Contains the PID of the packet to be received or transmitted on this 
   endpoint. The application must program the PID of the first packet to 
   be received or transmitted on this endpoint, after the endpoint is 
   activated. The application uses the SD0PID register field to program 
   either DATA0 or DATA1 PID.
   0: DATA0
   1: DATA1 */

/* Bit 15 - USB active endpoint */
#define OTG_FS_USBAEP (1 << 15)
/* Indicates whether this endpoint is active in the current configuration 
   and interface. The core clears this bit for all endpoints (other than EP 
   0) after detecting a USB reset. After receiving the SetConfiguration and 
   SetInterface commands, the application must program endpoint registers 
   accordingly and set this bit. */

/* [14..11] Reserved */

/* Bits [10..0] - Maximum packet size */
#define OTG_FS_MPSIZ ((10 - 0) << 0)
#define OTG_FS_MPSIZ_MSK (((1 << (10 - 0 + 1)) - 1) << 0)
#define OTG_FS_MPSIZ_SET(VAL) (((VAL) << 0) & OTG_FS_MPSIZ_MSK)
#define OTG_FS_MPSIZ_GET(REG) (((REG) & OTG_FS_MPSIZ_MSK) >> 0)
/* The application must program this field with the maximum packet size 
   for the current logical endpoint. This value is in bytes. */


/* --------------------------------------------------------------------------
 * OTG_FS device control OUT endpoint 0 control register */
#define STM32F_OTG_FS_DOEPCTL0 0xb00 

/* Bit 31 - Endpoint enable */
#define OTG_FS_EPENA (1 << 31)
/* The application sets this bit to start transmitting data on endpoint 0.
   The core clears this bit before setting any of the following 
   interrupts on this endpoint:
   – SETUP phase done
   – Endpoint disabled
   – Transfer completed */

/* Bit 30 - Endpoint disable */
#define OTG_FS_EPDIS (1 << 30)
/* The application cannot disable control OUT endpoint 0. */

/* [29..28] Reserved */


/* Bit 27 - Set NAK */
#define OTG_FS_SNAK (1 << 27)
/* A write to this bit sets the NAK bit for the endpoint.
   Using this bit, the application can control the transmission of NAK 
   handshakes on an endpoint.
   The core can also set this bit on a Transfer completed interrupt, 
   or after a SETUP is received on the endpoint. */

/* Bit 26 - Clear NAK */
#define OTG_FS_CNAK (1 << 26)
/* A write to this bit clears the NAK bit for the endpoint. */

/* [25..22] Reserved */


/* Bit 21 - STALL handshake */
#define OTG_FS_STALL (1 << 21)
/* The application can only set this bit, and the core clears it, when 
   a SETUP token is received for this endpoint. If a NAK bit or Global 
   OUT NAK is set along with this bit, the STALL bit takes priority. 
   Irrespective of this bit’s setting, the core always responds to 
   SETUP data packets with an ACK handshake. */

/* Bit 20 - Snoop mode */
#define OTG_FS_SNPM (1 << 20)
/* This bit configures the endpoint to Snoop mode. In Snoop mode, the 
   core does not check the correctness of OUT packets before transferring 
   them to application memory. */

/* Bits [19..18] - Endpoint type */
#define OTG_FS_EPTYP ((19 - 18) << 18)
/* Hardcoded to 2’b00 for control. */

/* Bit 17 - NAK status */
#define OTG_FS_NAKSTS (1 << 17)
/* Indicates the following:
   0: The core is transmitting non-NAK handshakes based on the FIFO status.
   1: The core is transmitting NAK handshakes on this endpoint.
   When either the application or the core sets this bit, the core stops 
   receiving data, even if there is space in the RxFIFO to accommodate the 
   incoming packet. Irrespective of this bit’s setting, the core always 
   responds to SETUP data packets with an ACK handshake. */

/* Bit 16 Reserved */


/* Bit 15 - USB active endpoint */
#define OTG_FS_USBAEP (1 << 15)
/* This bit is always set to 1, indicating that a control endpoint 0 is 
   always active in all configurations and interfaces. */

/* [14..2] Reserved */

/* Bits [1..0] - Maximum packet size */
#define OTG_FS_MPSIZ ((10 - 0) << 0)
/* The maximum packet size for control OUT endpoint 0 is the same as 
   what is programmed in control IN endpoint 0.
   00: 64 bytes
   01: 32 bytes
   10: 16 bytes
   11: 8 bytes */


/* --------------------------------------------------------------------------
 * OTG_FS device endpoint-x control register (OTG_FS_DOEPCTLx) 
 * (x = 1..3, where x = Endpoint_number)
 * Address offset for OUT endpoints: 0xB00 + (Endpoint_number × 0x20) */
#define STM32F_OTG_FS_DOEPCTL1 (0xb00 + (1 × 0x20))
#define STM32F_OTG_FS_DOEPCTL2 (0xb00 + (1 × 0x20))
#define STM32F_OTG_FS_DOEPCTL3 (0xb00 + (1 × 0x20))

/* Bit 31 - Endpoint enable */
#define OTG_FS_EPENA (1 << 31)
/* Applies to IN and OUT endpoints.
   The application sets this bit to start transmitting data on an endpoint.
   The core clears this bit before setting any of the following interrupts 
   on this endpoint:
   – SETUP phase done
   – Endpoint disabled
   – Transfer completed */

/* Bit 30 - Endpoint disable */
#define OTG_FS_EPDIS (1 << 30)
/* The application sets this bit to stop transmitting/receiving data on an 
   endpoint, even before the transfer for that endpoint is complete. The 
   application must wait for the Endpoint disabled interrupt before 
   treating the endpoint as disabled. The core clears this bit before 
   setting the Endpoint disabled interrupt. The application must set this 
   bit only if Endpoint enable is already set for this endpoint. */

/* Bit 29 - Set DATA1 PID */
#define OTG_FS_SD1PID (1 << 29)
/* Applies to interrupt/bulk IN and OUT endpoints only. Writing to this 
   field sets the endpoint data PID (DPID) field in this register to DATA1. */

/* Bit 29 - Set Set odd frame */
#define OTG_FS_SODDFRM (1 << 29)
/* Applies to isochronous IN and OUT endpoints only. Writing to this field 
   sets the Even/Odd frame (EONUM) field to odd frame. */

/* Bit 28 - Set DATA0 PID */
#define OTG_FS_SD0PID (1 << 28)
/* Applies to interrupt/bulk OUT endpoints only.
   Writing to this field sets the endpoint data PID (DPID) field in this 
   register to DATA0. */

/* Bit 28 - Set even frame */
#define OTG_FS_SEVNFRM (1 << 28)
/* Applies to isochronous OUT endpoints only. Writing to this field sets 
   the Even/Odd frame (EONUM) field to even frame. */

/* Bit 27 - Set NAK */
#define OTG_FS_SNAK (1 << 27)
/* A write to this bit sets the NAK bit for the endpoint.
   Using this bit, the application can control the transmission of NAK 
   handshakes on an endpoint. The core can also set this bit for OUT 
   endpoints on a Transfer Completed interrupt, or after a SETUP is 
   received on the endpoint. */

/* Bit 26 - Clear NAK */
#define OTG_FS_CNAK (1 << 26)
/* A write to this bit clears the NAK bit for the endpoint. */

/* [25..22] Reserved */

/* Bit 21 - STALL handshake */
#define OTG_FS_STALL (1 << 21)
/* Applies to non-control, non-isochronous OUT endpoints only (access type 
   is rw).
   The application sets this bit to stall all tokens from the USB host to 
   this endpoint. If a NAK bit, Global IN NAK, or Global OUT NAK is set 
   along with this bit, the STALL bit takes priority. Only the application 
   can clear this bit, never the core.
   Applies to control endpoints only (access type is rs).
   The application can only set this bit, and the core clears it, when a 
   SETUP token is received for this endpoint. If a NAK bit, Global IN NAK, 
   or Global OUT NAK is set along with this bit, the STALL bit takes 
   priority. Irrespective of this bit’s setting, the core always responds 
   to SETUP data packets with an ACK handshake. */

/* Bit 20 - Snoop mode */
#define OTG_FS_SNPM (1 << 20)
/* This bit configures the endpoint to Snoop mode. In Snoop mode, the core 
   does not check the correctness of OUT packets before transferring them 
   to application memory. */

/* Bits [19..18] - Endpoint type */
#define OTG_FS_EPTYP ((19 - 18) << 18)
#define OTG_FS_EPTYP_MSK (((1 << (19 - 18 + 1)) - 1) << 18)
/* This is the transfer type supported by this logical endpoint.
 * 00: Control
 * 01: Isochronous
 * 10: Bulk
 * 11: Interrupt */
#define OTG_FS_EPTYP_CTRL (0)
#define OTG_FS_EPTYP_ISOC (1)
#define OTG_FS_EPTYP_BULK (2)
#define OTG_FS_EPTYP_INT (3)

/* Bit 17 - NAK status */
#define OTG_FS_NAKSTS (1 << 17)
/* Indicates the following:
 * 0: The core is transmitting non-NAK handshakes based on the FIFO status.
 * 1: The core is transmitting NAK handshakes on this endpoint.
 * When either the application or the core sets this bit:
 * The core stops receiving any data on an OUT endpoint, even if there is 
 * space in the RxFIFO to accommodate the incoming packet.
 * Irrespective of this bit’s setting, the core always responds to SETUP 
 * data packets with an ACK handshake. */

/* Bit 16 - Even/odd frame */
#define OTG_FS_EONUM (1 << 16)
/* Applies to isochronous IN and OUT endpoints only.
   Indicates the frame number in which the core transmits/receives 
   isochronous data for this endpoint. The application must program the 
   even/odd frame number in which it intends to transmit/receive 
   isochronous data for this endpoint using the SEVNFRM and SODDFRM fields 
   in this register.
   0: Even frame
   1: Odd frame */

/* Bit 16 - Endpoint data PID */
#define OTG_FS_DPID (1 << 16)
#define OTG_FS_DPID0 (0 << 16)
#define OTG_FS_DPID1 (1 << 16)
/* Applies to interrupt/bulk OUT endpoints only.
   Contains the PID of the packet to be received or transmitted on this 
   endpoint. The application must program the PID of the first packet to 
   be received or transmitted on this endpoint, after the endpoint is 
   activated. The application uses the SD0PID register field to program 
   either DATA0 or DATA1 PID.
   0: DATA0
   1: DATA1 */

/* Bit 15 - USB active endpoint */
#define OTG_FS_USBAEP (1 << 15)
/* Indicates whether this endpoint is active in the current configuration 
   and interface. The core clears this bit for all endpoints (other than 
   EP 0) after detecting a USB reset. After receiving the SetConfiguration 
   and SetInterface commands, the application must program endpoint
   registers accordingly and set this bit. */

/* [14..11] Reserved */

/* Bits [10..0] - Maximum packet size */
#define OTG_FS_MPSIZ ((10 - 0) << 0)
/* The application must program this field with the maximum packet 
   size for the current logical endpoint. This value is in bytes. */


/* -------------------------------------------------------------------------
 * OTG_FS device endpoint-x interrupt register (OTG_FS_DIEPINTx) 
 * (x = 0..3, where x = Endpoint_number)
 * Address offset: 0x908 + (Endpoint_number × 0x20) */
#define STM32F_OTG_FS_DIEPINT0 (0x908 + (0 * 0x20))
#define STM32F_OTG_FS_DIEPINT1 (0x908 + (1 * 0x20))
#define STM32F_OTG_FS_DIEPINT2 (0x908 + (2 * 0x20))
#define STM32F_OTG_FS_DIEPINT3 (0x908 + (3 * 0x20))

/* [31..8] Reserved */

/* Bit 7 - Transmit FIFO empty */
#define OTG_FS_TXFE (1 << 7)
/* This interrupt is asserted when the TxFIFO for this endpoint is 
   either half or completely empty. The half or completely empty 
   status is determined by the TxFIFO Empty Level bit in 
   the OTG_FS_GAHBCFG register (TXFELVL bit in OTG_FS_GAHBCFG). */

/* Bit 6 - IN endpoint NAK effective */
#define OTG_FS_INEPNE (1 << 6)
/* This bit can be cleared when the application clears the IN endpoint 
   NAK by writing to the CNAK bit in OTG_FS_DIEPCTLx.
   This interrupt indicates that the core has sampled the NAK bit set 
   (either by the application or by the core). The interrupt indicates 
   that the IN endpoint NAK bit set by the application has taken effect 
   in the core.
   This interrupt does not guarantee that a NAK handshake is sent on the 
   USB. A STALL bit takes priority over a NAK bit. */

/* Bit 5 Reserved */

/* Bit 4 - IN token received when TxFIFO is empty */
#define OTG_FS_ITTXFE (1 << 4)
/* Applies to non-periodic IN endpoints only.
   Indicates that an IN token was received when the associated 
   TxFIFO (periodic/non-periodic) was empty. This interrupt is asserted 
   on the endpoint for which the IN token was received. */

/* Bit 3 - Timeout condition */
#define OTG_FS_TOC (1 << 3)
/* Applies only to Control IN endpoints.
   Indicates that the core has detected a timeout condition on 
   the USB for the last IN token on this endpoint. */

/* Bit 2 Reserved. */

/* Bit 1 - Endpoint disabled interrupt */
#define OTG_FS_EPDISD (1 << 1)
/* This bit indicates that the endpoint is disabled per the 
   application’s request. */

/* Bit 0 - Transfer completed interrupt */
#define OTG_FS_XFRC (1 << 0)
/* This field indicates that the programmed transfer is complete on the 
   AHB as well as on the USB, for this endpoint. */


/* -------------------------------------------------------------------------
 * OTG_FS device endpoint-x interrupt register (OTG_FS_DOEPINTx) 
 * (x = 0..3, where x = Endpoint_number)
 * Address offset: 0xB08 + (Endpoint_number × 0x20) */
#define STM32F_OTG_FS_DOEPINT0 (0xb08 + (0 * 0x20))
#define STM32F_OTG_FS_DOEPINT1 (0xb08 + (1 * 0x20))
#define STM32F_OTG_FS_DOEPINT2 (0xb08 + (2 * 0x20))
#define STM32F_OTG_FS_DOEPINT3 (0xb08 + (3 * 0x20))

/* [31..7] Reserved */

/* Bit 6 - Back-to-back SETUP packets received */
#define OTG_FS_B2BSTUP (1 << 6)
/* Applies to control OUT endpoint only.
   This bit indicates that the core has received more than three 
   back-to-back SETUP packets for this particular endpoint. */

/* Bit 5 Reserved */

/* Bit 4 - OUT token received when endpoint disabled */
#define OTG_FS_OTEPDIS (1 << 4)
/* Applies only to control OUT endpoints.
   Indicates that an OUT token was received when the endpoint was not 
   yet enabled. This interrupt is asserted on the endpoint for which the 
   OUT token was received. */

/* Bit 3 - SETUP phase done */
#define OTG_FS_STUP (1 << 3)
/* Applies to control OUT endpoint only.
   Indicates that the SETUP phase for the control endpoint is complete 
   and no more back-to-back SETUP packets were received for the 
   current control transfer. On this interrupt, the application can 
   decode the received SETUP data packet. */

/* Bit 2 Reserved */

/* Bit 1 - Endpoint disabled interrupt */
#define OTG_FS_EPDISD (1 << 1)
/* This bit indicates that the endpoint is disabled per the 
   application’s request. */

/* Bit 0 - Transfer completed interrupt */
#define OTG_FS_XFRC (1 << 0)
/* This field indicates that the programmed transfer is complete on 
   the AHB as well as on the USB, for this endpoint. */


/* -------------------------------------------------------------------------
 * OTG_FS device IN endpoint 0 transfer size register */
#define STM32F_OTG_FS_DIEPTSIZ0 0x910

/* Bits [20..19] - Packet count */
#define OTG_FS_PKTCNT ((28 - 19) << 19)
#define OTG_FS_PKTCNT_MSK (((1 << (28 - 19 + 1)) - 1) << 19)
#define OTG_FS_PKTCNT_SET(VAL) (((VAL) << 19) & OTG_FS_PKTCNT_MSK)
#define OTG_FS_PKTCNT_GET(REG) (((REG) & OTG_FS_PKTCNT_MSK) >> 19)
/* Indicates the total number of USB packets that constitute the Transfer 
   Size amount of data for endpoint 0.
   This field is decremented every time a packet (maximum size or short 
   packet) is read from the TxFIFO. */

/* [18..7] Reserved */

/* Bits [6..0] - Transfer size */
#define OTG_FS_XFRSIZ ((18 - 0) << 0)
#define OTG_FS_XFRSIZ_MSK (((1 << (18 - 0 + 1)) - 1) << 0)
#define OTG_FS_XFRSIZ_SET(VAL) (((VAL) << 0) & OTG_FS_XFRSIZ_MSK)
#define OTG_FS_XFRSIZ_GET(REG) (((REG) & OTG_FS_XFRSIZ_MSK) >> 0)
/* Indicates the transfer size in bytes for endpoint 0. The core 
   interrupts the application only after it has exhausted the transfer size 
   amount of data. The transfer size can be set to the maximum packet 
   size of the endpoint, to be interrupted at the end of each packet.
   The core decrements this field every time a packet from the external 
   memory is written to the TxFIFO. */

/* -------------------------------------------------------------------------
 * OTG_FS device OUT endpoint 0 transfer size register */
#define STM32F_OTG_FS_DOEPTSIZ0 0xb10

/* Bit 31 Reserved */

/* Bits [30..29] - SETUP packet count */
#define OTG_FS_STUPCNT ((30 - 29) << 29)
#define OTG_FS_STUPCNT_1 (1 << 29)
#define OTG_FS_STUPCNT_2 (2 << 29)
#define OTG_FS_STUPCNT_3 (3 << 29)
#define OTG_FS_STUPCNT_MSK (((1 << (30 - 29 + 1)) - 1) << 29)
#define OTG_FS_STUPCNT_SET(VAL) (((VAL) << 29) & OTG_FS_STUPCNT_MSK)
#define OTG_FS_STUPCNT_GET(REG) (((REG) & OTG_FS_STUPCNT_MSK) >> 29)
/* This field specifies the number of back-to-back SETUP data packets the 
   endpoint can receive.
   01: 1 packet
   10: 2 packets
   11: 3 packets */

/* [28..20] Reserved */

/* Bit 19 - Packet count */
#define OTG_FS_PKTCNT ((28 - 19) << 19)
#define OTG_FS_PKTCNT_MSK (((1 << (28 - 19 + 1)) - 1) << 19)
#define OTG_FS_PKTCNT_SET(VAL) (((VAL) << 19) & OTG_FS_PKTCNT_MSK)
#define OTG_FS_PKTCNT_GET(REG) (((REG) & OTG_FS_PKTCNT_MSK) >> 19)
/* This field is decremented to zero after a packet is written into 
   the RxFIFO. */

/* [18..7] Reserved */

/* Bits [6..0] - Transfer size */
#define OTG_FS_XFRSIZ ((18 - 0) << 0)
#define OTG_FS_XFRSIZ_MSK (((1 << (18 - 0 + 1)) - 1) << 0)
#define OTG_FS_XFRSIZ_SET(VAL) (((VAL) << 0) & OTG_FS_XFRSIZ_MSK)
#define OTG_FS_XFRSIZ_GET(REG) (((REG) & OTG_FS_XFRSIZ_MSK) >> 0)
/* Indicates the transfer size in bytes for endpoint 0. The core 
   interrupts the application only after it has exhausted the transfer size 
   amount of data. The transfer size can be set to the maximum packet 
   size of the endpoint, to be interrupted at the end of each packet.
   The core decrements this field every time a packet is read from the 
   RxFIFO and written to the external memory. */


/* -------------------------------------------------------------------------
 * OTG_FS device endpoint-x transfer size register (OTG_FS_DIEPTSIZx)
 * (x = 1..3, where x = Endpoint_number)
 * Address offset: 0x910 + (Endpoint_number × 0x20) */
#define STM32F_OTG_FS_DIEPTSIZ1 (0x910 + (1 * 0x20))
#define STM32F_OTG_FS_DIEPTSIZ2 (0x910 + (2 * 0x20))
#define STM32F_OTG_FS_DIEPTSIZ3 (0x910 + (3 * 0x20))

/* Bit 31 Reserved */


/* Bits [30..29] - Multi count */
#define OTG_FS_MCNT ((30 - 29) << 29)
/* For periodic IN endpoints, this field indicates the number of packets 
   that must be transmitted per frame on the USB. The core uses this 
   field to calculate the data PID for isochronous IN endpoints.
   01: 1 packet
   10: 2 packets
   11: 3 packets */

/* Bits [28..19] - Packet count */
#define OTG_FS_PKTCNT ((28 - 19) << 19)
/* Indicates the total number of USB packets that constitute the Transfer 
   Size amount of data for this endpoint.
   This field is decremented every time a packet (maximum size or short 
   packet) is read from the TxFIFO. */

/* Bits [18..0] - Transfer size */
#define OTG_FS_XFRSIZ ((18 - 0) << 0)
/* This field contains the transfer size in bytes for the current 
   endpoint. The core only interrupts the application after it has 
   exhausted the transfer size amount of data. The transfer size can be
   set to the maximum packet size of the endpoint, to be interrupted at 
   the end of each packet.
   The core decrements this field every time a packet from the external 
   memory is written to the TxFIFO. */


/* -------------------------------------------------------------------------
 * OTG_FS device IN endpoint transmit FIFO status register (OTG_FS_DTXFSTSx) 
 * (x = 0..3, where x = Endpoint_number)
 * Address offset for IN endpoints: 0x918 + (Endpoint_number × 0x20) */
#define OTG_FS_DTXFSTS0 (0x918 + (0 * 0x20))
#define OTG_FS_DTXFSTS1 (0x918 + (1 * 0x20))
#define OTG_FS_DTXFSTS2 (0x918 + (2 * 0x20))
#define OTG_FS_DTXFSTS3 (0x918 + (3 * 0x20))
/* This read-only register contains the free space information for the 
   Device IN endpoint TxFIFO. */

/* 31:16 Reserved */

/* Bits [15..0] - IN endpoint TxFIFO space available */
#define OTG_FS_INEPTFSAV_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_FS_INEPTFSAV_GET(REG) (((REG) & OTG_FS_INEPTFSAV_MSK) >> 0)
/* Indicates the amount of free space available in the Endpoint TxFIFO.
   Values are in terms of 32-bit words:
   0x0: Endpoint TxFIFO is full
   0x1: 1 word available
   0x2: 2 words available
   0xn: n words available 
   Others: Reserved */


/* -------------------------------------------------------------------------
 * OTG_FS device OUT endpoint-x transfer size register (OTG_FS_DOEPTSIZx)
 * (x = 1..3, where x = Endpoint_number)
 * Address offset: 0xB10 + (Endpoint_number × 0x20) */
#define OTG_FS_DOEPTSIZ1 (0xb10 + (1 * 0x20))
#define OTG_FS_DOEPTSIZ2 (0xb10 + (2 * 0x20))
#define OTG_FS_DOEPTSIZ3 (0xb10 + (3 * 0x20))

/* Bit 31 Reserved */

/* Bits [30..29] - Received data PID */
#define OTG_FS_RXDPID ((30 - 29) << 29)
/* Applies to isochronous OUT endpoints only.
   This is the data PID received in the last packet for this endpoint.
   00: DATA0
   01: DATA2
   10: DATA1
   11: MDATA */

/* Bits [30..29] - SETUP packet count */
#define OTG_FS_STUPCNT ((30 - 29) << 29)
/* Applies to control OUT Endpoints only. This field specifies the number 
   of back-to-back SETUP data packets the endpoint can receive.
   01: 1 packet
   10: 2 packets
   11: 3 packets */

/* Bits [28..19] - Packet count */
#define OTG_FS_PKTCNT ((28 - 19) << 19)
/* Indicates the total number of USB packets that constitute the Transfer 
   Size amount of data for this endpoint.
   This field is decremented every time a packet (maximum size or 
   short packet) is written to the RxFIFO. */

/* Bits [18..0] - Transfer size */
#define OTG_FS_XFRSIZ ((18 - 0) << 0)
/* This field contains the transfer size in bytes for the current endpoint. 
   The core only interrupts the application after it has exhausted the 
   transfer size amount of data. The transfer size can be set to the 
   maximum packet size of the endpoint, to be interrupted at the end of 
   each packet. The core decrements this field every time a packet is 
   read from the RxFIFO and written to the external memory. */


/* -------------------------------------------------------------------------
 * OTG_FS power and clock gating control register */
#define STM32F_OTG_FS_PCGCCTL 0xe00

/* [31..5] Reserved */

/* Bit 4 - PHY Suspended */
#define OTG_FS_PHYSUSP (1 << 4)
/* Indicates that the PHY has been suspended. This bit is updated once 
   the PHY is suspended after the application has set the 
   STPPCLK bit (bit 0). */

/* [3..2] Reserved */

/* Bit 1 - Gate HCLK */
#define OTG_FS_GATEHCLK (1 << 1)
/* The application sets this bit to gate HCLK to modules other than the 
   AHB Slave and Master and wakeup logic when the USB is suspended or the 
   session is not valid. The application clears this bit when the USB is 
   resumed or a new session starts. */

/* Bit 0 - Stop PHY clock */
#define OTG_FS_STPPCLK (1 << 0)
/* The application sets this bit to stop the PHY clock when the USB is 
   suspended, the session is not valid, or the device is disconnected. 
   The application clears this bit when the USB is resumed or a new 
   session starts. */


#define OTG_FS_RAM_SIZE (1024 + 256)

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <stdbool.h>

struct stm32f_otg_fs {
	/* 0x000 */
	volatile uint32_t gotgctl;
	volatile uint32_t gotgint;
	volatile uint32_t gahbcfg;
	volatile uint32_t gusbcfg;
	/* 0x010 */
	volatile uint32_t grstctl;
	volatile uint32_t gintsts;
	volatile uint32_t gintmsk;
	volatile uint32_t grxstsr;
	/* 0x020 */
	volatile uint32_t grxstsp;
	volatile uint32_t grxfsiz;
	union {
		volatile uint32_t hnptxfsiz;
		volatile uint32_t dieptxf0;
	};
	volatile uint32_t hnptxsts;
	/* 0x030 */
	uint32_t res1[2];
	volatile uint32_t gccfg;
	volatile uint32_t cid;
	/* 0x040 */
	uint32_t res2[(0x100 - 0x040) / 4];
	/* 0x100 */
	volatile uint32_t hptxfsiz;
	volatile uint32_t dieptxf1;
	volatile uint32_t dieptxf2;
	volatile uint32_t dieptxf3;
	/* 0x110 */
	uint32_t res3[(0x400 - 0x110) / 4];
	/* 0x400 */
	volatile uint32_t hcfg;
	volatile uint32_t hfir;
	volatile uint32_t hfnum;
	uint32_t res4;
	/* 0x410 */
	volatile uint32_t hptxsts;
	volatile uint32_t haint;
	volatile uint32_t haintmsk;
	/* 0x41c */
	uint32_t res5[(0x440 - 0x41c) / 4];
	/* 0x440 */
	volatile uint32_t hptr;
	/* 0x444 */
	uint32_t res6[(0x500 - 0x444) / 4];
	/* 0x500 */
	struct {
		volatile uint32_t hcchar; /* 0x500, 0x520 ... */
		uint32_t res1;    /* 0x504 */
		volatile uint32_t hcint;    /* 0x508 */
		volatile uint32_t hcintmsk; /* 0x50c */
		volatile uint32_t hctsiz;   /* 0x510 */
		uint32_t res2[3]; /* 0x514 */
	} hc[8];
	/* 0x600 */
	uint32_t res7[(0x800 - 0x600) / 4];
	/* 0x800 */
	volatile uint32_t dcfg;
	volatile uint32_t dctl;
	volatile uint32_t dsts;
	uint32_t res8;
	/* 0x810 */
	volatile uint32_t diepmsk;
	volatile uint32_t doepmsk;
	volatile uint32_t daint;
	volatile uint32_t daintmsk;
	/* 0x820 */
	uint32_t res9[2];
	/* 0x828 */
	volatile uint32_t dvbusdis;
	volatile uint32_t dvbuspulse;
	/* 0x830 */
	uint32_t res10;
	volatile uint32_t diepempmsk;
	/* 0x838 */
	uint32_t res11[(0x900 - 0x838) / 4];
	/* 0x900 */
	struct {
		volatile uint32_t diepctl; /* 0x900, 0x920, 0x940, 0x960 */
		uint32_t res1;
		volatile uint32_t diepint; /* 0x908 */
		uint32_t res2;
		volatile uint32_t dieptsiz; /* 0x910, 0x930, 0x950, 0x970*/
		uint32_t res3;
		volatile uint32_t dtxfsts; /* 0x918, 0x938, 0x958, 0x978*/
		uint32_t res4;
	} inep[4];
	/* 0x980 */
	uint32_t res12[(0xb00 - 0x980) / 4];
	struct {
		volatile uint32_t doepctl; /* 0xb00, 0xb20, 0xb40, 0xb60 */
		uint32_t res1;
		volatile uint32_t doepint; /* 0xb08, 0xb28, 0xb48, 0xb68 */
		uint32_t res2;
		volatile uint32_t doeptsiz; /* 0xb10, 0xb30, 0xb50, 0xb70*/
		uint32_t res3[3];
	} outep[4];
	/* 0xb80 */
	uint32_t res13[(0xe00 - 0xb80) / 4];
	/* 0xe00 */
	volatile uint32_t pcgcctl;
	/* 0xe04 */
	uint32_t res14[(0x1000 - 0xe04) / 4];
	/* 0x1000 */
	struct {
		union {
			volatile uint32_t pop;
			volatile uint32_t push;
		};
		uint32_t res1[(0x1000 - 4) / 4];
	} dfifo[8];
	uint32_t res15[(0x20000 - (0x1000 + (8 * 0x1000))) / 4];
	/* 0x20000 */
	uint32_t ram[32768];
};

extern const uint8_t stm32f_otg_fs_ep0_mpsiz_lut[];

#define OTGFS_EP0_MPSIZ_GET(DEPCTL) \
	stm32f_otg_fs_ep0_mpsiz_lut[OTG_FS_MPSIZ_GET(DEPCTL)]

#define OTG_FS_EP0_MPSIZ_SET(MPSIZ) ((MPSIZ == 64) ? OTG_FS_MPSIZ_64 : \
		((MPSIZ == 32) ? OTG_FS_MPSIZ_32 : \
		((MPSIZ == 16) ? OTG_FS_MPSIZ_16 : OTG_FS_MPSIZ_8)))

#ifdef __cplusplus
extern "C" {
#endif

void stm32f_otg_fs_core_reset(struct stm32f_otg_fs * otg_fs);

void stm32f_otg_fs_device_init(struct stm32f_otg_fs * otg_fs);

void stm32f_otg_fs_txfifo_flush(struct stm32f_otg_fs * otg_fs,
								unsigned int num);

void stm32f_otg_fs_rxfifo_flush(struct stm32f_otg_fs * otg_fs);

void stm32f_otg_fs_addr_set(struct stm32f_otg_fs * otg_fs, unsigned int addr);

void stm32f_otg_fs_ep_disable(struct stm32f_otg_fs * otg_fs, unsigned int addr);

void stm32f_otg_fs_ep_dump(struct stm32f_otg_fs * otg_fs, unsigned int addr);

void stm32f_otg_fs_ep_enable(struct stm32f_otg_fs * otg_fs, unsigned int addr,
							 unsigned int type, unsigned int mpsiz);

/* prepare TX fifo to send */
int stm32f_otg_fs_txf_setup(struct stm32f_otg_fs * otg_fs, unsigned int ep,
							 unsigned int len);

/* push data inot TX fifo */
int stm32f_otg_fs_txf_push(struct stm32f_otg_fs * otg_fs, unsigned int ep,
						   void * buf);

void stm32f_otg_fs_enum_done(struct stm32f_otg_fs * otg_fs);

#ifdef __cplusplus
}
#endif


#endif /* __ASSEMBLER__ */

#endif /* __STM32F_OTG_FS_H__ */

