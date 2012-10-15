/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file sys/mii.h
 * @brief YARD-ICE libtcpip
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_MII_H__
#define __SYS_MII_H__

/* Generic MII registers. */
/* Basic Mode Control Register */
#define MII_BMCR 0x00

#define BMCR_RESET (1 << 15) /* (0, RW/SC) Reset
								1=Software reset
								0=Normal operation
								This bit sets the status and controls the 
								PHY registers to their default states. 
								This bit, which is self-clearing, will 
								keep returning a value of one until the 
								reset process is completed */

#define BMCR_LOOPBACK (1 << 14) /* (0, RW) Loop-back control register
								   1 = Loop-back enabled
								   0 = Normal operation
								   When in 100Mbps operation mode, setting 
								   this bit may cause the descrambler to 
								   lose synchronization and produce a 720ms 
								   "dead time" before receive */

#define BMCR_SPEED100 (1 << 13) /* (1, RW) Speed Select
								   1 = 100Mbps
								   0 = 10Mbps
								   Link speed may be selected either by 
								   this bit or by auto-negotiation. When 
								   auto-negotiation is enabled and bit 12 
								   is set, this bit will return 
								   auto-negotiation selected medium type */

#define BMCR_ANENABLE (1 << 12) /* (1, RW) Auto-negotiation Enable
								   1 = Auto-negotiation is enabled, bit 8 
								   and 13 will be in auto-negotiation status */

#define BMCR_PDOWN (1 << 11) /* (0, RW) Power Down
								While in the power-down state, the PHY 
								should respond to management transactions.
								1=Power down
								0=Normal operation */

#define BMCR_ISOLATE (1 << 10) /* (0,RW) Isolate
                               Force to 0 in application. */

#define BMCR_ANRESTART (1 << 9) /* (0,RW/SC) Restart Auto-negotiation
								   1 = Restart auto-negotiation. 
								   Re-initiates the auto-negotiation process. 
								   When auto-negotiation is disabled (bit 12 
								   of this register cleared), this bit has 
								   no function and it should be cleared. 
								   This bit is self-clearing and it will 
								   keep returning to a value of 1 until 
								   auto-negotiation is initiated by 
								   the DM9000A. The operation of the 
								   auto-negotiation process will not be 
								   affected by the management entity that 
								   clears this bit
								   0 = Normal operation */


#define BMCR_FULLDPLX (1 << 8) /* (1,RW) Duplex Mode
								  1 = Full duplex operation. Duplex selection 
								  is allowed when Auto-negotiation is 
								  disabled (bit 12 of this register is 
								  cleared). With auto-negotiation enabled, 
								  this bit reflects the duplex capability 
								  selected by auto-negotiation
								  0 = Normal operation */

#define BMCR_CTST (1 << 7) /* (0,RW) Collision Test
							  1 = Collision test enabled. When set, this 
							  bit will cause the collision asserted during 
							  the transmit period.
							  0 = Normal operation */

#define BMCR_SPEED1000 (1 << 6)  /* Speed (1000) */


/* Basic Mode Status Register  */
#define MII_BMSR 0x01

#define BMSR_ERCAP              0x0001  /* Ext-reg capability          */
#define BMSR_JCD                0x0002  /* Jabber detected             */
#define BMSR_LSTATUS            0x0004  /* Link status                 */
#define BMSR_ANEGCAPABLE        0x0008  /* Able to do auto-negotiation */
#define BMSR_RFAULT             0x0010  /* Remote fault detected       */
#define BMSR_ANEGCOMPLETE       0x0020  /* Auto-negotiation complete   */
#define BMSR_ESTATEN            0x0100  /* Extended Status in R15 */
#define BMSR_100HALF2           0x0200  /* Can do 100BASE-T2 HDX */
#define BMSR_100FULL2           0x0400  /* Can do 100BASE-T2 FDX */
#define BMSR_10HALF             0x0800  /* Can do 10mbps, half-duplex  */
#define BMSR_10FULL             0x1000  /* Can do 10mbps, full-duplex  */
#define BMSR_100HALF            0x2000  /* Can do 100mbps, half-duplex */
#define BMSR_100FULL            0x4000  /* Can do 100mbps, full-duplex */
#define BMSR_100BASE4           0x8000  /* Can do 100mbps, 4k packets  */


/* PHY ID Identifier Register #1 */
#define MII_PHYID1 0x02
/* PHY ID Identifier Register #2 */
#define MII_PHYID2 0x03

/*Auto-negotiation Advertisement Register */
#define MII_ANAR 0x04        

