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
 * @file stm32f-eth.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_ETH_H__
#define __STM32F_ETH_H__

/*-------------------------------------------------------------------------
  Ethernet (ETH): media access control (MAC) with DMA controller
  -------------------------------------------------------------------------*/

/* Ethernet MAC configuration register */
#define STM32F_ETH_MACCR 0x0000

/* Bits 31:24 Reserved */

/* Bit 25 - CRC stripping for Type frames */
#define ETH_CSTF (1 << 25)
/* When set, the last 4 bytes (FCS) of all frames of Ether type (type 
   field greater than 0x0600) will be stripped and dropped before forwarding 
   the frame to the application. */

/* Bit 24 Reserved */

/* Bit 23 - Watchdog disable */
#define ETH_WD (1 << 23)
/* When this bit is set, the MAC disables the watchdog timer on the receiver, 
   and can receive frames of up to 16 384 bytes.
   When this bit is reset, the MAC allows no more than 2 048 bytes of the 
   frame being received and cuts off any bytes received after that. */

/* Bit 22 - Jabber disable */
#define ETH_JD (1 << 22)
/* When this bit is set, the MAC disables the jabber timer on the transmitter, 
   and can transfer frames of up to 16 384 bytes.
   When this bit is reset, the MAC cuts off the transmitter if the 
   application sends out more than 2048 bytes of data during transmission. */

/* [21..20] Reserved */

/* Bits [19..17] - Interframe gap */
#define ETH_IFG ((19 - 17) << 17)
/* These bits control the minimum interframe gap between frames 
   during transmission.
	000: 96 bit times
	001: 88 bit times
	010: 80 bit times
	....
	111: 40 bit times
	Note: In Half-duplex mode, the minimum IFG can be configured for 64 
	bit times (IFG = 100) only. Lower values are not considered. */

/* Bit 16 - Carrier sense disable */
#define ETH_CSD (1 << 16)
/* When set high, this bit makes the MAC transmitter ignore the MII CRS 
   signal during frame transmission in Half-duplex mode. No error is 
   generated due to Loss of Carrier or No Carrier during such transmission.
   When this bit is low, the MAC transmitter generates such errors due to 
   Carrier Sense and even aborts the transmissions. */

/* Bit 15 Reserved */


/* Bit 14 - Fast Ethernet speed */
#define ETH_FES (1 << 14)
/* Indicates the speed in Fast Ethernet (MII) mode:
 * 0: 10 Mbit/s
 * 1: 100 Mbit/s 
 */

/* Bit 13 - Receive own disable */
#define ETH_ROD (1 << 13)
/* When this bit is set, the MAC disables the reception of frames in 
   Half-duplex mode.
   When this bit is reset, the MAC receives all packets that are given by 
   the PHY while transmitting. 
   This bit is not applicable if the MAC is operating in Full-duplex mode. */

/* Bit 12 - Loopback mode */
#define ETH_LM (1 << 12)
/* When this bit is set, the MAC operates in loopback mode at the MII. The 
   MII receive clock input (RX_CLK) is required for the loopback to work 
   properly, as the transmit clock is not looped-back internally. */

/* Bit 11 - Duplex mode */
#define ETH_DM (1 << 11)
/* When this bit is set, the MAC operates in a Full-duplex mode where it can 
   transmit and receive simultaneously. */

/* Bit 10 - IPv4 checksum offload */
#define ETH_IPCO (1 << 10)
/* When set, this bit enables IPv4 checksum checking for received frame 
   payloads' TCP/UDP/ICMP headers. When this bit is reset, the checksum 
   offload function in the receiver is disabled and the corresponding PCE 
   and IP HCE status bits (see Table 142 on page 827) are always cleared. */

/* Bit 9 - Retry disable */
#define ETH_RD (1 << 9)
/* When this bit is set, the MAC attempts only 1 transmission. When a 
   collision occurs on the MII, the MAC ignores the current frame 
   transmission and reports a Frame Abort with excessive collision error in 
   the transmit frame status.
   When this bit is reset, the MAC attempts retries based on the 
   settings of BL.
   Note: This bit is applicable only in the Half-duplex mode. */

/* Bit 8 Reserved */


/* Bit 7 - Automatic pad/CRC stripping */
#define ETH_APCS (1 << 7)
/* When this bit is set, the MAC strips the Pad/FCS field on incoming frames 
   only if the length’s field value is less than or equal to 1 500 bytes. All 
   received frames with length field greater than or equal to 1 501 bytes are 
   passed on to the application without stripping the Pad/FCS field.
   When this bit is reset, the MAC passes all incoming frames unmodified. */

/* Bits [6..5] - Back-off limit */
#define ETH_BL ((6 - 5) << 5)
/* The Back-off limit determines the random integer number (r) of slot time 
   delays (4 096 bit times for 1000 Mbit/s and 512 bit times for 
   10/100 Mbit/s) the MAC waits before rescheduling a transmission attempt 
   during retries after a collision.
Note: This bit is applicable only to Half-duplex mode.
00: k = min (n, 10)
01: k = min (n, 8)
10: k = min (n, 4)
11: k = min (n, 1),
where n = retransmission attempt. The random integer r takes the value in the range 0 ≤ r < 2k */

/* Bit 4 - Deferral check */
#define ETH_DC (1 << 4)
/* When this bit is set, the deferral check function is enabled in the 
   MAC. The MAC issues a Frame Abort status, along with the excessive deferral 
   error bit set in the transmit frame status when the transmit state machine 
   is deferred for more than 24 288 bit times in 10/100-Mbit/s mode. Deferral 
   begins when the transmitter is ready to transmit, but is prevented because 
   of an active CRS (carrier sense) signal on the MII. Defer time is not 
   cumulative. If the transmitter defers for 10 000 bit times, then 
   transmits, collides, backs off, and then has to defer again after 
   completion of back-off, the deferral timer resets to 0 and restarts.
   When this bit is reset, the deferral check function is disabled and the 
   MAC defers until the CRS signal goes inactive. This bit is applicable only 
   in Half-duplex mode. */

/* Bit 3 - Transmitter enable */
#define ETH_TE (1 << 3)
/* When this bit is set, the transmit state machine of the MAC is enabled 
   for transmission on the MII.
   When this bit is reset, the MAC transmit state machine is disabled after 
   the completion of the transmission of the current frame, and does not 
   transmit any further frames. */

/* Bit 2 - Receiver enable */
#define ETH_RE (1 << 2)
/* When this bit is set, the receiver state machine of the MAC is enabled for 
   receiving frames from the MII.
   When this bit is reset, the MAC receive state machine is disabled after 
   the completion of the reception of the current frame, and will not receive 
   any further frames from the MII. */

/* [1..0] Reserved */


/* Ethernet MAC frame filter register */
#define STM32F_ETH_MACFFR 0x0004


/* Bit 31 - Receive all */
#define ETH_RA (1 << 31)
/* When this bit is set, the MAC receiver passes all received frames on to 
   the application, irrespective of whether they have passed the 
   address filter. The result of the SA/DA filtering is updated (pass or fail) 
   in the corresponding bits in the receive status word.
   When this bit is reset, the MAC receiver passes on to the application only 
   those frames that have passed the SA/DA address filter. */

/* [30..11] Reserved */


/* Bit 10 - Hash or perfect filter */
#define ETH_HPF (1 << 10)
/* When this bit is set and if the HM or HU bit is set, the address 
   filter passes frames that match either the perfect filtering or the hash 
   filtering.
   When this bit is cleared and if the HU or HM bit is set, only frames that 
   match the Hash filter are passed. */

/* Bit 9 - Source address filter */
#define ETH_SAF (1 << 9)
/* The MAC core compares the SA field of the received frames with the values 
   programmed in the enabled SA registers. If the comparison matches, then 
   the SAMatch bit in the RxStatus word is set high. When this bit is set 
   high and the SA filter fails, the MAC drops the frame.
   When this bit is reset, the MAC core forwards the received frame to the 
   application. It also forwards the updated SA Match bit in RxStatus 
   depending on the SA address comparison. */

/* Bit 8 - Source address inverse filtering */
#define ETH_SAIF (1 << 8)
/* When this bit is set, the address check block operates in inverse 
   filtering mode for the SA address comparison. The frames whose SA 
   matches the SA registers are marked as failing the SA address filter.
   When this bit is reset, frames whose SA does not match the SA registers 
   are marked as failing the SA address filter. */

/* Bits [7..6] - Pass control frames */
#define ETH_PCF ((7 - 6) << 6)
/* These bits control the forwarding of all control frames (including 
   unicast and multicast PAUSE frames). Note that the processing of PAUSE 
   control frames depends only on RFCE in Flow
   Control Register[2].
00: MAC prevents all control frames from reaching the application
01: MAC forwards all control frames to application except Pause control frames
10: MAC forwards all control frames to application even if they fail 
the address filter
11: MAC forwards control frames that pass the address filter.
These bits control the forwarding of all control frames (including unicast and multicast PAUSE
frames). Note that the processing of PAUSE control frames depends only on RFCE in Flow
Control Register[2].
00 or 01: MAC prevents all control frames from reaching the application
10: MAC forwards all control frames to application even if they fail the address filter
11: MAC forwards control frames that pass the address filter. */

/* Bit 5 - Broadcast frames disable */
#define ETH_BFD (1 << 5)
/* When this bit is set, the address filters filter all incoming 
   broadcast frames.
   When this bit is reset, the address filters pass all received 
   broadcast frames. */

/* Bit 4 - Pass all multicast */
#define ETH_PAM (1 << 4)
/* When set, this bit indicates that all received frames with a multicast 
   destination address (first bit in the destination address field is '1') 
   are passed.
   When reset, filtering of multicast frame depends on the HM bit. */

/* Bit 3 - Destination address inverse filtering */
#define ETH_DAIF (1 << 3)
/* When this bit is set, the address check block operates in inverse 
   filtering mode for the DA address comparison for both unicast and 
   multicast frames.
   When reset, normal filtering of frames is performed. */

/* Bit 2 - Hash multicast */
#define ETH_HM (1 << 2)
/* When set, MAC performs destination address filtering of received 
   multicast frames according to the hash table.
   When reset, the MAC performs a perfect destination address filtering for 
   multicast frames, that is, it compares the DA field with the values 
   programmed in DA registers. */

/* Bit 1 - Hash unicast */
#define ETH_HU (1 << 1)
/* When set, MAC performs destination address filtering of unicast 
   frames according to the hash table.
   When reset, the MAC performs a perfect destination address filtering 
   for unicast frames, that is, it compares the DA field with 
   the values programmed in DA registers. */

/* Bit 0 - Promiscuous mode */
#define ETH_PM (1 << 0)
/* When this bit is set, the address filters pass all incoming frames 
   regardless of their destination or source address. The SA/DA filter fails 
   status bits in the receive status word are always cleared when PM is set. */

/* Ethernet MAC hash table high register */
#define STM32F_ETH_MACHTHR 0x0008



/* Bits [31..0] - Hash table high */
#define ETH_HTH ((31 - 0) << 0)
/* 
This field contains the upper 32 bits of Hash table. */

/* Ethernet MAC hash table low register */
#define STM32F_ETH_MACHTLR 0x000C



/* Bits [31..0] - Hash table low */
#define ETH_HTL ((31 - 0) << 0)
/* 
This field contains the lower 32 bits of the Hash table */

/* Ethernet MAC MII address register */
#define STM32F_ETH_MACMIIAR 0x0010



/* [31..16] Reserved */


/* Bits [15..11] - PHY address */
//#define ETH_PA ((15 - 11) << 11)
#define ETH_PA_MSK (((1 << (4 + 1)) - 1) << 11)
#define ETH_PA_SET(VAL) (((VAL) << 11) & ETH_PA_MSK)
#define ETH_PA_GET(REG) (((REG) & ETH_PA_MSK) >> 11)
/* This field tells which of the 32 possible PHY devices are being accessed. */

/* Bits [10..6] - MII register */
#define ETH_MR_MSK (((1 << (4 + 1)) - 1) << 6)
#define ETH_MR_SET(VAL) (((VAL) << 6) & ETH_MR_MSK)
#define ETH_MR_GET(REG) (((REG) & ETH_MR_MSK) >> 6)
/* These bits select the desired MII register in the selected PHY device. */

/* Bit 5 Reserved */

/* Bits [4..2] - Clock range */
//#define ETH_CR ((4 - 2) << 2)
#define ETH_CR_MSK (((1 << (2 + 1)) - 1) << 2)
#define ETH_CR_SET(VAL) (((VAL) << 2) & ETH_CR_MSK)
#define ETH_CR_GET(REG) (((REG) & ETH_CR_MSK) >> 2)

#define ETH_CR_HCLK_42 (0 << 2)
#define ETH_CR_HCLK_62 (1 << 2)
#define ETH_CR_HCLK_16 (2 << 2)
#define ETH_CR_HCLK_26 (3 << 2)

/* The CR clock range selection determines the HCLK frequency and is used 
   to decide the frequency of the MDC clock:
	Selection HCLK MDC Clock
	000 60-100 MHz HCLK/42
	001 100-120 MHz HCLK/62
	010 20-35 MHz HCLK/16
	011 35-60 MHz HCLK/26
	100, 101, 110, 111 Reserved
- */

/* Bit 1 - MII write */
#define ETH_MII_MW (1 << 1)
/* When set, this bit tells the PHY that this will be a Write operation using 
   the MII Data register. If this bit is not set, this will be a Read 
   operation, placing the data in the MII Data register. */

