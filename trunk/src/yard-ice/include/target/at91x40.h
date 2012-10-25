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
 * @file .h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __TARGET_AT91X40_H__
#define __TARGET_AT91X40_H__

/*****************************************************************************
 WDC: Watchdog Timer
 *****************************************************************************/

/* Overflow Mode Register - Read/Write */
#define WD_OMR                      0x00
/* Watch Dog Enable */
#define WD_WDEN                     (1 << 0)
/* Reset Enable */
#define WD_RSTEN                    (1 << 1)
/* Interrupt Enable */
#define WD_IRQEN                    (1 << 2)
/* External Signal Enable */
#define WD_EXTEN                    (1 << 3)
/* Overflow Access Key */
#define WD_OKEY                     (0x234 << 4)

/* Clock Mode Register - Read/Write */
#define WD_CMR                      0x04
/* Clock Selection */
#define WD_WDCLKS_MCK8              (0 << 0)
#define WD_WDCLKS_MCK32             (1 << 0)
#define WD_WDCLKS_MCK128            (2 << 0)
#define WD_WDCLKS_MCK1024           (3 << 0)
/* High Preload Counter Value */
#define WD_HPCV(COUNT)              (((COUNT) >> 10) & 0x003c)

#define WD_HPCV_1                   (1 << 2)
#define WD_HPCV_2                   (2 << 2)
#define WD_HPCV_4                   (4 << 2)
#define WD_HPCV_8                   (8 << 2)
#define WD_HPCV_MAX                 (0x0f << 2)

/* Clock Access Key */
#define WD_CKEY                     (0x06e << 7)

/* Control Register - Write Only */
#define WD_CR                      0x08
/* Restart Key */
#define WD_RSTKEY                  0xc071

/* Status Register - Read Only */
#define WD_SR                      0x0C
/* Watchdog Overflow */
#define WD_WDOVF                    1

/*****************************************************************************
 SF: Special Function Registers
 *****************************************************************************/

/* Chip ID Register - Read Only Hardwired */
#define SF_CIDR                    0x00
/* Version of the chip */
#define SF_VERSION_MASK            (~(0x1f << 0))
/* Non Volatile Program Memory Size */
#define SF_NVPSIZ_MASK             (~(0x0f << 8))
#define SF_NVPSIZ_NONE             (0 << 8)
#define SF_NVPSIZ_32K              (3 << 8)
#define SF_NVPSIZ_64K              (5 << 8)
#define SF_NVPSIZ_128K             (7 << 8)
#define SF_NVPSIZ_256K             (9 << 8)
/* Volatile Data Memory Size */
#define SF_NVDSIZ_MASK             (~(0x0f << 12))
#define SF_NVDSIZ_NONE             (0 << 12)
/* Volatile Data Memory Size */
#define SF_VDSIZ_MASK              (~(0x0f << 16))
#define SF_VDSIZ_NONE              (0 << 16)
#define SF_VDSIZ_1K                (0 << 16)
#define SF_VDSIZ_2K                (0 << 16)
#define SF_VDSIZ_4K                (0 << 16)
#define SF_VDSIZ_8K                (0 << 16)
/* Chip Architecture */
#define	SF_ARCH_MASK               (~(0xff << 20))
#define	SF_ARCH_AT91x40            (0x40 << 20)
/* Non Volatile Program Memory Type */
#define SF_NVPTYP_MASK             (~(7 << 28))
#define SF_NVPTYP_M                (1 << 28)
#define SF_NVPTYP_R                (4 << 28)
/* Extension Flag */
#define SF_EXT                     (1 << 31)

/* Chip ID Extension Register - Read Only Hardwired */
#define SF_EXID                    0x04

/* Reset Status Register - Read Only */
#define SF_RSR                     0x08

#define SF_EXT_RESET               0x6c
#define SF_WD_RESET                0x53

/* Memory Mode Register - Read/Write */
#define SF_MMR                     0x0C

#define SF_RAMWU                   1

/* Protect Mode Register - Read/Write */
#define SF_PMR                     0x18

#define SF_PMRKEY                  (0x27a8 << 16)
#define SF_AIC                     (1 << 5)

