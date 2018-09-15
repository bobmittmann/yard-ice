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
 * @file dm365.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __DAVINCI_DM365_H__
#define __DAVINCI_DM365_H__

#include <stdint.h>
#include <target.h>

#include "davinci/dm36x-prtcss.h"
#include "davinci/dm36x-timer.h"
#include "davinci/dm36x-uart.h"
#include "davinci/dm36x-system.h"
#include "davinci/dm36x-pll.h"
#include "davinci/dm36x-i2c.h"
#include "davinci/dm36x-ddr.h"
#include "davinci/dm36x-aemif.h"
#include "davinci/dm36x-aintc.h"
#include "davinci/dm36x-gpio.h"
#include "davinci/dm36x-psc.h"

/****************************************************************************
 * DM365 
 *****************************************************************************/

/*
    DM365 Memory Map

	Start Addr   End Addr     Size     Description
	0x0000 0000  0x0000 3FFF  16K      ARM RAM0  (Instruction)
	0x0000 4000  0x0000 7FFF  16K      ARM RAM1 (Instruction)
	0x0000 8000  0x0000 BFFF  16K      ARM ROM (Instruction)
	0x0000 C000  0x0000 FFFF  16K      Reserved
	0x0001 0000  0x0001 3FFF  16K      ARM RAM0 (Data)
	0x0001 4000  0x0001 7FFF  16K      ARM RAM1 (Data)
	0x0001 8000  0x0001 BFFF  16K      ARM ROM 
	0x0001 C000  0x000F FFFF  912K     Reserved
	0x0010 0000  0x01BB FFFF  26M      Reserved
	0x01BC 0000  0x01BC 0FFF  4K       ARM ETB Mem
	0x01BC 1000  0x01BC 17FF  2K       ARM ETB Reg
	0x01BC 1800  0x01BC 18FF  256      ARM IceCrusher
	0x01BC 1900  0x01BC FFFF  59136    Reserved
	0x01BD 0000  0x01BF FFFF  192K     Reserved
	0x01C0 0000  0x01FF FFFF  4M       CFG Bus Peripherals
	0x0200 0000  0x09FF FFFF  128M     ASYNC EMIF (Data)
	0x0A00 0000  0x11EF FFFF  127M-16K Reserved
	0x11F0 0000  0x11F1 FFFF  128K     MJCP DMA Port
	0x11F2 0000  0x11FF FFFF  896K     Reserved
	0x1200 0000  0x1207 FFFF  512K     HDVICP DMA Port1
	0x1208 0000  0x120F FFFF  512K     Reserved - HDVICP DMA Port2
	0x1210 0000  0x1217 FFFF  512K     Reserved - HDVICP DMA Port3
	0x1218 0000  0x1FFF FFFF  222.5M   Reserved
	0x2000 0000  0x2000 7FFF  32K      DDR EMIF Control 
	0x2000 8000  0x41FF FFFF  544M-32K Reserved
	0x4200 0000  0x49FF FFFF  128M     Reserved
	0x4A00 0000  0x7FFF FFFF  864M     Reserved
	0x8000 0000  0x8FFF FFFF  256M     DDR EMIF
	0x9000 0000  0xFFFF FFFF  1792M    Reserved
*/

