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
 * @file stm32f-adc.h
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __STM32F_ADC_H__
#define __STM32F_ADC_H__

/*-------------------------------------------------------------------------
 * Analog-to-digital converter (ADC)
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * ADC status register */
#define STM32F_ADC_SR 0x00

/* Bits 31:6 Reserved, must be kept cleared. */

#if defined(STM32F2X) || defined(STM32F4X)

/* Bit 5 - Overrun */
#define ADC_OVR (1 << 5)
/* This bit is set by hardware when data are lost (either in single mode 
   or in dual/triple mode). It is cleared by software. Overrun detection is 
   enabled only when DMA = 1 or EOCS = 1.
   0: No overrun occurred
   1: Overrun has occurred */

#endif

/* Bit 4 - Regular channel start flag */
#define ADC_STRT (1 << 4)
/* This bit is set by hardware when regular channel conversion starts. It 
   is cleared by software.
   0: No regular channel conversion started
   1: Regular channel conversion has started */

/* Bit 3 - Injected channel start flag */
#define ADC_JSTRT (1 << 3)
/* This bit is set by hardware when injected group conversion starts. It 
   is cleared by software.
   0: No injected group conversion started
   1: Injected group conversion has started */

/* Bit 2 - Injected channel end of conversion */
#define ADC_JEOC (1 << 2)
/* This bit is set by hardware at the end of the conversion of all 
   injected channels in the group. It is cleared by software.
   0: Conversion is not complete
   1: Conversion complete */

/* Bit 1 - Regular channel end of conversion */
#define ADC_EOC (1 << 1)
/* This bit is set by hardware at the end of the conversion of a regular 
   group of channels. It is cleared by software or by reading the ADC_DR 
   register.
   0: Conversion not complete (EOCS=0), or sequence of conversions not 
   complete (EOCS=1)
   1: Conversion complete (EOCS=0), or sequence of conversions 
   complete (EOCS=1) */

/* Bit 0 - Analog watchdog flag */
#define ADC_AWD (1 << 0)
/* This bit is set by hardware when the converted voltage crosses the values 
   programmed in the ADC_LTR and ADC_HTR registers. It is cleared by software.
   0: No analog watchdog event occurred
   1: Analog watchdog event occurred */


/*-------------------------------------------------------------------------
 * ADC control register 1 */
#define STM32F_ADC_CR1 0x04

#if defined(STM32F2X) || defined(STM32F4X)

/* Bit 26 - Overrun interrupt enable */
#define ADC_OVRIE (1 << 26)
/* This bit is set and cleared by software to enable/disable the Overrun 
   interrupt.
   0: Overrun interrupt disabled
   1: Overrun interrupt enabled. An interrupt is generated when the OVR 
   bit is set. */

/* Bits [25..24] - Resolution */
#define ADC_RES_MSK (((1 << (1 + 1)) - 1) << 24)
#define ADC_RES_12BIT (0 << 24)
#define ADC_RES_10BIT (1 << 24)
#define ADC_RES_8BIT  (2 << 24)
#define ADC_RES_6BIT  (3 << 24)
#define ADC_RES_SET(VAL) (((VAL) << 24) & ADC_RES_MSK)
#define ADC_RES_GET(REG) (((REG) & ADC_RES_MSK) >> 24)
/* These bits are written by software to select the resolution of 
   the conversion.
   00: 12-bit (15 ADCCLK cycles)
   01: 10-bit (13 ADCCLK cycles)
   10: 8-bit (11 ADCCLK cycles)
   11: 6-bit (9 ADCCLK cycles) */
#endif

/* Bit 23 - Analog watchdog enable on regular channels */
#define ADC_AWDEN (1 << 23)
/* This bit is set and cleared by software.
   0: Analog watchdog disabled on regular channels
   1: Analog watchdog enabled on regular channels */

/* Bit 22 - Analog watchdog enable on injected channels */
#define ADC_JAWDEN (1 << 22)
/* This bit is set and cleared by software.
   0: Analog watchdog disabled on injected channels
   1: Analog watchdog enabled on injected channels */

/* Bits 21:16 Reserved, must be kept cleared. */

/* Bits [15..13] - Discontinuous mode channel count */
#define ADC_DISCNUM_MSK (((1 << (2 + 1)) - 1) << 13)
#define ADC_DISCNUM_SET(VAL) (((VAL) << 13) & ADC_DISCNUM_MSK)
#define ADC_DISCNUM_GET(REG) (((REG) & ADC_DISCNUM_MSK) >> 13)
/* These bits are written by software to define the number of regular 
   channels to be converted in discontinuous mode, after receiving an 
   external trigger.
   000: 1 channel
   001: 2 channels
   ...
   111: 8 channels */

/* Bit 12 - Discontinuous mode on injected channels */
#define ADC_JDISCEN (1 << 12)
/* This bit is set and cleared by software to enable/disable 
   discontinuous mode on the injected channels of a group.
   0: Discontinuous mode on injected channels disabled
   1: Discontinuous mode on injected channels enabled */

/* Bit 11 - Discontinuous mode on regular channels */
#define ADC_DISCEN (1 << 11)
/* This bit is set and cleared by software to enable/disable Discontinuous 
   mode on regular channels.
   0: Discontinuous mode on regular channels disabled
   1: Discontinuous mode on regular channels enabled */

/* Bit 10 - Automatic injected group conversion */
#define ADC_JAUTO (1 << 10)
/* This bit is set and cleared by software to enable/disable automatic 
   injected group conversion after regular group conversion.
   0: Automatic injected group conversion disabled
   1: Automatic injected group conversion enabled */

/* Bit 9 - Enable the watchdog on a single channel in scan mode */
#define ADC_AWDSGL (1 << 9)
/* This bit is set and cleared by software to enable/disable the analog 
   watchdog on the channel identified by the AWDCH[4:0] bits.
   0: Analog watchdog enabled on all channels
   1: Analog watchdog enabled on a single channel */

