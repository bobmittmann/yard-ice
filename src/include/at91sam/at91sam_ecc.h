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

#ifndef __AT91_ECC_H__
#define __AT91_ECC_H__

/* Error Correction Code controller */

#define ECC_CR          ( 0) /* ECC reset register */
#define ECC_MR          ( 4) /* ECC Page size register */
#define ECC_SR          ( 8) /* ECC Status register */
#define ECC_PR          (12) /* ECC Parity register */
#define ECC_NPR         (16) /* ECC Parity N register */
#define ECC_VR          (252) /* ECC Version register */

/* ECC_CR - ECC reset register */
#define ECC_RST             (0x1 <<  0) /* (ECC) ECC reset parity */

/* ECC_MR - ECC page size register */
#define ECC_PAGE_SIZE       (0x3 <<  0) /* (ECC) Nand Flash page size */

/* ECC_SR - ECC status register */
#define ECC_RECERR          (0x1 <<  0) /* (ECC) ECC error */
#define ECC_ECCERR          (0x1 <<  1) /* (ECC) ECC single error */
#define ECC_MULERR          (0x1 <<  2) /* (ECC) ECC_MULERR */

/* ECC_PR - ECC parity register */
#define ECC_BITADDR         (0xf <<  0) /* (ECC) Bit address error */
#define ECC_WORDADDR        (0xfff <<  4) /* (ECC) address of the failing bit */

/* ECC_NPR - ECC N parity register */
#define ECC_NPARITY         (0xffff <<  0) /* (ECC) ECC parity N */

/* ECC_VR - ECC version register */
#define ECC_VR_MASK         (0xf <<  0) /* (ECC) ECC version register */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct at91_ecc {
	uint32_t cr; /* ECC reset register */
	uint32_t mr; /* ECC Page size register */
	uint32_t sr; /* ECC Status register */
	uint32_t pr; /* ECC Parity register */
	uint32_t npr; /* ECC Parity N register */
	uint32_t vr; /* ECC Version register */
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_ECC_H__ */

