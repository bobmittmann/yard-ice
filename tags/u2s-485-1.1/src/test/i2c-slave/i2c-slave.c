/* 
 * File:	 i2c-slave.c
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
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

#include <sys/dcclog.h>

/* GPIO pin description */ 
struct stm32f_io {
	struct stm32f_gpio * gpio;
	uint8_t pin;
};

/* ----------------------------------------------------------------------
 * LEDs 
 * ----------------------------------------------------------------------
 */

#ifdef STM32F1X
const struct stm32f_io led_io[] = {
	{ STM32F_GPIOB, 14 },
	{ STM32F_GPIOB, 15 },
	{ STM32F_GPIOC, 13 },
	{ STM32F_GPIOC, 14 },
	{ STM32F_GPIOC, 15 }
};
#endif

#ifdef STM32F4X
const struct stm32f_io led_io[] = {
	{ STM32F_GPIOC, 1 },
	{ STM32F_GPIOC, 14 },
	{ STM32F_GPIOC, 7 },
	{ STM32F_GPIOC, 8 }
};
#endif

void led_on(int id)
{
	stm32f_gpio_set(led_io[id].gpio, led_io[id].pin);
}

void led_off(int id)
{
	stm32f_gpio_clr(led_io[id].gpio, led_io[id].pin);
}

void leds_init(void)
{
	int i;

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i) {
		stm32f_gpio_mode(led_io[i].gpio, led_io[i].pin,
						 OUTPUT, PUSH_PULL | SPEED_LOW);

		stm32f_gpio_clr(led_io[i].gpio, led_io[i].pin);
	}
}


/* ----------------------------------------------------------------------
 * Console 
 * ----------------------------------------------------------------------
 */

#define USART1_TX STM32F_GPIOB, 6
#define USART1_RX STM32F_GPIOB, 7

struct file stm32f_uart1_file = {
	.data = STM32F_USART1, 
	.op = &stm32f_usart_fops 
};

void stdio_init(void)
{
	struct stm32f_usart * uart = STM32F_USART1;
#if defined(STM32F1X)
	struct stm32f_afio * afio = STM32F_AFIO;
#endif

	DCC_LOG(LOG_TRACE, "...");

	/* USART1_TX */
	stm32f_gpio_mode(USART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);

#if defined(STM32F1X)
	/* USART1_RX */
	stm32f_gpio_mode(USART1_RX, INPUT, PULL_UP);
	/* Use alternate pins for USART1 */
	afio->mapr |= AFIO_USART1_REMAP;
#elif defined(STM32F4X)
	stm32f_gpio_mode(USART1_RX, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(USART1_RX, GPIO_AF7);
	stm32f_gpio_af(USART1_TX, GPIO_AF7);
#endif

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 115200);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	stdin = &stm32f_uart1_file;
	stdout = &stm32f_uart1_file;
	stderr = &stm32f_uart1_file;
}

/* ----------------------------------------------------------------------
 * I/O 
 * ----------------------------------------------------------------------
 */
#define TLV320RST STM32F_GPIOB, 10
#define TLV320CLK STM32F_GPIOA, 8

#define I2C1_SCL STM32F_GPIOB, 8
#define I2C1_SDA STM32F_GPIOB, 9

void io_init(void)
{
#if defined(STM32F1X)
	struct stm32f_rcc * rcc = STM32F_RCC;
#endif

	DCC_LOG(LOG_MSG, "Configuring GPIO pins...");

	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOC);

#if defined(STM32F1X)
	/* Enable Alternate Functions IO clock */
	rcc->apb2enr |= RCC_AFIOEN;
#endif

	stm32f_gpio_mode(TLV320CLK, ALT_FUNC, PUSH_PULL | SPEED_HIGH);

	stm32f_gpio_mode(TLV320RST, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_clr(TLV320RST);
	udelay(1000);
	stm32f_gpio_set(TLV320RST);
}

/* ----------------------------------------------------------------------
 * I2C
 * ----------------------------------------------------------------------
 */

struct i2c_io_blk {
	/* index to the memory position to be transfered next */
	uint32_t idx;
	/* top index for the I/O memory block */
	uint32_t max;
	/* transfer count */
	uint32_t cnt;
	/* transfer status/direction */
	int xfer;
	/* reference to the I/O memory block */
	void * mem;
	int32_t event;
};

enum {
	I2C_XFER_ERR = -1,
	I2C_XFER_IN,
	I2C_XFER_OUT
};

struct i2c_io_blk i2c_io;

struct i2c_io_blk * i2c_slave_init(unsigned int scl_freq, 
								   unsigned int addr,
								   void * mem, unsigned int size)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	struct stm32f_rcc * rcc = STM32F_RCC;
	uint32_t pclk = stm32f_apb1_hz;
	struct i2c_io_blk * io = &i2c_io;
#if defined(STM32F1X)
	struct stm32f_afio * afio = STM32F_AFIO;
#endif

	io->mem = mem;
	io->max = size - 1;
	io->idx = 0;
	io->cnt = 0;

	stm32f_gpio_mode(I2C1_SCL, ALT_FUNC, OPEN_DRAIN);
	stm32f_gpio_mode(I2C1_SDA, ALT_FUNC, OPEN_DRAIN);