/* Bit 8 - Scan mode */
#define ADC_SCAN (1 << 8)
/* This bit is set and cleared by software to enable/disable the Scan 
   mode. In Scan mode, the inputs selected through the ADC_SQRx or 
   ADC_JSQRx registers are converted.
   0: Scan mode disabled
   1: Scan mode enabled
   Note: An EOC interrupt is generated if the EOCIE bit is set:
   – At the end of each regular group sequence if the EOCS bit is cleared to 0
   – At the end of each regular channel conversion if the EOCS bit is set to 1
   Note: A JEOC interrupt is generated only on the end of conversion of the 
   last channel if the JEOCIE bit is set. */

/* Bit 7 - Interrupt enable for injected channels */
#define ADC_JEOCIE (1 << 7)
/* This bit is set and cleared by software to enable/disable the end of 
   conversion interrupt for injected channels.
   0: JEOC interrupt disabled
   1: JEOC interrupt enabled. An interrupt is generated when the 
   JEOC bit is set. */

/* Bit 6 - Analog watchdog interrupt enable */
#define ADC_AWDIE (1 << 6)
/* This bit is set and cleared by software to enable/disable the 
   analog watchdog interrupt. In Scan mode if the watchdog thresholds are 
   crossed, scan is aborted only if this bit is enabled.
   0: Analog watchdog interrupt disabled
   1: Analog watchdog interrupt enabled */

/* Bit 5 - Interrupt enable for EOC */
#define ADC_EOCIE (1 << 5)
/* This bit is set and cleared by software to enable/disable the end of 
   conversion interrupt.
   0: EOC interrupt disabled
   1: EOC interrupt enabled. An interrupt is generated when the EOC 
   bit is set. */

/* Bits [4..0] - Analog watchdog channel select bits */
#define ADC_AWDCH_MSK (((1 << (4 + 1)) - 1) << 0)
#define ADC_AWDCH_SET(VAL) (((VAL) << 0) & ADC_AWDCH_MSK)
#define ADC_AWDCH_GET(REG) (((REG) & ADC_AWDCH_MSK) >> 0)
/* These bits are set and cleared by software. They select the input 
   channel to be guarded by the analog watchdog.
   Note: 00000: ADC analog input Channel0
   00001: ADC analog input Channel1
   ...
   01111: ADC analog input Channel15
   10000: ADC analog input Channel16
   10001: ADC analog input Channel17
   10010: ADC analog input Channel18
   Other values reserved */


/*-------------------------------------------------------------------------
 * ADC control register 2 */
#define STM32F_ADC_CR2 0x08


#if defined(STM32F1X)


/* Bit 23 - Temperature sensor and VREFINT enable */
#define ADC_TSVREFE (1 << 30)
/* This bit is set and cleared by software to enable/disable the 
   temperature sensor and VREFINT channel.
   0: Temperature sensor and VREFINT channel disabled
   1: Temperature sensor and VREFINT channel enabled */

/* Bit 22 - Start conversion of regular channels */
#define ADC_SWSTART (1 << 22)
/* This bit is set by software to start conversion and cleared by hardware 
   as soon as the conversion starts. It starts a conversion of a group 
   of regular channels if SWSTART is selected as trigger event by 
   the EXTSEL[2:0] bits.
   0: Reset state
   1: Starts conversion of regular channels */


/* Bit 22 - Start conversion of injected channels */
#define ADC_JSWSTART (1 << 21)
/* This bit is set by software and cleared by hardware as soon as the 
   conversion starts.
   0: Reset state
   1: Starts conversion of injected channels
   Note: This bit can be set only when ADON = 1 otherwise no conversion 
   is launched. */


/* Bit 22 - External trigger conversion mode for regular channels */
#define ADC_EXTTRIG (1 << 20)
/* This bit is set and cleared by software to enable/disable the external 
   trigger used to start conversion of a regular channel group.
   0: Conversion on external event disabled
   1: Conversion on external event enabled */

/* Bits [19..17] - External event select for regular group */
#define ADC_EXTSEL_MSK (((1 << (3)) - 1) << 17)
#define ADC_EXTSEL_SET(VAL) (((VAL) << 17) & ADC_EXTSEL_MSK)
#define ADC_EXTSEL_GET(REG) (((REG) & ADC_EXTSEL_MSK) >> 17)

#define ADC_EXTSEL_TIM1_CC1  (0x0 << 17)
#define ADC_EXTSEL_TIM1_CC2  (0x1 << 17)
#define ADC_EXTSEL_TIM1_CC3  (0x2 << 17)
#define ADC_EXTSEL_TIM2_CC2  (0x3 << 17)
#define ADC_EXTSEL_TIM3_TRGO (0x4 << 17)
#define ADC_EXTSEL_TIM4_CC4  (0x5 << 17)
#define ADC_EXTSEL_EXTI      (0x6 << 17)
#define ADC_EXTSEL_SWSTART   (0x7 << 17)

/* These bits select the external event used to trigger the start of 
   conversion of a regular group:
   000: Timer 1 CC1 event
   001: Timer 1 CC2 event
   010: Timer 1 CC3 event
   011: Timer 2 CC2 event
   100: Timer 3 TRGO event
   101: Timer 4 CC4 event
   110: EXTI line11
   111: SWSTART */



/* Bit 22 - External trigger conversion mode for injected channels */
#define ADC_JEXTTRIG (1 << 15)
/* This bit is set and cleared by software to enable/disable the 
   external trigger used to start conversion of an injected channel group.
   0: Conversion on external event disabled
   1: Conversion on external event enabled */


