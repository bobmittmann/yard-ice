
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
 * @file arch/at91x40.h
 * @brief ARM CPU macros and definitions 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __AT91X40_H__
#define __AT91X40_H__

#define __AT91X40__

/* On-Chip Peripherals */
#define AT91X40_ONCHIP_PERIPHERAL   0xffc00000

/* On-Chip Primary RAM Bank */
#define AT91X40_ONCHIP_RAM0         0x00300000
#define AT91X40_ONCHIP_RAM1         0xffc00000

#define AT91X40_RAM0_BOOT_ADDR      0x00300000
#define AT91X40_RAM0_REMAP_ADDR     0x00000000

#define AT91X40_NCS0_BOOT_ADDR      0x00000000

#include <at91x40/at91_ebi.h>
#include <at91x40/at91_sf.h>
#include <at91x40/at91_tc.h>
#include <at91x40/at91_pio.h>
#include <at91x40/at91_ps.h>
#include <at91x40/at91_us.h>
#include <at91x40/at91_wd.h>
#include <at91x40/at91_aic.h>

#define AT91_BASE_PIO   0xffff0000
#define AT91_BASE_PS    0xffff4000
#define AT91_BASE_US0   0xfffd0000
#define AT91_BASE_US1   0xfffcc000
#define AT91_BASE_AIC   0xfffff000
#define AT91_BASE_TCB   0xfffe0000
#define AT91_BASE_TC0   (AT91_BASE_TCB + 0x00)
#define AT91_BASE_TC1   (AT91_BASE_TCB + 0x40)
#define AT91_BASE_TC2   (AT91_BASE_TCB + 0x80)
#define AT91_BASE_WD    0xffff8000
#define AT91_BASE_SF    0xfff00000

#define AT91_ID_FIQ     0
#define AT91_ID_SWI     1
#define AT91_ID_US0     2
#define AT91_ID_US1     3
#define AT91_ID_TC0     4
#define AT91_ID_TC1     5
#define AT91_ID_TC2     6
#define AT91_ID_WD      7
#define AT91_ID_PIO     8
#define AT91_ID_IRQ0    16
#define AT91_ID_IRQ1    17
#define AT91_ID_IRQ2    18

#define AT91_ALL_INT   ((1 << AT91_ID_FIQ) | (1 << AT91_ID_SWI) |\
	(1 << AT91_ID_US0) | (1 << AT91_ID_US1) | (1 << AT91_ID_TC0) |\
	(1 << AT91_ID_TC1) | (1 << AT91_ID_TC2) | (1 << AT91_ID_WD) |\
	(1 << AT91_ID_PIO) | (1 << AT91_ID_IRQ0) | (1 << AT91_ID_IRQ1) |\
    (1 << AT91_ID_IRQ2))

#define PS_PIO (1 << AT91_ID_PIO)
#define PS_TC2  (1 << AT91_ID_TC2)
#define PS_TC1  (1 << AT91_ID_TC1)
#define PS_TC0  (1 << AT91_ID_TC0)
#define PS_US1  (1 << AT91_ID_US1)
#define PS_US0  (1 << AT91_ID_US0)

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

#define PIO_TCLK0       (1 << 0)
#define PIO_TIOA0       (1 << 1)
#define PIO_TIOB0       (1 << 2)

#define PIO_TCLK1       (1 << 3)
#define PIO_TIOA1       (1 << 4)
#define PIO_TIOB1       (1 << 5)

#define PIO_TCLK2       (1 << 6)
#define PIO_TIOA2       (1 << 7)
#define PIO_TIOB2       (1 << 8)

#define PIO_IRQ0        (1 << 9)
#define PIO_IRQ1        (1 << 10)
#define PIO_IRQ2        (1 << 11)
#define PIO_FIQ         (1 << 12)

#define PIO_SCK0        (1 << 13)
#define PIO_TXD0        (1 << 14)
#define PIO_RXD0        (1 << 15)

#define PIO_SCK1        (1 << 20)
#define PIO_TXD1        (1 << 21)
#define PIO_RXD1        (1 << 22)

#define PIO_MCKO        (1 << 25)
#define PIO_NCS2        (1 << 26)
#define PIO_NCS3        (1 << 27)
#define PIO_CS7         (1 << 28)
#define PIO_CS6         (1 << 29)
#define PIO_CS5         (1 << 30)
#define PIO_CS4         (1 << 31)

#ifndef __ASSEMBLER__

struct at91x40_peripheral {
	uint32_t res0[524288];
	struct at91_ebi ebi;
	uint32_t res1[258048];
	struct at91_sf sf;
	uint32_t res2[204800];
	struct at91_us us1;
	struct at91_us us0;
	uint32_t res3[12288];
	struct at91_tcb tcb;
	uint32_t res4[12288];
	struct at91_pio pio;
	struct at91_ps ps;
	struct at91_wd wd;
	uint32_t res5[3072];
	struct at91_aic aic;
};

//#define AT91X40 ((struct at90x40_peripheral *)AT91X40_ONCHIP_PERIPHERAL)

extern struct at91x40_peripheral at91x40;

#define AT91_EBI ((struct at91_ebi *)&at91x40.ebi)
#define AT91_PIO ((struct at91_pio *)&at91x40.pio)
#define AT91_US0 ((struct at91_us *)&at91x40.us0)
#define AT91_US1 ((struct at91_us *)&at91x40.us1)
#define AT91_PS ((struct at91_ps *)&at91x40.ps)
#define AT91_AIC ((struct at91_aic *)&at91x40.aic)
#define AT91_TC0 ((struct at91_tc *)&at91x40.tcb.tc[0])
#define AT91_TC1 ((struct at91_tc *)&at91x40.tcb.tc[1])
#define AT91_TC2 ((struct at91_tc *)&at91x40.tcb.tc[2])
#define AT91_TCB ((struct at91_tcb *)&at91x40.tcb)
#define AT91_WD ((struct at91_wd *)&at91x40.wd)

#endif /* __ASSEMBLER__ */

#endif /* __AT91X40_H__ */

