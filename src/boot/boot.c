/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file boot.c
 * @brief YARD-ICE bootloader main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdbool.h>

#include <sys/stm32f.h>
#include <sys/delay.h>

#define __THINKOS_DBGMON__
#include <thinkos/dbgmon.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#include "board.h"
#include "version.h"

void monitor_task(struct dmon_comm * comm, void * param);

void __attribute__((naked, noreturn)) cm3_hard_fault_isr(void)
{
	DCC_LOG(LOG_ERROR, "!");
//	udelay(1000000);
	cm3_sysrst();
}

#if 0
static inline void __stm32_gpio_af(struct stm32_gpio * gpio, int pin, int af)
{
	uint32_t tmp;

	if (pin < 8) {
		tmp = gpio->afrl;
		tmp &= ~GPIO_AFRL_MASK(pin);
		tmp |= GPIO_AFRL_SET(pin, af);
		gpio->afrl = tmp;
	} else {
		tmp = gpio->afrh;
		tmp &= ~GPIO_AFRH_MASK(pin);
		tmp |= GPIO_AFRH_SET(pin, af);
		gpio->afrh = tmp;
	}

	/* select alternate function pin mode */
	tmp = (gpio->moder & ~GPIO_MODE_MASK(pin)) | GPIO_MODE_ALT_FUNC(pin);
	gpio->moder = tmp;
}
#endif

void board_init(void)
{
	struct stm32_gpio * gpio = STM32_GPIOA;
	struct stm32_rcc * rcc = STM32_RCC;
//	struct cm3_scb * scb = CM3_SCB;

	DCC_LOG(LOG_TRACE, "...");

	/* Disable exceptions */
//	scb->shcsr = 0;

	/* Reset all peripherals except USB_OTG and GPIOA */
	rcc->ahb1rstr = ~((1 << RCC_GPIOA) | (1 << RCC_GPIOC)); 
	rcc->ahb2rstr = ~(1 << RCC_OTGFS);
	rcc->apb1rstr = ~(0);
	rcc->apb2rstr = ~(0);
	/* clear reset bits */
	rcc->ahb1rstr = 0;
	rcc->ahb2rstr = 0;
	rcc->apb1rstr = 0;
	rcc->apb2rstr = 0;
	/* disable all peripherals clock sources except USB_OTG and GPIOA */
	rcc->ahb1enr = (1 << RCC_GPIOA); 
	rcc->ahb2enr = (1 << RCC_OTGFS);
	rcc->apb1enr = 0;
	rcc->apb2enr = 0;

	/* select alternate functions to USB pins ... */
	gpio->afrh = GPIO_AFRH_SET(11, GPIO_AF10) | GPIO_AFRH_SET(12, GPIO_AF10);
	gpio->moder = GPIO_MODE_ALT_FUNC(11) | GPIO_MODE_ALT_FUNC(12) ;
#if 0
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);

	/* - Relay ------------------------------------------------------------*/
	stm32_gpio_mode(IO_RELAY, OUTPUT, SPEED_LOW);
	stm32_gpio_clr(IO_RELAY);

	/* - External Power ---------------------------------------------------*/
	stm32_gpio_mode(IO_PWR_EN, OUTPUT, SPEED_LOW);
	stm32_gpio_clr(IO_PWR_EN);
	stm32_gpio_mode(IO_PWR_MON, INPUT, SPEED_LOW | PULL_UP);
	/* - Debug UART ---------------------------------------------------*/
	stm32_gpio_mode(IO_UART5_TX, OUTPUT, SPEED_LOW);
	stm32_gpio_clr(IO_UART5_TX);
	stm32_gpio_mode(IO_UART5_TX, INPUT, SPEED_LOW | PULL_UP);

//#define IO_JTRST          STM32_GPIOB, 4
#endif
//	stm32_gpio_mode(OTG_FS_DP, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
//	stm32_gpio_mode(OTG_FS_DM, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
//	stm32_gpio_mode(OTG_FS_VBUS, ALT_FUNC, SPEED_LOW);


//	__stm32_gpio_af(OTG_FS_DP, GPIO_AF10);
//__stm32_gpio_af(OTG_FS_DM, GPIO_AF10);
//	stm32_gpio_af(OTG_FS_VBUS, GPIO_AF10);

	/* Adjust USB OTG FS interrupts priority */
	cm3_irq_pri_set(STM32F_IRQ_OTG_FS, MONITOR_PRIORITY);
	/* Enable USB OTG FS interrupts */
	cm3_irq_enable(STM32F_IRQ_OTG_FS);
}

#define MONITOR_AUTOBOOT 1
#define MONITOR_SHELL 2

#define CTRL_C  0x03 /* ETX */

void main(int argc, char ** argv)
{
	struct dmon_comm * comm;
	uint32_t flags = 0;
	char buf[1];
	int i;

	DCC_LOG_INIT();
#if DEBUG > 2
	DCC_LOG_CONNECT();
#endif

#ifndef UDELAY_FACTOR 
	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate().");
	cm3_udelay_calibrate();
#endif

	DCC_LOG(LOG_TRACE, "2. thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

#if THINKOS_ENABLE_MPU
	DCC_LOG(LOG_TRACE, "3. thinkos_mpu_init()");
	thinkos_mpu_init(0x0c00);
#endif

	DCC_LOG(LOG_TRACE, "4. board_init().");
	board_init();

	/* Wait for the other power supply and subsystems to stabilize */
	DCC_LOG(LOG_TRACE, "5. thinkos_sleep().");
	thinkos_sleep(100);

	DCC_LOG(LOG_TRACE, "6. thinkos_console_init()");
	thinkos_console_init();

	DCC_LOG(LOG_TRACE, "7. usb_comm_init()");
	comm = usb_comm_init(&stm32f_otg_fs_dev);

	DCC_LOG(LOG_TRACE, "8. thinkos_dbgmon()");

	/* starts monitor with shell enabled */
	thinkos_dbgmon(monitor_task, comm, (void *)MONITOR_SHELL);
	flags = MONITOR_AUTOBOOT;

	for (i = 0; ; ++i) {
		if (thinkos_console_timedread(buf, 1, 500) == 1) {
			flags |= MONITOR_SHELL; 
			if (CTRL_C == buf[0]) {
				/* disable autoboot */
				flags &= ~MONITOR_AUTOBOOT; 
				thinkos_console_write("^C\r\n", 4);
			} else {
				/* echo back */
				thinkos_console_write(buf, 1);
			}
		} else if (flags & MONITOR_AUTOBOOT) {
			if (i < 9) {
				if (dmon_comm_isconnected(comm))
					thinkos_console_write(".", 1);
			} else
				break;
		}
		if (stm32_gpio_stat(IO_JTRST) == 0) {
			thinkos_console_write("TRST\r\n", 6);
		}
	}

#if THINKOS_ENABLE_MPU
	DCC_LOG(LOG_TRACE, "9. thinkos_userland()");
	thinkos_userland();
#endif

	/* starts/restarts monitor with autoboot enabled */
	thinkos_dbgmon(monitor_task, comm, (void *)flags);

	DCC_LOG(LOG_TRACE, "10. thinkos_thread_abort()");
	thinkos_thread_abort(0);

	DCC_LOG(LOG_ERROR, "11. unreachable code reched!!!");
}

