/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the libstm32f.
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
 * @file stm32f-comp.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32_COMP_H__
#define __STM32_cOMP_H__

/*-------------------------------------------------------------------------
 * comparator (COMP)
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * COMP comparator control and status register (COMP_CSR) */
#define STM32_COMP_CSR 0x00
/*  The COMP_CSR register is the control/status register of the comparators. 
	It contains all the bits related to both comparators. */

/* Bit 31 TSUSP: Suspend Timer Mode */
#define COMP_TSUSP (1 << 31)
/* 0: TIM9 ITR enabled to suspend OC TIM9 generation
1: TIM9 ITR not used to suspend OC TIM9 generation
Note: This bit is available in Cat.3, Cat.4 and Cat.5 devices only */

/* Bit 30 CAIF: Channel acquisition interrupt flag */
#define COMP_CAIF (1 << 30)
/* 0: Channel acquisition ongoing or not started
1: Channel acquisition completed
Note: This bit is available in Cat.3, Cat.4 and Cat.5 devices only */

/* Bit 29 CAIE: Channel Acquisition Interrupt Enable / Clear */
#define COMP_CAIE (1 << 29)
/* This bit is set and cleared to enable the Channel Acquisition 
   interrupt. When the Caif bit is set, it must be cleared by writing 0 
   to the CAIE bit.
   0: Channel acquisition interrupt disabled
   1: Channel acquisition interrupt enabled
   This bit is available in Cat.3, Cat.4 and Cat.5 devices only */

/* Bit 28 RCH13: Select GPIO port PC3 as re-routed ADC input channel CH13. */
#define COMP_RCH13 (1 << 28)
/* This bit is set and cleared by software in order configure PC3 to be 
   used as re-routed channel CH13 (selected by the ADC interface) if 
   OPAMP3 is in power down mode (OPA1PD bit = 0 in OPAMP_CSR register 
   (Cat.4 and Cat.5 device only). See Figure 71: OPAMP3 signal routing 
   (Cat.4 devices only) on page 342.
0: PC3 can be used as slow ADC channel
1: PC3 can be used as re-routed ADC channel
Note: This bit is available in Cat.3, Cat.4 and Cat.5devices only */

/* Bit 27 FCH8: Select GPIO port PB0 as fast ADC input channel CH8. */
#define COMP_FCH8 (1 << 27)
/* This bit is set and cleared by software in order configure PB0 to 
   be used as direct channel CH13 (selected by the ADC interface) if 
   OPAMP2 is in power down mode (OPA2PD bit = 0 in OPAMP_CSR register. 
   See Figure 70: OPAMP2 signal routing on page 341.
0: PB0 can be used as slow ADC channel
1: PB0 can be used as fast ADC channel
Note: This bit is available in Cat.3, Cat.4 and Cat.5 devices only */

/* Bit 26 FCH3: Select GPIO port PA3 as fast ADC input channel CH3. */
#define COMP_FCH3 (1 << )
/* This bit is set and cleared by software in order configure PA3 to 
   be used as direct channel CH13 (selected by the ADC interface) if 
   OPAMP1 is in power down mode (OPA1PD bit = 0 in OPAMP_CSR register. 
   See Figure 69: OPAMP1 signal routing on page 341.
0: PA3 can be used as slow ADC channel
1: PA3 can be used as fast ADC channel
Note: This bit is available in Cat.3, Cat.4 and Cat.5 devices only */

/* Bits 23:21 OUTSEL: Comparator 2 output selection */
#define COMP_OUTSEL (7 << 21)
/* These bits are written by software to connect the output of 
   COMP2 to a selected timer input. */
#define COMP_OUTSEL_TIM2_CAP4  (0 << 21)
#define COMP_OUTSEL_TIM2_OCREF (1 << 21)
#define COMP_OUTSEL_TIM3_CAP4  (2 << 21)
#define COMP_OUTSEL_TIM3_OCREF (3 << 21)
#define COMP_OUTSEL_TIM4_CAP4  (4 << 21)
#define COMP_OUTSEL_TIM4_OCREF (5 << 21)
#define COMP_OUTSEL_TIM10_CAP1 (6 << 21)
#define COMP_OUTSEL_NONE       (7 << 21)
/* 000 = TIM2 Input Capture 4
   001 = TIM2 OCREF_CLR
   010 = TIM3 Input Capture 4
   011 = TIM3 OCREF_CLR
   100 = TIM4 Input Capture 4
   101 = TIM4 OCREF_CLR
   110 = TIM10 Input Capture 1
   111 = no redirection */

