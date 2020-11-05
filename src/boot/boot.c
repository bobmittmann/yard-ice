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

#include "monitor-i.h"

#include <sys/stm32f.h>
#include <sys/delay.h>

#include <sys/dcclog.h>

#include "board.h"
#include "version.h"

#ifndef RELAY_CHATTER_ENABLE
#define RELAY_CHATTER_ENABLE 0
#endif

void board_init(void)
{
	struct stm32_gpio * gpio = STM32_GPIOA;
	struct stm32_rcc * rcc = STM32_RCC;

#if DEBUG
	DCC_LOG(LOG_TRACE, "Reset all peripherals ...");
	udelay(0x10000);
#endif
	/* Reset all peripherals except USB_OTG and GPIOA */
	rcc->ahb1rstr = ~(1 << RCC_GPIOA);
	rcc->ahb2rstr = ~(1 << RCC_OTGFS);
	rcc->apb1rstr = ~(0);
	rcc->apb2rstr = ~(0);
	/* clear reset bits */
#if DEBUG
	DCC_LOG(LOG_TRACE, "Clear reset bits...");
	udelay(0x10000);
#endif
	rcc->ahb1rstr = 0;
	rcc->ahb2rstr = 0;
	rcc->apb1rstr = 0;
	rcc->apb2rstr = 0;

	/* disable all peripherals clock sources except USB_OTG, GPIOA, GPIOB,
	   GPIOC and GPIOD */
#if DEBUG
	DCC_LOG(LOG_TRACE, "Disable peripheral clock sources ...");
	udelay(0x10000);
#endif
	rcc->ahb1enr = (1 << RCC_GPIOA) | (1 << RCC_GPIOB)  |
				   (1 << RCC_GPIOB) | (1 << RCC_GPIOC) | (1 << RCC_GPIOD); 
	rcc->ahb2enr = (1 << RCC_OTGFS);
	rcc->apb1enr = 0;
	rcc->apb2enr = 0;

#if DEBUG
	DCC_LOG(LOG_TRACE, "Select alternate functions to USB pins...");
	udelay(0x40000);
#endif

#if 1
	/* select alternate functions to USB pins ... */
	gpio->afrh = GPIO_AFRH_SET(12, GPIO_AF10) | GPIO_AFRH_SET(11, GPIO_AF10) |
		GPIO_AFRH_SET(9, GPIO_AF10);
	gpio->ospeedr = GPIO_OSPEED_HIGH(12) | GPIO_OSPEED_HIGH(11) |
		GPIO_OSPEED_LOW(9);
	gpio->otyper = GPIO_PUSH_PULL(12) | GPIO_PUSH_PULL(11);
	gpio->pupdr = 0;
	gpio->moder &= ~(GPIO_MODE_MASK(12) | GPIO_MODE_MASK(11) | 
					 GPIO_MODE_MASK(9));
	gpio->moder |= GPIO_MODE_ALT_FUNC(12) | GPIO_MODE_ALT_FUNC(11) | 
		GPIO_MODE_ALT_FUNC(9);

	DCC_LOG3(LOG_TRACE, "OTYPER=0x%08X OSPEEDR=0x%08X MODER=0x%08x", 
			 gpio->otyper, gpio->ospeedr, gpio->moder);
#else
	stm32_gpio_af(OTG_FS_DP, GPIO_AF10);
 	stm32_gpio_af(OTG_FS_DM, GPIO_AF10);
	stm32_gpio_af(OTG_FS_VBUS, GPIO_AF10);
	stm32_gpio_mode(OTG_FS_DP, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(OTG_FS_DM, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(OTG_FS_VBUS, ALT_FUNC, SPEED_LOW);
#endif

#if 0
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);

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
#endif

#if DEBUG
  #if RELAY_CHATTER_ENABLE
	DCC_LOG(LOG_TRACE, "Relay chatter ...");
	udelay(0x40000);
	/* - Relay ------------------------------------------------------------*/
	stm32_gpio_mode(IO_RELAY, OUTPUT, SPEED_LOW);
	stm32_gpio_clr(IO_RELAY);
	thinkos_sleep(256);
	stm32_gpio_set(IO_RELAY);
	thinkos_sleep(256);
	stm32_gpio_clr(IO_RELAY);
	thinkos_sleep(256);
	stm32_gpio_set(IO_RELAY);
	thinkos_sleep(256);
	stm32_gpio_clr(IO_RELAY);
  #endif
#endif
}

void board_reset(void)
{
#if DEBUG
	DCC_LOG(LOG_TRACE, "USB irq configure...");
	udelay(0x10000);
#endif
	/* Adjust USB OTG FS interrupts priority */
	cm3_irq_pri_set(STM32F_IRQ_OTG_FS, MONITOR_PRIORITY);
	/* Enable USB OTG FS interrupts */
	cm3_irq_enable(STM32F_IRQ_OTG_FS);
}


void main(int argc, char ** argv)
{
	const struct monitor_comm * comm;
	uint32_t flags = 0;
	char buf[1];
	int thread;
	int i;

	DCC_LOG_INIT();

#ifndef UDELAY_FACTOR 
	cm3_udelay_calibrate();
#endif

#if DEBUG
	DCC_LOG_CONNECT();
	mdelay(100);
	DCC_LOG(LOG_INFO, "______________________________________________________");
	DCC_LOG(LOG_INFO, "_________________ ! Board start up ! _________________");
	DCC_LOG(LOG_INFO, "______________________________________________________");
	mdelay(100);
#endif

#if DEBUG
	DCC_LOG1(LOG_TRACE, "1. udelay_factor=%d.", udelay_factor);
#endif

#if DEBUG
	DCC_LOG(LOG_TRACE, "2. thinkos_krn_init().");
	udelay(0x10000);
#endif
	thread = thinkos_krn_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0) |
	                 THINKOS_OPT_PRIVILEGED, NULL, NULL);
	(void)thread;
	DCC_LOG4(LOG_TRACE, "<%d> MSP=%08x PSP=%08x CTRL=%02x", 
			 thread, cm3_msp_get(), cm3_psp_get(), cm3_control_get());

