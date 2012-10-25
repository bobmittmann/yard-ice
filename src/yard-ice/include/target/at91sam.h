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

#ifndef __TARGET_AT91SAM_H__
#define __TARGET_AT91SAM_H__

/*****************************************************************************
 Watchdog Timer Controller 
 *****************************************************************************/

#define WDTC_WDCR       (0) /* Watchdog Control Register */
#define WDTC_WDMR       (4) /* Watchdog Mode Register */
#define WDTC_WDSR       (8) /* Watchdog Status Register */

/* WDTC_WDCR - Periodic Interval Image Register */
#define WDTC_WDRSTT         (0x1 <<  0) /* (WDTC) Watchdog Restart */
#define WDTC_KEY            (0xff << 24) /* (WDTC) Watchdog KEY Password */

/* WDTC_WDMR - Watchdog Mode Register */
#define WDTC_WDV            (0xfff <<  0) /* (WDTC) Watchdog Timer Restart */
#define WDTC_WDFIEN         (0x1 << 12) /* (WDTC) Watchdog Fault Interrupt Enable */
#define WDTC_WDRSTEN        (0x1 << 13) /* (WDTC) Watchdog Reset Enable */
#define WDTC_WDRPROC        (0x1 << 14) /* (WDTC) Watchdog Timer Restart */
#define WDTC_WDDIS          (0x1 << 15) /* (WDTC) Watchdog Disable */
#define WDTC_WDD            (0xfff << 16) /* (WDTC) Watchdog Delta Value */
#define WDTC_WDDBGHLT       (0x1 << 28) /* (WDTC) Watchdog Debug Halt */
#define WDTC_WDIDLEHLT      (0x1 << 29) /* (WDTC) Watchdog Idle Halt */

/* WDTC_WDSR - Watchdog Status Register */
#define WDTC_WDUNF          (0x1 <<  0) /* (WDTC) Watchdog Underflow */
#define WDTC_WDERR          (0x1 <<  1) /* (WDTC) Watchdog Error */

/*****************************************************************************
 Power Management Controler
 *****************************************************************************/

#define PMC_SCER        ( 0) /* System Clock Enable Register */
#define PMC_SCDR        ( 4) /* System Clock Disable Register */
#define PMC_SCSR        ( 8) /* System Clock Status Register */

#define PMC_PCER        (16) /* Peripheral Clock Enable Register */
#define PMC_PCDR        (20) /* Peripheral Clock Disable Register */
#define PMC_PCSR        (24) /* Peripheral Clock Status Register */

#define PMC_MOR         (32) /* Main Oscillator Register */
#define PMC_MCFR        (36) /* Main Clock  Frequency Register */
#define PMC_PLLAR       (40) /* PLL A Register */
#define PMC_PLLBR       (44) /* PLL B Register */
#define PMC_PLLR        (44) /* PLL Register */
#define PMC_MCKR        (48) /* Master Clock Register */

#define PMC_PCKR        (64) /* Programmable Clock Register */

#define PMC_IER         (96) /* Interrupt Enable Register */
#define PMC_IDR         (100) /* Interrupt Disable Register */
#define PMC_SR          (104) /* Status Register */
#define PMC_IMR         (108) /* Interrupt Mask Register */

/* PMC_SCER - System Clock Enable Register */
/* PMC_SCDR - System Clock Disable Register */
/* PMC_SCSR - System Clock Status Register */
#define PMC_PCK             (0x1 <<  0) /* (PMC) Processor Clock */
#define PMC_UHP             (0x1 <<  6) /* (PMC) USB Host Port Clock */
#define PMC_UDP             (0x1 <<  7) /* (PMC) USB Device Port Clock */
#define PMC_PCK0            (0x1 <<  8) /* (PMC) Programmable Clock Output */
#define PMC_PCK1            (0x1 <<  9) /* (PMC) Programmable Clock Output */
#define PMC_HCK0            (0x1 << 16) /* (PMC) AHB UHP Clock Output */
#define PMC_HCK1            (0x1 << 17) /* (PMC) AHB LCDC Clock Output */


/* CKGR_MOR - Main Oscillator Register */

/* CKGR_MCFR - Main Clock Frequency Register */

/* CKGR_PLLAR - PLL A Register */

