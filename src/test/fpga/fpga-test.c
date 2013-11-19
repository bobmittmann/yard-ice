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

#define UART_TX STM32F_GPIOC, 12
#define UART_RX STM32F_GPIOD, 2

void stdio_init(void)
{
	struct stm32f_usart * uart = STM32F_UART5;

	stm32f_gpio_clock_en(STM32F_GPIOC);
	stm32f_gpio_clock_en(STM32F_GPIOD);
	stm32f_gpio_mode(UART_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(UART_RX, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(UART_RX, GPIO_AF8);
	stm32f_gpio_af(UART_TX, GPIO_AF8);

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 115200);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	stderr = (struct file *)&stm32f_uart_file;
	stdout = stderr;
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

void memcpy32(uint32_t * dst, uint32_t * src, int cnt)
{
	uint32_t * end = dst + cnt;

	DCC_LOG3(LOG_INFO, "dst=0x%08x dst=0x%08x cnt=%d", dst, src, cnt);
	while (dst != end)
		*dst++ = *src++;
}

void memrand32(uint32_t * dst, int cnt)
{
	int i;

	DCC_LOG2(LOG_INFO, "dst=0x%08x cnt=%d", dst, cnt);
	for (i = 0; i < cnt; ++i)
		dst[i] = rand() + (rand() << 31);
}

int memcmp32(uint32_t * v1, uint32_t * v2, int cnt)
{
	int i;

	DCC_LOG3(LOG_INFO, "v1=0x%08x v2=0x%08x cnt=%d", v1, v2, cnt);
	for (i = 0; i < cnt; ++i) {
		if (v1[i] != v2[i])
			return i + 1;
	}

	return 0;
}

bool jtag_mem_test(void)
{
	uint32_t in[128];
	uint32_t out[128];
	int i;

	for (i = 32; i > 0; --i) {
		memrand32(in, 128);
		memcpy32((uint32_t *)JTAGDRV_AUX, in, 128);
		memcpy32(out, JTAGDRV_AUX, 128);

		if (memcmp32(in, out, 128) != 0) {
			tracef("memory test failed!");
			return false;
		}
	}

	tracef("memory test ok.");

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

int test_ir_scan(unsigned int desc, unsigned int ptr, 
				 unsigned int tx_addr, unsigned int rx_addr,
				 const jtag_vec_t vin, jtag_vec_t vout, 
				 int vlen, unsigned int final_state)
{
	unsigned int isr;

	DCC_LOG2(LOG_TRACE, "len=%d state=%s", vlen, jtag_state_name[final_state]);

	/* create a descriptor table with one entry */
	desc_wr(desc, JTAG_DESC(rx_addr, 0, tx_addr, 0, vlen));
	/* create a pointer to the descriptor */
	ptr_wr(ptr, JTAG_PTR(desc, 1));

	/* write the vector in the controller's memory */
	jtag3ctrl_vec_wr(tx_addr, (uint16_t *)vin, vlen);

	/* scan the vector */
	reg_wr(REG_INSN, INSN_IR_SCAN(ptr, final_state));
	isr = jtag3drv_int_wait(IRQ_TAP);

	if ((isr & IRQ_TAP) == 0) {
		DCC_LOG1(LOG_WARNING, "isr:0x%02x", isr);
		return -1;
	}

	DCC_LOG(LOG_INFO, ".");

	if (vout != NULL)
		jtag3ctrl_vec_rd(rx_addr, (uint16_t *)vout, vlen);

	return 0;
}

bool my_test(void)
{
	uint32_t vin[32];
	uint32_t vout[32];
	char s1[33 * 4];
	char s2[33 * 4];
	unsigned int desc;
	unsigned int ptr;
	unsigned int tx_addr;
	unsigned int rx_addr;
	unsigned int len = 32 * 4;
	int i;

	jtag_tck_freq_set(100000);

	/* enable TAP interrupts */
	jtag3ctrl_int_en(IRQ_TAP);

	/* clear interrupts */
	jtag3ctrl_int_clr(IRQ_TAP);

	jtag3ctrl_loopback(1);

	for (i = 1; i > 0; --i) {
//		len = rand() & 0x7f;
		len = 15;
		desc = rand() & 0xff;
		ptr = rand() & 0xff;
		rx_addr = (rand() % (0x400 - len)) & ~1;
		tx_addr = (rand() % (0x400 - len)) & ~1;
//		rx_addr = 0;
//		tx_addr = 128 + 64 + 32;

		/* create a descriptor table with one entry */
//		desc_wr(desc, JTAG_DESC(rx_addr, 0, tx_addr, 0, len));
		tracef("desc addr=%d,%d", desc * 4, reg_rd(REG_MEM_WR_ADDR));
//		break;

		vec_rand(vin, len);

		tracef("ir_scan(dsc=%d ptr=%d tx=%d rx=%d len=%d)",
			   desc, ptr, tx_addr, rx_addr, len);

		test_ir_scan(desc, ptr, tx_addr, rx_addr, 
					 vin, vout, len, JTAG_TAP_IDLE);
		
		tracef("desc =0x%04x%04x", reg_rd(REG_DESC_HI), reg_rd(REG_DESC_LO));

		if (vec_cmp(vin, vout, len) != 0) {
			tracef("tx=%d rx=%d", reg_rd(REG_MEM_WR_ADDR), 
				   reg_rd(REG_MEM_RD_ADDR));
			tracef("fail: vin=%s", vec_fmt(s1, vin, len));
			tracef("     vout=%s", vec_fmt(s2, vout, len));
			break;
		}

		trace("ok.");
	}

	return true;
}

bool jtag_desc_test(void)
{
	uint32_t vin[32];
	uint32_t vout[32];
	char s1[33 * 4];
	char s2[33 * 4];
	unsigned int desc;
	unsigned int ptr;
	unsigned int tx_addr;
	unsigned int rx_addr;
	unsigned int len = 32 * 4;
	int i;

	jtag_tck_freq_set(100000);

	/* enable TAP interrupts */
	jtag3ctrl_int_en(IRQ_TAP);

	/* clear interrupts */
	jtag3ctrl_int_clr(IRQ_TAP);

	jtag3ctrl_loopback(1);

	for (i = 1; i > 0; --i) {
		len = rand() & 0x7f;
		desc = rand() & 0xff;
		ptr = rand() & 0xff;
		tx_addr = (rand() % (0x080 - len)) & ~1;
		rx_addr = (rand() % (0x080 - len)) & ~1;
		tx_addr = 2;
		rx_addr = 2;

		vec_rand(vin, len);

		tracef("ir_scan(dsc=%d ptr=%d tx=%d rx=%d len=%d)",
			   desc, ptr, tx_addr, rx_addr, len);
		test_ir_scan(desc, ptr, tx_addr, rx_addr, 
					 vin, vout, len, JTAG_TAP_IDLE);

		//	vec_rshift(vout, vout, len, 1);

		if (vec_cmp(vin, vout, len) != 0) {
			tracef("tx=%d rx=%d", reg_rd(REG_MEM_WR_ADDR), 
				   reg_rd(REG_MEM_RD_ADDR));
			tracef("fail: vin=%s", vec_fmt(s1, vin, len));
			tracef("     vout=%s", vec_fmt(s2, vout, len));
			break;
		}

		trace("ok.");
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

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();
	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));

	trace_init();

	stdio_init();

/*
	thinkos_thread_create((void *)supervisor_task, (void *)NULL,
						  supervisor_stack, sizeof(supervisor_stack), 
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(8));
*/

	printf("\n\n");
	printf("-----------------------------------------\n");
	printf(" JTAGTOOL3 FPGA test\n");
	printf("-----------------------------------------\n");
	printf("\n");

//	stm32f_dac_init();
//	stm32f_dac_vout_set(3300);

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
		
	jtag_mem_test();

	for (i = 0; ; i++) {
		getchar();
//		jtag_loop_test();
//		jtag_desc_test();
		my_test();
//		tap_probe();
//		jtag_irscan_test();
	}

	return 0;
}

