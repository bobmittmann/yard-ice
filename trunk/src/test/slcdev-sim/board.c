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
#include <arch/cortex-m3.h>
#include <sys/dcclog.h>
#include <thinkos.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>

struct io_drv io_drv;

static inline void __led_on(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_mode_af(__gpio, __pin);
}

static inline void __led_off(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_mode_out(__gpio, __pin);
}

static inline bool __is_led_on(struct stm32_gpio *__gpio, int __pin) {
	return stm32_gpio_is_mode_af(__gpio, __pin);
}

const struct {
	struct stm32_gpio * gpio;
	int pin;
} led_io[] = {
	{ LED1 },
	{ LED2 },
	{ LED3 },
	{ LED4 },
	{ LED5 },
	{ LED6 }
};

#define IO_POLL_PERIOD_MS 16

void led_on(unsigned int id)
{
	__led_on(led_io[id].gpio, led_io[id].pin);
}

void led_off(unsigned int id)
{
	__led_off(led_io[id].gpio, led_io[id].pin);
}



void led_flash(unsigned int id, unsigned int ms)
{
	io_drv.led_tmr[id] = ms / IO_POLL_PERIOD_MS;
	__led_on(led_io[id].gpio, led_io[id].pin);
}

const uint8_t addr_sw_lut[] = {
/*  00, 01, 02, 03, 04, 05, 06, 07, 08, 09, 0a, 0b, 0c, 0d,  e, 0f */
	 0, 10, 40, 50, 20, 30, 60, 70, 80, 90,  0,  0,  0,  0,  0, 0,
/*	10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 1a, 1b, 1c, 1d, 1e, 1f */
	 1, 11, 41, 51, 21, 31, 61, 71, 81, 91,  0,  0,  0,  0,  0, 0,
/*	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 2a, 2b, 2c, 2d, 2e, 2f */
	 4, 14, 44, 54, 24, 34, 64, 74, 84, 94,  0,  0,  0,  0,  0, 0,
/*	30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 3a, 3b, 3c, 3d, 3e, 3f */
	 5, 15, 45, 55, 25, 35, 65, 75, 85, 95,  0,  0,  0,  0,  0, 0,
/*	40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 4a, 4b, 4c, 4d, 4e, 4f */
	 2, 12, 42, 52, 22, 32, 62, 72, 82, 92,  0,  0,  0,  0,  0, 0,
/*	50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 5a, 5b, 5c, 5d, 5e, 5f */
	 3, 13, 53, 53, 23, 33, 63, 73, 83, 93,  0,  0,  0,  0,  0, 0,
/*	60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 6a, 6b, 6c, 6d, 6e, 6f */
	 6, 16, 56, 56, 26, 36, 66, 76, 86, 96,  0,  0,  0,  0,  0, 0,
/*	70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 7a, 7b, 7c, 7d, 7e, 7f */
	 7, 17, 47, 57, 27, 37, 67, 77, 87, 97,  0,  0,  0,  0,  0, 0,
/*	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 8a, 8b, 8c, 8d, 8e, 8f */
	 8, 18, 48, 58, 28, 38, 68, 78, 88, 98,  0,  0,  0,  0,  0, 0,
/*	90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 9a, 9b, 9c, 9d, 9e, 9f */
	 9, 19, 92, 59, 29, 39, 69, 79, 89, 99,  0,  0,  0,  0,  0, 0
};


