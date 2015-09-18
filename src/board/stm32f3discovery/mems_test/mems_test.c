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
		{ LED9_IO }, /* LED9 */
		{ LED7_IO }, /* LED7 */
		{ LED5_IO }, /* LED5 */
		{ LED3_IO }, /* LED3 */
		{ LED4_IO }, /* LED4 */
		{ LED6_IO }, /* LED6 */
		{ LED8_IO }, /* LED8 */
		{ LED10_IO }, /* LED10 */
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


static int xy_octant(struct vector * p)
{
	int t;
	int o = 0;

	if (p->y <  0)  {
		p->x = -p->x;
		p->y = -p->y;
		o += 4;
	}

	if (p->x <= 0)  {
		t = p->x;
		p->x = p->y;
		p->y = -t;
		o += 2;
	}

	if (p->x <= p->y)  {
		t = p->y - p->x;
		p->x = p->x + p->y;
		p->y = t;
		o += 1;
	}

	return o;
}

#define COS_PI_8 0.92388
#define SIN_PI_8 0.38268

void xy_rotate_pi_8(struct vector * v)
{
	int x;
	int y;
	int c;
	int s;

	c = COS_PI_8 * 32768;
	s = COS_PI_8 * 32768;

	x = c * v->x - s * v->y;
	y = s * v->x + c * v->y;

	v->x = x / 32768;
	v->y = y / 32768;
}

void compass(void)
{
    int o = 0;

    /* Initialize the magnetic sensor device */
    lsm303_mag_init();

    for (;;) {
        struct vector v;
        int i;

    	/* get the magnetic vector */
        lsm303_mag_vec_get(&v);
        /* rotate PI/8 (22.5 dg) */
        xy_rotate_pi_8(&v);
        /* get the vector's octant */
        i = xy_octant(&v);
        /* update LEDs */
        if (i != o) {
        	led_on(i);
        	led_off(o);
        	o = i;
        }
    }
}

void level(void)
{
    int o = 0;

    /* Initialize the accelerometer device */
    lsm303_acc_init();

    for (;;) {
        struct vector v;
        int i;

    	/* get the magnetic vector */
        lsm303_acc_vec_get(&v);

        printf("%5d, %5d\n", v.x, v.y);

        /* get the vector's octant */
        i = xy_octant(&v);
        /* update LEDs */
        if (i != o) {
        	led_on(i);
        	led_off(o);
        	o = i;
        }
    }
}

int main(int argc, char ** argv)
{
	int i;

	thinkos_udelay_factor(&udelay_factor);

	stdio_init();

	lsm303_init();

	level();

	return 0;
}