/* Bit 0 - MII busy */
#define ETH_MII_MB (1 << 0)
/* This bit should read a logic 0 before writing to ETH_MACMIIAR and 
   ETH_MACMIIDR. This bit must also be reset to 0 during a Write to 
   ETH_MACMIIAR. During a PHY register access, this bit is set to 0b1 by the 
   application to indicate that a read or write access is in progress. 
   ETH_MACMIIDR (MII Data) should be kept valid until this bit is cleared by 
   the MAC during a PHY Write operation. The ETH_MACMIIDR is invalid until 
   this bit is cleared by the MAC during a PHY Read operation. The 
   ETH_MACMIIAR (MII Address) should not be written to until this bit is 
   cleared. */

/* Ethernet MAC MII data register */
#define STM32F_ETH_MACMIIDR 0x0014

/* [31..16] Reserved */

/* Bits [15..0] - MII data */
#define ETH_MII_MD (0xffff)
/* This contains the 16-bit data value read from the PHY after a 
   Management Read operation, or the 16-bit data value to be written to 
   the PHY before a Management Write operation. */


/* Ethernet MAC flow control register */
#define STM32F_ETH_MACFCR 0x0018


/* Bits [31..16] - Pause time */
#define ETH_PT ((31 - 16) << 16)
/* 
This field holds the value to be used in the Pause Time field in the transmit control frame. If the
Pause Time bits is configured to be double-synchronized to the MII clock domain, then
consecutive write operations to this register should be performed only after at least 4 clock
cycles in the destination clock domain. */

/* [15..8] Reserved */


/* Bit 7 - Zero-quanta pause disable */
#define ETH_ZQPD (1 << 7)
/* 
When set, this bit disables the automatic generation of Zero-quanta pause control frames on
the deassertion of the flow-control signal from the FIFO layer.
When this bit is reset, normal operation with automatic Zero-quanta pause control frame
generation is enabled. */

/* Bit 6 Reserved */


/* Bits [5..4] - Pause low threshold */
#define ETH_PLT ((5 - 4) << 4)
/* 
This field configures the threshold of the Pause timer at which the Pause frame is
automatically retransmitted. The threshold values should always be less than the Pause Time
configured in bits[31:16]. For example, if PT = 100H (256 slot-times), and PLT = 01, then a
second PAUSE frame is automatically transmitted if initiated at 228 (256 – 28) slot-times after
the first PAUSE frame is transmitted.
Selection
Threshold
00
Pause time minus 4 slot times
01
Pause time minus 28 slot times
10
Pause time minus 144 slot times
11
Pause time minus 256 slot times
Slot time is defined as time taken to transmit 512 bits (64 bytes) on the MII interface. */

/* Bit 3 - Unicast pause frame detect */
#define ETH_UPFD (1 << 3)
/* 
When this bit is set, the MAC detects the Pause frames with the station’s unicast address
specified in the ETH_MACA0HR and ETH_MACA0LR registers, in addition to detecting Pause
frames with the unique multicast address.
When this bit is reset, the MAC detects only a Pause frame with the unique multicast address
specified in the 802.3x standard. */

/* Bit 2 - Receive flow control enable */
#define ETH_RFCE (1 << 2)
/* 
When this bit is set, the MAC decodes the received Pause frame and disables its transmitter
for a specified (Pause Time) time.
When this bit is reset, the decode function of the Pause frame is disabled. */

/* Bit 1 - Transmit flow control enable */
#define ETH_TFCE (1 << 1)
/* 
In Full-duplex mode, when this bit is set, the MAC enables the flow control operation to
transmit Pause frames. When this bit is reset, the flow control operation in the MAC is
disabled, and the MAC does not transmit any Pause frames.
In Half-duplex mode, when this bit is set, the MAC enables the back-pressure operation. When
this bit is reset, the back pressure feature is disabled.
Bit 0 FCB/BPA: Flow control busy/back pressure activate
This bit initiates a Pause Control frame in Full-duplex mode and activates the back pressure
function in Half-duplex mode if TFCE bit is set.
In Full-duplex mode, this bit should be read as 0 before writing to the Flow control register. To
initiate a Pause control frame, the Application must set this bit to 1. During a transfer of the
Control frame, this bit continues to be set to signify that a frame transmission is in progress.
After completion of the Pause control frame transmission, the MAC resets this bit to 0. The
Flow control register should not be written to until this bit is cleared.
In Half-duplex mode, when this bit is set (and TFCE is set), back pressure is asserted by the
MAC core. During back pressure, when the MAC receives a new frame, the transmitter starts
sending a JAM pattern resulting in a collision. When the MAC is configured to Full-duplex
mode, the BPA is automatically disabled. */

/* Ethernet MAC VLAN tag register */
#define STM32F_ETH_MACVLANTR 0x001C



/* [31..17] Reserved */


/* Bit 16 - 12-bit VLAN tag comparison */
#define ETH_VLANTC (1 << 16)
/* 
When this bit is set, a 12-bit VLAN identifier, rather than the complete 16-bit VLAN tag, is used
for comparison and filtering. Bits[11:0] of the VLAN tag are compared with the corresponding
field in the received VLAN-tagged frame.
When this bit is reset, all 16 bits of the received VLAN frame’s fifteenth and sixteenth bytes are
used for comparison. */

/* Bits [15..0] - VLAN tag identifier (for receive frames) */
#define ETH_VLANTI ((15 - 0) << 0)
/* 
This contains the 802.1Q VLAN tag to identify VLAN frames, and is compared to the fifteenth
and sixteenth bytes of the frames being received for VLAN frames. Bits[15:13] are the user
priority, Bit[12] is the canonical format indicator (CFI) and bits[11:0] are the VLAN tag’s VLAN
identifier (VID) field. When the VLANTC bit is set, only the VID (bits[11:0]) is used for
comparison.
If VLANTI (VLANTI[11:0] if VLANTC is set) is all zeros, the MAC does not check the fifteenth
and sixteenth bytes for VLAN tag comparison, and declares all frames with a Type field value
of 0x8100 as VLAN frames. */

/* Ethernet MAC remote wakeup frame filter register */
#define STM32F_ETH_MACRWUFFR 0x0028



/* Ethernet MAC PMT control and status register */
#define STM32F_ETH_MACPMTCSR 0x002C



/* Bit 31 - Wakeup frame filter register pointer reset */
#define ETH_WFFRPR (1 << 31)
/* 
When set, it resets the Remote wakeup frame filter register pointer to 0b000. It is automatically
cleared after 1 clock cycle. */

/* [30..10] Reserved */


/* Bit 9 - Global unicast */
#define ETH_GU (1 << 9)
/* 
When set, it enables any unicast packet filtered by the MAC (DAF) address recognition to be a
wakeup frame. */

/* [8..7] Reserved */


/* Bit 6 - Wakeup frame received */
#define ETH_WFR (1 << 6)
/* 
When set, this bit indicates the power management event was generated due to reception of a
wakeup frame. This bit is cleared by a read into this register. */

/* Bit 5 - Magic packet received */
#define ETH_MPR (1 << 5)
/* 
When set, this bit indicates the power management event was generated by the reception of a
Magic Packet. This bit is cleared by a read into this register. */

/* [4..3] Reserved */


/* Bit 2 - Wakeup frame enable */
#define ETH_WFE (1 << 2)
/* 
When set, this bit enables the generation of a power management event due to wakeup frame
reception. */

/* Bit 1 - Magic Packet enable */
#define ETH_MPE (1 << 1)
/* 
When set, this bit enables the generation of a power management event due to Magic Packet
reception. */

/* Bit 0 - Power down */
#define ETH_PD (1 << 0)
/* 
When this bit is set, all received frames will be dropped. This bit is cleared automatically when
a magic packet or wakeup frame is received, and Power-down mode is disabled. Frames
received after this bit is cleared are forwarded to the application. This bit must only be set
when either the Magic Packet Enable or Wakeup Frame Enable bit is set high. */

/* Ethernet MAC debug register */
#define STM32F_ETH_MACDBGR 0x0034



/* [31..26] Reserved */


/* Bit 25 - Tx FIFO full */
#define ETH_TFF (1 << 25)
/* 
When high, it indicates that the Tx FIFO is full and hence no more frames will be accepted for
transmission. */

/* Bit 24 - Tx FIFO not empty */
#define ETH_TFNE (1 << 24)
/* 
When high, it indicates that the TxFIFO is not empty and has some data left for transmission. */

/* Bit 23 Reserved */


/* Bit 22 - Tx FIFO write active */
#define ETH_TFWA (1 << 22)
/* 
When high, it indicates that the TxFIFO write controller is active and transferring data to the
TxFIFO. */

/* Bits [21..20] - Tx FIFO read status */
#define ETH_TFRS ((21 - 20) << 20)
/* 
This indicates the state of the TxFIFO read controller:
00: Idle state
01: Read state (transferring data to the MAC transmitter)
10: Waiting for TxStatus from MAC transmitter
11: Writing the received TxStatus or flushing the TxFIFO */

/* Bit 19 - MAC transmitter in pause */
#define ETH_MTP (1 << 19)
/* 
When high, it indicates that the MAC transmitter is in Pause condition (in full-duplex mode
only) and hence will not schedule any frame for transmission */

/* Bits [18..17] - MAC transmit frame controller status */
#define ETH_MTFCS ((18 - 17) << 17)
/* 
This indicates the state of the MAC transmit frame controller:
00: Idle
01: Waiting for Status of previous frame or IFG/backoff period to be over
10: Generating and transmitting a Pause control frame (in full duplex mode)
11: Transferring input frame for transmission */

/* Bit 16 - MAC MII transmit engine active */
#define ETH_MMTEA (1 << 16)
/* 
When high, it indicates that the MAC MII transmit engine is actively transmitting data and that
it is not in the Idle state. */

/* [15..10] Reserved */


/* Bits [9..8] - Rx FIFO fill level */
#define ETH_RFFL ((9 - 8) << 8)
/* 
This gives the status of the Rx FIFO fill-level:
00: RxFIFO empty
01: RxFIFO fill-level below flow-control de-activate threshold
10: RxFIFO fill-level above flow-control activate threshold
11: RxFIFO full */

/* Bit 7 Reserved */


/* Bits [6..5] - Rx FIFO read controller status */
#define ETH_RFRCS ((6 - 5) << 5)
/* 
It gives the state of the Rx FIFO read controller:
00: IDLE state
01: Reading frame data
10: Reading frame status (or time-stamp)
11: Flushing the frame data and status */

/* Bit 4 - Rx FIFO write controller active */
#define ETH_RFWRA (1 << 4)
/* 
When high, it indicates that the Rx FIFO write controller is active and transferring a received
frame to the FIFO. */

/* Bit 3 Reserved */


/* Bits [2..1] - MAC small FIFO read / write controllers status */
#define ETH_MSFRWCS ((2 - 1) << 1)
/* When high, these bits indicate the respective active state of the 
   small FIFO read and write controllers of the MAC receive frame 
   controller module. */

/* Bit 0 - MAC MII receive protocol engine active */
#define ETH_MMRPEA (1 << 0)
/* When high, it indicates that the MAC MII receive protocol engine is 
   actively receiving data and is not in the Idle state. */

/* Ethernet MAC interrupt status register */
#define STM32F_ETH_MACSR 0x0038

/* [15..10] Reserved */

/* Bit 9 - Time stamp trigger status */
#define ETH_TSTS (1 << 9)
/* This bit is set high when the system time value equals or exceeds the 
   value specified in the Target time high and low registers. This bit 
   is cleared when this register is read. */

/* [8..7] Reserved */

/* Bit 6 - MMC transmit status */
#define ETH_MMCTS (1 << 6)
/* This bit is set high whenever an interrupt is generated in the 
   ETH_MMCTIR Register. This bit is cleared when all the bits in this 
   interrupt register (ETH_MMCTIR) are cleared. */

/* Bit 5 - MMC receive status */
#define ETH_MMCRS (1 << 5)
/* This bit is set high whenever an interrupt is generated in the 
   ETH_MMCRIR register. This bit is cleared when all the bits in this 
   interrupt register (ETH_MMCRIR) are cleared. */

/* Bit 4 - MMC status */
#define ETH_MMCS (1 << 4)
/* This bit is set high whenever any of bits 6:5 is set high. It is 
   cleared only when both bits are low. */

/* Bit 3 - PMT status */
#define ETH_PMTS (1 << 3)
/* This bit is set whenever a Magic packet or Wake-on-LAN frame is received 
   in Power-down mode (See bits 5 and 6 in the ETH_MACPMTCSR register 
   Ethernet MAC PMT control and status register (ETH_MACPMTCSR) on 
   page 883). This bit is cleared when both bits[6:5], of this last 
   register, are cleared due to a read operation to the 
   ETH_MACPMTCSR register. */

/* [2..0] Reserved */


/* Ethernet MAC interrupt mask register */
#define STM32F_ETH_MACIMR 0x003C

/* [15..10] Reserved */

/* Bit 9 - Time stamp trigger interrupt mask */
#define ETH_TSTIM (1 << 9)
/* When set, this bit disables the time stamp interrupt generation. */

/* [8..4] Reserved */

/* Bit 3 - PMT interrupt mask */
#define ETH_PMTIM (1 << 3)
/* When set, this bit disables the assertion of the interrupt signal due 
   to the setting of the PMT Status bit in ETH_MACSR. */

/* [2..0] Reserved */


/* Ethernet MAC address 0 high register */
#define STM32F_ETH_MACA0HR 0x0040



/* Bit 31 - Always 1. */
#define ETH_MO (1 << 31)
/*  */

/* [30..16] Reserved */


