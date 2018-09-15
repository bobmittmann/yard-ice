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
 * @file arch/at91sam9260.h
 * @brief ARM CPU macros and definitions 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */

#ifndef __AT91SAM9260_H__
#define __AT91SAM9260_H__

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif /* __ASSEMBLER__ */

#include <at91sam/at91sam_ebi.h>
#include <at91sam/at91sam_ecc.h>
#include <at91sam/at91sam_sdram.h>
#include <at91sam/at91sam_smc.h>
#include <at91sam/at91sam_matrix.h>
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
#include <at91sam/at91sam_mci.h>
#include <at91sam/at91sam_twi.h>
#include <at91sam/at91sam_us.h>
#include <at91sam/at91sam_ssc.h>
#include <at91sam/at91sam_spi.h>
#include <at91sam/at91sam_adc.h>
#include <at91sam/at91sam_emac.h>
#include <at91sam/at91sam_udp.h>
#include <at91sam/at91sam_uhp.h>
#include <at91sam/at91sam_isi.h>

/* Register definition for SYS peripheral */
#define SYS_GPBR3           (0xffffffff) /* (SYS) General Purpose Register 3 */
#define SYS_GPBR1           (0xffffffff) /* (SYS) General Purpose Register 1 */
#define SYS_GPBR2           (0xffffffff) /* (SYS) General Purpose Register 2 */
#define SYS_GPBR0           (0xffffffff) /* (SYS) General Purpose Register 0 */

