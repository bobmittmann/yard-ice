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
 * @file arch/at91sam7s256.h
 * @brief ARM CPU macros and definitions 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __AT91SAM7S256_H__
#define __AT91SAM7S256_H__

#ifndef AT91SAM7S256
#define AT91SAM7S256
#endif

#ifndef AT91SAM7
#define AT91SAM7S
#endif

#ifndef AT91SAM7
#define AT91SAM7
#endif

#ifndef AT91SAM
#define AT91SAM
#endif

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif /* __ASSEMBLER__ */

#include <at91sam/at91sam_smc.h>
#include <at91sam/at91sam_ccfg.h>
#include <at91sam/at91sam_pdc.h>
#include <at91sam/at91sam_dbgu.h>
#include <at91sam/at91sam_aic.h>
#include <at91sam/at91sam_pio.h>
#include <at91sam/at91sam_ckgr.h>
#include <at91sam/at91sam_pmc.h>
#include <at91sam/at91sam_rstc.h>
#include <at91sam/at91sam_shdwc.h>
#include <at91sam/at91sam_rttc.h>
#include <at91sam/at91sam_pitc.h>
#include <at91sam/at91sam_wdtc.h>
#include <at91sam/at91sam_tc.h>
#include <at91sam/at91sam_twi.h>
#include <at91sam/at91sam_us.h>
#include <at91sam/at91sam_ssc.h>
#include <at91sam/at91sam_spi.h>
#include <at91sam/at91sam_adc.h>
#include <at91sam/at91sam_udp.h>
#include <at91sam/at91sam_mc.h>
#include <at91sam/at91sam_vreg.h>
#include <at91sam/at91sam_pwmc.h>