#if defined(STM32F1X)
	/* Use alternate pins for I2C1 */
	afio->mapr |= AFIO_I2C1_REMAP;
#endif
#if defined(STM32F4X)
	stm32f_gpio_af(I2C1_SCL, GPIO_AF4);
	stm32f_gpio_af(I2C1_SDA, GPIO_AF4);
#endif


	/* Enable I2C clock */
	rcc->apb1enr |= RCC_I2C1EN;

	i2c->cr1 = I2C_SWRST; 

	DCC_LOG3(LOG_TRACE, "CR1=0x%04x CR2=0x%04x CCR=0x%04x", 
			 i2c->cr1, i2c->cr2, i2c->ccr);

	DCC_LOG3(LOG_TRACE, "OAR1=0x%04x OAR2=0x%04x TRISE=0X%04X", 
			 i2c->oar1, i2c->oar2, i2c->trise);

	DCC_LOG2(LOG_TRACE, "SR1=0x%04x SR2=0x%04x ", i2c->sr1, i2c->sr2);

	i2c->cr1 = 0;

	/* I2C Control register 2 (I2C_CR2) */
	i2c->cr2 = I2C_FREQ_SET(pclk / 1000000);
	/*	I2C Own address register 1 (I2C_OAR1) */
	i2c->oar1 = addr << 1;
	/*	I2C Own address register 2 (I2C_OAR2) */
	i2c->oar2 = addr << 1;
	/* I2C Clock control register (I2C_CCR) */ 
	i2c->ccr = I2C_CCR_SET(pclk / scl_freq / 2);
	/* I2C TRISE register (I2C_TRISE) */
	i2c->trise = I2C_TRISE_SET((pclk / 1000000) + 1);

	i2c_io.event = thinkos_ev_alloc();

	return io;
}

void i2c_slave_enable(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;

	cm3_irq_enable(STM32F_IRQ_I2C1_EV);
	cm3_irq_enable(STM32F_IRQ_I2C1_ER);

	DCC_LOG(LOG_TRACE, "Enabling interrupts....");
	/* enable ACK, events and errors */
	i2c->cr2 |= I2C_ITERREN | I2C_ITEVTEN | I2C_ITBUFEN;

	DCC_LOG(LOG_TRACE, "Enabling device ....");
	/* enable ACK */
	/* I2C Control register 1 (I2C_CR1) */
	i2c->cr1 = I2C_STOP | I2C_ACK | I2C_PE; 
}

/* ----------------------------------------------------------------------
 * I2C I/O busy blocking (no interrupts)
 * ----------------------------------------------------------------------
 */
int i2c_slave_io_busy(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	struct i2c_io_blk * io = &i2c_io;
	uint8_t * mem;
	uint32_t sr1;
	uint32_t sr2;
	int cnt;
	int idx;
	int max;
	int ret;

	
//	DCC_LOG(LOG_TRACE, "1.");
	/* enable ACK, events and errors */
	i2c->cr1 = I2C_STOP | I2C_ACK | I2C_PE; 

	/* Wait for address match */
	while (((sr1 = i2c->sr1) & I2C_ADDR) == 0) {
		/* Acknowledge failure */
		if (sr1 & I2C_AF) {
			DCC_LOG(LOG_TRACE, "AF");
			goto abort;
		}
		if (sr1 & I2C_STOPF) {
			DCC_LOG(LOG_TRACE, "STOPF");
			goto abort;
		}
	}

	/* Clear ADDR flag */
	sr2 = i2c->sr2;
	
//	DCC_LOG(LOG_TRACE, "2.");

	idx = io->idx;
	max = io->max;
	mem = (uint8_t *)io->mem;

	/* Check if the tranfer direction */
	if (sr2 & I2C_TRA) {
		/* Transmmit (slave --> master) */
		cnt = 0;
//		DCC_LOG(LOG_TRACE, "TRA=1 (xmit)");
		for (cnt = 0; ;++cnt) {
			do {
				sr1 = i2c->sr1;
				if (sr1 & I2C_AF) {
					DCC_LOG(LOG_TRACE, "AF");
					/* clear AF */
					goto eot;
				}
				if (sr1 & I2C_STOPF) {
					DCC_LOG(LOG_TRACE, "STOPF");
				}
				if (sr1 & I2C_BTF) {
					DCC_LOG(LOG_TRACE, "BTF");
				}
			} while ((sr1 & I2C_TXE) == 0);

			i2c->dr = mem[idx];

			idx = (idx == max) ? 0 : idx + 1;
		}
eot:
		do {
			sr2 = i2c->sr2;
		} while ((sr2 & I2C_BUSY));
		/* clear ACK */
		i2c->cr1 = I2C_PE; 
		/* clear AF */
		i2c->sr1 = 0;

		if (cnt) {
			/* discard the last item, as it was inserted in
			   the buffer but never transmitted */
			cnt--;
			idx = (idx == 0) ? max : idx - 1;
		}

		io->idx = idx;

		DCC_LOG1(LOG_TRACE, "cnt=%d", cnt);
//		DCC_LOG2(LOG_TRACE, "SR1=0x%04x SR2=0x%04x ", i2c->sr1, i2c->sr2);

		ret = I2C_XFER_IN;
	} else {
		/* Receive (slave <-- master) */
//		printf("TRA=0 ");
		DCC_LOG(LOG_TRACE, "TRA=0 (recv)");
		cnt = 0;
		ret = I2C_XFER_OUT;
	}

//	printf("\n");

	return ret;

abort:
	DCC_LOG2(LOG_TRACE, "Abort: SR1=0x%04x SR2=0x%04x ", i2c->sr1, i2c->sr2);
	/* clear AF */
	i2c->sr1 = 0;
	
	i2c->cr1 = I2C_PE; 
	udelay(500000);

	return I2C_XFER_ERR;
}