/* PIO Definitions for AT91SAM9260 */
#define PIO_PA0             (1 <<  0) /* Pin Controlled by PA0 */
#define PA0_SPI0_MISO       (PIO_PA0) /* SPI 0 Master In Slave */
#define PA0_MCDB0           (PIO_PA0) /* Multimedia Card B Data 0 */
#define PIO_PA1             (1 <<  1) /* Pin Controlled by PA1 */
#define PA1_SPI0_MOSI       (PIO_PA1) /* SPI 0 Master Out Slave */
#define PA1_MCCDB           (PIO_PA1) /* Multimedia Card B Command */
#define PIO_PA10            (1 << 10) /* Pin Controlled by PA10 */
#define PA10_MCDA2          (PIO_PA10) /* Multimedia Card A Data 2 */
#define PA10_ETX2           (PIO_PA10) /* Ethernet MAC Transmit Data 2 */
#define PIO_PA11            (1 << 11) /* Pin Controlled by PA11 */
#define PA11_MCDA3          (PIO_PA11) /* Multimedia Card A Data 3 */
#define PA11_ETX3           (PIO_PA11) /* Ethernet MAC Transmit Data 3 */
#define PIO_PA12            (1 << 12) /* Pin Controlled by PA12 */
#define PA12_ETX0           (PIO_PA12) /* Ethernet MAC Transmit Data 0 */
#define PIO_PA13            (1 << 13) /* Pin Controlled by PA13 */
#define PA13_ETX1           (PIO_PA13) /* Ethernet MAC Transmit Data 1 */
#define PIO_PA14            (1 << 14) /* Pin Controlled by PA14 */
#define PA14_ERX0           (PIO_PA14) /* Ethernet MAC Receive Data 0 */
#define PIO_PA15            (1 << 15) /* Pin Controlled by PA15 */
#define PA15_ERX1           (PIO_PA15) /* Ethernet MAC Receive Data 1 */
#define PIO_PA16            (1 << 16) /* Pin Controlled by PA16 */
#define PA16_ETXEN          (PIO_PA16) /* Ethernet MAC Transmit Enable */
#define PIO_PA17            (1 << 17) /* Pin Controlled by PA17 */
#define PA17_ERXDV          (PIO_PA17) /* Ethernet MAC Receive Data Valid */
#define PIO_PA18            (1 << 18) /* Pin Controlled by PA18 */
#define PA18_ERXER          (PIO_PA18) /* Ethernet MAC Receive Error */
#define PIO_PA19            (1 << 19) /* Pin Controlled by PA19 */
#define PA19_ETXCK          (PIO_PA19) /* Ethernet MAC Transmit Clock/Reference Clock */
#define PIO_PA2             (1 <<  2) /* Pin Controlled by PA2 */
#define PA2_SPI0_SPCK       (PIO_PA2) /* SPI 0 Serial Clock */
#define PIO_PA20            (1 << 20) /* Pin Controlled by PA20 */
#define PA20_EMDC           (PIO_PA20) /* Ethernet MAC Management Data Clock */
#define PIO_PA21            (1 << 21) /* Pin Controlled by PA21 */
#define PA21_EMDIO          (PIO_PA21) /* Ethernet MAC Management Data Input/Output */
#define PIO_PA22            (1 << 22) /* Pin Controlled by PA22 */
#define PA22_ADTRG          (PIO_PA22) /* ADC Trigger */
#define PA22_ETXER          (PIO_PA22) /* Ethernet MAC Transmikt Coding Error */
#define PIO_PA23            (1 << 23) /* Pin Controlled by PA23 */
#define PA23_TWD            (PIO_PA23) /* TWI Two-wire Serial Data */
#define PA23_ETX2           (PIO_PA23) /* Ethernet MAC Transmit Data 2 */
#define PIO_PA24            (1 << 24) /* Pin Controlled by PA24 */
#define PA24_TWCK           (PIO_PA24) /* TWI Two-wire Serial Clock */
#define PA24_ETX3           (PIO_PA24) /* Ethernet MAC Transmit Data 3 */
#define PIO_PA25            (1 << 25) /* Pin Controlled by PA25 */
#define PA25_TCLK0          (PIO_PA25) /* Timer Counter 0 external clock input */
#define PA25_ERX2           (PIO_PA25) /* Ethernet MAC Receive Data 2 */
#define PIO_PA26            (1 << 26) /* Pin Controlled by PA26 */
#define PA26_TIOA0          (PIO_PA26) /* Timer Counter 0 Multipurpose Timer I/O Pin A */
#define PA26_ERX3           (PIO_PA26) /* Ethernet MAC Receive Data 3 */
#define PIO_PA27            (1 << 27) /* Pin Controlled by PA27 */
#define PA27_TIOA1          (PIO_PA27) /* Timer Counter 1 Multipurpose Timer I/O Pin A */
#define PA27_ERXCK          (PIO_PA27) /* Ethernet MAC Receive Clock */
#define PIO_PA28            (1 << 28) /* Pin Controlled by PA28 */
#define PA28_TIOA2          (PIO_PA28) /* Timer Counter 2 Multipurpose Timer I/O Pin A */
#define PA28_ECRS           (PIO_PA28) /* Ethernet MAC Carrier Sense/Carrier Sense and Data Valid */
#define PIO_PA29            (1 << 29) /* Pin Controlled by PA29 */
#define PA29_SCK1           (PIO_PA29) /* USART 1 Serial Clock */
#define PA29_ECOL           (PIO_PA29) /* Ethernet MAC Collision Detected */
#define PIO_PA3             (1 <<  3) /* Pin Controlled by PA3 */
#define PA3_SPI0_NPCS0      (PIO_PA3) /* SPI 0 Peripheral Chip Select 0 */
#define PA3_MCDB3           (PIO_PA3) /* Multimedia Card B Data 3 */
#define PIO_PA30            (1 << 30) /* Pin Controlled by PA30 */
#define PA30_SCK2           (PIO_PA30) /* USART 2 Serial Clock */
#define PA30_RXD4           (PIO_PA30) /* USART 4 Receive Data */
#define PIO_PA31            (1 << 31) /* Pin Controlled by PA31 */
#define PA31_SCK0           (PIO_PA31) /* USART 0 Serial Clock */
#define PA31_TXD4           (PIO_PA31) /* USART 4 Transmit Data */
#define PIO_PA4             (1 <<  4) /* Pin Controlled by PA4 */
#define PA4_RTS2            (PIO_PA4) /* USART 2 Ready To Send */
#define PA4_MCDB2           (PIO_PA4) /* Multimedia Card B Data 2 */
#define PIO_PA5             (1 <<  5) /* Pin Controlled by PA5 */
#define PA5_CTS2            (PIO_PA5) /* USART 2 Clear To Send */
#define PA5_MCDB1           (PIO_PA5) /* Multimedia Card B Data 1 */
#define PIO_PA6             (1 <<  6) /* Pin Controlled by PA6 */
#define PA6_MCDA0           (PIO_PA6) /* Multimedia Card A Data 0 */
#define PIO_PA7             (1 <<  7) /* Pin Controlled by PA7 */
#define PA7_MCCDA           (PIO_PA7) /* Multimedia Card A Command */
#define PIO_PA8             (1 <<  8) /* Pin Controlled by PA8 */
#define PA8_MCCK            (PIO_PA8) /* Multimedia Card Clock */
#define PIO_PA9             (1 <<  9) /* Pin Controlled by PA9 */
#define PA9_MCDA1           (PIO_PA9) /* Multimedia Card A Data 1 */
#define PIO_PB0             (1 <<  0) /* Pin Controlled by PB0 */
#define PB0_SPI1_MISO       (PIO_PB0) /* SPI 1 Master In Slave */
#define PB0_TIOA3           (PIO_PB0) /* Timer Counter 3 Multipurpose Timer I/O Pin A */
#define PIO_PB1             (1 <<  1) /* Pin Controlled by PB1 */
#define PB1_SPI1_MOSI       (PIO_PB1) /* SPI 1 Master Out Slave */
#define PB1_TIOB3           (PIO_PB1) /* Timer Counter 3 Multipurpose Timer I/O Pin B */
#define PIO_PB10            (1 << 10) /* Pin Controlled by PB10 */
#define PB10_TXD3           (PIO_PB10) /* USART 3 Transmit Data */
#define PB10_ISI_D8         (PIO_PB10) /* Image Sensor Data 8 */
#define PIO_PB11            (1 << 11) /* Pin Controlled by PB11 */
#define PB11_RXD3           (PIO_PB11) /* USART 3 Receive Data */
#define PB11_ISI_D9         (PIO_PB11) /* Image Sensor Data 9 */
#define PIO_PB12            (1 << 12) /* Pin Controlled by PB12 */
#define PB12_TXD5           (PIO_PB12) /* USART 5 Transmit Data */
#define PB12_ISI_D10        (PIO_PB12) /* Image Sensor Data 10 */
#define PIO_PB13            (1 << 13) /* Pin Controlled by PB13 */
#define PB13_RXD5           (PIO_PB13) /* USART 5 Receive Data */
#define PB13_ISI_D11        (PIO_PB13) /* Image Sensor Data 11 */
#define PIO_PB14            (1 << 14) /* Pin Controlled by PB14 */
#define PB14_DRXD           (PIO_PB14) /* DBGU Debug Receive Data */
#define PIO_PB15            (1 << 15) /* Pin Controlled by PB15 */
#define PB15_DTXD           (PIO_PB15) /* DBGU Debug Transmit Data */
#define PIO_PB16            (1 << 16) /* Pin Controlled by PB16 */
#define PB16_TK0            (PIO_PB16) /* SSC0 Transmit Clock */
#define PB16_TCLK3          (PIO_PB16) /* Timer Counter 3 external clock input */
#define PIO_PB17            (1 << 17) /* Pin Controlled by PB17 */
#define PB17_TF0            (PIO_PB17) /* SSC0 Transmit Frame Sync */
#define PB17_TCLK4          (PIO_PB17) /* Timer Counter 4 external clock input */
#define PIO_PB18            (1 << 18) /* Pin Controlled by PB18 */
#define PB18_TD0            (PIO_PB18) /* SSC0 Transmit data */
#define PB18_TIOB4          (PIO_PB18) /* Timer Counter 4 Multipurpose Timer I/O Pin B */
#define PIO_PB19            (1 << 19) /* Pin Controlled by PB19 */
#define PB19_RD0            (PIO_PB19) /* SSC0 Receive Data */
#define PB19_TIOB5          (PIO_PB19) /* Timer Counter 5 Multipurpose Timer I/O Pin B */
#define PIO_PB2             (1 <<  2) /* Pin Controlled by PB2 */
#define PB2_SPI1_SPCK       (PIO_PB2) /* SPI 1 Serial Clock */
#define PB2_TIOA4           (PIO_PB2) /* Timer Counter 4 Multipurpose Timer I/O Pin A */
#define PIO_PB20            (1 << 20) /* Pin Controlled by PB20 */
#define PB20_RK0            (PIO_PB20) /* SSC0 Receive Clock */
#define PB20_ISI_D0         (PIO_PB20) /* Image Sensor Data 0 */
#define PIO_PB21            (1 << 21) /* Pin Controlled by PB21 */
#define PB21_RF0            (PIO_PB21) /* SSC0 Receive Frame Sync */
#define PB21_ISI_D1         (PIO_PB21) /* Image Sensor Data 1 */
#define PIO_PB22            (1 << 22) /* Pin Controlled by PB22 */
#define PB22_DSR0           (PIO_PB22) /* USART 0 Data Set ready */
#define PB22_ISI_D2         (PIO_PB22) /* Image Sensor Data 2 */
#define PIO_PB23            (1 << 23) /* Pin Controlled by PB23 */
#define PB23_DCD0           (PIO_PB23) /* USART 0 Data Carrier Detect */
#define PB23_ISI_D3         (PIO_PB23) /* Image Sensor Data 3 */
#define PIO_PB24            (1 << 24) /* Pin Controlled by PB24 */
#define PB24_DTR0           (PIO_PB24) /* USART 0 Data Terminal ready */
#define PB24_ISI_D4         (PIO_PB24) /* Image Sensor Data 4 */
#define PIO_PB25            (1 << 25) /* Pin Controlled by PB25 */
#define PB25_RI0            (PIO_PB25) /* USART 0 Ring Indicator */
#define PB25_ISI_D5         (PIO_PB25) /* Image Sensor Data 5 */
#define PIO_PB26            (1 << 26) /* Pin Controlled by PB26 */
#define PB26_RTS0           (PIO_PB26) /* USART 0 Ready To Send */
#define PB26_ISI_D6         (PIO_PB26) /* Image Sensor Data 6 */
#define PIO_PB27            (1 << 27) /* Pin Controlled by PB27 */
#define PB27_CTS0           (PIO_PB27) /* USART 0 Clear To Send */
#define PB27_ISI_D7         (PIO_PB27) /* Image Sensor Data 7 */
#define PIO_PB28            (1 << 28) /* Pin Controlled by PB28 */
#define PB28_RTS1           (PIO_PB28) /* USART 1 Ready To Send */
#define PB28_ISI_PCK        (PIO_PB28) /* Image Sensor Data Clock */
#define PIO_PB29            (1 << 29) /* Pin Controlled by PB29 */
#define PB29_CTS1           (PIO_PB29) /* USART 1 Clear To Send */
#define PB29_ISI_VSYNC      (PIO_PB29) /* Image Sensor Vertical Synchro */
#define PIO_PB3             (1 <<  3) /* Pin Controlled by PB3 */
#define PB3_SPI1_NPCS0      (PIO_PB3) /* SPI 1 Peripheral Chip Select 0 */
#define PB3_TIOA5           (PIO_PB3) /* Timer Counter 5 Multipurpose Timer I/O Pin A */
#define PIO_PB30            (1 << 30) /* Pin Controlled by PB30 */
#define PB30_PCK0_0         (PIO_PB30) /* PMC Programmable Clock Output 0 */
#define PB30_ISI_HSYNC      (PIO_PB30) /* Image Sensor Horizontal Synchro */
#define PIO_PB31            (1 << 31) /* Pin Controlled by PB31 */
#define PB31_PCK1_0         (PIO_PB31) /* PMC Programmable Clock Output 1 */
#define PB31_ISI_MCK        (PIO_PB31) /* Image Sensor Reference Clock */
#define PIO_PB4             (1 <<  4) /* Pin Controlled by PB4 */
#define PB4_TXD0            (PIO_PB4) /* USART 0 Transmit Data */
#define PIO_PB5             (1 <<  5) /* Pin Controlled by PB5 */
#define PB5_RXD0            (PIO_PB5) /* USART 0 Receive Data */
#define PIO_PB6             (1 <<  6) /* Pin Controlled by PB6 */
#define PB6_TXD1            (PIO_PB6) /* USART 1 Transmit Data */
#define PB6_TCLK1           (PIO_PB6) /* Timer Counter 1 external clock input */
#define PIO_PB7             (1 <<  7) /* Pin Controlled by PB7 */
#define PB7_RXD1            (PIO_PB7) /* USART 1 Receive Data */
#define PB7_TCLK2           (PIO_PB7) /* Timer Counter 2 external clock input */
#define PIO_PB8             (1 <<  8) /* Pin Controlled by PB8 */
#define PB8_TXD2            (PIO_PB8) /* USART 2 Transmit Data */
#define PIO_PB9             (1 <<  9) /* Pin Controlled by PB9 */
#define PB9_RXD2            (PIO_PB9) /* USART 2 Receive Data */
#define PIO_PC0             (1 <<  0) /* Pin Controlled by PC0 */
#define PC0_AD0             (PIO_PC0) /* ADC Analog Input 0 */
#define PC0_SCK3            (PIO_PC0) /* USART 3 Serial Clock */
#define PIO_PC1             (1 <<  1) /* Pin Controlled by PC1 */
#define PC1_AD1             (PIO_PC1) /* ADC Analog Input 1 */
#define PC1_PCK0            (PIO_PC1) /* PMC Programmable Clock Output 0 */
#define PIO_PC10            (1 << 10) /* Pin Controlled by PC10 */
#define PC10_A25_CFRNW      (PIO_PC10) /* Address Bus[25] */
#define PC10_CTS3           (PIO_PC10) /* USART 3 Clear To Send */
#define PIO_PC11            (1 << 11) /* Pin Controlled by PC11 */
#define PC11_NCS2           (PIO_PC11) /* Chip Select Line 2 */
#define PC11_SPI0_NPCS1     (PIO_PC11) /* SPI 0 Peripheral Chip Select 1 */
#define PIO_PC12            (1 << 12) /* Pin Controlled by PC12 */
#define PC12_IRQ0           (PIO_PC12) /* External Interrupt 0 */
#define PC12_NCS7           (PIO_PC12) /* Chip Select Line 7 */
#define PIO_PC13            (1 << 13) /* Pin Controlled by PC13 */
#define PC13_FIQ            (PIO_PC13) /* AIC Fast Interrupt Input */
#define PC13_NCS6           (PIO_PC13) /* Chip Select Line 6 */
#define PIO_PC14            (1 << 14) /* Pin Controlled by PC14 */
#define PC14_NCS3_NANDCS    (PIO_PC14) /* Chip Select Line 3 */
#define PC14_IRQ2           (PIO_PC14) /* External Interrupt 2 */
#define PIO_PC15            (1 << 15) /* Pin Controlled by PC15 */
#define PC15_NWAIT          (PIO_PC15) /* External Wait Signal */
#define PC15_IRQ1           (PIO_PC15) /* External Interrupt 1 */
#define PIO_PC16            (1 << 16) /* Pin Controlled by PC16 */
#define PC16_D16            (PIO_PC16) /* Data Bus[16] */
#define PC16_SPI0_NPCS2     (PIO_PC16) /* SPI 0 Peripheral Chip Select 2 */
#define PIO_PC17            (1 << 17) /* Pin Controlled by PC17 */
#define PC17_D17            (PIO_PC17) /* Data Bus[17] */
#define PC17_SPI0_NPCS3     (PIO_PC17) /* SPI 0 Peripheral Chip Select 3 */
#define PIO_PC18            (1 << 18) /* Pin Controlled by PC18 */
#define PC18_D18            (PIO_PC18) /* Data Bus[18] */
#define PC18_SPI1_NPCS1     (PIO_PC18) /* SPI 1 Peripheral Chip Select 1 */
#define PIO_PC19            (1 << 19) /* Pin Controlled by PC19 */
#define PC19_D19            (PIO_PC19) /* Data Bus[19] */
#define PC19_SPI1_NPCS2     (PIO_PC19) /* SPI 1 Peripheral Chip Select 2 */
#define PIO_PC2             (1 <<  2) /* Pin Controlled by PC2 */
#define PC2_AD2             (PIO_PC2) /* ADC Analog Input 2 */
#define PC2_PCK1            (PIO_PC2) /* PMC Programmable Clock Output 1 */
#define PIO_PC20            (1 << 20) /* Pin Controlled by PC20 */
#define PC20_D20            (PIO_PC20) /* Data Bus[20] */
#define PC20_SPI1_NPCS3     (PIO_PC20) /* SPI 1 Peripheral Chip Select 3 */
#define PIO_PC21            (1 << 21) /* Pin Controlled by PC21 */
#define PC21_D21            (PIO_PC21) /* Data Bus[21] */
#define PC21_EF100          (PIO_PC21) /* Ethernet MAC Force 100 Mbits/sec */
#define PIO_PC22            (1 << 22) /* Pin Controlled by PC22 */
#define PC22_D22            (PIO_PC22) /* Data Bus[22] */
#define PC22_TCLK5          (PIO_PC22) /* Timer Counter 5 external clock input */
#define PIO_PC23            (1 << 23) /* Pin Controlled by PC23 */
#define PC23_D23            (PIO_PC23) /* Data Bus[23] */
#define PIO_PC24            (1 << 24) /* Pin Controlled by PC24 */
#define PC24_D24            (PIO_PC24) /* Data Bus[24] */
#define PIO_PC25            (1 << 25) /* Pin Controlled by PC25 */
#define PC25_D25            (PIO_PC25) /* Data Bus[25] */
#define PIO_PC26            (1 << 26) /* Pin Controlled by PC26 */
#define PC26_D26            (PIO_PC26) /* Data Bus[26] */
#define PIO_PC27            (1 << 27) /* Pin Controlled by PC27 */
#define PC27_D27            (PIO_PC27) /* Data Bus[27] */
#define PIO_PC28            (1 << 28) /* Pin Controlled by PC28 */
#define PC28_D28            (PIO_PC28) /* Data Bus[28] */
#define PIO_PC29            (1 << 29) /* Pin Controlled by PC29 */
#define PC29_D29            (PIO_PC29) /* Data Bus[29] */
#define PIO_PC3             (1 <<  3) /* Pin Controlled by PC3 */
#define PC3_AD3             (PIO_PC3) /* ADC Analog Input 3 */
#define PC3_SPI1_NPCS3      (PIO_PC3) /* SPI 1 Peripheral Chip Select 3 */
#define PIO_PC30            (1 << 30) /* Pin Controlled by PC30 */
#define PC30_D30            (PIO_PC30) /* Data Bus[30] */
#define PIO_PC31            (1 << 31) /* Pin Controlled by PC31 */
#define PC31_D31            (PIO_PC31) /* Data Bus[31] */
#define PIO_PC4             (1 <<  4) /* Pin Controlled by PC4 */
#define PC4_A23             (PIO_PC4) /* Address Bus[23] */
#define PC4_SPI1_NPCS2      (PIO_PC4) /* SPI 1 Peripheral Chip Select 2 */
#define PIO_PC5             (1 <<  5) /* Pin Controlled by PC5 */
#define PC5_A24             (PIO_PC5) /* Address Bus[24] */
#define PC5_SPI1_NPCS1      (PIO_PC5) /* SPI 1 Peripheral Chip Select 1 */
#define PIO_PC6             (1 <<  6) /* Pin Controlled by PC6 */
#define PC6_TIOB2           (PIO_PC6) /* Timer Counter 2 Multipurpose Timer I/O Pin B */
#define PC6_CFCE1           (PIO_PC6) /* Compact Flash Enable 1 */
#define PIO_PC7             (1 <<  7) /* Pin Controlled by PC7 */
#define PC7_TIOB1           (PIO_PC7) /* Timer Counter 1 Multipurpose Timer I/O Pin B */
#define PC7_CFCE2           (PIO_PC7) /* Compact Flash Enable 2 */
#define PIO_PC8             (1 <<  8) /* Pin Controlled by PC8 */
#define PC8_NCS4_CFCS0      (PIO_PC8) /* Chip Select Line 4 */
#define PC8_RTS3            (PIO_PC8) /* USART 3 Ready To Send */
#define PIO_PC9             (1 <<  9) /* Pin Controlled by PC9 */
#define PC9_NCS5_CFCS1      (PIO_PC9) /* Chip Select Line 5 */
#define PC9_TIOB0           (PIO_PC9) /* Timer Counter 0 Multipurpose Timer I/O Pin B */

