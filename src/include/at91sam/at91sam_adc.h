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
 * @file arch/at91_adc.h
 * @brief ARM CPU macros and definitions 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __AT91_ADC_H__
#define __AT91_ADC_H__

/* Analog to Digital Converter */

#define ADC_CR          ( 0) /* ADC Control Register */
#define ADC_MR          ( 4) /* ADC Mode Register */
#define ADC_CHER        (16) /* ADC Channel Enable Register */
#define ADC_CHDR        (20) /* ADC Channel Disable Register */
#define ADC_CHSR        (24) /* ADC Channel Status Register */
#define ADC_SR          (28) /* ADC Status Register */
#define ADC_LCDR        (32) /* ADC Last Converted Data Register */
#define ADC_IER         (36) /* ADC Interrupt Enable Register */
#define ADC_IDR         (40) /* ADC Interrupt Disable Register */
#define ADC_IMR         (44) /* ADC Interrupt Mask Register */
#define ADC_CDR0        (48) /* ADC Channel Data Register 0 */
#define ADC_CDR1        (52) /* ADC Channel Data Register 1 */
#define ADC_CDR2        (56) /* ADC Channel Data Register 2 */
#define ADC_CDR3        (60) /* ADC Channel Data Register 3 */
#define ADC_CDR4        (64) /* ADC Channel Data Register 4 */
#define ADC_CDR5        (68) /* ADC Channel Data Register 5 */
#define ADC_CDR6        (72) /* ADC Channel Data Register 6 */
#define ADC_CDR7        (76) /* ADC Channel Data Register 7 */
#define ADC_RPR         (256) /* Receive Pointer Register */
#define ADC_RCR         (260) /* Receive Counter Register */
#define ADC_TPR         (264) /* Transmit Pointer Register */
#define ADC_TCR         (268) /* Transmit Counter Register */
#define ADC_RNPR        (272) /* Receive Next Pointer Register */
#define ADC_RNCR        (276) /* Receive Next Counter Register */
#define ADC_TNPR        (280) /* Transmit Next Pointer Register */
#define ADC_TNCR        (284) /* Transmit Next Counter Register */
#define ADC_PTCR        (288) /* PDC Transfer Control Register */
#define ADC_PTSR        (292) /* PDC Transfer Status Register */

/* ADC_CR - ADC Control Register */
#define ADC_SWRST           (0x1 <<  0) /* (ADC) Software Reset */
#define ADC_START           (0x1 <<  1) /* (ADC) Start Conversion */

/* ADC_MR - ADC Mode Register */
#define ADC_TRGEN           (0x1 <<  0) /* (ADC) Trigger Enable */
#define 	ADC_TRGEN_DIS                  (0x0) /* (ADC) Hradware triggers are disabled. Starting a conversion is only possible by software */
#define 	ADC_TRGEN_EN                   (0x1) /* (ADC) Hardware trigger selected by TRGSEL field is enabled. */
#define ADC_TRGSEL          (0x7 <<  1) /* (ADC) Trigger Selection */
#define 	ADC_TRGSEL_TIOA0 (0x0 <<  1) /* (ADC) Selected TRGSEL = TIAO0 */
#define 	ADC_TRGSEL_TIOA1 (0x1 <<  1) /* (ADC) Selected TRGSEL = TIAO1 */
#define 	ADC_TRGSEL_TIOA2 (0x2 <<  1) /* (ADC) Selected TRGSEL = TIAO2 */
#define 	ADC_TRGSEL_TIOA3 (0x3 <<  1) /* (ADC) Selected TRGSEL = TIAO3 */
#define 	ADC_TRGSEL_TIOA4 (0x4 <<  1) /* (ADC) Selected TRGSEL = TIAO4 */
#define 	ADC_TRGSEL_TIOA5 (0x5 <<  1) /* (ADC) Selected TRGSEL = TIAO5 */
#define 	ADC_TRGSEL_EXT   (0x6 <<  1) /* (ADC) Selected TRGSEL = External Trigger */
#define ADC_LOWRES          (0x1 <<  4) /* (ADC) Resolution. */
#define 	ADC_LOWRES_10_BIT     (0x0 <<  4) /* (ADC) 10-bit resolution */
#define 	ADC_LOWRES_8_BIT      (0x1 <<  4) /* (ADC) 8-bit resolution */
#define ADC_SLEEP           (0x1 <<  5) /* (ADC) Sleep Mode */
#define 	ADC_SLEEP_NORMAL_MODE (0x0 <<  5) /* (ADC) Normal Mode */
#define 	ADC_SLEEP_MODE        (0x1 <<  5) /* (ADC) Sleep Mode */
#define ADC_PRESCAL         (0x3f <<  8) /* (ADC) Prescaler rate selection */
#define ADC_STARTUP         (0x1f << 16) /* (ADC) Startup Time */
#define ADC_SHTIM           (0xf << 24) /* (ADC) Sample & Hold Time */

/* -------- 	ADC_CHER : (ADC Offset: 0x10) ADC Channel Enable Register -------- */
#define ADC_CH0             (0x1 <<  0) /* (ADC) Channel 0 */
#define ADC_CH1             (0x1 <<  1) /* (ADC) Channel 1 */
#define ADC_CH2             (0x1 <<  2) /* (ADC) Channel 2 */
#define ADC_CH3             (0x1 <<  3) /* (ADC) Channel 3 */
#define ADC_CH4             (0x1 <<  4) /* (ADC) Channel 4 */
#define ADC_CH5             (0x1 <<  5) /* (ADC) Channel 5 */
#define ADC_CH6             (0x1 <<  6) /* (ADC) Channel 6 */
#define ADC_CH7             (0x1 <<  7) /* (ADC) Channel 7 */