#if DEBUG
	DCC_LOG(LOG_TRACE, "3. board_init().");
	udelay(0x10000);
#endif
	board_init();

	/* Wait for the other power supply and subsystems to stabilize */
	DCC_LOG(LOG_TRACE, "4. thinkos_sleep().");
	thinkos_sleep(100);

#if DEBUG
	DCC_LOG(LOG_TRACE, "5. usb_comm_init()");
	udelay(0x40000);
#endif

	comm = usb_comm_init(&stm32f_otg_fs_dev);

#if THINKOS_ENABLE_CONSOLE
#if DEBUG
	DCC_LOG(LOG_TRACE, "6. thinkos_console_init()");
	udelay(0x40000);
#endif
	thinkos_krn_console_init();
#endif

#if THINKOS_ENABLE_MPU
#if DEBUG
	DCC_LOG(LOG_TRACE, "6. thinkos_mpu_init()");
	udelay(1024);
#endif
	thinkos_mpu_init(0x0c00);
#endif

	board_reset();

	DCC_LOG(LOG_TRACE, "8. thinkos_monitor()");

	if (stm32_gpio_stat(IO_JTRST) == 0) {
		DCC_LOG(LOG_TRACE, "SHELL | AUTOBOOT");
		/* If  jumper is inserted between JTRST and GND boot up now
		   and enable the monitor shell. */
		flags = MONITOR_SHELL | MONITOR_AUTOBOOT;
	} else {
		flags = MONITOR_SHELL | MONITOR_AUTOBOOT;
		/* starts monitor with shell enabled */
		thinkos_krn_monitor_init(comm, monitor_task, (void *)MONITOR_SHELL);
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
					if (monitor_comm_isconnected(comm))
						thinkos_console_write(".", 1);
				} else
					break;
			}
		}
	}

	/* starts/restarts monitor with autoboot enabled */
	thinkos_krn_monitor_init(comm, monitor_task, (void *)flags);

#if DEBUG
	__thinkos(&thinkos_rt);
#endif

	DCC_LOG(LOG_TRACE, "10. thinkos_abort()");
	thinkos_abort();

	DCC_LOG(LOG_ERROR, "11. unreachable code reched!!!");
}

