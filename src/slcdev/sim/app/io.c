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
#include "slcdev.h"
#include "io.h"

#define LED_COUNT 4

struct {
	uint8_t led_tmr[LED_COUNT];
	uint16_t usr_tmr[4];
} io_drv;

const struct {
	struct stm32_gpio * gpio;
	int pin;
} led_io[LED_COUNT] = {
	{ IO_LED1 },
	{ IO_LED2 },
	{ IO_LED3 },
	{ IO_LED4 }
};

void timer_set(unsigned int id, unsigned int ms)
{
	io_drv.usr_tmr[id] = (ms + IO_POLL_PERIOD_MS - 1) / IO_POLL_PERIOD_MS;
#if 0
	if (ms == 0) {
		/* clear possibly pending event */
		slcdev_event_clear(SLC_EV_TMR1 + id);
	}
#endif
}

unsigned int timer_get(unsigned int id)
{
	return io_drv.usr_tmr[id] * IO_POLL_PERIOD_MS;

}

void led_on(unsigned int id)
{
	__led_on(led_io[id].gpio, led_io[id].pin);
}

void led_off(unsigned int id)
{
	__led_off(led_io[id].gpio, led_io[id].pin);
}

bool led_status(unsigned int id)
{
	return __is_led_on(led_io[id].gpio, led_io[id].pin);
}

void led_flash(unsigned int id, unsigned int ms)
{
	io_drv.led_tmr[id] = ms / IO_POLL_PERIOD_MS;
	__led_on(led_io[id].gpio, led_io[id].pin);
}

int32_t clk_time = 0;

#define SW1_OFF (0 << 0)
#define SW1_A   (1 << 0)
#define SW1_B   (2 << 0)
#define SW1_MSK (3 << 0)

#define SW2_OFF (0 << 2)
#define SW2_A   (1 << 2)
#define SW2_B   (2 << 2)
#define SW2_MSK (3 << 2)

void __attribute__((noreturn)) io_event_task(void)
{
	struct stm32_gpio * gpiob = STM32_GPIOB;
	unsigned int sw0 = 0;
	unsigned int sw1 = 0;
	uint32_t clk = thinkos_clock();
	uint32_t clk_sec = clk + 1000;

	for (;;) {
		unsigned int sw;
		unsigned int d;
		uint32_t pb;
		int i;

		clk += IO_POLL_PERIOD_MS;
		thinkos_alarm(clk);

		/* update clock time */
		if ((int32_t)(clk - clk_sec) >= 1000) {
			clk_time++;
			clk_sec += 1000;
		}

		/* process led timers */
		for (i = 0; i < LED_COUNT; ++i) {
			if (io_drv.led_tmr[i] == 0)
				continue;
			if (--io_drv.led_tmr[i] == 0) 
				__led_off(led_io[i].gpio, led_io[i].pin);
		}

		/* process user timers */
		if (io_drv.usr_tmr[0]) {
			if (--io_drv.usr_tmr[0] == 0) 
				thinkos_ev_raise(SLCDEV_DRV_EV, SLC_EV_TMR1);
		}
		if (io_drv.usr_tmr[1]) {
			if (--io_drv.usr_tmr[1] == 0) 
				thinkos_ev_raise(SLCDEV_DRV_EV, SLC_EV_TMR2);
		}
		if (io_drv.usr_tmr[2]) {
			if (--io_drv.usr_tmr[2] == 0) 
				thinkos_ev_raise(SLCDEV_DRV_EV, SLC_EV_TMR3);
		}
		if (io_drv.usr_tmr[3]) {
			if (--io_drv.usr_tmr[3] == 0) 
				thinkos_ev_raise(SLCDEV_DRV_EV, SLC_EV_TMR4);
		}


		pb = gpiob->idr; 

		/* Lever switches */
		sw = (~pb >> 12) & 0xf; 
		if (sw != sw0) {
			/* Debouncing */
			sw0 = sw;
		} if ((d = sw ^ sw1) != 0) {
			/* State change */
			sw1 = sw;

			DCC_LOG1(LOG_INFO, "SW=%d", sw);

			if (d & SW1_MSK) {
				switch (sw & SW1_MSK) {
				case SW1_OFF:
					DCC_LOG(LOG_TRACE, "SW1 OFF");
					thinkos_ev_raise(SLCDEV_DRV_EV, SLC_EV_SW1_OFF);
					break;

				case SW1_A:
					DCC_LOG(LOG_TRACE, "SW1 A");
					thinkos_ev_raise(SLCDEV_DRV_EV, SLC_EV_SW1_DOWN);
					break;

				case SW1_B:
					DCC_LOG(LOG_TRACE, "SW1 B");
					thinkos_ev_raise(SLCDEV_DRV_EV, SLC_EV_SW1_UP);
					break;
				}
			}

			if (d & SW2_MSK) {
				switch (sw & SW2_MSK) {
				case SW2_OFF:
					DCC_LOG(LOG_TRACE, "SW2 OFF");
					thinkos_ev_raise(SLCDEV_DRV_EV, SLC_EV_SW2_OFF);
					break;

				case SW2_A:
					DCC_LOG(LOG_TRACE, "SW2 A");
					thinkos_ev_raise(SLCDEV_DRV_EV, SLC_EV_SW2_DOWN);
					break;

				case SW2_B:
					DCC_LOG(LOG_TRACE, "SW2 B");
					thinkos_ev_raise(SLCDEV_DRV_EV, SLC_EV_SW2_UP);
					break;
				}
			}
		}
	}
}

#ifndef ENABLE_SAVE_LED_STATE
#define ENABLE_SAVE_LED_STATE 0
#endif

void lamp_test(void)
{
#if ENABLE_SAVE_LED_STATE
	uint32_t state = 0;
	int i;

	/* save LED state */
	for (i = 0; i < LED_COUNT; ++i) {
		state = __is_led_on(led_io[i].gpio, led_io[i].pin) ? (1 << i) : 0;
		__led_off(led_io[i].gpio, led_io[i].pin);
	}
#endif

	led_flash(0, 64);
	led_flash(1, 64);
	thinkos_sleep(166);
	led_flash(2, 64);
	led_flash(3, 64);
	thinkos_sleep(166);
	led_flash(4, 64);
	led_flash(5, 64);
	thinkos_sleep(166);

#if ENABLE_SAVE_LED_STATE
	/* restore LEDs state */
	for (i = 0; i < LED_COUNT; ++i) {
		if (state & (1 << i))
			__led_on(led_io[i].gpio, led_io[i].pin);
	}
#endif
}

