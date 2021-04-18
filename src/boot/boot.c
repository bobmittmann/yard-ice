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
#include <vt100.h>

#include "board.h"
#include "version.h"

#ifndef RELAY_CHATTER_ENABLE
#define RELAY_CHATTER_ENABLE 0
#endif

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

int board_app_err(void)
{
	DCC_LOG(LOG_ERROR, "11. boot_app_err!!!");

	return 0;
}

bool board_integrity_check(void)
{
    uint32_t tick;

    for (tick = 0; tick < 10; ++tick) {
        thinkos_sleep(500);
    }

    return true;
}

uintptr_t board_app_get(void) 
{
	uintptr_t addr = APPLICATION_BLOCK_OFFS;
	const struct magic_blk * magic = &board_app_magic;
	uint32_t * signature = (uint32_t *)addr;
	struct flat_app  * hdr = (struct flat_app *)addr;
	int i;

	for (i = 0; i < magic->hdr.cnt; ++i) {
		if ((signature[i] & magic->rec[i].mask) != magic->rec[i].comp) {
			DCC_LOG3(LOG_ERROR, "%d  %08x %08x", i, signature[i], 
					 magic->rec[i].comp);
			return (uintptr_t)board_app_err;
		}
	}

	return hdr->entry;
}

//#void __attribute__((noreturn)) 
void main(int argc, char ** argv)
{
	const struct monitor_comm * comm;
	uint32_t flags = 0;
	uintptr_t entry;
	int (* app)(void);


#if DEBUG
    DCC_LOG_INIT();
    DCC_LOG_CONNECT();
    mdelay(25);

    DCC_LOG(LOG_TRACE, "\n\n" VT_PSH VT_BRI VT_FBL);
    DCC_LOG(LOG_TRACE, "*************************************************");
    DCC_LOG(LOG_TRACE, "*     YardICE ThinkOS Custom Bootloader         *");
    DCC_LOG(LOG_TRACE, "*************************************************"
            VT_POP "\n\n");
    mdelay(25);

    DCC_LOG(LOG_TRACE, VT_PSH VT_BRI VT_FGR
            "* 1. thinkos_krn_init()." VT_POP);
    mdelay(25);
#endif

	thinkos_krn_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0) |
					 THINKOS_OPT_PRIVILEGED, NULL, NULL);

#if DEBUG
    DCC_LOG(LOG_TRACE, VT_PSH VT_BRI VT_FGR
            "* 2. board_init()." VT_POP);
    mdelay(25);
#endif
	board_init();
	board_reset();

#if DEBUG
	DCC_LOG(LOG_TRACE, VT_PSH VT_BRI VT_FGR
			"* 3. usb_comm_init()." VT_POP);
#endif
	comm = usb_comm_init(&stm32f_otg_fs_dev);

#if DEBUG
    DCC_LOG(LOG_TRACE, VT_PSH VT_BRI VT_FGR
            "* 4. thinkos_krn_irq_on()." VT_POP);
#endif
    /* enable interrupts */
    thinkos_krn_irq_on();

#if 0
	if (stm32_gpio_stat(IO_JTRST) == 0) {
		DCC_LOG(LOG_TRACE, "SHELL | AUTOBOOT");
		/* If  jumper is inserted between JTRST and GND boot up now
		   and enable the monitor shell. */
		flags = MONITOR_SHELL | MONITOR_AUTOBOOT;
	} else {
		char buf[1];
		int i;

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
#endif

#if DEBUG
    DCC_LOG(LOG_TRACE, VT_PSH VT_BRI VT_FGR
            "* 5. thinkos_krn_monitor_init()." VT_POP);
#endif
	/* starts/restarts monitor with autoboot enabled */
	thinkos_krn_monitor_init(comm, monitor_task, (void *)flags);

#if DEBUG
    DCC_LOG(LOG_TRACE, VT_PSH VT_BRI VT_FGR
            "* 6. board_integrity_check()..." VT_POP);
    mdelay(25);
#endif
    if (!board_integrity_check()) {
        DCC_LOG(LOG_ERROR, VT_PSH VT_BRI VT_FRD
                "**** board_integrity_check() failed." VT_POP);
#if DEBUG
        mdelay(10000);
#endif
		entry = (uintptr_t)board_app_err;
    } else {
#if DEBUG
		DCC_LOG(LOG_TRACE, VT_PSH VT_BRI VT_FGR
				"* 8. boot_run_app()..." VT_POP);
		mdelay(25);
#endif
		entry = board_app_get();
	}

	app = (int (*)(void))(entry);

	app();
//	for(;;);
}

