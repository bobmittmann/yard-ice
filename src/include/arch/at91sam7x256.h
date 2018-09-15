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
 * @file arch/at91sam7x256.h
 * @brief ARM CPU macros and definitions 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __AT91SAM7X256_H__
#define __AT91SAM7X256_H__

// Hardware register definition

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR System Peripherals
// *****************************************************************************

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif /* __ASSEMBLER__ */

#include <at91sam/at91sam_ebi.h>
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
#include <at91sam/at91sam_emac.h>
#include <at91sam/at91sam_udp.h>
#include <at91sam/at91sam_vreg.h>
#include <at91sam/at91sam_mc.h>
#include <at91sam/at91sam_pwmc.h>

/* REGISTER ADDRESS DEFINITION FOR AT91SAM9260 */

/* TODO: verificar se existem estas definicoes no microcontrolador */

/* Register definition for SYS peripheral */
#define SYS_GPBR3           (0xffffffff) /* (SYS) General Purpose Register 3 */
#define SYS_GPBR1           (0xffffffff) /* (SYS) General Purpose Register 1 */
#define SYS_GPBR2           (0xffffffff) /* (SYS) General Purpose Register 2 */
#define SYS_GPBR0           (0xffffffff) /* (SYS) General Purpose Register 0 */

/* PIO definitions for at91sam7x256 */