void stm32_tim9_isr(void)
{
	struct stm32f_tim * tim = STM32_TIM9;
	struct stm32_gpio * gpioa = STM32_GPIOA;
	struct stm32_gpio * gpiob = STM32_GPIOB;
	struct stm32_gpio * gpioc = STM32_GPIOC;
	unsigned int addr;
	unsigned int sw;
	unsigned int d;
	uint32_t pa;
	uint32_t pb;
	uint32_t pc;
	unsigned int mod;
	int i;

	/* Clear interrupt flags */
	tim->sr = 0;

	pa = gpioa->idr; 
	pb = gpiob->idr; 
	pc = gpioc->idr; 

	/* Rotatory switches decoder */
	addr = addr_sw_lut[((~pa & (0x1f << 8)) | (~pc & (0x7 << 13))) >> 8];
	/* Sensor/Module Switch */
	mod = (pb & (1 << 4)) ? 0 : 0x80;
	addr |= mod;

	if (addr != io_drv.addr_prev) {
		/* Debouncing */
		io_drv.addr_prev = addr;
	} else if (addr != io_drv.addr) {
		/* State change */
		io_drv.addr = addr;
		io_event_set(EV_ADDR);  
		DCC_LOG1(LOG_INFO, "Addr=%d", addr);
		__thinkos_flag_signal(io_drv.flag);
	}

	/* Lever switches */
	sw = (~pb >> 12) & 0xf; 

	if (sw != io_drv.sw_prev) {
		/* Debouncing */
		io_drv.sw_prev = sw;
	} if ((d = sw ^ io_drv.sw) != 0) {
		/* State change */
		io_drv.sw = sw;

		DCC_LOG1(LOG_INFO, "SW=%d", sw);

		if ((d & SW1_MSK) != 0)
			io_event_set(EV_SW1);  

		if ((d & SW2_MSK) != 0)
			io_event_set(EV_SW2);  

		if ((d & (SW2_MSK | SW1_MSK)) != 0)
			__thinkos_flag_signal(io_drv.flag);
	}

//	DCC_LOG(LOG_TRACE, "TMR");

	/* process led timers */
	for (i = 0; i < 6; ++i) {
		if (io_drv.led_tmr[i] == 0)
			continue;
		if (--io_drv.led_tmr[i] == 0) 
			__led_off(led_io[i].gpio, led_io[i].pin);
	}
}

static void io_timer_init(void)
{
	struct stm32f_tim * tim = STM32_TIM9;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = ((stm32f_tim1_hz * IO_POLL_PERIOD_MS) + 500) / 1000;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div * 2 + pre) / (2 * pre);

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM9);
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	cm3_irq_pri_set(STM32_IRQ_TIM9, IRQ_PRIORITY_LOW);
	/* Enable interrupt */
	cm3_irq_enable(STM32_IRQ_TIM9);

	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */
}

#define PWM_FREQ 100000


void io_leds_init(void)
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

#ifndef DEBUG
	/* JTAG TRST Pin */
//	stm32_gpio_mode(MODSW, INPUT, SPEED_LOW);
#endif

	/* USART2_TX */
	stm32_gpio_mode(USART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(USART2_TX, GPIO_AF7);
	/* USART2_RX */
	stm32_gpio_mode(USART2_RX, ALT_FUNC, 0);
	stm32_gpio_af(USART2_RX, GPIO_AF7);

	io_timer_init();
	io_leds_init();

	io_drv.flag = thinkos_flag_alloc();
}

uint32_t io_event_wait(void)
{
	uint32_t event;

//	tracef("%s(): wait...", __func__);

	do {
		thinkos_flag_wait(io_drv.flag);
		thinkos_flag_clr(io_drv.flag);
		event = io_drv.event;
	} while (event == 0);

	return event;
}

void lamp_test(void)
{
	uint32_t state = 0;
	int i;

	for (i = 0; i < 6; ++i) {
		state = __is_led_on(led_io[i].gpio, led_io[i].pin) ? (1 << i) : 0;
		__led_off(led_io[i].gpio, led_io[i].pin);
	}

	led_flash(0, 64);
	led_flash(1, 64);
	thinkos_sleep(100);
	led_flash(2, 64);
	led_flash(3, 64);
	thinkos_sleep(100);
	led_flash(4, 64);
	led_flash(5, 64);
	thinkos_sleep(100);

	for (i = 0; i < 6; ++i) {
		if (state & (1 << i))
			__led_on(led_io[i].gpio, led_io[i].pin);
	}
}

void system_reset(void)
{
	DCC_LOG(LOG_TRACE, "...");
    CM3_SCB->aircr =  SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
	for(;;);
}

