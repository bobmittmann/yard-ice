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
 * @file stm32f-otg_hs.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_OTG_HS_H__
#define __STM32F_OTG_HS_H__

/* ------------------------------------------------------------------------- */
/* OTG_HS global registers
These registers are available in both host and peripheral modes, and do not need to be reprogrammed when switching between these modes.
Bit values in the register descriptions are expressed in binary unless otherwise specified. */

/* ------------------------------------------------------------------------- */
/* OTG_HS control and status register */
#define STM32F_OTG_HS_GOTGCTL 0x000

/* The OTG control and status register controls the behavior and reflects the status of the OTG function of the core. 
   */

/* [31..20] Reserved, must be kept at reset value. */


/* Bit 19 - B-session valid */
#define OTG_HS_BSVLD (1 << 19)
/* Indicates the peripheral mode transceiver status.
0: B-session is not valid.
1: B-session is valid.
In OTG mode, you can use this bit to determine if the device is connected or disconnected.
Note: Only accessible in peripheral mode. */

/* Bit 18 - A-session valid */
#define OTG_HS_ASVLD (1 << 18)
/* Indicates the host mode transceiver status.
0: A-session is not valid
1: A-session is valid
Note: Only accessible in host mode. */

/* Bit 17 - Long/short debounce time */
#define OTG_HS_DBCT (1 << 17)
/* Indicates the debounce time of a detected connection.
0: Long debounce time, used for physical connections (100 ms + 2.5 μs)
1: Short debounce time, used for soft connections (2.5 μs)
Note: Only accessible in host mode. */

/* Bit 16 - Connector ID status */
#define OTG_HS_CIDSTS (1 << 16)
/* Indicates the connector ID status on a connect event.
0: The OTG_HS controller is in A-device mode
1: The OTG_HS controller is in B-device mode
Note: Accessible in both peripheral and host modes. */

/* [15..12] Reserved, must be kept at reset value. */


/* Bit 11 - Device HNP enabled */
#define OTG_HS_DHNPEN (1 << 11)
/* The application sets this bit when it successfully receives a SetFeature.SetHNPEnable command from the connected USB host.
0: HNP is not enabled in the application
1: HNP is enabled in the application
Note: Only accessible in peripheral mode. */

/* Bit 10 - Host set HNP enable */
#define OTG_HS_HSHNPEN (1 << 10)
/* The application sets this bit when it has successfully enabled HNP (using the SetFeature.SetHNPEnable command) on the connected device.
0: Host Set HNP is not enabled
1: Host Set HNP is enabled
Note: Only accessible in host mode. */

/* Bit 9 - HNP request */
#define OTG_HS_HNPRQ (1 << 9)
/* The application sets this bit to initiate an HNP request to the connected USB host. The application can clear this bit by writing a 0 when the host negotiation success status change bit in the OTG interrupt register (HNSSCHG bit in OTG_HS_GOTGINT) is set. The core clears this bit when the HNSSCHG bit is cleared.
0: No HNP request
1: HNP request
Note: Only accessible in peripheral mode. */

/* Bit 8 - Host negotiation success */
#define OTG_HS_HNGSCS (1 << 8)
/* The core sets this bit when host negotiation is successful. The core clears this bit when the HNP Request (HNPRQ) bit in this register is set.
0: Host negotiation failure
1: Host negotiation success
Note: Only accessible in peripheral mode. */

/* [7..2] Reserved, must be kept at reset value. */


/* Bit 1 - Session request */
#define OTG_HS_SRQ (1 << 1)
/* The application sets this bit to initiate a session request on the USB. The application can clear this bit by writing a 0 when the host negotiation success status change bit in the OTG Interrupt register (HNSSCHG bit in OTG_HS_GOTGINT) is set. The core clears this bit when the HNSSCHG bit is cleared.
If you use the USB 1.1 full-speed serial transceiver interface to initiate the session request, the application must wait until V BUS discharges to 0.2 V, after the B-Session Valid bit in this register (BSVLD bit in OTG_HS_GOTGCTL) is cleared. This discharge time varies between different PHYs and can be obtained from the PHY vendor.
0: No session request
1: Session request
Note: Only accessible in peripheral mode. */

/* Bit 0 - Session request success */
#define OTG_HS_SRQSCS (1 << 0)
/* The core sets this bit when a session request initiation is successful.
0: Session request failure
1: Session request success
Note: Only accessible in peripheral mode. */


/* OTG_HS interrupt register */
#define STM32F_OTG_HS_GOTGINT 0x04

/* The application reads this register whenever there is an OTG interrupt and clears the bits in this register to clear the OTG interrupt. */

/* [31..20] Reserved, must be kept at reset value. */


/* Bit 19 - Debounce done */
#define OTG_HS_DBCDNE (1 << 19)
/* The core sets this bit when the debounce is completed after the device connect. The application can start driving USB reset after seeing this interrupt. This bit is only valid when the HNP Capable or SRP Capable bit is set in the Core USB Configuration register (HNPCAP
bit or SRPCAP bit in OTG_HS_GUSBCFG, respectively).
Note: Only accessible in host mode. */

/* Bit 18 - A-device timeout change */
#define OTG_HS_ADTOCHG (1 << 18)
/* The core sets this bit to indicate that the A-device has timed out while waiting for the B-device to connect.
Note: Accessible in both peripheral and host modes. */

/* Bit 17 - Host negotiation detected */
#define OTG_HS_HNGDET (1 << 17)
/* The core sets this bit when it detects a host negotiation request on the USB.
Note: Accessible in both peripheral and host modes. */

/* [16..10] Reserved, must be kept at reset value. */


/* Bit 9 - Host negotiation success status change */
#define OTG_HS_HNSSCHG (1 << 9)
/* The core sets this bit on the success or failure of a USB host negotiation request. The application must read the host negotiation success bit of the OTG Control and Status register (HNGSCS in OTG_HS_GOTGCTL) to check for success or failure.
Note: Accessible in both peripheral and host modes. */

/* [7..3] Reserved, must be kept at reset value. */


/* Bit 8 - Session request success status change */
#define OTG_HS_SRSSCHG (1 << 8)
/* The core sets this bit on the success or failure of a session request. The application must read the session request success bit in the OTG Control and status register (SRQSCS bit in OTG_HS_GOTGCTL) to check for success or failure.
Note: Accessible in both peripheral and host modes. */

/* Bit 2 - Session end detected */
#define OTG_HS_SEDET (1 << 2)
/* The core sets this bit to indicate that the level of the voltage on V BUS is no longer valid for a B-device session when V BUS < 0.8 V. */

/* [1..0] Reserved, must be kept at reset value. */



/* OTG_HS AHB configuration register */
#define STM32F_OTG_HS_GAHBCFG 0x008

/* This register can be used to configure the core after power-on or a change in mode. This register mainly contains AHB system-related configuration parameters. Do not change this register after the initial programming. The application must program this register before starting any transactions on either the AHB or the USB. */

/* [31..20] Reserved, must be kept at reset value. */


/* Bit 8 - Periodic TxFIFO empty level */
#define OTG_HS_PTXFELVL (1 << 8)
/* Indicates when the periodic TxFIFO empty interrupt bit in the Core interrupt register (PTXFE bit in OTG_HS_GINTSTS) is triggered.
0: PTXFE (in OTG_HS_GINTSTS) interrupt indicates that the Periodic TxFIFO is half empty
1: PTXFE (in OTG_HS_GINTSTS) interrupt indicates that the Periodic TxFIFO is completely empty
Note: Only accessible in host mode. */

/* Bit 7 - TxFIFO empty level */
#define OTG_HS_TXFELVL (1 << 7)
/* In peripheral mode, this bit indicates when the IN endpoint Transmit FIFO empty interrupt (TXFE in OTG_HS_DIEPINTx.) is triggered.
0: TXFE (in OTG_HS_DIEPINTx) interrupt indicates that the IN Endpoint TxFIFO is half empty
1: TXFE (in OTG_HS_DIEPINTx) interrupt indicates that the IN Endpoint TxFIFO is completely empty
Note: Only accessible in peripheral mode. */

/* Bit 6 Reserved, must be kept at reset value. */

/* Bit 5 - DMA enable */
#define OTG_HS_DMAEN (1 << 5)
/* 0: The core operates in slave mode
1: The core operates in DMA mode */

/* Bits [4..1] - Burst length/type */
#define OTG_HS_HBSTLEN_MSK (((1 << (4 - 1 + 1)) - 1) << 1)
#define OTG_HS_HBSTLEN_SET(VAL) (((VAL) << 1) & OTG_HS_HBSTLEN_MSK)
#define OTG_HS_HBSTLEN_GET(REG) (((REG) & OTG_HS_HBSTLEN_MSK) >> 1)
/* 0000 Single
0001 INCR
0011 INCR4
0101 INCR8
0111 INCR16
Others: Reserved */

/* Bit 0 - Global interrupt mask */
#define OTG_HS_GINTMSK (1 << 0)
/* This bit is used to mask or unmask the interrupt line assertion to the application. Irrespective
of this bit setting, the interrupt status registers are updated by the core.
0: Mask the interrupt assertion to the application.
1: Unmask the interrupt assertion to the application
Note: Accessible in both peripheral and host modes. */


/* OTG_HS USB configuration register */
#define STM32F_OTG_HS_GUSBCFG 0x00C

/* This register can be used to configure the core after power-on or a changing to host mode or peripheral mode. It contains USB and USB-PHY related configuration parameters. The
application must program this register before starting any transactions on either the AHB or the USB. Do not make changes to this register after the initial programming. */

/* Bit 31 - Corrupt Tx packet */
#define OTG_HS_CTXPKT (1 << 31)
/* This bit is for debug purposes only. Never set this bit to 1.
Note: Accessible in both peripheral and host modes. */

/* Bit 30 - Forced peripheral mode */
#define OTG_HS_FDMOD (1 << 30)
/* Writing a 1 to this bit forces the core to peripheral mode irrespective of the OTG_HS_ID input pin.
0: Normal mode
1: Forced peripheral mode
After setting the force bit, the application must wait at least 25 ms before the change takes effect.
Note: Accessible in both peripheral and host modes. */

/* Bit 29 - Forced host mode */
#define OTG_HS_FHMOD (1 << 29)
/* Writing a 1 to this bit forces the core to host mode irrespective of the OTG_HS_ID input pin.
0: Normal mode
1: Forced host mode
After setting the force bit, the application must wait at least 25 ms before the change takes effect.
Note: Accessible in both peripheral and host modes. */

/* [28..26] Reserved, must be kept at reset value. */


/* Bit 25 - ULPI interface protect disable */
#define OTG_HS_ULPIIPD (1 << 25)
/* This bit controls the circuitry built in the PHY to protect the ULPI interface when the link tri-states stp and data. Any pull-up or pull-down resistors employed by this feature can be disabled. Please refer to the ULPI specification for more details.
0: Enables the interface protection circuit
1: Disables the interface protection circuit */

/* Bit 24 - Indicator pass through */
#define OTG_HS_PTCI (1 << 24)
/* This bit controls whether the complement output is qualified with the internal V BUS valid comparator before being used in the V BUS state in the RX CMD. Please refer to the ULPI specification for more details.
0: Complement Output signal is qualified with the Internal V BUS valid comparator
1: Complement Output signal is not qualified with the Internal V BUS valid comparator */

/* Bit 23 - Indicator complement */
#define OTG_HS_PCCI (1 << 23)
/* This bit controls the PHY to invert the ExternalVbusIndicator input signal, and generate the complement output. Please refer to the ULPI specification for more details.
0: PHY does not invert the ExternalVbusIndicator signal
1: PHY inverts ExternalVbusIndicator signal */

/* Bit 22 - TermSel DLine pulsing selection */
#define OTG_HS_TSDPS (1 << 22)
/* This bit selects utmi_termselect to drive the data line pulse during SRP (session request protocol).
0: Data line pulsing using utmi_txvalid (default)
1: Data line pulsing using utmi_termsel */

/* Bit 21 - ULPI external V BUS indicator */
#define OTG_HS_ULPIEVBUSI (1 << 21)
/* This bit indicates to the ULPI PHY to use an external V BUS overcurrent indicator.
0: PHY uses an internal V BUS valid comparator
1: PHY uses an external V BUS valid comparator */

/* Bit 20 - ULPI External V BUS Drive */
#define OTG_HS_ULPIEVBUSD (1 << 20)
/* This bit selects between internal or external supply to drive 5 V on V BUS , in the ULPI PHY.
0: PHY drives V BUS using internal charge pump (default)
1: PHY drives V BUS using external supply. */

/* Bit 19 - ULPI Clock SuspendM */
#define OTG_HS_ULPICSM (1 << 19)
/* This bit sets the ClockSuspendM bit in the interface control register on the ULPI PHY. This bit applies only in the serial and carkit modes.
0: PHY powers down the internal clock during suspend
1: PHY does not power down the internal clock */

/* Bit 18 - ULPI Auto-resume */
#define OTG_HS_ULPIAR (1 << 18)
/* This bit sets the AutoResume bit in the interface control register on the ULPI PHY.
0: PHY does not use AutoResume feature
1: PHY uses AutoResume feature */

/* Bit 17 - ULPI FS/LS select */
#define OTG_HS_ULPIFSLS (1 << 17)
/* The application uses this bit to select the FS/LS serial interface for the ULPI PHY. This bit is valid only when the FS serial transceiver is selected on the ULPI PHY.
0: ULPI interface
1: ULPI FS/LS serial interface
Bit 16
Reserved, must be kept at reset value. */

/* Bit 15 - PHY Low-power clock select */
#define OTG_HS_PHYLPCS (1 << 15)
/* This bit selects either 480 MHz or 48 MHz (low-power) PHY mode. In FS and LS modes, the PHY can usually operate on a 48 MHz clock to save power.
0: 480 MHz internal PLL clock
1: 48 MHz external clock
In 480 MHz mode, the UTMI interface operates at either 60 or 30 MHz, depending on whether the 8- or 16-bit data width is selected. In 48 MHz mode, the UTMI interface operates at 48 MHz in FS and LS modes. */

/* Bit 14 Reserved, must be kept at reset value. */


/* Bits [13..10] - USB turnaround time */
#define OTG_HS_TRDT_MSK (((1 << (13 - 10 + 1)) - 1) << 10)
#define OTG_HS_TRDT_SET(VAL) (((VAL) << 10) & OTG_HS_TRDT_MSK)
#define OTG_HS_TRDT_GET(REG) (((REG) & OTG_HS_TRDT_MSK) >> 10)
/* These bits allows to set the turnaround time in PHY clocks. They must be configured according to Table 208: TRDT values, depending on the application AHB frequency. Higher TRDT values allow stretching the USB response time to IN tokens in order to compensate for longer AHB read access latency to the Data FIFO. */

/* Bit 9 - HNP-capable */
#define OTG_HS_HNPCAP (1 << 9)
/* The application uses this bit to control the OTG_HS controller’s HNP capabilities.
0: HNP capability is not enabled
1: HNP capability is enabled
Note: Accessible in both peripheral and host modes. */

/* Bit 8 - SRP-capable */
#define OTG_HS_SRPCAP (1 << 8)
/* The application uses this bit to control the OTG_HS controller’s SRP capabilities. If the core operates as a nonSRP-capable B-device, it cannot request the connected A-device (host) to activate V BUS and start a session.
0: SRP capability is not enabled
1: SRP capability is enabled
Note: Accessible in both peripheral and host modes. */

/* Bit 7 Reserved, must be kept at reset value. */


/* Bit 6 - USB 2.0 high-speed ULPI PHY or USB 1.1 full-speed serial transceiver select */
#define OTG_HS_PHYSEL (1 << 6)
/* 0: USB 2.0 high-speed ULPI PHY
1: USB 1.1 full-speed serial transceiver */

/* Bits 5:3 Reserved, must be kept at reset value. */

/* Bits [2..0] - FS timeout calibration */
#define OTG_HS_TOCAL_MSK (((1 << (2 - 0 + 1)) - 1) << 0)
#define OTG_HS_TOCAL_SET(VAL) (((VAL) << 0) & OTG_HS_TOCAL_MSK)
#define OTG_HS_TOCAL_GET(REG) (((REG) & OTG_HS_TOCAL_MSK) >> 0)
/* The number of PHY clocks that the application programs in this field is added to the full- speed interpacket timeout duration in the core to account for any additional delays introduced by the PHY. This can be required, because the delay introduced by the PHY in generating the line state condition can vary from one PHY to another.
The USB standard timeout value for full-speed operation is 16 to 18 (inclusive) bit times. The application must program this field based on the speed of enumeration. The number of bit times added per PHY clock is 0.25 bit times. */


/* OTG_HS reset register */
#define STM32F_OTG_HS_GRSTCTL 0x010

/* The application uses this register to reset various hardware features inside the core. */

/* Bit 31 - AHB master idle */
#define OTG_HS_AHBIDL (1 << 31)
/* Indicates that the AHB master state machine is in the Idle condition.
Note: Accessible in both peripheral and host modes. */

/* Bit 30 - DMA request signal */
#define OTG_HS_DMAREQ (1 << 30)
/* This bit indicates that the DMA request is in progress. Used for debug.
Bits 29:11
Reserved, must be kept at reset value. */

/* Bits [10..6] - TxFIFO number */
#define OTG_HS_TXFIFO_MSK (((1 << (10 - 6 + 1 + 1)) - 1) << 6)
#define OTG_HS_TXFIFO_SET(VAL) (((VAL) << 6) & OTG_HS_TXFIFO_MSK)
#define OTG_HS_TXFIFO_GET(REG) (((REG) & OTG_HS_TXFIFO_MSK) >> 6)
#define OTG_HS_TXFIFO_0  (0x00 << 6)
#define OTG_HS_TXFIFO_1  (0x01 << 6)
#define OTG_HS_TXFIFO_2  (0x02 << 6)
#define OTG_HS_TXFIFO_3  (0x03 << 6)
#define OTG_HS_TXFIFO_4  (0x04 << 6)
#define OTG_HS_TXFIFO_5  (0x05 << 6)
#define OTG_HS_TXFIFO_6  (0x06 << 6)
#define OTG_HS_TXFIFO_7  (0x07 << 6)
#define OTG_HS_TXFIFO_8  (0x08 << 6)
#define OTG_HS_TXFIFO_9  (0x09 << 6)
#define OTG_HS_TXFIFO_10 (0x0a << 6)
#define OTG_HS_TXFIFO_11 (0x0b << 6)
#define OTG_HS_TXFIFO_12 (0x0c << 6)
#define OTG_HS_TXFIFO_13 (0x0d << 6)
#define OTG_HS_TXFIFO_14 (0x0e << 6)
#define OTG_HS_TXFIFO_15 (0x0f << 6)
#define OTG_HS_TXFIFO_ALL (0x10 << 6)

/* This is the FIFO number that must be flushed using the TxFIFO Flush bit. This field must not be changed until the core clears the TxFIFO Flush bit.
00000: Nonperiodic TxFIFO flush in host mode | Tx FIFO 0 flush in peripheral mode
00001: Periodic TxFIFO flush in host mode | TXFIFO 1 flush in peripheral mode
00010: TXFIFO 2 flush in peripheral mode
...
′′
00101: TXFIFO 15 flush in peripheral mode
′′
10000: Flush all the transmit FIFOs in peripheral or host mode.
Note: Accessible in both peripheral and host modes. */

