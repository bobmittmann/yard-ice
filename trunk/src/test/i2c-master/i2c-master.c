/* 
 * File:	 i2c-master.c
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

void io_init(void)
{
#if defined(STM32F1X)
	struct stm32f_rcc * rcc = STM32F_RCC;

	/* Enable Alternate Functions IO clock */
	rcc->apb2enr |= RCC_AFIOEN;
#endif

	DCC_LOG(LOG_MSG, "Configuring GPIO pins...");

	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOC);


#if defined(STM32F1X)
	stm32f_gpio_mode(TLV320CLK, ALT_FUNC, PUSH_PULL | SPEED_HIGH);

	stm32f_gpio_mode(TLV320RST, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_clr(TLV320RST);
	udelay(1000);
	stm32f_gpio_set(TLV320RST);
#endif

#if defined(STM32F4X)
	stm32f_gpio_mode(STM32F_GPIOB, 10, INPUT, 0);
	stm32f_gpio_mode(STM32F_GPIOB, 11, INPUT, 0);
#endif

}

/* ----------------------------------------------------------------------
 * I2C
 * ----------------------------------------------------------------------
 */
#define I2C1_SCL STM32F_GPIOB, 8
#define I2C1_SDA STM32F_GPIOB, 9

void i2c_master_init(unsigned int scl_freq)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	struct stm32f_rcc * rcc = STM32F_RCC;
	uint32_t pclk = stm32f_apb1_hz;
#if defined(STM32F1X)
	struct stm32f_afio * afio = STM32F_AFIO;
	/* Use alternate pins for I2C1 */
	afio->mapr |= AFIO_I2C1_REMAP;
#endif

#if 0
	stm32f_gpio_mode(I2C1_SCL, INPUT, 0);
	stm32f_gpio_mode(I2C1_SDA, INPUT, 0);

	printf("%s() SDA=%d SCL=%d\n", __func__,
		   stm32f_gpio_stat(I2C1_SDA) ? 1 : 0,
		   stm32f_gpio_stat(I2C1_SCL) ? 1 : 0);

#endif

	stm32f_gpio_mode(I2C1_SCL, ALT_FUNC, OPEN_DRAIN);
	stm32f_gpio_mode(I2C1_SDA, ALT_FUNC, OPEN_DRAIN);

#if defined(STM32F4X)
	stm32f_gpio_af(I2C1_SCL, GPIO_AF4);
	stm32f_gpio_af(I2C1_SDA, GPIO_AF4);
#endif

	/* Enable I2C clock */
	rcc->apb1enr |= RCC_I2C1EN;

	printf("%s() i2c=0x%08x\n", __func__, (uint32_t)i2c);

	/* Software reset */
	i2c->cr1 = I2C_SWRST; 
	udelay(100);


	printf("CR1=0x%04x CR2=0x%04x CCR=0x%04x\n", 
			 i2c->cr1, i2c->cr2, i2c->ccr);

	printf("OAR1=0x%04x OAR2=0x%04x TRISE=0x%04x\n", 
			 i2c->oar1, i2c->oar2, i2c->trise);

	printf("SR1=0x%04x SR2=0x%04x\n", i2c->sr1, i2c->sr2);


	DCC_LOG3(LOG_TRACE, "CR1=0x%04x CR2=0x%04x CCR=0x%04x", 
			 i2c->cr1, i2c->cr2, i2c->ccr);

	DCC_LOG3(LOG_TRACE, "OAR1=0x%04x OAR2=0x%04x TRISE=0x%04x", 
			 i2c->oar1, i2c->oar2, i2c->trise);

	DCC_LOG2(LOG_TRACE, "SR1=0x%04x SR2=0x%04x ", i2c->sr1, i2c->sr2);

	i2c->cr1 = 0;

	/* I2C Control register 2 (I2C_CR2) */
	i2c->cr2 = I2C_FREQ_SET(pclk / 1000000);
	/*	I2C Own address register 1 (I2C_OAR1) */
	i2c->oar1 = 0;
	/*	I2C Own address register 2 (I2C_OAR2) */
	i2c->oar2 = 0;
	/* I2C Clock control register (I2C_CCR) */ 
	i2c->ccr = I2C_CCR_SET(pclk / scl_freq / 2);
	/* I2C TRISE register (I2C_TRISE) */
	i2c->trise = I2C_TRISE_SET((pclk / 1000000) + 1);
	/* I2C Control register 1 (I2C_CR1) */
	i2c->cr1 = I2C_PE;

	printf("%s() scl_freq=%d\n", __func__, scl_freq);
	printf("%s() SR1=0x%04x SR2=0x%04x\n", __func__, i2c->sr1, i2c->sr2);


	printf("CR1=0x%04x CR2=0x%04x CCR=0x%04x\n", 
			 i2c->cr1, i2c->cr2, i2c->ccr);

	printf("OAR1=0x%04x OAR2=0x%04x TRISE=0x%04x\n", 
			 i2c->oar1, i2c->oar2, i2c->trise);

	printf("SR1=0x%04x SR2=0x%04x\n", i2c->sr1, i2c->sr2);

}

