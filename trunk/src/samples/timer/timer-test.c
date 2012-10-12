/* 
 * File:	 dac-test.c
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


#include <sys/stm32f.h>
#include <stdio.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>

#ifndef CM3_SYSTICK_CLK_HZ
#error "CM3_SYSTICK_CLK_HZ undefined"
#endif

struct ticks {
	uint32_t tim7;
	uint32_t tim9;
};

volatile struct ticks g_ticks;

//void __attribute__((section(".ramcode"))) stm32f_tim7_isr(void)
void stm32f_tim7_isr(void)
{
	struct stm32f_tim * tim7 = STM32F_TIM7;
	uint32_t ticks;

	/* Clear update interrupt flag */
	tim7->sr = 0;

	ticks = g_ticks.tim7;
	g_ticks.tim7 = ticks + 1;

#if 0
	if (ticks & 1)
		stm32f_gpio_set(STM32F_GPIOB, 6);
	else
		stm32f_gpio_clr(STM32F_GPIOB, 6);
#endif
}

//void __attribute__((section(".ramcode"))) stm32f_tim1_brk_tim9_isr(void)
void stm32f_tim1_brk_tim9_isr(void)
{
	struct stm32f_tim * tim9 = STM32F_TIM9;
	uint32_t ticks;

	/* Clear update interrupt flag */
	tim9->sr = 0;

	ticks = g_ticks.tim9;
	g_ticks.tim9 = ticks + 1;

#if 0
	if (ticks & 1)
		stm32f_gpio_set(STM32F_GPIOB, 7);
	else
		stm32f_gpio_clr(STM32F_GPIOB, 7);
#endif
}

void tim7_init(void)
{
	struct stm32f_tim * tim7 = STM32F_TIM7;
	struct stm32f_rcc * rcc = STM32F_RCC;

	/* clock enable */
	rcc->apb1enr |= RCC_TIM7EN;

	tim7->cnt = 0;
	tim7->psc = 3 - 1; /* 2 * APB1_CLK(30MHz) / 2 = 20MHz*/
	tim7->arr = 10 - 1; /* 20MHz / 11 = 1.818182 MHz*/

	tim7->egr = TIM_UG; /* Update generation */
	tim7->egr = 0; /* Update generation */
	tim7->dier = TIM_UIE; /* Update interrupt enable */
	tim7->cr2 = 0;
	tim7->cr1 = TIM_URS | TIM_CEN;

	cm3_irq_enable(STM32F_IRQ_TIM7);
}

void tim9_init(void)
{
	struct stm32f_tim * tim9 = STM32F_TIM9;
	struct stm32f_rcc * rcc = STM32F_RCC;

	/* clock enable */
	rcc->apb2enr |= RCC_TIM9EN;

	tim9->cnt = 0;
	tim9->psc = 6 - 1; /* 2 * APB2_CLK(60MHz) / 6 = 20MHz*/
	tim9->arr = 10 - 1; /* 20MHz / 11 = 1.818182 MHz */

	tim9->egr = TIM_UG; /* Update generation */
	tim9->egr = 0; /* Update generation */
	tim9->dier = TIM_UIE; /* Update interrupt enable */
	tim9->cr2 = 0;
	tim9->cr1 = TIM_URS | TIM_CEN;

	cm3_irq_enable(STM32F_IRQ_TIM9);
}

void cm3_systick_init(uint32_t freq)
{
	struct cm3_systick * systick = CM3_SYSTICK;

	systick->load = (CM3_SYSTICK_CLK_HZ / freq) - 1;
	systick->val = 0;
	systick->ctrl = SYSTICK_CTRL_ENABLE;
}

FILE * stdout = (FILE *)&stm32f_usart5_file;
FILE * stderr = (FILE *)&stm32f_usart5_file;

void io_init(void)
{
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_mode(STM32F_GPIOB, 6, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32f_gpio_mode(STM32F_GPIOB, 7, OUTPUT, PUSH_PULL | SPEED_MED);
}

//int __attribute__((section(".ramcode"))) main(int argc, char ** argv)
int main(int argc, char ** argv)
{
	struct stm32f_usart * us = STM32F_USART5;
	struct cm3_systick * systick = CM3_SYSTICK;
	uint32_t sys_freq = 1;
	struct ticks ticks;
	uint32_t tim7_old;
	uint32_t tim7_freq;
	uint32_t tim9_old;
	uint32_t tim9_freq;
	uint32_t sec;

	stm32f_usart_open(us, 115200, SERIAL_8N1);
	cm3_udelay_calibrate();
	cm3_systick_init(sys_freq);

	io_init();
	tim7_init();
	tim9_init();

	cm3_except_pri_set(CM3_EXCEPT_SYSTICK, 0);
	cm3_irq_pri_set(STM32F_IRQ_TIM7, 0);
	cm3_irq_pri_set(STM32F_IRQ_TIM9, 0);

	printf("\r\n");

	ticks = g_ticks;
	/* initialize clock */
	sec = 0;
	tim7_old = ticks.tim7;
	tim9_old = ticks.tim9;
	for (;;) {
		while (!(systick->ctrl & SYSTICK_CTRL_COUNTFLAG));
		ticks = g_ticks;

		sec++;
		printf("- %6d:", sec);
		tim7_freq = ticks.tim7 - tim7_old; 	
		tim7_old = ticks.tim7; 	
		printf("%9d", tim7_freq);

		tim9_freq = ticks.tim9 - tim9_old; 	
		tim9_old = ticks.tim9; 	
		printf("%9d", tim9_freq);

		printf(" Hz \n");
	}

	return 0;
}