/*****************************************************************************
 PIO: Parallel I/O Controller 
 *****************************************************************************/
/* PIO Enable Register */
#define PIO_PER                 0x00 
/* PIO Disable Register */
#define PIO_PDR                 0x04 
/* PIO Status Register */
#define PIO_PSR                 0x08 
/* Output Enable Register */
#define PIO_OER                 0x10 
/* Output Disable Register */
#define PIO_ODR                 0x14 
/* Output Status Register */
#define PIO_OSR                 0x18 
/* Input Filter Enable Register */
#define PIO_IFER                0x20 
/* Input Filter Disable Register */
#define PIO_IFDR                0x24 
/* Input Filter Status Register */
#define PIO_IFSR                0x28 
/* Set Output Data Register */
#define PIO_SODR                0x30 
/* Clear Output Data Register */
#define PIO_CODR                0x34 
/* Output Data Status Register */
#define PIO_ODSR                0x38 
/* Pin Data Status Register */
#define PIO_PDSR                0x3c 
/* Interrupt Enable Register */
#define PIO_IER                 0x40 
/* Interrupt Disable Register */
#define PIO_IDR                 0x44
/* Interrupt Mask Register */
#define PIO_IMR                 0x48
/* Interrupt Status Register */
#define PIO_ISR                 0x4c

#define PIO_P0          (1 << 0)
#define PIO_P1          (1 << 1)
#define PIO_P2          (1 << 2)
#define PIO_P3          (1 << 3)
#define PIO_P4          (1 << 4)
#define PIO_P5          (1 << 5)
#define PIO_P6          (1 << 6)
#define PIO_P7          (1 << 7)
#define PIO_P8          (1 << 8)
#define PIO_P9          (1 << 9)
#define PIO_P10         (1 << 10)
#define PIO_P11         (1 << 11)
#define PIO_P12         (1 << 12)
#define PIO_P13         (1 << 13)
#define PIO_P14         (1 << 14)
#define PIO_P15         (1 << 15)
#define PIO_P16         (1 << 16)
#define PIO_P17         (1 << 17)
#define PIO_P18         (1 << 18)
#define PIO_P19         (1 << 19)
#define PIO_P20         (1 << 20)
#define PIO_P21         (1 << 21)
#define PIO_P22         (1 << 22)
#define PIO_P23         (1 << 23)
#define PIO_P24         (1 << 24)
#define PIO_P25         (1 << 25)
#define PIO_P26         (1 << 26)
#define PIO_P27         (1 << 27)
#define PIO_P28         (1 << 28)
#define PIO_P29         (1 << 29)
#define PIO_P30         (1 << 30)
#define PIO_P31         (1 << 31)

#define AT91_BASE_PIO   0xffff0000
#define AT91_BASE_WD    0xffff8000
#define AT91_BASE_SF    0xfff00000

#define AT91M40800 0x14080044
#define AT91R40807 0x44080746
#define AT91M40807 0x14080745
#define AT91R40008 0x44000840

#include <stdint.h>
#include <target.h>
#include <target/arm.h>
#include <stdlib.h>
#include "armice.h"
    
#include <sys/dcclog.h>

extern const struct target_cpu at91m40800_cpu;

extern const struct target_cpu at91r40008_cpu;

extern const struct armice_cfg at91x40_cfg;

#ifdef __cplusplus
extern "C" {
#endif

int at91x40_reset(FILE * f, const struct ice_drv * ice, 
				  ice_mem_entry_t * mem);

int at91x40_on_halt(FILE * f, const struct ice_drv * ice, 
					ice_mem_entry_t * mem);

int at91x40_on_run(FILE * f, const struct ice_drv * ice, 
				   ice_mem_entry_t * mem);

int at91r40008_probe(FILE * f, const struct ice_drv * ice, 
				   ice_mem_entry_t * mem);

int at91m40800_probe(FILE * f, const struct ice_drv * ice, 
				   ice_mem_entry_t * mem);

#ifdef __cplusplus
	}
#endif

#endif /* __TARGET_AT91X40_H__ */

