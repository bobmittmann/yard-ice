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

#define FREQ_1MHZ 1000000

void isink_start(unsigned int mode, unsigned int pre, unsigned int pulse)
{
	struct stm32f_tim * tim = STM32_TIM4;

	if (trig_mode == TRIG_ADDR) {
		stm32_gpio_mode(SINK1, INPUT, 0);
		stm32_gpio_mode(SINK2, INPUT, 0);
		stm32_gpio_mode(SINK3, INPUT, 0);
		return;
	}

	tim->arr = pre;

	switch (mode) {
	case 0:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pulse;
		tim->ccr3 = pre;
		tim->ccr4 = pre;
		break;
	case 1:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pre;
		tim->ccr3 = pulse;
		tim->ccr4 = pre;
		break;
	case 2:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pre;
		tim->ccr3 = pre;
		tim->ccr4 = pulse;
		break;
	case 3:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pulse;
		tim->ccr3 = pulse;
		tim->ccr4 = pre;
		break;
	case 4:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pre;
		tim->ccr3 = pulse;
		tim->ccr4 = pulse;
		break;
	case 5:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pulse;
		tim->ccr3 = pre;
		tim->ccr4 = pulse;
		break;
	case 6:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pre;
		tim->ccr3 = pulse;
		break;
	case 7:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pulse;
		tim->ccr3 = pre;
		break;
	case 8:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pre;
		tim->ccr4 = pulse;
		break;
	case 9:
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr2 = pulse;
		tim->ccr4 = pre;
		break;
	case 10:
		stm32_gpio_mode(SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr3 = pre;
		tim->ccr4 = pulse;
		break;
	case 11:
		stm32_gpio_mode(SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		tim->ccr3 = pulse;
		tim->ccr4 = pre;
		break;
	}

 	tim->cnt = pulse + pre;
	/* trigger the timer */
	tim->cr1 = TIM_CMS_EDGE | TIM_DIR_DOWN | TIM_OPM | TIM_URS | TIM_CEN; 
}

void isink_stop(void)
{
	stm32_gpio_mode(SINK1, INPUT, 0);
	stm32_gpio_mode(SINK2, INPUT, 0);
	stm32_gpio_mode(SINK3, INPUT, 0);
	stm32_gpio_mode(SINK4, INPUT, 0);
	stm32_gpio_clr(SINK1);
	stm32_gpio_clr(SINK2);
	stm32_gpio_clr(SINK3);
	stm32_gpio_clr(SINK4);
	stm32_gpio_mode(SINK1, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(SINK4, OUTPUT, PUSH_PULL | SPEED_MED);
}

void isink_pulse(unsigned int pre, unsigned int pulse)
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

void isink_pulse_dual(unsigned int pre, unsigned int pulse)
{
	struct stm32f_tim * tim = STM32_TIM4;

	stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);

	tim->ccr2 = pre;
	tim->ccr4 = pulse;
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
	stm32_gpio_mode(SINK4, OUTPUT, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_af(SINK1, GPIO_AF2);
	stm32_gpio_af(SINK2, GPIO_AF2);
	stm32_gpio_af(SINK3, GPIO_AF2);
	stm32_gpio_af(SINK4, GPIO_AF2);

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


	/* I/O pins config */
	stm32_gpio_mode(IRATE, ANALOG, 0);
	stm32_gpio_clr(IRATE);

	/* DAC clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_DAC);

	/* DAC disable */
	dac->cr = 0;
	/* DAC configure */
	dac->cr = DAC_EN2 | DAC_EN1;
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
//	DCC_LOG2(LOG_TRACE, "rate=%dmA/us DAC=%d", rate, dac_val);

	dac->dhr12r2 = dac_val;
}