/* Peripheral id definitions for at91sam9260 */
#define AT91_ID_FIQ         ( 0) /* Advanced Interrupt Controller (FIQ) */
#define AT91_ID_SYS         ( 1) /* System Controller */
#define AT91_ID_PIOA        ( 2) /* Parallel IO Controller A */
#define AT91_ID_PIOB        ( 3) /* Parallel IO Controller B */
#define AT91_ID_PIOC        ( 4) /* Parallel IO Controller C */
#define AT91_ID_ADC         ( 5) /* ADC */
#define AT91_ID_US0         ( 6) /* USART 0 */
#define AT91_ID_US1         ( 7) /* USART 1 */
#define AT91_ID_US2         ( 8) /* USART 2 */
#define AT91_ID_MCI         ( 9) /* Multimedia Card Interface 0 */
#define AT91_ID_UDP         (10) /* USB Device Port */
#define AT91_ID_TWI         (11) /* Two-Wire Interface */
#define AT91_ID_SPI0        (12) /* Serial Peripheral Interface 0 */
#define AT91_ID_SPI1        (13) /* Serial Peripheral Interface 1 */
#define AT91_ID_SSC0        (14) /* Serial Synchronous Controller 0 */
#define AT91_ID_TC0         (17) /* Timer Counter 0 */
#define AT91_ID_TC1         (18) /* Timer Counter 1 */
#define AT91_ID_TC2         (19) /* Timer Counter 2 */
#define AT91_ID_UHP         (20) /* USB Host Port */
#define AT91_ID_EMAC        (21) /* Ethernet Mac */
#define AT91_ID_HISI        (22) /* Image Sensor Interface */
#define AT91_ID_US3         (23) /* USART 3 */
#define AT91_ID_US4         (24) /* USART 4 */
#define AT91_ID_US5         (25) /* USART 5 */
#define AT91_ID_TC3         (26) /* Timer Counter 3 */
#define AT91_ID_TC4         (27) /* Timer Counter 4 */
#define AT91_ID_TC5         (28) /* Timer Counter 5 */
#define AT91_ID_IRQ0        (29) /* Advanced Interrupt Controller (IRQ0) */
#define AT91_ID_IRQ1        (30) /* Advanced Interrupt Controller (IRQ1) */
#define AT91_ID_IRQ2        (31) /* Advanced Interrupt Controller (IRQ2) */
#define AT91_ALL_INT        (0xfffe7fff) /* ALL VALID INTERRUPTS */