#define PIO_PA0        (1 <<  0) // Pin Controlled by PA0
#define PA0_RXD0     (PIO_PA0) //  USART 0 Receive Data
#define PIO_PA1        (1 <<  1) // Pin Controlled by PA1
#define PA1_TXD0     (PIO_PA1) //  USART 0 Transmit Data
#define PIO_PA10       (1 << 10) // Pin Controlled by PA10
#define PA10_TWD      (PIO_PA10) //  TWI Two-wire Serial Data
#define PIO_PA11       (1 << 11) // Pin Controlled by PA11
#define PA11_TWCK     (PIO_PA11) //  TWI Two-wire Serial Clock
#define PIO_PA12       (1 << 12) // Pin Controlled by PA12
#define PA12_SPI0_NPCS0 (PIO_PA12) //  SPI 0 Peripheral Chip Select 0
#define PIO_PA13       (1 << 13) // Pin Controlled by PA13
#define PA13_SPI0_NPCS1 (PIO_PA13) //  SPI 0 Peripheral Chip Select 1
#define PA13_PCK1     (PIO_PA13) //  PMC Programmable Clock Output 1
#define PIO_PA14       (1 << 14) // Pin Controlled by PA14
#define PA14_SPI0_NPCS2 (PIO_PA14) //  SPI 0 Peripheral Chip Select 2
#define PA14_IRQ1     (PIO_PA14) //  External Interrupt 1
#define PIO_PA15       (1 << 15) // Pin Controlled by PA15
#define PA15_SPI0_NPCS3 (PIO_PA15) //  SPI 0 Peripheral Chip Select 3
#define PA15_TCLK2    (PIO_PA15) //  Timer Counter 2 external clock input
#define PIO_PA16       (1 << 16) // Pin Controlled by PA16
#define PA16_SPI0_MISO (PIO_PA16) //  SPI 0 Master In Slave
#define PIO_PA17       (1 << 17) // Pin Controlled by PA17
#define PA17_SPI0_MOSI (PIO_PA17) //  SPI 0 Master Out Slave
#define PIO_PA18       (1 << 18) // Pin Controlled by PA18
#define PA18_SPI0_SPCK (PIO_PA18) //  SPI 0 Serial Clock
#define PIO_PA19       (1 << 19) // Pin Controlled by PA19
#define PA19_CANRX    (PIO_PA19) //  CAN Receive
#define PIO_PA2        (1 <<  2) // Pin Controlled by PA2
#define PA2_SCK0     (PIO_PA2) //  USART 0 Serial Clock
#define PA2_SPI1_NPCS1 (PIO_PA2) //  SPI 1 Peripheral Chip Select 1
#define PIO_PA20       (1 << 20) // Pin Controlled by PA20
#define PA20_CANTX    (PIO_PA20) //  CAN Transmit
#define PIO_PA21       (1 << 21) // Pin Controlled by PA21
#define PA21_TF       (PIO_PA21) //  SSC Transmit Frame Sync
#define PA21_SPI1_NPCS0 (PIO_PA21) //  SPI 1 Peripheral Chip Select 0
#define PIO_PA22       (1 << 22) // Pin Controlled by PA22
#define PA22_TK       (PIO_PA22) //  SSC Transmit Clock
#define PA22_SPI1_SPCK (PIO_PA22) //  SPI 1 Serial Clock
#define PIO_PA23       (1 << 23) // Pin Controlled by PA23
#define PA23_TD       (PIO_PA23) //  SSC Transmit data
#define PA23_SPI1_MOSI (PIO_PA23) //  SPI 1 Master Out Slave
#define PIO_PA24       (1 << 24) // Pin Controlled by PA24
#define PA24_RD       (PIO_PA24) //  SSC Receive Data
#define PA24_SPI1_MISO (PIO_PA24) //  SPI 1 Master In Slave
#define PIO_PA25       (1 << 25) // Pin Controlled by PA25
#define PA25_RK       (PIO_PA25) //  SSC Receive Clock
#define PA25_SPI1_NPCS1 (PIO_PA25) //  SPI 1 Peripheral Chip Select 1
#define PIO_PA26       (1 << 26) // Pin Controlled by PA26
#define PA26_RF       (PIO_PA26) //  SSC Receive Frame Sync
#define PA26_SPI1_NPCS2 (PIO_PA26) //  SPI 1 Peripheral Chip Select 2
#define PIO_PA27       (1 << 27) // Pin Controlled by PA27
#define PA27_DRXD     (PIO_PA27) //  DBGU Debug Receive Data
#define PA27_PCK3     (PIO_PA27) //  PMC Programmable Clock Output 3
#define PIO_PA28       (1 << 28) // Pin Controlled by PA28
#define PA28_DTXD     (PIO_PA28) //  DBGU Debug Transmit Data
#define PIO_PA29       (1 << 29) // Pin Controlled by PA29
#define PA29_FIQ      (PIO_PA29) //  AIC Fast Interrupt Input
#define PA29_SPI1_NPCS3 (PIO_PA29) //  SPI 1 Peripheral Chip Select 3
#define PIO_PA3        (1 <<  3) // Pin Controlled by PA3
#define PA3_RTS0     (PIO_PA3) //  USART 0 Ready To Send
#define PA3_SPI1_NPCS2 (PIO_PA3) //  SPI 1 Peripheral Chip Select 2
#define PIO_PA30       (1 << 30) // Pin Controlled by PA30
#define PA30_IRQ0     (PIO_PA30) //  External Interrupt 0
#define PA30_PCK2     (PIO_PA30) //  PMC Programmable Clock Output 2
#define PIO_PA4        (1 <<  4) // Pin Controlled by PA4
#define PA4_CTS0     (PIO_PA4) //  USART 0 Clear To Send
#define PA4_SPI1_NPCS3 (PIO_PA4) //  SPI 1 Peripheral Chip Select 3
#define PIO_PA5        (1 <<  5) // Pin Controlled by PA5
#define PA5_RXD1     (PIO_PA5) //  USART 1 Receive Data
#define PIO_PA6        (1 <<  6) // Pin Controlled by PA6
#define PA6_TXD1     (PIO_PA6) //  USART 1 Transmit Data
#define PIO_PA7        (1 <<  7) // Pin Controlled by PA7
#define PA7_SCK1     (PIO_PA7) //  USART 1 Serial Clock
#define PA7_SPI0_NPCS1 (PIO_PA7) //  SPI 0 Peripheral Chip Select 1
#define PIO_PA8        (1 <<  8) // Pin Controlled by PA8
#define PA8_RTS1     (PIO_PA8) //  USART 1 Ready To Send
#define PA8_SPI0_NPCS2 (PIO_PA8) //  SPI 0 Peripheral Chip Select 2
#define PIO_PA9        (1 <<  9) // Pin Controlled by PA9
#define PA9_CTS1     (PIO_PA9) //  USART 1 Clear To Send
#define PA9_SPI0_NPCS3 (PIO_PA9) //  SPI 0 Peripheral Chip Select 3
#define PIO_PB0        (1 <<  0) // Pin Controlled by PB0
#define PB0_ETXCK_EREFCK (PIO_PB0) //  Ethernet MAC Transmit Clock/Reference Clock
#define PB0_PCK0     (PIO_PB0) //  PMC Programmable Clock Output 0
#define PIO_PB1        (1 <<  1) // Pin Controlled by PB1
#define PB1_ETXEN    (PIO_PB1) //  Ethernet MAC Transmit Enable
#define PIO_PB10       (1 << 10) // Pin Controlled by PB10
#define PB10_ETX2     (PIO_PB10) //  Ethernet MAC Transmit Data 2
#define PB10_SPI1_NPCS1 (PIO_PB10) //  SPI 1 Peripheral Chip Select 1
#define PIO_PB11       (1 << 11) // Pin Controlled by PB11
#define PB11_ETX3     (PIO_PB11) //  Ethernet MAC Transmit Data 3
#define PB11_SPI1_NPCS2 (PIO_PB11) //  SPI 1 Peripheral Chip Select 2
#define PIO_PB12       (1 << 12) // Pin Controlled by PB12
#define PB12_ETXER    (PIO_PB12) //  Ethernet MAC Transmikt Coding Error
#define PB12_TCLK0    (PIO_PB12) //  Timer Counter 0 external clock input
#define PIO_PB13       (1 << 13) // Pin Controlled by PB13
#define PB13_ERX2     (PIO_PB13) //  Ethernet MAC Receive Data 2
#define PB13_SPI0_NPCS1 (PIO_PB13) //  SPI 0 Peripheral Chip Select 1
#define PIO_PB14       (1 << 14) // Pin Controlled by PB14
#define PB14_ERX3     (PIO_PB14) //  Ethernet MAC Receive Data 3
#define PB14_SPI0_NPCS2 (PIO_PB14) //  SPI 0 Peripheral Chip Select 2
#define PIO_PB15       (1 << 15) // Pin Controlled by PB15
#define PB15_ERXDV_ECRSDV (PIO_PB15) //  Ethernet MAC Receive Data Valid
#define PIO_PB16       (1 << 16) // Pin Controlled by PB16
#define PB16_ECOL     (PIO_PB16) //  Ethernet MAC Collision Detected
#define PB16_SPI1_NPCS3 (PIO_PB16) //  SPI 1 Peripheral Chip Select 3
#define PIO_PB17       (1 << 17) // Pin Controlled by PB17
#define PB17_ERXCK    (PIO_PB17) //  Ethernet MAC Receive Clock
#define PB17_SPI0_NPCS3 (PIO_PB17) //  SPI 0 Peripheral Chip Select 3
#define PIO_PB18       (1 << 18) // Pin Controlled by PB18
#define PB18_EF100    (PIO_PB18) //  Ethernet MAC Force 100 Mbits/sec
#define PB18_ADTRG    (PIO_PB18) //  ADC External Trigger
#define PIO_PB19       (1 << 19) // Pin Controlled by PB19
#define PB19_PWM0     (PIO_PB19) //  PWM Channel 0
#define PB19_TCLK1    (PIO_PB19) //  Timer Counter 1 external clock input
#define PIO_PB2        (1 <<  2) // Pin Controlled by PB2
#define PB2_ETX0     (PIO_PB2) //  Ethernet MAC Transmit Data 0
#define PIO_PB20       (1 << 20) // Pin Controlled by PB20
#define PB20_PWM1     (PIO_PB20) //  PWM Channel 1
#define PB20_PCK0     (PIO_PB20) //  PMC Programmable Clock Output 0
#define PIO_PB21       (1 << 21) // Pin Controlled by PB21
#define PB21_PWM2     (PIO_PB21) //  PWM Channel 2
#define PB21_PCK1     (PIO_PB21) //  PMC Programmable Clock Output 1
#define PIO_PB22       (1 << 22) // Pin Controlled by PB22
#define PB22_PWM3     (PIO_PB22) //  PWM Channel 3
#define PB22_PCK2     (PIO_PB22) //  PMC Programmable Clock Output 2
#define PIO_PB23       (1 << 23) // Pin Controlled by PB23
#define PB23_TIOA0    (PIO_PB23) //  Timer Counter 0 Multipurpose Timer I/O Pin A
#define PB23_DCD1     (PIO_PB23) //  USART 1 Data Carrier Detect
#define PIO_PB24       (1 << 24) // Pin Controlled by PB24
#define PB24_TIOB0    (PIO_PB24) //  Timer Counter 0 Multipurpose Timer I/O Pin B
#define PB24_DSR1     (PIO_PB24) //  USART 1 Data Set ready
#define PIO_PB25       (1 << 25) // Pin Controlled by PB25
#define PB25_TIOA1    (PIO_PB25) //  Timer Counter 1 Multipurpose Timer I/O Pin A
#define PB25_DTR1     (PIO_PB25) //  USART 1 Data Terminal ready
#define PIO_PB26       (1 << 26) // Pin Controlled by PB26
#define PB26_TIOB1    (PIO_PB26) //  Timer Counter 1 Multipurpose Timer I/O Pin B
#define PB26_RI1      (PIO_PB26) //  USART 1 Ring Indicator
#define PIO_PB27       (1 << 27) // Pin Controlled by PB27
#define PB27_TIOA2    (PIO_PB27) //  Timer Counter 2 Multipurpose Timer I/O Pin A
#define PB27_PWM0     (PIO_PB27) //  PWM Channel 0
#define PIO_PB28       (1 << 28) // Pin Controlled by PB28
#define PB28_TIOB2    (PIO_PB28) //  Timer Counter 2 Multipurpose Timer I/O Pin B
#define PB28_PWM1     (PIO_PB28) //  PWM Channel 1
#define PIO_PB29       (1 << 29) // Pin Controlled by PB29
#define PB29_PCK1     (PIO_PB29) //  PMC Programmable Clock Output 1
#define PB29_PWM2     (PIO_PB29) //  PWM Channel 2
#define PIO_PB3        (1 <<  3) // Pin Controlled by PB3
#define PB3_ETX1     (PIO_PB3) //  Ethernet MAC Transmit Data 1
#define PIO_PB30       (1 << 30) // Pin Controlled by PB30
#define PB30_PCK2     (PIO_PB30) //  PMC Programmable Clock Output 2
#define PB30_PWM3     (PIO_PB30) //  PWM Channel 3
#define PIO_PB4        (1 <<  4) // Pin Controlled by PB4
#define PB4_ECRS     (PIO_PB4) //  Ethernet MAC Carrier Sense/Carrier Sense and Data Valid
#define PIO_PB5        (1 <<  5) // Pin Controlled by PB5
#define PB5_ERX0     (PIO_PB5) //  Ethernet MAC Receive Data 0
#define PIO_PB6        (1 <<  6) // Pin Controlled by PB6
#define PB6_ERX1     (PIO_PB6) //  Ethernet MAC Receive Data 1
#define PIO_PB7        (1 <<  7) // Pin Controlled by PB7
#define PB7_ERXER    (PIO_PB7) //  Ethernet MAC Receive Error
#define PIO_PB8        (1 <<  8) // Pin Controlled by PB8
#define PB8_EMDC      (PIO_PB8) //  Ethernet MAC Management Data Clock
#define PIO_PB9        (1 <<  9) // Pin Controlled by PB9
#define PB9_EMDIO    (PIO_PB9) //  Ethernet MAC Management Data Input/Output

