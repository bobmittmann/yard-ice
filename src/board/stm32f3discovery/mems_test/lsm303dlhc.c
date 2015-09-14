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
	uint8_t lst[128];
	int n;

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	stm32_gpio_mode(LSM303_INT1, INPUT, SPEED_LOW);
	stm32_gpio_mode(LSM303_INT2, INPUT, SPEED_LOW);

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

