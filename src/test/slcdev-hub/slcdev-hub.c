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

int console_shell(void);
void env_init(void);

void __attribute__((noreturn)) watchdog_task(void)
{
	unsigned int i;

	for (i = 0; ;++i) {
		thinkos_sleep(1000);
		DCC_LOG1(LOG_TRACE, "tick %d", i);
	}
}

FILE * monitor_stream;
bool monitor_auto_flush;

void __attribute__((noreturn)) monitor_task(void)
{
	int opt;

	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

	for (;;) {
		thinkos_sleep(250);

		if (monitor_auto_flush)
			opt = TRACE_FLUSH;      
		else
			opt = 0;

		if (trace_fprint(monitor_stream, opt) < 0) {
			/* fall back to stdout */
			monitor_stream = stdout;
		}
	}
}

uint32_t __attribute__((aligned(8))) monitor_stack[512];

void monitor_init(void)
{
	monitor_stream = stdout;
	monitor_auto_flush = false;

	thinkos_thread_create((void *)monitor_task, (void *)NULL,
						  monitor_stack, sizeof(monitor_stack), 
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));
}

uint32_t __attribute__((aligned(8))) watchdog_stack[32];

void watchdog_init(void)
{

	thinkos_thread_create((void *)watchdog_task, (void *)NULL,
						  watchdog_stack, sizeof(watchdog_stack), 
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));
}
	

const struct file stm32_uart_file = {
	.data = STM32_UART5, 
	.op = &stm32_usart_fops 
};

struct uart_console_dev * uart_console_init(unsigned int baudrate, 
											unsigned int flags);

struct file * uart_console_fopen(struct uart_console_dev * dev);

void stdio_init(void)
{
	struct uart_console_dev * dev;
	struct tty_dev * tty;
	FILE * f_tty;
	FILE * f_raw;

	dev = uart_console_init(115200, SERIAL_8N1);
	f_raw = uart_console_fopen(dev);
	tty = tty_attach(f_raw);
	f_tty = tty_fopen(tty);

	stderr = (struct file *)&stm32_uart_file;
	stdout = f_tty;
	stdin = f_tty;
}

int stdio_shell(void);

void __attribute__((noreturn)) console_shell_task(void)
{
	DCC_LOG1(LOG_TRACE, "thread=%d", thinkos_thread_self());

//	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

	for (;;) {
		stdio_shell();
	}
}

uint32_t __attribute__((aligned(8))) console_shell_stack[1024 + 256];

void console_shell_init(void)
{
	thinkos_thread_create((void *)console_shell_task, (void *)NULL,
						  console_shell_stack, sizeof(console_shell_stack), 
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));
}


int usb_shell(usb_cdc_class_t * cdc);

void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

#if 0
	/* JTAG TRST Pin */
	stm32_gpio_mode(MODSW, INPUT, SPEED_LOW);
#endif

	stm32_gpio_mode(UART5_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(UART5_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART5_RX, GPIO_AF8);
	stm32_gpio_af(UART5_TX, GPIO_AF8);

	/* USART1_TX */
	stm32_gpio_mode(UART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART1_TX, GPIO_AF7);
	/* USART1_RX */
	stm32_gpio_mode(UART1_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART1_RX, GPIO_AF7);

	/* USART6_TX */
	stm32_gpio_mode(UART6_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(UART6_TX, GPIO_AF7);
	/* USART6_RX */
	stm32_gpio_mode(UART6_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(UART6_RX, GPIO_AF7);

}

int main(int argc, char ** argv)
{
	usb_cdc_class_t * cdc;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "2. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "3. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(7));

	io_init();

	DCC_LOG(LOG_TRACE, "1. stdio_init()");
	stdio_init();
	printf("\n---\n");

	DCC_LOG(LOG_TRACE, "4. trace_init()");
	trace_init();

	DCC_LOG(LOG_TRACE, "5. env_init()");
	env_init();

	/* create some threads */
	DCC_LOG(LOG_TRACE, "6. monitor_init()");
	monitor_init();

	DCC_LOG(LOG_TRACE, "7. watchdog_init()");
	watchdog_init();

	DCC_LOG(LOG_TRACE, "7. console_shell_init()");
	console_shell_init();

	DCC_LOG(LOG_TRACE, "8. usb_cdc_init()");
	cdc = usb_cdc_init(&stm32f_otg_fs_dev, *((uint64_t *)STM32F_UID));

	DCC_LOG(LOG_TRACE, "9. usb_shell()");

	for (;;) {
		usb_shell(cdc);
	}
}

