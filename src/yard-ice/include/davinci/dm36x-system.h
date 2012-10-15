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



#ifndef __DM36X_SYSTEM_H__
#define __DM36X_SYSTEM_H__

/* System Control Module registers */
#define SYS_PINMUX(N) (0x00 + ((N) * 4))
#define SYS_PINMUX0 0x00
#define SYS_PINMUX1 0x04
#define SYS_PINMUX2 0x08
#define SYS_PINMUX3 0x0c
#define SYS_PINMUX4 0x10
#define SYS_BOOTCFG 0x14
#define SYS_ARM_INTMUX 0x18
#define SYS_EDMA_EVTMUX 0x1c
#define SYS_DDR_SLEW 0x20
#define SYS_CLKOUT 0x24
#define SYS_DEVICE_ID 0x28
#define SYS_VDAC_CONFIG 0x2c
#define SYS_TIMER64_CTL 0x30
#define SYS_USBPHY_CTL 0x34
#define SYS_MISC 0x38

#define SYS_MSTPRI(N) (0x3c + ((N) * 4))
#define SYS_MSTPRI0 0x3c
#define SYS_MSTPRI1 0x40

#define SYS_VPSS_CLKCTL 0x44
#define SYS_PERI_CLKCTL 0x48
#define SYS_DEEPSLEEP 0x4c
#define SYS_DFT_ENABLE 0x50

#define SYS_DEBOUNCE(N) (0x54 + ((N) * 4))
#define SYS_DEBOUNCE0 0x54
#define SYS_DEBOUNCE1 0x58
#define SYS_DEBOUNCE2 0x5c
#define SYS_DEBOUNCE3 0x60
#define SYS_DEBOUNCE4 0x64
#define SYS_DEBOUNCE5 0x68
#define SYS_DEBOUNCE6 0x6c
#define SYS_DEBOUNCE7 0x70

#define SYS_VTPIOCR 0x74
#define SYS_PUPDCTL0 0x78
#define SYS_PUPDCTL1 0x7c
#define SYS_HDIMCOPBT 0x80

#define SYS_PLLC1_CONFIG 0x84
#define SYS_PLLC2_CONFIG 0x88




/*****************************************************************************
 * 
 ****************************************************************************/

/*****************************************************************************
 * Peripheral Clock Control (PERI_CLKCTL) Register
 ****************************************************************************/

/* PRTCSS clock source selection 
   0      RTCXI (OSC)
   1      PLLC1AUXCLK clock Divider */
#define PERI_CLKCTL_PRTCCLKS (1 << 30)
#define PRTCCLKS_RTCXI  (0 << 30)
#define PRTCCLKS_PLLC1AUXCLK (1 << 30)

/* ARM926 clock source selection 
   When changing the source clock (either 0 to 1 or 1 to 0), 
   ARM926 clock frequency must be ≥ CFG/DMA bus clock frequency (PLLC1SYSCLK4).
   0      PLLC1SYSCLK2
   1      PLLC2SYSCLK2 */
#define PERI_CLKCTL_ARMCLKS (1 << 29)
#define ARMCLKS_PLLC1SYSCLK2 (0 << 29)
#define ARMCLKS_PLLC2SYSCLK2 (1 << 29)

/* KeyScan clock source selection 
   0      RTCXI (MXI)
   1      PLLC1AUXCLK clock Divider  */
#define PERI_CLKCTL_KEYSCLKS (1 << 28)
#define KEYSCLKS_RTCXI  (0 << 28)
#define KEYSCLKS_PLLC1AUXCLK (1 << 28)

/* DDR2 clock source selection 
   0      PLLC1SYSCLK7
   1      PLLC2SYSCLK3 */
#define PERI_CLKCTL_DDRCLKS (1 << 27)
#define DDRCLKS_PLLC1SYSCLK7 (0 << 27)
#define DDRCLKS_PLLC2SYSCLK3 (1 << 27)

/* HDVICP Processing logic clock source selection 
   0      PLLC1SYSCLK2
   1      PLLC2SYSCLK2 */
#define PERI_CLKCTL_HDVICPCLKS (1 << 26)
#define HDVICPCLKS_PLLC1SYSCLK2 (0 << 26)
#define HDVICPCLKS_PLLC2SYSCLK2 (1 << 26)

/* PLL clock divider for Key Scan and PRTCSS 
   Key Scan and PRTCSS Peripheral Clock = PLLC1AUXCLK / (DIV3+1) */
#define PERI_CLKCTL_DIV3 (0x3ff << 16)
#define KEYSCAN_DIV(N) (((N - 1) & 0x3ff) << 16)

/* PLL clock divider for Voice Codec
   Voice Codec Peripheral Clock = PLLC2SYSCLK4 / (DIV2+1) */
#define PERI_CLKCTL_DIV2 (0x1ff << 7)
#define VCODEC_DIV(N) (((N - 1) & 0x1ff) << 7)

/* PLL clock divider for CLKOUT2
   CLKOUT2 = PLLCL1SYSCLK9 / (DIV1+1) */
