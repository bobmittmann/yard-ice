/* 
 * File:	 spi-test.c
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


#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/stm32f.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <thinkos.h>
#include <hexdump.h>

#include <yard-ice/drv.h>
#include <sys/dcclog.h>

int altera_configure(const uint8_t * buf, int len);

struct fpga_io { 
	union {
		uint16_t h[2048];
		uint32_t w[1024];
		uint64_t d[512];
	};
	uint32_t res1[0x2000 - 1024];
	uint16_t mem[256];
	uint32_t res2[0x4000 - 0x2000 + 128];
	union {
		volatile uint16_t reg[8];
		volatile uint32_t r32[4];
		volatile uint64_t r64[2];
		struct {
			volatile uint16_t ist;
			volatile uint16_t ien;

			volatile uint16_t cnt;
			volatile uint16_t dwn;

			volatile uint16_t src;
			volatile uint16_t dst;
			volatile uint16_t len;
			volatile uint16_t ctl;

		};
	};
};

static inline void __strd(void * addr, uint64_t value) {
	asm volatile ("strd %0, %1, [%2]" : : "r" (value), 
				  "r" (value >> 32), "r" (addr));
}

static inline uint64_t __ldrd(void * addr) {
	register uint32_t rl;
	register uint32_t rh;
	asm volatile ("ldrd %0, %1, [%2]" : "=r" (rl), "=r" (rh) : "r" (addr));
	return ((uint64_t)rh << 32) + rl;
}

static inline void __ldqd(void * addr, uint32_t r[]) {
	register uint32_t r0;
	register uint32_t r1;
	register uint32_t r2;
	register uint32_t r3;
	asm volatile ("ldmia %4, {%0, %1, %2, %3}" : "=r" (r0), "=r" (r1),
				  "=r" (r2), "=r" (r3): "r" (addr));
	r[0] = r0;
	r[1] = r1;
	r[2] = r2;
	r[3] = r3;
}

void __move(uint16_t * dst, uint16_t * src, unsigned int len) 
{
	struct stm32f_dma * dma = STM32F_DMA2;
	struct stm32f_rcc * rcc = STM32F_RCC;

	/* DMA clock enable */
	rcc->ahb1enr |= RCC_DMA2EN;
	/* DMA Disable */
	dma->s[0].cr &= ~DMA_EN;	
	/* Wait for the channel to be ready .. */
	while (dma->s[0].cr & DMA_EN); 

	/* Source address */
	dma->s[0].par = (void *)src;
	/* Destination address */
	dma->s[0].m0ar = (void *)dst;
	/* Number of data items to transfer */
	dma->s[0].ndtr = 4;
	dma->s[0].fcr = DMA_DMDIS | DMA_FTH_FULL;
	/* Configuration single buffer */
	dma->s[0].cr = DMA_CHSEL_SET(1) | 
		DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_32 | DMA_PSIZE_32 | 
		DMA_MINC | DMA_PINC | 
		DMA_DIR_MTM | DMA_EN;

	/* wait for the DMA transfer to complete */
	while ((dma->lisr & DMA_TCIF0) == 0);
	/* clear the DMA transfer complete flag */
	dma->lifcr = DMA_CTCIF0;
}


int fpga_init(struct fpga_io * fpga, const void * rbf, int size) 
{
	struct stm32f_exti * exti = STM32F_EXTI;
	int ret; 

	/* Enable clock output */
	stm32f_mco2_init();

	/* Configure memory controller ... */
	stm32f_fsmc_init();

	stm32f_fsmc_speed(1);

	/* Configure external interrupt ... */
	stm32f_exti_init(STM32F_GPIOD, 6, EXTI_EDGE_RISING);

	if ((ret = altera_configure(rbf, size)) < 0) {
		printf(" # altera_configure() failed: %d!\n", ret);
		for(;;);
	};

	printf("- FPGA configuration done.\n");

	/* wait for the FPGA initialize */
	thinkos_sleep(20);

	DCC_LOG(LOG_TRACE, "...");

	/* Disable interrupts */
	fpga->ien = 0x0000;

	/* Clear FPGA's pending interrupts */
	fpga->ist = 0xffff;

	/* Clear EXTI pending flag */
	exti->pr = (1 << 6);

	/* Clear Cortex Interrupt Pending */
	cm3_irq_pend_clr(STM32F_IRQ_EXTI9_5);

	return 0;
}