int i2c_master_rd(unsigned int addr, void * buf, int len)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint8_t * ptr = (uint8_t *)buf;
	uint32_t sr1;
	uint32_t sr2;
	int again;
	int rem;

//	printf("%s() addr=%d\n", __func__, addr);

//	printf("1. START");
	DCC_LOG(LOG_INFO, "1. START");

	i2c->cr1 = I2C_START | I2C_PE; /* generate a Start condition */

	/* Once the Start condition is sent:
	● The SB bit is set by hardware and an interrupt is generated if 
	the ITEVFEN bit is set.
	Then the master waits for a read of the SR1 register followed by 
	a write in the DR register with the Slave address (see Figure 237 
	& Figure 238 Transfer sequencing EV5). */

	again = 0;
	while (((sr1 = i2c->sr1) & I2C_SB) == 0) {
		/* If for some reason the I2C lines get stuck
		   at high level. The start bit will
		   never be reported. No other error will be reported also.
			TODO: Implement a timeout mechanism ... */

//		printf("%s() SDA=%d SCL=%d\n", __func__,
//			   stm32f_gpio_stat(I2C1_SDA) ? 1 : 0,
//			   stm32f_gpio_stat(I2C1_SCL) ? 1 : 0);

		(void)sr1;
//		sr2 = i2c->sr2;;
//		(void)sr2;
//		if ((sr1 != 0) || (sr2 != 0)) {
//			printf("CR1=0x%04x SR1=0x%04x SR2=0x%04x\n", i2c->cr1, sr1, sr2);
//			DCC_LOG2(LOG_INFO, "SR1=0x%04x SR2=0x%04x", sr1, sr2);
//		}
		if (++again == 100)
			goto abort;

		udelay(10);
	}


	/* – To enter Receiver mode, a master sends the slave 
	   address with LSB set. */
