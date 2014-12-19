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

#ifndef __AT91C_UHP_H__
#define __AT91C_UHP_H__

/* USB Host Interface */

#ifndef __ASSEMBLER__
struct at91s_uhp {
};
#endif /* __ASSEMBLER__ */


#define UHP_HcRevision  ( 0) /* Revision */
#define UHP_HcControl   ( 4) /* Operating modes for the Host Controller */
#define UHP_HcCommandStatus ( 8) /* Command & status Register */
#define UHP_HcInterruptStatus (12) /* Interrupt Status Register */
#define UHP_HcInterruptEnable (16) /* Interrupt Enable Register */
#define UHP_HcInterruptDisable (20) /* Interrupt Disable Register */
#define UHP_HcHCCA      (24) /* Pointer to the Host Controller Communication Area */
#define UHP_HcPeriodCurrentED (28) /* Current Isochronous or Interrupt Endpoint Descriptor */
#define UHP_HcControlHeadED (32) /* First Endpoint Descriptor of the Control list */
#define UHP_HcControlCurrentED (36) /* Endpoint Control and Status Register */
#define UHP_HcBulkHeadED (40) /* First endpoint register of the Bulk list */
#define UHP_HcBulkCurrentED (44) /* Current endpoint of the Bulk list */
#define UHP_HcBulkDoneHead (48) /* Last completed transfer descriptor */
#define UHP_HcFmInterval (52) /* Bit time between 2 consecutive SOFs */
#define UHP_HcFmRemaining (56) /* Bit time remaining in the current Frame */
#define UHP_HcFmNumber  (60) /* Frame number */
#define UHP_HcPeriodicStart (64) /* Periodic Start */
#define UHP_HcLSThreshold (68) /* LS Threshold */
#define UHP_HcRhDescriptorA (72) /* Root Hub characteristics A */
#define UHP_HcRhDescriptorB (76) /* Root Hub characteristics B */
#define UHP_HcRhStatus  (80) /* Root Hub Status register */
#define UHP_HcRhPortStatus (84) /* Root Hub Port Status Register */


#endif /* __AT91C_UHP_H__ */