/* Bits [14..12] - External event select for injected group */
#define ADC_JEXTSEL_MSK (((1 << 3) - 1) << 12)
#define ADC_JEXTSEL_SET(VAL) (((VAL) << 12) & ADC_JEXTSEL_MSK)
#define ADC_JEXTSEL_GET(REG) (((REG) & ADC_JEXTSEL_MSK) >> 12)
/* These bits select the external event used to trigger the start of 
   conversion of an injected group.
   000: Timer 1 TRGO event
   001: Timer 1 CC4 event
   010: Timer 2 TRGO event
   011: Timer 2 CC1 event
   100: Timer 3 CC4 event
   101: Timer 4 TRGO event
   110: EXTI line15 
   111: JSWSTART */

#elif defined(STM32F2X) || defined(STM32F4X)

/* Bit 30 - Start conversion of regular channels */
#define ADC_SWSTART (1 << 30)
/* This bit is set by software to start conversion and cleared by hardware 
   as soon as the conversion starts.
   0: Reset state
   1: Starts conversion of regular channels
   Note: This bit can be set only when ADON = 1 otherwise no conversion 
   is launched. */

/* Bits [29..28] - External trigger enable for regular channels */
#define ADC_EXTEN ((29 - 28) << 28)
#define ADC_EXTEN_RISING (1 << 28)
#define ADC_EXTEN_FALLING (2 << 28)
#define ADC_EXTEN_BOTH (3 << 28)
/* These bits are set and cleared by software to select the external 
   trigger polarity and enable the trigger of a regular group.
   00: Trigger detection disabled
   01: Trigger detection on the rising edge
   10: Trigger detection on the falling edge
   11: Trigger detection on both the rising and falling edges */

/* Bits [27..24] - External event select for regular group */
#define ADC_EXTSEL_MSK (((1 << (4)) - 1) << 24)
#define ADC_EXTSEL_SET(VAL) (((VAL) << 24) & ADC_EXTSEL_MSK)
#define ADC_EXTSEL_GET(REG) (((REG) & ADC_EXTSEL_MSK) >> 24)
#define ADC_EXTSEL_TIM1_CC1  (0x0 << 24)
#define ADC_EXTSEL_TIM1_CC2  (0x1 << 24)
#define ADC_EXTSEL_TIM1_CC3  (0x2 << 24)
#define ADC_EXTSEL_TIM2_CC2  (0x3 << 24)
#define ADC_EXTSEL_TIM2_CC3  (0x4 << 24)
#define ADC_EXTSEL_TIM2_CC4  (0x5 << 24)
#define ADC_EXTSEL_TIM2_TRGO (0x6 << 24)
#define ADC_EXTSEL_TIM3_CC1  (0x7 << 24)
#define ADC_EXTSEL_TIM3_TRGO (0x8 << 24)
#define ADC_EXTSEL_TIM4_CC4  (0x9 << 24)
#define ADC_EXTSEL_TIM5_CC1  (0xa << 24)
#define ADC_EXTSEL_TIM5_CC2  (0xb << 24)
#define ADC_EXTSEL_TIM5_CC3  (0xc << 24)
#define ADC_EXTSEL_TIM8_CC1  (0xd << 24)
#define ADC_EXTSEL_TIM8_TRGO (0xe << 24)
#define ADC_EXTSEL_EXTI      (0xf << 24)
/* These bits select the external event used to trigger the start of 
   conversion of a regular group:
   0000: Timer 1 CC1 event
   0001: Timer 1 CC2 event
   0010: Timer 1 CC3 event
   0011: Timer 2 CC2 event
   0100: Timer 2 CC3 event
   0101: Timer 2 CC4 event
   0110: Timer 2 TRGO event
   0111: Timer 3 CC1 event
   1000: Timer 3 TRGO event
   1001: Timer 4 CC4 event
   1010: Timer 5 CC1 event
   1011: Timer 5 CC2 event
   1100: Timer 5 CC3 event
   1101: Timer 8 CC1 event
   1110: Timer 8 TRGO event
   1111: EXTI line11 */

/* Bit 23 Reserved, must be kept cleared. */

/* Bit 22 - Start conversion of injected channels */
#define ADC_JSWSTART (1 << 22)
/* This bit is set by software and cleared by hardware as soon as the 
   conversion starts.
   0: Reset state
   1: Starts conversion of injected channels
   Note: This bit can be set only when ADON = 1 otherwise no conversion 
   is launched. */

/* Bits [21..20] - External trigger enable for injected channels */
#define ADC_JEXTEN ((21 - 20) << 20)
/* These bits are set and cleared by software to select the external trigger 
   polarity and enable the trigger of an injected group.
   00: Trigger detection disabled
   01: Trigger detection on the rising edge
   10: Trigger detection on the falling edge
   11: Trigger detection on both the rising and falling edges */


/* Bits [19..16] - External event select for injected group */
#define ADC_JEXTSEL_MSK (((1 << 4) - 1) << 16)
#define ADC_JEXTSEL_SET(VAL) (((VAL) << 16) & ADC_JEXTSEL_MSK)
#define ADC_JEXTSEL_GET(REG) (((REG) & ADC_JEXTSEL_MSK) >> 16)
/* These bits select the external event used to trigger the start of 
   conversion of an injected group.
   0000: Timer 1 CC4 event
   0001: Timer 1 TRGO event
   0010: Timer 2 CC1 event
   0011: Timer 2 TRGO event
   0100: Timer 3 CC2 event
   0101: Timer 3 CC4 event
   0110: Timer 4 CC1 event
   0111: Timer 4 CC2 event
   1000: Timer 4 CC3 event
   1001: Timer 4 TRGO event
   1010: Timer 5 CC4 event
   1011: Timer 5 TRGO event
   1100: Timer 8 CC2 event
   1101: Timer 8 CC3 event
   1110: Timer 8 CC4 event
   1111: EXTI line15 */

#endif

/* Bit 11 - Data alignment */
#define ADC_ALIGN (1 << 11)
/* This bit is set and cleared by software. 
   Refer to Figure 32 and Figure 33.
   0: Right alignment
   1: Left alignment */

#if defined(STM32F2X) || defined(STM32F4X)

