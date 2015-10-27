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
 * @file compass.c
 * @brief magnetometer application test
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <sys/console.h>
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

void leds_init(void)
{
	int i;

	for (i = 0; i < LED_COUNT; ++i) {
		__led_off(led_io[i].gpio, led_io[i].pin);
	}
}


/* ----------------------------------------------------------------------
 * Console
 * ----------------------------------------------------------------------
 */

void stdio_init(void)
{
	FILE * f;

	f = console_fopen();
	/* initialize STDIO */
	stderr = f;
	stdout = f;
	stdin = f;
}

/* ----------------------------------------------------------------------
 * Vector transformations
 * ----------------------------------------------------------------------
 */

/* get the octant of the vector in the XY plane */
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

/* rotate the vector in the XY plane by 22.5 degrees */
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

int32_t isqrt(uint32_t x);

int32_t mag(struct vector * v)
{
	int32_t w;

	w = v->x * v->x + v->y * v->y + v->z * v->z;
	return isqrt(w);
}

int main(int argc, char ** argv)
{
    int o = 0;

    leds_init();

    /* Initialize the console */
	stdio_init();

    /* Initialize the LSM303DLHC MEMS */
	lsm303_init();

    /* Initialize the magnetic sensor device */
    lsm303_mag_init();

    for (;;) {
        struct vector v;
        int i;

    	/* get the magnetic vector */
        lsm303_mag_vec_get(&v);

        printf("%4d,%4d,%4d %4d : ", v.x, v.y, v.z, mag(&v));
        /* rotate PI/8 (22.5 dg) */
        xy_rotate_pi_8(&v);

        printf("%4d,%4d,%4d %4d : ", v.x, v.y, v.z, mag(&v));

        /* get the vector's octant */
        i = xy_octant(&v);
        printf("%d\n", i);

        /* update LEDs */
        if (i != o) {
			/* turn on the LED on the new octant */
        	led_on(i);
			/* turn off the LED on the old octant */
        	led_off(o);
			/* save the new octant for next round */
        	o = i;
        }
    }

	return 0;
}

