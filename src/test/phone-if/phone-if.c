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

#include "io.h"
#include "i2c.h"
#include "dac.h"
#include "dgpot.h"
#include "adc.h"
#include "console.h"

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

void stm32f_usart1_isr(void)
{
	struct stm32f_usart * uart = STM32F_USART1;

	DCC_LOG(LOG_TRACE, "...");

	uart_console_isr(uart);
}

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

	stderr = &stm32f_uart1_file;

	stdin = uart_console_open(uart);
	stdout = uart_console_open(uart);

	cm3_irq_enable(STM32F_IRQ_USART1);

}

void sys_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

	/* Enable IO clocks */
	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOC);

	/* Enable Alternate Functions IO clock */
	rcc->apb2enr |= RCC_AFIOEN;
}

#define OPT_SELF_TEST 1
#define OPT_RESET 2

void self_test(void)
{
	int i;

	DCC_LOG(LOG_TRACE, "...");

	for (i = 0; i < 5; ++i) {
		led_on(i);
		relay_on(i);
		thinkos_sleep(100);
		led_off(i);
		relay_off(i);
		thinkos_sleep(100);
	}
}

void system_reset(void)
{
	DCC_LOG(LOG_TRACE, "...");

	thinkos_sleep(10);
    CM3_SCB->aircr =  SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
	for(;;);
}

int wdt_task(void)
{
	struct stm32f_iwdg * iwdg = STM32F_IWDG;

	for(;;) {
		DCC_LOG(LOG_TRACE, "WDT");
		/* WD reset */
		iwdg->kr = 0xaaaa;
		thinkos_sleep(250);
	}
}

uint32_t wdt_stack[32];

void wdt_init(void)
{
	struct stm32f_iwdg * iwdg = STM32F_IWDG;
	DCC_LOG(LOG_TRACE, "1. WDT config");

	/* WD unlock */
	iwdg->kr = 0x5555;

	while (iwdg->sr & IWDG_PVU);
	iwdg->pr = IWDG_PR_16;  /* 0.4 ms tick */

	while (iwdg->sr & IWDG_RVU);
	iwdg->rlr = 2500;  /* 500 ms / 0.4 ms */

	/* WD start */
	iwdg->kr = 0xcccc;

	DCC_LOG(LOG_TRACE, "2. thinkos_thread_create()");
	thinkos_thread_create((void *)wdt_task, (void *)NULL,
						  wdt_stack, sizeof(wdt_stack), 
						  THINKOS_OPT_PRIORITY(3) | THINKOS_OPT_ID(3));

	thinkos_sleep(10);
	DCC_LOG(LOG_TRACE, "3. done.");
}

void show_menu(void)
{
	printf("\n");
	printf("---------------------------------------\n");
	printf("Options:\n");
	printf("  r - reset\n");
	printf("  t - test\n");
	printf("\n");
}

int shell_task(void)
{
	int c;

	for(;;) {
		c = getchar();
		printf("%c", c);
		switch (c) {
		case '\n':
			show_menu();
			break;
		case 'r':
			system_reset();
			break;
		case 't':
			self_test();
			break;
		}
	}
}

uint32_t shell_stack[128];

void shell_init(void)
{
	DCC_LOG(LOG_TRACE, "thinkos_thread_create()");
	thinkos_thread_create((void *)shell_task, (void *)NULL,
						  shell_stack, sizeof(shell_stack), 
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));

	thinkos_sleep(10);
}

struct io_block {
	uint8_t magic[2];
	uint16_t adc[ADC_CHANS];
	uint8_t led;
	uint8_t relay;
	uint8_t dgpot[2];
	uint8_t opt;
};


struct io_block rd_block = {
	.magic = { 'P', 'H' }
};

struct io_block wr_block;

