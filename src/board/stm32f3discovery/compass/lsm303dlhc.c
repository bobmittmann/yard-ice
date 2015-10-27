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
 * Magnetometer
 * ----------------------------------------------------------------------
 */

#define MAG_AVG_N 8
#define FIX_ENDIAN(X) (int16_t)((((X) << 8) | ((X) >> 8)) & 0xffff)

struct mag_dev {
	volatile struct vector v;
	int mutex;
	int sem;
};

void magnetometer_task(struct mag_dev * mag)
{
	struct stm32f_exti * exti = STM32F_EXTI;
	struct {
		int16_t x;
		int16_t z;
		int16_t y;
	} data;
	struct vector v;
	int cnt = 0;
	int x0;
	int y0;
	int z0;

    v.x = 0;
    v.y = 0;
    v.z = 0;

	for (; ;) {
		/* wait for DRDY interrupt */
		thinkos_irq_wait(STM32F_IRQ_EXTI2);
	    /* clear interrupt flag */
	    exti->pr = LSM303_DRDY_EXTI;
		/* read the sensor */
		lsm303_mag_rd(LSM303_OUT_X_H_M, &data, 6);

		/* Fix byte order: the sensor output the high byte first then the low byte,
		 * this result in a wrong byte order for little endian machines (Cortex-M) */
		x0 = FIX_ENDIAN(data.x);
		y0 = FIX_ENDIAN(data.y);
		z0 = FIX_ENDIAN(data.z);

		/* Filter */
		v.x = (v.x * (MAG_AVG_N - 1) / MAG_AVG_N) + x0;
		v.y = (v.y * (MAG_AVG_N - 1) / MAG_AVG_N) + y0;
		v.z = (v.z * (MAG_AVG_N - 1) / MAG_AVG_N) + z0;

		if ((++cnt % (MAG_AVG_N / 2)) == 0) {
			thinkos_mutex_lock(mag->mutex);
			mag->v = v;
			thinkos_mutex_unlock(mag->mutex);
			thinkos_sem_post(mag->sem);
		}
	}
}

uint32_t magnetometer_stack[64];

const struct thinkos_thread_inf magnetometer_inf = {
    .stack_ptr = magnetometer_stack,
    .stack_size = sizeof(magnetometer_stack),
    .priority = 1,
    .thread_id = 1,
    .paused = false,
    .tag = "MAG"
};

struct mag_dev magdev;

void lsm303_mag_vec_get(struct vector * v)
{
	/* Wait for data from magnetic sensor device */
    thinkos_sem_wait(magdev.sem);
    /* Read data with exclusive acccess */
    thinkos_mutex_lock(magdev.mutex);
    *v = magdev.v;
    thinkos_mutex_unlock(magdev.mutex);
}

void lsm303_mag_init(void)
{
	struct stm32f_exti * exti = STM32F_EXTI;
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;
	uint8_t cfg[3];
	uint32_t tmp;

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);
	stm32_clk_enable(STM32_RCC, STM32_CLK_SYSCFG);

	stm32_gpio_mode(LSM303_DRDY_GIO, INPUT, SPEED_LOW);

	/* Select PE as EXTI2 input */
	tmp = syscfg->exticr[0];
	tmp &= ~SYSCFG_EXTI2_MSK;
	tmp |= SYSCFG_EXTI2_PE;
	syscfg->exticr[0] = tmp;

	/* set falling edge trigger */
	exti->ftsr |= LSM303_DRDY_EXTI;
	/* clear rising edge trigger */
	exti->rtsr &= ~LSM303_DRDY_EXTI;
	/* Clear pending flag */
	exti->pr = LSM303_DRDY_EXTI;
	/* unmask interrupt */
	exti->imr |= LSM303_DRDY_EXTI;

	cfg[0] = CRA_TEMP_EN | CRA_DO_30_HZ;
	cfg[1] = CRB_GN_8_1; /* ±8.1 Gauss */
	cfg[2] = MR_MD_CONTINOUS;
	lsm303_mag_wr(LSM303_CRA_REG_M, cfg, 3);

    magdev.sem = thinkos_sem_alloc(0);
    magdev.mutex = thinkos_mutex_alloc();

    thinkos_thread_create_inf((void *)magnetometer_task, (void *)&magdev,
                              &magnetometer_inf);
}

/* ----------------------------------------------------------------------
 * Accelerometer
 * ----------------------------------------------------------------------
 */

int lsm303_init(void)
{
	uint8_t lst[128];
	int n;

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	stm32_gpio_mode(LSM303_INT1_GIO, INPUT, SPEED_LOW);
	stm32_gpio_mode(LSM303_INT2_GIO, INPUT, SPEED_LOW);

	i2c_init();

again:
	n = i2c_bus_scan(0x19, 0x1e, lst, 6);

	if (n < 2) {
		printf("Error: LSM303DLHC not found!\n");
		thinkos_sleep(500);
		goto again;
		return -1;
	}

	printf("LSM303DLHC found.\n");

	return 0;
}

int lsm303_acc_wr(unsigned int reg, const void * buf, unsigned int len)
{
	return i2c_reg_write(LSM303_ACC_ADDR, reg, buf, len);
}

int lsm303_acc_rd(unsigned int reg, void * buf, unsigned int len)
{
	return i2c_reg_read(LSM303_ACC_ADDR, reg, buf, len);
}

int lsm303_mag_wr(unsigned int reg, const void * buf, unsigned int len)
{
	return i2c_reg_write(LSM303_MAG_ADDR, reg, buf, len);
}

int lsm303_mag_rd(unsigned int reg, void * buf, unsigned int len)
{
	return i2c_reg_read(LSM303_MAG_ADDR, reg, buf, len);
}

