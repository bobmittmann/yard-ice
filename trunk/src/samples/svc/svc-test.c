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
#include <string.h>
#include <arch/cortex-m3.h>
#include <sys/serial.h>
#include <sys/delay.h>

#include "syscalls.h"

void cm3_svc_init(uint32_t * stack_ptr);

uint32_t main_stack[128];

/* thread context */
struct cxos_context {
	/* saved context */
	void * impure;
	uint32_t r4;

	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;

	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	uint32_t exc_return;

	/* exception context */
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;

	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t xpsr;
};

struct cxos_thread {
	struct cxos_context * sp;
	uint32_t ticks;
	int8_t id;
	int8_t priority;
};

#define CXOS_THREADS_MAX 32
#define CXOS_WAIT_QUEUE_MAX 32

#define SVC_CXOS_THREAD_CREATE 1
#define SVC_CXOS_WAIT 1
#define SVC_CXOS_SIGNAL 1

struct cxos_rt {
	/* Pointer to the active (current) thread */
	struct cxos_thread * th_active;
	uint32_t paused;
	uint32_t ready;
	uint8_t pri_lut[CXOS_THREADS_MAX];
	uint8_t th_lut[CXOS_THREADS_MAX];
	uint32_t th_free;
	struct cxos_thread th[CXOS_THREADS_MAX];
	uint32_t wq_free;
	uint32_t wq[CXOS_WAIT_QUEUE_MAX];
};

/* Pointer to thread private data */
void * cxos_impure_ptr;

struct cxos_rt cxos_rt;

/* set a bit in a bit map atomically */
static void bmp_bit_set(void * bmp, unsigned int bit)
{
	/* get the bitband bit address */
	uint32_t * bit_addr = CM3_BITBAND_MEM(bmp, 0);  
	bit_addr[bit] = 1;
}

/* clear a bit in a bit map atomically */
static void bmp_bit_clr(void * bmp, unsigned int bit)
{
	/* get the bitband bit address */
	uint32_t * bit_addr = CM3_BITBAND_MEM(bmp, 0);  
	bit_addr[bit] = 1;
}

static int bmp_ffs32(void * bmp)
{
	return cm3_clz(*(uint32_t *)bmp);
}

/* flags a defered execution of the scheduller */
static void inline __attribute__((always_inline)) cxos_defer_sched(void) {
	struct cm3_scb * scb = CM3_SCB;
	/* rise a pending service interrupt */
	scb->icsr = SCB_ICSR_PENDSVSET;
}

void cxos_wq_insert(unsigned int wq_id, unsigned int th_id)
{
	bmp_bit_set(&cxos_rt.wq[wq_id], th_id);  
}

void cxos_wq_remove(unsigned int wq_id, unsigned int th_id)
{
	bmp_bit_clr(&cxos_rt.wq[wq_id], th_id);  
}

int cxos_wq_get_head(unsigned int wq_id)
{
	return bmp_ffs32(&cxos_rt.wq[wq_id]);
}

void  cxos_ready_insert(unsigned int th_id)
{
	unsigned int pri_id;
	/* get the thread schedule bit from the
	   priority lookup table */
	pri_id = cxos_rt.pri_lut[th_id];
	bmp_bit_set(&cxos_rt.ready, pri_id);  
}

void  cxos_ready_remove(unsigned int th_id)
{
	unsigned int pri_id;
	/* get the thread schedule bit from the
	   priority lookup table */
	pri_id = cxos_rt.pri_lut[th_id];
	bmp_bit_clr(&cxos_rt.ready, pri_id);  
}

void cxos_signal(unsigned int wq_id)
{
	int th_id;

	th_id = cxos_wq_get_head(wq_id);
	cxos_wq_remove(wq_id, th_id);
	cxos_ready_insert(th_id);
	cxos_defer_sched();
}

void __attribute__((noreturn, naked)) cxos_idle_task(void)
{
	for (;;) {
#if (ENABLE_CPU_HALT_WHEN_IDLE)
		asm volatile ("wfi\n"); /* wait for interrupt */
#endif
	}
}

struct cxos_context cxos_idle_stack __attribute__ ((aligned (8))) = {
	.impure = (void *)(uint8_t[]){ 'I', 'D', 'L', 'E' },
	.r4 = 4,
	.r5 = 5,
	.r6 = 6,
	.r7 = 7,
	.r8 = 8,
	.r9 = 9,
	.r10 = 10,
	.r11 = 11,
	.exc_return = 0xfffffffd,
	.r0 = 0,
	.r1 = 1,
	.r2 = 2,
	.r3 = 3,
	.r12 = 12,
	.lr = (uint32_t)cxos_idle_task,
	.pc = (uint32_t)cxos_idle_task,
	.xpsr = 0x01000000
};

struct cxos_thread idle_thread = {
	.sp = &cxos_idle_stack,
	.ticks = 0,
	.id = -1,
	.priority = -1
};

struct cxos_context * cxos_scheduler(struct cxos_context * ctx)
{
	int th_id;
	int pri_id;

	if (act) {
		cxos_ready_insert(act->id);
	}

	/* get next from the ready list */
	pri_id = bmp_ffs32(&cxos_rt.ready);
	bmp_bit_clr(&cxos_rt.ready, pri_id);  

	/* get the thread id bit from the
	   thread id lookup table */
	th_id = cxos_rt.th_lut[pri_id];
	return &cxos_rt.th[th_id];
}


void svc_handler(int svc, uint32_t * arg)
{
	uint32_t ret = -1;

	switch (svc) {
	case 0:
		ret = 0;
		break;
	case 1:
		ret = 11;
		break;
	case 2:
		ret = 22;
		break;
	case 3:
		ret = 33;
		break;
	}
	arg[0] = ret;
}

/* 256 bytes exception stack */
uint64_t cm3_except_stack[32];



int main(int argc, char ** argv)
{
	int ret;

	cm3_udelay_calibrate();
	stdout = stm32f_usart_open(STM32F_UART5, 115200, SERIAL_8N1);

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" Cortex M3 SVC Test\n");
	printf("---------------------------------------------------------\n");
	printf("\n");

	cm3_svc_init(main_stack + sizeof(main_stack) / sizeof(uint32_t));

	for (;;) {
		udelay(200000);
		ret = __SVC(0);
		printf("SVC0=%d\n", ret);

		udelay(200000);
		ret = __SVC(1);
		printf("SVC1=%d\n", ret);

		udelay(200000);

	}

	return 0;
}

