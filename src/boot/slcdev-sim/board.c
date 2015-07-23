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

#include <thinkos.h>

#include "board.h"

#define PWM_FREQ 100000

static void io_leds_init(void)
{
	struct stm32f_tim * tim;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = (stm32f_tim1_hz + (PWM_FREQ / 2)) / PWM_FREQ;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div * 2 + pre) / (2 * pre);

	stm32_gpio_mode(LED3, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED4, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED5, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED6, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_af(LED3, GPIO_AF2);
	stm32_gpio_af(LED4, GPIO_AF2);
	stm32_gpio_af(LED5, GPIO_AF2);
	stm32_gpio_af(LED6, GPIO_AF2);

	stm32_gpio_mode_out(LED5);
	stm32_gpio_mode_out(LED6);
	stm32_gpio_set(LED3);
	stm32_gpio_set(LED4);
	stm32_gpio_set(LED5);
	stm32_gpio_set(LED6);

	/* Timer clock enable */
	tim = STM32_TIM3;
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM3);
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->ccmr1 = TIM_OC1M_PWM_MODE1 | TIM_OC2M_PWM_MODE1;
	tim->ccmr2 = TIM_OC3M_PWM_MODE1 | TIM_OC4M_PWM_MODE1;
	tim->ccer = TIM_CC1E | TIM_CC2E | TIM_CC3E | TIM_CC4E;
	tim->ccr1 = (tim->arr * 15) / 16;
	tim->ccr2 = (tim->arr * 15) / 16;
	tim->ccr3 = (tim->arr * 15) / 16;
	tim->ccr4 = (tim->arr * 15) / 16;
	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */

	stm32_gpio_mode(LED1, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED2, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_af(LED1, GPIO_AF1);
	stm32_gpio_af(LED2, GPIO_AF1);
	stm32_gpio_set(LED1);
	stm32_gpio_set(LED2);

	/* Timer clock enable */
	tim = STM32_TIM2;
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM2);
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->ccmr1 = TIM_OC1M_PWM_MODE1 | TIM_OC2M_PWM_MODE1;
	tim->ccmr2 = TIM_OC3M_PWM_MODE1 | TIM_OC4M_PWM_MODE1;
	tim->ccer = TIM_CC1E | TIM_CC2E | TIM_CC3E | TIM_CC4E;
	tim->ccr1 = (tim->arr * 4) / 8;
	tim->ccr2 = (tim->arr * 4) / 8;
	tim->ccr3 = (tim->arr * 14) / 16; /* White LED */
	tim->ccr4 = (tim->arr * 10) / 16; /* Blue LED */
	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */

	/* Negative voltage supply */
	stm32_gpio_mode(VNEG_SW, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(VNEG_SW, GPIO_AF1);
}

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);

	stm32_gpio_mode(ADDR0, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(ADDR1, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(ADDR2, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(ADDR3, INPUT, PULL_UP | SPEED_LOW);

	stm32_gpio_mode(ADDR4, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(ADDR5, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(ADDR6, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(ADDR7, INPUT, PULL_UP | SPEED_LOW);

	stm32_gpio_mode(SW3A, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(SW3B, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(SW4A, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(SW4B, INPUT, PULL_UP | SPEED_LOW);

	/* USART2_TX */
	stm32_gpio_mode(USART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(USART2_TX, GPIO_AF7);
	/* USART2_RX */
	stm32_gpio_mode(USART2_RX, ALT_FUNC, 0);
	stm32_gpio_af(USART2_RX, GPIO_AF7);

	io_leds_init();
}

