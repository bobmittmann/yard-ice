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

const struct stm32f_io led_io[] = {
	{ STM32F_GPIOB, 14 },
	{ STM32F_GPIOB, 15 },
	{ STM32F_GPIOC, 13 },
	{ STM32F_GPIOC, 14 },
	{ STM32F_GPIOC, 15 }
};

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

	for (i = 0; i < 5; ++i) {
		stm32f_gpio_mode(led_io[i].gpio, led_io[i].pin,
						 OUTPUT, PUSH_PULL | SPEED_LOW);

		stm32f_gpio_clr(led_io[i].gpio, led_io[i].pin);
	}
}

/* ----------------------------------------------------------------------
 * Relays 
 * ----------------------------------------------------------------------
 */

const struct stm32f_io relay_io[] = {
	{ STM32F_GPIOA, 9 },
	{ STM32F_GPIOA, 10 },
	{ STM32F_GPIOA, 11 },
	{ STM32F_GPIOA, 12 },
	{ STM32F_GPIOB, 2 }
};

void relay_on(int id)
{
	stm32f_gpio_set(relay_io[id].gpio, relay_io[id].pin);
}

void relay_off(int id)
{
	stm32f_gpio_clr(relay_io[id].gpio, relay_io[id].pin);
}

void relays_init(void)
{
	int i;

	for (i = 0; i < 5; ++i) {
		stm32f_gpio_mode(relay_io[i].gpio, relay_io[i].pin,
						 OUTPUT, PUSH_PULL | SPEED_LOW);

		stm32f_gpio_clr(relay_io[i].gpio, relay_io[i].pin);
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
	struct stm32f_usart * us = STM32F_USART1;
	struct stm32f_afio * afio = STM32F_AFIO;

	/* USART1_TX */
	stm32f_gpio_mode(USART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	/* USART1_RX */
	stm32f_gpio_mode(USART1_RX, INPUT, PULL_UP);
	/* Use alternate pins for USART1 */
	afio->mapr |= AFIO_USART1_REMAP;

	stm32f_usart_init(us);
	stm32f_usart_baudrate_set(us, 115200);
	stm32f_usart_mode_set(us, SERIAL_8N1);
	stm32f_usart_enable(us);

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
	struct stm32f_rcc * rcc = STM32F_RCC;

	DCC_LOG(LOG_MSG, "Configuring GPIO pins...");

	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOC);

	/* Enable Alternate Functions IO clock */
	rcc->apb2enr |= RCC_AFIOEN;

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
#define I2C1_SCL STM32F_GPIOB, 8
#define I2C1_SDA STM32F_GPIOB, 9

void i2c_master_init(unsigned int scl_freq)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	struct stm32f_afio * afio = STM32F_AFIO;
	struct stm32f_rcc * rcc = STM32F_RCC;
	uint32_t pclk = stm32f_apb1_hz;

	stm32f_gpio_mode(I2C1_SCL, OUTPUT, PUSH_PULL);
	stm32f_gpio_mode(I2C1_SDA, ALT_FUNC, PULL_UP);
	/* Use alternate pins for I2C1 */
	afio->mapr |= AFIO_I2C1_REMAP;


	/* Enable I2C clock */
	rcc->apb1enr |= RCC_I2C1EN;

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
	i2c->cr1 = I2C_PE | I2C_SWRST; /* Enable peripheral */

	i2c->cr1 = I2C_SWRST | I2C_PE; /* Enable peripheral */
	udelay(100);
	i2c->cr1 = I2C_PE; /* Enable peripheral */

	printf("%s() scl_freq=%d\n", __func__, scl_freq);
	printf("%s() SR1=0x%04x SR2=0x%04x\n", __func__, i2c->sr1, i2c->sr2);
}

int i2c_master_rd(unsigned int addr, void * buf, int len)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint8_t * ptr = (uint8_t *)buf;
	uint32_t sr;
	uint32_t sr1;
	uint32_t sr2;
	int again;

	printf("%s() addr=%d\n", __func__, addr);

	i2c->cr1 |= I2C_START; /* generate a Start condition */

	/* Once the Start condition is sent:
	● The SB bit is set by hardware and an interrupt is generated if 
	the ITEVFEN bit is set.
	Then the master waits for a read of the SR1 register followed by 
	a write in the DR register with the Slave address (see Figure 237 
	& Figure 238 Transfer sequencing EV5). */

	printf("1. START\n");
	again = 0;
	while (((sr1 = i2c->sr1) & I2C_SB) == 0) {
		(void)sr1;
		sr2 = i2c->sr2;;
		(void)sr2;
		if (!again)
			printf("SR1=0x%04x SR2=0x%04x\n", sr1, sr2);
		++again;
		udelay(100000);
	}


	/* – To enter Receiver mode, a master sends the slave 
	   address with LSB set. */

	i2c->dr = (addr << 1) | 1;

	printf("2. ADDR\n");
	while ((sr = (i2c->sr1 & I2C_ADDR)) == 0) {
		(void)sr;
	}

	if (len == 1) {
	/* ● Case of a single byte to be received:
		– In the ADDR event, clear the ACK bit.
		– Clear ADDR
		– Program the STOP/START bit.
		– Read the data after the RxNE flag is set.a */
		sr = i2c->sr2; /* Clear ADDR */
		(void)sr;
	
		i2c->cr1 = I2C_STOP | I2C_PE; /* generate a Stop condition */
	
		printf("3. STOP/DATA\n");
	}


	while ((sr = (i2c->sr1 & I2C_RXNE)) == 0) {
		(void)sr;
	}


	*ptr = i2c->dr;

	return len;

}

int main(int argc, char ** argv)
{
	uint8_t buf[32];
	uint8_t addr = 0;
	int led;
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. leds_init()");
	leds_init();

	DCC_LOG(LOG_TRACE, "3. relays_init()");
	relays_init();

	DCC_LOG(LOG_TRACE, "4. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));

	DCC_LOG(LOG_TRACE, "5. stdio_init()");
	stdio_init();

	printf("\n\n");
	printf("-----------------------------------------\n");
	printf(" I2C master test\n");
	printf("-----------------------------------------\n");
	printf("\n");

	i2c_master_init(100000);

	for (i = 0; ; i++) {
	
		i2c_master_rd(addr++, buf, 1);

		led = i % 5;

		led_on(led);
		printf(" - %3d ", i);
		thinkos_sleep(100);
		led_off(led);

		thinkos_sleep(900);
	
		printf("\n");
	}

	return 0;
}

