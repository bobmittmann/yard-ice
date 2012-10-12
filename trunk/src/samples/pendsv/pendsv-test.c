/* 
 * File:	 pendsv-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2012 Bob Mittmann. All Rights Reserved.
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
#include <stdio.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>

#ifndef CM3_SYSTICK_CLK_HZ
#error "CM3_SYSTICK_CLK_HZ undefined"
#endif

#define __SVC_CALL(N) asm("svc " #N "\n")
#define __SVC(N) __SVC_CALL(N)

void cm3_pendsv_isr(void)
{
	printf("[PendSV]");
}

void cm3_svc_handler(void)
{
	struct cm3_scb * scb = CM3_SCB; 
	printf("[entry]");

	udelay(20000);
	scb->icsr = SCB_ICSR_PENDSVSET;
	udelay(20000);

	printf("[exit]");
}

void cm3_systick_isr(void)
{
	unsigned int irq;

	/* get irq id */
	irq = (CM3_SCB->icsr & 0xff) - 15;

	printf("%d", irq);
}

void cm3_systick_init(void)
{
	struct cm3_systick * systick = CM3_SYSTICK;

	/* 100ms reload */
	systick->load = (CM3_SYSTICK_CLK_HZ / 10) - 1;
	systick->val = 0;
	systick->ctrl = SYSTICK_CTRL_ENABLE | SYSTICK_CTRL_TICKINT;
}

int main(int argc, char ** argv)
{
	cm3_udelay_calibrate();

	stdout = stm32f_usart_open(STM32F_UART5, 115200, SERIAL_8N1);

	printf("\n");
	printf("-------------------------------------------------------------------------\n");
	printf("- PendSV Test\n");
	printf("-------------------------------------------------------------------------\n");
	printf("\n");

	cm3_systick_init();

	cm3_except_pri_set(CM3_EXCEPT_PENDSV, 32);
	cm3_except_pri_set(CM3_EXCEPT_SVC, 0);
	cm3_except_pri_set(CM3_EXCEPT_SYSTICK, 64);
	
	cm3_basepri_set(64 + 32);

	for (;;) {
		udelay(1000000);
		__SVC(1);
	}

	return 0;
}