/* Bits [15..0] - MAC address0 high [47:32] */
#define ETH_MACA0H ((15 - 0) << 0)
/* 
This field contains the upper 16 bits (47:32) of the 6-byte MAC address0. This is used by the
MAC for filtering for received frames and for inserting the MAC address in the transmit flow
control (Pause) frames. */

/* Ethernet MAC address 0 low register */
#define STM32F_ETH_MACA0LR 0x0044



/* Bits [31..0] - MAC address0 low [31:0] */
#define ETH_MACA0L ((31 - 0) << 0)
/* 
This field contains the lower 32 bits of the 6-byte MAC address0. This is used by the MAC for
filtering for received frames and for inserting the MAC address in the transmit flow control
(Pause) frames. */

/* Ethernet MAC address 1 high register */
#define STM32F_ETH_MACA1HR 0x0048



/* Bit 31 - Address enable */
#define ETH_AE (1 << 31)
/* 
When this bit is set, the address filters use the MAC address1 for perfect filtering. When this bit
is cleared, the address filters ignore the address for filtering. */

/* Bit 30 - Source address */
#define ETH_SA (1 << 30)
/* 
When this bit is set, the MAC address1[47:0] is used for comparison with the SA fields of the
received frame.
When this bit is cleared, the MAC address1[47:0] is used for comparison with the DA fields of
the received frame. */

/* Bits [29..24] - Mask byte control */
#define ETH_MBC ((29 - 24) << 24)
/* 
These bits are mask control bits for comparison of each of the MAC address1 bytes. When
they are set high, the MAC core does not compare the corresponding byte of received DA/SA
with the contents of the MAC address1 registers. Each bit controls the masking of the bytes as
follows:
– Bit 29: ETH_MACA1HR [15:8]
– Bit 28: ETH_MACA1HR [7:0]
– Bit 27: ETH_MACA1LR [31:24]
...
– Bit 24: ETH_MACA1LR [7:0] */

/* [23..16] Reserved */


/* Bits [15..0] - MAC address1 high [47:32] */
#define ETH_MACA1H ((15 - 0) << 0)
/* 
This field contains the upper 16 bits (47:32) of the 6-byte second MAC address. */

/* Ethernet MAC address1 low register */
#define STM32F_ETH_MACA1LR 0x004C



/* Bits [31..0] - MAC address1 low [31:0] */
#define ETH_MACA1L ((31 - 0) << 0)
/* 
This field contains the lower 32 bits of the 6-byte MAC address1. The content of this field is
undefined until loaded by the application after the initialization process. */

/* Ethernet MAC address 2 high register */
#define STM32F_ETH_MACA2HR 0x0050

/* Bit 31 - Address enable */
#define ETH_AE (1 << 31)
/*When this bit is set, the address filters use the MAC address2 for perfect filtering. When reset,
the address filters ignore the address for filtering. */

/* Bit 30 - Source address */
#define ETH_SA (1 << 30)
/*
When this bit is set, the MAC address 2 [47:0] is used for comparison with the SA fields of the
received frame.
When this bit is reset, the MAC address 2 [47:0] is used for comparison with the DA fields of
the received frame.
MBC: Mask byte control
These bits are mask control bits for comparison of each of the MAC address2 bytes. When set
high, the MAC core does not compare the corresponding byte of received DA/SA with the
contents of the MAC address 2 registers. Each bit controls the masking of the bytes as follows:
Bits 29:24– Bit 29: ETH_MACA2HR [15:8]
– Bit 28: ETH_MACA2HR [7:0]
– Bit 27: ETH_MACA2LR [31:24]
...
– Bit 24: ETH_MACA2LR [7:0] */

/* [23..16] Reserved */

/* Bits [15..0] - MAC address1 high [47:32] */
#define ETH_MACA2H ((15 - 0) << 0)
/* This field contains the upper 16 bits (47:32) of the 6-byte MAC address2. */

/* Ethernet MAC address 2 low register */
#define STM32F_ETH_MACA2LR 0x0054

/* Bits [31..0] - MAC address2 low [31:0] */
#define ETH_MACA2L ((31 - 0) << 0)
/* This field contains the lower 32 bits of the 6-byte second MAC address2. The content of this
field is undefined until loaded by the application after the initialization process. */

/* Ethernet MAC address 3 high register */
#define STM32F_ETH_MACA3HR 0x0058


/* Bit 31 - Address enable */
#define ETH_AE (1 << 31)
/* 
When this bit is set, the address filters use the MAC address3 for perfect filtering. When this bit
is cleared, the address filters ignore the address for filtering. */

/* Bit 30 - Source address */
#define ETH_SA (1 << 30)
/* 
When this bit is set, the MAC address 3 [47:0] is used for comparison with the SA fields of the
received frame.
When this bit is cleared, the MAC address 3[47:0] is used for comparison with the DA fields of
the received frame. */

/* Bits [29..24] - Mask byte control */
#define ETH_MBC ((29 - 24) << 24)
/* 
These bits are mask control bits for comparison of each of the MAC address3 bytes. When
these bits are set high, the MAC core does not compare the corresponding byte of received
DA/SA with the contents of the MAC address 3 registers. Each bit controls the masking of the
bytes as follows:
– Bit 29: ETH_MACA3HR [15:8]
– Bit 28: ETH_MACA3HR [7:0]
– Bit 27: ETH_MACA3LR [31:24]
...
– Bit 24: ETH_MACA3LR [7:0] */

/* [23..16] Reserved */


/* Bits [15..0] - MAC address3 high [47:32] */
#define ETH_MACA3H ((15 - 0) << 0)
/* 
This field contains the upper 16 bits (47:32) of the 6-byte MAC address3. */

/* Ethernet MAC address 3 low register */
#define STM32F_ETH_MACA3LR 0x005C



/* Bits [31..0] - MAC address3 low [31:0] */
#define ETH_MACA3L ((31 - 0) << 0)
/* 
This field contains the lower 32 bits of the 6-byte second MAC address3. The content of this
field is undefined until loaded by the application after the initialization process.
MMC register description */

/* Ethernet MMC control register */
#define STM32F_ETH_MMCCR 0x0100



/* [31..6] Reserved */


/* Bit 5 - MMC counter Full-Half preset */
#define ETH_MCFHP (1 << 5)
/* 
When MCFHP is low and bit4 is set, all MMC counters get preset to almost-half value. All
frame-counters get preset to 0x7FFF_FFF0 (half - 16)
When MCFHP is high and bit4 is set, all MMC counters get preset to almost-full value. All
frame-counters get preset to 0xFFFF_FFF0 (full - 16) */

/* Bit 4 - MMC counter preset */
#define ETH_MCP (1 << 4)
/* 
When set, all counters will be initialized or preset to almost full or almost half as per
Bit5 above. This bit will be cleared automatically after 1 clock cycle. This bit along
with bit5 is useful for debugging and testing the assertion of interrupts due to MMC
counter becoming half-full or full. */

/* Bit 3 - MMC counter freeze */
#define ETH_MCF (1 << 3)
/* 
When set, this bit freezes all the MMC counters to their current value. (None of the MMC
counters are updated due to any transmitted or received frame until this bit is cleared to 0. If
any MMC counter is read with the Reset on Read bit set, then that counter is also cleared in
this mode.) */

/* Bit 2 - Reset on read */
#define ETH_ROR (1 << 2)
/* 
When this bit is set, the MMC counters is reset to zero after read (self-clearing after reset). The
counters are cleared when the least significant byte lane (bits [7:0]) is read. */

/* Bit 1 - Counter stop rollover */
#define ETH_CSR (1 << 1)
/* 
When this bit is set, the counter does not roll over to zero after it reaches the maximum value. */

/* Bit 0 - Counter reset */
#define ETH_CR (1 << 0)
/* 
When it is set, all counters are reset. This bit is cleared automatically after 1 clock cycle. */

/* Ethernet MMC receive interrupt register */
#define STM32F_ETH_MMCRIR 0x0104



/* [31..18] Reserved */


/* Bit 17 - Received Good Unicast Frames Status */
#define ETH_RGUFS (1 << 17)
/* 
This bit is set when the received, good unicast frames, counter reaches half the maximum
value. */

/* [16..7] Reserved */


/* Bit 6 - Received frames alignment error status */
#define ETH_RFAES (1 << 6)
/* 
This bit is set when the received frames, with alignment error, counter reaches half the
maximum value. */

/* Bit 5 - Received frames CRC error status */
#define ETH_RFCES (1 << 5)
/* 
This bit is set when the received frames, with CRC error, counter reaches half the maximum
value. */

/* [4..0] Reserved */


/* Ethernet MMC transmit interrupt register */
#define STM32F_ETH_MMCTIR 0x0108



/* [31..22] Reserved */


/* Bit 21 - Transmitted good frames status */
#define ETH_TGFS (1 << 21)
/* 
This bit is set when the transmitted, good frames, counter reaches half the maximum value. */

/* [20..16] Reserved */


/* Bit 15 - Transmitted good frames more single collision status */
#define ETH_TGFMSCS (1 << 15)
/* 
This bit is set when the transmitted, good frames after more than a single collision, counter
reaches half the maximum value. */

/* Bit 14 - Transmitted good frames single collision status */
#define ETH_TGFSCS (1 << 14)
/* 
This bit is set when the transmitted, good frames after a single collision, counter reaches half
the maximum value. */

/* [13..0] Reserved */


/* Ethernet MMC receive interrupt mask register */
#define STM32F_ETH_MMCRIMR 0x010C



/* [31..18] Reserved */


/* Bit 17 - Received good unicast frames mask */
#define ETH_RGUFM (1 << 17)
/* 
Setting this bit masks the interrupt when the received, good unicast frames, counter reaches
half the maximum value. */

/* [16..7] Reserved */


/* Bit 6 - Received frames alignment error mask */
#define ETH_RFAEM (1 << 6)
/* Setting this bit masks the interrupt when the received frames, with
 * alignment error, counter reaches half the maximum value. */

/* Bit 5 - Received frame CRC error mask */
#define ETH_RFCEM (1 << 5)
/* Setting this bit masks the interrupt when the received frames, with CRC
 * error, counter reaches half the maximum value. */

/* [4..0] Reserved */


/* Ethernet MMC transmit interrupt mask register */
#define STM32F_ETH_MMCTIMR 0x0110



/* [31..22] Reserved */


/* Bit 21 - Transmitted good frames mask */
#define ETH_TGFM (1 << 21)
/* Setting this bit masks the interrupt when the transmitted, good frames, counter reaches half
the maximum value. */

/* [20..16] Reserved */


/* Bit 15 - Transmitted good frames more single collision mask */
#define ETH_TGFMSCM (1 << 15)
/* Setting this bit masks the interrupt when the transmitted good frames after more than a single
collision counter reaches half the maximum value. */

/* Bit 14 - Transmitted good frames single collision mask */
#define ETH_TGFSCM (1 << 14)
/* Setting this bit masks the interrupt when the transmitted good frames after a single collision
counter reaches half the maximum value. */

/* [13..0] Reserved */

/* Ethernet MMC transmitted good frames after a single collision counter register */
#define STM32F_ETH_MMCTGFSCCR 0x014C

/* Bits [31..0] - Transmitted good frames single collision counter */
#define ETH_TGFSCC ((31 - 0) << 0)
/* Transmitted good frames after a single collision counter.
 * Ethernet MMC transmitted good frames after more than a single collision
 */

/* counter register */
#define STM32F_ETH_MMCTGFMSCCR 0x0150



/* Bits [31..0] - Transmitted good frames more single collision counter */
#define ETH_TGFMSCC ((31 - 0) << 0)
/* Transmitted good frames after more than a single collision counter */

/* Ethernet MMC transmitted good frames counter register */
#define STM32F_ETH_MMCTGFCR 0x0168



/* Bits [31..0] - Transmitted good frames counter */
#define ETH_TGFC ((31 - 0) << 0)

/* Ethernet MMC received frames with CRC error counter register */
#define STM32F_ETH_MMCRFCECR 0x0194


/* Bits [31..0] - Received frames CRC error counter */
#define ETH_RFCEC ((31 - 0) << 0)
/* Received frames with CRC error counter */

/* Ethernet MMC received frames with alignment error counter register */
#define STM32F_ETH_MMCRFAECR 0x0198


/* Bits [31..0] - Received frames alignment error counter */
#define ETH_RFAEC ((31 - 0) << 0)
/* 
Received frames with alignment error counter */

/* MMC received good unicast frames counter register */
#define STM32F_ETH_MMCRGUFCR 0x01C4

/* Bits [31..0] - Received good unicast frames counter */
#define ETH_RGUFC ((31 - 0) << 0)

/* EEE 1588 time stamp registers
 * This section describes the registers required to support precision network
 * clock synchronization functions under the IEEE 1588 standard. */

/* Ethernet PTP time stamp control register */
#define STM32F_ETH_PTPTSCR 0x0700


/* [31..19] Reserved */

/* Bit 18 - Time stamp PTP frame filtering MAC address enable */
#define ETH_TSPFFMAE (1 << 18)
/* When set, this bit uses the MAC address (except for MAC address 0) to
 * filter the PTP frames when PTP is sent directly over Ethernet. */

/* Bits [17..16] - Time stamp clock node type */
#define ETH_TSCNT ((17 - 16) << 16)
/* The following are the available types of clock node:
 00: Ordinary clock
 01: Boundary clock
 10: End-to-end transparent clock
 11: Peer-to-peer transparent clock */

/* Bit 15 - Time stamp snapshot for message relevant to master enable */
#define ETH_TSSMRME (1 << 15)
/* When this bit is set, the snapshot is taken for messages relevant to the
 * master node only. When this bit is cleared the snapshot is taken for
 * messages relevant to the slave node only.
 * This is valid only for the ordinary clock and boundary clock nodes.
 */