/* Bit 5 - TxFIFO flush */
#define OTG_HS_TXFFLSH (1 << 5)
/* This bit selectively flushes a single or all transmit FIFOs, but cannot do so if the core is in the midst of a transaction.
The application must write this bit only after checking that the core is neither writing to the TxFIFO nor reading from the TxFIFO. Verify using these registers:
- Read: the NAK effective interrupt ensures the core is not reading from the FIFO
- Write: the AHBIDL bit in OTG_HS_GRSTCTL ensures that the core is not writing anything to the FIFO
Note: Accessible in both peripheral and host modes. */

/* Bit 4 - RxFIFO flush */
#define OTG_HS_RXFFLSH (1 << 4)
/* The application can flush the entire RxFIFO using this bit, but must first ensure that the core is not in the middle of a transaction.
The application must only write to this bit after checking that the core is neither reading from the RxFIFO nor writing to the RxFIFO.
The application must wait until the bit is cleared before performing any other operation. This bit requires 8 clocks (slowest of PHY or AHB clock) to be cleared.
Note: Accessible in both peripheral and host modes. */

/* Bit 2 - Host frame counter reset */
#define OTG_HS_FCRST (1 << 2)
/* The application writes this bit to reset the frame number counter inside the core. When the frame counter is reset, the subsequent SOF sent out by the core has a frame number of 0.
Note: Only accessible in host mode. */

/* Bit 1 - HCLK soft reset */
#define OTG_HS_HSRST (1 << 1)
/* The application uses this bit to flush the control logic in the AHB Clock domain. Only AHB Clock Domain pipelines are reset.
FIFOs are not flushed with this bit.
All state machines in the AHB clock domain are reset to the Idle state after terminating the transactions on the AHB, following the protocol.
CSR control bits used by the AHB clock domain state machines are cleared.
To clear this interrupt, status mask bits that control the interrupt status and are generated by the AHB clock domain state machine are cleared.
Because interrupt status bits are not cleared, the application can get the status of any core events that occurred after it set this bit.
This is a self-clearing bit that the core clears after all necessary logic is reset in the core. This can take several clocks, depending on the core’s current state.
Note: Accessible in both peripheral and host modes. */

/* Bit 0 - Core soft reset */
#define OTG_HS_CSRST (1 << 0)
/* Resets the HCLK and PCLK domains as follows:
Clears the interrupts and all the CSR register bits except for the following bits:
– RSTPDMODL bit in OTG_HS_PCGCCTL
– GAYEHCLK bit in OTG_HS_PCGCCTL
– PWRCLMP bit in OTG_HS_PCGCCTL
– STPPCLK bit in OTG_HS_PCGCCTL
– FSLSPCS bit in OTG_HS_HCFG
– DSPD bit in OTG_HS_DCFG
All module state machines (except for the AHB slave unit) are reset to the Idle state, and all the transmit FIFOs and the receive FIFO are flushed.
Any transactions on the AHB Master are terminated as soon as possible, after completing the last data phase of an AHB transfer. Any transactions on the USB are terminated immediately.
The application can write to this bit any time it wants to reset the core. This is a self-clearing bit and the core clears this bit after all the necessary logic is reset in the core, which can take several clocks, depending on the current state of the core. Once this bit has been cleared, the software must wait at least 3 PHY clocks before accessing the PHY domain (synchronization delay). The software must also check that bit 31 in this register is set to 1 (AHB Master is Idle) before starting any operation.
Typically, the software reset is used during software development and also when you dynamically change the PHY selection bits in the above listed USB configuration registers.
When you change the PHY, the corresponding clock for the PHY is selected and used in the PHY domain. Once a new clock is selected, the PHY domain has to be reset for proper operation.
Note: Accessible in both peripheral and host modes. */


/* OTG_HS core interrupt register */
#define STM32F_OTG_HS_GINTSTS 0x014

/* This register interrupts the application for system-level events in the current mode (peripheral mode or host mode).
Some of the bits in this register are valid only in host mode, while others are valid in peripheral mode only. This register also indicates the current mode. To clear the interrupt status bits of the rc_w1 type, the application must write 1 into the bit.
The FIFO status interrupts are read-only; once software reads from or writes to the FIFO while servicing these interrupts, FIFO interrupt conditions are cleared automatically.
The application must clear the OTG_HS_GINTSTS register at initialization before unmasking the interrupt bit to avoid any interrupts generated prior to initialization. */

/* Bit 31 - Resume/remote wakeup detected interrupt */
#define OTG_HS_WKUPINT (1 << 31)
/* In peripheral mode, this interrupt is asserted when a resume is detected on the USB. In host mode, this interrupt is asserted when a remote wakeup is detected on the USB.
Note: Accessible in both peripheral and host modes. */

/* Bit 30 - Session request/new session detected interrupt */
#define OTG_HS_SRQINT (1 << 30)
/* In host mode, this interrupt is asserted when a session request is detected from the device. In peripheral mode, this interrupt is asserted when V BUS is in the valid range for a B-device device. Accessible in both peripheral and host modes. */

/* Bit 29 - Disconnect detected interrupt */
#define OTG_HS_DISCINT (1 << 29)
/* Asserted when a device disconnect is detected.
Note: Only accessible in host mode. */

/* Bit 28 - Connector ID status change */
#define OTG_HS_CIDSCHG (1 << 28)
/* The core sets this bit when there is a change in connector ID status.
Note: Accessible in both peripheral and host modes. */

/* Bit 27 Reserved, must be kept at reset value. */


/* Bit 26 - Periodic TxFIFO empty */
#define OTG_HS_PTXFE (1 << 26)
/* Asserted when the periodic transmit FIFO is either half or completely empty and there is space for at least one entry to be written in the periodic request queue. The half or completely empty status is determined by the periodic TxFIFO empty level bit in the Core AHB configuration register (PTXFELVL bit in OTG_HS_GAHBCFG).
Note: Only accessible in host mode. */

/* Bit 25 - Host channels interrupt */
#define OTG_HS_HCINT (1 << 25)
/* The core sets this bit to indicate that an interrupt is pending on one of the channels of the core (in host mode). The application must read the host all channels interrupt (OTG_HS_HAINT) register to determine the exact number of the channel on which the interrupt occurred, and then read the corresponding host channel-x interrupt (OTG_HS_HCINTx) register to determine the exact cause of the interrupt. The application must clear the appropriate status bit in the OTG_HS_HCINTx register to clear this bit.
Note: Only accessible in host mode. */

/* Bit 24 - Host port interrupt */
#define OTG_HS_HPRTINT (1 << 24)
/* The core sets this bit to indicate a change in port status of one of the OTG_HS controller ports in host mode. The application must read the host port control and status (OTG_HS_HPRT) register to determine the exact event that caused this interrupt. The application must clear the appropriate status bit in the host port control and status register to
clear this bit.
Note: Only accessible in host mode. */

/* Bit 23 Reserved, must be kept at reset value. */


/* Bit 22 - Data fetch suspended */
#define OTG_HS_DATAFSUSP (1 << 22)
/* This interrupt is valid only in DMA mode. This interrupt indicates that the core has stopped fetching data for IN endpoints due to the unavailability of TxFIFO space or request queue space. This interrupt is used by the application for an endpoint mismatch algorithm. For
example, after detecting an endpoint mismatch, the application:
– Sets a global nonperiodic IN NAK handshake
– Disables IN endpoints
– Flushes the FIFO
– Determines the token sequence from the IN token sequence learning queue
– Re-enables the endpoints
– Clears the global nonperiodic IN NAK handshake.
If the global nonperiodic IN NAK is cleared, the core has not yet fetched data for the IN endpoint, and the IN token is received: the core generates an “IN token received when FIFO empty” interrupt.
The OTG then sends a NAK response to the host. To avoid this scenario, the application can check the FetSusp interrupt in OTG_FS_GINTSTS, which ensures that the FIFO is full before clearing a global NAK handshake. Alternatively, the application can mask the “IN token received when FIFO empty” interrupt when clearing a global IN NAK handshake. */

/* Bit 21 - Incomplete periodic transfer */
#define OTG_HS_IPXFR (1 << 21)
/* In host mode, the core sets this interrupt bit when there are incomplete periodic transactions still pending, which are scheduled for the current frame.
Note: Only accessible in host mode. */

/* Bit 21 - Incomplete isochronous OUT transfer */
#define OTG_HS_INCOMPISOOUT (1 << 21)
/* In peripheral mode, the core sets this interrupt to indicate that there is at least one isochronous OUT endpoint on which the transfer is not completed in the current frame. This interrupt is asserted along with the End of periodic frame interrupt (EOPF) bit in this register.
Note: Only accessible in peripheral mode. */

/* Bit 20 - Incomplete isochronous IN transfer */
#define OTG_HS_IISOIXFR (1 << 20)
/* The core sets this interrupt to indicate that there is at least one isochronous IN endpoint on which the transfer is not completed in the current frame. This interrupt is asserted along with the End of periodic frame interrupt (EOPF) bit in this register.
Note: Only accessible in peripheral mode. */

/* Bit 19 - OUT endpoint interrupt */
#define OTG_HS_OEPINT (1 << 19)
/* The core sets this bit to indicate that an interrupt is pending on one of the OUT endpoints of the core (in peripheral mode). The application must read the device all endpoints interrupt (OTG_HS_DAINT) register to determine the exact number of the OUT endpoint on which the interrupt occurred, and then read the corresponding device OUT Endpoint-x Interrupt (OTG_HS_DOEPINTx) register to determine the exact cause of the interrupt. The application must clear the appropriate status bit in the corresponding OTG_HS_DOEPINTx register to clear this bit.
Note: Only accessible in peripheral mode. */

/* Bit 18 - IN endpoint interrupt */
#define OTG_HS_IEPINT (1 << 18)
/* The core sets this bit to indicate that an interrupt is pending on one of the IN endpoints of the core (in peripheral mode). The application must read the device All Endpoints Interrupt (OTG_HS_DAINT) register to determine the exact number of the IN endpoint on which the interrupt occurred, and then read the corresponding device IN Endpoint-x interrupt (OTG_HS_DIEPINTx) register to determine the exact cause of the interrupt. The application must clear the appropriate status bit in the corresponding OTG_HS_DIEPINTx register to clear this bit.
Note: Only accessible in peripheral mode. */

/* [17..16] Reserved, must be kept at reset value. */


/* Bit 15 - End of periodic frame interrupt */
#define OTG_HS_EOPF (1 << 15)
/* Indicates that the period specified in the periodic frame interval field of the device configuration register (PFIVL bit in OTG_HS_DCFG) has been reached in the current frame.
Note: Only accessible in peripheral mode. */

/* Bit 14 - Isochronous OUT packet dropped interrupt */
#define OTG_HS_ISOODRP (1 << 14)
/* The core sets this bit when it fails to write an isochronous OUT packet into the RxFIFO because the RxFIFO does not have enough space to accommodate a maximum size packet for the isochronous OUT endpoint.
Note: Only accessible in peripheral mode. */

/* Bit 13 - Enumeration done */
#define OTG_HS_ENUMDNE (1 << 13)
/* The core sets this bit to indicate that speed enumeration is complete. The application must read the device Status (OTG_HS_DSTS) register to obtain the enumerated speed.
Note: Only accessible in peripheral mode. */

/* Bit 12 - USB reset */
#define OTG_HS_USBRST (1 << 12)
/* The core sets this bit to indicate that a reset is detected on the USB.
Note: Only accessible in peripheral mode. */

/* Bit 11 - USB suspend */
#define OTG_HS_USBSUSP (1 << 11)
/* The core sets this bit to indicate that a suspend was detected on the USB. The core enters the Suspended state when there is no activity on the data lines for a period of 3 ms.
Note: Only accessible in peripheral mode. */

/* Bit 10 - Early suspend */
#define OTG_HS_ESUSP (1 << 10)
/* The core sets this bit to indicate that an Idle state has been detected on the USB for 3 ms.
Note: Only accessible in peripheral mode. */

/* Bit 7 - Global OUT NAK effective */
#define OTG_HS_GONAKEFF (1 << 7)
/* Indicates that the Set global OUT NAK bit in the Device control register (SGONAK bit in OTG_HS_DCTL), set by the application, has taken effect in the core. This bit can be cleared by writing the Clear global OUT NAK bit in the Device control register (CGONAK bit in OTG_HS_DCTL).
Note: Only accessible in peripheral mode. */

/* Bit 6 - Global IN nonperiodic NAK effective */
#define OTG_HS_GINAKEFF (1 << 6)
/* Indicates that the Set global nonperiodic IN NAK bit in the Device control register (SGINAK bit in OTG_HS_DCTL), set by the application, has taken effect in the core. That is, the core has sampled the Global IN NAK bit set by the application. This bit can be cleared by clearing the Clear global nonperiodic IN NAK bit in the Device control register (CGINAK bit in OTG_HS_DCTL).
This interrupt does not necessarily mean that a NAK handshake is sent out on the USB. The STALL bit takes precedence over the NAK bit.
Note: Only accessible in peripheral mode. */

/* Bit 5 - Nonperiodic TxFIFO empty */
#define OTG_HS_NPTXFE (1 << 5)
/* This interrupt is asserted when the nonperiodic TxFIFO is either half or completely empty, and there is space in at least one entry to be written to the nonperiodic transmit request queue. The half or completely empty status is determined by the nonperiodic TxFIFO empty level bit in the OTG_HS_GAHBCFG register (TXFELVL bit in OTG_HS_GAHBCFG).
Note: Only accessible in host mode. */

/* Bit 4 - RxFIFO nonempty */
#define OTG_HS_RXFLVL (1 << 4)
/* Indicates that there is at least one packet pending to be read from the RxFIFO.
Note: Accessible in both host and peripheral modes. */

/* Bit 3 - Start of frame */
#define OTG_HS_SOF (1 << 3)
/* In host mode, the core sets this bit to indicate that an SOF (FS), or Keep-Alive (LS) is transmitted on the USB. The application must write a 1 to this bit to clear the interrupt. In peripheral mode, in the core sets this bit to indicate that an SOF token has been received on the USB. The application can read the Device Status register to get the current frame number. This interrupt is seen only when the core is operating in FS.
Note: Accessible in both host and peripheral modes. */

/* Bit 2 - OTG interrupt */
#define OTG_HS_OTGINT (1 << 2)
/* The core sets this bit to indicate an OTG protocol event. The application must read the OTG Interrupt Status (OTG_HS_GOTGINT) register to determine the exact event that caused this interrupt. The application must clear the appropriate status bit in the OTG_HS_GOTGINT register to clear this bit.
Note: Accessible in both host and peripheral modes. */

/* Bit 1 - Mode mismatch interrupt */
#define OTG_HS_MMIS (1 << 1)
/* The core sets this bit when the application is trying to access:
A host mode register, when the core is operating in peripheral mode
A peripheral mode register, when the core is operating in host mode
The register access is completed on the AHB with an OKAY response, but is ignored by the core internally and does not affect the operation of the core.
Note: Accessible in both host and peripheral modes. */

/* Bit 0 - Current mode of operation */
#define OTG_HS_CMOD (1 << 0)
/* Indicates the current mode.
0: Peripheral mode
1: Host mode
Note: Accessible in both host and peripheral modes. */


/* OTG_HS interrupt mask register */
#define STM32F_OTG_HS_GINTMSK 0x018

/* This register works with the Core interrupt register to interrupt the application. When an interrupt bit is masked, the interrupt associated with that bit is not generated. However, the
Core Interrupt (OTG_HS_GINTSTS) register bit corresponding to that interrupt is still set. */

/* Bit 31 - Resume/remote wakeup detected interrupt mask */
#define OTG_HS_WUIM (1 << 31)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Accessible in both host and peripheral modes. */

/* Bit 30 - Session request/new session detected interrupt mask */
#define OTG_HS_SRQIM (1 << 30)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Accessible in both host and peripheral modes. */

/* Bit 29 - Disconnect detected interrupt mask */
#define OTG_HS_DISCINT (1 << 29)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Accessible in both host and peripheral modes. */

/* Bit 28 - Connector ID status change mask */
#define OTG_HS_CIDSCHGM (1 << 28)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Accessible in both host and peripheral modes. */

/* Bit 27 Reserved, must be kept at reset value. */


/* Bit 26 - Periodic TxFIFO empty mask */
#define OTG_HS_PTXFEM (1 << 26)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in host mode. */

/* Bit 25 - Host channels interrupt mask */
#define OTG_HS_HCIM (1 << 25)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in host mode. */

/* Bit 24 - Host port interrupt mask */
#define OTG_HS_PRTIM (1 << 24)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in host mode. */

/* Bit 23 Reserved, must be kept at reset value.  */


/* Bit 22 - Data fetch suspended mask */
#define OTG_HS_FSUSPM (1 << 22)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* Bit 21 - Incomplete periodic transfer mask */
#define OTG_HS_IPXFRM (1 << 21)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in host mode. */

/* Bit 21 - Incomplete isochronous OUT transfer mask */
#define OTG_HS_IISOOXFRM (1 << 21)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* Bit 20 - Incomplete isochronous IN transfer mask */
#define OTG_HS_IISOIXFRM (1 << 20)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* Bit 19 - OUT endpoints interrupt mask */
#define OTG_HS_OEPINTM (1 << 19)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* Bit 18 - IN endpoints interrupt mask */
#define OTG_HS_IEPINTM (1 << 18)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* Bit 17 - Endpoint mismatch interrupt mask */
#define OTG_HS_EPMISM (1 << 17)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* Bit 16 Reserved, must be kept at reset value. */


/* Bit 15 - End of periodic frame interrupt mask */
#define OTG_HS_EOPFM (1 << 15)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* Bit 14 - Isochronous OUT packet dropped interrupt mask */
#define OTG_HS_ISOODRPM (1 << 14)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* Bit 13 - Enumeration done mask */
#define OTG_HS_ENUMDNEM (1 << 13)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* Bit 12 - USB reset mask */
#define OTG_HS_USBRSTM (1 << 12)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* Bit 11 - USB suspend mask */
#define OTG_HS_USBSUSPM (1 << 11)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* Bit 10 - Early suspend mask */
#define OTG_HS_ESUSPM (1 << 10)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* [9..8] Reserved, must be kept at reset value. */


/* Bit 7 - Global OUT NAK effective mask */
#define OTG_HS_GONAKEFFM (1 << 7)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* Bit 6 - Global nonperiodic IN NAK effective mask */
#define OTG_HS_GINAKEFFM (1 << 6)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Only accessible in peripheral mode. */

/* Bit 5 - Nonperiodic TxFIFO empty mask */
#define OTG_HS_NPTXFEM (1 << 5)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Accessible in both peripheral and host modes. */

/* Bit 4 - Receive FIFO nonempty mask */
#define OTG_HS_RXFLVLM (1 << 4)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Accessible in both peripheral and host modes. */

/* Bit 3 - Start of frame mask */
#define OTG_HS_SOFM (1 << 3)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Accessible in both peripheral and host modes. */

/* Bit 2 - OTG interrupt mask */
#define OTG_HS_OTGINTM (1 << 2)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Accessible in both peripheral and host modes. */

/* Bit 1 - Mode mismatch interrupt mask */
#define OTG_HS_MMISM (1 << 1)
/* 0: Masked interrupt
1: Unmasked interrupt
Note: Accessible in both peripheral and host modes. */

/* Bit 0 Reserved, must be kept at reset value. */


/* OTG_HS Receive status debug read/OTG status read and pop registers */
#define STM32F_OTG_HS_GRXSTSR 0x01C
#define STM32F_OTG_HS_GRXSTSP 0x020

