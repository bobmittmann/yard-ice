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
#include <sys/delay.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

#include <sys/dcclog.h>

#define FREQ_1MHZ 1000000
#define ISINK_DELAY_COMPENASTION 4

struct {
	uint8_t mode;
	uint32_t s1;
	uint32_t s2;
	uint32_t s3;
	uint32_t s4;
} isink_drv;

static void isink_io_cfg(unsigned int mode)
{
	int32_t s1;
	int32_t s2;
	int32_t s3;

	switch (mode) {

	case 0:
	/* (128 45)[mA] (640 225)[mV] [ VCC VCC GND ] [ GND VCC GND ] GND  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		s1 = -1;
		s2 = 0;
		s3 = 0;
		break;

	case 1:
	/* (160 45)[mA] (800 225)[mV] [ VCC VCC NC  ] [ GND VCC GND ] GND  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		break;

	case 2:
	default:
	/* (160 57)[mA] (800 285)[mV] [ VCC VCC NC  ] [ GND VCC NC  ] GND  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		s1 = -1;
		s2 = 0;
		s3 = 0;
		break;

	case 3:
	/* (168 76)[mA] (840 380)[mV] [ NC  VCC NC  ] [ GND VCC GND ] VCC  */
		stm32_gpio_mode(SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		break;

	case 4:
	/* (179 45)[mA] (895 225)[mV] [ GND VCC VCC ] [ GND VCC GND ] GND  */
		stm32_gpio_mode(SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		break;

	case 5:
	/* (204 45)[mA] (1020 225)[mV] [ NC  VCC VCC ] [ GND VCC GND ] GND  */
		stm32_gpio_mode(SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		break;

	case 6:
	/* (204 52)[mA] (1020 260)[mV] [ NC  VCC VCC ] [ NC  VCC GND ] GND  */
		stm32_gpio_mode(SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		break;

	case 7:
	/* (214 76)[mA] (1070 380)[mV] [ VCC VCC GND ] [ GND VCC GND ] VCC  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = -1;
		s2 = 0;
		s3 = 0;
		break;

	case 8:
	/* (216 82)[mA] (1080 410)[mV] [ VCC GND VCC ] [ VCC GND GND ] GND  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		break;

	case 9:
	/* (232 82)[mA] (1160 410)[mV] [ VCC NC  VCC ] [ VCC GND GND ] GND  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		break;

	case 10:
	/* (232 89)[mA] (1160 445)[mV] [ VCC NC  VCC ] [ VCC NC  GND ] GND  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		break;

	case 11:
	/* (261 45)[mA] (1305 225)[mV] [ VCC VCC VCC ] [ GND VCC GND ] GND  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		break;

	case 12:
	/* (261 82)[mA] (1305 410)[mV] [ VCC VCC VCC ] [ VCC GND GND ] GND  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		break;

	case 13:
	/* (261 128)[mA] (1305 640)[mV] [ VCC VCC VCC ] [ VCC VCC GND ] GND  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_clr(SINK4);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		break;

	case 14:
	/* (299 76)[mA] (1495 380)[mV] [ GND VCC VCC ] [ GND VCC GND ] VCC  */
		stm32_gpio_mode(SINK1, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		break;

	case 15:
	/* (324 76)[mA] (1620 380)[mV] [ VCC VCC NC  ] [ GND VCC GND ] VCC  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		break;

	case 16:
	/* (324 115)[mA] (1620 575)[mV] [ VCC VCC NC  ] [ GND VCC NC  ] VCC  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = -1;
		s2 = 0;
		s3 = 0;
		break;

	case 17:
	/* (324 138)[mA] (1620 690)[mV] [ VCC VCC NC  ] [ VCC GND GND ] VCC  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		break;

	case 18:
	/* (362 138)[mA] (1810 690)[mV] [ VCC GND VCC ] [ VCC GND GND ] VCC  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, OUTPUT, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		break;

	case 19:
	/* (379 76)[mA] (1895 380)[mV] [ NC  VCC VCC ] [ GND VCC GND ] VCC  */
		stm32_gpio_mode(SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		break;

	case 20:
	/* (379 96)[mA] (1895 480)[mV] [ NC  VCC VCC ] [ NC  VCC GND ] VCC  */
		stm32_gpio_mode(SINK1, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		break;

	case 21:
	/* (409 138)[mA] (2045 690)[mV] [ VCC NC  VCC ] [ VCC GND GND ] VCC  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		break;

	case 22:
	/* (409 156)[mA] (2045 780)[mV] [ VCC NC  VCC ] [ VCC NC  GND ] VCC  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, OPEN_DRAIN | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		break;

	case 23:
	/* (438 76)[mA] (2190 380)[mV] [ VCC VCC VCC ] [ GND VCC GND ] VCC  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = -1;
		s2 = 0;
		s3 = -1;
		break;

	case 24:
	/* (438 138)[mA] (2190 690)[mV] [ VCC VCC VCC ] [ VCC GND GND ] VCC  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = 0;
		s2 = -1;
		s3 = -1;
		break;

	case 25:
	/* (438 214)[mA] (2190 1070)[mV] [ VCC VCC VCC ] [ VCC VCC GND ] VCC  */
		stm32_gpio_mode(SINK1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_mode(SINK3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
		stm32_gpio_set(SINK4);
		s1 = 0;
		s2 = 0;
		s3 = -1;
		break;
	}

	isink_drv.s1 = s1;
	isink_drv.s2 = s2;
	isink_drv.s3 = s3;
}

void isink_pulse(unsigned int pre, unsigned int pulse)
{
	struct stm32f_tim * tim = STM32_TIM4;
	unsigned int t1 = pulse - pre;	

	tim->ccr2 = (isink_drv.s1 & t1) + 1;
	tim->ccr3 = (isink_drv.s2 & t1) + 1;
	tim->ccr4 = (isink_drv.s3 & t1) + 1;
	tim->arr = 1;
 	tim->cnt = pulse + ISINK_DELAY_COMPENASTION;
	tim->cr1 = TIM_CMS_EDGE | TIM_DIR_DOWN | TIM_OPM | TIM_URS | TIM_CEN; 
}

void isink_stop(void)
{
	struct stm32f_tim * tim = STM32_TIM4;

 	tim->cnt = 0;
}

#define SLEWRATE_DAC_VAL(X) ((4095 * ((X) - SLEWRATE_MIN)) / \
							 (SLEWRATE_MAX - SLEWRATE_MIN))

const uint16_t slewrate_dac_lut[4] = {
	[ISINK_RATE_VERY_SLOW >> 5] = SLEWRATE_DAC_VAL(100),
	[ISINK_RATE_SLOW >> 5]      = SLEWRATE_DAC_VAL(625),
	[ISINK_RATE_NORMAL >> 5]    = SLEWRATE_DAC_VAL(1250),
	[ISINK_RATE_FAST >> 5]      = SLEWRATE_DAC_VAL(2500)
};

void isink_mode_set(unsigned int mode)
{
	struct stm32f_dac * dac = STM32_DAC;

	if (isink_drv.mode == mode)
		return;
	
	isink_io_cfg(mode & 0x3f);
	dac->dhr12r2 = slewrate_dac_lut[(mode >> 6) & 0x3];
	isink_drv.mode = mode;

	DCC_LOG1(LOG_INFO, "irate DAC=%d", dac->dhr12r2);

}

void isink_sleep(void)
{
	struct stm32f_tim * tim = STM32_TIM4;

	/* stop the timer */
 	tim->cnt = 0;
	/* disable clock */
	stm32_clk_disable(STM32_RCC, STM32_CLK_TIM4);
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
	tim->arr = 1;
	tim->ccmr1 = TIM_OC1M_PWM_MODE2 | TIM_OC2M_PWM_MODE2;
	tim->ccmr2 = TIM_OC3M_PWM_MODE2 | TIM_OC4M_PWM_MODE2;
	tim->ccer = TIM_CC1E | TIM_CC2E | TIM_CC3E | TIM_CC4E;
	//	TIM_CC1P | TIM_CC2P | TIM_CC3P | TIM_CC4P;
	tim->ccr1 = 1;
	tim->ccr2 = 1;
	tim->ccr3 = 1;
	tim->ccr4 = 1;


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

	isink_drv.mode = -1;
}


#if 0

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

void isink_test(void)
{
	unsigned int pre;
	unsigned int pulse;
	int mode;

	pre = 500;
	pulse = 1000;

	/* disable interrupts */	
	cm3_cpsid_i();
	for (mode = 0; mode < 26; ++mode) {
		isink_io_cfg(mode);
		isink_pulse(pre, pulse);
		udelay(pulse + 500);
	}
	cm3_cpsie_i();

	isink_drv.mode = -1;
}

#endif