/* Bit 14 - Time stamp snapshot for event message enable */
#define ETH_TSSEME (1 << 14)
/* When this bit is set, the time stamp snapshot is taken for event messages
 * only (SYNC, Delay_Req, Pdelay_Req or Pdelay_Resp). When this bit is cleared
 * the snapshot is taken for all other messages except for Announce,
 * Management and Signaling.
 */


/* Bit 13 - Time stamp snapshot for IPv4 frames enable */
#define ETH_TSSIPV4FE (1 << 13)
/* When this bit is set, the time stamp snapshot is taken for IPv4 frames. */

/* Bit 12 - Time stamp snapshot for IPv6 frames enable */
#define ETH_TSSIPV6FE (1 << 12)
/* When this bit is set, the time stamp snapshot is taken for IPv6 frames. */

/* Bit 11 - Time stamp snapshot for PTP over ethernet frames enable */
#define ETH_TSSPTPOEFE (1 << 11)
/* When this bit is set, the time stamp snapshot is taken for frames which have
 * PTP messages in Ethernet frames (PTP over Ethernet) also. By default
 * snapshots are taken for UDP-IPEthernet PTP packets.
 */

/* Bit 10 - Time stamp PTP packet snooping for version2 format enable */
#define ETH_TSPTPPSV2E (1 << 10)
/* When this bit is set, the PTP packets are snooped using the version 2
 * format. When the bit is cleared, the PTP packets are snooped using the
 * version 1 format.
 * Note: IEEE 1588 Version 1 and Version 2 formats as indicated in IEEE
 * standard 1588-2008 (Revision of IEEE STD. 1588-2002).
 */

/* Bit 9 - Time stamp subsecond rollover: digital or binary rollover control */
#define ETH_TSSSR (1 << 9)
/* When this bit is set, the Time stamp low register rolls over when the
 * subsecond counter reaches the value 0x3B9A C9FF (999 999 999 in decimal),
 * and increments the Time Stamp (high) seconds.
 * When this bit is cleared, the rollover value of the subsecond register
 * reaches 0x7FFF FFFF.
 * The subsecond increment has to be programmed correctly depending on the
 * PTP’s reference clock frequency and this bit value.
 */

/* Bit 8 - Time stamp snapshot for all received frames enable */
#define ETH_TSSARFE (1 << 8)
/* When this bit is set, the time stamp snapshot is enabled for all frames
 * received by the core.
 */

/* [7..6] Reserved */


/* Bit 5 - Time stamp addend register update */
#define ETH_TSARU (1 << 5)
/* When this bit is set, the Time stamp addend register’s contents are
 * updated to the PTP block for fine correction. This bit is cleared when the
 * update is complete. This register bit must be
 * read as zero before you can set it.
 */


/* Bit 4 - Time stamp interrupt trigger enable */
#define ETH_TSITE (1 << 4)
/* When this bit is set, a time stamp interrupt is generated when the system
 * time becomes greater than the value written in the Target time register.
 *  When the Time stamp trigger interrupt is generated, this bit is cleared.
 */

/* Bit 3 - Time stamp system time update */
#define ETH_TSSTU (1 << 3)
/* 
When this bit is set, the system time is updated (added to or subtracted from) with the value
specified in the Time stamp high update and Time stamp low update registers. Both the
TSSTU and TSSTI bits must be read as zero before you can set this bit. Once the update is
completed in hardware, this bit is cleared. */

/* Bit 2 - Time stamp system time initialize */
#define ETH_TSSTI (1 << 2)
/* 
When this bit is set, the system time is initialized (overwritten) with the value specified in the
Time stamp high update and Time stamp low update registers. This bit must be read as zero
before you can set it. When initialization is complete, this bit is cleared. */

/* Bit 1 - Time stamp fine or coarse update */
#define ETH_TSFCU (1 << 1)
/* 
When set, this bit indicates that the system time stamp is to be updated using the Fine Update
method. When cleared, it indicates the system time stamp is to be updated using the Coarse
method. */

/* Bit 0 - Time stamp enable */
#define ETH_TSE (1 << 0)
/* 
When this bit is set, time stamping is enabled for transmit and receive frames. When this bit is
cleared, the time stamp function is suspended and time stamps are not added for transmit and
receive frames. Because the maintained system time is suspended, you must always initialize
the time stamp feature (system time) after setting this bit high. */

/* Ethernet PTP subsecond increment register */
#define STM32F_ETH_PTPSSIR 0x0704



/* [31..8] Reserved */


/* Bits [7..0] - System time subsecond increment */
#define ETH_STSSI ((7 - 0) << 0)
/* 
The value programmed in this register is added to the contents of the subsecond value of the
system time in every update.
For example, to achieve 20 ns accuracy, the value is: 20 / 0.467 = ~ 43 (or 0x2A). */

/* Ethernet PTP time stamp high register */
#define STM32F_ETH_PTPTSHR 0x0708


/* Bits [31..0] - System time second */
#define ETH_STS ((31 - 0) << 0)
/* 
The value in this field indicates the current value in seconds of the System Time maintained by
the core. */

/* Ethernet PTP time stamp low register */
#define STM32F_ETH_PTPTSLR 0x070C



/* Bit 31 - System time positive or negative sign */
#define ETH_STPNS (1 << 31)
/* 
This bit indicates a positive or negative time value. When set, the bit indicates that time
representation is negative. When cleared, it indicates that time representation is positive.
Because the system time should always be positive, this bit is normally zero. */

/* Bits [30..0] - System time subseconds */
#define ETH_STSS ((30 - 0) << 0)
/* 
The value in this field has the subsecond time representation, with 0.46 ns accuracy. */

/* Ethernet PTP time stamp high update register */
#define STM32F_ETH_PTPTSHUR 0x0710



/* Bits [31..0] - Time stamp update second */
#define ETH_TSUS ((31 - 0) << 0)
/* 
The value in this field indicates the time, in seconds, to be initialized or added to the system
time. */

/* Ethernet PTP time stamp low update register */
#define STM32F_ETH_PTPTSLUR 0x0714



/* Bit 31 - Time stamp update positive or negative sign */
#define ETH_TSUPNS (1 << 31)
/* 
This bit indicates positive or negative time value. When set, the bit indicates that time
representation is negative. When cleared, it indicates that time representation is positive.
When TSSTI is set (system time initialization) this bit should be zero. If this bit is set when
TSSTU is set, the value in the Time stamp update registers is subtracted from the system
time. Otherwise it is added to the system time. */

/* Bits [30..0] - Time stamp update subseconds */
#define ETH_TSUSS ((30 - 0) << 0)
/* 
The value in this field indicates the subsecond time to be initialized or added to the system
time. This value has an accuracy of 0.46 ns (in other words, a value of 0x0000_0001 is
0.46 ns). */

/* Ethernet PTP time stamp addend register */
#define STM32F_ETH_PTPTSAR 0x0718



/* Bits [31..0] - Time stamp addend */
#define ETH_TSA ((31 - 0) << 0)
/* 
This register indicates the 32-bit time value to be added to the Accumulator register to achieve
time synchronization. */

/* Ethernet PTP target time high register */
#define STM32F_ETH_PTPTTHR 0x071C



/* Bits [31..0] - Target time stamp high */
#define ETH_TTSH ((31 - 0) << 0)
/* 
This register stores the time in seconds. When the time stamp value matches or exceeds both
Target time stamp registers, the MAC, if enabled, generates an interrupt. */

/* Ethernet PTP target time low register */
#define STM32F_ETH_PTPTTLR 0x0720



/* Bits [31..0] - Target time stamp low */
#define ETH_TTSL ((31 - 0) << 0)
/* This register stores the time in (signed) nanoseconds. When the value of
 * the time stamp matches or exceeds both Target time stamp registers, the
 * MAC, if enabled, generates an interrupt. */


/* Ethernet PTP time stamp status register */
#define STM32F_ETH_PTPTSSR 0x0728


/* Bit 1 - Time stamp target time reached */
#define ETH_TSTTR (1 << 1)
/* When set, this bit indicates that the value of the system time is greater
 * than or equal to the value specified in the Target time high and low
 * registers
 */

/* Bit 0 - Time stamp second overflow */
#define ETH_TSSO (1 << 0)
/* When set, this bit indicates that the second value of the time stamp has
 * overflowed beyond 0xFFFF FFFF.
 */

/* Ethernet PTP PPS control register */
#define STM32F_ETH_PTPPPSCR 0x072C



/* [31..4] Reserved */


/* Bits [3..0] - PPS frequency selection */
#define ETH_PPSFREQ ((3 - 0) << 0)
/* 
The PPS output frequency is set to 2PPSFREQ Hz.
0000: 1 Hz with a pulse width of 125 ms for binary rollover and, of 100 ms for digital rollover
0001: 2 Hz with 50% duty cycle for binary rollover (digital rollover not recommended)
0010: 4 Hz with 50% duty cycle for binary rollover (digital rollover not recommended)
0011: 8 Hz with 50% duty cycle for binary rollover (digital rollover not recommended)
0100: 16 Hz with 50% duty cycle for binary rollover (digital rollover not recommended)
...
1111: 32768 Hz with 50% duty cycle for binary rollover (digital rollover not recommended)
Note: If digital rollover is used (TSSSR=1, bit 9 in ETH_PTPTSCR), it is recommended not to
use the PPS output with a frequency other than 1 Hz. Otherwise, with digital rollover,
the PPS output has irregular waveforms at higher frequencies (though its average
frequency will always be correct during any one-second window). */


/*-------------------------------------------------------------------------
 * Ethernet DMA bus mode register 
 */
#define STM32F_ETH_DMABMR 0x1000


/* Bits 31:267 Reserved */

/* Bit 26 - Mixed burst */
#define ETH_MB (1 << 26)
/* When this bit is set high and the FB bit is low, the AHB master interface 
   starts all bursts of a length greater than 16 with INCR (undefined burst). 
   When this bit is cleared, it reverts to fixed burst transfers 
   (INCRx and SINGLE) for burst lengths of 16 and below. */

/* Bit 25 - Address-aligned beats */
#define ETH_AAB (1 << 25)
/* When this bit is set high and the FB bit equals 1, the AHB interface 
   generates all bursts aligned to the start address LS bits. If the FB bit 
   equals 0, the first burst (accessing the data buffer’s start address) is 
   not aligned, but subsequent bursts are aligned to the address. */

/* Bit 24 - 4xPBL mode */
#define ETH_FPM (1 << 24)
/* When set high, this bit multiplies the PBL value programmed 
   (bits [22:17] and bits [13:8]) four times. Thus the DMA transfers data in 
   a maximum of 4, 8, 16, 32, 64 and 128 beats depending on the PBL value. */

/* Bit 23 - Use separate PBL */
#define ETH_USP (1 << 23)
/* When set high, it configures the RxDMA to use the value configured in 
   bits [22:17] as PBL while the PBL value in bits [13:8] is applicable to 
   TxDMA operations only.
   When this bit is cleared, the PBL value in bits [13:8] is applicable 
   for both DMA engines. */

/* Bits [22..17] - Rx DMA PBL */
#define ETH_RDP ((22 - 17) << 17)
#define ETH_RDP_MSK (((1 << (5 + 1)) - 1) << 17)
#define ETH_RDP_SET(VAL) (((VAL) << 17) & ETH_RDP_MSK)
#define ETH_RDP_GET(REG) (((REG) & ETH_RDP_MSK) >> 17)
/* These bits indicate the maximum number of beats to be transferred in one 
   RxDMA transaction.
   This is the maximum value that is used in a single block read/write 
   operation. The RxDMA always attempts to burst as specified in RDP each 
   time it starts a burst transfer on the host bus. RDP can be programmed 
   with permissible values of 1, 2, 4, 8, 16, and 32. Any other value
   results in undefined behavior.
   These bits are valid and applicable only when USP is set high. */

/* Bit 16 - Fixed burst */
#define ETH_FB (1 << 16)
/* This bit controls whether the AHB Master interface performs fixed burst 
   transfers or not.
   When set, the AHB uses only SINGLE, INCR4, INCR8 or INCR16 during start 
   of normal burst transfers. 
   When reset, the AHB uses SINGLE and INCR burst transfer operations. */

/* Bits [15..14] - Rx Tx priority ratio */
#define ETH_RTPR ((15 - 14) << 14)
#define ETH_RTPR_MSK (((1 << (1 + 1)) - 1) << 14)
#define ETH_RTPR_SET(VAL) (((VAL) << 14) & ETH_RTPR_MSK)
#define ETH_RTPR_GET(REG) (((REG) & ETH_RTPR_MSK) >> 14)
/* RxDMA requests are given priority over TxDMA requests in 
   the following ratio:
   00: 1:1
   01: 2:1
   10: 3:1
   11: 4:1
   This is valid only when the DA bit is cleared. */

/* Bits [13..8] - Programmable burst length */
#define ETH_PBL ((13 - 8) << 8)
#define ETH_BPL_MSK (((1 << (5 + 1)) - 1) << 8)
#define ETH_BPL_SET(VAL) (((VAL) << 8) & ETH_BPL_MSK)
#define ETH_BPL_GET(REG) (((REG) & ETH_BPL_MSK) >> 8)
/* These bits indicate the maximum number of beats to be transferred in 
   one DMA transaction. 
   This is the maximum value that is used in a single block read/write 
   operation. The DMA always attempts to burst as specified in PBL each 
   time it starts a burst transfer on the host bus. PBL can be 
   programmed with permissible values of 1, 2, 4, 8, 16, and 32. Any 
   other value results in undefined behavior. When USP is set, this PBL 
   value is applicable for TxDMA transactions only.
   The PBL values have the following limitations:
   – The maximum number of beats (PBL) possible is limited by the size 
   of the Tx FIFO and Rx FIFO.
   – The FIFO has a constraint that the maximum beat supported is half 
   the depth of the FIFO.
   – If the PBL is common for both transmit and receive DMA, the minimum 
   Rx FIFO and Tx FIFO depths must be considered.
   – Do not program out-of-range PBL values, because the system may 
   not behave properly. */