/* A read to the Receive status debug read register returns the contents of the top of the Receive FIFO. A read to the Receive status read and pop register additionally pops the top data entry out of the RxFIFO.
The receive status contents must be interpreted differently in host and peripheral modes.
The core ignores the receive status pop/read when the receive FIFO is empty and returns a value of 0x0000 0000. The application must only pop the Receive Status FIFO when the Receive FIFO nonempty bit of the Core interrupt register (RXFLVL bit in
OTG_HS_GINTSTS) is asserted. */

/* [31..21] Reserved, must be kept at reset value. */


/* Bits [20..17] - Packet status */
#define OTG_HS_PKTSTS_MSK (((1 << (20 - 17 + 1)) - 1) << 17)
#define OTG_HS_PKTSTS_SET(VAL) (((VAL) << 17) & OTG_HS_PKTSTS_MSK)
#define OTG_HS_PKTSTS_GET(REG) (((REG) & OTG_HS_PKTSTS_MSK) >> 17)

/* Indicates the status of the received packet
0010: IN data packet received
0011: IN transfer completed (triggers an interrupt)
0101: Data toggle error (triggers an interrupt)
0111: Channel halted (triggers an interrupt)
Others: Reserved */

/* Bits [16..15] - Data PID */
#define OTG_HS_DPID_MSK (((1 << (16 - 15 + 1)) - 1) << 15)
#define OTG_HS_DPID_SET(VAL) (((VAL) << 15) & OTG_HS_DPID_MSK)
#define OTG_HS_DPID_GET(REG) (((REG) & OTG_HS_DPID_MSK) >> 15)
/* Indicates the Data PID of the received packet
00: DATA0
10: DATA1
01: DATA2
11: MDATA */

/* Bits [14..4] - Byte count */
#define OTG_HS_BCNT_MSK (((1 << (14 - 4 + 1)) - 1) << 4)
#define OTG_HS_BCNT_SET(VAL) (((VAL) << 4) & OTG_HS_BCNT_MSK)
#define OTG_HS_BCNT_GET(REG) (((REG) & OTG_HS_BCNT_MSK) >> 4)
/* Indicates the byte count of the received IN data packet. */

/* Bits [3..0] - Channel number */
#define OTG_HS_CHNUM_MSK (((1 << (3 - 0 + 1)) - 1) << 0)
#define OTG_HS_CHNUM_SET(VAL) (((VAL) << 0) & OTG_HS_CHNUM_MSK)
#define OTG_HS_CHNUM_GET(REG) (((REG) & OTG_HS_CHNUM_MSK) >> 0)
/* Indicates the channel number to which the current received packet belongs. */ 
/* [31..25] Reserved, must be kept at reset value. */


/* Bits [24..21] - Frame number */
#define OTG_HS_FRMNUM_MSK (((1 << (24 - 21 + 1)) - 1) << 21)
#define OTG_HS_FRMNUM_SET(VAL) (((VAL) << 21) & OTG_HS_FRMNUM_MSK)
#define OTG_HS_FRMNUM_GET(REG) (((REG) & OTG_HS_FRMNUM_MSK) >> 21)
/* This is the least significant 4 bits of the frame number in which the packet is received on the USB. This field is supported only when isochronous OUT endpoints are supported. */

/* Bits [20..17] - Packet status */
#define OTG_HS_PKTSTS_MSK (((1 << (20 - 17 + 1)) - 1) << 17)
#define OTG_HS_PKTSTS_SET(VAL) (((VAL) << 17) & OTG_HS_PKTSTS_MSK)
#define OTG_HS_PKTSTS_GET(REG) (((REG) & OTG_HS_PKTSTS_MSK) >> 17)

#define OTG_HS_PKTSTS_GOUT_NACK      (0x1 << 17)
#define OTG_HS_PKTSTS_OUT_DATA_UPDT  (0x2 << 17)
#define OTG_HS_PKTSTS_OUT_XFER_COMP  (0x3 << 17)
#define OTG_HS_PKTSTS_SETUP_COMP     (0x4 << 17)
#define OTG_HS_PKTSTS_SETUP_UPDT     (0x6 << 17)

/* Indicates the status of the received packet
0001: Global OUT NAK (triggers an interrupt)
0010: OUT data packet received
0011: OUT transfer completed (triggers an interrupt)
0100: SETUP transaction completed (triggers an interrupt)
0110: SETUP data packet received
Others: Reserved */

/* Bits [16..15] - Data PID */
#define __OTG_HS_DPID (((1 << (16 - 15 + 1)) - 1) << 15)
/* Indicates the Data PID of the received OUT data packet
00: DATA0
10: DATA1
01: DATA2
11: MDATA */

/* Bits [14..4] - Byte count */
#define OTG_HS_BCNT_MSK (((1 << (14 - 4 + 1)) - 1) << 4)
#define OTG_HS_BCNT_SET(VAL) (((VAL) << 4) & OTG_HS_BCNT_MSK)
#define OTG_HS_BCNT_GET(REG) (((REG) & OTG_HS_BCNT_MSK) >> 4)
/* Indicates the byte count of the received data packet. */ 

/* Bits [3..0] - Endpoint number */
#define OTG_HS_EPNUM_MSK (((1 << (3 - 0 + 1)) - 1) << 0)
#define OTG_HS_EPNUM_SET(VAL) (((VAL) << 0) & OTG_HS_EPNUM_MSK)
#define OTG_HS_EPNUM_GET(REG) (((REG) & OTG_HS_EPNUM_MSK) >> 0)
/* Indicates the endpoint number to which the current received packet belongs. */


/* OTG_HS Receive FIFO size register */
#define STM32F_OTG_HS_GRXFSIZ 0x024
/* The application can program the RAM size that must be allocated to the RxFIFO. */

/* [31..16] Reserved, must be kept at reset value. */


/* Bits [15..0] - RxFIFO depth */
#define OTG_HS_RXFD_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_RXFD_SET(VAL) (((VAL) << 0) & OTG_HS_RXFD_MSK)
#define OTG_HS_RXFD_GET(REG) (((REG) & OTG_HS_RXFD_MSK) >> 0)
/* This value is in terms of 32-bit words.
Minimum value is 16
Maximum value is 1024
The power-on reset value of this register is specified as the largest Rx data FIFO depth.
OTG_HS nonperiodic transmit FIFO size/Endpoint 0 transmit FIFO size register (OTG_HS_GNPTXFSIZ/OTG_HS_TX0FSIZ)
Address offset: 0x028 */


/* Bits [31..16] - Nonperiodic TxFIFO depth */
#define OTG_HS_NPTXFD_MSK (((1 << (31 - 16 + 1)) - 1) << 16)
#define OTG_HS_NPTXFD_SET(VAL) (((VAL) << 16) & OTG_HS_NPTXFD_MSK)
#define OTG_HS_NPTXFD_GET(REG) (((REG) & OTG_HS_NPTXFD_MSK) >> 16)
/* This value is in terms of 32-bit words.
Minimum value is 16
Maximum value is 1024 */

/* Bits [15..0] - Nonperiodic transmit RAM start address */
#define OTG_HS_NPTXFSA_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_NPTXFSA_SET(VAL) (((VAL) << 0) & OTG_HS_NPTXFSA_MSK)
#define OTG_HS_NPTXFSA_GET(REG) (((REG) & OTG_HS_NPTXFSA_MSK) >> 0)
/* This field contains the memory start address for nonperiodic transmit FIFO RAM. */

/* Bits [31..16] - Endpoint 0 TxFIFO depth */
#define OTG_HS_TX0FD_MSK (((1 << (31 - 16 + 1)) - 1) << 16)
#define OTG_HS_TX0FD_SET(VAL) (((VAL) << 16) & OTG_HS_TX0FD_MSK)
#define OTG_HS_TX0FD_GET(REG) (((REG) & OTG_HS_TX0FD_MSK) >> 16)
/* This value is in terms of 32-bit words.
Minimum value is 16
Maximum value is 256 */

/* Bits [15..0] - Endpoint 0 transmit RAM start address */
#define OTG_HS_TX0FSA_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_TX0FSA_SET(VAL) (((VAL) << 0) & OTG_HS_TX0FSA_MSK)
#define OTG_HS_TX0FSA_GET(REG) (((REG) & OTG_HS_TX0FSA_MSK) >> 0)
/* This field contains the memory start address for Endpoint 0 transmit FIFO RAM. */


/* OTG_HS nonperiodic transmit FIFO/queue status register */
#define STM32F_OTG_HS_GNPTXSTS 0x02C

/* Note:
In peripheral mode, this register is not valid.
This read-only register contains the free space information for the nonperiodic TxFIFO and the nonperiodic transmit request queue. */

/* Bit 31 Reserved, must be kept at reset value. */


/* Bits [30..24] - Top of the nonperiodic transmit request queue Entry in the nonperiodic Tx request queue that is currently being processed by the MAC. */
#define OTG_HS_NPTXQTOP_MSK (((1 << (30 - 24 + 1)) - 1) << 24)
#define OTG_HS_NPTXQTOP_SET(VAL) (((VAL) << 24) & OTG_HS_NPTXQTOP_MSK)
#define OTG_HS_NPTXQTOP_GET(REG) (((REG) & OTG_HS_NPTXQTOP_MSK) >> 24)
/* Bits [30:27]: Channel/endpoint number
Bits [26:25]:
– 00: IN/OUT token
– 01: Zero-length transmit packet (device IN/host OUT)
– 10: PING/CSPLIT token
– 11: Channel halt command
Bit [24]: Terminate (last entry for selected channel/endpoint) */

/* Bits [23..16] - Nonperiodic transmit request queue space available */
#define OTG_HS_NPTQXSAV_MSK (((1 << (23 - 16 + 1)) - 1) << 16)
#define OTG_HS_NPTQXSAV_SET(VAL) (((VAL) << 16) & OTG_HS_NPTQXSAV_MSK)
#define OTG_HS_NPTQXSAV_GET(REG) (((REG) & OTG_HS_NPTQXSAV_MSK) >> 16)
/* Indicates the amount of free space available in the nonperiodic transmit request queue.
This queue holds both IN and OUT requests in host mode. Peripheral mode has only IN requests.
00: Nonperiodic transmit request queue is full
01: dx1 location available
10: dx2 locations available
bxn: dxn locations available (0 ≤ n ≤ dx8)
Others: Reserved */

/* Bits [15..0] - Nonperiodic TxFIFO space available */
#define OTG_HS_NPTXFSAV_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_NPTXFSAV_SET(VAL) (((VAL) << 0) & OTG_HS_NPTXFSAV_MSK)
#define OTG_HS_NPTXFSAV_GET(REG) (((REG) & OTG_HS_NPTXFSAV_MSK) >> 0)
/* Indicates the amount of free space available in the nonperiodic TxFIFO.
Values are in terms of 32-bit words.
00: Nonperiodic TxFIFO is full
01: dx1 word available
10: dx2 words available
0xn: dxn words available (where 0 ≤ n ≤ dx1024)
Others: Reserved */


/* OTG_HSI2C access register */
#define STM32F_OTG_HS_GI2CCTL 0x030



/* Bit 31 - I2C Busy/Done */
#define OTG_HS_BSYDNE (1 << 31)
/* The application sets this bit to 1 to start a request on the I2C interface. When the transfer is complete, the core deasserts this bit to 0. As long as the bit is set indicating that the I2C interface is busy, the application cannot start another request on the interface. */

/* Bit 30 - Read/Write Indicator */
#define OTG_HS_RW (1 << 30)
/* This bit indicates whether a read or write register transfer must be performed on the interface.
0: Write
1: Read
Note: Read/write bursting is not supported for registers. */

/* Bit 29 Reserved, must be kept at reset value. */


/* Bit 28 - I2C DatSe0 USB mode */
#define OTG_HS_I2CDATSE0 (1 << 28)
/* This bit is used to select the full-speed interface USB mode.
0: VP_VM USB mode
1: DAT_SE0 USB mode */

/* Bits [27..26] - I2C Device Address */
#define OTG_HS_I2CDEVADR (((1 << (27 - 26 + 1)) - 1) << 26)
/* This bit selects the address of the I2C slave on the USB 1.1 full-speed serial transceiver corresponding to the one used by the core for OTG signalling. */

/* Bit 25 Reserved, must be kept at reset value. */


/* Bit 24 - I2C ACK */
#define OTG_HS_I2CACK (1 << 24)
/* This bit indicates whether an ACK response was received from the I2C slave. It is valid when BSYDNE is cleared by the core, after the application has initiated an I2C access.
0: NAK
1: ACK */

/* Bit 23 - I2C Enable */
#define OTG_HS_I2CEN (1 << 23)
/* This bit enables the I2C master to initiate transactions on the I2C interface. */

/* Bits [22..16] - I2C Address */
#define OTG_HS_ADDR_MSK (((1 << (22 - 16 + 1)) - 1) << 16)
#define OTG_HS_ADDR_SET(VAL) (((VAL) << 16) & OTG_HS_ADDR_MSK)
#define OTG_HS_ADDR_GET(REG) (((REG) & OTG_HS_ADDR_MSK) >> 16)
/* This is the 7-bit I2C device address used by the application to access any external I2C slave, including the I2C slave on a USB 1.1 OTG full-speed serial transceiver. */

/* Bits [15..8] - I2C Register Address */
#define OTG_HS_REGADDR_MSK (((1 << (15 - 8 + 1)) - 1) << 8)
#define OTG_HS_REGADDR_SET(VAL) (((VAL) << 8) & OTG_HS_REGADDR_MSK)
#define OTG_HS_REGADDR_GET(REG) (((REG) & OTG_HS_REGADDR_MSK) >> 8)
/* These bits allow to program the address of the register to be read from or written to. */

/* Bits [7..0] - I2C Read/Write Data */
#define OTG_HS_RWDATA_MSK (((1 << (7 - 0 + 1)) - 1) << 0)
#define OTG_HS_RWDATA_SET(VAL) (((VAL) << 0) & OTG_HS_RWDATA_MSK)
#define OTG_HS_RWDATA_GET(REG) (((REG) & OTG_HS_RWDATA_MSK) >> 0)
/* After a register read operation, these bits hold the read data for the application.
During a write operation, the application can use this register to program the data to be written to a register. */


/* OTG_HS general core configuration register */
#define STM32F_OTG_HS_GCCFG 0x038



/* [31..22] Reserved, must be kept at reset value. */


/* Bit 21 - V BUS sensing disable option */
#define OTG_HS_NOVBUSSENS (1 << 21)
/* When this bit is set, V BUS is considered internally to be always at V BUS valid level (5 V). This option removes the need for a dedicated V BUS pad, and leave this pad free to be used for other purposes such as a shared functionality. V BUS connection can be remapped on another general purpose input pad and monitored by software.
This option is only suitable for host-only or device-only applications.
0: V BUS sensing available by hardware
1: V BUS sensing not available by hardware. */

/* Bit 20 - SOF output enable */
#define OTG_HS_SOFOUTEN (1 << 20)
/* 0: SOF pulse not available on PAD
1: SOF pulse available on PAD */

/* Bit 19 - Enable the V BUS sensing “B” device */
#define OTG_HS_VBUSBSEN (1 << 19)
/* 0: V BUS sensing “B” disabled
1: V BUS sensing “B” enabled */

/* Bit 18 - Enable the V BUS sensing “A” device */
#define OTG_HS_VBUSASEN (1 << 18)
/* 0: V BUS sensing “A” disabled
1: V BUS sensing “A” enabled */

/* Bit 17 - Enable I2C bus connection for the external I2C PHY interface. */
#define OTG_HS_I2CPADEN (1 << 17)
/* 0: I2C bus disabled
1: I2C bus enabled */

/* Bit 16 - Power down */
#define OTG_HS_PWRDWN (1 << 16)
/* Used to activate the transceiver in transmission/reception
0: Power down active
1: Power down deactivated (“Transceiver active”) */

/* [15..0] Reserved, must be kept at reset value. */



/* OTG_HS core ID register */
#define STM32F_OTG_HS_CID 0x03C

/* This is a read only register containing the Product ID. */

/* Bits [31..0] - Product ID field */
#define OTG_HS_PRODUCT_ID_MSK (((1 << (31 - 0 + 1)) - 1) << 0)
#define OTG_HS_PRODUCT_ID_SET(VAL) (((VAL) << 0) & OTG_HS_PRODUCT_ID_MSK)
#define OTG_HS_PRODUCT_ID_GET(REG) (((REG) & OTG_HS_PRODUCT_ID_MSK) >> 0)
/* Application-programmable ID field. */


/* OTG_HS Host periodic transmit FIFO size register */
#define STM32F_OTG_HS_HPTXFSIZ 0x100



/* Bits [31..16] - Host periodic TxFIFO depth */
#define OTG_HS_PTXFD_MSK (((1 << (31 - 16 + 1)) - 1) << 16)
#define OTG_HS_PTXFD_SET(VAL) (((VAL) << 16) & OTG_HS_PTXFD_MSK)
#define OTG_HS_PTXFD_GET(REG) (((REG) & OTG_HS_PTXFD_MSK) >> 16)
/* This value is in terms of 32-bit words.
Minimum value is 16
Maximum value is 512 */

/* Bits [15..0] - Host periodic TxFIFO start address */
#define OTG_HS_PTXSA_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_PTXSA_SET(VAL) (((VAL) << 0) & OTG_HS_PTXSA_MSK)
#define OTG_HS_PTXSA_GET(REG) (((REG) & OTG_HS_PTXSA_MSK) >> 0)
/* The power-on reset value of this register is the sum of the largest Rx data FIFO depth and
largest nonperiodic Tx data FIFO depth. */


/* OTG_HS device IN endpoint transmit FIFO size register */
#define STM32F_OTG_HS_DIEPTXFX 0x104



/* Bits [31..16] - IN endpoint TxFIFO depth */
#define OTG_HS_INEPTXFD_MSK (((1 << (31 - 16 + 1)) - 1) << 16)
#define OTG_HS_INEPTXFD_SET(VAL) (((VAL) << 16) & OTG_HS_INEPTXFD_MSK)
#define OTG_HS_INEPTXFD_GET(REG) (((REG) & OTG_HS_INEPTXFD_MSK) >> 16)
/* This value is in terms of 32-bit words.
Minimum value is 16
Maximum value is 512
The power-on reset value of this register is specified as the largest IN endpoint FIFO
number depth. */

/* Bits [15..0] - IN endpoint FIFOx transmit RAM start address */
#define OTG_HS_INEPTXSA_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_INEPTXSA_SET(VAL) (((VAL) << 0) & OTG_HS_INEPTXSA_MSK)
#define OTG_HS_INEPTXSA_GET(REG) (((REG) & OTG_HS_INEPTXSA_MSK) >> 0)
/* This field contains the memory start address for IN endpoint transmit FIFOx. The address
must be aligned with a 32-bit memory location.

Host-mode registers
Bit values in the register descriptions are expressed in binary unless otherwise specified.
Host-mode registers affect the operation of the core in the host mode. Host mode registers must not be accessed in peripheral mode, as the results are undefined. Host mode registers can be categorized as follows: */


/* OTG_HS host configuration register */
#define STM32F_OTG_HS_HCFG 0x400

/* This register configures the core after power-on. Do not change to this register after initializing the host. */

/* [31..3] Reserved, must be kept at reset value. */


/* Bit 2 - FS- and LS-only support */
#define OTG_HS_FSLSS (1 << 2)
/* The application uses this bit to control the core’s enumeration speed. Using this bit, the application can make the core enumerate as an FS host, even if the connected device supports HS traffic. Do not make changes to this field after initial programming. 
0: HS/FS/LS, based on the maximum speed supported by the connected device
1: FS/LS-only, even if the connected device can support HS (read-only) */