#define PERI_CLKCTL_DIV1 (0x0f << 3)
#define CLKOUT2_DIV(N) (((N - 1) & 0x0f) << 3)

/* CLOCKOUT2EN 
   0      Output CLOCKOUT2 enable
   1      Output CLOCKOUT2 disable */
#define PERI_CLKCTL_CLOCKOUT2EN (1 << 2)
#define CLOCKOUT2_ENABLE (0 << 2)
#define CLOCKOUT2_DISABLE (1 << 2)

/* CLOCKOUT1EN
   0     Output CLOCKOUT1 enable
   1     Output CLOCKOUT1 disable */
#define PERI_CLKCTL_CLOCKOUT1EN (1 << 1)
#define CLOCKOUT1_ENABLE (0 << 1)
#define CLOCKOUT1_DISABLE (1 << 1)

/* CLOCKOUT0EN 
   0     Output CLOCKOUT0 enable
   1     Output CLOCKOUT0 disable */
#define PERI_CLKCTL_CLOCKOUT0EN (0 << 0)
#define CLOCKOUT0_ENABLE (0 << 0)
#define CLOCKOUT0_DISABLE (1 << 0)

/*****************************************************************************
 * PLLC1 Configuration (PLLC1_CONFIG) Register
 ****************************************************************************/

/* PLL in lock mode condition */
#define PLLC1_CONFIG_LOCK (0x07 << 25)

/*****************************************************************************
 * PLLC2 Configuration (PLLC2_CONFIG) Register
 ****************************************************************************/

/* PLL in lock mode condition */
#define PLLC2_CONFIG_LOCK (0x07 << 25)

/*****************************************************************************
 * VTP IO Control (VTPIOCR) Register
 ****************************************************************************/

/* Active low reset is used to reset the DLL. This should 
   code ‘1’ in normal operation. */
#define VTPIOCR_DLLRSTZ (1 << 20)

/* Active low reset is used to reset the clock divider of DDR2 
   address/data macro. This should code ‘1’ in normal operation. */
#define VTPIOCR_CLKRSTZ (1 << 19)

/* Internal DDR2 IO Vref enable 
   0     Connected to pad, external reference
   1     Connected to internal reference */
#define VTPIOCR_VREFEN (1 << 18)

/* Selection for internal reference voltage level
   0     Vref = 50.0% of VDDS
   1     Vref = 47.5% of VDDS
   2     Vref = 52.5% of VDDS
   3     Vref = 50.0% of VDDS */
#define VTPIOCR_VREFTAP(N) (((N) & 0x3) << 16)

/* VTP Ready status
   0     VTP not ready
   1     VTP ready */
#define VTPIOCR_READY (1 << 15)

/* Power down control enable for DDR2 input buffer
   0     Disable power down control by config_pwrdnen register
   1     Enable power down control by config_pwrdnen register */
#define VTPIOCR_IOPWRDN (1 << 14)

/* VTP clear. Write 0 to clear VTP flops. */
#define VTPIOCR_CLRZ (1 << 13)

/* Force decrease PFET drive */
#define VTPIOCR_FORCEDNP (1 << 12)

/* Force decrease NFET drive */
#define VTPIOCR_FORCEDNN (1 << 11)

/* Force increase PFET drive */
#define VTPIOCR_FORCEUPP (1 << 10)

/* Force increase PFET drive */
#define VTPIOCR_FORCEUPN (1 << 9)

/* VTP Power Save Mode
   0     Disable power save mode
   1     Enable power save mode */
#define VTPIOCR_PWRSAVE (1 << 8)

/* VTP Impedance Lock 
   0      Unlock impedance
   1      Lock impedance */
#define VTPIOCR_LOCK (1 << 7)

/* VTP Power Down 
   0      Disable power down
   1      Enable power down */
#define VTPIOCR_PWRDN (1 << 6)

/* Drive strength control bit */
#define VTPIOCR_D0 (1 << 5)

/* Drive strength control bit */
#define VTPIOCR_D1 (1 << 4)

/* Drive strength control bit */
#define VTPIOCR_D2 (1 << 3)

/* Digital filter control bit */
#define VTPIOCR_F0 (1 << 2)

/* Digital filter control bit */
#define VTPIOCR_F1 (1 << 1)

/* Digital filter control bit */
#define VTPIOCR_F2 (1 << 0)






#ifndef __ASSEMBLER__


#include <stdint.h>

struct dm36x_system {
	volatile uint32_t id; /* PRTCIF peripheral ID register */
	volatile uint32_t ctrl; /* PRTCIF control register */
	volatile uint32_t ldata; /* PRTCIF access lower data register */
	volatile uint32_t udata; /* PRTCIF access upper data register */
	volatile uint32_t inten; /* PRTCIF interrupt enable register */
	volatile uint32_t intflg; /* PRTCIF interrupt flag register */  
};

#endif /* __ASSEMBLER__ */


#endif /* __DM36X_SYSTEM_H__ */