/* Peripheral id definitions for at91sam9260 */

#define AT91_ID_FIQ    ( 0) /* Advanced Interrupt Controller (FIQ) */
#define AT91_ID_SYS    ( 1) /* System Peripheral */
#define AT91_ID_PIOA   ( 2) /* Parallel IO Controller A */
#define AT91_ID_PIOB   ( 3) /* Parallel IO Controller B */
#define AT91_ID_SPI0   ( 4) /* Serial Peripheral Interface 0 */
#define AT91_ID_SPI1   ( 5) /* Serial Peripheral Interface 1 */
#define AT91_ID_US0    ( 6) /* USART 0 */
#define AT91_ID_US1    ( 7) /* USART 1 */
#define AT91_ID_SSC    ( 8) /* Serial Synchronous Controller */
#define AT91_ID_TWI    ( 9) /* Two-Wire Interface */
#define AT91_ID_PWMC   (10) /* PWM Controller */
#define AT91_ID_UDP    (11) /* USB Device Port */
#define AT91_ID_TC0    (12) /* Timer Counter 0 */
#define AT91_ID_TC1    (13) /* Timer Counter 1 */
#define AT91_ID_TC2    (14) /* Timer Counter 2 */
#define AT91_ID_CAN    (15) /* Control Area Network Controller */
#define AT91_ID_EMAC   (16) /* Ethernet MAC */
#define AT91_ID_ADC    (17) /* Analog-to-Digital Converter */
#define AT91_ID_IRQ0   (30) /* Advanced Interrupt Controller (IRQ0) */
#define AT91_ID_IRQ1   (31) /* Advanced Interrupt Controller (IRQ1) */
#define AT91_ALL_INT   (0xc003ffff) /* all valid interrupts */