/* Bit 7 - Enhanced descriptor format enable */
#define ETH_EDFE (1 << 7)
/* When this bit is set, the enhanced descriptor format is enabled and the 
   descriptor size is increased to 32 bytes (8 DWORDS). This is required when 
   time stamping is activated (TSE=1, ETH_PTPTSCR bit 0) or if IPv4 checksum 
   offload is activated (IPCO=1, ETH_MACCR bit 10). */

/* Bit 7 Reserved */


/* Bits [6..2] - Descriptor skip length */
#define ETH_DSL ((6 - 2) << 2)
#define ETH_DSL_MSK (((1 << (4 + 1)) - 1) << 2)
#define ETH_DSL_SET(VAL) (((VAL) << 2) & ETH_DSL_MSK)
#define ETH_DSL_GET(REG) (((REG) & ETH_DSL_MSK) >> 2)
/* This bit specifies the number of words to skip between two unchained 
   descriptors. The address skipping starts from the end of current descriptor 
   to the start of next descriptor. When DSL value equals zero, the descriptor 
   table is taken as contiguous by the DMA, in Ring mode. */

/* Bit 1 - DMA Arbitration */
#define ETH_DA (1 << 1)
/* 0: Round-robin with Rx:Tx priority given in bits [15:14]
   1: Rx has priority over Tx */

/* Bit 0 - Software reset */
#define ETH_SRST (1 << 0)
/* When this bit is set, the MAC DMA controller resets all MAC Subsystem 
   internal registers and logic. It is cleared automatically after the reset 
   operation has completed in all of the core clock domains. Read a 0 value 
   in this bit before re-programming any register of the core. */

/*-------------------------------------------------------------------------
 * Ethernet DMA transmit poll demand register 
 */
#define STM32F_ETH_DMATPDR 0x1004

/* Bits [31..0] - Transmit poll demand */
#define ETH_TPD ((31 - 0) << 0)
/* When these bits are written with any value, the DMA reads the current 
   descriptor pointed to by the ETH_DMACHTDR register. If that descriptor is 
   not available (owned by Host), transmission returns to the Suspend state 
   and ETH_DMASR register bit 2 is asserted. If the descriptor is available, 
   transmission resumes. */


/*-------------------------------------------------------------------------
 * EHERNET DMA receive poll demand register
 */
#define STM32F_ETH_DMARPDR 0x1008

/* Bits [31..0] - Receive poll demand */
#define ETH_RPD ((31 - 0) << 0)
/* When these bits are written with any value, the DMA reads the current 
   descriptor pointed to by the ETH_DMACHRDR register. If that descriptor 
   is not available (owned by Host), reception returns to the Suspended 
   state and ETH_DMASR register bit 7 is not asserted. If the descriptor 
   is available, the Receive DMA returns to active state. */


/*-------------------------------------------------------------------------
 * Ethernet DMA receive descriptor list address register 
 */
#define STM32F_ETH_DMARDLAR 0x100C

/* Bits [31..0] - Start of receive list */
#define ETH_SRL ((31 - 0) << 0)
/* This field contains the base address of the first descriptor in the 
   receive descriptor list. The LSB bits [1/2/3:0] for 32/64/128-bit 
   bus width) are internally ignored and taken as all-zero by the DMA. 
   Hence these LSB bits are read only. */


/*-------------------------------------------------------------------------
 * Ethernet DMA transmit descriptor list address register 
 */
#define STM32F_ETH_DMATDLAR 0x1010

/* Bits [31..0] - Start of transmit list */
#define ETH_STL ((31 - 0) << 0)
/* This field contains the base address of the first descriptor in the 
   transmit descriptor list. The LSB bits [1/2/3:0] for 32/64/128-bit 
   bus width) are internally ignored and taken as all-zero by the DMA. 
   Hence these LSB bits are read-only. */


/*-------------------------------------------------------------------------
 * Ethernet DMA status register
 */
#define STM32F_ETH_DMASR 0x1014

/* [31..30] Reserved */

/* Bit 29 - Time stamp trigger status */
#define ETH_DMA_TSTS (1 << 29)
/* This bit indicates an interrupt event in the MAC core's Time stamp 
 * generator block. The software must read the MAC core’s status 
 * register, clearing its source (bit 9), to reset this bit to 0.
 * When this bit is high an interrupt is generated if enabled. */

/* Bit 28 - PMT status */
#define ETH_DMA_PMTS (1 << 28)
/* This bit indicates an event in the MAC core’s PMT. The software must 
 * read the corresponding registers in the MAC core to get the exact cause 
 * of interrupt and clear its source to reset this bit to 0. The interrupt 
 * is generated when this bit is high if enabled. */

/* Bit 27 - MMC status */
#define ETH_DMA_MMCS (1 << 27)
/* This bit reflects an event in the MMC of the MAC core. The software must 
 * read the corresponding registers in the MAC core to get the exact cause 
 * of interrupt and clear the source of interrupt to make this bit as 0. 
 * The interrupt is generated when this bit is high if enabled. */

/* Bit 26 Reserved */


/* Bits [25..23] - Error bits status */
#define ETH_EBS (((1 << (25 - 23 + 1)) - 1) << 23)
#define ETH_EBS_MSK (((1 << (25 - 23 + 1)) - 1) << 23)
#define ETH_EBS_SET(VAL) (((VAL) << 23) & ETH_EBS_MSK)
#define ETH_EBS_GET(REG) (((REG) & ETH_EBS_MSK) >> 23)
/* These bits indicate the type of error that caused a bus error (error 
 * response on the AHB interface). Valid only with the fatal bus error 
 * bit (ETH_DMASR register [13]) set. This field does not generate an 
 * interrupt.
 * Bit 23
 * 1 - Error during data transfer by TxDMA
 * 0 - Error during data transfer by RxDMA
 * Bit 24
 * 1 - Error during read transfer
 * 0 - Error during write transfer
 * Bit 25
 * 1 - Error during descriptor access
 * 0 - Error during data buffer access */

/* Bits [22..20] - Transmit process state */
#define ETH_TPS ((22 - 20) << 20)
/* These bits indicate the Transmit DMA FSM state. This field does not 
 * generate an interrupt.
 * 000: Stopped; Reset or Stop Transmit Command issued
 * 001: Running; Fetching transmit transfer descriptor
 * 010: Running; Waiting for status
 * 011: Running; Reading Data from host memory buffer and queuing it to 
 * transmit buffer (Tx FIFO)
 * 100, 101: Reserved for future use
 * 110: Suspended; Transmit descriptor unavailable or transmit buffer underflow
 * 111: Running; Closing transmit descriptor */

/* Bits [19..17] - Receive process state */
#define ETH_RPS (((1 << (19 - 17 + 1)) - 1) << 17)
#define ETH_RPS_MSK (((1 << (19 - 17 + 1)) - 1) << 17)
#define ETH_RPS_SET(VAL) (((VAL) << 17) & ETH_RPS_MSK)
#define ETH_RPS_GET(REG) (((REG) & ETH_RPS_MSK) >> 17)
/* These bits indicate the Receive DMA FSM state. This field does not 
 * generate an interrupt.
 * 000: Stopped: Reset or Stop Receive Command issued
 * 001: Running: Fetching receive transfer descriptor
 * 010: Reserved for future use
 * 011: Running: Waiting for receive packet
 * 100: Suspended: Receive descriptor unavailable
 * 101: Running: Closing receive descriptor
 * 110: Reserved for future use
 * 111: Running: Transferring the receive packet data from receive buffer to 
 * host memory */

/* Bit 16 - Normal interrupt summary */
#define ETH_NIS (1 << 16)
/* The normal interrupt summary bit value is the logical OR of the 
 * following when the corresponding interrupt bits are enabled in the 
 * ETH_DMAIER register:
 * – ETH_DMASR [0]: Transmit interrupt
 * – ETH_DMASR [2]: Transmit buffer unavailable
 * – ETH_DMASR [6]: Receive interrupt
 * – ETH_DMASR [14]: Early receive interrupt
 * Only unmasked bits affect the normal interrupt summary bit.
 * This is a sticky bit and it must be cleared (by writing a 1 to this 
 * bit) each time a corresponding bit that causes NIS to be set is cleared. */

/* Bit 15 - Abnormal interrupt summary */
#define ETH_AIS (1 << 15)
/* The abnormal interrupt summary bit value is the logical OR of the 
 * following when the corresponding interrupt bits are enabled in the 
 * ETH_DMAIER register:
 * – ETH_DMASR [1]: Transmit process stopped
 * – ETH_DMASR [3]: Transmit jabber timeout
 * – ETH_DMASR [4]: Receive FIFO overflow
 * – ETH_DMASR [5]: Transmit underflow
 * – ETH_DMASR [7]: Receive buffer unavailable
 * – ETH_DMASR [8]: Receive process stopped
 * – ETH_DMASR [9]: Receive watchdog timeout
 * – ETH_DMASR [10]: Early transmit interrupt
 * – ETH_DMASR [13]: Fatal bus error
 * Only unmasked bits affect the abnormal interrupt summary bit.
 * This is a sticky bit and it must be cleared each time a corresponding 
 * bit that causes AIS to be set is cleared. */

/* Bit 14 - Early receive status */
#define ETH_ERS (1 << 14)
/* This bit indicates that the DMA had filled the first data buffer of 
 * the packet. Receive Interrupt ETH_DMASR [6] automatically clears 
 * this bit. */

/* Bit 13 - Fatal bus error status */
#define ETH_FBES (1 << 13)
/* This bit indicates that a bus error occurred, as detailed in [25:23]. 
 * When this bit is set, the corresponding DMA engine disables all its 
 * bus accesses. */

/* [12..11] Reserved */

/* Bit 10 - Early transmit status */
#define ETH_ETS (1 << 10)
/* This bit indicates that the frame to be transmitted was fully 
 * transferred to the Transmit FIFO. */

/* Bit 9 - Receive watchdog timeout status */
#define ETH_RWTS (1 << 9)
/* This bit is asserted when a frame with a length greater than 
 * 2 048 bytes is received. */

/* Bit 8 - Receive process stopped status */
#define ETH_RPSS (1 << 8)
/* This bit is asserted when the receive process enters the Stopped state. */

/* Bit 7 - Receive buffer unavailable status */
#define ETH_RBUS (1 << 7)
/* This bit indicates that the next descriptor in the receive list is owned 
 * by the host and cannot be acquired by the DMA. Receive process 
 * is suspended. To resume processing receive descriptors, the host should 
 * change the ownership of the descriptor and issue a Receive Poll Demand 
 * command. If no Receive Poll Demand is issued, receive process resumes 
 * when the next recognized incoming frame is received. ETH_DMASR [7] is 
 * set only when the previous receive descriptor was owned by the DMA. */

/* Bit 6 - Receive status */
#define ETH_RS (1 << 6)
/* This bit indicates the completion of the frame reception. Specific frame 
 * status information has been posted in the descriptor. Reception 
 * remains in the Running state. */

/* Bit 5 - Transmit underflow status */
#define ETH_TUS (1 << 5)
/* This bit indicates that the transmit buffer had an underflow during frame
 * transmission. Transmission is suspended and an underflow error
 * TDES0[1] is set.
 */

/* Bit 4 - Receive overflow status */
#define ETH_ROS (1 << 4)
/* This bit indicates that the receive buffer had an overflow during frame
 * reception. If the partial frame is transferred to the application, the
 * overflow status is set in RDES0[11].
 */

/* Bit 3 - Transmit jabber timeout status */
#define ETH_TJTS (1 << 3)
/* This bit indicates that the transmit jabber timer expired, meaning that
 * the transmitter had been excessively active. The transmission process is
 * aborted and placed in the Stopped state. This causes the transmit jabber
 * timeout TDES0[14] flag to be asserted.
 */

/* Bit 2 - Transmit buffer unavailable status */
#define ETH_TBUS (1 << 2)
/* This bit indicates that the next descriptor in the transmit list is owned
 * by the host and cannot be acquired by the DMA. Transmission is suspended.
 * Bits [22:20] explain the transmit process state transitions. To resume
 * processing transmit descriptors, the host should change the ownership of
 * the bit of the descriptor and then issue a Transmit Poll Demand command.
 */

/* Bit 1 - Transmit process stopped status */
#define ETH_TPSS (1 << 1)
/* This bit is set when the transmission is stopped.
 */

/* Bit 0 - Transmit status */
#define ETH_TS (1 << 0)
/* This bit indicates that frame transmission is finished and TDES1[31] is
 * set in the first descriptor.
 */

/*-------------------------------------------------------------------------
 * Ethernet DMA operation mode register 
 */
#define STM32F_ETH_DMAOMR 0x1018


/* [31..27] Reserved */


/* Bit 26 - Dropping of TCP/IP checksum error frames disable */
#define ETH_DTCEFD (1 << 26)
/* When this bit is set, the core does not drop frames that only have errors
 * detected by the receive checksum offload engine. Such frames do not have
 * any errors (including FCS error) in the Ethernet frame received by the
 * MAC but have errors in the encapsulated payload only.
 * When this bit is cleared, all error frames are dropped if the FEF bit
 * is reset.
 */