/* Bits [1..0] - FS/LS PHY clock select */
#define OTG_HS_FSLSPCS_MSK (((1 << (1 - 0 + 1)) - 1) << 0)
#define OTG_HS_FSLSPCS_SET(VAL) (((VAL) << 0) & OTG_HS_FSLSPCS_MSK)
#define OTG_HS_FSLSPCS_GET(REG) (((REG) & OTG_HS_FSLSPCS_MSK) >> 0)
/* When the core is in FS host mode:
01: PHY clock is running at 48 MHz
Others: Reserved
When the core is in LS host mode:
00: Reserved
01: PHY clock is running at 48 MHz.
10: Select 6 MHz PHY clock frequency
11: Reserved
Note: The FSLSPCS bit must be set on a connection event according to the speed of the connected device. A software reset must be performed after changing this bit. */


/* OTG_HS Host frame interval register */
#define STM32F_OTG_HS_HFIR 0x404

/* This register stores the frame interval information for the current speed to which the OTG_HS controller has enumerated. */

/* [31..16] Reserved, must be kept at reset value. */


/* Bits [15..0] - Frame interval */
#define OTG_HS_FRIVL_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_FRIVL_SET(VAL) (((VAL) << 0) & OTG_HS_FRIVL_MSK)
#define OTG_HS_FRIVL_GET(REG) (((REG) & OTG_HS_FRIVL_MSK) >> 0)
/* The value that the application programs to this field specifies the interval between two consecutive SOFs (FS), micro-SOFs (HS) or Keep-Alive tokens (LS). This field contains the number of PHY clocks that constitute the required frame interval. The application can write a value to this register only after the Port enable bit of the host port control and status register (PENA bit in OTG_HS_HPRT) has been set. If no value is programmed, the core calculates the value based on the PHY clock specified in the FS/LS PHY Clock Select field of the Host configuration register (FSLSPCS in OTG_HS_HCFG): frame duration × PHY clock frequency
Note: The FRIVL bit can be modified whenever the application needs to change the Frame interval time. */


/* OTG_HS host frame number/frame time remaining register */
#define STM32F_OTG_HS_HFNUM 0x408

/* This register indicates the current frame number. It also indicates the time remaining (in terms of the number of PHY clocks) in the current frame. */

/* Bits [31..16] - Frame time remaining */
#define OTG_HS_FTREM_MSK (((1 << (31 - 16 + 1)) - 1) << 16)
#define OTG_HS_FTREM_SET(VAL) (((VAL) << 16) & OTG_HS_FTREM_MSK)
#define OTG_HS_FTREM_GET(REG) (((REG) & OTG_HS_FTREM_MSK) >> 16)
/* Indicates the amount of time remaining in the current frame, in terms of PHY clocks. This field decrements on each PHY clock. When it reaches zero, this field is reloaded with the value in the Frame interval register and a new SOF is transmitted on the USB. */

/* Bits [15..0] - Frame number */
#define OTG_HS_FRNUM_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_FRNUM_SET(VAL) (((VAL) << 0) & OTG_HS_FRNUM_MSK)
#define OTG_HS_FRNUM_GET(REG) (((REG) & OTG_HS_FRNUM_MSK) >> 0)
/* This field increments when a new SOF is transmitted on the USB, and is cleared to 0 when it reaches 0x3FFF. */


/* OTG_HS_Host periodic transmit FIFO/queue status register */
#define STM32F_OTG_HS_HPTXSTS 0x410

/* This read-only register contains the free space information for the periodic TxFIFO and the periodic transmit request queue. */

/* Bits [31..24] - Top of the periodic transmit request queue */
#define OTG_HS_PTXQTOP_MSK (((1 << (31 - 24 + 1)) - 1) << 24)
#define OTG_HS_PTXQTOP_SET(VAL) (((VAL) << 24) & OTG_HS_PTXQTOP_MSK)
#define OTG_HS_PTXQTOP_GET(REG) (((REG) & OTG_HS_PTXQTOP_MSK) >> 24)
/* This indicates the entry in the periodic Tx request queue that is currently being processed by the MAC.
This register is used for debugging.
Bit [31]: Odd/Even frame
– 0: send in even (micro) frame
– 1: send in odd (micro) frame
Bits [30:27]: Channel/endpoint number
Bits [26:25]: Type
– 00: IN/OUT
– 01: Zero-length packet
– 11: Disable channel command
Bit [24]: Terminate (last entry for the selected channel/endpoint) */

/* Bits [23..16] - Periodic transmit request queue space available */
#define OTG_HS_PTXQSAV_MSK (((1 << (23 - 16 + 1)) - 1) << 16)
#define OTG_HS_PTXQSAV_SET(VAL) (((VAL) << 16) & OTG_HS_PTXQSAV_MSK)
#define OTG_HS_PTXQSAV_GET(REG) (((REG) & OTG_HS_PTXQSAV_MSK) >> 16)
/* Indicates the number of free locations available to be written in the periodic transmit request
queue. This queue holds both IN and OUT requests.
00: Periodic transmit request queue is full
01: dx1 location available
10: dx2 locations available
bxn: dxn locations available (0 ≤ dxn ≤ PTXFD)
Others: Reserved */

/* Bits [15..0] - Periodic transmit data FIFO space available */
#define OTG_HS_PTXFSAVL_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_PTXFSAVL_SET(VAL) (((VAL) << 0) & OTG_HS_PTXFSAVL_MSK)
#define OTG_HS_PTXFSAVL_GET(REG) (((REG) & OTG_HS_PTXFSAVL_MSK) >> 0)
/* Indicates the number of free locations available to be written to in the periodic TxFIFO.
Values are in terms of 32-bit words
0000: Periodic TxFIFO is full
0001: dx1 word available
0010: dx2 words available
bxn: dxn words available (where 0 ≤ dxn ≤ dx512)
Others: Reserved */


/* OTG_HS Host all channels interrupt register */
#define STM32F_OTG_HS_HAINT 0x414

/* When a significant event occurs on a channel, the host all channels interrupt register interrupts the application using the host channels interrupt bit of the Core interrupt register (HCINT bit in OTG_HS_GINTSTS). This is shown in Figure 412. There is one interrupt bit per channel, up to a maximum of 16 bits. Bits in this register are set and cleared when the application sets and clears bits in the corresponding host channel-x interrupt register. */

/* [31..16] Reserved, must be kept at reset value. */


/* Bits [15..0] - Channel interrupts */
#define OTG_HS_HAINT_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_HAINT_SET(VAL) (((VAL) << 0) & OTG_HS_HAINT_MSK)
#define OTG_HS_HAINT_GET(REG) (((REG) & OTG_HS_HAINT_MSK) >> 0)
/* One bit per channel: Bit 0 for Channel 0, bit 15 for Channel 15 */


/* OTG_HS host all channels interrupt mask register */
#define STM32F_OTG_HS_HAINTMSK 0x418

/* The host all channel interrupt mask register works with the host all channel interrupt register to interrupt the application when an event occurs on a channel. There is one interrupt mask bit per channel, up to a maximum of 16 bits. */

/* [31..16] Reserved, must be kept at reset value. */


/* Bits [15..0] - Channel interrupt mask */
#define OTG_HS_HAINTM_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_HAINTM_SET(VAL) (((VAL) << 0) & OTG_HS_HAINTM_MSK)
#define OTG_HS_HAINTM_GET(REG) (((REG) & OTG_HS_HAINTM_MSK) >> 0)
/* 0: Masked interrupt
1: Unmasked interrupt
One bit per channel: Bit 0 for channel 0, bit 15 for channel 15 */


/* OTG_HS host port control and status register */
#define STM32F_OTG_HS_HPRT 0x440

/* This register is available only in host mode. Currently, the OTG host supports only one port.
A single register holds USB port-related information such as USB reset, enable, suspend, resume, connect status, and test mode for each port. It is shown in Figure 412. The rc_w1 bits in this register can trigger an interrupt to the application through the host port interrupt bit of the core interrupt register (HPRTINT bit in OTG_HS_GINTSTS). On a Port Interrupt, the application must read this register and clear the bit that caused the interrupt. For the rc_w1 bits, the application must write a 1 to the bit to clear the interrupt. */

/* [31..19] Reserved, must be kept at reset value. */


/* Bits [18..17] - Port speed */
#define OTG_HS_PSPD_MSK (((1 << (18 - 17 + 1)) - 1) << 17)
#define OTG_HS_PSPD_SET(VAL) (((VAL) << 17) & OTG_HS_PSPD_MSK)
#define OTG_HS_PSPD_GET(REG) (((REG) & OTG_HS_PSPD_MSK) >> 17)
/* Indicates the speed of the device attached to this port.
00: High speed
01: Full speed
10: Low speed
11: Reserved */

/* Bits [16..13] - Port test control */
#define OTG_HS_PTCTL_MSK (((1 << (16 - 13 + 1)) - 1) << 13)
#define OTG_HS_PTCTL_SET(VAL) (((VAL) << 13) & OTG_HS_PTCTL_MSK)
#define OTG_HS_PTCTL_GET(REG) (((REG) & OTG_HS_PTCTL_MSK) >> 13)
/* The application writes a nonzero value to this field to put the port into a Test mode, and the corresponding pattern is signaled on the port.
0000: Test mode disabled
0001: Test_J mode
0010: Test_K mode
0011: Test_SE0_NAK mode
0100: Test_Packet mode
0101: Test_Force_Enable
Others: Reserved */

/* Bit 12 - Port power */
#define OTG_HS_PPWR (1 << 12)
/* The application uses this field to control power to this port, and the core clears this bit on an overcurrent condition.
0: Power off
1: Power on */

/* Bits [11..10] - Port line status */
#define OTG_HS_PLSTS_MSK (((1 << (11 - 10 + 1)) - 1) << 10)
#define OTG_HS_PLSTS_SET(VAL) (((VAL) << 10) & OTG_HS_PLSTS_MSK)
#define OTG_HS_PLSTS_GET(REG) (((REG) & OTG_HS_PLSTS_MSK) >> 10)
/* Indicates the current logic level USB data lines
Bit [10]: Logic level of OTG_HS_FS_DP
Bit [11]: Logic level of OTG_HS_FS_DM */

/* Bit 9 Reserved, must be kept at reset value. */


/* Bit 8 - Port reset */
#define OTG_HS_PRST (1 << 8)
/* When the application sets this bit, a reset sequence is started on this port. The application must time the reset period and clear this bit after the reset sequence is complete.
0: Port not in reset
1: Port in reset
The application must leave this bit set for a minimum duration of at least 10 ms to start a reset on the port. The application can leave it set for another 10 ms in addition to the required minimum duration, before clearing the bit, even though there is no maximum limit set by the USB standard.
High speed: 50 ms
Full speed/Low speed: 10 ms */

/* Bit 7 - Port suspend */
#define OTG_HS_PSUSP (1 << 7)
/* The application sets this bit to put this port in Suspend mode. The core only stops sending SOFs when this is set. To stop the PHY clock, the application must set the Port clock stop bit, which asserts the suspend input pin of the PHY.
The read value of this bit reflects the current suspend status of the port. This bit is cleared by the core after a remote wakeup signal is detected or the application sets the Port reset bit or Port resume bit in this register or the Resume/remote wakeup detected interrupt bit or Disconnect detected interrupt bit in the Core interrupt register (WKUINT or DISCINT in OTG_HS_GINTSTS, respectively).
0: Port not in Suspend mode
1: Port in Suspend mode
Bit 6
PRES: Port resume
The application sets this bit to drive resume signaling on the port. The core continues to drive the resume signal until the application clears this bit. If the core detects a USB remote wakeup sequence, as indicated by the Port resume/remote wakeup detected interrupt bit of the Core interrupt register (WKUINT bit in OTG_HS_GINTSTS), the core starts driving resume signaling without application intervention and clears this bit when it detects a disconnect condition. The read value of this bit indicates whether the core is currently driving resume signaling.
0: No resume driven
1: Resume driven */

/* Bit 5 - Port overcurrent change */
#define OTG_HS_POCCHNG (1 << 5)
/* The core sets this bit when the status of the Port overcurrent active bit (bit 4) in this register changes. */

/* Bit 4 - Port overcurrent active */
#define OTG_HS_POCA (1 << 4)
/* Indicates the overcurrent condition of the port.
0: No overcurrent condition
1: Overcurrent condition */

/* Bit 3 - Port enable/disable change */
#define OTG_HS_PENCHNG (1 << 3)
/* The core sets this bit when the status of the Port enable bit [2] in this register changes. */

/* Bit 2 - Port enable */
#define OTG_HS_PENA (1 << 2)
/* A port is enabled only by the core after a reset sequence, and is disabled by an overcurrent condition, a disconnect condition, or by the application clearing this bit. The application cannot set this bit by a register write. It can only clear it to disable the port. This bit does not trigger any interrupt to the application.
0: Port disabled
1: Port enabled */

/* Bit 1 - Port connect detected */
#define OTG_HS_PCDET (1 << 1)
/* The core sets this bit when a device connection is detected to trigger an interrupt to the application using the host port interrupt bit in the Core interrupt register (HPRTINT bit in OTG_HS_GINTSTS). The application must write a 1 to this bit to clear the interrupt. */

/* Bit 0 - Port connect status */
#define OTG_HS_PCSTS (1 << 0)
/* 0: No device is attached to the port
1: A device is attached to the port */


/* OTG_HS host channel-x characteristics register */
#define STM32F_OTG_HS_HCCHARX 0x500



/* Bit 31 - Channel enable */
#define OTG_HS_CHENA (1 << 31)
/* This field is set by the application and cleared by the OTG host.
0: Channel disabled
1: Channel enabled */

/* Bit 30 - Channel disable */
#define OTG_HS_CHDIS (1 << 30)
/* The application sets this bit to stop transmitting/receiving data on a channel, even before the transfer for that channel is complete. The application must wait for the Channel disabled interrupt before treating the channel as disabled. */

/* Bit 29 - Odd frame */
#define OTG_HS_ODDFRM (1 << 29)
/* This field is set (reset) by the application to indicate that the OTG host must perform a transfer in an odd frame. This field is applicable for only periodic (isochronous and interrupt) transactions.
0: Even (micro) frame
1: Odd (micro) frame */

/* Bits [28..22] - Device address */
#define __OTG_HS_DAD (((1 << (28 - 22 + 1)) - 1) << 22)
/* This field selects the specific device serving as the data source or sink. */

/* Bits [21..20] - Multi Count (MC) / Error Count (EC) */
#define OTG_HS_MC_MSK (((1 << (21 - 20 + 1)) - 1) << 20)
#define OTG_HS_MC_SET(VAL) (((VAL) << 20) & OTG_HS_MC_MSK)
#define OTG_HS_MC_GET(REG) (((REG) & OTG_HS_MC_MSK) >> 20)
/* – When the split enable bit (SPLITEN) in the host channel-x split control register (OTG_HS_HCSPLTx) is reset (0), this field indicates to the host the number of transactions that must be executed per micro-frame for this periodic endpoint. For nonperiodic transfers, this field specifies the number of packets to be fetched for this channel before the internal DMA engine changes arbitration.
00: Reserved This field yields undefined results
01: 1 transaction
10: 2 transactions to be issued for this endpoint per micro-frame
11: 3 transactions to be issued for this endpoint per micro-frame.
– When the SPLITEN bit is set (1) in OTG_HS_HCSPLTx, this field indicates the number of immediate retries to be performed for a periodic split transaction on transaction errors. This field must be set to at least 01. */

/* Bits [19..18] - Endpoint type */
#define OTG_HS_EPTYP_MSK (((1 << (19 - 18 + 1)) - 1) << 18)
#define OTG_HS_EPTYP_SET(VAL) (((VAL) << 18) & OTG_HS_EPTYP_MSK)
#define OTG_HS_EPTYP_GET(REG) (((REG) & OTG_HS_EPTYP_MSK) >> 18)
#define OTG_HS_EPTYP_CTRL (0)
#define OTG_HS_EPTYP_ISOC (1)
#define OTG_HS_EPTYP_BULK (2)
#define OTG_HS_EPTYP_INT  (3)
/* Indicates the transfer type selected.
00: Control
01: Isochronous
10: Bulk
11: Interrupt */

/* Bit 17 - Low-speed device */
#define OTG_HS_LSDEV (1 << 17)
/* This field is set by the application to indicate that this channel is communicating to a low- speed device. */

/* Bit 16 Reserved, must be kept at reset value. */


/* Bit 15 - Endpoint direction */
#define OTG_HS_EPDIR (1 << 15)
/* Indicates whether the transaction is IN or OUT.
0: OUT
1: IN */

/* Bits [14..11] - Endpoint number */
//#define OTG_HS_EPNUM_MSK (((1 << (14 - 11 + 1)) - 1) << 11)
//#define OTG_HS_EPNUM_SET(VAL) (((VAL) << 11) & OTG_HS_EPNUM_MSK)
//#define OTG_HS_EPNUM_GET(REG) (((REG) & OTG_HS_EPNUM_MSK) >> 11)
#define OTG_HS_HCCHARX_EPNUM_MSK (((1 << (14 - 11 + 1)) - 1) << 11)
#define OTG_HS_HCCHARX_EPNUM_SET(VAL) (((VAL) << 11) & OTG_HS_HCCHARX_EPNUM_MSK)
#define OTG_HS_HCCHARX_EPNUM_GET(REG) (((REG) & OTG_HS_HCCHARX_EPNUM_MSK) >> 11)
/* Indicates the endpoint number on the device serving as the data source or sink. */

/* Bits [10..0] - Maximum packet size */
#define OTG_HS_MPSIZ_MSK (((1 << (10 - 0 + 1)) - 1) << 0)
#define OTG_HS_MPSIZ_SET(VAL) (((VAL) << 0) & OTG_HS_MPSIZ_MSK)
#define OTG_HS_MPSIZ_GET(REG) (((REG) & OTG_HS_MPSIZ_MSK) >> 0)
/* Indicates the maximum packet size of the associated endpoint. */


/* OTG_HS host channel-x split control register */
#define STM32F_OTG_HS_HCSPLTX 0x504



/* Bit 31 - Split enable */
#define OTG_HS_SPLITEN (1 << 31)
/* The application sets this bit to indicate that this channel is enabled to perform split transactions.
Bits 30:17
Reserved, must be kept at reset value. */

/* Bit 16 - Do complete split */
#define OTG_HS_COMPLSPLT (1 << 16)
/* The application sets this bit to request the OTG host to perform a complete split transaction. */

/* Bits [15..14] - Transaction position */
#define OTG_HS_XACTPOS_MSK (((1 << (15 - 14 + 1)) - 1) << 14)
#define OTG_HS_XACTPOS_SET(VAL) (((VAL) << 14) & OTG_HS_XACTPOS_MSK)
#define OTG_HS_XACTPOS_GET(REG) (((REG) & OTG_HS_XACTPOS_MSK) >> 14)
/* This field is used to determine whether to send all, first, middle, or last payloads with each OUT transaction.
11: All. This is the entire data payload of this transaction (which is less than or equal to 188 bytes)
10: Begin. This is the first data payload of this transaction (which is larger than 188 bytes)
00: Mid. This is the middle payload of this transaction (which is larger than 188 bytes)
01: End. This is the last payload of this transaction (which is larger than 188 bytes) */