/* Base address definitions for at91sam9260 */

#define AT91_BASE_SYS       0xfffff000 /* (SYS) Base Address */
#define AT91_BASE_AIC       0xfffff000 /* (AIC) Base Address */
#define AT91_BASE_PDC_DBGU  0xfffff300 /* (PDC_DBGU) Base Address */
#define AT91_BASE_DBGU      0xfffff200 /* (DBGU) Base Address */
#define AT91_BASE_PIOA      0xfffff400 /* (PIOA) Base Address */
#define AT91_BASE_PIOB      0xfffff600 /* (PIOB) Base Address */
#define AT91_BASE_CKGR      0xfffffc20 /* (CKGR) Base Address */
#define AT91_BASE_PMC       0xfffffc00 /* (PMC) Base Address */
#define AT91_BASE_RSTC      0xfffffd00 /* (RSTC) Base Address */
#define AT91_BASE_RTTC      0xfffffd20 /* (RTTC) Base Address */
#define AT91_BASE_PITC      0xfffffd30 /* (PITC) Base Address */
#define AT91_BASE_WDTC      0xfffffd40 /* (WDTC) Base Address */
#define AT91_BASE_VREG      0xfffffd60 /* (VREG) Base Address */
#define AT91_BASE_MC        0xffffff00 /* (MC) Base Address */
#define AT91_BASE_PDC_SPI1  0xfffe4100 /* (PDC_SPI1) Base Address */
#define AT91_BASE_SPI1      0xfffe4000 /* (SPI1) Base Address */
#define AT91_BASE_PDC_SPI0  0xfffe0100 /* (PDC_SPI0) Base Address */
#define AT91_BASE_SPI0      0xfffe0000 /* (SPI0) Base Address */
#define AT91_BASE_PDC_US1   0xfffc4100 /* (PDC_US1) Base Address */
#define AT91_BASE_US1       0xfffc4000 /* (US1) Base Address */
#define AT91_BASE_PDC_US0   0xfffc0100 /* (PDC_US0) Base Address */
#define AT91_BASE_US0       0xfffc0000 /* (US0) Base Address */
#define AT91_BASE_PDC_SSC   0xfffd4100 /* (PDC_SSC) Base Address */
#define AT91_BASE_SSC       0xfffd4000 /* (SSC) Base Address */
#define AT91_BASE_TWI       0xfffb8000 /* (TWI) Base Address */
#define AT91_BASE_PWMC_CH3  0xfffcc260 /* (PWMC_CH3) Base Address */
#define AT91_BASE_PWMC_CH2  0xfffcc240 /* (PWMC_CH2) Base Address */
#define AT91_BASE_PWMC_CH1  0xfffcc220 /* (PWMC_CH1) Base Address */
#define AT91_BASE_PWMC_CH0  0xfffcc200 /* (PWMC_CH0) Base Address */
#define AT91_BASE_PWMC      0xfffcc000 /* (PWMC) Base Address */
#define AT91_BASE_UDP       0xfffb0000 /* (UDP) Base Address */