/* Bit 10 - End of conversion selection */
#define ADC_EOCS (1 << 10)
/* This bit is set and cleared by software.
   0: The EOC bit is set at the end of each sequence of regular 
   conversions. Overrun detection is enabled only if DMA=1.
   1: The EOC bit is set at the end of each regular conversion. Overrun 
   detection is enabled. */

/* Bit 9 - DMA disable selection (for single ADC mode) */
#define ADC_DDS (1 << 9)
/* This bit is set and cleared by software.
   0: No new DMA request is issued after the last transfer (as configured 
   in the DMA controller)
   1: DMA requests are issued as long as data are converted and DMA=1 */

#endif

/* Bit 8 - Direct memory access mode (for single ADC mode) */
#define ADC_DMA (1 << 8)
/* This bit is set and cleared by software. Refer to the DMA controller 
   chapter for more details.
   0: DMA mode disabled
   1: DMA mode enabled */

/* Bits 7:2 Reserved, must be kept cleared. */

#if defined(STM32F1X)

/* Bit 3 - Reset calibration */
#define ADC_RSTCAL (1 << 3)
/* This bit is set by software and cleared by hardware. It is cleared after 
   the calibration registers are initialized.
   0: Calibration register initialized.
   1: Initialize calibration register.
   Note: If RSTCAL is set when conversion is ongoing, additional cycles 
   are required to clear the calibration registers. */

/* Bit 2 - A/D Calibration */
#define ADC_CAL (1 << 2)
/* This bit is set by software to start the calibration. It is reset 
   by hardware after calibration is complete.
   0: Calibration completed
   1: Enable calibration */

#endif


/* Bit 1 - Continuous conversion */
#define ADC_CONT (1 << 1)
/* This bit is set and cleared by software. If it is set, conversion 
   takes place continuously until it is cleared.
   0: Single conversion mode
   1: Continuous conversion mode */

/* Bit 0 - A/D Converter ON / OFF */
#define ADC_ADON (1 << 0)
/* This bit is set and cleared by software.
   0: Disable ADC conversion and go to power down mode
   1: Enable ADC */


/*-------------------------------------------------------------------------
 * ADC sample time register 1 */
#define STM32F_ADC_SMPR1 0x0C
/* Bits [26..0] - Channel x sampling time selection */

/*-------------------------------------------------------------------------
 * ADC sample time register 2 */
#define STM32F_ADC_SMPR2 0x10
/* Bits [29..0] - Channel x sampling time selection */

#define ADC_SMP_MSK(X) (0x7 << (3 * (X)))
#define ADC_SMP_SET(X, VAL) (((VAL) << (3 * (X))) & ADC_SMP_MSK(X))
#define ADC_SMP_GET(X, REG) (((REG) & ADC_SMP_MSK(X)) >> (3 * (X)))
/* These bits are written by software to select the sampling time 
   individually for each channel. During sampling cycles, the channel 
   selection bits must remain unchanged.
   Note: 000: 3 cycles
   001: 15 cycles
   010: 28 cycles
   011: 56 cycles
   100: 84 cycles
   101: 112 cycles
   110: 144 cycles
   111: 480 cycles */

#if defined(STM32F1X)

#define ADC_SMP_1_CYC   0
#define ADC_SMP_7_CYC   1
#define ADC_SMP_13_CYC  2
#define ADC_SMP_28_CYC  3
#define ADC_SMP_41_CYC  4
#define ADC_SMP_56_CYC  5
#define ADC_SMP_71_CYC  6
#define ADC_SMP_239_CYC 7

#elif defined(STM32F2X) || defined(STM32F4X)

#define ADC_SMP_3_CYC   0
#define ADC_SMP_15_CYC  1
#define ADC_SMP_28_CYC  2
#define ADC_SMP_56_CYC  3
#define ADC_SMP_84_CYC  4
#define ADC_SMP_112_CYC 5
#define ADC_SMP_144_CYC 6
#define ADC_SMP_480_CYC 7

#endif

/*-------------------------------------------------------------------------
 * ADC injected channel data offset register x */
#define STM32F_ADC_JOFR1 0x14
#define STM32F_ADC_JOFR2 0x18
#define STM32F_ADC_JOFR3 0x1c
#define STM32F_ADC_JOFR4 0x20


/* Bits [11..0] - Data offset for injected channel x */
#define ADC_JOFFSETX_MSK (((1 << (11 + 1)) - 1) << 0)
#define ADC_JOFFSETX_SET(VAL) (((VAL) << 0) & ADC_JOFFSETX_MSK)
#define ADC_JOFFSETX_GET(REG) (((REG) & ADC_JOFFSETX_MSK) >> 0)
/* These bits are written by software to define the offset to be subtracted from the raw
converted data when converting injected channels. The conversion result can be read from
in the ADC_JDRx registers. */


/*-------------------------------------------------------------------------
 * ADC watchdog higher threshold register */
#define STM32F_ADC_HTR 0x24


/* Bits [11..0] - Analog watchdog higher threshold */
#define ADC_HT_MSK (((1 << (11 + 1)) - 1) << 0)
#define ADC_HT_SET(VAL) (((VAL) << 0) & ADC_HT_MSK)
#define ADC_HT_GET(REG) (((REG) & ADC_HT_MSK) >> 0)
/* These bits are written by software to define the higher threshold for the analog watchdog.
 */

/*-------------------------------------------------------------------------
 * ADC watchdog lower threshold register */
#define STM32F_ADC_LTR 0x28


/* Bits [11..0] - Analog watchdog lower threshold */
#define ADC_LT_MSK (((1 << (11 + 1)) - 1) << 0)
#define ADC_LT_SET(VAL) (((VAL) << 0) & ADC_LT_MSK)
#define ADC_LT_GET(REG) (((REG) & ADC_LT_MSK) >> 0)
/* These bits are written by software to define the lower threshold for the analog watchdog. */


/*-------------------------------------------------------------------------
 * ADC regular sequence register 1 */
#define STM32F_ADC_SQR1 0x2C