/* Base address definitions for at91sam9260 */

#define AT91_BASE_SYS            (0xfffffd00) /* (SYS) Base Address */
#define AT91_BASE_EBI            (0xffffea00) /* (EBI) Base Address */
#define AT91_BASE_HECC           (0xffffe800) /* (HECC) Base Address */
#define AT91_BASE_SDRAMC         (0xffffea00) /* (SDRAMC) Base Address */
#define AT91_BASE_SMC            (0xffffec00) /* (SMC) Base Address */
#define AT91_BASE_MATRIX         (0xffffee00) /* (MATRIX) Base Address */
#define AT91_BASE_CCFG           (0xffffef10) /* (CCFG) Base Address */
#define AT91_BASE_PDC_DBGU       (0xfffff300) /* (PDC_DBGU) Base Address */
#define AT91_BASE_DBGU           (0xfffff200) /* (DBGU) Base Address */
#define AT91_BASE_AIC            (0xfffff000) /* (AIC) Base Address */
#define AT91_BASE_PIOA           (0xfffff400) /* (PIOA) Base Address */
#define AT91_BASE_PIOB           (0xfffff600) /* (PIOB) Base Address */
#define AT91_BASE_PIOC           (0xfffff800) /* (PIOC) Base Address */
#define AT91_BASE_CKGR           (0xfffffc20) /* (CKGR) Base Address */
#define AT91_BASE_PMC            (0xfffffc00) /* (PMC) Base Address */
#define AT91_BASE_RSTC           (0xfffffd00) /* (RSTC) Base Address */
#define AT91_BASE_SHDWC          (0xfffffd10) /* (SHDWC) Base Address */
#define AT91_BASE_RTTC           (0xfffffd20) /* (RTTC) Base Address */
#define AT91_BASE_PITC           (0xfffffd30) /* (PITC) Base Address */
#define AT91_BASE_WDTC           (0xfffffd40) /* (WDTC) Base Address */
#define AT91_BASE_TC0            (0xfffa0000) /* (TC0) Base Address */
#define AT91_BASE_TC1            (0xfffa0040) /* (TC1) Base Address */
#define AT91_BASE_TC2            (0xfffa0080) /* (TC2) Base Address */
#define AT91_BASE_TC3            (0xfffdc000) /* (TC3) Base Address */
#define AT91_BASE_TC4            (0xfffdc040) /* (TC4) Base Address */
#define AT91_BASE_TC5            (0xfffdc080) /* (TC5) Base Address */
#define AT91_BASE_TCB0           (0xfffa0000) /* (TCB0) Base Address */
#define AT91_BASE_TCB1           (0xfffdc000) /* (TCB1) Base Address */
#define AT91_BASE_PDC_MCI        (0xfffa8100) /* (PDC_MCI) Base Address */
#define AT91_BASE_MCI            (0xfffa8000) /* (MCI) Base Address */
#define AT91_BASE_PDC_TWI        (0xfffac100) /* (PDC_TWI) Base Address */
#define AT91_BASE_TWI            (0xfffac000) /* (TWI) Base Address */
#define AT91_BASE_PDC_US0        (0xfffb0100) /* (PDC_US0) Base Address */
#define AT91_BASE_US0            (0xfffb0000) /* (US0) Base Address */
#define AT91_BASE_PDC_US1        (0xfffb4100) /* (PDC_US1) Base Address */
#define AT91_BASE_US1            (0xfffb4000) /* (US1) Base Address */
#define AT91_BASE_PDC_US2        (0xfffb8100) /* (PDC_US2) Base Address */
#define AT91_BASE_US2            (0xfffb8000) /* (US2) Base Address */
#define AT91_BASE_PDC_US3        (0xfffd0100) /* (PDC_US3) Base Address */
#define AT91_BASE_US3            (0xfffd0000) /* (US3) Base Address */
#define AT91_BASE_PDC_US4        (0xfffd4100) /* (PDC_US4) Base Address */
#define AT91_BASE_US4            (0xfffd4000) /* (US4) Base Address */
#define AT91_BASE_PDC_US5        (0xfffd8100) /* (PDC_US5) Base Address */
#define AT91_BASE_US5            (0xfffd8000) /* (US5) Base Address */
#define AT91_BASE_PDC_SSC0       (0xfffbc100) /* (PDC_SSC0) Base Address */
#define AT91_BASE_SSC0           (0xfffbc000) /* (SSC0) Base Address */
#define AT91_BASE_PDC_SPI0       (0xfffc8100) /* (PDC_SPI0) Base Address */
#define AT91_BASE_SPI0           (0xfffc8000) /* (SPI0) Base Address */
#define AT91_BASE_PDC_SPI1       (0xfffcc100) /* (PDC_SPI1) Base Address */
#define AT91_BASE_SPI1           (0xfffcc000) /* (SPI1) Base Address */
#define AT91_BASE_PDC_ADC        (0xfffe0100) /* (PDC_ADC) Base Address */
#define AT91_BASE_ADC            (0xfffe0000) /* (ADC) Base Address */
#define AT91_BASE_EMACB          (0xfffc4000) /* (EMACB) Base Address */
#define AT91_BASE_UDP            (0xfffa4000) /* (UDP) Base Address */
#define AT91_BASE_UHP            (0x00500000) /* (UHP) Base Address */
#define AT91_BASE_HISI           (0xfffc0000) /* (HISI) Base Address */

