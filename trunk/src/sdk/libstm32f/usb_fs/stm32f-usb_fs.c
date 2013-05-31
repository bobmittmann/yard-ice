/* 
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
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
 * @file stm32f-usb_fs.c
 * @brief YARD-ICE UART console
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/dcclog.h>

#if defined(STM32F103) || defined(STM32F30X)

#define USB_FS_DP STM32F_GPIOA, 12
#define USB_FS_DM STM32F_GPIOA, 11
#define USB_FS_VBUS STM32F_GPIOB, 12

void stm32f_usb_io_init(void)
{
	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);

	stm32f_gpio_mode(USB_FS_DP, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(USB_FS_DM, ALT_FUNC, PUSH_PULL | SPEED_HIGH);

	/* PB12: External Pull-up */
	stm32f_gpio_mode(USB_FS_VBUS, INPUT, 0);
	stm32f_gpio_set(USB_FS_VBUS);
}

void stm32f_usb_vbus_connect(bool connect)
{
	if (connect)
		stm32f_gpio_mode(USB_FS_VBUS, OUTPUT, PUSH_PULL | SPEED_LOW);
	else
		stm32f_gpio_mode(USB_FS_VBUS, INPUT, SPEED_LOW);
#if 0
	if (connect)
		stm32f_gpio_mode(USB_FS_VBUS, ALT_FUNC, SPEED_LOW);
	else
		stm32f_gpio_mode(USB_FS_VBUS, INPUT, 0);
#endif
}

void stm32f_usb_power_on(struct stm32f_usb * usb)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

	DCC_LOG(LOG_TRACE, "Enabling USB device clock...");

	stm32f_usb_vbus_connect(true);

	rcc->apb1enr |= RCC_USBEN;

	/* USB power ON */
	usb->cntr = USB_FRES;
	/* Wait tSTARTUP time ... */
	udelay(2);
	/* Removing the reset condition */
	usb->cntr = 0;

	/* Removing any spurious pending interrupts */
	usb->istr = 0;

	/* enable Cortex interrupts */
	cm3_irq_enable(STM32F_IRQ_USB_LP);

	cm3_irq_enable(STM32F_IRQ_USB_HP);

}

void stm32f_usb_power_off(struct stm32f_usb * usb)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

	usb->cntr = USB_FRES;
	/* Removing any spurious pending interrupts */
	usb->istr = 0;

	stm32f_usb_vbus_connect(false);

	usb->cntr = USB_FRES | USB_PDWN;

	DCC_LOG(LOG_TRACE, "Disabling USB device clock...");
	rcc->apb1enr &= ~RCC_USBEN;

	/* disabling IO pins */
	stm32f_gpio_mode(USB_FS_DP, INPUT, 0);
	stm32f_gpio_mode(USB_FS_DM, INPUT, 0);

}

#endif /* STM32F103 */
