/* 
 * File:	 usb-test.c
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
#include <sys/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/dcclog.h>
#include <sys/usb-cdc.h>
#include <sys/tty.h>
#include <trace.h>

#include <thinkos.h>

#include "board.h"
#include "net.h"
#include "io.h"

#define VERSION_NUM "0.3"
#define VERSION_DATE "Nov, 2014"

const char * version_str = "SLC Device Hub " \
							VERSION_NUM " - " VERSION_DATE;
const char * copyright_str = "(c) Copyright 2014 - Mircom Group";


int stdio_shell(void);
void env_init(void);

/* -------------------------------------------------------------------------
 * System Supervision
 * ------------------------------------------------------------------------- */

const char * const trace_lvl_tab[] = {
		"   NONE",
		"  ERROR",
		"WARNING",
		"   INFO",
		"  DEBUG"
};

FILE * monitor_stream;
bool monitor_auto_flush;

void __attribute__((noreturn)) supervisor_task(void)
{
	struct trace_entry trace;
	uint32_t clk;

	INF("<%d> started...", thinkos_thread_self());

	/* fall back to stdout */
	monitor_stream = stdout;

	trace_tail(&trace);

	clk = thinkos_clock();
	for (;;) {
		struct timeval tv;
		char s[80];

		/* 8Hz periodic task */
		clk += 125;
		thinkos_alarm(clk);

		while (trace_getnext(&trace, s, sizeof(s)) >= 0) {
			trace_ts2timeval(&tv, trace.dt);
			fprintf(monitor_stream, "%s %2d.%06d: %s\n", 
					trace_lvl_tab[trace.ref->lvl],
					(int)tv.tv_sec, (int)tv.tv_usec, s);
		}

		if (monitor_auto_flush)
			trace_flush(&trace);
	}
}


uint32_t supervisor_stack[128];

const struct thinkos_thread_inf supervisor_inf = {
	.stack_ptr = supervisor_stack,
	.stack_size = sizeof(supervisor_stack),
	.priority = 32,
	.thread_id = 31,
	.paused = false,
	.tag = "SUPV"
};

void supervisor_init(void)
{
	trace_init();

	thinkos_thread_create_inf((void *)supervisor_task, (void *)NULL,
							  &supervisor_inf);
}

void __attribute__((noreturn)) stdio_shell_task(void * arg)
{
	DCC_LOG1(LOG_TRACE, "thread=%d", thinkos_thread_self());

	DBG("<%d> started...", thinkos_thread_self());
	
	for (;;) {
		printf("\n...\n");
		DCC_LOG(LOG_TRACE, "14. usb_shell()");
		stdio_shell();
	}
}

uint32_t __attribute__((aligned(8))) stdio_shell_stack[1024 + 256];

void stdio_init(void)
{
	usb_cdc_class_t * cdc;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	DCC_LOG(LOG_TRACE, "13. usb_cdc_init()");
	usb_cdc_sn_set(*((uint64_t *)STM32F_UID));

	cdc = usb_cdc_init(&stm32f_otg_fs_dev, 
					   cdc_acm_def_str, 
					   cdc_acm_def_strcnt);

	f_raw = usb_cdc_fopen(cdc);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stderr = f_tty;
	stdout = f_tty;
	stdin = f_tty;

	thinkos_thread_create((void *)stdio_shell_task, (void *)cdc,
						  stdio_shell_stack, sizeof(stdio_shell_stack) | 
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));
}


void board_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	/* - LEDs ---------------------------------------------------------*/
	stm32_gpio_mode(IO_LED1, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_clr(IO_LED1);

	stm32_gpio_mode(IO_LED2, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_clr(IO_LED2);

	/* - Address switch -------------------------------------------------*/
	stm32_gpio_mode(IO_ADDR0, INPUT, PULL_UP);
	stm32_gpio_mode(IO_ADDR1, INPUT, PULL_UP);
	stm32_gpio_mode(IO_ADDR2, INPUT, PULL_UP);
	stm32_gpio_mode(IO_ADDR3, INPUT, PULL_UP);

	/* - Mode switch ---------------------------------------------------*/
	stm32_gpio_mode(IO_MODE0, INPUT, PULL_UP);
	stm32_gpio_mode(IO_MODE1, INPUT, PULL_UP);
	stm32_gpio_mode(IO_MODE2, INPUT, PULL_UP);
	stm32_gpio_mode(IO_MODE3, INPUT, PULL_UP);

	/* - RS485 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_RS485_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(IO_RS485_RX, RS485_USART_AF);

	stm32_gpio_mode(IO_RS485_TX, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(IO_RS485_TX, RS485_USART_AF);

	stm32_gpio_mode(IO_RS485_MODE, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_set(IO_RS485_MODE);

	stm32_gpio_mode(IO_RS485_DBG, INPUT, PULL_UP);

	/* - UART 2 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART2_TX, UART2_AF);
	stm32_gpio_mode(IO_UART2_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART2_RX, UART2_AF);

	/* - UART 3 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART3_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART3_TX, UART3_AF);
	stm32_gpio_mode(IO_UART3_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART3_RX, UART3_AF);

	/* - UART 4 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART4_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART4_TX, UART4_AF);
	stm32_gpio_mode(IO_UART4_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART4_RX, UART4_AF);

	/* - UART 5 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART5_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART5_TX, UART5_AF);
	stm32_gpio_mode(IO_UART5_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART5_RX, UART5_AF);

	/* - UART 6 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART6_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART6_TX, UART6_AF);
	stm32_gpio_mode(IO_UART6_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART6_RX, UART6_AF);

	/* - Serial Flash ----------------------------------------------------*/
	stm32_gpio_mode(IO_SFLASH_CS, OUTPUT, SPEED_HIGH);
	stm32_gpio_set(IO_SFLASH_CS);

	stm32_gpio_mode(IO_SFLASH_SCK, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_af(IO_SFLASH_SCK, SFLASH_SPI_AF);

	stm32_gpio_mode(IO_SFLASH_MISO, ALT_FUNC, SPEED_HIGH);
	stm32_gpio_af(IO_SFLASH_MISO, SFLASH_SPI_AF);

	stm32_gpio_mode(IO_SFLASH_MOSI, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_af(IO_SFLASH_MOSI, SFLASH_SPI_AF);
}

int main(int argc, char ** argv)
{
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. board_init()");
	board_init();

	DCC_LOG(LOG_TRACE, "2. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "3. env_init().");
	stm32f_nvram_env_init();

	DCC_LOG(LOG_TRACE, "4. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(7));

	DCC_LOG(LOG_TRACE, "5. iodrv_init()");
	iodrv_init();

	DCC_LOG(LOG_TRACE, "5. stdio_init()");
	stdio_init();

	DCC_LOG(LOG_TRACE, "6. trace_init()");
	trace_init();

	DCC_LOG(LOG_TRACE, "9. supervisor_init()");
	supervisor_init();

	led_set_rate(LED_X1, RATE_2BPS);

	DCC_LOG(LOG_TRACE, "10. net_init()");
	net_init();

	return 0;
}

