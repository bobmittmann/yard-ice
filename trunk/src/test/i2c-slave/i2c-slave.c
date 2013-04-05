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

void i2c_slave_init(unsigned int scl_freq, unsigned int addr)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	struct stm32f_rcc * rcc = STM32F_RCC;
	uint32_t pclk = stm32f_apb1_hz;
#if defined(STM32F1X)
	struct stm32f_afio * afio = STM32F_AFIO;
#endif

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
	i2c->oar1 = addr;
	/*	I2C Own address register 2 (I2C_OAR2) */
	i2c->oar2 = addr;
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

int i2c_slave_io(void)
{
	uint8_t buf[1024];
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint8_t * ptr = (uint8_t *)buf;
	uint32_t sr1;
	uint32_t sr2;
	int again = 0;

	printf(".");

	while (((sr1 = i2c->sr1) & I2C_ADDR) == 0) {
		/* Acknowledge failure */
		if (sr1 & I2C_AF)
			goto abort;
		if ((again++ & 0xff) == 0) {
			sr2 = i2c->sr2;;
//			printf("CR1=0x%04x SR1=0x%04x SR2=0x%04x\n", i2c->cr1, sr1, sr2);
		}
		udelay(1000);
	}

	sr2 = i2c->sr2;
	
	if (sr2 & I2C_TRA) {
		printf("TRA=1 ");
	} else {
		printf("TRA=0 ");
	}

	printf("\n");
	udelay(100000);

	return 0;

abort:
	printf("ABORT.\n");

	return -1;
}


int main(int argc, char ** argv)
{
	int led = 0;
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

	i2c_slave_init(100000, 32);

	for (i = 0; ; ++i) {
		if ((i & 0xff) == 0) {
			led = i % 5;
			led_on(led);
		}

		i2c_slave_io();

		led_off(led);
	}

	return 0;
}

