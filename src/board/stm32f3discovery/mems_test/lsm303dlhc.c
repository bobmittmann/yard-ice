/* 
 * File:	 rtos_basic.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/delay.h>

#include <thinkos.h>

#include "lsm303dlhc.h"
#include "i2c.h"
#include "board.h"


/* ----------------------------------------------------------------------
 * Accelerometer
 * ----------------------------------------------------------------------
 */

int lsm303_init(void)
{
	uint8_t lst[1];
	int n;

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	stm32_gpio_mode(LSM303_INT1, INPUT, SPEED_LOW);
	stm32_gpio_mode(LSM303_INT2, INPUT, SPEED_LOW);

	i2c_init();

	n = i2c_bus_scan(0x32, 0x32, lst, 1);

	if (n != 1) {
		printf("Error: LSM303DLHC not found!");
		return -1;
	}

	printf("LSM303DLHC found.");

	return 0;
}

#if 0

#define AVG_N 16

struct acc_info {
	volatile int y;
	volatile int x;
	volatile int z;
	volatile bool cal_req;
	int sem;
};

static int accelerometer_task(struct acc_info * acc)
{
	struct {
		int8_t x;
		uint8_t res1;
		int8_t y;
		uint8_t res2;
		int8_t z;
	} data;
	uint8_t cfg[4];
	uint8_t st;
	int x = 0;
	int y = 0;
	int z = 0;
	int x_off = 0;
	int y_off = 0;
	int z_off = 0;

	printf("%s(): thread %d started.\n", __func__, thinkos_thread_self());

	if (lsm303_init() < 0) {
		return -1;
	}

	cfg[0] = CTRL_PD | CTRL_ZEN | CTRL_YEN | CTRL_XEN;
	cfg[1] = 0;
	cfg[3] = 0;
	lsm303_wr(LIS302_CTRL_REG1, cfg, 3);

	for (; ;) {
		thinkos_sleep(1);
		/* poll the sensor */
		lsm303_rd(LIS302_STATUS_REG, &st, 1);

		if (st & STAT_ZYXDA) {
			/* get the forces data */
			lsm303_rd(LIS302_OUT_X, &data, 5);

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

#endif

