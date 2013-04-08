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

#include "i2c.h"
#include "console.h"
#include "io.h"


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

/* ----------------------------------------------------------------------
 * Supervisory task
 * ----------------------------------------------------------------------
 */
int supervisor_task(void)
{
	printf("%s() started...\n", __func__);

	for (;;) {
		DCC_LOG(LOG_INFO, "...");
		thinkos_sleep(200);
		led_on(3);
		thinkos_sleep(100);
		led_off(3);
		thinkos_sleep(400);
	}
}

void system_reset(void)
{
	DCC_LOG(LOG_TRACE, "...");

	CM3_SCB->aircr =  SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;

	for(;;);

}

/* ----------------------------------------------------------------------
 * User interface task
 * ----------------------------------------------------------------------
 */
int ui_task(void)
{
	int btn_st[2];
	int ev_press;
	int ev_release;
	int rst_tmr = 0;


	printf("%s() started...\n", __func__);

	btn_st[0] = push_btn_stat() ? 0 : 1;
	for (;;) {
		/* process push button */
		btn_st[1] = push_btn_stat() ? 0 : 1;
		ev_press = btn_st[1] & (btn_st[1] ^ btn_st[0]);
		ev_release = btn_st[0] & (btn_st[1] ^ btn_st[0]);
		btn_st[0] = btn_st[1];

		if (ev_press) {
			DCC_LOG(LOG_TRACE, "BTN Down");
			printf("\nI2C reset...\n");
			i2c_reset();
			/* set reset timer */
			rst_tmr = 50;
		}

		if (ev_release) {
			DCC_LOG(LOG_TRACE, "BTN Up");
			/* clear 'reset timer' */
			rst_tmr = 0;
		}

		if (rst_tmr)
			rst_tmr--;

		switch (rst_tmr) {
		case 18:
		case 16:
		case 14:
		case 12:
		case 10:
		case 8:
		case 4:
		case 2:
			led_on(0);
			led_on(1);
			led_on(2);
			led_on(3);
			break;
		case 17:
		case 15:
		case 13:
		case 11:
		case 9:
		case 7:
		case 5:
		case 3:
			led_off(0);
			led_off(1);
			led_off(2);
			led_off(3);
			break;
		case 1:
			system_reset();
			break;
		}

		thinkos_sleep(100);
	}
}

int32_t i2c_mutex;
int32_t phif_addr = 0x55;
int32_t codec_addr = 64;

#define PHIF_ID_REG 0
#define PHIF_ADC_REG 2
#define PHIF_LED_REG 12
#define PHIF_RLY_REG 13
#define PHIF_VR0_REG 14
#define PHIF_VR1_REG 15

int acq_task(void)
{
	uint16_t adc[5];
	uint8_t reg;
	int i;

	DCC_LOG(LOG_TRACE, "started...");
	printf("%s() started...\n", __func__);

	for (;;) {
		DCC_LOG(LOG_INFO, "Poll...");
		thinkos_sleep(1000);
		thinkos_mutex_lock(i2c_mutex);
		reg = PHIF_ADC_REG;
		if (i2c_master_wr(phif_addr, &reg, 1) > 0) {
			DCC_LOG(LOG_INFO, "i2c_master_wr().");
			if (i2c_master_rd(phif_addr, adc, sizeof(adc)) > 0) {
				DCC_LOG5(LOG_TRACE, "ADC %5d %5d %5d %5d %5d",
						 adc[0], adc[1], adc[2], adc[3], adc[4]);
				printf("ADC: ");
				for (i = 0; i < 5; ++i) {
					printf("%5d", adc[i]);
				}
				printf("\n");
			} else {
				DCC_LOG(LOG_WARNING, "i2c_master_rd() failed!");
			}
		} else {
			DCC_LOG(LOG_WARNING, "i2c_master_wr() failed!");
		}
		thinkos_mutex_unlock(i2c_mutex);
	}
}

void vr_set(unsigned int val0, unsigned int val1)
{
	uint8_t pkt[3];

	DCC_LOG2(LOG_TRACE, "vr0=%d vr1=%d", val0, val1);

	thinkos_mutex_lock(i2c_mutex);

	pkt[0] = PHIF_VR0_REG;
	pkt[1] = val0;
	pkt[2] = val1;

	if (i2c_master_wr(phif_addr, pkt, 3) > 0) {
		DCC_LOG(LOG_INFO, "ok.");
	} else {
		DCC_LOG(LOG_WARNING, "i2c_master_wr() failed!");
	}

	thinkos_mutex_unlock(i2c_mutex);
}

void i2c_bus_scan(void)
{
	uint8_t buf[4];
	uint8_t addr = 0;

	thinkos_mutex_lock(i2c_mutex);

	printf("- I2C bus scan: ");

	/* 7 bit addresses range from 0 to 0x78 */
	for (addr = 1; addr < 0x78; ++addr) {

		DCC_LOG1(LOG_TRACE, "Addr=0x%02x", addr);

		buf[0] = 0;
		if (i2c_master_wr(addr, buf, 1) <= 0) {
			printf(".");
			continue;
		}

		printf("\nI2C device found @ %d", addr);

		if (i2c_master_rd(addr, buf, 2) != 2) {
			printf("\n");
			continue;
		}

		DCC_LOG3(LOG_TRACE, "Addr=0x%02x [0]--> 0x%02x%02x", 
				 addr, buf[1], buf[0]);
		printf(" 0x%02x%02x\n", buf[1], buf[0]);

		if ((buf[0] == 'P') && (buf[1] == 'H')) {
			printf(" Phone Interface.\n");
			DCC_LOG1(LOG_TRACE, "Phone Interface @ 0x%02x", addr);
			phif_addr = addr;
		}
	}

	DCC_LOG(LOG_TRACE, "done.");
	thinkos_mutex_unlock(i2c_mutex);

	printf("\n");
}

uint32_t supervisor_stack[256];
uint32_t ui_stack[256];
uint32_t acq_stack[256];

int main(int argc, char ** argv)
{
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

	DCC_LOG(LOG_TRACE, "5. i2c_master_init()");
	i2c_master_init(100000);

	DCC_LOG(LOG_TRACE, "6. i2c_master_enable()");
	i2c_master_enable();

	thinkos_sleep(100);

	i2c_mutex = thinkos_mutex_alloc();
	printf("I2C mutex=%d\n", i2c_mutex);

	i2c_bus_scan();

	thinkos_thread_create((void *)supervisor_task, (void *)NULL,
						  supervisor_stack, sizeof(supervisor_stack), 
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	thinkos_thread_create((void *)ui_task, (void *)NULL,
						  ui_stack, sizeof(ui_stack), 
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	thinkos_thread_create((void *)acq_task, (void *)NULL,
						  acq_stack, sizeof(acq_stack), 
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));

	for (i = 0; ; ++i) {
		thinkos_sleep(3000);
		vr_set(i, 2 * i);
	}


	return 0;
}