/* ----------------------------------------------------------------------
 * I2C I/O blocking (interrupts)
 * ----------------------------------------------------------------------
 */
int i2c_slave_io(void)
{
	struct i2c_io_blk * io = &i2c_io;

	DCC_LOG(LOG_TRACE, "wait...");
	thinkos_ev_wait(io->event);
	DCC_LOG(LOG_TRACE, "wake up.");

	return io->xfer;
}

void stm32f_i2c1_ev_isr(void) 
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	struct i2c_io_blk * io = &i2c_io;
	static uint32_t irq_cnt = 0;
	uint8_t * mem = (uint8_t *)io->mem;
	uint32_t sr1;
	uint32_t sr2;

	irq_cnt++;

	sr1 = i2c->sr1;
	
	if (sr1 & I2C_ADDR) {
		/* Clear ADDR flag */
		sr2 = i2c->sr2;
		/* Check if the tranfer direction */
		if (sr2 & I2C_TRA) {
			io->xfer = I2C_XFER_OUT;
//			DCC_LOG1(LOG_TRACE, "%d ADDR: TRA=1 (xmit)", irq_cnt);
		} else {
			io->xfer = I2C_XFER_IN;
//			DCC_LOG1(LOG_TRACE, "%d ADDR: TRA=0 (recv)", irq_cnt);
		} 
		io->cnt = 0;
	}

	if (sr1 & I2C_STOPF) {
		/* Clear STOPF */
		i2c->cr1 = I2C_ACK | I2C_PE; 
		DCC_LOG3(LOG_TRACE, "%d STOPF: cnt=%d %s", irq_cnt, io->cnt,
			(io->xfer == I2C_XFER_IN) ? "IN" : "OUT");
		__thinkos_ev_raise(io->event);
	}

	if (sr1 & I2C_BTF) {
		DCC_LOG(LOG_TRACE, "BTF");
	}

	if (sr1 & I2C_RXNE) {
		mem[io->idx] = i2c->dr;
	//	DCC_LOG2(LOG_TRACE, "%d RXNE %02x", irq_cnt, mem[io->idx]);
		io->idx = (io->idx == io->max) ? 0 : io->idx + 1;
		io->cnt++;
	}

	if (sr1 & I2C_TXE) {
	//	DCC_LOG1(LOG_TRACE, "%d TXE", irq_cnt);
		i2c->dr = mem[io->idx];
		io->idx = (io->idx == io->max) ? 0 : io->idx + 1;
		io->cnt++;
		DCC_LOG2(LOG_TRACE, "%d TXE %02x", irq_cnt, mem[io->idx]);
	}
}

void stm32f_i2c1_er_isr(void) 
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint32_t sr1;

	sr1 = i2c->sr1;

	if (sr1 & I2C_BERR) {
		DCC_LOG(LOG_TRACE, "BERR");
	}

	if (sr1 & I2C_AF) {
		DCC_LOG(LOG_TRACE, "AF");
		/* clear AF */
		i2c->sr1 = 0;
	}

	if (sr1 & I2C_OVR) {
		DCC_LOG(LOG_TRACE, "OVR");
	}
}

#define IO_MEM_SIZE 32

uint8_t io_mem[IO_MEM_SIZE];

int main(int argc, char ** argv)
{
	int led = 0;
	int ret;
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. leds_init()");
	leds_init();

	DCC_LOG(LOG_TRACE, "3. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));

	DCC_LOG(LOG_TRACE, "4. stdio_init()");
	stdio_init();

	printf("\n\n");
	printf("-----------------------------------------\n");
	printf(" I2C slave test\n");
	printf("-----------------------------------------\n");
	printf("\n");

	for (i = 0; i < IO_MEM_SIZE; ++i)
		io_mem[i] = i * 2;

	i2c_slave_init(100000, 0x55, io_mem, IO_MEM_SIZE);

	i2c_slave_enable();

	for (i = 0; ; ++i) {
		if ((i & 0xff) == 0) {
			led = i % 5;
			led_on(led);
		}

		ret = i2c_slave_io();
		switch (ret) {
		case I2C_XFER_IN:
			printf("In ");
			break;
		case I2C_XFER_OUT:
			printf("Out ");
			break;
		case I2C_XFER_ERR:
			printf("Err ");
			break;
		}

		led_off(led);
	}

	return 0;
}

