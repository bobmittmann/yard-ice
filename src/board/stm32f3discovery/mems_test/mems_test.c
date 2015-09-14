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
 * @file hello.c
 * @brief application test
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <sys/console.h>
#include <sys/delay.h>
#include <stdio.h>
#include <thinkos.h>

#include "lsm303dlhc.h"
#include "board.h"

/* GPIO pin description */ 
struct stm32f_io {
	struct stm32_gpio * gpio;
	uint8_t pin;
};

/* ----------------------------------------------------------------------
 * LEDs 
 * ----------------------------------------------------------------------
 */

const struct stm32f_io led_io[] = {
	{ LED3_IO }, /* LED3 */
	{ LED5_IO }, /* LED5 */
	{ LED7_IO }, /* LED7 */
	{ LED9_IO }, /* LED9 */
	{ LED10_IO }, /* LED10 */
	{ LED8_IO }, /* LED8 */
	{ LED6_IO }, /* LED6 */
	{ LED4_IO }, /* LED4 */
};

#define LED_COUNT (sizeof(led_io) / sizeof(struct stm32f_io))

void led_on(unsigned int id)
{
	__led_on(led_io[id].gpio, led_io[id].pin);
}

void led_off(unsigned int id)
{
	__led_off(led_io[id].gpio, led_io[id].pin);
}

void led_toggle(unsigned int id)
{
	__led_toggle(led_io[id].gpio, led_io[id].pin);
}

void stdio_init(void)
{
	FILE * f;

	f = console_fopen();
	/* initialize STDIO */
	stderr = f;
	stdout = f;
	stdin = f;
}

#define AVG_N 16

struct acc_info {
	volatile int y;
	volatile int x;
	volatile int z;
	volatile bool cal_req;
	int sem;
};

void accelerometer_task(struct acc_info * acc)
{
	struct {
		int16_t x;
		int16_t y;
		int16_t z;
	} data;
	uint8_t cfg[6];
	uint8_t st;
	int x = 0;
	int y = 0;
	int z = 0;
	int x_off = 0;
	int y_off = 0;
	int z_off = 0;

	printf("%s(): thread %d started.\n", __func__, thinkos_thread_self());

	cfg[0] = ODR_100HZ | CTRL_ZEN | CTRL_YEN | CTRL_XEN;
	cfg[1] = 0;
	cfg[2] = 0;
	cfg[3] = 0;
	cfg[4] = 0;
	cfg[5] = 0;
	lsm303_acc_wr(LSM303_CTRL_REG1_A, cfg, 6);

	for (; ;) {
		thinkos_sleep(10);

		/* poll the sensor */
		lsm303_acc_rd(LSM303_STATUS_REG_A, &st, 1);

		if ((st & STAT_ZYXDA) ||
			((st & (STAT_ZDA | STAT_YDA | STAT_XDA)) ==
					(STAT_ZDA | STAT_YDA | STAT_XDA))) {
			/* get the forces data */
			lsm303_acc_rd(LSM303_OUT_X_L_A, &data, 6);

			/* Filter */
			x = (x * (AVG_N - 1) / AVG_N) + data.x;
			y = (y * (AVG_N - 1) / AVG_N) + data.y;
			z = (z * (AVG_N - 1) / AVG_N) + data.z;

			if (acc->cal_req) {
				x_off = -x;
				y_off = -y;
				z_off = -z;
				acc->cal_req = false;
			}

			acc->x = x_off + x;
			acc->y = y_off + y;
			acc->z = z_off + z;

			thinkos_sem_post(acc->sem);
		}
	}
}

uint32_t accelerometer_stack[128];

const struct thinkos_thread_inf supervisor_inf = {
    .stack_ptr = accelerometer_stack,
    .stack_size = sizeof(accelerometer_stack),
    .priority = 1,
    .thread_id = 1,
    .paused = false,
    .tag = "ACC"
};

void accelerometer(void)
{
	struct acc_info acc;
    uint32_t cnt = 0;
    int dt;
    int x;
    int y;
    int i;

    acc.sem = thinkos_sem_alloc(0);

    thinkos_thread_create_inf((void *)accelerometer_task, (void *)&acc,
                              &supervisor_inf);

    for (i = 0; ; ++i) {
        thinkos_sem_wait(acc.sem);
#if 0
        if (btn_event_get() == BTN_PRESSED) {
            /* request calibration */
            acc.cal_req = true;
        }
#endif
        /* Scale */
        x = acc.x * 64 / 512;
        y = acc.y * 64 / 512;

        if ((++cnt & 0x03) == 0) {
            printf("%5d,%5d\n", x, y);
        }

        if (x == 0) {
            led_on(1);
            led_on(3);
        } else if (x < 0) {
//            led_blink(3, -x);
            led_off(1);
            led_on(3);
        } else {
//            led_blink(1, x);
            led_off(3);
            led_on(1);
        }

        if (y == 0) {
            led_on(0);
            led_on(2);
        } else if (y < 0) {
            led_off(0);
            led_on(2);
//            led_blink(2, -y);
        } else {
            led_off(2);
//            led_blink(0, y);
            led_on(0);
        }
    }

}


int main(int argc, char ** argv)
{
	int i;

	thinkos_udelay_factor(&udelay_factor);

	stdio_init();

	lsm303_init();

	accelerometer();

	for (i = 0; ; ++i) {
		led_off((i - 2) & 0x7);
		led_on(i & 0x7);
		/* wait 100 ms */
		thinkos_sleep(100);
	}

	return 0;
}