/* Bits [13..7] - Hub address */
#define OTG_HS_HUBADDR_MSK (((1 << (13 - 7 + 1)) - 1) << 7)
#define OTG_HS_HUBADDR_SET(VAL) (((VAL) << 7) & OTG_HS_HUBADDR_MSK)
#define OTG_HS_HUBADDR_GET(REG) (((REG) & OTG_HS_HUBADDR_MSK) >> 7)
/* This field holds the device address of the transaction translator’s hub. */

/* Bits [6..0] - Port address */
#define OTG_HS_PRTADDR_MSK (((1 << (6 - 0 + 1)) - 1) << 0)
#define OTG_HS_PRTADDR_SET(VAL) (((VAL) << 0) & OTG_HS_PRTADDR_MSK)
#define OTG_HS_PRTADDR_GET(REG) (((REG) & OTG_HS_PRTADDR_MSK) >> 0)
/* This field is the port number of the recipient transaction translator. */


/* OTG_HS host channel-x interrupt register */
#define STM32F_OTG_HS_HCINTX 0x508

/* This register indicates the status of a channel with respect to USB- and AHB-related events. It is shown in Figure 412. The application must read this register when the host channels interrupt bit in the Core interrupt register (HCINT bit in OTG_HS_GINTSTS) is set. Before the application can read this register, it must first read the host all channels interrupt (OTG_HS_HAINT) register to get the exact channel number for the host channel-x interrupt register. The application must clear the appropriate bit in this register to clear the corresponding bits in the OTG_HS_HAINT and OTG_HS_GINTSTS registers. */

/* [31..11] Reserved, must be kept at reset value. */


/* Bit 10 - Data toggle error */
#define OTG_HS_DTERR (1 << 10)

/* Bit 9 - Frame overrun */
#define OTG_HS_FRMOR (1 << 9)

/* Bit 8 - Babble error */
#define OTG_HS_BBERR (1 << 8)

/* Bit 7 - Transaction error */
#define OTG_HS_TXERR (1 << 7)
/* Indicates one of the following errors occurred on the USB.
- CRC check failure
- Timeout
- Bit stuff error
- False EOP */

/* Bit 6 - Response received interrupt */
//#define OTG_HS_NYET (1 << 6)
#define OTG_HS_HNYET (1 << 6)

/* Bit 5 - ACK response received/transmitted interrupt */
#define OTG_HS_ACK (1 << 5)

/* Bit 4 - NAK response received interrupt */
//#define OTG_HS_NAK (1 << 4)
#define OTG_HS_HNAK (1 << 4)

/* Bit 3 - STALL response received interrupt */
//#define OTG_HS_STALL (1 << 3)
#define OTG_HS_HSTALL (1 << 3)

/* Bit 2 - AHB error */
#define OTG_HS_AHBERR (1 << 2)
/* This error is generated only in Internal DMA mode when an AHB error occurs during an AHB read/write operation. The application can read the corresponding DMA channel address register to get the error address. */

/* Bit 1 - Channel halted */
#define OTG_HS_CHH (1 << 1)
/* Indicates the transfer completed abnormally either because of any USB transaction error or in response to disable request by the application. */

/* Bit 0 - Transfer completed */
#define OTG_HS_XFRC (1 << 0)
/* Transfer completed normally without any errors. */


/* OTG_HS host channel-x interrupt mask register */
#define STM32F_OTG_HS_HCINTMSKX 0x50C

/* This register reflects the mask for each channel status described in the previous section. */

/* [31..11] Reserved, must be kept at reset value. */


/* Bit 10 - Data toggle error mask */
#define OTG_HS_DTERRM (1 << 10)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 9 - Frame overrun mask */
#define OTG_HS_FRMORM (1 << 9)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 8 - Babble error mask */
#define OTG_HS_BBERRM (1 << 8)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 7 - Transaction error mask */
#define OTG_HS_TXERRM (1 << 7)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 6 - response received interrupt mask */
//#define OTG_HS_NYET (1 << 6)
#define OTG_HS_HNYET (1 << 6)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 5 - ACK response received/transmitted interrupt mask */
#define OTG_HS_ACKM (1 << 5)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 4 - NAK response received interrupt mask */
#define OTG_HS_HNAKM (1 << 4)
//#define OTG_HS_NAKM (1 << 4)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 3 - STALL response received interrupt mask */
#define OTG_HS_STALLM (1 << 3)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 2 - AHB error */
#define OTG_HS_AHBERR (1 << 2)
/* This is generated only in Internal DMA mode when there is an AHB error during AHB read/write. The application can read the corresponding channel’s DMA address register to get the error address. */

/* Bit 1 - Channel halted mask */
#define OTG_HS_CHHM (1 << 1)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 0 - Transfer completed mask */
#define OTG_HS_XFRCM (1 << 0)
/* 0: Masked interrupt
1: Unmasked interrupt */


/* OTG_HS host channel-x transfer size register */
#define STM32F_OTG_HS_HCTSIZX 0x510



/* Bit 31 - Doping */
#define OTG_HS_DOPING (1 << 31)
/* This bit is used only for OUT transfers. Setting this field to 1 directs the host to do PING protocol.
Note: Do not set this bit for IN transfers. If this bit is set for IN transfers it disables the channel. */

/* Bits [30..29] - Data PID */
//#define OTG_HS_DPID_MSK (((1 << (30 - 29 + 1)) - 1) << 29)
//#define OTG_HS_DPID_SET(VAL) (((VAL) << 29) & OTG_HS_DPID_MSK)
//#define OTG_HS_DPID_GET(REG) (((REG) & OTG_HS_DPID_MSK) >> 29)
#define OTG_HS_HDPID_MSK (((1 << (30 - 29 + 1)) - 1) << 29)
#define OTG_HS_HDPID_SET(VAL) (((VAL) << 29) & OTG_HS_HDPID_MSK)
#define OTG_HS_HDPID_GET(REG) (((REG) & OTG_HS_DPID_MSK) >> 29)
/* The application programs this field with the type of PID to use for the initial transaction. The host maintains this field for the rest of the transfer.
00: DATA0
01: DATA2
10: DATA1
11: MDATA (noncontrol)/SETUP (control) */

/* Bits [28..19] - Packet count */
#define OTG_HS_PKTCNT_MSK (((1 << (28 - 19 + 1)) - 1) << 19)
#define OTG_HS_PKTCNT_SET(VAL) (((VAL) << 19) & OTG_HS_PKTCNT_MSK)
#define OTG_HS_PKTCNT_GET(REG) (((REG) & OTG_HS_PKTCNT_MSK) >> 19)
/* This field is programmed by the application with the expected number of packets to be transmitted (OUT) or received (IN).
The host decrements this count on every successful transmission or reception of an OUT/IN packet. Once this count reaches zero, the application is interrupted to indicate normal completion. */

/* Bits [18..0] - Transfer size */
#define OTG_HS_XFRSIZ_MSK (((1 << (18 - 0 + 1)) - 1) << 0)
#define OTG_HS_XFRSIZ_SET(VAL) (((VAL) << 0) & OTG_HS_XFRSIZ_MSK)
#define OTG_HS_XFRSIZ_GET(REG) (((REG) & OTG_HS_XFRSIZ_MSK) >> 0)
/* For an OUT, this field is the number of data bytes the host sends during the transfer.
For an IN, this field is the buffer size that the application has reserved for the transfer. The application is expected to program this field as an integer multiple of the maximum packet size for IN transactions (periodic and nonperiodic). */


/* OTG_HS host channel-x DMA address register */
#define STM32F_OTG_HS_HCDMAX 0x514



/* Bits [31..0] - DMA address */
#define OTG_HS_DMAADDR_MSK (((1 << (31 - 0 + 1)) - 1) << 0)
#define OTG_HS_DMAADDR_SET(VAL) (((VAL) << 0) & OTG_HS_DMAADDR_MSK)
#define OTG_HS_DMAADDR_GET(REG) (((REG) & OTG_HS_DMAADDR_MSK) >> 0)
/* This field holds the start address in the external memory from which the data for the endpoint must be fetched or to which it must be stored. This register is incremented on every AHB transaction. */





/* Device-mode registers */

/* OTG_HS device configuration register */
#define STM32F_OTG_HS_DCFG 0x800
/* This register configures the core in peripheral mode after power-on or after certain control commands or enumeration. Do not make changes to this register after initial programming. */

/* [31..26] Reserved, must be kept at reset value. */


/* Bits [25..24] - Periodic scheduling interval */
#define OTG_HS_PERSCHIVL_MSK (((1 << (25 - 24 + 1)) - 1) << 24)
#define OTG_HS_PERSCHIVL_SET(VAL) (((VAL) << 24) & OTG_HS_PERSCHIVL_MSK)
#define OTG_HS_PERSCHIVL_GET(REG) (((REG) & OTG_HS_PERSCHIVL_MSK) >> 24)
/* This field specifies the amount of time the Internal DMA engine must allocate for fetching periodic IN endpoint data. Based on the number of periodic endpoints, this value must be specified as 25, 50 or 75% of the (micro)frame.
– When any periodic endpoints are active, the internal DMA engine allocates the specified amount of time in fetching periodic IN endpoint data
– When no periodic endpoint is active, then the internal DMA engine services nonperiodic endpoints, ignoring this field
– After the specified time within a (micro)frame, the DMA switches to fetching nonperiodic endpoints
00: 25% of (micro)frame
01: 50% of (micro)frame
10: 75% of (micro)frame
11: Reserved */

/* [23..13] Reserved, must be kept at reset value. */


/* Bits [12..11] - Periodic (micro)frame interval */
#define OTG_HS_PFIVL_MSK (((1 << (12 - 11 + 1)) - 1) << 11)
#define OTG_HS_PFIVL_SET(VAL) (((VAL) << 11) & OTG_HS_PFIVL_MSK)
#define OTG_HS_PFIVL_GET(REG) (((REG) & OTG_HS_PFIVL_MSK) >> 11)
#define OTG_HS_PFIVL_80 (0 << 11)
#define OTG_HS_PFIVL_85 (1 << 11)
#define OTG_HS_PFIVL_90 (2 << 11)
#define OTG_HS_PFIVL_95 (3 << 11)
/* Indicates the time within a (micro) frame at which the application must be notified using the end of periodic (micro) frame interrupt. This can be used to determine if all the isochronous traffic for that frame is complete.
00: 80% of the frame interval
01: 85% of the frame interval
10: 90% of the frame interval
11: 95% of the frame interval */

/* Bits [10..4] - Device address */
#define OTG_HS_DAD_MSK (((1 << (10 - 4 + 1)) - 1) << 4)
#define OTG_HS_DAD_SET(VAL) (((VAL) << 4) & OTG_HS_DAD_MSK)
#define OTG_HS_DAD_GET(REG) (((REG) & OTG_HS_DAD_MSK) >> 4)
/* The application must program this field after every SetAddress control command. */

/* Bit 3 Reserved, must be kept at reset value. */


/* Bit 2 - Nonzero-length status OUT handshake */
#define OTG_HS_NZLSOHSK (1 << 2)
/* The application can use this field to select the handshake the core sends on receiving a nonzero-length data packet during the OUT transaction of a control transfer’s Status stage. 1: Send a STALL handshake on a nonzero-length status OUT transaction and do not send
the received OUT packet to the application.
0: Send the received OUT packet to the application (zero-length or nonzero-length) and send a handshake based on the NAK and STALL bits for the endpoint in the device endpoint control register. */

/* Bits [1..0] - Device speed */
#define OTG_HS_DSPD_MSK (((1 << (1 - 0 + 1)) - 1) << 0)
#define OTG_HS_DSPD_SET(VAL) (((VAL) << 0) & OTG_HS_DSPD_MSK)
#define OTG_HS_DSPD_GET(REG) (((REG) & OTG_HS_DSPD_MSK) >> 0)
#define OTG_HS_DSPD_HIGH     (0x0 << 0)
#define OTG_HS_DSPD_FULL_EXT (0x1 << 0)
#define OTG_HS_DSPD_FULL_INT (0x3 << 0)
#define OTG_HS_DSPD_FULL     OTG_HS_DSPD_FULL_INT 

/* Indicates the speed at which the application requires the core to enumerate, or the maximum speed the application can support. However, the actual bus speed is determined only after the chirp sequence is completed, and is based on the speed of the USB host to which the core is connected.
00: High speed
01: Full speed using external ULPI PHY
10: Reserved
11: Full speed using internal embedded PHY */


/* OTG_HS device control register */
#define STM32F_OTG_HS_DCTL 0x804



/* [31..12] Reserved, must be kept at reset value. */


/* Bit 11 - Power-on programming done */
#define OTG_HS_POPRGDNE (1 << 11)
/* The application uses this bit to indicate that register programming is completed after a wakeup from power down mode. */

/* Bit 10 - Clear global OUT NAK */
#define OTG_HS_CGONAK (1 << 10)
/* A write to this field clears the Global OUT NAK. */

/* Bit 9 - Set global OUT NAK */
#define OTG_HS_SGONAK (1 << 9)
/* A write to this field sets the Global OUT NAK.
The application uses this bit to send a NAK handshake on all OUT endpoints.
The application must set the this bit only after making sure that the Global OUT NAK effective bit in the Core interrupt register (GONAKEFF bit in OTG_HS_GINTSTS) is cleared. */

/* Bit 8 - Clear global IN NAK */
#define OTG_HS_CGINAK (1 << 8)
/* A write to this field clears the Global IN NAK. */

/* Bit 7 - Set global IN NAK */
#define OTG_HS_SGINAK (1 << 7)
/* A write to this field sets the Global nonperiodic IN NAK.The application uses this bit to send a NAK handshake on all nonperiodic IN endpoints.
The application must set this bit only after making sure that the Global IN NAK effective bit in the Core interrupt register (GINAKEFF bit in OTG_HS_GINTSTS) is cleared. */

/* Bits [6..4] - Test control */
#define OTG_HS_TCTL_MSK (((1 << (6 - 4 + 1)) - 1) << 4)
#define OTG_HS_TCTL_SET(VAL) (((VAL) << 4) & OTG_HS_TCTL_MSK)
#define OTG_HS_TCTL_GET(REG) (((REG) & OTG_HS_TCTL_MSK) >> 4)
/* 000: Test mode disabled
001: Test_J mode
010: Test_K mode
011: Test_SE0_NAK mode
100: Test_Packet mode
101: Test_Force_Enable
Others: Reserved */

/* Bit 3 - Global OUT NAK status */
#define OTG_HS_GONSTS (1 << 3)
/* 0: A handshake is sent based on the FIFO Status and the NAK and STALL bit settings.
1: No data is written to the RxFIFO, irrespective of space availability. Sends a NAK handshake on all packets, except on SETUP transactions. All isochronous OUT packets are dropped. */

/* Bit 2 - Global IN NAK status */
#define OTG_HS_GINSTS (1 << 2)
/* 0: A handshake is sent out based on the data availability in the transmit FIFO.
1: A NAK handshake is sent out on all nonperiodic IN endpoints, irrespective of the data availability in the transmit FIFO. */

/* Bit 1 - Soft disconnect */
#define OTG_HS_SDIS (1 << 1)
/* The application uses this bit to signal the USB OTG core to perform a soft disconnect. As long as this bit is set, the host does not see that the device is connected, and the device does not receive signals on the USB. The core stays in the disconnected state until the application clears this bit.
0: Normal operation. When this bit is cleared after a soft disconnect, the core generates a device connect event to the USB host. When the device is reconnected, the USB host restarts device enumeration.
1: The core generates a device disconnect event to the USB host. */

/* Bit 0 - Remote wakeup signaling */
#define OTG_HS_RWUSIG (1 << 0)
/* When the application sets this bit, the core initiates remote signaling to wake up the USB host. The application must set this bit to instruct the core to exit the Suspend state. As specified in the USB 2.0 specification, the application must clear this bit 1 ms to 15 ms after
setting it.
Table 209 contains the minimum duration (according to device state) for which the Soft disconnect (SDIS) bit must be set for the USB host to detect a device disconnect. To accommodate clock jitter, it is recommended that the application add some extra delay to the specified minimum duration.
Table 209. Minimum duration for soft disconnect
Operating speed Device state Minimum duration
High speed Not Idle or Suspended (Performing transactions) 125 μs
Full speed Suspended 1 ms + 2.5 μs
Full speed Idle 2.5 μs
Full speed Not Idle or Suspended (Performing transactions) 2.5 μs */


/* OTG_HS device status register */
#define STM32F_OTG_HS_DSTS 0x808
/* This register indicates the status of the core with respect to USB-related events. It must be read on interrupts from the device all interrupts (OTG_HS_DAINT) register. */

/* [31..22] Reserved, must be kept at reset value. */


/* Bits [21..8] - Frame number of the received SOF */
#define OTG_HS_FNSOF_MSK (((1 << (21 - 8 + 1)) - 1) << 8)
#define OTG_HS_FNSOF_SET(VAL) (((VAL) << 8) & OTG_HS_FNSOF_MSK)
#define OTG_HS_FNSOF_GET(REG) (((REG) & OTG_HS_FNSOF_MSK) >> 8)

/* [7..4] Reserved, must be kept at reset value. */


/* Bit 3 - Erratic error */
#define OTG_HS_EERR (1 << 3)
/* The core sets this bit to report any erratic errors.
Due to erratic errors, the OTG_HS controller goes into Suspended state and an interrupt is generated to the application with Early suspend bit of the Core interrupt register (ESUSP bit in OTG_HS_GINTSTS). If the early suspend is asserted due to an erratic error, the application can only perform a soft disconnect recover. */

/* Bits [2..1] - Enumerated speed */
#define OTG_HS_ENUMSPD_MSK (((1 << (2 - 1 + 1)) - 1) << 1)
#define OTG_HS_ENUMSPD_SET(VAL) (((VAL) << 1) & OTG_HS_ENUMSPD_MSK)
#define OTG_HS_ENUMSPD_GET(REG) (((REG) & OTG_HS_ENUMSPD_MSK) >> 1)
/* Indicates the speed at which the OTG_HS controller has come up after speed detection through a chirp sequence.
00: High speed
01: Reserved
10: Reserved
11: Full speed (PHY clock is running at 48 MHz)
Others: reserved */

/* Bit 0 - Suspend status */
#define OTG_HS_SUSPSTS (1 << 0)
/* In peripheral mode, this bit is set as long as a Suspend condition is detected on the USB. 
The core enters the Suspended state when there is no activity on the USB data lines for a period of 3 ms. The core comes out of the suspend:
– When there is an activity on the USB data lines
– When the application writes to the Remote wakeup signaling bit in the Device control register (RWUSIG bit in OTG_HS_DCTL). */


/* OTG_HS device IN endpoint common interrupt mask register */
#define STM32F_OTG_HS_DIEPMSK 0x810
/* This register works with each of the Device IN endpoint interrupt (OTG_HS_D    IEPINTx) registers for all endpoints to generate an interrupt per IN endpoint. The IN endpoint interrupt for a specific status in the OTG_HS_DIEPINTx register can be masked by writing to the corresponding bit in this register. Status bits are masked by default. */

/* [31..10] Reserved, must be kept at reset value. */


/* Bit 9 - BNA interrupt mask */
#define OTG_HS_BIM (1 << 9)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 8 - FIFO underrun mask */
#define OTG_HS_TXFURM (1 << 8)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 7 Reserved, must be kept at reset value. */