/* CKGR_PLLBR - PLL B Register */

/* PMC_MCKR - Master Clock Register */
#define PMC_CSS             (0x3 <<  0) /* (PMC) Programmable Clock Selection */
#define 	PMC_CSS_SLOW_CLK             (0x0) /* (PMC) Slow Clock is selected */
#define 	PMC_CSS_MAIN_CLK             (0x1) /* (PMC) Main Clock is selected */
#define 	PMC_CSS_PLLA_CLK             (0x2) /* (PMC) Clock from PLL A is selected */
#define 	PMC_CSS_PLLB_CLK             (0x3) /* (PMC) Clock from PLL B is selected */
#define 	PMC_CSS_PLL_CLK             (0x3) /* (PMC) Clock from PLL is selected */

#define PMC_PRES            (0x7 <<  2) /* (PMC) Programmable Clock Prescaler */
#define 	PMC_PRES_CLK    (0x0 <<  2) /* (PMC) Selected clock */
#define 	PMC_PRES_CLK_2  (0x1 <<  2) /* (PMC) Selected clock divided by 2 */
#define 	PMC_PRES_CLK_4  (0x2 <<  2) /* (PMC) Selected clock divided by 4 */
#define 	PMC_PRES_CLK_8  (0x3 <<  2) /* (PMC) Selected clock divided by 8 */
#define 	PMC_PRES_CLK_16 (0x4 <<  2) /* (PMC) Selected clock divided by 16 */
#define 	PMC_PRES_CLK_32 (0x5 <<  2) /* (PMC) Selected clock divided by 32 */
#define 	PMC_PRES_CLK_64 (0x6 <<  2) /* (PMC) Selected clock divided by 64 */
#define PMC_MDIV            (0x3 <<  8) /* (PMC) Master Clock Division */
#define 	PMC_MDIV_1      (0x0 <<  8) /* (PMC) The master clock and the processor clock are the same */
#define 	PMC_MDIV_2      (0x1 <<  8) /* (PMC) The processor clock is twice as fast as the master clock */
#define 	PMC_MDIV_3      (0x2 <<  8) /* (PMC) The processor clock is four times faster than the master clock */

/* PMC_PCKR - Programmable Clock Register */

/* PMC_IER - PMC Interrupt Enable Register */
/* PMC_IDR - PMC Interrupt Disable Register */
/* PMC_SR - PMC Status Register */
/* PMC_IMR - PMC Interrupt Mask Register */
#define PMC_MOSCS           (0x1 <<  0) /* (PMC) MOSC Status/Enable/Disable/Mask */
#define PMC_LOCKA           (0x1 <<  1) /* (PMC) PLL A Status/Enable/Disable/Mask */
#define PMC_LOCKB           (0x1 <<  2) /* (PMC) PLL B Status/Enable/Disable/Mask */
#define PMC_LOCK           (0x1 <<  2) /* (PMC) PLL Status/Enable/Disable/Mask */
#define PMC_MCKRDY          (0x1 <<  3) /* (PMC) Master Clock Status/Enable/Disable/Mask */
#define PMC_PCK0RDY         (0x1 <<  8) /* (PMC) PCK0_RDY Status/Enable/Disable/Mask */
#define PMC_PCK1RDY         (0x1 <<  9) /* (PMC) PCK1_RDY Status/Enable/Disable/Mask */

/*****************************************************************************
 Clock Generator Controler
 *****************************************************************************/

#define CKGR_MOR        ( 0) /* Main Oscillator Register */
#define CKGR_MCFR       ( 4) /* Main Clock  Frequency Register */
#define CKGR_PLLAR      ( 8) /* PLL A Register */
#define CKGR_PLLBR      (12) /* PLL B Register */

#define CKGR_PLLR      (12) /* Single PLL Systems PLL Register */

/* CKGR_MOR - Main Oscillator Register */
#define CKGR_MOSCEN         (0x1 <<  0) /* (CKGR) Main Oscillator Enable */
#define CKGR_OSCBYPASS      (0x1 <<  1) /* (CKGR) Main Oscillator Bypass */
#define CKGR_OSCOUNT(VAL)   (((VAL) & 0xff) <<  8) /* (CKGR) Main Oscillator Start-up Time */