void process_data_in(void)
{
	uint8_t set;
	uint8_t clr;
	uint8_t val;
	int i;

	DCC_LOG(LOG_TRACE, "...");

	if (wr_block.opt & OPT_SELF_TEST) {
		printf("[SELF TEST]");
		self_test();
	}

	if (wr_block.opt & OPT_RESET) {
		printf("[RESET]");
		system_reset();
	}

	set = wr_block.led & (wr_block.led ^ wr_block.led);
	clr = wr_block.led & (wr_block.led ^ wr_block.led);

	for (i = 0; i < 5; ++i) {
		if (clr & (1 << i)) {
			printf("[LED%d ON]", i);
			led_on(i);
		} else if (set & (1 << i)) {
			printf("[LED%d OFF]", i);
			led_off(i);
		}
	}
	
	/* update read block */
	rd_block.led = wr_block.led;

	set = wr_block.relay & (wr_block.relay ^ wr_block.relay);
	clr = wr_block.relay & (wr_block.relay ^ wr_block.relay);

	for (i = 0; i < 5; ++i) {
		if (clr & (1 << i)) {
			printf("[RELAY%d ON]", i);
			relay_on(i);
		} else if (set & (1 << i)) {
			printf("[RELAY%d OFF]", i);
			relay_off(i);
		}
		rd_block.relay = wr_block.relay;
	}

	val = dgpot_set(0, wr_block.dgpot[0]);
	if (val != rd_block.dgpot[0]) {
		rd_block.dgpot[0] = val;
		printf("[POT0 %d]", val);
	}

	val = dgpot_set(1, wr_block.dgpot[1]);
	if (val != rd_block.dgpot[1]) {
		rd_block.dgpot[1] = val;
		printf("[POT1 %d]", val);
	}
}

#define DEVICE_ADDR 0x55

int main(int argc, char ** argv)
{
	int xfer;
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "2. sys_init()");
	sys_init();

	DCC_LOG(LOG_TRACE, "3. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "4. stdio_init()");
	stdio_init();

	DCC_LOG(LOG_TRACE, "5. wdt_init()");
//	wdt_init();

	DCC_LOG(LOG_TRACE, "6. leds_init()");
	leds_init();

	DCC_LOG(LOG_TRACE, "7. relays_init()");
	relays_init();

	DCC_LOG(LOG_TRACE, "8. codec_io_init()");
	codec_io_init();

	DCC_LOG(LOG_TRACE, "9. dac_init()");
	dac_init();

	DCC_LOG(LOG_TRACE, "10. adc_init()");
	adc_init(rd_block.adc);

	DCC_LOG(LOG_TRACE, "11. dgpot_init()");
	dgpot_init();

	DCC_LOG(LOG_TRACE, "12. i2c_slave_init()");
	i2c_slave_init(100000, DEVICE_ADDR, &rd_block, &wr_block, 
				   sizeof(struct io_block));

	leds_all_on();
	thinkos_sleep(100);
	leds_all_off();
	thinkos_sleep(100);
	leds_all_on();
	thinkos_sleep(100);
	leds_all_off();
	thinkos_sleep(100);
	leds_all_on();
	thinkos_sleep(100);
	leds_all_off();

	DCC_LOG(LOG_TRACE, "13. self_test()");
	self_test();

	DCC_LOG(LOG_TRACE, "14. i2c_slave_enable()");
	i2c_slave_enable();

	DCC_LOG(LOG_TRACE, "15. adc_start()");
	adc_start();

	DCC_LOG(LOG_TRACE, "16. dac_start()");
	dac_start();

	DCC_LOG(LOG_TRACE, "17. shell_init()");
	shell_init();

	printf("\n\n");
	printf("-----------------------------------------\n");
	printf(" Phone interface\n");
	printf("-----------------------------------------\n");
	printf("\n");

	for (i = 0; ; ++i) {
		xfer = i2c_slave_io();
		switch (xfer) {
		case I2C_XFER_IN:
			DCC_LOG(LOG_TRACE, "IN");
			printf("In ");
			process_data_in();
			break;
		case I2C_XFER_OUT:
			DCC_LOG(LOG_TRACE, "OUT");
			printf("Out ");
			DCC_LOG5(LOG_TRACE, "%5d %5d %5d %5d %5d ", 
					 rd_block.adc[0], rd_block.adc[1], 
					 rd_block.adc[2], rd_block.adc[3], 
					 rd_block.adc[4] );
			break;
		case I2C_XFER_ERR:
			printf("Err ");
			break;
		}

	}

	return 0;
}

