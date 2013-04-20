/* 
 * File:	 serial-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
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
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <thinkos.h>
#include <sys/dcclog.h>

#include <yard-ice/drv.h>
#include <jtag3drv.h>

#include <bitvec.h>

#include <trace.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>

const struct file stm32f_uart_file = {
	.data = STM32F_UART5, 
	.op = &stm32f_usart_fops 
};

void stdio_init(void)
{
	stderr = (struct file *)&stm32f_uart_file;
	stdout = uart_console_fopen(uart_console_init(115200, SERIAL_8N1));
	stdin = stdout;
}

int supervisor_task(void)
{
	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

	for (;;) {
		trace_fprint(stdout, TRACE_FLUSH);
		thinkos_sleep(250);
	}
}

int tap_probe(void)
{
	uint8_t irlen[32];
	jtag_tap_t * tap;
	unsigned int cnt;
	int i;
	
	tracef(" - TAP probe...");
	jtag3ctrl_int_en(IRQ_TAP);

	jtag3ctrl_nrst(true);
	udelay(100);
	jtag3ctrl_nrst(false);

	jtag3ctrl_trst(true);
	udelay(100);
	jtag3ctrl_trst(false);

	insn_tap_reset(5, JTAG_TAP_RESET);

	if (jtag_chain_probe(irlen, 32, &cnt) != JTAG_OK) {
		tracef("jtag_chain_probe() fail!");
		DCC_LOG(LOG_ERROR, "jtag_chain_probe() fail!");
		return -1;
	}

	if (cnt == 0) {
		tracef(" - No devices detected.");
		return 0;
	}

	if (cnt == 1)
		tracef(" - 1 device in the scan path.");
	else
		tracef(" - %d devices in the scan path.", cnt);

	tracef(" - Initializing the JTAG chain...");

	/* initializing the jtag chain */
	if (jtag_chain_init(irlen, cnt)  != JTAG_OK) {
		tracef("jtag_chain_init() fail!");
		DCC_LOG(LOG_ERROR, "jtag_chain_init() fail!");
		return -1;
	}

	tracef("  | SEQ | IR LEN | ID CODE    | ");

	for (i = 0; i < cnt; i++) {
		if (jtag_tap_get(&tap, i) != JTAG_OK) {
			tracef("jtag_tap_get() fail!");
			DCC_LOG(LOG_ERROR, "jtag_tap_get() fail!");
			return -1;
		}
		tracef("  | %3d | %6d | 0x%08x |", i, tap->irlen, tap->idcode);
	}


	return 0;
}

void memset16(uint16_t * dst, uint16_t val, int cnt)
{
	int i;

	for (i = 0; i < cnt; ++i)
		dst[i] = val;
}

void memcpy16(uint16_t * dst, uint16_t * src, int cnt)
{
	int i;

	DCC_LOG3(LOG_TRACE, "dst=0x%08x dst=0x%08x cnt=%d", dst, src, cnt);

	for (i = 0; i < cnt; ++i)
		dst[i] = src[i];
}

void memrand16(uint16_t * dst, int cnt)
{
	int i;

	DCC_LOG2(LOG_TRACE, "dst=0x%08x cnt=%d", dst, cnt);

	for (i = 0; i < cnt; ++i)
		dst[i] = rand();
}

int memcmp16(uint16_t * p1, uint16_t * p2, int cnt)
{
	int i;
	uint16_t v1;
	uint16_t v2;

	DCC_LOG3(LOG_TRACE, "p1=0x%08x p2=0x%08x cnt=%d", p1, p2, cnt);

	for (i = 0; i < cnt; ++i) {
		v1 = p1[i];
		v2 = p2[i];
		if (v1 != v2) {
			DCC_LOG2(LOG_TRACE, "v1(0x%04x) != v2(0x%04x)", v1, v2);
			return i + 1;
		}
	}

	return 0;
}

void memcpy64(uint64_t * dst, uint64_t * src, int cnt)
{
	int i;

	DCC_LOG3(LOG_TRACE, "dst=0x%08x dst=0x%08x cnt=%d", dst, src, cnt);

	for (i = 0; i < cnt; ++i)
		dst[i] = src[i];
}