/* Bits [23..20] - Regular channel sequence length */
#define ADC_L_MSK (((1 << (4)) - 1) << 20)
#define ADC_L_SET(VAL) ((((VAL) - 1) << 20) & ADC_L_MSK)
#define ADC_L_GET(REG) ((((REG) & ADC_L_MSK) >> 20) + 1)
/* These bits are written by software to define the total number of 
   conversions in the regular channel conversion sequence.
   0000: 1 conversion
   0001: 2 conversions
   ...
   1111: 16 conversions */

/* Bits [19..15] - 16th conversion in regular sequence */
#define ADC_SQ16_MSK (((1 << (4 + 1)) - 1) << 15)
#define ADC_SQ16_SET(VAL) (((VAL) << 15) & ADC_SQ16_MSK)
#define ADC_SQ16_GET(REG) (((REG) & ADC_SQ16_MSK) >> 15)
/* These bits are written by software with the channel number (0..18) 
   assigned as the 16th in the conversion sequence. */

/* Bits [14..10] 15th conversion in regular sequence */
#define ADC_SQ15_MSK (((1 << (4 + 1)) - 1) << 10)
#define ADC_SQ15_SET(VAL) (((VAL) << 10) & ADC_SQ15_MSK)
#define ADC_SQ15_GET(REG) (((REG) & ADC_SQ15_MSK) >> 10)

/* Bits [9..5] - 14th conversion in regular sequence */
#define ADC_SQ14_MSK (((1 << (4 + 1)) - 1) << 5)
#define ADC_SQ14_SET(VAL) (((VAL) << 5) & ADC_SQ14_MSK)
#define ADC_SQ14_GET(REG) (((REG) & ADC_SQ14_MSK) >> 5)

/* Bits [4..0] - 13th conversion in regular sequence */
#define ADC_SQ13_MSK (((1 << (4 + 1)) - 1) << 0)
#define ADC_SQ13_SET(VAL) (((VAL) << 0) & ADC_SQ13_MSK)
#define ADC_SQ13_GET(REG) (((REG) & ADC_SQ13_MSK) >> 0)


/*-------------------------------------------------------------------------
 * ADC regular sequence register 2 */
#define STM32F_ADC_SQR2 0x30


/* Bits [29..26] - 12th conversion in regular sequence */
#define ADC_SQ12_MSK (((1 << (4 + 1)) - 1) << 26)
#define ADC_SQ12_SET(VAL) (((VAL) << 26) & ADC_SQ12_MSK)
#define ADC_SQ12_GET(REG) (((REG) & ADC_SQ12_MSK) >> 26)
/* These bits are written by software with the channel number (0..18) 
   assigned as the 12th in the sequence to be converted. */

/* Bits [24..20] - 11th conversion in regular sequence */
#define ADC_SQ11_MSK (((1 << (4 + 1)) - 1) << 20)
#define ADC_SQ11_SET(VAL) (((VAL) << 20) & ADC_SQ11_MSK)
#define ADC_SQ11_GET(REG) (((REG) & ADC_SQ11_MSK) >> 20)

/* Bits [19..15] - 10th conversion in regular sequence */
#define ADC_SQ10_MSK (((1 << (4 + 1)) - 1) << 15)
#define ADC_SQ10_SET(VAL) (((VAL) << 15) & ADC_SQ10_MSK)
#define ADC_SQ10_GET(REG) (((REG) & ADC_SQ10_MSK) >> 15)

/* Bits [14..10] - 9th conversion in regular sequence */
#define ADC_SQ9_MSK (((1 << (4 + 1)) - 1) << 10)
#define ADC_SQ9_SET(VAL) (((VAL) << 10) & ADC_SQ9_MSK)
#define ADC_SQ9_GET(REG) (((REG) & ADC_SQ9_MSK) >> 10)

/* Bits [9..5] - 8th conversion in regular sequence */
#define ADC_SQ8_MSK (((1 << (4 + 1)) - 1) << 5)
#define ADC_SQ8_SET(VAL) (((VAL) << 5) & ADC_SQ8_MSK)
#define ADC_SQ8_GET(REG) (((REG) & ADC_SQ8_MSK) >> 5)

/* Bits [4..0] - 7th conversion in regular sequence */
#define ADC_SQ7_MSK (((1 << (4 + 1)) - 1) << 0)
#define ADC_SQ7_SET(VAL) (((VAL) << 0) & ADC_SQ7_MSK)
#define ADC_SQ7_GET(REG) (((REG) & ADC_SQ7_MSK) >> 0)


/*-------------------------------------------------------------------------
 * ADC regular sequence register 3 */
#define STM32F_ADC_SQR3 0x34


/* Bits [29..25] - 6th conversion in regular sequence */
#define ADC_SQ6_MSK (((1 << (4 + 1)) - 1) << 25)
#define ADC_SQ6_SET(VAL) (((VAL) << 25) & ADC_SQ6_MSK)
#define ADC_SQ6_GET(REG) (((REG) & ADC_SQ6_MSK) >> 25)
/* These bits are written by software with the channel number (0..18) 
   assigned as the 6th in the sequence to be converted. */

/* Bits [24..20] - 5th conversion in regular sequence */
#define ADC_SQ5_MSK (((1 << (4 + 1)) - 1) << 20)
#define ADC_SQ5_SET(VAL) (((VAL) << 20) & ADC_SQ5_MSK)
#define ADC_SQ5_GET(REG) (((REG) & ADC_SQ5_MSK) >> 20)

/* Bits [19..15] - 4th conversion in regular sequence */
#define ADC_SQ4_MSK (((1 << (4 + 1)) - 1) << 15)
#define ADC_SQ4_SET(VAL) (((VAL) << 15) & ADC_SQ4_MSK)
#define ADC_SQ4_GET(REG) (((REG) & ADC_SQ4_MSK) >> 15)