void __stm32f_exti9_5_isr(void)
{
	struct fpga_io * fpga =  (struct fpga_io *)STM32F_FSMC_NE1;
	struct stm32f_exti * exti = STM32F_EXTI;
	unsigned int st;
	unsigned int en;

	/* Clear pending flag */
	exti->pr = (1 << 6);

	/* Clear interrupt flag */
	st = fpga->ist;
	(void)st;
	en = fpga->ien;
	(void)en;
	DCC_LOG2(LOG_TRACE, "IRQ: st=0x%02x en=0x%02x", st, en);

	fpga->ist = 0x01;
	/* Clear interrupt flag */
//	fpga->ist = st & en;
//	DCC_LOG1(LOG_TRACE, "IRQ: st=0x%02x", fpga->ist);
}

void fpga_irq_disable(void)
{
	cm3_irq_disable(STM32F_IRQ_EXTI9_5);
}

void fpga_irq_enable(void)
{
	cm3_irq_enable(STM32F_IRQ_EXTI9_5);
}

void fpga_irq_wait(struct fpga_io * fpga, unsigned int mask) 
{
	struct stm32f_exti * exti = STM32F_EXTI;
	int st;

	while (((st = fpga->ist) & mask) == 0) {
		DCC_LOG2(LOG_TRACE, "IRQ: st=0x%02x en=0x%02x", st, fpga->ien);
		thinkos_irq_wait(STM32F_IRQ_EXTI9_5);
	}
	/* Clear pending flag */
	exti->pr = (1 << 6);
	/* Clear interrupt flag */
	fpga->ist = st;
}

gpio_io_t irq_io = GPIO(GPIOD, 6);

void fill_up_64(struct fpga_io * fpga, uint64_t * buf, int len)
{
	int i;

	for (i = 0; i < len; ++i)
		buf[i] = ((uint64_t)rand() << 32) + 0x4000000000000000LL +
			rand() + 0x40000000;
	for (i = 0; i < len; ++i)
		fpga->d[i] = buf[i];
}

void fill_down_64(struct fpga_io * fpga, uint64_t * buf, int len)
{
	int i;

	for (i = 0; i < len; ++i)
		buf[i] = ((uint64_t)rand() << 32) + 0x4000000000000000LL +
			rand() + 0x40000000;
	for (i = len - 1; i >= 0; i--)
		fpga->d[i] = buf[i];
}


bool cmp_64(struct fpga_io * fpga, uint64_t * buf, int len)
{
	uint64_t val;
	int i;

	for (i = 0; i < len; ++i) {
//		val = __ldrd((void *)&fpga->d[i]);
		val = fpga->d[i];
		if (buf[i] != val)
			return false;
	}
	return true;
}

bool cmp_32(struct fpga_io * fpga, uint64_t * buf, int len)
{
	uint64_t val;
	int i;

	for (i = 0; i < len * 2; i += 2) {
		val = (uint64_t)(fpga->w[i]) + ((uint64_t)(fpga->w[i + 1]) << 32);
		if (buf[i / 2] != val)
			return false;
	}

	return true;
}

bool cmp_16(struct fpga_io * fpga, uint64_t * buf, int len)
{
	uint64_t val;
	int i;

	for (i = 0; i < len  * 4; i += 4) {
		val = (uint64_t)fpga->h[i] + 
			((uint64_t)fpga->h[i + 1] << 16) +
			((uint64_t)fpga->h[i + 2] << 32) +
			((uint64_t)fpga->h[i + 3] << 48);
		if (buf[i / 4] != val)
			return false;
	}

	return true;
}

void io_test(struct fpga_io * fpga)
{
	uint32_t val;
	int i = 0;
	int r;
	int c;
	
	c = getchar();
	while (c != '\033') {
		r = c - '0';

		fpga->reg[r] = i;
		thinkos_sleep(100);
		val = fpga->reg[r];
		printf("%2d - r[%d]-> %d 0x%08x\n", i, r, val, fpga->r32[r >> 1]);
		i++;

		c = getchar();
	}
}

void reg_test(struct fpga_io * fpga)
{
	uint32_t st;
	int r = 0;
	int i = 0;
	int c;
	
	printf("Register read test:\n");

	c = getchar();
	while (c != '\033') {
		if (c <= '9') {
			r = c - '0';
			st = fpga->reg[r];
		} else if (c <= 'Z') {
			r = c - 'A';
			st = __ldrd((void *)&fpga->r64[r]);
//			__move((uint16_t *)buf, (uint16_t *)&fpga->r32[r], 16);
		} else if (c <= 'z') {
			r = c - 'a';
			st = fpga->r32[r];
		} else {
			st = 0;
		}
		printf("%2d - reg[%d] = 0x%04x\n", i, r, st);
		i++;
		c = getchar();
	}
}

