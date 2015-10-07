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

#include <thinkos.h>
#include <sys/serial.h>
#include <crc.h>

#include "board.h"
#include "config.h"

void board_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_CRC);

	/* LEDs */
	stm32_gpio_mode(IO_LED1, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED2, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED3, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(IO_LED4, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_clr(IO_LED1);
	stm32_gpio_clr(IO_LED2);
	stm32_gpio_clr(IO_LED3);
	stm32_gpio_clr(IO_LED4);

	/* USART2_TX */
	stm32_gpio_mode(IO_UART_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART_TX, GPIO_AF7);
	/* USART2_RX */
	stm32_gpio_mode(IO_UART_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART_RX, GPIO_AF7);

#if 0
	/* Switches */
	stm32_gpio_mode(IO_SW1_UP,  INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(IO_SW1_DWN, INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(IO_SW2_UP,  INPUT, PULL_UP | SPEED_LOW);
	stm32_gpio_mode(IO_SW2_DWN, INPUT, PULL_UP | SPEED_LOW);

	/* Comparator */ 
	stm32_gpio_mode(IO_COMP, INPUT, SPEED_MED);

	/* Negative volatge supply */
	stm32_gpio_mode(IO_VNEG_SW, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(IO_VNEG_SW, GPIO_AF1);

	/* Current rate control */
	stm32_gpio_mode(IO_IRATE, ANALOG, 0);

	/* Current sink */
	stm32_gpio_mode(IO_SINK1, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(IO_SINK2, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(IO_SINK3, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(IO_SINK4, OUTPUT, PUSH_PULL | SPEED_MED);
	stm32_gpio_clr(IO_SINK1);
	stm32_gpio_clr(IO_SINK2);
	stm32_gpio_clr(IO_SINK3);
	stm32_gpio_clr(IO_SINK4);
#endif
}

/* -------------------------------------------------------------------------
 * Application execution
 * ------------------------------------------------------------------------- */

extern uint32_t _stack;
extern const struct thinkos_thread_inf thinkos_main_inf;

void board_soft_reset(void)
{
	DCC_LOG(LOG_TRACE, "1. disable all interrupts"); 
	__thinkos_irq_disable_all();

	DCC_LOG(LOG_TRACE, "2. kill all threads...");
	__thinkos_kill_all(); 

	DCC_LOG(LOG_TRACE, "4. ThinkOS reset...");
	__thinkos_reset();

	cm3_irq_enable(STM32_IRQ_USART2);
	cm3_irq_enable(STM32_IRQ_DMA1_CH7);
	cm3_irq_enable(STM32_IRQ_DMA1_CH6);
}

void board_reboot(int delay)
{
	DCC_LOG(LOG_TRACE, "thinkos_sleep()");
	thinkos_sleep(delay);
	DCC_LOG(LOG_TRACE, "cm3_sysrst()");
	cm3_sysrst();
}

void board_exec(void (* func)(int), int mode)
{
	int thread_id = 0;

	DCC_LOG(LOG_TRACE, "__thinkos_thread_abort()");
	__thinkos_thread_abort(thread_id);


	DCC_LOG1(LOG_TRACE, "__thinkos_thread_init(mode=%d)", mode);
	__thinkos_thread_init(thread_id, (uintptr_t)&_stack, func, (void *)mode);

#if THINKOS_ENABLE_THREAD_INFO
	__thinkos_thread_inf_set(thread_id, &thinkos_main_inf);
#endif

	DCC_LOG(LOG_TRACE, "__thinkos_thread_resume()");
	__thinkos_thread_resume(thread_id);

	DCC_LOG(LOG_TRACE, "__thinkos_defer_sched()");
	__thinkos_defer_sched();
}

bool board_app_exec(uint32_t addr, int mode)
{
	uint32_t * signature = (uint32_t *)addr;

	if ((signature[0] != 0x0a0de004) ||
		(signature[1] != 0x6e696854) ||
		(signature[2] != 0x00534f6b)) {
		DCC_LOG1(LOG_WARNING, "invalid application signature, addr=%p!", 
				 signature);

		return false;
	}

	board_exec((void *)(addr | 1), mode);

	return true;
}

void board_test(int mode)
{
	if (mode == APP_MODE_FAULT) {
		for (;;) {
			__led_on(IO_LED1);
//			__led_on(IO_LED2);
			__led_on(IO_LED3);
//			__led_on(IO_LED4);
			thinkos_sleep(100);
			__led_off(IO_LED1);
			__led_off(IO_LED2);
			__led_off(IO_LED3);
			__led_off(IO_LED4);
			thinkos_sleep(234);
		}
	}

	for (;;) {
		__led_on(IO_LED1);
		thinkos_sleep(25);
		__led_off(IO_LED3);
		thinkos_sleep(100);
		__led_on(IO_LED2);
		thinkos_sleep(25);
		__led_off(IO_LED1);
		thinkos_sleep(100);
		__led_on(IO_LED4);
		thinkos_sleep(25);
		__led_off(IO_LED2);
		thinkos_sleep(100);
		__led_on(IO_LED3);
		thinkos_sleep(25);
		__led_off(IO_LED4);
		thinkos_sleep(100);
	}	
}

int app_default(int) __attribute__((weak, alias("board_test")));