/* Bit 25 - Receive store and forward */
#define ETH_RSF (1 << 25)
/* When this bit is set, a frame is read from the Rx FIFO after the 
   complete frame has been written to it, ignoring RTC bits. 
   When this bit is cleared, the Rx FIFO operates in Cut-through mode, 
   subject to the threshold specified by the RTC bits. */

/* Bit 24 - Disable flushing of received frames */
#define ETH_DFRF (1 << 24)
/* When this bit is set, the RxDMA does not flush any frames due to the 
 * unavailability of receive descriptors/buffers as it does normally when 
 * this bit is cleared. (See Receive process suspended on page 860) */

/* [23..22] Reserved */


/* Bit 21 - Transmit store and forward */
#define ETH_TSF (1 << 21)
/* When this bit is set, transmission starts when a full frame resides in 
 * the Transmit FIFO. When this bit is set, the TTC values specified by 
 * the ETH_DMAOMR register bits [16:14] are ignored. When this bit is 
 * cleared, the TTC values specified by the ETH_DMAOMR register 
 * bits [16:14] are taken into account.
 * This bit should be changed only when transmission is stopped. */

/* Bit 20 - Flush transmit FIFO */
#define ETH_FTF (1 << 20)
/* When this bit is set, the transmit FIFO controller logic is reset to its 
 * default values and thus all data in the Tx FIFO are lost/flushed. This 
 * bit is cleared internally when the flushing operation is complete. 
 * The Operation mode register should not be written to until this 
 * bit is cleared. */

/* [19..17] Reserved */


/* Bits [16..14] - Transmit threshold control */
#define ETH_TTC ((16 - 14) << 14)
/* These three bits control the threshold level of the Transmit FIFO. 
 * Transmission starts when the frame size within the Transmit FIFO is 
 * larger than the threshold. In addition, full frames with a length less 
 * than the threshold are also transmitted. These bits are used only when 
 * the TSF bit (Bit 21) is cleared.
 * 000: 64
 * 001: 128
 * 010: 192
 * 011: 256
 * 100: 40
 * 101: 32
 * 110: 24
 * 111: 16 */

/* Bit 13 - Start/stop transmission */
#define ETH_ST (1 << 13)
/* When this bit is set, transmission is placed in the Running state, and 
 * the DMA checks the transmit list at the current position for a frame 
 * to be transmitted. Descriptor acquisition is attempted either from 
 * the current position in the list, which is the transmit list base 
 * address set by the ETH_DMATDLAR register, or from the position retained 
 * when transmission was stopped previously. If the current descriptor is 
 * not owned by the DMA, transmission enters the Suspended state and the 
 * transmit buffer unavailable bit (ETH_DMASR [2]) is set. The Start 
 * Transmission command is effective only when transmission is stopped. If 
 * the command is issued before setting the DMA ETH_DMATDLAR register, the 
 * DMA behavior is unpredictable.
 * When this bit is cleared, the transmission process is placed in the 
 * Stopped state after completing the transmission of the current frame. 
 * The next descriptor position in the transmit list is saved, and becomes 
 * the current position when transmission is restarted. The Stop 
 * Transmission command is effective only when the transmission of the 
 * current frame is complete or when the transmission is in the 
 * Suspended state. */

/* [12..8] Reserved */


/* Bit 7 - Forward error frames */
#define ETH_FEF (1 << 7)
/* When this bit is set, all frames except runt error frames are forwarded 
 * to the DMA.
 * When this bit is cleared, the Rx FIFO drops frames with error status 
 * (CRC error, collision error, giant frame, watchdog timeout, overflow). 
 * However, if the frame’s start byte (write) pointer is already 
 * transferred to the read controller side (in Threshold mode), then the 
 * frames are not dropped. The Rx FIFO drops the error frames if that 
 * frame's start byte is not transferred (output) on the ARI bus. */

/* Bit 6 - Forward undersized good frames */
#define ETH_FUGF (1 << 6)
/* When this bit is set, the Rx FIFO forwards undersized frames (frames \
 * with no error and length less than 64 bytes) including pad-bytes and CRC).
 * When this bit is cleared, the Rx FIFO drops all frames of less than 
 * 64 bytes, unless such a frame has already been transferred due to lower 
 * value of receive threshold (e.g., RTC = 01). */

/* Bit 5 Reserved */


/* Bits [4..3] - Receive threshold control */
#define ETH_RTC ((4 - 3) << 3)
/* These two bits control the threshold level of the Receive FIFO. Transfer 
 * (request) to DMA starts when the frame size within the Receive FIFO 
 * is larger than the threshold. In addition, full frames with a length 
 * less than the threshold are transferred automatically.
 * Note: Note that value of 11 is not applicable if the configured Receive 
 * FIFO size is 128 bytes.
 * Note: These bits are valid only when the RSF bit is zero, and are 
 * ignored when the RSF bit is set to 1.
 * 00: 64
 * 01: 32
 * 10: 96
 * 11: 128 */

/* Bit 2 - Operate on second frame */
#define ETH_OSF (1 << 2)
/* When this bit is set, this bit instructs the DMA to process a second
 * frame of Transmit data even before status for first frame is obtained.
 */

/* Bit 1 - Start/stop receive */
#define ETH_SR (1 << 1)
/* When this bit is set, the receive process is placed in the Running state.
 * The DMA attempts to acquire the descriptor from the receive list and
 * processes incoming frames. Descriptor acquisition is attempted from the
 * current position in the list, which is the address set by the
 * DMA ETH_DMARDLAR register or the position retained when the receive process
 * was previously stopped. If no descriptor is owned by the DMA, reception is
 * suspended and the receive buffer unavailable bit (ETH_DMASR [7]) is set.
 * The Start Receive command is effective only when reception has stopped.
 * If the command was issued before setting the DMA ETH_DMARDLAR register,
 * the DMA behavior is unpredictable.
 * When this bit is cleared, RxDMA operation is stopped after the transfer of
 * the current frame. The next descriptor position in the receive list is
 * saved and becomes the current position when the receive process is
 * restarted. The Stop Receive command is effective only when the Receive
 * process is in either the Running (waiting for receive packet) or
 * the Suspended state.
 */

/* Bit 0 Reserved */

/* Ethernet DMA interrupt enable register */
#define STM32F_ETH_DMAIER 0x101C


/* [31..17] Reserved */

/* Bit 16 - Normal interrupt summary enable */
#define ETH_NISE (1 << 16)
/* When this bit is set, a normal interrupt is enabled. When this bit is 
   cleared, a normal interrupt is disabled. This bit enables 
   the following bits:
   – ETH_DMASR [0]: Transmit Interrupt
   – ETH_DMASR [2]: Transmit buffer unavailable
   – ETH_DMASR [6]: Receive interrupt
   – ETH_DMASR [14]: Early receive interrupt */

/* Bit 15 - Abnormal interrupt summary enable */
#define ETH_AISE (1 << 15)
/* When this bit is set, an abnormal interrupt is enabled. When this 
   bit is cleared, an abnormal interrupt is disabled. This bit enables 
   the following bits:
   – ETH_DMASR [1]: Transmit process stopped
   – ETH_DMASR [3]: Transmit jabber timeout
   – ETH_DMASR [4]: Receive overflow
   – ETH_DMASR [5]: Transmit underflow
   – ETH_DMASR [7]: Receive buffer unavailable
   – ETH_DMASR [8]: Receive process stopped
   – ETH_DMASR [9]: Receive watchdog timeout
   – ETH_DMASR [10]: Early transmit interrupt
   – ETH_DMASR [13]: Fatal bus error */

/* Bit 14 - Early receive interrupt enable */
#define ETH_ERIE (1 << 14)
/* When this bit is set with the normal interrupt summary enable bit 
   (ETH_DMAIER register[16]), the early receive interrupt is enabled.
   When this bit is cleared, the early receive interrupt is disabled. */

/* Bit 13 - Fatal bus error interrupt enable */
#define ETH_FBEIE (1 << 13)
/* When this bit is set with the abnormal interrupt summary enable bit 
   (ETH_DMAIER register[15]), the fatal bus error interrupt is enabled.
   When this bit is cleared, the fatal bus error enable interrupt 
   is disabled. */

/* [12..11] Reserved */


/* Bit 10 - Early transmit interrupt enable */
#define ETH_ETIE (1 << 10)
/* When this bit is set with the abnormal interrupt summary enable bit 
   (ETH_DMAIER register [15]), the early transmit interrupt is enabled.
   When this bit is cleared, the early transmit interrupt is disabled. */

/* Bit 9 - receive watchdog timeout interrupt enable */
#define ETH_RWTIE (1 << 9)
/* When this bit is set with the abnormal interrupt summary enable bit 
   (ETH_DMAIER register[15]), the receive watchdog timeout interrupt 
   is enabled.
   When this bit is cleared, the receive watchdog timeout interrupt 
   is disabled. */

/* Bit 8 - Receive process stopped interrupt enable */
#define ETH_RPSIE (1 << 8)
/* When this bit is set with the abnormal interrupt summary enable bit 
   (ETH_DMAIER register[15]), the receive stopped interrupt is enabled. 
   When this bit is cleared, the receive stopped interrupt is disabled. */

/* Bit 7 - Receive buffer unavailable interrupt enable */
#define ETH_RBUIE (1 << 7)
/* When this bit is set with the abnormal interrupt summary enable bit 
   (ETH_DMAIER register[15]), the receive buffer unavailable interrupt is 
   enabled.
   When this bit is cleared, the receive buffer unavailable interrupt 
   is disabled. */

/* Bit 6 - Receive interrupt enable */
#define ETH_RIE (1 << 6)
/* When this bit is set with the normal interrupt summary enable bit 
   (ETH_DMAIER register[16]), the receive interrupt is enabled.
   When this bit is cleared, the receive interrupt is disabled. */

/* Bit 5 - Underflow interrupt enable */
#define ETH_TUIE (1 << 5)
/* When this bit is set with the abnormal interrupt summary enable bit 
   (ETH_DMAIER register[15]), the transmit underflow interrupt is enabled.
   When this bit is cleared, the underflow interrupt is disabled. */

/* Bit 4 - Overflow interrupt enable */
#define ETH_ROIE (1 << 4)
/* When this bit is set with the abnormal interrupt summary enable bit 
   (ETH_DMAIER register[15]), the receive overflow interrupt is enabled.
   When this bit is cleared, the overflow interrupt is disabled. */

/* Bit 3 - Transmit jabber timeout interrupt enable */
#define ETH_TJTIE (1 << 3)
/* When this bit is set with the abnormal interrupt summary enable bit 
   (ETH_DMAIER register[15]), the transmit jabber timeout interrupt 
   is enabled.
   When this bit is cleared, the transmit jabber timeout 
   interrupt is disabled. */

/* Bit 2 - Transmit buffer unavailable interrupt enable */
#define ETH_TBUIE (1 << 2)
/* When this bit is set with the normal interrupt summary enable 
   bit (ETH_DMAIER register[16]), the transmit buffer unavailable interrupt 
   is enabled.
   When this bit is cleared, the transmit buffer unavailable 
   interrupt is disabled. */

/* Bit 1 - Transmit process stopped interrupt enable */
#define ETH_TPSIE (1 << 1)
/* When this bit is set with the abnormal interrupt summary enable bit 
   (ETH_DMAIER register[15]), the transmission stopped interrupt is 
   enabled.
   When this bit is cleared, the transmission stopped interrupt is disabled. */

/* Bit 0 - Transmit interrupt enable */
#define ETH_TIE (1 << 0)
/* When this bit is set with the normal interrupt summary enable bit 
   (ETH_DMAIER register[16]), the transmit interrupt is enabled.
   When this bit is cleared, the transmit interrupt is disabled. */

/* Ethernet DMA missed frame and buffer overflow counter register */
#define STM32F_ETH_DMAMFBOCR 0x1020

/* [31..29] Reserved */

/* Bit 28 - Overflow bit for FIFO overflow counter */
#define ETH_OFOC (1 << 28)

/* Bits [27..17] - Missed frames by the application */
#define ETH_MFA ((27 - 17) << 17)
/* Indicates the number of frames missed by the application */

/* Bit 16 - Overflow bit for missed frame counter */
#define ETH_OMFC (1 << 16)

/* Bits [15..0] - Missed frames by the controller */
#define ETH_MFC ((15 - 0) << 0)
/* Indicates the number of frames missed by the Controller due to the 
   host receive buffer being unavailable. This counter is incremented 
   each time the DMA discards an incoming frame. */


/* Ethernet DMA receive status watchdog timer register */
#define STM32F_ETH_DMARSWTR 0x1024

/* [31..8] Reserved */

/* Bits [7..0] - Receive status (RS) watchdog timer count */
#define ETH_RSWTC ((7 - 0) << 0)
/* Indicates the number of HCLK clock cycles multiplied by 256 for which 
   the watchdog timer is set. The watchdog timer gets triggered with the 
   programmed value after the RxDMA completes the transfer of a frame for 
   which the RS status bit is not set due to the setting of RDES1[31] in 
   the corresponding descriptor. When the watchdog timer runs out, the RS 
   bit is set and the timer is stopped. The watchdog timer is reset when 
   the RS bit is set high due to automatic setting of RS as per 
   RDES1[31] of any received frame. */

/* Ethernet DMA current host transmit descriptor register */
#define STM32F_ETH_DMACHTDR 0x1048

/* Bits [31..0] - Host transmit descriptor address pointer */
#define ETH_HTDAP ((31 - 0) << 0)
/* Cleared on reset. Pointer updated by DMA during operation. */


/* Ethernet DMA current host receive descriptor register */
#define STM32F_ETH_DMACHRDR 0x104C