void registers_test(struct fpga_io * fpga)
{
	uint16_t src;
	uint16_t dst;
	uint16_t len;
	uint16_t ctl;
	uint16_t ien;
	uint16_t val;
	int j;

	printf("- Registers test...");

	for (j = 1000; j > 0; --j) {
		src = rand() & 0x00ff;
		dst = rand() & 0x00ff;
		len = rand() & 0x00ff;
		ctl = rand() & 0xffff;
		ien = rand() & 0x0003;
		fpga->src = src;
		fpga->dst = dst;
		fpga->len = len;
		fpga->ctl = ctl;
		fpga->ien = ien;

		if ((val = fpga->src) != src) {
			printf("SRC: 0x%04x != 0x%04x\n", val, src);
			break;
		}
		if ((val = fpga->dst) != dst) {
			printf("DST: 0x%04x != 0x%04x\n", val, dst);
			break;
		}
		if ((val = fpga->len) != len) {
			printf("LEN: 0x%04x != 0x%04x\n", val, len);
			break;
		}
		if ((val = fpga->ctl) != ctl) {
			printf("CTL: 0x%04x != 0x%04x\n", val, ctl);
			break;
		}
		if ((val = fpga->ien) != ien) {
			printf("IEN: 0x%04x != 0x%04x\n", val, ien);
			break;
		}
	}

	if (j == 0)
		printf(" OK.\n");
}

bool memcpy_test(struct fpga_io * fpga)
{
	uint16_t buf_in[256];
	uint16_t buf_out[256];
	unsigned int src;
	unsigned int dst;
	unsigned int len;
	int size = 256;
	int i = 0;
	int j;

	printf("- Memory copy test ");

	DCC_LOG(LOG_TRACE, "Enabling memcpy interrupts...");
	fpga->ien = 0x01;
	 	
	for (j = 0; j < 1000; ++j) {

		printf(".");

		for (i = 0; i < size; ++i) {
			/* fill in the input buffer, with random nonzero values */
			while ((buf_in[i] = rand()) == 0);
			/* zero the output buffer */
			buf_out[i] = 0;
		}

		for (i = 0; i < size; ++i) {
			/* copy input to FPGA write only memory */
			fpga->mem[i] = buf_in[i];
		}

		len = rand() & 0x7f;
		src = rand() % (size - len);
		dst = rand() % (size - len);

		/* copy from write mem to read mem */
		fpga->src = src;
		fpga->dst = dst;
		fpga->len = len;
		fpga->ctl = 1;

		fpga_irq_wait(fpga, 0x01); 

		/* read from FPGA */
		for (i = 0; i < size; ++i)
			buf_out[i] = fpga->mem[i];

		/* compare */
		for (i = 0; i < len; ++i) {
			if (buf_out[dst + i] != buf_in[src + i]) {
				printf("\nError @ %d: 0x%04x != 0x%04x\n", 
					   i, buf_out[dst + i], buf_in[src + i]);
				printf("      0x%02x->0x%02x,%-3d\n", src, dst, len);
				return false;
			}
		}
	}

	printf(" OK.\n");

	return true;
}

void slow_test(struct fpga_io * fpga)
{
	int i;
	uint64_t v64;
	uint32_t count;

	for (i = 0; i < 64; i += 4) {
		v64 = i + ((i  + 1) << 16) + 
			((uint64_t)(i  + 2) << 32) + ((uint64_t)(i  + 3) << 48);
		fpga->d[i / 4] = v64;
		printf("%016llx\n", v64);
	}

	printf("\n");

	for (i = 0; i < 64; i += 4) {
//		fpga->src = 222;
		fpga->cnt = 100 + i;
		thinkos_sleep(1000);
		count = fpga->cnt;
		v64 = fpga->d[i / 4];
		printf("%016llx %d\n", v64, count);
	}
}

void count_test(struct fpga_io * fpga)
{
	uint16_t cnt;
	uint16_t dwn;
	uint32_t u32;
	int diff;
	int i;

//	DCC_LOG(LOG_TRACE, "Enabling timer interrupts...");
//	fpga->ien = 2;

	printf("  Up: ");

	for (i = 0; i < 4; ++i) {
		cnt = fpga->cnt;
		thinkos_sleep(100);
		diff = fpga->cnt - cnt;
		printf("%4d ", diff);
	}

	printf("\nDown: ");
	for (i = 0; i < 4; ++i) {
		cnt = fpga->dwn;
		thinkos_sleep(100);
		diff = fpga->dwn - cnt;
		printf("%4d ", diff);
	}

	printf("\nDiff: ");

	for (i = 0; i < 4; ++i) {
		u32 = fpga->r32[1];
		cnt = u32;
		dwn = u32 >> 16;
		printf("%4d ", (cnt + dwn) & 0xffff);
		thinkos_sleep(100);
	}

	printf("\n");
}

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