/* CKGR_MCFR - Main Clock Frequency Register */
#define CKGR_MAINF          (0xffff <<  0) /* (CKGR) Main Clock Frequency */
#define CKGR_MAINRDY        (0x1 << 16) /* (CKGR) Main Clock Ready */

/* CKGR_PLLAR - PLL A Register */
#define CKGR_DIVA(VAL)   (((VAL) & 0xff) <<  0) /* (CKGR) Divider A Selected */
#define   CKGR_DIVA_0        (0x0) /* (CKGR) Divider A output is 0 */
#define   CKGR_DIVA_BYPASS   (0x1) /* (CKGR) Divider A is bypassed */

#define CKGR_PLLACOUNT(VAL)  ((VAL & 0x3f) <<  8) /* (CKGR) PLL A Counter */

#define CKGR_OUTA    (0x3 << 14) /* (CKGR) PLL A Output Frequency Range */
#define  CKGR_OUTA_0 (0x0 << 14) /* (CKGR) Please refer to the PLLA datasheet */
#define  CKGR_OUTA_1 (0x1 << 14) /* (CKGR) Please refer to the PLLA datasheet */
#define  CKGR_OUTA_2 (0x2 << 14) /* (CKGR) Please refer to the PLLA datasheet */
#define  CKGR_OUTA_3 (0x3 << 14) /* (CKGR) Please refer to the PLLA datasheet */

#define CKGR_MULA(VAL) (((VAL - 1) & 0x7ff) << 16) /* (CKGR) PLL A Multiplier */

#define CKGR_SRCA      (0x1 << 29) /* (CKGR) this bit should be one allways */


/* CKGR_PLLBR - PLL B Register */
#define CKGR_DIVB           (0xff <<  0) /* (CKGR) Divider B Selected */
#define 	CKGR_DIVB_0                    (0x0) /* (CKGR) Divider B output is 0 */
#define 	CKGR_DIVB_BYPASS               (0x1) /* (CKGR) Divider B is bypassed */
#define CKGR_PLLBCOUNT      (0x3f <<  8) /* (CKGR) PLL B Counter */
#define CKGR_OUTB           (0x3 << 14) /* (CKGR) PLL B Output Frequency Range */
#define 	CKGR_OUTB_0                    (0x0 << 14) /* (CKGR) Please refer to the PLLB datasheet */
#define 	CKGR_OUTB_1                    (0x1 << 14) /* (CKGR) Please refer to the PLLB datasheet */
#define 	CKGR_OUTB_2                    (0x2 << 14) /* (CKGR) Please refer to the PLLB datasheet */
#define 	CKGR_OUTB_3                    (0x3 << 14) /* (CKGR) Please refer to the PLLB datasheet */
#define CKGR_MULB(VAL)    (((VAL - 1) & 0x7ff) << 16) /* (CKGR) PLL A Multiplier */
/* (CKGR) Divider for USB Clocks */
#define CKGR_USBDIV(VAL) ((((VAL) / 2) & 0x3) << 28) 
#define 	CKGR_USBDIV_0                    (0x0 << 28) /* (CKGR) Divider output is PLL clock output */
#define 	CKGR_USBDIV_1                    (0x1 << 28) /* (CKGR) Divider output is PLL clock output divided by 2 */
#define 	CKGR_USBDIV_2                    (0x2 << 28) /* (CKGR) Divider output is PLL clock output divided by 4 */

/* Aliases */
#define CKGR_MUL(VAL) CKGR_MULA(VAL) /* (CKGR) PLL Multiplier */
#define CKGR_PLLCOUNT(VAL) CKGR_PLLACOUNT(VAL) /* (CKGR) PLL Counter */
#define CKGR_DIV(VAL) CKGR_DIVA(VAL) /* (CKGR) Divider Selected */

#define CKGR_PLLMUL(VAL) CKGR_MUL(VAL)
#define CKGR_PLLDIV(VAL) CKGR_DIV(VAL)


/*****************************************************************************
  Reset Controller Interface 
 *****************************************************************************/

#define RSTC_CR        (0) /* Reset Control Register */
#define RSTC_SR        (4) /* Reset Status Register */
#define RSTC_MR        (8) /* Reset Mode Register */