/* Bits [31..0] - Host receive descriptor address pointer */
#define ETH_HRDAP ((31 - 0) << 0)
/* Cleared on Reset. Pointer updated by DMA during operation. */


/* Ethernet DMA current host transmit buffer address register */
#define STM32F_ETH_DMACHTBAR 0x1050

/* Bits [31..0] - Host transmit buffer address pointer */
#define ETH_HTBAP ((31 - 0) << 0)
/* Cleared on reset. Pointer updated by DMA during operation. */


/* Ethernet DMA current host receive buffer address register */
#define STM32F_ETH_DMACHRBAR 0x1054

/* Bits [31..0] - Host receive buffer address pointer */
#define ETH_HRBAP ((31 - 0) << 0)
/* Cleared on reset. Pointer updated by DMA during operation. */


/* Normal Rx DMA descriptors */

/* Bit 31 - Own bit */
#define ETH_RXDMA_OWN (1 << 31)
/* When set, this bit indicates that the descriptor is owned by the DMA of 
   the MAC Subsystem. When this bit is reset, it indicates that the 
   descriptor is owned by the Host. The DMA clears this bit either 
   when it completes the frame reception or when the buffers that are 
   associated with this descriptor are full. */

/* Bit 30 - Destination address filter fail */
#define ETH_RXDMA_AFM (1 << 30)
/* When set, this bit indicates a frame that failed the DA filter in 
   the MAC Core. */

/* Bits [29..16] - Frame length */
#define ETH_RXDMA_FL ((29 - 16) << 16)
/* These bits indicate the byte length of the received frame that was 
   transferred to host memory (including CRC). This field is valid only 
   when last descriptor (RDES0[8]) is set and descriptor error 
   (RDES0[14]) is reset.
   This field is valid when last descriptor (RDES0[8]) is set. When the 
   last descriptor and error summary bits are not set, this field 
   indicates the accumulated number of bytes that have been 
   transferred for the current frame. */

/* Bit 15 - Error summary */
#define ETH_RXDMA_ES (1 << 15)
/* Indicates the logical OR of the following bits:
   – RDES0[1]: CRC error
   – RDES0[3]: Receive error
   – RDES0[4]: Watchdog timeout
   – RDES0[6]: Late collision
   – RDES0[7]: Giant frame (This is not applicable when RDES0[7] 
   indicates an IPV4 header checksum error.)
   – RDES0[11]: Overflow error
   – RDES0[14]: Descriptor error.
   This field is valid only when the last descriptor (RDES0[8]) is set. */

/* Bit 14 - Descriptor error */
#define ETH_RXDMA_DSE (1 << 14)
/* When set, this bit indicates a frame truncation caused by a frame that 
   does not fit within the current descriptor buffers, and that the DMA 
   does not own the next descriptor. The frame is truncated. This field 
   is valid only when the last descriptor (RDES0[8]) is set. */

/* Bit 13 - Source address filter fail */
#define ETH_RXDMA_SAF (1 << 13)
/* When set, this bit indicates that the SA field of frame failed the 
   SA filter in the MAC Core.
 */
/* Bit 12 - Length error */
#define ETH_RXDMA_LE (1 << 12)
/* When set, this bit indicates that the actual length of the received 
   frame does not match the value in the Length/ Type field. This bit 
   is valid only when the Frame type (RDES0[5]) bit is reset. */

/* Bit 11 - Overflow error */
#define ETH_RXDMA_OE (1 << 11)
/* When set, this bit indicates that the received frame was damaged due 
   to buffer overflow. */

/* Bit 10 - VLAN tag */
#define ETH_RXDMA_VLAN (1 << 10)
/* When set, this bit indicates that the frame pointed to by this 
   descriptor is a VLAN frame tagged by the MAC core. */

/* Bit 9 - First descriptor */
#define ETH_RXDMA_FS (1 << 9)
/* When set, this bit indicates that this descriptor contains the first 
   buffer of the frame. If the size of the first buffer is 0, the second 
   buffer contains the beginning of the frame. If the size of the second 
   buffer is also 0, the next descriptor contains the beginning 
   of the frame. */

/* Bit 8 - Last descriptor */
#define ETH_RXDMA_LS (1 << 8)
/* When set, this bit indicates that the buffers pointed to by this 
   descriptor are the last buffers of the frame.
   Bit 7 IPHCE/TSV: IPv header checksum error / time stamp valid
   If IPHCE is set, it indicates an error in the IPv4 or IPv6 header. This 
   error can be due to inconsistent Ethernet Type field and IP header 
   Version field values, a header checksum mismatch in IPv4, or an 
   Ethernet frame lacking the expected number of IP header bytes. This 
   bit can take on special meaning as specified in Table 145. If enhanced 
   descriptor format is enabled (EDFE=1, bit 7 of ETH_DMABMR), this bit 
   takes on the TSV function (otherwise it is IPHCE). When TSV is set, 
   it indicates that a snapshot of the timestamp is written in descriptor 
   words 6 (RDES6) and 7 (RDES7). TSV is valid only when the Last 
   descriptor bit (RDES0[8]) is set. */

/* Bit 6 - Late collision */
#define ETH_RXDMA_LCO (1 << 6)
/* When set, this bit indicates that a late collision has occurred while 
   receiving the frame in Half-duplex mode. */

/* Bit 5 - Frame type */
#define ETH_RXDMA_FT (1 << 5)
/* When set, this bit indicates that the Receive frame is an Ethernet-type 
   frame (the LT field is greater than or equal to 0x0600). When this bit 
   is reset, it indicates that the received frame is an IEEE802.3 frame. 
   This bit is not valid for Runt frames less than 14 bytes. When the 
   normal descriptor format is used (ETH_DMABMR EDFE=0), FT can take on 
   special meaning as specified in Table 145. */

/* Bit 4 - Receive watchdog timeout */
#define ETH_RXDMA_RWT (1 << 4)
/* When set, this bit indicates that the Receive watchdog timer has expired 
   while receiving the current frame and the current frame is truncated 
   after the watchdog timeout. */

/* Bit 3 - Receive error */
#define ETH_RXDMA_RE (1 << 3)
/* When set, this bit indicates that the RX_ERR signal is asserted 
   while RX_DV is asserted during frame reception. */

/* Bit 2 - Dribble bit error */
#define ETH_RXDMA_DE (1 << 2)
/* When set, this bit indicates that the received frame has a non-integer 
   multiple of bytes (odd nibbles). This bit is valid only in MII mode. */

/* Bit 1 - CRC error */
#define ETH_RXDMA_CE (1 << 1)
/* When set, this bit indicates that a cyclic redundancy check (CRC) 
   error occurred on the received frame. This field is valid only when the 
   last descriptor (RDES0[8]) is set.
   Bit 0 PCE/ESA: Payload checksum error / extended status available
   When set, it indicates that the TCP, UDP or ICMP checksum the core 
   calculated does not match the received encapsulated TCP, UDP or ICMP 
   segment’s Checksum field. This bit is also set when the received number 
   of payload bytes does not match the value indicated in the Length 
   field of the encapsulated IPv4 or IPv6 datagram in the received Ethernet 
   frame. This bit can take on special meaning as specified in Table 145.
	If the enhanced descriptor format is enabled (EDFE=1, bit 7 in 
	ETH_DMABMR), this bit takes on the ESA function (otherwise it is PCE).
	When ESA is set, it indicates that the extended status is available 
	in descriptor word 4 (RDES4). ESA is valid only when the last 
	descriptor bit (RDES0[8]) is set. */

#define ETH_IPPT_UNKOWN (0 << 0)
#define ETH_IPPT_UDP    (1 << 0)
#define ETH_IPPT_TCP    (2 << 0)
#define ETH_IPPT_ICMP   (3 << 0)


/* Bit 31 - Own bit */
#define ETH_TXDMA_OWN (1 << 31)
/* When set, this bit indicates that the descriptor is owned by the 
   DMA. When this bit is reset, it indicates that the descriptor is 
   owned by the CPU. The DMA clears this bit either when it completes 
   the frame transmission or when the buffers allocated in the 
   descriptor are read completely. The ownership bit of the frame’s 
   first descriptor must be set after all subsequent descriptors belonging 
   to the same frame have been set. */

/* Bit 30 - Interrupt on completion */
#define ETH_TXDMA_IC (1 << 30)
/* When set, this bit sets the Transmit Interrupt (Register 5[0]) after 
   the present frame has been transmitted. */

/* Bit 29 - Last segment */
#define ETH_TXDMA_LS (1 << 29)
/* When set, this bit indicates that the buffer contains the last segment 
   of the frame. */

/* Bit 28 - First segment */
#define ETH_TXDMA_FS (1 << 28)
/* When set, this bit indicates that the buffer contains the first 
   segment of a frame. */

/* Bit 27 - Disable CRC */
#define ETH_TXDMA_DC (1 << 27)
/* When this bit is set, the MAC does not append a cyclic redundancy 
   check (CRC) to the end of the transmitted frame. This is valid only 
   when the first segment (TDES0[28]) is set. */

/* Bit 26 - Disable pad */
#define ETH_TXDMA_DP (1 << 26)
/* When set, the MAC does not automatically add padding to a frame 
   shorter than 64 bytes.
   When this bit is reset, the DMA automatically adds padding and CRC to 
   a frame shorter than 64 bytes, and the CRC field is added despite the 
   state of the DC (TDES0[27]) bit. This is valid only when the first 
   segment (TDES0[28]) is set. */

/* Bit 25 - Transmit time stamp enable */
#define ETH_TXDMA_TTSE (1 << 25)
/* When TTSE is set and when TSE is set (ETH_PTPTSCR bit 0), IEEE1588 
   hardware time stamping is activated for the transmit frame described by 
   the descriptor. This field is only valid when the First segment control 
   bit (TDES0[28]) is set. */

/* Bit 24 - Reserved, must be kept at reset value. */

/* Bits 23:22 - Checksum insertion control */
#define ETH_TXDMA_CIC (3 << 22)
/* These bits control the checksum calculation and insertion. Bit 
   encoding is as shown below:
   00: Checksum Insertion disabled
   01: Only IP header checksum calculation and insertion are enabled
   10: IP header checksum and payload checksum calculation and insertion 
   are enabled, but pseudo-header checksum is not calculated in hardware
   11: IP Header checksum and payload checksum calculation and insertion 
   are enabled, and pseudo-header checksum is calculated in hardware. */

/* Bit 21 - Transmit end of ring */
#define ETH_TXDMA_TER (1 << 21)
/* When set, this bit indicates that the descriptor list reached its 
   final descriptor. The DMA returns to the base address of the list, 
   creating a descriptor ring. */

/* Bit 20 - Second address chained */
#define ETH_TXDMA_TCH (1 << 20)
/* When set, this bit indicates that the second address in the 
   descriptor is the next descriptor address rather than the second buffer 
   address. When TDES0[20] is set, TBS2 (TDES1[28:16]) is a “don’t 
   care” value. TDES0[21] takes precedence over TDES0[20]. */

/* Bits 19:18 Reserved, must be kept at reset value. */

/* Bit 17 - Transmit time stamp status */
#define ETH_TXDMA_TTSS (1 << 17)
/* This field is used as a status bit to indicate that a time stamp was 
   captured for the described transmit frame. When this bit is set, 
   TDES2 and TDES3 have a time stamp value captured for the transmit 
   frame. This field is only valid when the descriptor’s Last segment 
   control bit (TDES0[29]) is set.
   Note that when enhanced descriptors are enabled (EDFE=1 in 
   ETH_DMABMR), TTSS=1 indicates that TDES6 and TDES7 have the 
   time stamp value. */

/* Bit 16 - IP header error */
#define ETH_TXDMA_IHE (1 << 16)
/* When set, this bit indicates that the MAC transmitter detected an 
   error in the IP datagram header. The transmitter checks the header 
   length in the IPv4 packet against the number of header bytes received 
   from the application and indicates an error status if there is a 
   mismatch. For IPv6 frames, a header error is reported if the main 
   header length is not 40 bytes. Furthermore, the Ethernet length/type 
   field value for an IPv4 or IPv6 frame must match the IP header version 
   received with the packet. For IPv4 frames, an error status is also 
   indicated if the Header Length field has a value less than 0x5. */

/* Bit 15 - Error summary */
#define ETH_TXDMA_ES (1 << 15)
/* Indicates the logical OR of the following bits:
   TDES0[14]: Jabber timeout
   TDES0[13]: Frame flush
   TDES0[11]: Loss of carrier
   TDES0[10]: No carrier
   TDES0[9]: Late collision
   TDES0[8]: Excessive collision
   TDES0[2]:Excessive deferral
   TDES0[1]: Underflow error
   TDES0[16]: IP header error
   TDES0[12]: IP payload error */

/* Bit 14 - Jabber timeout */
#define ETH_TXDMA_JT (1 << 14)
/* When set, this bit indicates the MAC transmitter has experienced a 
   jabber timeout. This bit is only set when the MAC configuration 
   register’s JD bit is not set. */

/* Bit 13 - Frame flushed */
#define ETH_TXDMA_FF (1 << 13)
/* When set, this bit indicates that the DMA/MTL flushed the frame due 
   to a software Flush command given by the CPU. */

/* Bit 12 - IP payload error */
#define ETH_TXDMA_IPE (1 << 12)
/* When set, this bit indicates that MAC transmitter detected an error 
   in the TCP, UDP, or ICMP IP datagram payload. The transmitter checks 
   the payload length received in the IPv4 or IPv6 header against the 
   actual number of TCP, UDP or ICMP packet bytes received from the
   application and issues an error status in case of a mismatch. */

