/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "board.h"
#include "isink.h"
#include <sys/dcclog.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#define FREQ_1MHZ 1000000
#define ISINK_DELAY_COMPENASTION 4


void isink_start(unsigned int mode, unsigned int pre, unsigned int pulse)
{
	struct stm32f_tim * tim = STM32_TIM4;

	switch (mode) {

	case 0:
		/* 130mA, 48mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		tim->ccr2 = pulse - pre;
		tim->ccr3 = 1;
		break;

	case 1:
		/* 160mA, 58mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		tim->ccr2 = pulse - pre;
		tim->ccr3 = 1;
		tim->ccr4 = 1;
		break;

	case 2:
		/* 180mA, 48mA */
		stm32_gpio_mode(SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;

	case 3:
		/* 200mA, 54mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		tim->ccr2 = 1;
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;

	case 4:
		/* 214mA, 78mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		tim->ccr2 = pulse - pre;
		tim->ccr3 = 1;
		break;

	case 5:
		/* 216mA, 85mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		tim->ccr2 = 1;
		tim->ccr4 = pulse - pre;
		break;

	case 6:
		/* 230mA, 90mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		tim->ccr2 = 1;
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;

	case 7:
		/* 260mA, 48mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		tim->ccr2 = pulse - pre;
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;

	case 8:
		/* 260mA, 85mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		tim->ccr2 = 1;
		tim->ccr3 = pulse - pre;
		tim->ccr4 = pulse - pre;
		break;

	case 9:
		/* 260mA, 131mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		tim->ccr2 = 1;
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;

	case 10:
		/* 298mA, 78mA */
		stm32_gpio_mode(SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;

	case 11:
		/* 320mA, 114mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		tim->ccr2 = pulse - pre;
		tim->ccr3 = 1;
		tim->ccr4 = 1;
		break;

	case 12:
		/* 360mA, 140mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		tim->ccr2 = 1;
		tim->ccr4 = pulse - pre;
		break;

	case 13:
		/* 380mA, 96mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		tim->ccr2 = 1;
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;

	case 14:
		/* 408mA, 156mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		tim->ccr2 = 1;
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;

	case 15:
		/* 440mA, 78mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		tim->ccr2 = pulse - pre;
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;

	case 16:
		/* 440mA, 144mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		tim->ccr2 = 1;
		tim->ccr3 = pulse - pre;
		tim->ccr4 = pulse - pre;
		break;

	case 17:
		/* 440mA, 200mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		tim->ccr2 = 1;
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;

	case 18:
		/* 440mA, 220mA */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		tim->ccr2 = pulse - pre;
		tim->ccr3 = pulse - pre;
		tim->ccr4 = 1;
		break;

	}

	tim->arr = 1;
 	tim->cnt = pulse + ISINK_DELAY_COMPENASTION;
	tim->cr1 = TIM_CMS_EDGE | TIM_DIR_DOWN | TIM_OPM | TIM_URS | TIM_CEN; 
}

void _isink_start(unsigned int mode, unsigned int pre, unsigned int pulse)
{
	struct stm32f_tim * tim = STM32_TIM4;

//	tim->arr = pre;
//	tim->ccr2 = pulse - pre;
//	tim->ccr3 = 1;

	if (mode >= 20) {
//		stm32_gpio_mode(SINK4, INPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		mode -= 20;
	} else {
		stm32_gpio_clr(SINK4);
//		stm32_gpio_mode(SINK4, OUTPUT, PUSH_PULL | SPEED_HIGH);
	}

	switch (mode) {
	case 0:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pulse - pre;
		tim->ccr3 = 1;
		tim->ccr4 = 1;
		break;
	case 1:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = 1;
		tim->ccr3 = pulse - pre;
		tim->ccr4 = 1;
		break;
	case 2:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = 1;
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;
	case 3:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pulse - pre;
		tim->ccr3 = pulse - pre;
		tim->ccr4 = 1;
		break;
	case 4:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = 1;
		tim->ccr3 = pulse - pre;
		tim->ccr4 = pulse - pre;
		break;
	case 5:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pulse - pre;
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;
	case 6:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = 1;
		tim->ccr3 = pulse - pre;
		break;
	case 7:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pulse - pre;
		tim->ccr3 = 1;
		break;
	case 8:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = 1;
		tim->ccr4 = pulse - pre;
		break;
	case 9:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pulse - pre;
		tim->ccr4 = 1;
		break;
	case 10:
		stm32_gpio_mode(SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;
	case 11:
		stm32_gpio_mode(SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr3 = pulse - pre;
		tim->ccr4 = 1;
		break;

	case 12:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		tim->ccr2 = 1;
		tim->ccr3 = pulse - pre;
		tim->ccr4 = 1;
		break;
	case 13:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		tim->ccr2 = pulse - pre;
		tim->ccr3 = 1;
		tim->ccr4 = 1;
		break;
	case 14:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = 1;
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;
	case 15:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pulse - pre;
		tim->ccr3 = 1;
		tim->ccr4 = 1;
		break;
	case 16:
		stm32_gpio_mode(SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = 1;
		tim->ccr3 = 1;
		tim->ccr4 = pulse - pre;
		break;
	case 17:
		stm32_gpio_mode(SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = 1;
		tim->ccr3 = pulse - pre;
		tim->ccr4 = 1;
		break;
	}

	tim->arr = 1;
 	tim->cnt = pulse + ISINK_DELAY_COMPENASTION;
	tim->cr1 = TIM_CMS_EDGE | TIM_DIR_DOWN | TIM_OPM | TIM_URS | TIM_CEN; 
}

void isink_stop(void)
{
	struct stm32f_tim * tim = STM32_TIM4;

	tim->cr1 = 0;

	stm32_gpio_mode(SINK1, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(SINK4, OUTPUT, PUSH_PULL | SPEED_MED);
}

#define SLEWRATE_DAC_VAL(X) ((4095 * ((X) - SLEWRATE_MIN)) / \
							 (SLEWRATE_MAX - SLEWRATE_MIN))

const uint16_t slewrate_dac_lut[4] = {
	[ISINK_RATE_VERY_SLOW >> 4] = SLEWRATE_DAC_VAL(100),
	[ISINK_RATE_SLOW >> 4]      = SLEWRATE_DAC_VAL(625),
	[ISINK_RATE_NORMAL >> 4]    = SLEWRATE_DAC_VAL(1250),
	[ISINK_RATE_FAST >> 4]      = SLEWRATE_DAC_VAL(2500)
};

void isink_mode_set(unsigned int mode)
{
	struct stm32f_dac * dac = STM32_DAC;

#if 0
	switch (mode & 0x3) {
	case ISINK_CURRENT_LOW:
		DCC_LOG(LOG_TRACE, "current low");
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		break;

	case ISINK_CURRENT_NOM:
		DCC_LOG(LOG_TRACE, "current nominal");
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		break;

	case ISINK_CURRENT_HIGH:
		DCC_LOG(LOG_TRACE, "current high");
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		break;

	case ISINK_CURRENT_DOUBLE:
		DCC_LOG(LOG_TRACE, "current double");
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		break;
	}
#endif

	dac->dhr12r2 = slewrate_dac_lut[(mode >> 4) & 0x3];

	DCC_LOG1(LOG_TRACE, "irate DAC=%d", dac->dhr12r2);
}

void isink_pulse(unsigned int pre, unsigned int pulse)
{
	struct stm32f_tim * tim = STM32_TIM4;

	stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);

	tim->ccr2 = pulse - pre;
	tim->ccr3 = 1;
	tim->arr = 1;
 	tim->cnt = pulse + ISINK_DELAY_COMPENASTION;
	tim->cr1 = TIM_CMS_EDGE | TIM_DIR_DOWN | TIM_OPM | TIM_URS | TIM_CEN; 
}

void isink_pulse_low(unsigned int pre, unsigned int pulse)
{
	struct stm32f_tim * tim = STM32_TIM4;

	stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);

	tim->ccr2 = pulse - pre;
	tim->ccr3 = 1;
	tim->arr = 1;
 	tim->cnt = pulse + ISINK_DELAY_COMPENASTION;
	tim->cr1 = TIM_CMS_EDGE | TIM_DIR_DOWN | TIM_OPM | TIM_URS | TIM_CEN; 
}

void isink_pulse_high(unsigned int pre, unsigned int pulse)
{
	struct stm32f_tim * tim = STM32_TIM4;

	stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);

	tim->ccr2 = pulse;
	tim->ccr3 = pre;
	tim->arr = pre;
 	tim->cnt = pulse + pre;
	tim->cr1 = TIM_CMS_EDGE | TIM_DIR_DOWN | TIM_OPM | TIM_URS | TIM_CEN; 
}

void isink_pulse_double(unsigned int pre, unsigned int pulse)
{
	struct stm32f_tim * tim = STM32_TIM4;

	stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);

	tim->ccr2 = pulse;
	tim->ccr3 = pre;
	tim->arr = pre;
 	tim->cnt = pulse + pre;
	tim->cr1 = TIM_CMS_EDGE | TIM_DIR_DOWN | TIM_OPM | TIM_URS | TIM_CEN; 
}

void isink_init(void)
{
	struct stm32f_dac * dac = STM32_DAC;
	struct stm32f_tim * tim = STM32_TIM4;
	uint32_t div;

	stm32_gpio_clr(SINK1);
	stm32_gpio_clr(SINK2);
	stm32_gpio_clr(SINK3);
	stm32_gpio_clr(SINK4);
	stm32_gpio_mode(SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(SINK4, OUTPUT, OPEN_DRAIN | SPEED_HIGH);
	stm32_gpio_af(SINK1, GPIO_AF2);
	stm32_gpio_af(SINK2, GPIO_AF2);
	stm32_gpio_af(SINK3, GPIO_AF2);

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM4);
	
	/* get the total divisior */
	div = (stm32f_tim1_hz + (FREQ_1MHZ / 2)) / FREQ_1MHZ;
	/* Timer configuration */
	tim->cr1 = 0;
	tim->psc = div - 1;
	tim->arr = 100;
	tim->ccmr1 = TIM_OC1M_PWM_MODE2 | TIM_OC2M_PWM_MODE2;
	tim->ccmr2 = TIM_OC3M_PWM_MODE2 | TIM_OC4M_PWM_MODE2;
	tim->ccer = TIM_CC1E | TIM_CC2E | TIM_CC3E | TIM_CC4E;
	//	TIM_CC1P | TIM_CC2P | TIM_CC3P | TIM_CC4P;
	tim->ccr1 = 0;
	tim->ccr2 = 0;
	tim->ccr3 = 0;
	tim->ccr4 = 0;

	tim->dier = TIM_UIE; /* Update interrupt enable */
	cm3_irq_pri_set(STM32_IRQ_TIM4, IRQ_PRIORITY_HIGH);
	/* Enable interrupt */
	cm3_irq_enable(STM32_IRQ_TIM4);


	/* I/O pins config */
	stm32_gpio_mode(IRATE, ANALOG, 0);
	stm32_gpio_clr(IRATE);

	/* DAC clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_DAC);

	/* DAC disable */
	dac->cr = 0;
	/* DAC configure */
	dac->cr = DAC_EN2;
	/* DAC channel 2 initial value */
	dac->dhr12r2 = 0;
	/* DAC channel 1 initial value */
	dac->dhr12r1 = 0;
}

void isink_slewrate_set(unsigned int rate)
{
	struct stm32f_dac * dac = STM32_DAC;
	unsigned int dac_val;

	if (rate < SLEWRATE_MIN)
		rate = SLEWRATE_MIN;
	else if (rate >= SLEWRATE_MAX)
		rate = SLEWRATE_MAX;

	dac_val = (4095 * (rate - SLEWRATE_MIN)) / (SLEWRATE_MAX - SLEWRATE_MIN); 
	DCC_LOG2(LOG_TRACE, "rate=%dmA/us DAC=%d", rate, dac_val);

	dac->dhr12r2 = dac_val;
}