/* RSTC_RCR - Reset Control Register */
#define RSTC_PROCRST        (0x1 <<  0) /* (RSTC) Processor Reset */
#define RSTC_ICERST         (0x1 <<  1) /* (RSTC) ICE Interface Reset */
#define RSTC_PERRST         (0x1 <<  2) /* (RSTC) Peripheral Reset */
#define RSTC_EXTRST         (0x1 <<  3) /* (RSTC) External Reset */
#define RSTC_KEY(KEY)       (((KEY) & 0xff) << 24) /* (RSTC) Password */

/* RSTC_RSR - Reset Status Register */
#define RSTC_URSTS          (0x1 <<  0) /* (RSTC) User Reset Status */
#define RSTC_RSTTYP         (0x7 <<  8) /* (RSTC) Reset Type */
#define RSTC_RSTTYP_GENERAL    (0x0 <<  8) /* (RSTC) General reset. 
											  Both VDDCORE and VDDBU rising. */
#define RSTC_RSTTYP_WAKEUP     (0x1 <<  8) /* (RSTC) WakeUp Reset. 
											  VDDCORE rising. */
#define RSTC_RSTTYP_WATCHDOG   (0x2 <<  8) /* (RSTC) Watchdog Reset. 
											  Watchdog overflow occured. */
#define RSTC_RSTTYP_SOFTWARE   (0x3 <<  8) /* (RSTC) Software Reset. 
											  Processor reset required by 
											  the software. */
#define RSTC_RSTTYP_USER       (0x4 <<  8) /* (RSTC) User Reset. 
											  NRST pin detected low. */
#define RSTC_RSTTYP_BROWNOUT   (0x5 <<  8) /* (RSTC) Brownout reset coccurred */

#define RSTC_NRSTL          (0x1 << 16) /* (RSTC) NRST pin level */
#define RSTC_SRCMP          (0x1 << 17) /* (RSTC) Software Reset 
										   Command in Progress. */

/* RSTC_RMR - Reset Mode Register */
#define RSTC_URSTEN         (0x1 <<  0) /* (RSTC) User Reset Enable */
#define RSTC_URSTIEN        (0x1 <<  4) /* (RSTC) User Reset Interrupt Enable */
#define RSTC_ERSTL(X)       (((X) & 0xf) <<  8) /* (RSTC) User Reset Length */
#define ERSTL_60_US 0
#define ERSTL_120_US 1
#define ERSTL_240_US 2
#define ERSTL_480_US 3
#define ERSTL_960_US 4
#define ERSTL_2_MS 5
#define ERSTL_4_MS 6
#define ERSTL_8_MS 7
#define ERSTL_16_MS 8
#define ERSTL_31_MS 9
#define ERSTL_62_MS 10
#define ERSTL_125_MS 11
#define ERSTL_250_MS 12
#define ERSTL_500_MS 13
#define ERSTL_1_S 14
#define ERSTL_2_S 15

#define RSTC_BODIEN         (0x1 <<  16) /* (RSTC) Brownout Detection 
											Interrupt Enable */

/*****************************************************************************
 Debug Unit 
 *****************************************************************************/

#define US_CHMODE           (0x3 << 14) /* (DBGU) Channel Mode */
#define 	US_CHMODE_NORMAL               (0x0 << 14) /* (DBGU) Normal Mode: The USART channel operates as an RX/TX USART. */
#define 	US_CHMODE_AUTO                 (0x1 << 14) /* (DBGU) Automatic Echo: Receiver Data Input is connected to the TXD pin. */
#define 	US_CHMODE_LOCAL                (0x2 << 14) /* (DBGU) Local Loopback: Transmitter Output Signal is connected to Receiver Input Signal. */
#define 	US_CHMODE_REMOTE               (0x3 << 14) /* (DBGU) Remote Loopback: RXD pin is internally connected to TXD pin. */

/* Debug Unit */

#define DBGU_CR         ( 0) /* Control Register */
#define DBGU_MR         ( 4) /* Mode Register */
#define DBGU_IER        ( 8) /* Interrupt Enable Register */
#define DBGU_IDR        (12) /* Interrupt Disable Register */
#define DBGU_IMR        (16) /* Interrupt Mask Register */
#define DBGU_CSR        (20) /* Channel Status Register */
#define DBGU_RHR        (24) /* Receiver Holding Register */
#define DBGU_THR        (28) /* Transmitter Holding Register */
#define DBGU_BRGR       (32) /* Baud Rate Generator Register */