/* Bit 6 - IN endpoint NAK effective mask */
#define OTG_HS_INEPNEM (1 << 6)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 5 - IN token received with EP mismatch mask */
#define OTG_HS_INEPNMM (1 << 5)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 4 - IN token received when TxFIFO empty mask */
#define OTG_HS_ITTXFEMSK (1 << 4)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 3 - Timeout condition mask (nonisochronous endpoints) */
#define OTG_HS_TOM (1 << 3)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 2 Reserved, must be kept at reset value. */


/* Bit 1 - Endpoint disabled interrupt mask */
#define OTG_HS_EPDM (1 << 1)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 0 - Transfer completed interrupt mask */
#define OTG_HS_XFRCM (1 << 0)
/* 0: Masked interrupt
1: Unmasked interrupt */


/* OTG_HS device OUT endpoint common interrupt mask register */
#define STM32F_OTG_HS_DOEPMSK 0x814
/* This register works with each of the Device OUT endpoint interrupt (OTG_HS_DOEPINTx) registers for all endpoints to generate an interrupt per OUT endpoint. The OUT endpoint interrupt for a specific status in the OTG_HS_DOEPINTx register can be masked by writing into the corresponding bit in this register. Status bits are masked by default. */

/* [31..10] Reserved, must be kept at reset value. */


/* Bit 9 - BNA interrupt mask */
#define OTG_HS_BOIM (1 << 9)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 8 - OUT packet error mask */
#define OTG_HS_OPEM (1 << 8)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 7 Reserved, must be kept at reset value. */


/* Bit 6 - Back-to-back SETUP packets received mask */
#define OTG_HS_B2BSTUP (1 << 6)
/* Applies to control OUT endpoints only.
0: Masked interrupt
1: Unmasked interrupt */

/* Bit 5 Reserved, must be kept at reset value. */


/* Bit 4 - OUT token received when endpoint disabled mask */
#define OTG_HS_OTEPDM (1 << 4)
/* Applies to control OUT endpoints only.
0: Masked interrupt
1: Unmasked interrupt */

/* Bit 3 - SETUP phase done mask */
#define OTG_HS_STUPM (1 << 3)
/* Applies to control endpoints only.
0: Masked interrupt
1: Unmasked interrupt */

/* Bit 2 Reserved, must be kept at reset value. */


/* Bit 1 - Endpoint disabled interrupt mask */
#define OTG_HS_EPDM (1 << 1)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 0 - Transfer completed interrupt mask */
#define OTG_HS_XFRCM (1 << 0)
/* 0: Masked interrupt
1: Unmasked interrupt */


/* OTG_HS device all endpoints interrupt register */
#define STM32F_OTG_HS_DAINT 0x818
/* When a significant event occurs on an endpoint, a device all endpoints interrupt register interrupts the application using the Device OUT endpoints interrupt bit or Device IN endpoints interrupt bit of the Core interrupt register (OEPINT or IEPINT in OTG_HS_GINTSTS, respectively). There is one interrupt bit per endpoint, up to a maximum of 16 bits for OUT endpoints and 16 bits for IN endpoints. For a bidirectional endpoint, the corresponding IN and OUT interrupt bits are used. Bits in this register are set and cleared when the application sets and clears bits in the corresponding Device Endpoint-x interrupt
register (OTG_HS_DIEPINTx/OTG_HS_DOEPINTx). */

/* Bits [31..16] - OUT endpoint interrupt bits */
#define OTG_HS_OEPINT_MASK_MSK (((1 << (31 - 16 + 1)) - 1) << 16)
#define OTG_HS_OEPINT_MASK_SET(VAL) (((VAL) << 16) & OTG_HS_OEPINT_MASK_MSK)
#define OTG_HS_OEPINT_MASK_GET(REG) (((REG) & OTG_HS_OEPINT_MASK_MSK) >> 16)
#define OTG_HS_OEPINT0  (1 << 16)
#define OTG_HS_OEPINT1  (1 << 17)
#define OTG_HS_OEPINT2  (1 << 18)
#define OTG_HS_OEPINT3  (1 << 19)
#define OTG_HS_OEPINT4  (1 << 20)
#define OTG_HS_OEPINT5  (1 << 21)
#define OTG_HS_OEPINT6  (1 << 22)
#define OTG_HS_OEPINT7  (1 << 23)
#define OTG_HS_OEPINT8  (1 << 24)
#define OTG_HS_OEPINT9  (1 << 25)
#define OTG_HS_OEPINT10 (1 << 26)
#define OTG_HS_OEPINT11 (1 << 27)
#define OTG_HS_OEPINT12 (1 << 28)
#define OTG_HS_OEPINT13 (1 << 29)
#define OTG_HS_OEPINT14 (1 << 30)
#define OTG_HS_OEPINT15 (1 << 31)
/* One bit per OUT endpoint:
Bit 16 for OUT endpoint 0, bit 31 for OUT endpoint 15 */

/* Bits [15..0] - IN endpoint interrupt bits */
#define OTG_HS_IEPINT_MASK_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_IEPINT_MASK_SET(VAL) (((VAL) << 0) & OTG_HS_IEPINT_MASK_MSK)
#define OTG_HS_IEPINT_MASK_GET(REG) (((REG) & OTG_HS_IEPINT_MASK_MSK) >> 0)
#define OTG_HS_IEPINT0  (1 << 0)
#define OTG_HS_IEPINT1  (1 << 1)
#define OTG_HS_IEPINT2  (1 << 2)
#define OTG_HS_IEPINT3  (1 << 3)
#define OTG_HS_IEPINT4  (1 << 4)
#define OTG_HS_IEPINT5  (1 << 5)
#define OTG_HS_IEPINT6  (1 << 6)
#define OTG_HS_IEPINT7  (1 << 7)
#define OTG_HS_IEPINT8  (1 << 8)
#define OTG_HS_IEPINT9  (1 << 9)
#define OTG_HS_IEPINT10 (1 << 10)
#define OTG_HS_IEPINT11 (1 << 11)
#define OTG_HS_IEPINT12 (1 << 12)
#define OTG_HS_IEPINT13 (1 << 13)
#define OTG_HS_IEPINT14 (1 << 14)
#define OTG_HS_IEPINT15 (1 << 15)
/* One bit per IN endpoint:
Bit 0 for IN endpoint 0, bit 15 for endpoint 15 */


/* OTG_HS all endpoints interrupt mask register */
#define STM32F_OTG_HS_DAINTMSK 0x81C
/* The device endpoint interrupt mask register works with the device endpoint interrupt register to interrupt the application when an event occurs on a device endpoint. However, the device all endpoints interrupt (OTG_HS_DAINT) register bit corresponding to that interrupt is still set. */

/* Bits [31..16] - OUT EP interrupt mask bits */
#define OTG_HS_OEPM_MSK (((1 << (31 - 16 + 1)) - 1) << 16)
#define OTG_HS_OEPM_SET(VAL) (((VAL) << 16) & OTG_HS_OEPM_MSK)
#define OTG_HS_OEPM_GET(REG) (((REG) & OTG_HS_OEPM_MSK) >> 16)
#define OTG_HS_OEPM(N) (1 << (16 + (N)))
#define OTG_HS_OEPM0  (1 << 16)
#define OTG_HS_OEPM1  (1 << 17)
#define OTG_HS_OEPM2  (1 << 18)
#define OTG_HS_OEPM3  (1 << 19)
#define OTG_HS_OEPM4  (1 << 20)
#define OTG_HS_OEPM5  (1 << 21)
#define OTG_HS_OEPM6  (1 << 22)
#define OTG_HS_OEPM7  (1 << 23)
#define OTG_HS_OEPM8  (1 << 24)
#define OTG_HS_OEPM9  (1 << 25)
#define OTG_HS_OEPM10 (1 << 26)
#define OTG_HS_OEPM11 (1 << 27)
#define OTG_HS_OEPM12 (1 << 28)
#define OTG_HS_OEPM13 (1 << 29)
#define OTG_HS_OEPM14 (1 << 30)
#define OTG_HS_OEPM15 (1 << 31)
/* One per OUT endpoint:
Bit 16 for OUT EP 0, bit 18 for OUT EP 3
0: Masked interrupt
1: Unmasked interrupt */

/* Bits [15..0] - IN EP interrupt mask bits */
#define OTG_HS_IEPM_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_IEPM_SET(VAL) (((VAL) << 0) & OTG_HS_IEPM_MSK)
#define OTG_HS_IEPM_GET(REG) (((REG) & OTG_HS_IEPM_MSK) >> 0)
#define OTG_HS_IEPM(N) (1 << (N))
#define OTG_HS_IEPM0  (1 << 0)
#define OTG_HS_IEPM1  (1 << 1)
#define OTG_HS_IEPM2  (1 << 2)
#define OTG_HS_IEPM3  (1 << 3)
#define OTG_HS_IEPM4  (1 << 4)
#define OTG_HS_IEPM5  (1 << 5)
#define OTG_HS_IEPM6  (1 << 6)
#define OTG_HS_IEPM7  (1 << 7)
#define OTG_HS_IEPM8  (1 << 8)
#define OTG_HS_IEPM9  (1 << 9)
#define OTG_HS_IEPM10 (1 << 10)
#define OTG_HS_IEPM11 (1 << 11)
#define OTG_HS_IEPM12 (1 << 12)
#define OTG_HS_IEPM13 (1 << 13)
#define OTG_HS_IEPM14 (1 << 14)
#define OTG_HS_IEPM15 (1 << 15)
/* One bit per IN endpoint:
Bit 0 for IN EP 0, bit 3 for IN EP 3
0: Masked interrupt
1: Unmasked interrupt */


/* OTG_HS device V BUS discharge time register */
#define STM32F_OTG_HS_DVBUSDIS 0x0828
/* This register specifies the V BUS discharge time after V BUS pulsing during SRP. */

/* [31..16] Reserved, must be kept at reset value. */


/* Bits [15..0] - Device V BUS discharge time */
#define OTG_HS_VBUSDT_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_VBUSDT_SET(VAL) (((VAL) << 0) & OTG_HS_VBUSDT_MSK)
#define OTG_HS_VBUSDT_GET(REG) (((REG) & OTG_HS_VBUSDT_MSK) >> 0)
/* Specifies the V BUS discharge time after V BUS pulsing during SRP. This value equals:
V BUS discharge time in PHY clocks / 1 024
Depending on your V BUS load, this value may need adjusting. */


/* OTG_HS device V BUS pulsing time register */
#define STM32F_OTG_HS_DVBUSPULSE 0x082C
/* This register specifies the V BUS pulsing time during SRP. */

/* [31..12] Reserved, must be kept at reset value. */


/* Bits [11..0] - Device V BUS pulsing time */
#define OTG_HS_DVBUSP_MSK (((1 << (11 - 0 + 1)) - 1) << 0)
#define OTG_HS_DVBUSP_SET(VAL) (((VAL) << 0) & OTG_HS_DVBUSP_MSK)
#define OTG_HS_DVBUSP_GET(REG) (((REG) & OTG_HS_DVBUSP_MSK) >> 0)
/* Specifies the V BUS pulsing time during SRP. This value equals:
V BUS pulsing time in PHY clocks / 1 024 */


/* OTG_HS Device threshold control register */
#define STM32F_OTG_HS_DTHRCTL 0x0830



/* [31..28] Reserved, must be kept at reset value. */


/* Bit 27 - Arbiter parking enable */
#define OTG_HS_ARPEN (1 << 27)
/* This bit controls internal DMA arbiter parking for IN endpoints. When thresholding is enabled and this bit is set to one, then the arbiter parks on the IN endpoint for which there is a token received on the USB. This is done to avoid getting into underrun conditions. By default parking is enabled.
Bit 26
Reserved, must be kept at reset value.
Bits 25: 17 RXTHRLEN: Receive threshold length
This field specifies the receive thresholding size in DWORDS. This field also specifies the amount of data received on the USB before the core can start transmitting on the AHB. The threshold length has to be at least eight DWORDS. The recommended value for RXTHRLEN is to be the same as the programmed AHB burst length (HBSTLEN bit in OTG_HS_GAHBCFG). */

/* Bit 16 - Receive threshold enable */
#define OTG_HS_RXTHREN (1 << 16)
/* When this bit is set, the core enables thresholding in the receive direction.
Bits 15: 11
Reserved, must be kept at reset value. */

/* Bits [10..2] - Transmit threshold length */
#define OTG_HS_TXTHRLEN_MSK (((1 << (10 - 2 + 1)) - 1) << 2)
#define OTG_HS_TXTHRLEN_SET(VAL) (((VAL) << 2) & OTG_HS_TXTHRLEN_MSK)
#define OTG_HS_TXTHRLEN_GET(REG) (((REG) & OTG_HS_TXTHRLEN_MSK) >> 2)
/* This field specifies the transmit thresholding size in DWORDS. This field specifies the amount of data in bytes to be in the corresponding endpoint transmit FIFO, before the core can start transmitting on the USB. The threshold length has to be at least eight DWORDS. This field controls both isochronous and nonisochronous IN endpoint thresholds. The recommended value for TXTHRLEN is to be the same as the programmed AHB burst length (HBSTLEN bit in OTG_HS_GAHBCFG). */

/* Bit 1 - ISO IN endpoint threshold enable */
#define OTG_HS_ISOTHREN (1 << 1)
/* When this bit is set, the core enables thresholding for isochronous IN endpoints. */

/* Bit 0 - Nonisochronous IN endpoints threshold enable */
#define OTG_HS_NONISOTHREN (1 << 0)
/* When this bit is set, the core enables thresholding for nonisochronous IN endpoints. */


/* OTG_HS device IN endpoint FIFO empty interrupt mask register: */
#define STM32F_OTG_HS_DIEPEMPMSK 0x834

/* This register is used to control the IN endpoint FIFO empty interrupt generation (TXFE_OTG_HS_DIEPINTx). */

/* [31..16] Reserved, must be kept at reset value. */


/* Bits [15..0] - IN EP Tx FIFO empty interrupt mask bits */
#define OTG_HS_INEPTXFEM_MSK (((1 << (15 - 0 + 1)) - 1) << 0)
#define OTG_HS_INEPTXFEM_SET(VAL) (((VAL) << 0) & OTG_HS_INEPTXFEM_MSK)
#define OTG_HS_INEPTXFEM_GET(REG) (((REG) & OTG_HS_INEPTXFEM_MSK) >> 0)
/* These bits act as mask bits for OTG_HS_DIEPINTx.
TXFE interrupt one bit per IN endpoint:
Bit 0 for IN endpoint 0, bit 15 for IN endpoint 15
0: Masked interrupt
1: Unmasked interrupt */


/* OTG_HS device each endpoint interrupt register */
#define STM32F_OTG_HS_DEACHINT 0x0838

/* There is one interrupt bit for endpoint 1 IN and one interrupt bit for endpoint 1 OUT. */

/* [31..18] Reserved, must be kept at reset value. */

/* Bit 17 - OUT endpoint 1 interrupt bit */
#define OTG_HS_OEP1INT (1 << 17)

/* [16..2] Reserved, must be kept at reset value. */


/* Bit 1 - IN endpoint 1interrupt bit */
#define OTG_HS_IEP1INT (1 << 1)

/* OTG_HS device each endpoint interrupt register mask */
#define STM32F_OTG_HS_DEACHINTMSK 0x083C

/* There is one interrupt bit for endpoint 1 IN and one interrupt bit for endpoint 1 OUT. */

/* [31..18] Reserved, must be kept at reset value. */


/* Bit 17 - OUT Endpoint 1 interrupt mask bit */
#define OTG_HS_OEP1INTM (1 << 17)
/* Bits 16:2
Reserved, must be kept at reset value. */

/* Bit 1 - IN Endpoint 1 interrupt mask bit */
#define OTG_HS_IEP1INTM (1 << 1)
/* Bit 0
Reserved, must be kept at reset value. */


/* OTG_HS device each in endpoint-1 interrupt register */
#define STM32F_OTG_HS_DIEPEACHMSK1 0x844



/* [31..14] Reserved, must be kept at reset value. */


/* Bit 13 - NAK interrupt mask */
#define OTG_HS_NAKM (1 << 13)
/* 0: Masked interrupt
1: unmasked interrupt */

/* [12..10] Reserved, must be kept at reset value. */


/* Bit 9 - BNA interrupt mask */
#define OTG_HS_BIM (1 << 9)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 8 - FIFO underrun mask */
#define OTG_HS_TXFURM (1 << 8)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 7 Reserved, must be kept at reset value. */


/* Bit 6 - IN endpoint NAK effective mask */
#define OTG_HS_INEPNEM (1 << 6)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 5 - IN token received with EP mismatch mask */
#define OTG_HS_INEPNMM (1 << 5)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 4 - IN token received when TxFIFO empty mask */
#define OTG_HS_ITTXFEMSK (1 << 4)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 3 - Timeout condition mask (nonisochronous endpoints) */
#define OTG_HS_TOM (1 << 3)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 2 Reserved, must be kept at reset value. */


/* Bit 1 - Endpoint disabled interrupt mask */
#define OTG_HS_EPDM (1 << 1)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 0 - Transfer completed interrupt mask */
#define OTG_HS_XFRCM (1 << 0)
/* 0: Masked interrupt
1: Unmasked interrupt */


/* OTG_HS device each OUT endpoint-1 interrupt register */
#define STM32F_OTG_HS_DOEPEACHMSK1 0x884



/* [31..15] Reserved, must be kept at reset value. */


/* Bit 14 - NYET interrupt mask */
#define OTG_HS_NYETM (1 << 14)
/* 0: Masked interrupt
1: unmasked interrupt */

/* Bit 13 - NAK interrupt mask */
#define OTG_HS_NAKM (1 << 13)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 12 - Bubble error interrupt mask */
#define OTG_HS_BERRM (1 << 12)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* [11..10] Reserved, must be kept at reset value. */


/* Bit 9 - BNA interrupt mask */
#define OTG_HS_BIM (1 << 9)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 8 - OUT packet error mask */
#define OTG_HS_OPEM (1 << 8)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* [7..3] Reserved, must be kept at reset value. */


/* Bit 2 - AHB error mask */
#define OTG_HS_AHBERRM (1 << 2)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 1 - Endpoint disabled interrupt mask */
#define OTG_HS_EPDM (1 << 1)
/* 0: Masked interrupt
1: Unmasked interrupt */

/* Bit 0 - Transfer completed interrupt mask */
#define OTG_HS_XFRCM (1 << 0)
/* 0: Masked interrupt
1: Unmasked interrupt */


/* OTG device endpoint-x control register */
#define STM32F_OTG_HS_DIEPCTLX 0x900

/* The application uses this register to control the behavior of each logical endpoint other than endpoint 0. */

/* Bit 31 - Endpoint enable */
#define OTG_HS_EPENA (1 << 31)
/* The application sets this bit to start transmitting data on an endpoint.
The core clears this bit before setting any of the following interrupts on this endpoint:
– SETUP phase done
– Endpoint disabled
– Transfer completed */

/* Bit 30 - Endpoint disable */
#define OTG_HS_EPDIS (1 << 30)
/* The application sets this bit to stop transmitting/receiving data on an endpoint, even before the transfer for that endpoint is complete. The application must wait for the Endpoint disabled interrupt before treating the endpoint as disabled. The core clears this bit before setting the Endpoint disabled interrupt. The application must set this bit only if Endpoint enable is already set for this endpoint. */

/* Bit 29 - Set odd frame */
#define OTG_HS_SODDFRM (1 << 29)
/* Applies to isochronous IN and OUT endpoints only.
Writing to this field sets the Even/Odd frame (EONUM) field to odd frame. */