/* Advertisement control register. */
#define ANAR_SLCT          0x001f  /* Selector bits               */
#define ANAR_CSMA          0x0001  /* Only selector supported     */
#define ANAR_10HALF        0x0020  /* Try for 10mbps half-duplex  */
#define ANAR_1000XFULL     0x0020  /* Try for 1000BASE-X full-duplex */
#define ANAR_10FULL        0x0040  /* Try for 10mbps full-duplex  */
#define ANAR_1000XHALF     0x0040  /* Try for 1000BASE-X half-duplex */
#define ANAR_100HALF       0x0080  /* Try for 100mbps half-duplex */
#define ANAR_1000XPAUSE    0x0080  /* Try for 1000BASE-X pause    */
#define ANAR_100FULL       0x0100  /* Try for 100mbps full-duplex */
#define ANAR_1000XPSE_ASYM 0x0100  /* Try for 1000BASE-X asym pause */
#define ANAR_100BASE4      0x0200  /* Try for 100mbps 4k packets  */
#define ANAR_PAUSE_CAP     0x0400  /* Try for pause               */
#define ANAR_PAUSE_ASYM    0x0800  /* Try for asymetric pause     */
#define ANAR_RFAULT        0x2000  /* Say we can detect faults    */
#define ANAR_LPACK         0x4000  /* Ack link partners response  */
#define ANAR_NPAGE         0x8000  /* Next page bit               */

#define ANAR_FULL (ANAR_100FULL | ANAR_10FULL | \
						ANAR_CSMA)
#define ANAR_ALL (ANAR_10HALF | ANAR_10FULL | \
					   ANAR_100HALF | ANAR_100FULL)


/*Auto-negotiation Link Partner Ability Register */
#define MII_ANLPAR 0x05        

/* Link partner ability register. */
#define ANLPAR_SLCT                0x001f  /* Same as advertise selector  */
#define ANLPAR_10HALF              0x0020  /* Can do 10mbps half-duplex   */
#define ANLPAR_1000XFULL           0x0020  /* Can do 1000BASE-X full-duplex */
#define ANLPAR_10FULL              0x0040  /* Can do 10mbps full-duplex   */
#define ANLPAR_1000XHALF           0x0040  /* Can do 1000BASE-X half-duplex */
#define ANLPAR_100HALF             0x0080  /* Can do 100mbps half-duplex  */
#define ANLPAR_1000XPAUSE          0x0080  /* Can do 1000BASE-X pause     */
#define ANLPAR_100FULL             0x0100  /* Can do 100mbps full-duplex  */
#define ANLPAR_1000XPAUSE_ASYM     0x0100  /* Can do 1000BASE-X pause asym*/
#define ANLPAR_100BASE4            0x0200  /* Can do 100mbps 4k packets   */
#define ANLPAR_PAUSE_CAP           0x0400  /* Can pause                   */
#define ANLPAR_PAUSE_ASYM          0x0800  /* Can pause asymetrically     */
#define ANLPAR_RFAULT              0x2000  /* Link partner faulted        */
#define ANLPAR_LPACK               0x4000  /* Link partner acked us       */
#define ANLPAR_NPAGE               0x8000  /* Next page bit               */

#define ANLPAR_DUPLEX		(ANLPAR_10FULL | ANLPAR_100FULL)
#define ANLPAR_100			(ANLPAR_100FULL | ANLPAR_100HALF | ANLPAR_100BASE4)


/*Auto-negotiation Expansion Register */
#define MII_ANER 0x06

#define ANER_PDF (1 << 4) /* (0, RO/LH) Local Device Parallel Detection Fault
                       1: A fault detected via parallel detection function.
                       0: No fault detected via parallel detection function */

#define ANER_LP_NP_ABLE (1 << 3) /* (0, RO)  Link Partner Next Page Able
									1: Link partner, next page available
									0: Link partner, no next page */

#define ANER_NP_ABLE (1 << 2) /* (0,RO/P)  Local Device Next Page Able
                       1: DM9000A, next page available
                       0: DM9000A, no next page
                       DM9000A does not support this function, so this bit is
                       always 0 */

#define ANER_PAGE_RX (1 << 1) /* (0, RO/LH) New Page Received
								 A new link code word page received. This bit 
								 will be automatically cleared when the 
								 register (register 6) is read by management */

#define ANER_LP_AN_ABLE (1 << 0) /* (0, RO)  Link Partner Auto-negotiation Able
                       A “1” in this bit indicates that the link 
					   partner supports Auto-negotiation */


#endif /* __SYS_MII_H__ */