/* Bits [14..10] - 3rd conversion in regular sequence */
#define ADC_SQ3_MSK (((1 << (4 + 1)) - 1) << 10)
#define ADC_SQ3_SET(VAL) (((VAL) << 10) & ADC_SQ3_MSK)
#define ADC_SQ3_GET(REG) (((REG) & ADC_SQ3_MSK) >> 10)

/* Bits [9..5] - 2nd conversion in regular sequence */
#define ADC_SQ2_MSK (((1 << (4 + 1)) - 1) << 5)
#define ADC_SQ2_SET(VAL) (((VAL) << 5) & ADC_SQ2_MSK)
#define ADC_SQ2_GET(REG) (((REG) & ADC_SQ2_MSK) >> 5)

/* Bits [4..0] - 1st conversion in regular sequence */
#define ADC_SQ1_MSK (((1 << (4 + 1)) - 1) << 0)
#define ADC_SQ1_SET(VAL) (((VAL) << 0) & ADC_SQ1_MSK)
#define ADC_SQ1_GET(REG) (((REG) & ADC_SQ1_MSK) >> 0)


/*-------------------------------------------------------------------------
 * ADC injected sequence register */
#define STM32F_ADC_JSQR 0x38


/* Bits [21..20] - Injected sequence length */
#define ADC_JL_MSK (((1 << (1 + 1)) - 1) << 20)
#define ADC_JL_SET(VAL) (((VAL) << 20) & ADC_JL_MSK)
#define ADC_JL_GET(REG) (((REG) & ADC_JL_MSK) >> 20)
/* These bits are written by software to define the total number of conversions in the injected
channel conversion sequence.
00: 1 conversion
01: 2 conversions
10: 3 conversions
11: 4 conversions */

/* Bits [19..15] - 4th conversion in injected sequence (when JL[1:0]=3, see note below) */
#define ADC_JSQ4_MSK (((1 << (4 + 1)) - 1) << 15)
#define ADC_JSQ4_SET(VAL) (((VAL) << 15) & ADC_JSQ4_MSK)
#define ADC_JSQ4_GET(REG) (((REG) & ADC_JSQ4_MSK) >> 15)
/* These bits are written by software with the channel number (0..18) assigned as the 4th in the
sequence to be converted. */

/* Bits [14..10] - 3rd conversion in injected sequence (when JL[1:0]=3, see note below) */
#define ADC_JSQ3_MSK (((1 << (4 + 1)) - 1) << 10)
#define ADC_JSQ3_SET(VAL) (((VAL) << 10) & ADC_JSQ3_MSK)
#define ADC_JSQ3_GET(REG) (((REG) & ADC_JSQ3_MSK) >> 10)

/* Bits [9..5] - 2nd conversion in injected sequence (when JL[1:0]=3, see note below) */
#define ADC_JSQ2_MSK (((1 << (4 + 1)) - 1) << 5)
#define ADC_JSQ2_SET(VAL) (((VAL) << 5) & ADC_JSQ2_MSK)
#define ADC_JSQ2_GET(REG) (((REG) & ADC_JSQ2_MSK) >> 5)

/* Bits [4..0] - 1st conversion in injected sequence (when JL[1:0]=3, see note below) */
#define ADC_JSQ1_MSK (((1 << (4 + 1)) - 1) << 0)
#define ADC_JSQ1_SET(VAL) (((VAL) << 0) & ADC_JSQ1_MSK)
#define ADC_JSQ1_GET(REG) (((REG) & ADC_JSQ1_MSK) >> 0)
/* Note:
When JL[1:0]=3 (4 injected conversions in the sequencer), the ADC converts the channels
in the following order: JSQ1[4:0], JSQ2[4:0], JSQ3[4:0], and JSQ4[4:0].
When JL=2 (3 injected conversions in the sequencer), the ADC converts the channels in the
following order: JSQ2[4:0], JSQ3[4:0], and JSQ4[4:0].
When JL=1 (2 injected conversions in the sequencer), the ADC converts the channels in
starting from JSQ3[4:0], and then JSQ4[4:0].
When JL=0 (1 injected conversion in the sequencer), the ADC converts only JSQ4[4:0]
channel. */


/*-------------------------------------------------------------------------
 * ADC injected data register x */
#define STM32F_ADC_JDRX 0x3C


/* Bits [15..0] - Injected data */
#define ADC_JDATA_MSK (((1 << (15 + 1)) - 1) << 0)
#define ADC_JDATA_SET(VAL) (((VAL) << 0) & ADC_JDATA_MSK)
#define ADC_JDATA_GET(REG) (((REG) & ADC_JDATA_MSK) >> 0)
/* These bits are read-only. They contain the conversion result from 
   injected channel x. The data are left -or right-aligned as shown 
   in Figure 32 and Figure 33. */


/*-------------------------------------------------------------------------
 * ADC regular data register */
#define STM32F_ADC_DR 0x4C


/* Bits [15..0] - Regular data */
#define ADC_DATA_MSK (((1 << (15 + 1)) - 1) << 0)
#define ADC_DATA_SET(VAL) (((VAL) << 0) & ADC_DATA_MSK)
#define ADC_DATA_GET(REG) (((REG) & ADC_DATA_MSK) >> 0)
/* These bits are read-only. They contain the conversion result from 
   the regular channels. The data are left- or right-aligned as shown 
   in Figure 32 and Figure 33. */


#if defined(STM32F2X) || defined(STM32F4X)

/*-------------------------------------------------------------------------
 * ADC Common status register */
#define STM32F_ADC_CSR 0x00
/* This register provides an image of the status bits of the different ADCs. 
   Nevertheless it is read-only and does not allow to clear the different 
   status bits. Instead each status bit must be cleared by writing it to 0 
   in the corresponding ADC_SR register. */

/* Bit 21 - Overrun flag of ADC3 */
#define ADC_OVR3 (1 << 21)
/* This bit is a copy of the OVR bit in the ADC3_SR register. */