/* Bit 28 - Set DATA0 PID */
#define OTG_HS_SD0PID (1 << 28)
/* Applies to interrupt/bulk IN endpoints only.
Writing to this field sets the endpoint data PID (DPID) field in this register to DATA0.
SEVNFRM: Set even frame
Applies to isochronous IN endpoints only.
Writing to this field sets the Even/Odd frame (EONUM) field to even frame. */

/* Bit 27  - Set NAK */
#define OTG_HS_SNAK (1 << 27)
/* A write to this bit sets the NAK bit for the endpoint.
Using this bit, the application can control the transmission of NAK handshakes on an endpoint. The core can also set this bit for OUT endpoints on a Transfer completed interrupt, or after a SETUP is received on the endpoint. */

/* Bit 26 - Clear NAK */
#define OTG_HS_CNAK (1 << 26)
/* A write to this bit clears the NAK bit for the endpoint. */

/* Bits [25..22] - TxFIFO number */
#define OTG_HS_TXFNUM_MSK (((1 << (25 - 22 + 1)) - 1) << 22)
#define OTG_HS_TXFNUM_SET(VAL) (((VAL) << 22) & OTG_HS_TXFNUM_MSK)
#define OTG_HS_TXFNUM_GET(REG) (((REG) & OTG_HS_TXFNUM_MSK) >> 22)
/* These bits specify the FIFO number associated with this endpoint. Each active IN endpoint must be programmed to a separate FIFO number.
This field is valid only for IN endpoints. */

/* Bit 21 - STALL handshake */
#define OTG_HS_STALL (1 << 21)
/* Applies to noncontrol, nonisochronous IN endpoints only (access type is rw).
The application sets this bit to stall all tokens from the USB host to this endpoint. If a NAK bit, Global IN NAK, or Global OUT NAK is set along with this bit, the STALL bit takes priority.
Only the application can clear this bit, never the core.
Applies to control endpoints only (access type is rs).
The application can only set this bit, and the core clears it, when a SETUP token is received for this endpoint. If a NAK bit, Global IN NAK, or Global OUT NAK is set along with this bit, the STALL bit takes priority. Irrespective of this bit’s setting, the core always responds to SETUP data packets with an ACK handshake. */

/* Bit 20 Reserved, must be kept at reset value. */


/* Bits [19..18] - Endpoint type */
#define OTG_HS_EPTYP_MSK (((1 << (19 - 18 + 1)) - 1) << 18)
#define OTG_HS_EPTYP_SET(VAL) (((VAL) << 18) & OTG_HS_EPTYP_MSK)
#define OTG_HS_EPTYP_GET(REG) (((REG) & OTG_HS_EPTYP_MSK) >> 18)
/* This is the transfer type supported by this logical endpoint.
00: Control
01: Isochronous
10: Bulk
11: Interrupt */

/* Bit 17 - NAK status */
#define OTG_HS_NAKSTS (1 << 17)
/* It indicates the following:
0: The core is transmitting nonNAK handshakes based on the FIFO status.
1: The core is transmitting NAK handshakes on this endpoint.
When either the application or the core sets this bit:
For nonisochronous IN endpoints: The core stops transmitting any data on an IN endpoint, even if there are data available in the TxFIFO.
For isochronous IN endpoints: The core sends out a zero-length data packet, even if there are data available in the TxFIFO.
Irrespective of this bit’s setting, the core always responds to SETUP data packets with an ACK handshake. */

/* Bit 16 - Even/odd frame */
#define OTG_HS_EONUM (1 << 16)
/* Applies to isochronous IN endpoints only.
Indicates the frame number in which the core transmits/receives isochronous data for this endpoint. The application must program the even/odd frame number in which it intends to transmit/receive isochronous data for this endpoint using the SEVNFRM and SODDFRM fields in this register.
0: Even frame
1: Odd frame
DPID: Endpoint data PID
Applies to interrupt/bulk IN endpoints only.
Contains the PID of the packet to be received or transmitted on this endpoint. The application must program the PID of the first packet to be received or transmitted on this endpoint, after the endpoint is activated. The application uses the SD0PID register field to program either DATA0 or DATA1 PID.
0: DATA0
1: DATA1 */

/* Bit 15 - USB active endpoint */
#define OTG_HS_USBAEP (1 << 15)
/* Indicates whether this endpoint is active in the current configuration and interface. The core clears this bit for all endpoints (other than EP 0) after detecting a USB reset. After receiving the SetConfiguration and SetInterface commands, the application must program endpoint registers accordingly and set this bit. */

/* [14..11] Reserved, must be kept at reset value. */


/* Bits [10..0] - Maximum packet size */
#define OTG_HS_MPSIZ_MSK (((1 << (10 - 0 + 1)) - 1) << 0)
#define OTG_HS_MPSIZ_SET(VAL) (((VAL) << 0) & OTG_HS_MPSIZ_MSK)
#define OTG_HS_MPSIZ_GET(REG) (((REG) & OTG_HS_MPSIZ_MSK) >> 0)
/* The application must program this field with the maximum packet size for the current logical endpoint. This value is in bytes. */


/* OTG_HS device control OUT endpoint 0 control register */
#define STM32F_OTG_HS_DOEPCTL0 0xB00

/* This section describes the device control OUT endpoint 0 control register. Nonzero control endpoints use registers for endpoints 1–15. */

/* Bit 31 - Endpoint enable */
#define OTG_HS_EPENA (1 << 31)
/* The application sets this bit to start transmitting data on endpoint 0.
The core clears this bit before setting any of the following interrupts on this endpoint:
– SETUP phase done
– Endpoint disabled
– Transfer completed */

/* Bit 30 - Endpoint disable */
#define OTG_HS_EPDIS (1 << 30)
/* The application cannot disable control OUT endpoint 0. */

/* [29..28] Reserved, must be kept at reset value. */


/* Bit 27 - Set NAK */
#define OTG_HS_SNAK (1 << 27)
/* A write to this bit sets the NAK bit for the endpoint.
Using this bit, the application can control the transmission of NAK handshakes on an endpoint. The core can also set this bit on a Transfer completed interrupt, or after a SETUP is received on the endpoint. */

/* Bit 26 - Clear NAK */
#define OTG_HS_CNAK (1 << 26)
/* A write to this bit clears the NAK bit for the endpoint. */

/* [25..22] Reserved, must be kept at reset value. */


/* Bit 21 - STALL handshake */
#define OTG_HS_STALL (1 << 21)
/* The application can only set this bit, and the core clears it, when a SETUP token is received for this endpoint. If a NAK bit or Global OUT NAK is set along with this bit, the STALL bit takes priority. Irrespective of this bit’s setting, the core always responds to SETUP data
packets with an ACK handshake. */

/* Bit 20 - Snoop mode */
#define OTG_HS_SNPM (1 << 20)
/* This bit configures the endpoint to Snoop mode. In Snoop mode, the core does not check the correctness of OUT packets before transferring them to application memory. */

/* Bits [19..18] - Endpoint type */
#define OTG_HS_EPTYP_MSK (((1 << (19 - 18 + 1)) - 1) << 18)
#define OTG_HS_EPTYP_SET(VAL) (((VAL) << 18) & OTG_HS_EPTYP_MSK)
#define OTG_HS_EPTYP_GET(REG) (((REG) & OTG_HS_EPTYP_MSK) >> 18)
/* Hardcoded to 2’b00 for control. */

/* Bit 17 - NAK status */
#define OTG_HS_NAKSTS (1 << 17)
/* Indicates the following:
0: The core is transmitting nonNAK handshakes based on the FIFO status.
1: The core is transmitting NAK handshakes on this endpoint.
When either the application or the core sets this bit, the core stops receiving data, even if there is space in the RxFIFO to accommodate the incoming packet. Irrespective of this bit’s setting, the core always responds to SETUP data packets with an ACK handshake. */

/* Bit 16 Reserved, must be kept at reset value. */


/* Bit 15 - USB active endpoint */
#define OTG_HS_USBAEP (1 << 15)
/* This bit is always set to 1, indicating that a control endpoint 0 is always active in all configurations and interfaces. */

/* [14..2] Reserved, must be kept at reset value. */


/* Bits [1..0] - Maximum packet size */
#define OTG_HS_MPSIZ_MSK (((1 << (10 - 0 + 1)) - 1) << 0)
#define OTG_HS_MPSIZ_64 (0 << 0)
#define OTG_HS_MPSIZ_32 (1 << 0)
#define OTG_HS_MPSIZ_16 (2 << 0)
#define OTG_HS_MPSIZ_8  (3 << 0)
/* The maximum packet size for control OUT endpoint 0 is the same as what is programmed in control IN endpoint 0.
00: 64 bytes
01: 32 bytes
10: 16 bytes
11: 8 bytes */

/* OTG_HS device endpoint-x control register (OTG_HS_DOEPCTLx) (x = 1..3, where x = Endpoint_number)
Address offset for OUT endpoints: 0xB00 + (Endpoint_number × 0x20) */
/* The application uses this register to control the behavior of each logical endpoint other than endpoint 0. */

/* Bit 31 - Endpoint enable */
#define OTG_HS_EPENA (1 << 31)
/* Applies to IN and OUT endpoints.
The application sets this bit to start transmitting data on an endpoint.
The core clears this bit before setting any of the following interrupts on this endpoint:
– SETUP phase done
– Endpoint disabled
– Transfer completed */

/* Bit 30 - Endpoint disable */
#define OTG_HS_EPDIS (1 << 30)
/* The application sets this bit to stop transmitting/receiving data on an endpoint, even before the transfer for that endpoint is complete. The application must wait for the Endpoint disabled interrupt before treating the endpoint as disabled. The core clears this bit before setting the Endpoint disabled interrupt. The application must set this bit only if Endpoint enable is already set for this endpoint. */

/* Bit 29 - Set odd frame */
#define OTG_HS_SODDFRM (1 << 29)
/* Applies to isochronous OUT endpoints only.
Writing to this field sets the Even/Odd frame (EONUM) field to odd frame. */

/* Bit 28 - Set DATA0 PID */
#define OTG_HS_SD0PID (1 << 28)
/* Applies to interrupt/bulk OUT endpoints only.
Writing to this field sets the endpoint data PID (DPID) field in this register to DATA0.
SEVNFRM: Set even frame
Applies to isochronous OUT endpoints only.
Writing to this field sets the Even/Odd frame (EONUM) field to even frame. */

/* Bit 27 - Set NAK */
#define OTG_HS_SNAK (1 << 27)
/* A write to this bit sets the NAK bit for the endpoint.
Using this bit, the application can control the transmission of NAK handshakes on an endpoint. The core can also set this bit for OUT endpoints on a Transfer Completed interrupt, or after a SETUP is received on the endpoint. */

/* Bit 26 - Clear NAK */
#define OTG_HS_CNAK (1 << 26)
/* A write to this bit clears the NAK bit for the endpoint. */

/* [25..22] Reserved, must be kept at reset value. */


/* Bit 21 - STALL handshake */
#define OTG_HS_STALL (1 << 21)
/* Applies to noncontrol, nonisochronous OUT endpoints only (access type is rw).
The application sets this bit to stall all tokens from the USB host to this endpoint. If a NAK bit, Global IN NAK, or Global OUT NAK is set along with this bit, the STALL bit takes priority. Only the application can clear this bit, never the core.
Applies to control endpoints only (access type is rs).
The application can only set this bit, and the core clears it, when a SETUP token is received for this endpoint. If a NAK bit, Global IN NAK, or Global OUT NAK is set along with this bit, the STALL bit takes priority. Irrespective of this bit’s setting, the core always responds to SETUP data packets with an ACK handshake. */

/* Bit 20 - Snoop mode */
#define OTG_HS_SNPM (1 << 20)
/* This bit configures the endpoint to Snoop mode. In Snoop mode, the core does not check the correctness of OUT packets before transferring them to application memory. */

/* Bits [19..18] - Endpoint type */
#define OTG_HS_EPTYP_MSK (((1 << (19 - 18 + 1)) - 1) << 18)
#define OTG_HS_EPTYP_SET(VAL) (((VAL) << 18) & OTG_HS_EPTYP_MSK)
#define OTG_HS_EPTYP_GET(REG) (((REG) & OTG_HS_EPTYP_MSK) >> 18)
/* This is the transfer type supported by this logical endpoint.
00: Control
01: Isochronous
10: Bulk
11: Interrupt */

/* Bit 17 - NAK status */
#define OTG_HS_NAKSTS (1 << 17)
/* Indicates the following:
0: The core is transmitting nonNAK handshakes based on the FIFO status.
1: The core is transmitting NAK handshakes on this endpoint.
When either the application or the core sets this bit:
The core stops receiving any data on an OUT endpoint, even if there is space in the RxFIFO to accommodate the incoming packet.
Irrespective of this bit’s setting, the core always responds to SETUP data packets with an
ACK handshake. */

/* Bit 16 - Even/odd frame */
#define OTG_HS_EONUM (1 << 16)
/* Applies to isochronous IN and OUT endpoints only.
Indicates the frame number in which the core transmits/receives isochronous data for this endpoint. The application must program the even/odd frame number in which it intends to transmit/receive isochronous data for this endpoint using the SEVNFRM and SODDFRM fields in this register.
0: Even frame
1: Odd frame
DPID: Endpoint data PID
Applies to interrupt/bulk OUT endpoints only. Contains the PID of the packet to be received or transmitted on this endpoint. The application must program the PID of the first packet to be received or transmitted on this endpoint, after the endpoint is activated. The application uses the SD0PID register field to program either DATA0 or DATA1 PID.
0: DATA0
1: DATA1 */

/* Bit 15 - USB active endpoint */
#define OTG_HS_USBAEP (1 << 15)
/* Indicates whether this endpoint is active in the current configuration and interface. The core
clears this bit for all endpoints (other than EP 0) after detecting a USB reset. After receiving the SetConfiguration and SetInterface commands, the application must program endpoint registers accordingly and set this bit.  */

/* [14..11] Reserved, must be kept at reset value. */


/* Bits [10..0] - Maximum packet size */
#define OTG_HS_MPSIZ_MSK (((1 << (10 - 0 + 1)) - 1) << 0)
#define OTG_HS_MPSIZ_SET(VAL) (((VAL) << 0) & OTG_HS_MPSIZ_MSK)
#define OTG_HS_MPSIZ_GET(REG) (((REG) & OTG_HS_MPSIZ_MSK) >> 0)
/* The application must program this field with the maximum packet size for the current logical endpoint. This value is in bytes. */


/* OTG_HS device endpoint-x interrupt register */
#define STM32F_OTG_HS_DIEPINTX 0x908

/* This register indicates the status of an endpoint with respect to USB- and AHB-related events. It is shown in Figure 412. The application must read this register when the IN endpoints interrupt bit of the Core interrupt register (IEPINT in OTG_HS_GINTSTS) is set.
Before the application can read this register, it must first read the device all endpoints interrupt (OTG_HS_DAINT) register to get the exact endpoint number for the device endpoint-x interrupt register. The application must clear the appropriate bit in this register to clear the corresponding bits in the OTG_HS_DAINT and OTG_HS_GINTSTS registers. */

/* [31..14] Reserved, must be kept at reset value. */


/* Bit 13 - NAK interrupt */
#define OTG_HS_NAK (1 << 13)
/* The core generates this interrupt when a NAK is transmitted or received by the device. In case of isochronous IN endpoints the interrupt gets generated when a zero length packet is transmitted due to unavailability of data in the Tx FIFO. */

/* Bit 12 - Babble error interrupt */
#define OTG_HS_BERR (1 << 12)
/* Bit 11 - Packet dropped status */
#define OTG_HS_PKTDRPSTS (1 << 11)
/* This bit indicates to the application that an ISOC OUT packet has been dropped. This bit does not have an associated mask bit and does not generate an interrupt. */

/* Bit10 Reserved, must be kept at reset value. */

/* Bit 9 - Buffer not available interrupt */
#define OTG_HS_BNA (1 << 9)
/* The core generates this interrupt when the descriptor accessed is not ready for the Core to process, such as host busy or DMA done. */

/* Bit 8 - Transmit Fifo Underrun (TxfifoUndrn) The core generates this interrupt when it detects a transmit FIFO underrun condition for this endpoint. */
#define OTG_HS_TXFIFOUDRN (1 << 8)
/* Dependency: This interrupt is valid only when Thresholding is enabled */

/* Bit 7 - Transmit FIFO empty */
#define OTG_HS_TXFE (1 << 7)
/* This interrupt is asserted when the TxFIFO for this endpoint is either half or completely empty. The half or completely empty status is determined by the TxFIFO empty level bit in the Core AHB configuration register (TXFELVL bit in OTG_HS_GAHBCFG). */

/* Bit 6 - IN endpoint NAK effective */
#define OTG_HS_INEPNE (1 << 6)
/* This bit can be cleared when the application clears the IN endpoint NAK by writing to the CNAK bit in OTG_HS_DIEPCTLx.
This interrupt indicates that the core has sampled the NAK bit set (either by the application or by the core). The interrupt indicates that the IN endpoint NAK bit set by the application has taken effect in the core.
This interrupt does not guarantee that a NAK handshake is sent on the USB. A STALL bit takes priority over a NAK bit.
Bit 5
Reserved, must be kept at reset value. */

/* Bit 4 - IN token received when TxFIFO is empty */
#define OTG_HS_ITTXFE (1 << 4)
/* Applies to nonperiodic IN endpoints only.
Indicates that an IN token was received when the associated TxFIFO (periodic/nonperiodic) was empty. This interrupt is asserted on the endpoint for which the IN token was received. */

/* Bit 3 - Timeout condition */
#define OTG_HS_TOC (1 << 3)
/* Applies only to Control IN endpoints.
Indicates that the core has detected a timeout condition on the USB for the last IN token on this endpoint.
Bit 2
Reserved, must be kept at reset value. */

/* Bit 1 - Endpoint disabled interrupt */
#define OTG_HS_EPDISD (1 << 1)
/* This bit indicates that the endpoint is disabled per the application’s request. */

/* Bit 0 - Transfer completed interrupt */
#define OTG_HS_XFRC (1 << 0)
/* This field indicates that the programmed transfer is complete on the AHB as well as on the USB, for this endpoint. */


/* OTG_HS device endpoint-x interrupt register */
#define STM32F_OTG_HS_DOEPINTX 0xB08

/* This register indicates the status of an endpoint with respect to USB- and AHB-related events. It is shown in Figure 412. The application must read this register when the OUT
Endpoints Interrupt bit of the Core interrupt register (OEPINT bit in OTG_HS_GINTSTS) is set. Before the application can read this register, it must first read the device all endpoints interrupt (OTG_HS_DAINT) register to get the exact endpoint number for the device
Endpoint-x interrupt register. The application must clear the appropriate bit in this register to clear the corresponding bits in the OTG_HS_DAINT and OTG_HS_GINTSTS registers. */

/* [31..15] Reserved, must be kept at reset value. */


/* Bit 14 - NYET interrupt */
#define OTG_HS_NYET (1 << 14)
/* The core generates this interrupt when a NYET response is transmitted for a nonisochronous OUT endpoint. */

/* [13..7] Reserved, must be kept at reset value. */


/* Bit 6 - Back-to-back SETUP packets received */
#define OTG_HS_B2BSTUP (1 << 6)
/* Applies to Control OUT endpoint only.
This bit indicates that the core has received more than three back-to-back SETUP packets for this particular endpoint.
Bit 5
Reserved, must be kept at reset value. */