#define AT91_BASE_TCB       0xfffa0000 /* (TCB) Base Address */
#define AT91_BASE_TC0       0xfffa0000 /* (TC0) Base Address */
#define AT91_BASE_TC1       0xfffa0040 /* (TC1) Base Address */
#define AT91_BASE_TC2       0xfffa0080 /* (TC2) Base Address */

#define AT91_BASE_CAN_MB0   0xfffd0200 /* (CAN_MB0) Base Address */
#define AT91_BASE_CAN_MB1   0xfffd0220 /* (CAN_MB1) Base Address */
#define AT91_BASE_CAN_MB2   0xfffd0240 /* (CAN_MB2) Base Address */
#define AT91_BASE_CAN_MB3   0xfffd0260 /* (CAN_MB3) Base Address */
#define AT91_BASE_CAN_MB4   0xfffd0280 /* (CAN_MB4) Base Address */
#define AT91_BASE_CAN_MB5   0xfffd02a0 /* (CAN_MB5) Base Address */
#define AT91_BASE_CAN_MB6   0xfffd02c0 /* (CAN_MB6) Base Address */
#define AT91_BASE_CAN_MB7   0xfffd02e0 /* (CAN_MB7) Base Address */
#define AT91_BASE_CAN       0xfffd0000 /* (CAN) Base Address */
#define AT91_BASE_EMAC      0xfffdc000 /* (EMAC) Base Address */
#define AT91_BASE_PDC_ADC   0xfffd8100 /* (PDC_ADC) Base Address */
#define AT91_BASE_ADC       0xfffd8000 /* (ADC) Base Address */
#define AT91_BASE_PDC_AES   0xfffa4100 /* (PDC_AES) Base Address */
#define AT91_BASE_AES       0xfffa4000 /* (AES) Base Address */
#define AT91_BASE_PDC_TDES  0xfffa8100 /* (PDC_TDES) Base Address */
#define AT91_BASE_TDES      0xfffa8000 /* (TDES) Base Address */