void memrand64(uint64_t * dst, int cnt)
{
	int i;

	DCC_LOG2(LOG_TRACE, "dst=0x%08x cnt=%d", dst, cnt);

	for (i = 0; i < cnt; ++i)
		dst[i] = ((uint64_t)rand() << 32) + 0x4000000000000000LL +
			rand() + 0x40000000;
}

int memcmp64(uint64_t * v1, uint64_t * v2, int cnt)
{
	int i;

	DCC_LOG3(LOG_TRACE, "v1=0x%08x v2=0x%08x cnt=%d", v1, v2, cnt);

	for (i = 0; i < cnt; ++i) {
		if (v1[i] != v2[i])
			return i + 1;
	}

	return 0;
}

bool jtag_mem_test(struct jtag_io * jtag)
{
	uint64_t buf[512];
	volatile uint16_t * ptr = (uint16_t*)0x60004000;
	int len = 512;
	int ret;
	int i;

	printf("%s():..\n", __func__);

	DCC_LOG2(LOG_TRACE, "jtagt=0x%08x len=%d", jtag, len);

	for (;;) {
		ptr[0] = 0x8001;
		ptr[1] = 0x4002;
		ptr[2] = 0x2004;
		ptr[3] = 0x1008;
		tracef("%04x %04x %04x %04x", ptr[0], ptr[1], ptr[2], ptr[3]);
		thinkos_sleep(200);
	}

	for (i = 0; i < 100; ++i) {
		len = 128;
		memrand16((uint16_t *)buf, len);
		memcpy16(jtag->desc.h, (uint16_t *)buf, len);
		if ((ret = memcmp16(jtag->desc.h, (uint16_t *)buf, len)) != 0) {
			printf("%s(): 16bits error @ %d!\n", __func__, ret);
			return false;
		}

		len = 32;
		memrand64(buf, len);
		memcpy64(jtag->desc.d, buf, len);
		if ((ret = memcmp64(jtag->desc.d, buf, len)) != 0) {
			printf("%s(): 64bits error @ %d!\n", __func__, ret);
			return false;
		}
	}

	return true;
}

bool jtag_loop_test(void)
{
	uint32_t vin[32];
	uint32_t vout[32];
	char s1[33 * 4];
	char s2[33 * 4];
	int len = 32 * 4;
	int i;

	jtag_tck_freq_set(100000);

	/* enable TAP interrupts */
	jtag3ctrl_int_en(IRQ_TAP);

	/* clear interrupts */
	jtag3ctrl_int_clr(IRQ_TAP);

	jtag3ctrl_loopback(1);

	for (i = 1; i > 0; --i) {
		vec_rand(vin, len);

		trace("ir_scan()");
		jtag_ir_scan(vin, vout, len, JTAG_TAP_IDLE);
		trace("done()");

		//	vec_rshift(vout, vout, len, 1);

		if (vec_cmp(vin, vout, len - 1) != 0) {
			tracef(" vin=%s", vec_fmt(s1, vin, len - 1));
			tracef("vout=%s", vec_fmt(s2, vout, len - 1));
		}

	}

	return true;
}

bool jtag_irscan_test(void)
{
	uint32_t vin[32];
	uint32_t vout[32];
	char s1[33 * 4];
	char s2[33 * 4];
	int len = 32;
	int i;

	tracef("%s()...", __func__);

	jtag_tck_freq_set(100000);
	/* enable TAP interrupts */
	jtag3ctrl_int_en(IRQ_TAP);
	/* clear interrupts */
	jtag3ctrl_int_clr(IRQ_TAP);

	jtag3ctrl_nrst(true);
	udelay(100);
	jtag3ctrl_nrst(false);

	jtag3ctrl_trst(true);
	jtag_run_test(1, JTAG_TAP_IDLE);
	jtag3ctrl_trst(false);

	jtag_tap_reset();

	for (i = 1; i > 0; --i) {
//		vec_zeros(vin, len);
		vec_ones(vin, len);
		trace("ir_scan()");
//		jtag_ir_scan(vin, vout, len, JTAG_TAP_IRUPDATE);
		jtag_ir_scan(vin, vout, len, JTAG_TAP_IDLE);
//		jtag_dr_scan(vin, vout, len, JTAG_TAP_IDLE);
		trace("done()");
		tracef(" vin=%s", vec_fmt(s1, vin, len - 1));
		tracef("vout=%s", vec_fmt(s2, vout, len - 1));
	}

	return true;
}