/* -------- 	ADC_CHDR : (ADC Offset: 0x14) ADC Channel Disable Register -------- */

/* -------- 	ADC_CHSR : (ADC Offset: 0x18) ADC Channel Status Register -------- */

/* ADC_SR - ADC Status Register */
#define ADC_EOC0            (0x1 <<  0) /* (ADC) End of Conversion */
#define ADC_EOC1            (0x1 <<  1) /* (ADC) End of Conversion */
#define ADC_EOC2            (0x1 <<  2) /* (ADC) End of Conversion */
#define ADC_EOC3            (0x1 <<  3) /* (ADC) End of Conversion */
#define ADC_EOC4            (0x1 <<  4) /* (ADC) End of Conversion */
#define ADC_EOC5            (0x1 <<  5) /* (ADC) End of Conversion */
#define ADC_EOC6            (0x1 <<  6) /* (ADC) End of Conversion */
#define ADC_EOC7            (0x1 <<  7) /* (ADC) End of Conversion */
#define ADC_OVRE0           (0x1 <<  8) /* (ADC) Overrun Error */
#define ADC_OVRE1           (0x1 <<  9) /* (ADC) Overrun Error */
#define ADC_OVRE2           (0x1 << 10) /* (ADC) Overrun Error */
#define ADC_OVRE3           (0x1 << 11) /* (ADC) Overrun Error */
#define ADC_OVRE4           (0x1 << 12) /* (ADC) Overrun Error */
#define ADC_OVRE5           (0x1 << 13) /* (ADC) Overrun Error */
#define ADC_OVRE6           (0x1 << 14) /* (ADC) Overrun Error */
#define ADC_OVRE7           (0x1 << 15) /* (ADC) Overrun Error */
#define ADC_DRDY            (0x1 << 16) /* (ADC) Data Ready */
#define ADC_GOVRE           (0x1 << 17) /* (ADC) General Overrun */
#define ADC_ENDRX           (0x1 << 18) /* (ADC) End of Receiver Transfer */
#define ADC_RXBUFF          (0x1 << 19) /* (ADC) RXBUFF Interrupt */

/* ADC_LCDR - ADC Last Converted Data Register */
#define ADC_LDATA           (0x3ff <<  0) /* (ADC) Last Data Converted */

/* ADC_IER - ADC Interrupt Enable Register */

/* ADC_IDR - ADC Interrupt Disable Register */

/* ADC_IMR - ADC Interrupt Mask Register */

/* -------- ADC_CDR0 : (ADC Offset: 0x30) ADC Channel Data Register 0 -------- */
#define ADC_DATA            (0x3ff <<  0) /* (ADC) Converted Data */

/* ADC_CDR1 - ADC Channel Data Register 1 */

/* ADC_CDR2 - ADC Channel Data Register 2 */

/* ADC_CDR3 - ADC Channel Data Register 3 */

/* ADC_CDR4 - ADC Channel Data Register 4 */

/* ADC_CDR5 - ADC Channel Data Register 5 */

/* ADC_CDR6 - ADC Channel Data Register 6 */

/* ADC_CDR7 - ADC Channel Data Register 7 */

#ifndef __ASSEMBLER__
/*  */
#include <stdint.h>

struct at91_adc {
	volatile uint32_t cr; /* ADC Control Register */
	volatile uint32_t mr; /* ADC Mode Register */
	uint32_t res0[2];
	volatile uint32_t cher; /* ADC Channel Enable Register */
	volatile uint32_t chdr; /* ADC Channel Disable Register */
	volatile uint32_t chsr; /* ADC Channel Status Register */
	volatile uint32_t sr; /* ADC Status Register */
	volatile uint32_t lcdr; /* ADC Last Converted Data Register */
	volatile uint32_t ier; /* ADC Interrupt Enable Register */
	volatile uint32_t idr; /* ADC Interrupt Disable Register */
	volatile uint32_t imr; /* ADC Interrupt Mask Register */
	volatile uint32_t cdr0; /* ADC Channel Data Register 0 */
	volatile uint32_t cdr1; /* ADC Channel Data Register 1 */
	volatile uint32_t cdr2; /* ADC Channel Data Register 2 */
	volatile uint32_t cdr3; /* ADC Channel Data Register 3 */
	volatile uint32_t cdr4; /* ADC Channel Data Register 4 */
	volatile uint32_t cdr5; /* ADC Channel Data Register 5 */
	volatile uint32_t cdr6; /* ADC Channel Data Register 6 */
	volatile uint32_t cdr7; /* ADC Channel Data Register 7 */
	uint32_t res1[44];
#if 0
	volatile uint32_t rpr; /* Receive Pointer Register */
	volatile uint32_t rcr; /* Receive Counter Register */
	volatile uint32_t tpr; /* Transmit Pointer Register */
	volatile uint32_t tcr; /* Transmit Counter Register */
	volatile uint32_t rnpr; /* Receive Next Pointer Register */
	volatile uint32_t rncr; /* Receive Next Counter Register */
	volatile uint32_t tnpr; /* Transmit Next Pointer Register */
	volatile uint32_t tncr; /* Transmit Next Counter Register */
	volatile uint32_t ptcr; /* PDC Transfer Control Register */
	volatile uint32_t ptsr; /* PDC Transfer Status Register */
#endif
};

#endif /* __ASSEMBLER__ */

#endif /* __AT91_ADC__ */