/* Pio definitions for at91sam7s256 */
#define PIO_PA0        (1 <<  0) /* Pin Controlled by PA0 */
#define PA0_PWM0     (PIO_PA0) /* PWM Channel 0 */
#define PA0_TIOA0    (PIO_PA0) /* Timer Counter 0 Multipurpose Timer I/O Pin A */
#define PIO_PA1        (1 <<  1) /* Pin Controlled by PA1 */
#define PA1_PWM1     (PIO_PA1) /* PWM Channel 1 */
#define PA1_TIOB0    (PIO_PA1) /* Timer Counter 0 Multipurpose Timer I/O Pin B */
#define PIO_PA10       (1 << 10) /* Pin Controlled by PA10 */
#define PA10_DTXD     (PIO_PA10) /* DBGU Debug Transmit Data */
#define PA10_NPCS2    (PIO_PA10) /* SPI Peripheral Chip Select 2 */
#define PIO_PA11       (1 << 11) /* Pin Controlled by PA11 */
#define PA11_NPCS0    (PIO_PA11) /* SPI Peripheral Chip Select 0 */
#define PA11_PWM0     (PIO_PA11) /* PWM Channel 0 */
#define PIO_PA12       (1 << 12) /* Pin Controlled by PA12 */
#define PA12_MISO     (PIO_PA12) /* SPI Master In Slave */
#define PA12_PWM1     (PIO_PA12) /* PWM Channel 1 */
#define PIO_PA13       (1 << 13) /* Pin Controlled by PA13 */
#define PA13_MOSI     (PIO_PA13) /* SPI Master Out Slave */
#define PA13_PWM2     (PIO_PA13) /* PWM Channel 2 */
#define PIO_PA14       (1 << 14) /* Pin Controlled by PA14 */
#define PA14_SPCK     (PIO_PA14) /* SPI Serial Clock */
#define PA14_PWM3     (PIO_PA14) /* PWM Channel 3 */
#define PIO_PA15       (1 << 15) /* Pin Controlled by PA15 */
#define PA15_TF       (PIO_PA15) /* SSC Transmit Frame Sync */
#define PA15_TIOA1    (PIO_PA15) /* Timer Counter 1 Multipurpose Timer I/O Pin A */
#define PIO_PA16       (1 << 16) /* Pin Controlled by PA16 */
#define PA16_TK       (PIO_PA16) /* SSC Transmit Clock */
#define PA16_TIOB1    (PIO_PA16) /* Timer Counter 1 Multipurpose Timer I/O Pin B */
#define PIO_PA17       (1 << 17) /* Pin Controlled by PA17 */
#define PA17_TD       (PIO_PA17) /* SSC Transmit data */
#define PA17_PCK1     (PIO_PA17) /* PMC Programmable Clock Output 1 */
#define PIO_PA18       (1 << 18) /* Pin Controlled by PA18 */
#define PA18_RD       (PIO_PA18) /* SSC Receive Data */
#define PA18_PCK2     (PIO_PA18) /* PMC Programmable Clock Output 2 */
#define PIO_PA19       (1 << 19) /* Pin Controlled by PA19 */
#define PA19_RK       (PIO_PA19) /* SSC Receive Clock */
#define PA19_FIQ      (PIO_PA19) /* AIC Fast Interrupt Input */
#define PIO_PA2        (1 <<  2) /* Pin Controlled by PA2 */
#define PA2_PWM2     (PIO_PA2) /* PWM Channel 2 */
#define PA2_SCK0     (PIO_PA2) /* USART 0 Serial Clock */
#define PIO_PA20       (1 << 20) /* Pin Controlled by PA20 */
#define PA20_RF       (PIO_PA20) /* SSC Receive Frame Sync */
#define PA20_IRQ0     (PIO_PA20) /* External Interrupt 0 */
#define PIO_PA21       (1 << 21) /* Pin Controlled by PA21 */
#define PA21_RXD1     (PIO_PA21) /* USART 1 Receive Data */
#define PA21_PCK1     (PIO_PA21) /* PMC Programmable Clock Output 1 */
#define PIO_PA22       (1 << 22) /* Pin Controlled by PA22 */
#define PA22_TXD1     (PIO_PA22) /* USART 1 Transmit Data */
#define PA22_NPCS3    (PIO_PA22) /* SPI Peripheral Chip Select 3 */
#define PIO_PA23       (1 << 23) /* Pin Controlled by PA23 */
#define PA23_SCK1     (PIO_PA23) /* USART 1 Serial Clock */
#define PA23_PWM0     (PIO_PA23) /* PWM Channel 0 */
#define PIO_PA24       (1 << 24) /* Pin Controlled by PA24 */
#define PA24_RTS1     (PIO_PA24) /* USART 1 Ready To Send */
#define PA24_PWM1     (PIO_PA24) /* PWM Channel 1 */
#define PIO_PA25       (1 << 25) /* Pin Controlled by PA25 */
#define PA25_CTS1     (PIO_PA25) /* USART 1 Clear To Send */
#define PA25_PWM2     (PIO_PA25) /* PWM Channel 2 */
#define PIO_PA26       (1 << 26) /* Pin Controlled by PA26 */
#define PA26_DCD1     (PIO_PA26) /* USART 1 Data Carrier Detect */
#define PA26_TIOA2    (PIO_PA26) /* Timer Counter 2 Multipurpose Timer I/O Pin A */
#define PIO_PA27       (1 << 27) /* Pin Controlled by PA27 */
#define PA27_DTR1     (PIO_PA27) /* USART 1 Data Terminal ready */
#define PA27_TIOB2    (PIO_PA27) /* Timer Counter 2 Multipurpose Timer I/O Pin B */
#define PIO_PA28       (1 << 28) /* Pin Controlled by PA28 */
#define PA28_DSR1     (PIO_PA28) /* USART 1 Data Set ready */
#define PA28_TCLK1    (PIO_PA28) /* Timer Counter 1 external clock input */
#define PIO_PA29       (1 << 29) /* Pin Controlled by PA29 */
#define PA29_RI1      (PIO_PA29) /* USART 1 Ring Indicator */
#define PA29_TCLK2    (PIO_PA29) /* Timer Counter 2 external clock input */
#define PIO_PA3        (1 <<  3) /* Pin Controlled by PA3 */
#define PA3_TWD      (PIO_PA3) /* TWI Two-wire Serial Data */
#define PA3_NPCS3    (PIO_PA3) /* SPI Peripheral Chip Select 3 */
#define PIO_PA30       (1 << 30) /* Pin Controlled by PA30 */
#define PA30_IRQ1     (PIO_PA30) /* External Interrupt 1 */
#define PA30_NPCS2    (PIO_PA30) /* SPI Peripheral Chip Select 2 */
#define PIO_PA31       (1 << 31) /* Pin Controlled by PA31 */
#define PA31_NPCS1    (PIO_PA31) /* SPI Peripheral Chip Select 1 */
#define PA31_PCK2     (PIO_PA31) /* PMC Programmable Clock Output 2 */
#define PIO_PA4        (1 <<  4) /* Pin Controlled by PA4 */
#define PA4_TWCK     (PIO_PA4) /* TWI Two-wire Serial Clock */
#define PA4_TCLK0    (PIO_PA4) /* Timer Counter 0 external clock input */
#define PIO_PA5        (1 <<  5) /* Pin Controlled by PA5 */
#define PA5_RXD0     (PIO_PA5) /* USART 0 Receive Data */
#define PA5_NPCS3    (PIO_PA5) /* SPI Peripheral Chip Select 3 */
#define PIO_PA6        (1 <<  6) /* Pin Controlled by PA6 */
#define PA6_TXD0     (PIO_PA6) /* USART 0 Transmit Data */
#define PA6_PCK0     (PIO_PA6) /* PMC Programmable Clock Output 0 */
#define PIO_PA7        (1 <<  7) /* Pin Controlled by PA7 */
#define PA7_RTS0     (PIO_PA7) /* USART 0 Ready To Send */
#define PA7_PWM3     (PIO_PA7) /* PWM Channel 3 */
#define PIO_PA8        (1 <<  8) /* Pin Controlled by PA8 */
#define PA8_CTS0     (PIO_PA8) /* USART 0 Clear To Send */
#define PA8_ADTRG    (PIO_PA8) /* ADC External Trigger */
#define PIO_PA9        (1 <<  9) /* Pin Controlled by PA9 */
#define PA9_DRXD     (PIO_PA9) /* DBGU Debug Receive Data */
#define PA9_NPCS1    (PIO_PA9) /* SPI Peripheral Chip Select 1 */

