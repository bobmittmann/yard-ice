/* 
 * File:	 comm-test.c
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

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ice-comm.h>

FILE * stdout = (FILE *)&ice_comm_file;
FILE * stdin = (FILE *)&ice_comm_file;
FILE * stderr = (FILE *)&ice_comm_file;

volatile uint32_t clock;

void SysTick_Handler(void)
{
	clock++;
}

void cm3_systick_init(void)
{
	struct cm3_systick * systick = CM3_SYSTICK;

	/* 1ms reload */
	systick->load = CM3_SYSTICK_CLK_HZ / 1000;
	systick->val = 0;
	systick->ctrl = SYSTICK_CTRL_ENABLE | SYSTICK_CTRL_TICKINT;
}

int ice_io_comm_write(struct ice_comm_blk * comm, const void * buf, 
					   unsigned int len);

int main(int argc, char ** argv)
{
//	int i = 0;
	char buf[256];
	int len;
	int cnt;
	char c;
	uint32_t t0;
	uint32_t t1;
	uint32_t dt;
	int sent;
	int i;

	cm3_udelay_calibrate();

	cm3_systick_init();

	printf("1.\n");
	delay(1);
	printf("2.\n");
	delay(1);
	printf("3.\n");

	/* fill the buffer */
	c = '0';
	for (i = 0; i < 256; i++) {
		buf[i] = c++;
		if (c > 'z')
			c = '0';
	}

	cnt = 100000;
	len = 128;
	
	t0 = clock;
	for (i = 0; i < cnt; i += len) {
		ice_io_comm_write(&ice_comm_blk, buf, len);
	}
	t1 = clock;

	sent = i * len;
	dt = t1 - t0;
	printf("\n%d bytes, %d ms = %d bytes/sec\n", sent, dt, (sent * 1000) / dt);

	for (;;);
}