void jtag_reg_test(struct jtag_io * jtag)
{
	uint16_t int_en;
	uint16_t r3;
	uint16_t div;
	uint16_t rtdiv;
	uint16_t val;
	int j;

	printf("- Registers test...");

	for (j = 1000; j > 0; --j) {
		int_en = rand() & 0x0003;
		r3 = rand() & 0xffff;
		div = rand() & 0xffff;
		rtdiv = rand() & 0xffff;

		jtag->int_en = int_en;
		jtag->r3 = r3;
		jtag->div = div;
		jtag->rtdiv = rtdiv;

		if ((val = jtag->int_en) != int_en) {
			printf("INT_EN: 0x%04x != 0x%04x\n", val, int_en);
			break;
		}

		if ((val = jtag->r3) != r3) {
			printf("    R3: 0x%04x != 0x%04x\n", val, r3);
			break;
		}

		if ((val = jtag->div) != div) {
			printf("   DIV: 0x%04x != 0x%04x\n", val, div);
			break;
		}

		if ((val = jtag->rtdiv) != rtdiv) {
			printf("  RTDIV: 0x%04x != 0x%04x\n", val, rtdiv);
			break;
		}
	}

	if (j == 0)
		printf(" OK.\n");
}

void jtag_irq_init(void)
{
	struct stm32f_exti * exti = STM32F_EXTI;

	/* Clear EXTI pending flag */
	exti->pr = (1 << 6);
	/* Clear Cortex Interrupt Pending */
	cm3_irq_pend_clr(STM32F_IRQ_EXTI9_5);
}

void jtag_irq_disable(void)
{
	cm3_irq_disable(STM32F_IRQ_EXTI9_5);
}

void jtag_irq_enable(void)
{
	cm3_irq_enable(STM32F_IRQ_EXTI9_5);
}

void __stm32f_exti9_5_isr(void)
{
	struct stm32f_exti * exti = STM32F_EXTI;

	trace("IRQ...");

	/* Clear pending flag */
	exti->pr = (1 << 6);
//	reg_wr(REG_INT_ST, IRQ_TMR); /* clear interrupt */
}

void jtag_irq_test(void)
{
	int j;

	tracef("- Interrupt test...");

	jtag3ctrl_int_en(IRQ_TMR);

	for (j = 5; j > 0; --j) {
		trace("wait...");
		reg_wr(REG_TMR, 100);
		jtag3drv_int_wait(IRQ_TMR);
		trace("wakeup...");
    	/* Clear pending flag */
	}

	jtag3ctrl_int_en(0);
}


uint32_t supervisor_stack[256];

int main(int argc, char ** argv)
{
//	struct jtag_io * jtag = JTAG_IO;
	int i = 0;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	cm3_udelay_calibrate();
	trace_init();
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));
	stdio_init();

	thinkos_thread_create((void *)supervisor_task, (void *)NULL,
						  supervisor_stack, sizeof(supervisor_stack), 
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(8));

	printf("\n\n");
	printf("-----------------------------------------\n");
	printf(" JTAGTOOL3 FPGA test\n");
	printf("-----------------------------------------\n");
	printf("\n");

	stm32f_dac_init();
	stm32f_dac_vout_set(3300);

	if (jtag_drv_init() != 0) {

	}

	jtag_tck_freq_set(100000);
	jtag3ctrl_int_clr(0xffff);
	jtag3ctrl_int_en(0);

	jtag_irq_init();

//	jtag_irq_test();

//	if (jtag_loop_test() == false) {
//		for(;;);
//	}


//	jtag_reg_test(jtag);

//	if (jtag_mem_test(jtag) == false) {
//		for(;;);
//	}
//	printf("JTAG memory Ok.\n");
//	jtag_irq_test();
		
	for (i = 0; ; i++) {
		getchar();
//		jtag_loop_test();
		tap_probe();
//		jtag_irscan_test();
	}

	return 0;
}