/* Peripheral id definitions for at91sam7s256 */
#define AT91_ID_FIQ    ( 0) /* Advanced Interrupt Controller (FIQ) */
#define AT91_ID_SYS    ( 1) /* System Peripheral */
#define AT91_ID_PIOA   ( 2) /* Parallel IO Controller */
#define AT91_ID_ADC    ( 4) /* Analog-to-Digital Converter */
#define AT91_ID_SPI    ( 5) /* Serial Peripheral Interface */
#define AT91_ID_US0    ( 6) /* USART 0 */
#define AT91_ID_US1    ( 7) /* USART 1 */
#define AT91_ID_SSC    ( 8) /* Serial Synchronous Controller */
#define AT91_ID_TWI    ( 9) /* Two-Wire Interface */
#define AT91_ID_PWMC   (10) /* PWM Controller */
#define AT91_ID_UDP    (11) /* USB Device Port */
#define AT91_ID_TC0    (12) /* Timer Counter 0 */
#define AT91_ID_TC1    (13) /* Timer Counter 1 */
#define AT91_ID_TC2    (14) /* Timer Counter 2 */
#define AT91_ID_IRQ0   (30) /* Advanced Interrupt Controller (IRQ0) */
#define AT91_ID_IRQ1   (31) /* Advanced Interrupt Controller (IRQ1) */
#define AT91_ALL_INT   (0xc0007ff7) /* ALL VALID INTERRUPTS */

 /* Base address definitions for at91sam7s256 */
#define AT91_BASE_SYS 0xfffff000 /* (SYS) Base Address */
#define AT91_BASE_AIC 0xfffff000 /* (AIC) Base Address */
#define AT91_BASE_PDC_DBGU 0xfffff300 /* (PDC_DBGU) Base Address */
#define AT91_BASE_DBGU 0xfffff200 /* (DBGU) Base Address */
#define AT91_BASE_PIOA 0xfffff400 /* (PIOA) Base Address */
#define AT91_BASE_CKGR 0xfffffc20 /* (CKGR) Base Address */
#define AT91_BASE_PMC 0xfffffc00 /* (PMC) Base Address */
#define AT91_BASE_RSTC 0xfffffd00 /* (RSTC) Base Address */
#define AT91_BASE_RTTC 0xfffffd20 /* (RTTC) Base Address */
#define AT91_BASE_PITC 0xfffffd30 /* (PITC) Base Address */
#define AT91_BASE_WDTC 0xfffffd40 /* (WDTC) Base Address */
#define AT91_BASE_VREG 0xfffffd60 /* (VREG) Base Address */
#define AT91_BASE_MC 0xffffff00 /* (MC) Base Address */
#define AT91_BASE_PDC_SPI 0xfffe0100 /* (PDC_SPI) Base Address */
#define AT91_BASE_SPI 0xfffe0000 /* (SPI) Base Address */
#define AT91_BASE_PDC_ADC 0xfffd8100 /* (PDC_ADC) Base Address */
#define AT91_BASE_ADC 0xfffd8000 /* (ADC) Base Address */
#define AT91_BASE_PDC_SSC 0xfffd4100 /* (PDC_SSC) Base Address */
#define AT91_BASE_SSC 0xfffd4000 /* (SSC) Base Address */
#define AT91_BASE_PDC_US1 0xfffc4100 /* (PDC_US1) Base Address */
#define AT91_BASE_US1 0xfffc4000 /* (US1) Base Address */
#define AT91_BASE_PDC_US0 0xfffc0100 /* (PDC_US0) Base Address */
#define AT91_BASE_US0 0xfffc0000 /* (US0) Base Address */
#define AT91_BASE_TWI 0xfffb8000 /* (TWI) Base Address */
#define AT91_BASE_TC0 0xfffa0000 /* (TC0) Base Address */
#define AT91_BASE_TC1 0xfffa0040 /* (TC1) Base Address */
#define AT91_BASE_TC2 0xfffa0080 /* (TC2) Base Address */
#define AT91_BASE_TCB 0xfffa0000 /* (TCB) Base Address */
#define AT91_BASE_PWMC_CH3 0xfffcc260 /* (PWMC_CH3) Base Address */
#define AT91_BASE_PWMC_CH2 0xfffcc240 /* (PWMC_CH2) Base Address */
#define AT91_BASE_PWMC_CH1 0xfffcc220 /* (PWMC_CH1) Base Address */
#define AT91_BASE_PWMC_CH0 0xfffcc200 /* (PWMC_CH0) Base Address */
#define AT91_BASE_PWMC 0xfffcc000 /* (PWMC) Base Address */
#define AT91_BASE_UDP 0xfffb0000 /* (UDP) Base Address */

 /* Memory mapping definitions for at91sam7s256 */

 /* ISRAM */
