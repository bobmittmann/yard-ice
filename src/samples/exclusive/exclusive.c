/* 
 * File:	 dac-test.c
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
#include <stdio.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>

#ifndef CM3_SYSTICK_CLK_HZ
#error "CM3_SYSTICK_CLK_HZ undefined"
#endif

static uint32_t inline __bit_mem_rdex(void * ptr, int32_t bit) {
	/* get the bitband bit address */
	uint32_t * bit_addr = CM3_BITBAND_MEM(ptr, 0);
	return __ldrexw(&bit_addr[bit]);
}

/* write a bit into a bitbang memory region */
static uint32_t inline __bit_mem_wrex(void * ptr, int32_t bit, uint32_t val) {
	/* get the bitband bit address */
	uint32_t * bit_addr = CM3_BITBAND_MEM(ptr, 0);
	return __strexw(&bit_addr[bit], val);
}

void cm3_systick_init(uint32_t freq)
{
	struct cm3_systick * systick = CM3_SYSTICK;

	systick->load = (CM3_SYSTICK_CLK_HZ / freq) - 1;
	systick->val = 0;
	systick->ctrl = SYSTICK_CTRL_ENABLE;
}

static inline void __strw(uint32_t * addr, uint32_t val) {
	asm volatile ("str %0, [%1]" : : "r" (addr), "r" (val));
}

uint32_t lock[8192];
uint32_t bmp[8192];

int main(int argc, char ** argv)
{
	uint32_t val;
	uint32_t ret;

	cm3_udelay_calibrate();
	stdout = stm32f_usart_open(STM32F_UART5, 115200, SERIAL_8N1);
	cm3_systick_init(100);

	printf("\n");
	printf("------------------------------------------------------------\n");
	printf("Exclusive access test\n");
	printf("------------------------------------------------------------\n");
	printf("\n");

	__bit_mem_wr(lock, 0, 1);

	printf(" data=0x%08x bit[0]=%d\n", lock[0], __bit_mem_rd(lock, 0));

	val = __ldrexw(lock);
	printf(" __ldrexw(lock) = 0x%08x\n", val);
	__clrx();
	printf(" __clrx()\n");

	ret = __strexw(lock, 3);
	printf(" __strexw(lock) %s\n", ret ? "failed!" : "ok.");

	ret = __bit_mem_wrex(lock, 0, 1);
	printf(" __bit_mem_wr(lock) %s\n", ret ? "failed!" : "ok.");

	val = __bit_mem_rdex(lock, 0);
	printf(" __bit_mem_rdex(lock) = %d\n", val);

//	val = __ldrexw(lock);
//	printf(" __ldrexw(lock) = 0x%08x\n", val);

	__strw(lock, 7);
	printf(" __strw(lock)\n");

	ret = __strexw(bmp, 7);
	printf(" __strexw(bmp) %s\n", ret ? "failed!" : "ok.");

	ret = __bit_mem_wrex(lock, 0, 1);
	printf(" __bit_mem_wr(lock) %s\n", ret ? "failed!" : "ok.");

	ret = __strexw(lock, 7);
	printf(" __strexw(lock) %s\n", ret ? "failed!" : "ok.");

	val = __ldrexw(lock);
	printf(" __ldrexw(lock) = 0x%08x\n", val);

	ret = __strexw(lock, 8);
	printf(" __strexw(lock) %s\n", ret ? "failed!" : "ok.");

	__clrx();

	delay(10);
	return 0;
}