//	printf(", 2. ADDR %d", addr);
//	printf("1.");
//	udelay(100);
	DCC_LOG1(LOG_INFO, "2. ADDR %d", addr);
	i2c->dr = (addr << 1) | 1;

	if (len == 1) {
		/* ● Case of a single byte to be received:
		   – In the ADDR event, clear the ACK bit.
		   – Clear ADDR
		   – Program the STOP/START bit.
		   – Read the data after the RxNE flag is set. */

//		printf("2.");
//		printf("~");
		again = 0;
		while (((sr1 = i2c->sr1) & I2C_ADDR) == 0) {
			/* Acknowledge failure */
			if (sr1 & I2C_AF )
				goto abort;
			if (++again == 100)
				goto abort;
			udelay(10);
		}

		/* Clear ADDR */
		sr2 = i2c->sr2;
		(void)sr2;
	
		/* Program the STOP/START bit. */
		i2c->cr1 = I2C_STOP | I2C_PE; 
	
//		printf("^");
		/* Read the data after the RxNE flag is set. */
		while ((i2c->sr1 & I2C_RXNE) == 0);

		DCC_LOG(LOG_TRACE, "3. STOP/DATA");
//		printf("3. STOP/DATA\n");
	} else if  (len == 2) {
		/* ● Case of two bytes to be received:
		   – Set POS and ACK
		   – Wait for the ADDR flag to be set
		   – Clear ADDR
		   – Clear ACK
		   – Wait for BTF to be set
		   – Program STOP
		   – Read DR twice */

		/* Set POS and ACK */
		i2c->cr1 = I2C_POS | I2C_ACK | I2C_PE; 

		/* Wait for the ADDR flag to be set */
		while (((sr1 = i2c->sr1) & I2C_ADDR) == 0) {
			/* Acknowledge failure */
			if (sr1 & I2C_AF )
				goto abort;
			if (++again == 100)
				goto abort;
			udelay(10);
		}

		/* Clear ADDR */
		sr2 = i2c->sr2; 
		(void)sr2;

		DCC_LOG1(LOG_TRACE, "CR1=0x%04x", i2c->cr1);
		/* Clear ACK ???? */
		i2c->cr1 = I2C_POS | I2C_PE; 

		/* Wait for BTF to be set */
		while (((sr1 = i2c->sr1) & I2C_BTF) == 0) {
			/* Acknowledge failure */
			if (sr1 & I2C_AF )
				goto abort;
			if (++again == 100)
				goto abort;
			udelay(10);
		}

		/* Program STOP */
		i2c->cr1 = I2C_STOP | I2C_PE; 

		/* Read DR twice */
		*ptr++ = i2c->dr;
	} else {
		rem = len;
		while (rem > 3) {
		}
		/* When 3 bytes remain to be read:
		   ● RxNE = 1 => Nothing (DataN-2 not read).
		   ● DataN-1 received
		   ● BTF = 1 because both shift and data registers are full: 
		   DataN-2 in DR and DataN-1 in the shift register => SCL tied low: 
		   no other data will be received on the bus.
		   ● Clear ACK bit
		   ● Read DataN-2 in DR => This will launch the DataN reception 
		   in the shift register
		   ● DataN received (with a NACK)
		   ● Program START/STOP
		   ● Read DataN-1
		   ● RxNE = 1
		   ● Read DataN
		 */
	}

	*ptr = i2c->dr;

	return len;

abort:
	DCC_LOG2(LOG_TRACE, "Abort: SR1=0x%04x SR2=0x%04x", i2c->sr1, i2c->sr2);
//	printf("?");
	sr1 = i2c->sr1 = 0;
	i2c->cr1 = I2C_STOP | I2C_PE; /* generate a Stop condition */

	sr1 = i2c->sr1;
	sr2 = i2c->sr2;
	
	if ((sr1 != 0) || (sr2 != 0)) {
//		printf("\nAbort: SR1=0x%04x SR2=0x%04x\n", i2c->sr1, i2c->sr2);
		DCC_LOG2(LOG_TRACE, "Reset: SR1=0x%04x SR2=0x%04x", i2c->sr1, i2c->sr2);
	}
//	printf("!");

	i2c->cr1 = I2C_STOP | I2C_PE; 
	again = 0;
	while (i2c->sr2 & I2C_BUSY) {
		if (++again == 1000)
			break;
		udelay(10);
	};

//	i2c->cr1 = 0; 
	i2c->cr1 = I2C_PE; 
#if 0
	udelay(100000);

	sr1 = i2c->sr1;
	sr2 = i2c->sr2;
	
	if ((sr1 != 0) || (sr2 != 0)) {
		printf("stick: SR1=0x%04x SR2=0x%04x\n", sr1, sr2);
	}
#endif
	return -1;
}