/* Bits 20:18 INSEL: Inverted input selection */
#define COMP_INSEL (7 << 18)

#define COMP_INSEL_NONE     (0 << 18)
#define COMP_INSEL_EXT      (1 << 18)
#define COMP_INSEL_VREFINT  (2 << 18)
#define COMP_INSEL_VREF_3_4 (3 << 18)
#define COMP_INSEL_VREF_1_2 (4 << 18)
#define COMP_INSEL_VREF_1_4 (5 << 18)
#define COMP_INSEL_DAC_OUT1 (6 << 18)
#define COMP_INSEL_DAC_OUT2 (7 << 18)
/* 000 = no selection
   001 = External I/O: PB3 (COMP2_INM)
   010 = VREFINT
   011 = 3/4 VREFINT
   100 = 1/2 VREFINT
   101 = 1/4 VREFINT
   110 = DAC_OUT1
   111 = DAC_OUT2  

Note: The COMP2 comparator is enabled when the INSEL bit values 
are different from "000". */
	
/* Bit 17 WNDWE: Window mode enablea */
#define COMP_WNDWE (1 << 17)
/* 0: Disabled
1: Enabled */

/* Bit 16 VREFOUTEN: VREFINT output enable */
#define COMP_VREFOUTEN (1 << 16)
/* This bit is used to output VREFINT on Group 3 (refer to Figure 28: 
   Internal reference voltage output).
0: Disabled
1: Enabled */

/* Bit 13 CMP2OUT: Comparator 2 output */
#define COMP_CMP2OUT (1 << 13)
/* This bit indicates the low or high level of the comparator 2 output.
0: Comparator 2 output is low when the non-inverting input is at 
a lower voltage than the inverting input
1: Comparator 2 output is high when the non-inverting input is at 
a higher voltage than the inverting input */

/* Bit 12 SPEED: Comparator 2 speed mode */
#define COMP_SPEED (1 << 12)
/* 0: slow speed
1: fast speed */

/* Bit 7 CMP1OUT: Comparator 1 output */
#define COMP_CMP1OUT (1 << 7)
/* This bit indicates the high or low level of the comparator 1 output.
0: Comparator 1 output is low when the non-inverting input is at a lower 
   voltage than the inverting input
1: Comparator 1 output is high when the non-inverting input is at a 
   higher voltage than the inverting input */

/* Bit 5 SW1: COMP1_SW1 analog switch enable */
#define COMP_COMP1_SW1 (1 << 5)
/* This bit is set and cleared by software to control the COMP1_SW1 analog 
   switch in order to redirect OPAMP3 output or PC3 to the ADC switch 
   matrix and/or the negative input of COMP1.
0: COMP1_SW1 analog switch open
1: COMP1_SW1 analog switch closed
Note: This bit is available in Cat.4 and Cat.5 devices only */

/* Bit 4 CMP1EN: Comparator 1 enable */
#define COMP_CMP1EN (1 << 4)
/* 0: Comparator 1 disabled
1: Comparator 1 enabled */

/* Bit 3 400KPD: 400 kO pull-down resistor */
#define COMP_400KPD (1 << 3)
/* This bit enables the 400 kO pull-down resistor.
0: 400 kO pull-down resistor disabled
1: 400 kO pull-down resistor enabled */

/* Bit 2 10KPD: 10 kO pull-down resistor */
#define COMP_10KPD (1 << 2)
/* This bit enables the 10 kO pull-down resistor.
0: 10 kO pull-down resistor disabled
1: 10 kO pull-down resistor enabled */

/* Bit 1 400KPU: 400 kO pull-up resistor */
#define COMP_400KPU (1 << 1)
/* This bit enables the 400 kO pull-up resistor.
0: 400 kO pull-up resistor disabled
1: 400 kO pull-up resistor enabled */

/* Bit 0 10KPU: 10 kO pull-up resistor */
#define COMP_10KPU (1 << 0)
/* This bit enables the 10 kO pull-up resistor.
0: 10 kO pull-up resistor disabled
1: 10 kO pull-up resistor enabled */


#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32_comp {
	volatile uint32_t csr;
};


#endif /* __ASSEMBLER__ */


#endif /* __STM32F_ADC_H__ */