#define AT91_ISRAM	 (0x00200000) /* Internal SRAM base address */
#define AT91_ISRAM_SIZE	 (0x00010000) /* Internal SRAM size in byte (64 Kbytes) */
 /* IFLASH */
#define AT91_IFLASH	 (0x00100000) /* Internal FLASH base address */
#define AT91_IFLASH_SIZE	 (0x00040000) /* Internal FLASH size in byte (256 Kbytes) */
#define AT91_IFLASH_PAGE_SIZE	 (256) /* Internal FLASH Page Size: 256 bytes */
#define AT91_IFLASH_LOCK_REGION_SIZE	 (16384) /* Internal FLASH Lock Region Size: 16 Kbytes */
#define AT91_IFLASH_NB_OF_PAGES	 (1024) /* Internal FLASH Number of Pages: 1024 bytes */
#define AT91_IFLASH_NB_OF_LOCK_BITS	 (16) /* Internal FLASH Number of Lock Bits: 16 bytes */

#ifndef __ASSEMBLER__

#define AT91_DBGU ((struct at91_dbgu *)AT91_BASE_DBGU)
#define AT91_PMC ((struct at91_pmc *)AT91_BASE_PMC)
#define AT91_PIOA ((struct at91_pio *)AT91_BASE_PIOA)
#define AT91_PIOB ((struct at91_pio *)AT91_BASE_PIOB)
#define AT91_SMC ((struct at91_smc *)AT91_BASE_SMC)
#define AT91_WDTC ((struct at91_wdtc *)AT91_BASE_WDTC)
#define AT91_SDRAMC ((struct at91_sdramc *)AT91_BASE_SDRAMC)
#define AT91_CCFG ((struct at91_ccfg *)AT91_BASE_CCFG)
#define AT91_RSTC ((struct at91_rstc *)AT91_BASE_RSTC)
#define AT91_PITC ((struct at91_pitc *)AT91_BASE_PITC)
#define AT91_RTTC ((struct at91_rttc *)AT91_BASE_RTTC)

#define AT91_AIC ((struct at91_aic *)AT91_BASE_AIC)

#define AT91_TCB ((struct at91_tcb *)AT91_BASE_TCB)
#define AT91_TC0 ((struct at91_tc *)AT91_BASE_TC0)
#define AT91_TC1 ((struct at91_tc *)AT91_BASE_TC1)
#define AT91_TC2 ((struct at91_tc *)AT91_BASE_TC2)

#define AT91_US0 ((struct at91_us *)AT91_BASE_US0)
#define AT91_US1 ((struct at91_us *)AT91_BASE_US1)

#define AT91_MC ((struct at91_mc *)AT91_BASE_MC)
#define AT91_VREG ((struct at91_vreg *)AT91_BASE_VREG)
#define AT91_PWMC ((struct at91_pwmc *)AT91_BASE_PWMC)

#define AT91_UDP ((struct at91_udp *)AT91_BASE_UDP)

#endif /* __ASSEMBLER__ */

#endif /* __AT91SAM7S256_H__ */