#define DBGU_CIDR       (64) /* Chip ID Register */

#define CIDR_VER(CIDR) (((CIDR) >> 0) & 0x1f)
#define CIDR_EPROC(CIDR) (((CIDR) >> 5) & 0x03)
#define CIDR_NVPSIZ(CIDR) (((CIDR) >> 8) & 0x0f)
#define CIDR_NVPSIZ2(CIDR) (((CIDR) >> 12) & 0x0f)
#define CIDR_SRAMSIZ(CIDR) (((CIDR) >> 16) & 0x0f)
#define CIDR_ARCH(CIDR) (((CIDR) >> 20) & 0xff)
#define CIDR_NVPTYP(CIDR) (((CIDR) >> 28) & 0x03)
#define CIDR_EXT(CIDR) (((CIDR) >> 31) & 0x01)

#define DBGU_EXID       (68) /* Chip ID Extension Register */
#define DBGU_FNTR       (72) /* Force NTRST Register */

#define DBGU_RPR        (256) /* Receive Pointer Register */
#define DBGU_RCR        (260) /* Receive Counter Register */
#define DBGU_TPR        (264) /* Transmit Pointer Register */
#define DBGU_TCR        (268) /* Transmit Counter Register */
#define DBGU_RNPR       (272) /* Receive Next Pointer Register */
#define DBGU_RNCR       (276) /* Receive Next Counter Register */
#define DBGU_TNPR       (280) /* Transmit Next Pointer Register */
#define DBGU_TNCR       (284) /* Transmit Next Counter Register */
#define DBGU_PTCR       (288) /* PDC Transfer Control Register */
#define DBGU_PTSR       (292) /* PDC Transfer Status Register */


/* DBGU_IDR - Debug Unit Interrupt Disable Register */

/* DBGU_IMR - Debug Unit Interrupt Mask Register */

/* DBGU_CSR - Debug Unit Channel Status Register */

/* DBGU_FNTR - Debug Unit FORCE_NTRST Register */
#define AT91C_US_FORCE_NTRST      (0x1 <<  0) /* (DBGU) Force NTRST in JTAG */

/*****************************************************************************
 PIO 
 *****************************************************************************/

#define PIO_PER         ( 0) /* PIO Enable Register */
#define PIO_PDR         ( 4) /* PIO Disable Register */
#define PIO_PSR         ( 8) /* PIO Status Register */

#define PIO_OER         (16) /* Output Enable Register */
#define PIO_ODR         (20) /* Output Disable Registerr */
#define PIO_OSR         (24) /* Output Status Register */

#define PIO_IFER        (32) /* Input Filter Enable Register */
#define PIO_IFDR        (36) /* Input Filter Disable Register */
#define PIO_IFSR        (40) /* Input Filter Status Register */

#define PIO_SODR        (48) /* Set Output Data Register */
#define PIO_CODR        (52) /* Clear Output Data Register */
#define PIO_ODSR        (56) /* Output Data Status Register */
#define PIO_PDSR        (60) /* Pin Data Status Register */
#define PIO_IER         (64) /* Interrupt Enable Register */
#define PIO_IDR         (68) /* Interrupt Disable Register */
#define PIO_IMR         (72) /* Interrupt Mask Register */
#define PIO_ISR         (76) /* Interrupt Status Register */
#define PIO_MDER        (80) /* Multi-driver Enable Register */
#define PIO_MDDR        (84) /* Multi-driver Disable Register */
#define PIO_MDSR        (88) /* Multi-driver Status Register */

#define PIO_PUDR       (96) /* Pull-up Disable Register */
#define PIO_PUER       (100) /* Pull-up Enable Register */
#define PIO_PUSR       (104) /* Pull-up Status Register */

#define PIO_ASR         (112) /* Select A Register */
#define PIO_BSR         (116) /* Select B Register */
#define PIO_ABSR        (120) /* AB Select Status Register */

#define PIO_OWER        (160) /* Output Write Enable Register */
#define PIO_OWDR        (164) /* Output Write Disable Register */
#define PIO_OWSR        (168) /* Output Write Status Register */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
	}
#endif

#endif /* __TARGET_AT91SAM_H__ */