/*
	EDMA CC           0x01C0 0000 0x01C0 FFFF 64K
	EDMA TC0          0x01C1 0000 0x01C1 03FF 1K
	EDMA TC1          0x01C1 0400 0x01C1 07FF 1K
	EDMA TC2          0x01C1 0800 0x01C1 0BFF 1K
	EDMA TC3          0x01C1 0C00 0x01C1 0FFF 1K
	Reserved          0x01C1 1000 0x01C1 FFFF 60 K
	UART0             0x01C2 0000 0x01C2 03FF 1K
	Reserved          0x01C2 0400 0x0120 7FFF 1K
	Timer 3           0x01C2 0800 0x01C2 0BFF 1K
	Real-time out     0x01C2 0C00 0x01C2 0FFF 1K
	I2C               0x01C2 1000 0x01C2 13FF 1K
	Timer 0           0x01C2 1400 0x01C2 17FF 1K
	Timer 1           0x01C2 1800 0x01C2 1BFF 1K
	Timer 2           0x01C2 1C00 0x01C2 1FFF 1K
	PWM0              0x01C2 2000 0x01C2 23FF 1K
	PWM1              0x01C2 2400 0x01C2 27FF 1K
	PWM2              0x01C2 2800 0x01C2 2BFF 1K
	PWM3              0x01C2 2C00 0x01C2 2FFF 1K
	SPI4              0x01C2 3000 0x01C2 37FF 2K
	Timer 4           0x01C2 3800 0x01C2 3BFF 1K
	ADCIF             0x01C2 3C00 0x01C2 3FFF 1K
	Reserved          0x01C2 4000 0x01C3 4FFF 112K
	System Module     0x01C4 0000 0x01C4 07FF 2K
	PLL Controller 1  0x01C4 0800 0x01C4 0BFF 1K
	PLL Controller 2  0x01C4 0C00 0x01C4 0FFF 1K
	Power/Sleep Ctlr  0x01C4 1000 0x01C4 1FFF 4K
	Reserved          0x01C4 2000 0x01C4 7FFF 24K
	ARM Int Ctlr      0x01C4 8000 0x01C4 83FF 1K
	Reserved          0x01C4 8400 0x01C6 3FFF 111K
	USB OTG 2.0       0x01C6 4000 0x01C6 5FFF 8K
	SPI0              0x01C6 6000 0x01C6 67FF 2K
	SPI1              0x01C6 6800 0x01C6 6FFF 2K
	GPIO              0x01C6 7000 0x01C6 77FF 2K
	SPI2              0x01C6 7800 0x01C6 FFFF 2K
	SPI3              0x01C6 8000 0x01C6 87FF 2K
	Reserved          0x01C6 8800 0x01C6 87FF 2K
	PRTCSS Iface Regs 0x01C6 9000 0x01C6 93FF 1K
	KEYSCAN           0x01C6 9400 0x01C6 97FF 1K
	HPI               0x01C6 9800 0x01C6 9FFF 2K
	Reserved          0x01C6 A000 0x01C6 FFFF 24K

	ISP System Conf Regs 0x01C7 0000 0x01C7 00FF 256
	VPBE Clock Ctrl Reg 0x01C7 0200 0x01C7 02FF 256
	Resizer Registers 0x01C7 0400 0x01C7 07FF 1K
	IPIPE Registers   0x01C7 0800 0x01C7 0FFF 2K
	ISIF Registers    0x01C7 1000 0x01C7 11FF 512
	
	IPIPEIF Registers 0x01C7 1200 0x01C7 12FF 768
	H3A Registers     0x01C7 1400 0x01C7 14FF 256
	Reserved          0x01C7 1600 0x01C7 17FF 512
	FDIF Registers    0x01C7 1800 0x01C7 1BFF 1K
	OSD Registers     0x01C7 1C00 0x01C7 1CFF 256
	Reserved          0x01C7 1D00 0x01C7 1DFF 256
	VENC Registers    0x01C7 1E00 0x01C7 1FFF 512
	Reserved          0x01C7 2000 0x01CF FFFF 568K
	Multimedia / SD 1 0x01D0 0000 0x01D0 1FFF 8K
	McBSP             0x01D0 2000 0x01D0 3FFF 8K
	Reserved          0x01D0 4000 0x01D0 5FFF 8K
	UART1             0x01D0 6000 0x01D0 63FF 1K
	Reserved          0x01D0 6400 0x01D0 7FFF 3K
	EMAC Ctrl Regs    0x01D0 7000 0x01D0 7FFF 4K
	EMAC Ctrl Mod RAM 0x01D0 8000 0x01D0 9FFF 8K
	EMAC Ctrl Mod Reg 0x01D0 A000 0x01D0 AFFF 4K
	EMAC MDIO Ctl Reg 0x01D0 B000 0x01D0 B7FF 2K
	Voice Codec       0x01D0 C000 0x01D0 C3FF 1K
	Reserved          0x01D0 C400 0x01D0 FFFF 17K
	ASYNC EMIF Ctrl   0x01D1 0000 0x01D1 0FFF 4K
	Multimedia / SD 0 0x01D1 1000 0x01D1 FFFF 60K
	Reserved          0x01D2 0000 0x01D3 FFFF 128K
	Reserved          0x01D4 0000 0x01DF FFFF 768K
	Reserved          0x01E0 0000 0x01FF FFFF 2M
	ASYNC EMIF Data (CE0) 0x0200 0000 0x03FF FFFF 32M
	ASYNC EMIF Data (CE1) 0x0400 0000 0x05FF FFFF 32M
	Reserved          0x0600 0000 0x09FF FFFF 64M
	Reserved          0x0A00 0000 0x0FFF FFFF 96M
*/

#define DM365_PRTCIF_BASE 0x01c69000
#define DM365_SYSTEM_BASE 0x01c40000
#define DM365_GPIO_BASE 0x01c67000
#define DM365_AINTC_BASE 0x01c48000
#define DM365_PLL1_BASE 0x01c40800
#define DM365_PLL2_BASE 0x01c40c00
#define DM365_PSC_BASE 0x01c41000
#define DM365_DDR_BASE 0x20000000
#define DM365_AEMIF_BASE 0x01d10000
#define DM365_UART0_BASE 0x01c20000
#define DM365_UART1_BASE 0x01d06000
#define DM365_TIMER0_BASE 0x01c21400
#define DM365_CSL_RTSIF_0_REGS_BASE 0x01c69000
#define DM365_I2C0_BASE 0x01c21000

#define DM365_CE0_BASE 0x02000000
#define DM365_CE1_BASE 0x04000000

/* DM365 NAND Flash */

#define DM365_NAND_DATA_OFFSET 0x0000
#define DM365_NAND_ALE_OFFSET 0x0008
#define DM365_NAND_CLE_OFFSET 0x0010