/* Memory mapping definitions for at91sam9260 */
/* Internal ROM */
#define AT91_IROM 	         (0x00100000) /* Internal ROM base address */
#define AT91_IROM_SIZE	     (0x00008000) /* Internal ROM size in byte (32 Kbytes) */
// IRAM_1
#define AT91_IRAM_1	     (0x00200000) /* Maximum IRAM_1 Area : 4Kbyte base address */
#define AT91_IRAM_1_SIZE	 (0x00001000) /* Maximum IRAM_1 Area : 4Kbyte size in byte (4 Kbytes) */
// IRAM_2
#define AT91_IRAM_2	     (0x00300000) /* Maximum IRAM_2 AREA : 4 Kbyte base address */
#define AT91_IRAM_2_SIZE	 (0x00001000) /* Maximum IRAM_2 AREA : 4 Kbyte size in byte (4 Kbytes) */
// EBI_CS0
#define AT91_EBI_CS0	     (0x10000000) /* EBI Chip Select 0 base address */
#define AT91_EBI_CS0_SIZE	 (0x10000000) /* EBI Chip Select 0 size in byte (262144 Kbytes) */
// EBI_CS1
#define AT91_EBI_CS1	     (0x20000000) /* EBI Chip Select 1 base address */
#define AT91_EBI_CS1_SIZE	 (0x10000000) /* EBI Chip Select 1 size in byte (262144 Kbytes) */
// EBI_SDRAM
#define AT91_EBI_SDRAM	     (0x20000000) /* SDRAM on EBI Chip Select 1 base address */
#define AT91_EBI_SDRAM_SIZE (0x10000000) /* SDRAM on EBI Chip Select 1 size in byte (262144 Kbytes) */
// EBI_SDRAM_16BIT
#define AT91_EBI_SDRAM_16BIT	    (0x20000000) /* SDRAM on EBI Chip Select 1 base address */
#define AT91_EBI_SDRAM_16BIT_SIZE	 (0x02000000) /* SDRAM on EBI Chip Select 1 size in byte (32768 Kbytes) */
// EBI_SDRAM_32BIT
#define AT91_EBI_SDRAM_32BIT	    (0x20000000) /* SDRAM on EBI Chip Select 1 base address */
#define AT91_EBI_SDRAM_32BIT_SIZE	 (0x04000000) /* SDRAM on EBI Chip Select 1 size in byte (65536 Kbytes) */
// EBI_CS2
#define AT91_EBI_CS2	            (0x30000000) /* EBI Chip Select 2 base address */
#define AT91_EBI_CS2_SIZE	       (0x10000000) /* EBI Chip Select 2 size in byte (262144 Kbytes) */
// EBI_CS3
#define AT91_EBI_CS3	            (0x40000000) /* EBI Chip Select 3 base address */
#define AT91_EBI_CS3_SIZE	       (0x10000000) /* EBI Chip Select 3 size in byte (262144 Kbytes) */
// EBI_SM
#define AT91_EBI_SM	             (0x40000000) /* SmartMedia on Chip Select 3 base address */
#define AT91_EBI_SM_SIZE	        (0x10000000) /* SmartMedia on Chip Select 3 size in byte (262144 Kbytes) */
// EBI_CS4
#define AT91_EBI_CS4	            (0x50000000) /* EBI Chip Select 4 base address */
#define AT91_EBI_CS4_SIZE	       (0x10000000) /* EBI Chip Select 4 size in byte (262144 Kbytes) */
// EBI_CF0
#define AT91_EBI_CF0	            (0x50000000) /* CompactFlash 0 on Chip Select 4 base address */
#define AT91_EBI_CF0_SIZE	       (0x10000000) /* CompactFlash 0 on Chip Select 4 size in byte (262144 Kbytes) */
// EBI_CS5
#define AT91_EBI_CS5	            (0x60000000) /* EBI Chip Select 5 base address */
#define AT91_EBI_CS5_SIZE	       (0x10000000) /* EBI Chip Select 5 size in byte (262144 Kbytes) */
// EBI_CF1
#define AT91_EBI_CF1	            (0x60000000) /* CompactFlash 1 on Chip Select 5 base address */
#define AT91_EBI_CF1_SIZE	       (0x10000000) /* CompactFlash 1 on Chip Select 5 size in byte (262144 Kbytes) */
// EBI_CS6
#define AT91_EBI_CS6	            (0x70000000) /* EBI Chip Select 6 base address */
#define AT91_EBI_CS6_SIZE	       (0x10000000) /* EBI Chip Select 6 size in byte (262144 Kbytes) */
// EBI_CS7
#define AT91_EBI_CS7	            (0x80000000) /* EBI Chip Select 7 base address */
#define AT91_EBI_CS7_SIZE	       (0x10000000) /* EBI Chip Select 7 size in byte (262144 Kbytes) */