/* Bit 11 - Loss of carrier */
#define ETH_TXDMA_LCA (1 << 11)
/* When set, this bit indicates that a loss of carrier occurred during 
   frame transmission (that is, the MII_CRS signal was inactive for one 
   or more transmit clock periods during frame transmission). This is 
   valid only for the frames transmitted without collision when the MAC
   operates in Half-duplex mode. */

/* Bit 10 - No carrier */
#define ETH_TXDMA_NC (1 << 10)
/* When set, this bit indicates that the Carrier Sense signal form the 
   PHY was not asserted during transmission. */

/* Bit 9 - Late collision */
#define ETH_TXDMA_LCO (1 << 9)
/* When set, this bit indicates that frame transmission was aborted due 
   to a collision occurring after the collision window (64 byte times, 
   including preamble, in MII mode). This bit is not valid if the 
   Underflow Error bit is set. */

/* Bit 8 - Excessive collision */
#define ETH_TXDMA_EC (1 << 8)
/* When set, this bit indicates that the transmission was aborted after 
   16 successive collisions while attempting to transmit the current 
   frame. If the RD (Disable retry) bit in the MAC Configuration register 
   is set, this bit is set after the first collision, and the transmission 
   of the frame is aborted. */

/* Bit 7 - VLAN frame */
#define ETH_TXDMA_VF (1 << 7)
/* When set, this bit indicates that the transmitted frame was a 
   VLAN-type frame. */

/* Bits 6:3 - Collision count */
#define ETH_TXDMA_CC (0xf << 3)
/*	This 4-bit counter value indicates the number of collisions 
	occurring before the frame was transmitted. The count is not 
	valid when the Excessive collisions bit (TDES0[8]) is set. */

/* Bit 2 - Excessive deferral */
#define ETH_TXDMA_ED (1 << 2)
/* When set, this bit indicates that the transmission has ended 
   because of excessive deferral of over 24 288 bit times if the 
   Deferral check (DC) bit in the MAC Control register is set high. */

/* Bit 1 - Underflow error */
#define ETH_TXDMA_UF (1 << 1)
/* When set, this bit indicates that the MAC aborted the frame because 
   data arrived late from the RAM memory. Underflow error indicates that 
   the DMA encountered an empty transmit buffer while transmitting the 
   frame. The transmission process enters the Suspended state and sets 
   both Transmit underflow (Register 5[5]) and Transmit interrupt 
   (Register 5[0]). */

/* Bit 0 - Deferred bit */
#define ETH_TXDMA_DB (1 << 0)
/* When set, this bit indicates that the MAC defers before transmission 
   because of the presence of the carrier. This bit is valid only 
   in Half-duplex mode. */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32f_eth {
	volatile uint32_t maccr;
	volatile uint32_t macffr;
	volatile uint32_t machthr;
	volatile uint32_t machtlr;
	volatile uint32_t macmiiar;
	volatile uint32_t macmiidr;
	volatile uint32_t macfcr;
	volatile uint32_t macvlantr; /* 8 */
	uint32_t reserved0[2];
	volatile uint32_t macrwuffr; /* 11 */
	volatile uint32_t macpmtcsr;
	uint32_t reserved1[2];
	volatile uint32_t macsr; /* 15 */
	volatile uint32_t macimr;
	struct {
		volatile uint32_t hr;
		volatile uint32_t lr;
	} maca[4]; /* 24 */
	uint32_t reserved2[40];
	volatile uint32_t mmccr; /* 65 */
	volatile uint32_t mmcrir;
	volatile uint32_t mmctir;
	volatile uint32_t mmcrimr;
	volatile uint32_t mmctimr; /* 69 */
	uint32_t reserved3[14];
	volatile uint32_t mmctgfsccr; /* 84 */
	volatile uint32_t mmctgfmsccr;
	uint32_t reserved4[5];
	volatile uint32_t mmctgfcr;
	uint32_t reserved5[10];
	volatile uint32_t mmcrfcecr;
	volatile uint32_t mmcrfaecr;
	uint32_t reserved6[10];
	volatile uint32_t mmcrgufcr;
	uint32_t reserved7[334];
	volatile uint32_t ptptscr;
	volatile uint32_t ptpssir;
	volatile uint32_t ptptshr;
	volatile uint32_t ptptslr;
	volatile uint32_t ptptshur;
	volatile uint32_t ptptslur;
	volatile uint32_t ptptsar;
	volatile uint32_t ptptthr;
	volatile uint32_t ptpttlr;
	volatile uint32_t reserved8;
	volatile uint32_t ptptssr;
	uint32_t reserved9[565];
	volatile uint32_t dmabmr;
	volatile uint32_t dmatpdr;
	volatile uint32_t dmarpdr;
	volatile uint32_t dmardlar;
	volatile uint32_t dmatdlar;
	volatile uint32_t dmasr;
	volatile uint32_t dmaomr;
	volatile uint32_t dmaier;
	volatile uint32_t dmamfbocr;
	volatile uint32_t dmarswtr;
	uint32_t reserved10[8];
	volatile uint32_t dmachtdr;
	volatile uint32_t dmachrdr;
	volatile uint32_t dmachtbar;
	volatile uint32_t dmachrbar;
};

struct rxdma_st {
	uint32_t pce: 1; /* Payload checksum error */
	uint32_t ce: 1; /* CRC error */
	uint32_t dbe: 1; /* Dribble bit error */
	uint32_t re: 1; /* Receive error */
	uint32_t rwt: 1; /* Receive watchdog timeout */
	uint32_t ft: 1; /* Frame type */
	uint32_t lco: 1; /* Late collision */
	uint32_t iphce_tsv: 1; /* IPv header checksum error / 
							  time stamp valid */
	uint32_t ls: 1; /* Last descriptor */
	uint32_t fs: 1; /* First descriptor */
	uint32_t vlan: 1; /* VLAN tag */
	uint32_t oe: 1; /* Overflow error */
	uint32_t le: 1; /* Lenght error */
	uint32_t saf: 1; /* Source address filter fail */
	uint32_t de: 1; /* Descriptor error*/
	uint32_t es: 1; /* Error summary */
	uint32_t fl: 14; /* Frame length */
	uint32_t afm: 1; /* Destination address filter fail */
	uint32_t own: 1; /* Own bit */
};

/* Normal receive descriptor */
struct rxdma_desc {
	volatile union {
		uint32_t rdes0;
		struct rxdma_st st;
	};
	union {
		uint32_t rdes1;
		struct {
			uint32_t rbs1: 13; /* Receive buffer 1 size */
			uint32_t res1: 1;
			uint32_t rch: 1; /* Second address chained */
			uint32_t rer: 1; /* Receive end of ring */
			uint32_t rbs2: 15; /* Receive buffer 2 size */
			uint32_t dic: 1; /* Disable interrupt on completion */
		};
	};
	union {
		uint32_t rdes2;
		uint32_t * rbap1; /* Receive buffer 1 address pointer */
		uint32_t rtsl; /* Receive frame time stamp low */
	};
	union {
		uint32_t rdes3;
		uint32_t * rbap2; /* Receive buffer 2 address pointer */
		uint32_t rtsh; /* Receive frame time stamp high */
	};
};

struct rxdma_ext_st {
	uint32_t ippt: 3; /* IP payload type */
	uint32_t iphe: 1; /* IP header error */
	uint32_t ippe: 1; /* IP payload error */
	uint32_t ipcb: 1; /* IP checksum bypass */
	uint32_t ipv4pr: 1; /* IPv4 packet received */
	uint32_t ipv6pr: 1; /* IPv6 packet received */
	uint32_t pmt: 4; /* PTP message type */
	uint32_t pft: 1; /* PTP frame type */
	uint32_t pv: 1; /* PTP version */
	uint32_t res: 18;
};

/* Enhanced receive descriptor */
struct rxdma_enh_desc {
	volatile union {
		uint32_t rdes0;
		struct rxdma_st st;
	};
	union {
		uint32_t rdes1;
		struct {
			uint32_t rbs1: 13; /* Receive buffer 1 size */
			uint32_t res1: 1;
			uint32_t rch: 1; /* Second address chained */
			uint32_t rer: 1; /* Receive end of ring */
			uint32_t rbs2: 15; /* Receive buffer 2 size */
			uint32_t dic: 1; /* Disable interrupt on completion */
		};
	};
	union {
		uint32_t rdes2;
		uint32_t * rbap1; /* Receive buffer 1 address pointer */
	};
	union {
		uint32_t rdes3;
		uint32_t * rbap2; /* Receive buffer 2 address pointer */
	};
	union {
		uint32_t rdes4;
		struct rxdma_ext_st ext_st; /* Extended status */
	};
	union {
		uint32_t rdes5;
		uint32_t rtsl; /* Receive frame time stamp low */
	};
	union {
		uint32_t rdes6;
		uint32_t rtsh; /* Receive frame time stamp high */
	};
};

struct txdma_st {
	uint32_t db: 1; /* Deferred bit */
	uint32_t uf: 1; /* Underflow error */
	uint32_t ed: 1; /* Excessive deferral */
	uint32_t cc: 4; /* Collision count */
	uint32_t vf: 1; /* VLAN frame */

	uint32_t ec: 1; /* Excessive collision */
	uint32_t lco: 1; /* Late collision */
	uint32_t nc: 1; /* No carrier */
	uint32_t lca: 1; /* Loss of carrier */
	uint32_t ipe: 1; /* IP payload error */
	uint32_t ff: 1; /* Frame flushed */
	uint32_t jt: 1; /* Jabber timeout */
	uint32_t es: 1; /* Error summary */

	uint32_t ihe: 1; /* IP header error */
	uint32_t ttss: 1; /* Transmit time stamp status */
	uint32_t res1: 2; 
	uint32_t tch: 1; /* Second address chained */
	uint32_t ter: 1; /* Transmit end of ring */
	uint32_t cic: 2; /* Checksum insertion control */

	uint32_t res2: 1;
	uint32_t ttse: 1; /* Transmit time stamp enable */
	uint32_t dp: 1; /* Disable pad */
	uint32_t dc: 1; /* Disable CRC */
	uint32_t fs: 1; /* First segment */
	uint32_t ls: 1; /* Last segment */
	uint32_t ic: 1; /* Interrupt on completion */
	uint32_t own: 1; /* Own bit */
};

/* Normal transmit descriptor */
struct txdma_desc {
	volatile union {
		uint32_t tdes0;
		struct txdma_st st;
	};
	union {
		uint32_t tdes1;
		struct {
			uint32_t tbs1: 13; /* Buffer 1 byte count */
			uint32_t res1: 3;
			uint32_t tbs2: 13; /* Buffer 2 byte count */
			uint32_t res2: 3;
		};
	};
	union {
		uint32_t tdes2;
		uint32_t * tbap1; /* Transmit buffer 1 address pointer */
		uint32_t ttsl; /* Transmit frame time stamp low */
	};
	union {
		uint32_t tdes3;
		uint32_t * tbap2; /* Transmit buffer 2 address pointer */
		uint32_t ttsh; /* Transmit frame time stamp high */
	};
};

/* Enhanced transmit descriptor */
struct txdma_enh_desc {
	volatile union {
		uint32_t tdes0;
		volatile struct txdma_st st;
		struct {
			uint32_t db: 1; /* Deferred bit */
			uint32_t uf: 1; /* Underflow error */
			uint32_t ed: 1; /* Excessive deferral */
			uint32_t cc: 4; /* Collision count */
			uint32_t vf: 1; /* VLAN frame */

			uint32_t ec: 1; /* Excessive collision */
			uint32_t lco: 1; /* Late collision */
			uint32_t nc: 1; /* No carrier */
			uint32_t lca: 1; /* Loss of carrier */
			uint32_t ipe: 1; /* IP payload error */
			uint32_t ff: 1; /* Frame flushed */
			uint32_t jt: 1; /* Jabber timeout */
			uint32_t es: 1; /* Error summary */

			uint32_t ihe: 1; /* IP header error */
			uint32_t ttss: 1; /* Transmit time stamp status */
			uint32_t res1: 2; 
			uint32_t tch: 1; /* Second address chained */
			uint32_t ter: 1; /* Transmit end of ring */
			uint32_t cic: 2; /* Checksum insertion control */

			uint32_t res2: 1;
			uint32_t ttse: 1; /* Transmit time stamp enable */
			uint32_t dp: 1; /* Disable pad */
			uint32_t dc: 1; /* Disable CRC */
			uint32_t fs: 1; /* First segment */
			uint32_t ls: 1; /* Last segment */
			uint32_t ic: 1; /* Interrupt on completion */
			volatile uint32_t own: 1; /* Own bit */
		};
	};
	union {
		uint32_t tdes1;
		struct {
			uint32_t tbs1: 13; /* Buffer 1 byte count */
			uint32_t res3: 3;
			uint32_t tbs2: 13; /* Buffer 2 byte count */
			uint32_t res4: 3;
		};
	};
	union {
		uint32_t tdes2;
		uint32_t * volatile tbap1; /* Transmit buffer 1 address pointer */
	};
	union {
		uint32_t tdes3;
		uint32_t * volatile tbap2; /* Transmit buffer 2 address pointer */
	};
	union {
		uint32_t tdes4;
		uint32_t res5;
	};
	union {
		uint32_t tdes5;
		uint32_t res6;
	};
	union {
		uint32_t tdes6;
		uint32_t ttsl; /* Transmit frame time stamp low */
	};
	union {
		uint32_t tdes7;
		uint32_t ttsh; /* Transmit frame time stamp high */
	};
};

#endif /* __ASSEMBLER__ */

#endif /* __STM32F_ETH_H__ */