/* Bit 20 - Regular channel Start flag of ADC3 */
#define ADC_STRT3 (1 << 20)
/* This bit is a copy of the STRT bit in the ADC3_SR register. */

/* Bit 19 - Injected channel Start flag of ADC3 */
#define ADC_JSTRT3 (1 << 19)
/* This bit is a copy of the JSTRT bit in the ADC3_SR register. */

/* Bit 18 - Injected channel end of conversion of ADC3 */
#define ADC_JEOC3 (1 << 18)
/* This bit is a copy of the JEOC bit in the ADC3_SR register. */

/* Bit 17 - End of conversion of ADC3 */
#define ADC_EOC3 (1 << 17)
/* This bit is a copy of the EOC bit in the ADC3_SR register. */

/* Bit 16 - Analog watchdog flag of ADC3 */
#define ADC_AWD3 (1 << 16)
/* This bit is a copy of the AWD bit in the ADC3_SR register. */

/* Bits 15:14 Reserved, must be kept cleared. */

/* Bit 13 - Overrun flag of ADC2 */
#define ADC_OVR2 (1 << 13)
/* This bit is a copy of the OVR bit in the ADC2_SR register. */

/* Bit 12 - Regular channel Start flag of ADC2 */
#define ADC_STRT2 (1 << 12)
/* This bit is a copy of the STRT bit in the ADC2_SR register. */

/* Bit 11 - Injected channel Start flag of ADC2 */
#define ADC_JSTRT2 (1 << 11)
/* This bit is a copy of the JSTRT bit in the ADC2_SR register. */

/* Bit 10 - Injected channel end of conversion of ADC2 */
#define ADC_JEOC2 (1 << 10)
/* This bit is a copy of the JEOC bit in the ADC2_SR register. */

/* Bit 9 - End of conversion of ADC2 */
#define ADC_EOC2 (1 << 9)
/* This bit is a copy of the EOC bit in the ADC2_SR register. */

/* Bit 8 - Analog watchdog flag of ADC2 */
#define ADC_AWD2 (1 << 8)
/* This bit is a copy of the AWD bit in the ADC2_SR register. */

/* Bits 7:6 Reserved, must be kept cleared. */

/* Bit 5 - Overrun flag of ADC1 */
#define ADC_OVR1 (1 << 5)
/* This bit is a copy of the OVR bit in the ADC1_SR register. */

/* Bit 4 - Regular channel Start flag of ADC1 */
#define ADC_STRT1 (1 << 4)
/* This bit is a copy of the STRT bit in the ADC1_SR register. */

/* Bit 3 - Injected channel Start flag of ADC1 */
#define ADC_JSTRT1 (1 << 3)
/* This bit is a copy of the JSTRT bit in the ADC1_SR register. */

/* Bit 2 - Injected channel end of conversion of ADC1 */
#define ADC_JEOC1 (1 << 2)
/* This bit is a copy of the JEOC bit in the ADC1_SR register. */

/* Bit 1 - End of conversion of ADC1 */
#define ADC_EOC1 (1 << 1)
/* This bit is a copy of the EOC bit in the ADC1_SR register. */

/* Bit 0 - Analog watchdog flag of ADC1 */
#define ADC_AWD1 (1 << 0)
/* This bit is a copy of the AWD bit in the ADC1_SR register. */

/*-------------------------------------------------------------------------
 * ADC common control register */
#define STM32F_ADC_CCR 0x04

/* Bit 23 - Temperature sensor and VREFINT enable */
#define ADC_TSVREFE (1 << 23)
/* This bit is set and cleared by software to enable/disable the 
   temperature sensor and the VREFINT channel.
   0: Temperature sensor and VREFINT channel disabled
   1: Temperature sensor and VREFINT channel enabled */

/* Bit 22 - VBAT enable */
#define ADC_VBATE (1 << 22)
/* This bit is set and cleared by software to enable/disable the VBAT channel.
   0: VBAT channel disabled
   1: VBAT channel enabled */

/* Bits 21:18 Reserved, must be kept cleared. */

/* Bits [17..16] - ADC prescaler */
#define ADC_ADCPRE ((17 - 16) << 16)
#define ADC_ADCPRE_2 (0 << 16)
#define ADC_ADCPRE_4 (1 << 16)
#define ADC_ADCPRE_6 (2 << 16)
#define ADC_ADCPRE_8 (3 << 16)
/* Set and cleared by software to select the frequency of the clock 
   to the ADC. The clock is common for all the ADCs.
   Note: 00: PCLK2 divided by 2
   01: PCLK2 divided by 4
   10: PCLK2 divided by 6
   11: PCLK2 divided by 8 */

/* Bits [15..14] - Direct memory access mode for multi ADC mode */
#define ADCC_DMA ((15 - 14) << 14)
//#define ADC_DMA ((15 - 14) << 14)
/* This bit-field is set and cleared by software. Refer to the DMA 
   controller section for more details.
   00: DMA mode disabled
   01: DMA mode 1 enabled (2 / 3 half-words one by one - 1 then 2 then 3)
   10: DMA mode 2 enabled (2 / 3 half-words by pairs - 2&1 then 1&3 then 3&2)
   11: DMA mode 3 enabled (2 / 3 bytes by pairs - 2&1 then 1&3 then 3&2) */

/* Bit 13 - DMA disable selection (for multi-ADC mode) */
#define ADCC_DDS (1 << 13)
/* This bit is set and cleared by software.
   0: No new DMA request is issued after the last transfer (as configured 
   in the DMA controller).
   DMA bits are not cleared by hardware, however they must have been 
   cleared and set to the wanted mode by software before new DMA requests 
   can be generated.
   1: DMA requests are issued as long as data are converted 
   and DMA = 01, 10 or 11. */

/* Bit 12 Reserved, must be kept cleared. */

/* Bits [11..8] - Delay between 2 sampling phases */
#define ADC_DELAY ((11 - 8) << 8)
/* Set and cleared by software. These bits are used in dual or triple 
   interleaved modes.
   0000: 5 * TADCCLK
   0001: 6 * TADCCLK
   0010: 7 * TADCCLK
   ...
   1111: 20 * TADCCLK */