#define AT91_NR_PIO               (32 * 3)


#ifndef __ASSEMBLER__

struct at91_sysc {
	uint32_t res0[2560];
	struct at91_ecc ecc;
	struct at91_sdramc sdramc;
	struct at91_smc smc;
	struct at91_matrix matrix;
	struct at91_ccfg ccfg;
	struct at91_aic aic;
	struct at91_dbgu dbgu;
	struct at91_pio pioa;
	struct at91_pio piob;
	struct at91_pio pioc;
	struct at91_pio piod;
	struct at91_pmc pmc;
	struct at91_rstc rstc;
	struct at91_shdwc shdwc;
	struct at91_rttc rttc;
	struct at91_pitc pitc;
	struct at91_wdtc wdtc;
/*	struct at91_gpbr gpbr; */
};

#define AT91_ECC ((struct at91_ecc *)AT91_BASE_ECC)
#define AT91_SDRAMC ((struct at91_sdramc *)AT91_BASE_SDRAMC)
#define AT91_SMC ((struct at91_smc *)AT91_BASE_SMC)
#define AT91_MATRIX ((struct at91_matrix *)AT91_BASE_MATRIX)
#define AT91_CCFG ((struct at91_ccfg *)AT91_BASE_CCFG)
#define AT91_AIC ((struct at91_aic *)AT91_BASE_AIC)
#define AT91_DBGU ((struct at91_dbgu *)AT91_BASE_DBGU)
#define AT91_PIOA ((struct at91_pio *)AT91_BASE_PIOA)
#define AT91_PIOB ((struct at91_pio *)AT91_BASE_PIOB)
#define AT91_PIOC ((struct at91_pio *)AT91_BASE_PIOC)
/*
#define AT91_PIO(N) ((struct at91_pio *)((AT91_BASE_PIOA + (N) * \
										 (AT91_BASE_PIOB - AT91_BASE_PIOA))
*/
#define AT91_PMC ((struct at91_pmc *)AT91_BASE_PMC)
#define AT91_CKGR ((struct at91_ckgr *)AT91_BASE_CKGR)