/* Memory mapping definitions for at91sam7xc256 */

/* ISRAM */
/* Internal SRAM base address */
#define AT91_ISRAM	 0x00200000 
/* Internal SRAM size in byte (64 Kbytes) */
#define AT91_ISRAM_SIZE	 0x00010000 

/* IFLASH */
/* Internal FLASH base address */
#define AT91_IFLASH 0x00100000 
/* Internal FLASH size in byte (256 Kbytes) */
#define AT91_IFLASH_SIZE 0x00040000 
/* Internal FLASH Page Size: 256 bytes */
#define AT91_IFLASH_PAGE_SIZE 256 
/* Internal FLASH Lock Region Size: 16 Kbytes */
#define AT91_IFLASH_LOCK_REGION_SIZE 16384 
/* Internal FLASH Number of Pages: 1024 bytes */
#define AT91_IFLASH_NB_OF_PAGES 1024 
/* Internal FLASH Number of Lock Bits: 16 bytes */
#define AT91_IFLASH_NB_OF_LOCK_BITS 16 



#ifndef __ASSEMBLER__


#define AT91_CCFG ((struct at91_ccfg *)AT91_BASE_CCFG)
#define AT91_AIC ((struct at91_aic *)AT91_BASE_AIC)
#define AT91_DBGU ((struct at91_dbgu *)AT91_BASE_DBGU)

#define AT91_PIOA ((struct at91_pio *)AT91_BASE_PIOA)
#define AT91_PIOB ((struct at91_pio *)AT91_BASE_PIOB)

#define AT91_PMC ((struct at91_pmc *)AT91_BASE_PMC)
#define AT91_CKGR ((struct at91_ckgr *)AT91_BASE_CKGR)
#define AT91_RSTC ((struct at91_rstc *)AT91_BASE_RSTC)
#define AT91_SHDWC ((struct at91_shdwc *)AT91_BASE_SHDWC)
#define AT91_RTTC ((struct at91_rttc *)AT91_BASE_RTTC)
#define AT91_PITC ((struct at91_pitc *)AT91_BASE_PITC)
#define AT91_WDTC ((struct at91_wdtc *)AT91_BASE_WDTC)

#define AT91_TCB ((struct at91_tcb *)AT91_BASE_TCB)
#define AT91_TC0 ((struct at91_tc *)AT91_BASE_TC0)
#define AT91_TC1 ((struct at91_tc *)AT91_BASE_TC1)
#define AT91_TC2 ((struct at91_tc *)AT91_BASE_TC2)

#define AT91_UDP ((struct at91_udp *)AT91_BASE_UDP)
#define AT91_TWI ((struct at91_twi *)AT91_BASE_TWI)
#define AT91_US0 ((struct at91_us *)AT91_BASE_US0)
#define AT91_US1 ((struct at91_us *)AT91_BASE_US1)
#define AT91_SSC ((struct at91_ssc *)AT91_BASE_SSC)
#define AT91_EMAC ((struct at91_emac *)AT91_BASE_EMAC)
#define AT91_SPI0 ((struct at91_spi *)AT91_BASE_SPI0)
#define AT91_SPI1 ((struct at91_spi *)AT91_BASE_SPI1)
#define AT91_ADC ((struct at91_adc *)AT91_BASE_ADC)
#define AT91_MC ((struct at91_mc *)AT91_BASE_MC)
#define AT91_VREG ((struct at91_vreg *)AT91_BASE_VREG)

#define AT91_PWMC ((struct at91_pwmc *)AT91_BASE_PWMC)
#define AT91_PWMC_CH0 ((struct at91_pwmc_ch *)AT91_BASE_PWMC_CH0)
#define AT91_PWMC_CH1 ((struct at91_pwmc_ch *)AT91_BASE_PWMC_CH1)
#define AT91_PWMC_CH2 ((struct at91_pwmc_ch *)AT91_BASE_PWMC_CH2)
#define AT91_PWMC_CH3 ((struct at91_pwmc_ch *)AT91_BASE_PWMC_CH3)
#define AT91_PWMC_CH(N) &(((struct at91_pwmc *)AT91_BASE_PWMC)->ch[(N)])

#endif /* __ASSEMBLER__ */

#endif /* __AT91SAM9260_H__ */