/* Bits 7:5 Reserved, must be kept cleared. */

/* Bits [4..0] - Multi ADC mode selection */
#define ADC_MULTI_MSK (((1 << (4 + 1)) - 1) << 0)
#define ADC_MULTI_SET(VAL) (((VAL) << 0) & ADC_MULTI_MSK)
#define ADC_MULTI_GET(REG) (((REG) & ADC_MULTI_MSK) >> 0)
/* These bits are written by software to select the operating mode.
   – All the ADCs independent:
   00000: Independent mode
   – 00001 to 01001: Dual mode, ADC1 and ADC2 working together, 
   ADC3 is independent
   00001: Combined regular simultaneous + injected simultaneous mode
   00010: Combined regular simultaneous + alternate trigger mode
   00011: Reserved
   00101: Injected simultaneous mode only
   00110: Regular simultaneous mode only
   00111: interleaved mode only
   01001: Alternate trigger mode only
   – 10001 to 11001: Triple mode: ADC1, 2 and 3 working together
   10001: Combined regular simultaneous + injected simultaneous mode
   10010: Combined regular simultaneous + alternate trigger mode
   10011: Reserved
   10101: Injected simultaneous mode only
   10110: Regular simultaneous mode only
   10111: interleaved mode only
   11001: Alternate trigger mode only
   All other combinations are reserved and must not be programmed
   Note: In multi mode, a change of channel configuration generates an 
   abort that can cause a loss of synchronization. It is recommended to 
   disable the multi ADC mode before any configuration change. */


/*-------------------------------------------------------------------------
 * ADC common regular data register for dual and triple modes */
#define STM32F_ADC_CDR 0x08 
/* this offset address is relative to ADC1 base address + 0x300 */

/* Bits [31..16] - 2nd data item of a pair of regular conversions */
#define ADC_DATA2_MSK (((1 << (15 + 1)) - 1) << 16)
#define ADC_DATA2_SET(VAL) (((VAL) << 16) & ADC_DATA2_MSK)
#define ADC_DATA2_GET(REG) (((REG) & ADC_DATA2_MSK) >> 16)
/* – In dual mode, these bits contain the regular data of ADC2. Refer to 
   Dual ADC mode.
   – In triple mode, these bits contain alternatively the regular data of 
   ADC2, ADC1 and ADC3.
   Refer to Triple ADC mode. */

/* Bits [15..0] - 1st data item of a pair of regular conversions */
#define ADC_DATA1_MSK (((1 << (15 + 1)) - 1) << 0)
#define ADC_DATA1_SET(VAL) (((VAL) << 0) & ADC_DATA1_MSK)
#define ADC_DATA1_GET(REG) (((REG) & ADC_DATA1_MSK) >> 0)
/* – In dual mode, these bits contain the regular data of ADC1. Refer to 
   Dual ADC mode
   – In triple mode, these bits contain alternatively the regular data of 
   ADC1, ADC3 and ADC2. 
   Refer to Triple ADC mode. */

#endif /* STM32F2X */

#ifndef __ASSEMBLER__

#include <stdint.h>

struct stm32f_adc {
	volatile uint32_t sr;
	volatile uint32_t cr1;
	volatile uint32_t cr2;
	volatile uint32_t smpr1;

	volatile uint32_t smpr2;
	volatile uint32_t jofr1;
	volatile uint32_t jofr2;
	volatile uint32_t jofr3;

	volatile uint32_t jofr4;
	volatile uint32_t htr;
	volatile uint32_t ltr;
	volatile uint32_t sqr1;

	volatile uint32_t sqr2;
	volatile uint32_t sqr3;
	volatile uint32_t jsqr;
	volatile uint32_t jsdr1;

	volatile uint32_t jsdr2;
	volatile uint32_t jsdr3;
	volatile uint32_t jsdr4;
	volatile uint32_t dr;
};

#if defined(STM32F2X) || defined(STM32F4X)
struct stm32f_adcc {
	volatile uint32_t csr;
	volatile uint32_t ccr;
	volatile uint32_t cdr;
};
#endif

/* set the sample time for an specific ADC channel */
static inline void stm32f_adc_smp_set(struct stm32f_adc * adc, 
									   unsigned int chan, unsigned int val) {
	uint32_t smpr;
	
	if (chan > 9) {
		smpr = adc->smpr1 & ~ADC_SMP_MSK(chan - 10);
		adc->smpr1 = smpr | ADC_SMP_SET(chan - 10, val);
	} else {
		smpr = adc->smpr2 & ~ADC_SMP_MSK(chan);
		adc->smpr2 = smpr | ADC_SMP_SET(chan, val);
	}
}

/* set the conversion channel sequence */
static inline void stm32f_adc_seq_set(struct stm32f_adc * adc, 
									  const uint8_t * chan, unsigned int len) {
	uint32_t sqr1;
	uint32_t sqr2;
	uint32_t sqr3;
	int n;
	int i;

	sqr1 = ADC_L_SET(len);
	sqr2 = 0;
	sqr3 = 0;

	/* First 6 channels */
	n = (len > 6) ? 6 : len;
	for (i = 0; i < n; i++)
		sqr3 |= chan[i] << (5 * i);

	/* Channels 7 to 12 */
	len -= n;
	n = (len > 6) ? 6 : len;
	for (i = 0; i < len; i++)
		sqr2 |= chan[i + 6] << (5 * i);

	/* Channels 13 to 16 */
	len -= n;
	n = (len > 4) ? 4 : len;
	for (i = 0; i < len; i++)
		sqr1 |= chan[i + 12] << (5 * i);

	adc->sqr1 = sqr1;
	adc->sqr2 = sqr2;
	adc->sqr3 = sqr3;
}


#endif /* __ASSEMBLER__ */


#endif /* __STM32F_ADC_H__ */