int i2c_master_wr(unsigned int addr, const void * buf, int len)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint8_t * ptr = (uint8_t *)buf;
	uint32_t sr1;
	uint32_t sr2;
	int rem;

	DCC_LOG(LOG_INFO, "1. START");

	i2c->cr1 = I2C_START | I2C_PE; /* generate a Start condition */

	/* Once the Start condition is sent:
	● The SB bit is set by hardware and an interrupt is generated if 
	the ITEVFEN bit is set.
	Then the master waits for a read of the SR1 register followed by 
	a write in the DR register with the Slave address (see Figure 237 
	& Figure 238 Transfer sequencing EV5). */
	while ((i2c->sr1 & I2C_SB) == 0);

	/* – To enter Transmitter mode, a master sends the slave 
	   address with LSB reset. */
	DCC_LOG1(LOG_TRACE, "2. ADDR %d", addr);
	i2c->dr = (addr << 1);

	while (((sr1 = i2c->sr1) & I2C_ADDR) == 0) {
		/* Acknowledge failure */
		if (sr1 & I2C_AF )
			goto abort;
	}

	/* Clear ADDR */
	sr2 = i2c->sr2;
	(void)sr2;

	rem = len;

	while (rem > 0) {
		while ((sr1 = (i2c->sr1 & I2C_TXE)) == 0) {
			(void)sr1;
		}

		/* Acknowledge failure */
		if (sr1 & I2C_AF ) {
			goto abort;
		}

		i2c->dr = *ptr++;

		rem--;
	}

	while ((i2c->sr1 & I2C_BTF) == 0);

	/* Program STOP. */
	i2c->cr1 = I2C_STOP | I2C_PE; 

	return len;

abort:
	DCC_LOG2(LOG_TRACE, "Abort: SR1=0x%04x SR2=0x%04x", i2c->sr1, i2c->sr2);
	sr1 = i2c->sr1 = 0;
	i2c->cr1 = I2C_STOP | I2C_PE; /* generate a Start condition */

	while (i2c->sr2 & I2C_BUSY) {
	};

	return -1;
}

/* ----------------------------------------------------------------------
 * Supervisory task
 * ----------------------------------------------------------------------
 */
int supervisor_task(void)
{
	for (;;) {
		thinkos_sleep(200);
		led_on(3);
		thinkos_sleep(100);
		led_off(3);
		thinkos_sleep(400);
	}
}


uint32_t supervisor_stack[256];

int main(int argc, char ** argv)
{
	uint8_t buf[32];
	uint8_t addr = 0;
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
	printf(" I2C master test\n");
	printf("-----------------------------------------\n");
	printf("\n");

	thinkos_thread_create((void *)supervisor_task, (void *)NULL,
						  supervisor_stack, sizeof(supervisor_stack), 
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	i2c_master_init(100000);

	printf("I2C scanning ");
	addr = 0x55;
	do {
		thinkos_sleep(900);
		printf(".");
		led_on(0);
		thinkos_sleep(100);
		ret = i2c_master_rd(addr, buf, 2);
		led_off(0);
		if (ret > 0) {
			DCC_LOG3(LOG_TRACE, "addr:%d -> 0x%02x 0x%02x", 
					 addr, buf[0], buf[1]);
			printf("\nI2C device found @ %d ->  0x%02x 0x%02x ", 
				   addr, buf[0], buf[1]);
		}
	} while (1);

again:
	do {
		thinkos_sleep(1000);
		printf("I2C scanning ");
		for (addr = 0; addr < 128; ++addr) {
			printf(".");
			led_on(0);
			ret = i2c_master_rd(addr, buf, 2);
			led_off(0);
			if (ret > 0) {
				DCC_LOG3(LOG_TRACE, "addr:%d -> 0x%02x 0x%02x", 
						 addr, buf[0], buf[1]);
				printf("\nI2C device found @ %d ->  0x%02x 0x%02x ", 
					   addr, buf[0], buf[1]);
	//			break;
			}
			thinkos_sleep(10);
		}

		printf("\n");
	} while (addr == 128);

	led_on(1);
	thinkos_sleep(100);
	led_off(1);
	thinkos_sleep(900);

	goto again;

	for (i = 0; ; ++i) {
		led_on(0);
		thinkos_sleep(100);
		led_off(0);
		thinkos_sleep(900);

		if (i2c_master_rd(addr, buf, 2) > 0) {
			DCC_LOG3(LOG_TRACE, "addr:%d -> 0x%02x 0x%02x", addr, 
					 buf[0], buf[1]);
		}

//		i2c_master_wr(addr, buf, 2);

	}

	return 0;
}