int main(int argc, char ** argv)
{
	struct fpga_io * fpga =  (struct fpga_io *)STM32F_FSMC_NE1;
	uint8_t * rbf = (uint8_t *)0x08040000;
	uint64_t val;
	uint64_t buf[512];
	int i;
	int n = 512;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "3. stdio_init()");
	stdio_init();

	printf("\n");
	printf("------------------------------------------------------\n");
	printf("- FSMC test\n");
	printf("------------------------------------------------------\n");
	printf("\n");
	printf("\r\n");

	stm32f_dac_init();
	stm32f_dac_vout_set(3300);

	fpga_init(fpga, rbf, 50000);

	val = 0;

	count_test(fpga);

	registers_test(fpga);

//	memcpy_test(fpga);

	io_test(fpga);

	slow_test(fpga);

	for (;;) {
		printf("- Write 64bits ascending\n");
		fill_up_64(fpga, buf, n);
		udelay(50000);

		printf("- Read 64bits... ");
		if (cmp_64(fpga, buf, n) == false) {
			printf("fail!\n");
		} else {
			printf("ok.\n");
		}
		delay(1);

		printf("- Write 64bits descending\n");
		fill_down_64(fpga, buf, n);
		udelay(50000);
		printf("- Read 64bits... ");
		if (cmp_64(fpga, buf, n) == false) {
			printf("fail!\n");
		} else {
			printf("ok.\n");
		}
		delay(1);

		printf("- Write 64bits\n");
		fill_up_64(fpga, buf, n);
		udelay(50000);
		printf("- Read 32bits... ");
		if (cmp_32(fpga, buf, n) == false) {
			printf("fail!\n");
		} else {
			printf("ok.\n");
		}
		delay(1);

		printf("- Write 64bits\n");
		fill_up_64(fpga, buf, n);
		udelay(50000);
		printf("- Read 16bits... ");
		if (cmp_16(fpga, buf, n) == false) {
			printf("fail!\n");
		} else {
			printf("ok.\n");
		}
		delay(1);

		printf("- Write 16bits\n");
		for (i = 0; i < n; ++i)
			buf[i] = ((uint64_t)rand() << 32) + rand();
		for (i = 0; i < n * 4; i += 4) {
			val = buf[i / 4];
			fpga->h[i] = val;
			fpga->h[i + 1] = val >> 16;
			fpga->h[i + 2] = val >> 32;
			fpga->h[i + 3] = val >> 48;
		}
		udelay(50000);
		printf("- Read 64bits... ");
		if (cmp_64(fpga, buf, n) == false) {
			printf("fail!\n");
		} else {
			printf("ok.\n");
		}
		delay(1);

		printf("- Write 32bits\n");
		for (i = 0; i < n; ++i)
			buf[i] = ((uint64_t)rand() << 32) + rand();
		for (i = 0; i < n * 2; i += 2) {
			val = buf[i / 2];
			fpga->w[i] = val;
			fpga->w[i + 1] = val >> 32;
		}
		udelay(50000);
		printf("- Read 64bits... ");
		if (cmp_64(fpga, buf, n) == false) {
			printf("fail!\n");
		} else {
			printf("ok.\n");
		}
		delay(1);
	}

	for (;;) {
		for (i = 0; i < 16; ++i) {
			fpga->w[i] = (1 << i) + (1 << (31 - i));
		}

		for (i = 0; i < 8; i ++) {
			val = fpga->w[i];
			printf(" %08x", (int)val);
		}

		printf("\n");

		for (i = 8; i < 16; i ++) {
			val = fpga->w[i];
			printf(" %08x", (int)val);
		}

		printf("\n");
		udelay(2500000);
	}

	for (;;) {
		delay(2);
		printf("1.8V");
		val = fpga->h[0];
		fpga->h[10] = val;
		fpga->h[11] = val;

		delay(2);
		printf("2.5V");
		val = fpga->h[1];
		fpga->h[11] = val;

		delay(2);
		printf("3.0V");
		val = fpga->h[1];
		fpga->h[12] = val;

		delay(2);
		printf("3.3V");
		val = fpga->h[1];
		fpga->h[13] = val;
	}

	return 0;
}