/* Bit 4 - OUT token received when endpoint disabled */
#define OTG_HS_OTEPDIS (1 << 4)
/* Applies only to control OUT endpoint.
Indicates that an OUT token was received when the endpoint was not yet enabled. This interrupt is asserted on the endpoint for which the OUT token was received. */

/* Bit 3 - SETUP phase done */
#define OTG_HS_STUP (1 << 3)
/* Applies to control OUT endpoints only.
Indicates that the SETUP phase for the control endpoint is complete and no more back-to-back SETUP packets were received for the current control transfer. On this interrupt, the application can decode the received SETUP data packet.
Bit 2
Reserved, must be kept at reset value. */

/* Bit 1 - Endpoint disabled interrupt */
#define OTG_HS_EPDISD (1 << 1)
/* This bit indicates that the endpoint is disabled per the application’s request. */

/* Bit 0 - Transfer completed interrupt */
#define OTG_HS_XFRC (1 << 0)
/* This field indicates that the programmed transfer is complete on the AHB as well as on the USB, for this endpoint. */


/* OTG_HS device IN endpoint 0 transfer size register */
#define STM32F_OTG_HS_DIEPTSIZ0 0x910

/* The application must modify this register before enabling endpoint 0. Once endpoint 0 is enabled using the endpoint enable bit in the device control endpoint 0 control registers (EPENA in OTG_HS_DIEPCTL0), the core modifies this register. The application can only read this register once the core has cleared the Endpoint enable bit.
Nonzero endpoints use the registers for endpoints 1–15. */

/* [31..21] Reserved, must be kept at reset value. */


/* Bits [20..19] - Packet count */
//#define OTG_HS_PKTCNT_MSK (((1 << (20 - 19 + 1)) - 1) << 19)
//#define OTG_HS_PKTCNT_SET(VAL) (((VAL) << 19) & OTG_HS_PKTCNT_MSK)
//#define OTG_HS_PKTCNT_GET(REG) (((REG) & OTG_HS_PKTCNT_MSK) >> 19)
#define OTG_HS_PKTCNT0_MSK (((1 << (20 - 19 + 1)) - 1) << 19)
#define OTG_HS_PKTCNT0(VAL) (((VAL) << 19) & OTG_HS_PKTCNT0_MSK)
#define OTG_HS_PKTCNT0_GET(REG) (((REG) & OTG_HS_PKTCNT0_MSK) >> 19)
/* Indicates the total number of USB packets that constitute the Transfer Size amount of data for endpoint 0.
This field is decremented every time a packet (maximum size or short packet) is read from the TxFIFO. */

/* [18..7] Reserved, must be kept at reset value. */


/* Bits [6..0] - Transfer size */
//#define OTG_HS_XFRSIZ_MSK (((1 << (6 - 0 + 1)) - 1) << 0)
//#define OTG_HS_XFRSIZ_SET(VAL) (((VAL) << 0) & OTG_HS_XFRSIZ_MSK)
//#define OTG_HS_XFRSIZ_GET(REG) (((REG) & OTG_HS_XFRSIZ_MSK) >> 0)
#define OTG_HS_XFRSIZ0_MSK (((1 << (6 - 0 + 1)) - 1) << 0)
#define OTG_HS_XFRSIZ0(VAL) (((VAL) << 0) & OTG_HS_XFRSIZ0_MSK)
#define OTG_HS_XFRSIZ0_GET(REG) (((REG) & OTG_HS_XFRSIZ0_MSK) >> 0)
/* Indicates the transfer size in bytes for endpoint 0. The core interrupts the application only after it has exhausted the transfer size amount of data. The transfer size can be set to the maximum packet size of the endpoint, to be interrupted at the end of each packet.
The core decrements this field every time a packet from the external memory is written to the TxFIFO. */


/* OTG_HS device OUT endpoint 0 transfer size register */
#define STM32F_OTG_HS_DOEPTSIZ0 0xB10

/* The application must modify this register before enabling endpoint 0. Once endpoint 0 is enabled using the Endpoint enable bit in the device control endpoint 0 control registers (EPENA bit in OTG_HS_DOEPCTL0), the core modifies this register. The application can only read this register once the core has cleared the Endpoint enable bit.
Nonzero endpoints use the registers for endpoints 1–15. */

/* Bit 31 Reserved, must be kept at reset value. */


/* Bits [30..29] - SETUP packet count */
#define OTG_HS_STUPCNT_MSK (((1 << (30 - 29 + 1)) - 1) << 29)
#define OTG_HS_STUPCNT_SET(VAL) (((VAL) << 29) & OTG_HS_STUPCNT_MSK)
#define OTG_HS_STUPCNT_GET(REG) (((REG) & OTG_HS_STUPCNT_MSK) >> 29)
/* This field specifies the number of back-to-back SETUP data packets the endpoint can receive.
01: 1 packet
10: 2 packets
11: 3 packets */

/* [28..20] Reserved, must be kept at reset value. */


/* Bit 19 - Packet count */
//#define OTG_HS_PKTCNT (1 << 19)
#define OTG_HS_PKTCNT_MSK (((1 << (28 - 19 + 1)) - 1) << 19)
#define OTG_HS_PKTCNT_SET(VAL) (((VAL) << 19) & OTG_HS_PKTCNT_MSK)
#define OTG_HS_PKTCNT_GET(REG) (((REG) & OTG_HS_PKTCNT_MSK) >> 19)
/* This field is decremented to zero after a packet is written into the RxFIFO. */

/* [18..7] Reserved, must be kept at reset value. */


/* Bits [6..0] - Transfer size */
//#define OTG_HS_XFRSIZ_MSK (((1 << (6 - 0 + 1)) - 1) << 0)
//#define OTG_HS_XFRSIZ_SET(VAL) (((VAL) << 0) & OTG_HS_XFRSIZ_MSK)
//#define OTG_HS_XFRSIZ_GET(REG) (((REG) & OTG_HS_XFRSIZ_MSK) >> 0)
#define OTG_HS_XFRSIZ0_MSK (((1 << (6 - 0 + 1)) - 1) << 0)
#define OTG_HS_XFRSIZ0(VAL) (((VAL) << 0) & OTG_HS_XFRSIZ0_MSK)
#define OTG_HS_XFRSIZ0_GET(REG) (((REG) & OTG_HS_XFRSIZ0_MSK) >> 0)
/* Indicates the transfer size in bytes for endpoint 0. The core interrupts the application only after it has exhausted the transfer size amount of data. The transfer size can be set to the maximum packet size of the endpoint, to be interrupted at the end of each packet.
The core decrements this field every time a packet is read from the RxFIFO and written to the external memory. */

/* OTG_HS device endpoint-x transfer size register (OTG_HS_DIEPTSIZx)
(x = 1..3, where x = Endpoint_number)
Address offset: 0x910 + (Endpoint_number × 0x20) */
/* The application must modify this register before enabling the endpoint. Once the endpoint is enabled using the Endpoint enable bit in the device endpoint-x control registers (EPENA bit in OTG_HS_DIEPCTLx), the core modifies this register. The application can only read this register once the core has cleared the Endpoint enable bit. */

/* Bit 31 Reserved, must be kept at reset value. */


/* Bits [30..29] - Multi count */
#define OTG_HS_MCNT_MSK (((1 << (30 - 29 + 1)) - 1) << 29)
#define OTG_HS_MCNT_SET(VAL) (((VAL) << 29) & OTG_HS_MCNT_MSK)
#define OTG_HS_MCNT_GET(REG) (((REG) & OTG_HS_MCNT_MSK) >> 29)
/* For periodic IN endpoints, this field indicates the number of packets that must be transmitted per frame on the USB. The core uses this field to calculate the data PID for isochronous IN
endpoints.
01: 1 packet
10: 2 packets
11: 3 packets */

/* Bits [28..19] - Packet count */
#define OTG_HS_PKTCNT_MSK (((1 << (28 - 19 + 1)) - 1) << 19)
#define OTG_HS_PKTCNT_SET(VAL) (((VAL) << 19) & OTG_HS_PKTCNT_MSK)
#define OTG_HS_PKTCNT_GET(REG) (((REG) & OTG_HS_PKTCNT_MSK) >> 19)
/* Indicates the total number of USB packets that constitute the Transfer Size amount of data for this endpoint.
This field is decremented every time a packet (maximum size or short packet) is read from the TxFIFO. */

/* Bits [18..0] - Transfer size */
#define OTG_HS_XFRSIZ_MSK (((1 << (18 - 0 + 1)) - 1) << 0)
#define OTG_HS_XFRSIZ_SET(VAL) (((VAL) << 0) & OTG_HS_XFRSIZ_MSK)
#define OTG_HS_XFRSIZ_GET(REG) (((REG) & OTG_HS_XFRSIZ_MSK) >> 0)
/* This field contains the transfer size in bytes for the current endpoint. The core only interrupts the application after it has exhausted the transfer size amount of data. The transfer size can be set to the maximum packet size of the endpoint, to be interrupted at the end of each packet.
The core decrements this field every time a packet from the external memory is written to the TxFIFO. */

/* OTG_HS device IN endpoint transmit FIFO status register (OTG_HS_DTXFSTSx) (x = 0..5, where x = Endpoint_number)
Address offset for IN endpoints: 0x918 + (Endpoint_number × 0x20)  */
/* This read-only register contains the free space information for the Device IN endpoint TxFIFO. */
/* 31:16 Reserved, must be kept at reset value.
15:0 INEPTFSAV: IN endpoint TxFIFO space avail ()
Indicates the amount of free space available in the Endpoint TxFIFO.
Values are in terms of 32-bit words:
0x0: Endpoint TxFIFO is full
0x1: 1 word available
0x2: 2 words available
0xn: n words available (0 < n < 512)
Others: Reserved */


/* OTG_HS device endpoint-x transfer size register */
#define STM32F_OTG_HS_DOEPTSIZX 0xB10

/* The application must modify this register before enabling the endpoint. Once the endpoint is enabled using Endpoint Enable bit of the device endpoint-x control registers (EPENA bit in OTG_HS_DOEPCTLx), the core modifies this register. The application can only read this register once the core has cleared the Endpoint enable bit. */

/* Bit 31 Reserved, must be kept at reset value. */


/* Bits [30..29] - Received data PID */
#define OTG_HS_RXDPID_MSK (((1 << (30 - 29 + 1)) - 1) << 29)
#define OTG_HS_RXDPID_SET(VAL) (((VAL) << 29) & OTG_HS_RXDPID_MSK)
#define OTG_HS_RXDPID_GET(REG) (((REG) & OTG_HS_RXDPID_MSK) >> 29)
/* Applies to isochronous OUT endpoints only.
This is the data PID received in the last packet for this endpoint.
00: DATA0
01: DATA2
10: DATA1
11: MDATA
STUPCNT: SETUP packet count
Applies to control OUT Endpoints only.
This field specifies the number of back-to-back SETUP data packets the endpoint can receive.
01: 1 packet
10: 2 packets
11: 3 packets */

/* Bits [28..19] - Packet count */
#define OTG_HS_PKTCNT_MSK (((1 << (28 - 19 + 1)) - 1) << 19)
#define OTG_HS_PKTCNT_SET(VAL) (((VAL) << 19) & OTG_HS_PKTCNT_MSK)
#define OTG_HS_PKTCNT_GET(REG) (((REG) & OTG_HS_PKTCNT_MSK) >> 19)
/* Indicates the total number of USB packets that constitute the Transfer Size amount of data for this endpoint.
This field is decremented every time a packet (maximum size or short packet) is written to the RxFIFO. */

/* Bits [18..0] - Transfer size */
#define OTG_HS_XFRSIZ_MSK (((1 << (18 - 0 + 1)) - 1) << 0)
#define OTG_HS_XFRSIZ_SET(VAL) (((VAL) << 0) & OTG_HS_XFRSIZ_MSK)
#define OTG_HS_XFRSIZ_GET(REG) (((REG) & OTG_HS_XFRSIZ_MSK) >> 0)
/* This field contains the transfer size in bytes for the current endpoint. The core only interrupts the application after it has exhausted the transfer size amount of data. The transfer size can be set to the maximum packet size of the endpoint, to be interrupted at the end of each packet.
The core decrements this field every time a packet is read from the RxFIFO and written to the external memory. */



/* OTG_HS device endpoint-x DMA address register (OTG_HS_DIEPDMAx / OTG_HS_DOEPDMAx) (x = 1..5, where x = Endpoint_number)
Address offset for IN endpoints: 0x914 + (Endpoint_number × 0x20)
Address offset for OUT endpoints: 0xB14 + (Endpoint_number × 0x20) */

/* Bits [31..0] - DMA address */
#define OTG_HS_DMAADDR_MSK (((1 << (31 - 0 + 1)) - 1) << 0)
#define OTG_HS_DMAADDR_SET(VAL) (((VAL) << 0) & OTG_HS_DMAADDR_MSK)
#define OTG_HS_DMAADDR_GET(REG) (((REG) & OTG_HS_DMAADDR_MSK) >> 0)
/* This bit holds the start address of the external memory for storing or fetching endpoint data.
Note: For control endpoints, this field stores control OUT data packets as well as SETUP transaction data packets. When more than three SETUP packets are received back-to-back, the SETUP data packet in the memory is overwritten. This register is incremented on every AHB transaction. The application can give only a DWORD-aligned address. */


/* OTG_HS power and clock gating control register */
#define STM32F_OTG_HS_PCGCCTL 0xE00

/* This register is available in host and peripheral modes. */

/* [31..5] Reserved, must be kept at reset value. */


/* Bit 4 - PHY suspended */
#define OTG_HS_PHYSUSP (1 << 4)
/* Indicates that the PHY has been suspended. This bit is updated once the PHY is suspended after the application has set the STPPCLK bit (bit 0). */

/* [3..2] Reserved, must be kept at reset value. */


/* Bit 1 - Gate HCLK */
#define OTG_HS_GATEHCLK (1 << 1)
/* The application sets this bit to gate HCLK to modules other than the AHB Slave and Master and wakeup logic when the USB is suspended or the session is not valid. The application clears this bit when the USB is resumed or a new session starts. */

/* Bit 0 - Stop PHY clock */
#define OTG_HS_STPPCLK (1 << 0)
/* The application sets this bit to stop the PHY clock when the USB is suspended, the session is not valid, or the device is disconnected. The application clears this bit when the USB is resumed or a new session starts. */

#define STM32F_OTG_HS_RAM_SIZE (4096)

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <stdbool.h>

struct stm32f_otg_hs {
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
	volatile uint32_t gi2cctl;
	uint32_t res1;
	volatile uint32_t gccfg;
	volatile uint32_t cid;
	/* 0x040 */
	uint32_t res2[(0x100 - 0x040) / 4];
	/* 0x100 */
	volatile uint32_t hptxfsiz;
	volatile uint32_t dieptxf1;
	volatile uint32_t dieptxf2;
	volatile uint32_t dieptxf3;
	volatile uint32_t dieptxf4;
	/* 0x110 */
	uint32_t res3[(0x400 - 0x114) / 4];
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
		volatile uint32_t hctdma;   /* 0x514 */
		uint32_t res2[2]; /* 0x518 */
	} hc[12];
	/* 0x600 */
	uint32_t res7[(0x800 - 0x680) / 4];
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
	volatile uint32_t dthrctl;
	volatile uint32_t diepempmsk;
	volatile uint32_t deachint;
	volatile uint32_t deachintmsk;
	/* 0x840 */
	uint32_t res10;
	volatile uint32_t diepeachmsk1;
	uint32_t res11[(0x884 - 0x848) / 4];
	/* 0x884 */
	volatile uint32_t doepeachmsk1;
	uint32_t res12[(0x900 - 0x888) / 4];
	/* 0x900 */
	struct {
		volatile uint32_t diepctl; /* 0x900, 0x920, 0x940, 0x960,
									  0x980, 0x9a0, 0x9c0, 0x9e0 */
		uint32_t res1;
		volatile uint32_t diepint; /* 0x908 .. */
		uint32_t res2;
		volatile uint32_t dieptsiz; /* 0x910 ... */
		volatile uint32_t diepdma; /* 0x914 ...  */
		volatile uint32_t dtxfsts; /* 0x918, 0x938, 0x958, 0x978 ... */
		uint32_t res4;
	} inep[8];
	/* 0xa00 */
	uint32_t res13[(0xb00 - 0xa00) / 4];
	struct {
		volatile uint32_t doepctl; /* 0xb00, 0xb20, 0xb40, 0xb60 ... */
		uint32_t res1;
		volatile uint32_t doepint; /* 0xb08, 0xb28, 0xb48, 0xb68 ... */
		uint32_t res2;
		volatile uint32_t doeptsiz; /* 0xb10, 0xb30, 0xb50, 0xb70 ... */
		volatile uint32_t doepdma; /* 0xb14, 0xb34, 0xb54, 0xb74 ... */
		uint32_t res3[2];
	} outep[8];
	/* 0xb80 */
	uint32_t res14[(0xe00 - 0xb80) / 4];
	/* 0xe00 */
	volatile uint32_t pcgcctl;
	/* 0xe04 */
	uint32_t res15[(0x1000 - 0xe04) / 4];
	/* 0x1000 */
	struct {
		union {
			volatile uint32_t pop;
			volatile uint32_t push;
		};
		uint32_t res1[(0x1000 - 4) / 4];
	} dfifo[8];
	uint32_t res16[(0x20000 - (0x1000 + (8 * 0x1000))) / 4];
	/* 0x20000 */
	uint32_t ram[32768];
};

extern const uint8_t stm32f_otg_hs_ep0_mpsiz_lut[];

#define OTG_HS_EP0_MPSIZ_GET(DEPCTL) \
	stm32f_otg_hs_ep0_mpsiz_lut[OTG_HS_MPSIZ_GET(DEPCTL)]

#define OTG_HS_EP0_MPSIZ_SET(MPSIZ) ((MPSIZ == 64) ? OTG_HS_MPSIZ_64 : \
		((MPSIZ == 32) ? OTG_HS_MPSIZ_32 : \
		((MPSIZ == 16) ? OTG_HS_MPSIZ_16 : OTG_HS_MPSIZ_8)))

#ifdef __cplusplus
extern "C" {
#endif

void stm32f_otg_hs_core_reset(struct stm32f_otg_hs * otg_hs);

void stm32f_otg_hs_device_init(struct stm32f_otg_hs * otg_hs);

void stm32f_otg_hs_txfifo_flush(struct stm32f_otg_hs * otg_hs,
								unsigned int num);

void stm32f_otg_hs_rxfifo_flush(struct stm32f_otg_hs * otg_hs);

void stm32f_otg_hs_addr_set(struct stm32f_otg_hs * otg_hs, unsigned int addr);

void stm32f_otg_hs_ep_disable(struct stm32f_otg_hs * otg_hs, unsigned int addr);

void stm32f_otg_hs_ep_dump(struct stm32f_otg_hs * otg_hs, unsigned int addr);

void stm32f_otg_hs_ep_enable(struct stm32f_otg_hs * otg_hs, unsigned int addr,
							 unsigned int type, unsigned int mpsiz);

/* prepare TX fifo to send */
int stm32f_otg_hs_txf_setup(struct stm32f_otg_hs * otg_hs, unsigned int ep,
							 unsigned int len);

/* push data inot TX fifo */
int stm32f_otg_hs_txf_push(struct stm32f_otg_hs * otg_hs, unsigned int ep,
						   void * buf);

void stm32f_otg_hs_enum_done(struct stm32f_otg_hs * otg_hs);

#ifdef __cplusplus
}
#endif


#endif /* __ASSEMBLER__ */

#endif /* __STM32F_OTG_HS_H__ */
