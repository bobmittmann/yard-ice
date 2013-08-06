/* 
 * File:	 dgpot.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
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
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <stdint.h>
#include <stdio.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

#include <sys/dcclog.h>

/* 
 * MCP402X digital potentiometer protocol 
 * 
 * Increment:
 *
 *  __  ______                              _________
 *  CS        |___________________________|
 *            :
 *            :--LCUR---:--THI--:-TLO-:
 *                      :       :     :  
 *   _     _______       _______       _______
 * U/D  __|       |_____|       |_____|       |_______
 *                      :             :
 *                      :             :
 *                      :              ______________
 *   W                   _____________|
 *      ________________|
 *
 * Decrement:
 *
 *  __  ______                            ___________
 *  CS        |__________________________|
 *            :
 *            :-LCUR-:--THI--:--TLO--:
 *                   :       :       :  
 *   _                _______         _________    
 * U/D  _____________|       |_______|         |________
 *                   :               :
 *                   :               :
 *      _____________                :
 *   W               |_______________
 *                                   |__________________
 *
 * T_LCUR(MIN) = 3uS
 * T_LO(MIN) = 500nS
 * T_HI(MI) = 500nS
 * 
 */


#define DGPOT_STEPS 64
#define DGPOT_CHIPS 2

struct dgpot_drv {
	int32_t flag;
	uint32_t lcur;
	uint8_t pos[DGPOT_CHIPS];
} dgpot;

#define DGPOT_NCS0 STM32F_GPIOB, 11
#define DGPOT_NCS1 STM32F_GPIOB, 12
#define DGPOT_UPDWN STM32F_GPIOB, 13

void stm32f_tim1_up_tim16_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM1;

	/* Clear interrupt flags */
	tim->sr = 0;

	stm32f_gpio_set(DGPOT_NCS0);
	stm32f_gpio_set(DGPOT_NCS1);

	DCC_LOG1(LOG_TRACE, "__thinkos_ev_raise(%d)", dgpot.flag);
	__thinkos_flag_signal(dgpot.flag);
}

#define TIMER_CLK_FREQ 1800000

#define DGPOT_IRQ_PRIORITY IRQ_PRIORITY_REGULAR

static void dgpot_timer_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_tim * tim = STM32F_TIM1;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	DCC_LOG(LOG_TRACE, "..");

	/* get the total divisior */
	div = ((2 * stm32f_apb2_hz) + (TIMER_CLK_FREQ / 2)) / TIMER_CLK_FREQ;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;

	/* Timer clock enable */
	rcc->apb2enr |= RCC_TIM1EN;
	
	/* Timer configuration */
	tim->sr= 0; /* Clear interrupts */
	tim->psc = pre - 1;
	tim->arr = n - 1;

	dgpot.lcur = 6 * n - 1;

	tim->cnt = 0;
	tim->egr = 0;
	tim->rcr = 4;
	/* */
	DCC_LOG1(LOG_TRACE, "ARR=%d", tim->arr);
	tim->ccr1 = tim->arr / 2;
	tim->ccmr1 = TIM_OC1M_TOGGLE | TIM_OC1PE;
	tim->ccer = TIM_CC1NE;
	tim->bdtr = TIM_MOE | TIM_OSSR;
	/* Edge mode, down count, one pulse, enable counter */
	tim->cr2 = 0;
	tim->cr1 = TIM_CMS_EDGE | TIM_DIR_DOWN | TIM_OPM | TIM_URS | TIM_CEN; 
	tim->dier = TIM_UIE; /* Update interrupt enable */

	/* Set IRQ to low priority */
	cm3_irq_pri_set(STM32F_IRQ_TIM1_UP, DGPOT_IRQ_PRIORITY);
	cm3_irq_enable(STM32F_IRQ_TIM1_UP);
}

unsigned int dgpot_set(unsigned int cs, int pos)
{
	struct stm32f_tim * tim = STM32F_TIM1;
	int diff;

	/* limit the range */
	if (pos < 0)
		pos = 0;

	if (pos > (DGPOT_STEPS - 1))
		pos = (DGPOT_STEPS - 1);


	diff = pos - dgpot.pos[cs];

	if (diff == 0)
		return pos;

	DCC_LOG(LOG_TRACE, "wait...");
	thinkos_flag_wait(dgpot.flag);
	DCC_LOG(LOG_TRACE, "done.");
	__thinkos_flag_clr(dgpot.flag);

	DCC_LOG2(LOG_TRACE, "cs=%d pos=%d", cs, pos);

	if (diff < 0) {
		/* decrement */
		diff = -diff;
		/* set the repetition counter register */
		tim->rcr = (diff * 2) - 2;
		/* force initial UP/DOWN level to LOW */
		tim->ccmr1 = TIM_OC1M_FORCE_INACTIVE;
	} else {
		/* increment */
		/* set the repetition counter register */
		tim->rcr = (diff * 2) - 1;
		/* force initial UP/DOWN level to HIGH */
		tim->ccmr1 = TIM_OC1M_FORCE_ACTIVE;
	}

	tim->ccmr1 = TIM_OC1M_TOGGLE | TIM_OC1PE;

	/* Generate an update flag to start */
	tim->egr = TIM_UG;

	tim->cnt = dgpot.lcur;

	if (cs == 1)
		stm32f_gpio_clr(DGPOT_NCS1);
	else
		stm32f_gpio_clr(DGPOT_NCS0);

	tim->cr1 = TIM_CMS_EDGE | TIM_DIR_DOWN | TIM_OPM | TIM_URS | TIM_CEN; 

	dgpot.pos[cs] = pos;

	return pos;
}

void dgpot_init(void)
{
	int i;

	DCC_LOG(LOG_TRACE, "...");

	stm32f_gpio_mode(DGPOT_NCS0, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(DGPOT_NCS1, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(DGPOT_UPDWN, ALT_FUNC, PUSH_PULL | SPEED_HIGH);

	stm32f_gpio_set(DGPOT_NCS0);
	stm32f_gpio_set(DGPOT_NCS1);

	dgpot.flag = thinkos_flag_alloc();
	DCC_LOG1(LOG_TRACE, "flag=%d", dgpot.flag);

	dgpot_timer_init();

	for (i = 0; i < DGPOT_CHIPS; ++i) {
		/* artificially set the potentiometer to the 
		   maximum level to allow zeroing */
		dgpot.pos[i] = DGPOT_STEPS - 1;
		dgpot_set(i, 0);
	}
}

