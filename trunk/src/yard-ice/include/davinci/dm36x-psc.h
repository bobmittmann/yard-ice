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
 * @file dm36x-psc.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __DM36X_PSC_H__
#define __DM36X_PSC_H__

/* Power/Sleep Ctrl Registers */
#define PSC_PID 0x000
#define PSC_INTEVAL 0x018
#define PSC_MERRPR0 0x040
#define PSC_MERRPR1 0x044
#define PSC_MERRCR0 0x050
#define PSC_MERRCR1 0x054
#define PSC_PERRPR 0x060
#define PSC_PERRCR 0x068
#define PSC_EPCPR 0x070
#define PSC_EPCCR 0x078
#define PSC_PTCMD 0x120
#define PSC_PTSTAT 0x128
#define PSC_PDSTAT0 0x200
#define PSC_PDSTAT1 0x204
#define PSC_PDCTL0 0x300
#define PSC_PDCTL1 0x304
#define PSC_MCKOUT0 0x520
#define PSC_MCKOUT1 0x524
#define PSC_MDSTAT(N) (0x800 + ((N) * 4))
#define PSC_MDCTL(N) (0xa00 + ((N) * 4))


/* PSC constants */
#define LPSC_TPCC 0
#define LPSC_TPTC0 1
#define LPSC_TPTC1 2
#define LPSC_TPTC2 3
#define LPSC_TPTC3 4
#define LPSC_TIMER3 5
#define LPSC_SPI1 6
#define LPSC_MMC_SD1 7
#define LPSC_ASP1 8
#define LPSC_USB 9
#define LPSC_PWM3 10
#define LPSC_SPI2 11
#define LPSC_RTO 12
#define LPSC_DDR 13
#define LPSC_AEMIF 14
#define LPSC_MMC_SD0 15
#define LPSC_MEMSTK 16
#define TIMER4 17
#define LPSC_I2C 18
#define LPSC_UART0 19
#define LPSC_UART1 20
#define LPSC_UHPI 21
#define LPSC_SPIO 22
#define LPSC_PWM0 23
#define LPSC_PWM1 24
#define LPSC_PWM2 25
#define LPSC_GPIO 26
#define LPSC_TIMER0 27
#define LPSC_TIMER1 28
#define LPSC_TIMER2 29
#define LPSC_SYSMOD 30
#define LPSC_ARM 31
#define LPSC_SPI3 38
#define LPSC_SPI4 39
#define LPSC_CPGMAC 40
#define LPSC_RTC 41
#define LPSC_KEYSCAN 42
#define LPSC_ADC 43
#define LPSC_VOICECODEC 44
#define LPSC_IMCOP 50
#define LPSC_KALEIDO 51

#ifndef __ASSEMBLER__

#include <stdint.h>

struct dm36x_psc {
	uint32_t pid;
};

#endif /* __ASSEMBLER__ */


#endif /* __DM36X_PSC_H__ */