#define DM365_NAND_TIMEOUT 10240

/* Max Bytes per operation (EMIF IP constrained) */
#define DM365_NAND_MAX_DATA_PER_OP 512
/* Max Spare Bytes per operation */
#define DM365_NAND_MAX_SPARE_PER_OP 16
/* Min Spare Bytes per operation (ECC operation constrained) */
#define DM365_NAND_MIN_SPARE_PER_OP 10


/***************************************************************************/

#define DM36X_PRTCIF_BASE DM365_PRTCIF_BASE
#define DM36X_SYSTEM_BASE DM365_SYSTEM_BASE
#define DM36X_GPIO_BASE DM365_GPIO_BASE
#define DM36X_AINTC_BASE DM365_AINTC_BASE
#define DM36X_PLL1_BASE DM365_PLL1_BASE
#define DM36X_PLL2_BASE DM365_PLL2_BASE
#define DM36X_PSC_BASE DM365_PSC_BASE
#define DM36X_DDR_BASE DM365_DDR_BASE
#define DM36X_AEMIF_BASE DM365_AEMIF_BASE
#define DM36X_UART0_BASE DM365_UART0_BASE
#define DM36X_UART1_BASE DM365_UART1_BASE
#define DM36X_TIMER0_BASE DM365_TIMER0_BASE
#define DM36X_CSL_RTSIF_0_REGS_BASE DM365_CSL_RTSIF_0_REGS_BASE
#define DM36X_I2C0_BASE DM365_I2C0_BASE

/****************************************************************************/

#define BOOTCFG_BOOTMODE_MASK 0x000000e0
#define BOOTCFG_BOOTMODE_SHIFT 5

#define BOOTCFG_EMIFWIDTH_MASK 0x00000004
#define BOOTCFG_EMIFWIDTH_SHIFT 2

#define PINMUX_UART0_MASK 0x00000001
#define PINMUX_EMIF_MASK 0x00000fff

#define PINMUX_UART0_EN 0x00000001
#define PINMUX_EMIF_EN 0x00000000
#define PINMUX_ATA_EN 0x00020000

#define VTPIOCR_PWRDN_MASK 0x00000040
#define VTPIOCR_LOCK_MASK 0x00000080
#define VTPIOCR_PWRSAVE_MASK 0x00000100
#define VTPIOCR_CLR_MASK 0x00002000
#define VTPIOCR_VTPIOREADY_MASK 0x00004000
#define VTPIOCR_READY_MASK 0x00008000

#define MISC_PLL1POSTDIV_MASK 0x00000002
#define MISC_AIMWAITST_MASK 0x00000001
#define MISC_TIMER2WDT_MASK 0x00000010

#define PLLCTL_PLLEN_MASK 0x00000001
#define PLLCTL_PLLPWRDN_MASK 0x00000002
#define PLLCTL_PLLRST_MASK 0x00000008
#define PLLCTL_PLLDIS_MASK 0x00000010
#define PLLCTL_PLLENSRC_MASK 0x00000020
#define PLLCTL_CLKMODE_MASK 0x00000100

#define PLLCMD_GOSET_MASK 0x00000001
#define PLLSTAT_GOSTAT_MASK 0x00000001
#define PLLDIV_EN_MASK 0x00008000
#define PLLSTAT_LOCK_MASK 0x00000002

//these values have been taken from the GEl file
#define PLL1_MULTIPLIER 0x51
#define PLL2_MULTIPLIER 0x63

#define OSC_FREQ 24000000
#define SYSTEM_FREQ ((PLL1_MULTIPLIER * OSC_FREQ)>>3)



#define EMURSTIE_MASK 0x00000200

#define PD0 0

#define PSC_ENABLE 0x3
#define PSC_DISABLE 0x2
#define PSC_SYNCRESET 0x1
#define PSC_SWRSTDISABLE 0x0



#define UART0_DESIRED_BAUD 115200
#define UART0_OVERSAMPLE_CNT 16


#define CSL_PID 0x00
#define CSL_DMACMD 0x04
#define CSL_DMADAT0 0x08
#define CSL_DMADAT1 0x0c
#define CSL_INTEN 0x10
#define CSL_INTFLG 0x14


/* bootmodes */
#define BOOTMODE_NAND    0x0    /* NAND boot mode (RBL -> UBL)  */
#define BOOTMODE_AEMIF   0x1    /* NOR boot mode (execute in place) */
#define BOOTMODE_SD_MMC  0x2    /* SD card */
#define BOOTMODE_UART    0x3    /* UART */
#define BOOTMODE_USB     0x4     
#define BOOTMODE_SPI     0x5    
#define BOOTMODE_EMAC    0x6    
#define BOOTMODE_HPI     0x7     




/* The size of DDR is 1GB */
#define DDR_RAM_SIZE 0x40000000

#define MAX_IMAGE_SIZE 0x02000000
#define DDR_START_ADDR 0x80000000
#define DDR_END_ADDR ((DDR_START_ADDR + DDR_RAM_SIZE))








#endif /* __DAVINCI_DM365_H__ */

