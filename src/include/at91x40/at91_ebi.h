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


#ifndef __AT91_EBI_H__
#define __AT91_EBI_H__

/*****************************************************************************
  EBI: External Bus Interface 
 *****************************************************************************/
#define AT91_EBI_BASE               0xffe00000
/* Chip Select Register 0 */
#define EBI_CSR0_OFFS               0x00
#define EBI_CSR0                    AT91_EBI_BASE + EBI_CSR0_OFFS
/* Chip Select Register 1 */
#define EBI_CSR1_OFFS               0x04
#define EBI_CSR1                    AT91_EBI_BASE + EBI_CSR1_OFFS
/* Chip Select Register 2 */
#define EBI_CSR2_OFFS               0x08
#define EBI_CSR2                    AT91_EBI_BASE + EBI_CSR2_OFFS
/* Chip Select Register 3 */
#define EBI_CSR3_OFFS               0x0c
#define EBI_CSR3                    AT91_EBI_BASE + EBI_CSR3_OFFS
/* Chip Select Register 4 */
#define EBI_CSR4_OFFS               0x10
#define EBI_CSR4                    AT91_EBI_BASE + EBI_CSR4_OFFS
/* Chip Select Register 5 */
#define EBI_CSR5_OFFS               0x14
#define EBI_CSR5                    AT91_EBI_BASE + EBI_CSR5_OFFS
/* Chip Select Register 6 */
#define EBI_CSR6_OFFS               0x18
#define EBI_CSR6                    AT91_EBI_BASE + EBI_CSR6_OFFS
/* Chip Select Register 7 */
#define EBI_CSR7_OFFS               0x1c
#define EBI_CSR7                    AT91_EBI_BASE + EBI_CSR7_OFFS
/* Data Bus Width */
#define EBI_DBW_16                  (1 << 0)
#define EBI_DBW_8                   (2 << 0)
/* Number of Wait States */
#define EBI_NWS_1                   (0 << 2)
#define EBI_NWS_2                   (1 << 2)
#define EBI_NWS_3                   (2 << 2)
#define EBI_NWS_4                   (3 << 2)
#define EBI_NWS_5                   (4 << 2)
#define EBI_NWS_6                   (5 << 2)
#define EBI_NWS_7                   (6 << 2)
#define EBI_NWS_8                   (7 << 2)
#define EBI_NWS_MASK                (7 << 2)
/* Wait State Enable */
#define EBI_WSE                     (1 << 5)
#define EBI_NWS(CSR)                ((CSR & EBI_WSE) ? (((CSR & EBI_NWS_MASK) >> 2) + 1) : 0)
/* Page Size */
#define EBI_PAGES_1M                (0 << 7)
#define EBI_PAGES_4M                (1 << 7)
#define EBI_PAGES_16M               (2 << 7)
#define EBI_PAGES_64M               (3 << 7)
#define EBI_PAGES_MASK              (3 << 7)
#define EBI_PAGES(CSR)              ((CSR & EBI_PAGES_MASK) >> 7)
/* Data Float Output Time */
#define EBI_TDF_0                   (0 << 9)
#define EBI_TDF_1                   (1 << 9)
#define EBI_TDF_2                   (2 << 9)
#define EBI_TDF_3                   (3 << 9)
#define EBI_TDF_4                   (4 << 9)
#define EBI_TDF_5                   (5 << 9)
#define EBI_TDF_6                   (6 << 9)
#define EBI_TDF_7                   (7 << 9)
#define EBI_TDF_MASK                (7 << 9)
#define EBI_TDF(CSR)                ((CSR & EBI_TDF_MASK) >> 9)
/* Byte Access Type */
#define EBI_BAT_BYTE_WRITE          (0 << 12)
#define EBI_BAT_BYTE_SELECT         (1 << 12)
/* Chip Select Enable */
#define EBI_CSEN                    (1 << 13)
/* Base Address */
#define EBI_BA_MASK                 0xfff00000
#define EBI_BA(CSR)                 ((CSR) & EBI_BA_MASK)

/* Remap Control Register - Write Only */
#define EBI_RCR_OFFS                0x20
#define EBI_RCR                     AT91_EBI_BASE + EBI_RCR_OFFS
/* Remap Command Bit */
#define EBI_RCB                     1

/* Memory Control Register - Read/Write */
#define EBI_MCR_OFFS                0x24
#define EBI_MCR                     AT91_EBI_BASE + EBI_MCR_OFFS
/* Address Line Enable */
#define EBI_ALE_16M                 0x0
#define EBI_ALE_8M                  0x4
#define EBI_ALE_4M                  0x5
#define EBI_ALE_2M                  0x6
#define EBI_ALE_1M                  0x7
/* Data Read Protocol */
#define EBI_DRP_STANDARD            (0 << 4)
#define EBI_DRP_EARLY               (1 << 4)

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_ebi {
	uint32_t csr[8];
	volatile uint32_t rcr;
	uint32_t mcr;
	uint32_t res[4086];
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_EBI_H__ */