#define AT91_RSTC ((struct at91_rstc *)AT91_BASE_RSTC)
#define AT91_SHDWC ((struct at91_shdwc *)AT91_BASE_SHDWC)
#define AT91_RTTC ((struct at91_rttc *)AT91_BASE_RTTC)
#define AT91_PITC ((struct at91_pitc *)AT91_BASE_PITC)
#define AT91_WDTC ((struct at91_wdtc *)AT91_BASE_WDTC)

struct at91sam9260_peripheral {
	uint32_t res0[67010560];
	struct at91_tcb tcb1;
	struct at91_udp udp;
	struct at91_mci mci;
	struct at91_twi twi;
	struct at91_us us0;
	struct at91_us us1;
	struct at91_us us2;
	struct at91_ssc ssc;
	struct at91_isi isi;
	struct at91_emac emac;
	struct at91_spi spi0;
	struct at91_spi spi1;
	struct at91_us us3;
	struct at91_us us4;
	struct at91_us us5;
	struct at91_tcb tcb2;
	struct at91_adc adc;
	uint32_t res1[24576];
	struct at91_sysc sysc;
};

extern struct at91sam9260_peripheral at91sam9260;

#define AT91_TC0 ((struct at91_tc *)AT91_BASE_TC0)
#define AT91_TC1 ((struct at91_tc *)AT91_BASE_TC1)
#define AT91_TC2 ((struct at91_tc *)AT91_BASE_TC2)
#define AT91_UDP ((struct at91_udp *)AT91_BASE_UDP)
#define AT91_MCI ((struct at91_mci *)AT91_BASE_MCI)
#define AT91_TWI ((struct at91_twi *)AT91_BASE_TWI)
#define AT91_US0 ((struct at91_us *)AT91_BASE_US0)
#define AT91_US1 ((struct at91_us *)AT91_BASE_US1)
#define AT91_US2 ((struct at91_us *)AT91_BASE_US2)
#define AT91_SSC ((struct at91_ssc *)AT91_BASE_SSC)
#define AT91_ISI ((struct at91_isi *)AT91_BASE_ISI)
#define AT91_EMAC ((struct at91_emac *)AT91_BASE_EMACB)
#define AT91_SPI ((struct at91_spi *)AT91_BASE_SPI)
#define AT91_US3 ((struct at91_us *)AT91_BASE_US3)
#define AT91_US4 ((struct at91_us *)AT91_BASE_US4)
#define AT91_US5 ((struct at91_us *)AT91_BASE_US5)
#define AT91_TC3 ((struct at91_tc *)AT91_BASE_TC3)
#define AT91_TC4 ((struct at91_tc *)AT91_BASE_TC4)
#define AT91_TC5 ((struct at91_tc *)AT91_BASE_TC5)
#define AT91_ADC ((struct at91_adc *)AT91_BASE_ADC)

#endif /* __ASSEMBLER__ */

#endif /* __AT91SAM9260_H__ */
