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

#include "board.h"

/* Pin 1 */

void pin1_sel_input(void)
{
	stm32_gpio_mode(USART2_RTS, INPUT, SPEED_LOW | PULL_UP);
	stm32_gpio_mode(USART3_RX, INPUT, SPEED_LOW | PULL_UP);
}

void pin1_sel_vcc(void)
{
	stm32_gpio_mode(USART2_RTS, INPUT, SPEED_LOW);
	stm32_gpio_set(USART2_RTS);
	stm32_gpio_set(USART3_RX);
	stm32_gpio_mode(USART2_RTS, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(USART3_RX, OUTPUT, PUSH_PULL | SPEED_LOW);
}

void pin1_sel_gnd(void)
{
	stm32_gpio_mode(USART2_RTS, INPUT, SPEED_LOW);
	stm32_gpio_clr(USART2_RTS);
	stm32_gpio_clr(USART3_RX);
	stm32_gpio_mode(USART2_RTS, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(USART3_RX, OUTPUT, PUSH_PULL | SPEED_LOW);
}

void pin1_sel_usart3_rx(void)
{
	stm32_gpio_mode(USART2_RTS, INPUT, PULL_UP);
	stm32_gpio_mode(USART3_RX, INPUT, PULL_UP);
}

void pin1_sel_i2c_sda(void)
{
	stm32_gpio_mode(USART2_RTS, INPUT, PULL_UP);
	stm32_gpio_mode(USART3_RX, ALT_FUNC, OPEN_DRAIN | PULL_UP);
}

/* Pin 2 */

void pin2_sel_input(void)
{
	stm32_gpio_mode(USART2_CK, INPUT, SPEED_LOW);
	stm32_gpio_mode(USART3_TX, INPUT, SPEED_LOW);
	stm32_gpio_mode(EXTRST1_IO, INPUT, SPEED_LOW);
}

void pin2_sel_open_drain(void)
{
	stm32_gpio_mode(USART2_CK, INPUT, 0);
	stm32_gpio_mode(USART3_TX, INPUT, 0);
	stm32_gpio_mode(EXTRST1_IO, OUTPUT, OPEN_DRAIN | PULL_UP);
}

void pin2_sel_vcc(void)
{
	stm32_gpio_mode(USART2_CK, INPUT, 0);
	stm32_gpio_mode(USART3_TX, INPUT, 0);
	stm32_gpio_set(USART2_CK);
	stm32_gpio_set(USART3_TX);
	stm32_gpio_set(EXTRST1_IO);
	stm32_gpio_mode(USART2_CK, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(USART3_TX, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(EXTRST1_IO, OUTPUT, PUSH_PULL | SPEED_LOW);
}

void pin2_sel_gnd(void)
{
	stm32_gpio_mode(USART2_CK, INPUT, 0);
	stm32_gpio_mode(USART3_TX, INPUT, 0);
	stm32_gpio_clr(USART2_CK);
	stm32_gpio_clr(USART3_TX);
	stm32_gpio_clr(EXTRST1_IO);
	stm32_gpio_mode(USART2_CK, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(USART3_TX, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(EXTRST1_IO, OUTPUT, PUSH_PULL | SPEED_LOW);
}

void pin2_sel_usart3_tx(void)
{
	stm32_gpio_mode(USART2_CK, INPUT, 0);
	stm32_gpio_mode(EXTRST1_IO, INPUT, 0);
	stm32_gpio_mode(USART3_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
}

void pin2_sel_i2c_scl(void)
{
	stm32_gpio_mode(USART2_CK, INPUT, 0);
	stm32_gpio_mode(EXTRST1_IO, INPUT, 0);
	stm32_gpio_mode(USART3_TX, ALT_FUNC, OPEN_DRAIN | PULL_UP);
}


/* Pin 4 */

void pin4_sel_input(void)
{
	stm32_gpio_mode(USART2_RX, INPUT, 0);
	stm32_gpio_mode(USART1_RX, INPUT, 0);
}

void pin4_sel_vcc(void)
{
	stm32_gpio_mode(USART2_RX, INPUT, 0);
	stm32_gpio_set(USART2_RX);
	stm32_gpio_set(USART1_RX);
	stm32_gpio_mode(USART2_RX, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(USART1_RX, OUTPUT, PUSH_PULL | SPEED_LOW);
}

void pin4_sel_gnd(void)
{
	stm32_gpio_mode(USART2_RX, INPUT, 0);
	stm32_gpio_clr(USART2_RX);
	stm32_gpio_clr(USART1_RX);
	stm32_gpio_mode(USART2_RX, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(USART1_RX, OUTPUT, PUSH_PULL | SPEED_LOW);
}

void pin4_sel_usart2_rx(void)
{
	stm32_gpio_mode(USART2_RX, INPUT, PULL_UP);
	stm32_gpio_mode(USART1_RX, INPUT, PULL_UP);
}

/* Pin 5 */

void pin5_sel_input(void)
{
	stm32_gpio_mode(USART2_TX, INPUT, 0);
	stm32_gpio_mode(USART1_TX, INPUT, 0);
}

void pin5_sel_vcc(void)
{
	stm32_gpio_mode(USART2_TX, INPUT, 0);
	stm32_gpio_set(USART2_TX);
	stm32_gpio_set(USART1_TX);
	stm32_gpio_mode(USART2_TX, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(USART1_TX, OUTPUT, PUSH_PULL | SPEED_LOW);
}

void pin5_sel_gnd(void)
{
	stm32_gpio_mode(USART2_TX, INPUT, 0);
	stm32_gpio_clr(USART2_TX);
	stm32_gpio_clr(USART1_TX);
	stm32_gpio_mode(USART2_TX, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_mode(USART1_TX, OUTPUT, PUSH_PULL | SPEED_LOW);
}

void pin5_sel_usart2_tx(void)
{
	stm32_gpio_mode(USART1_TX, INPUT, 0);
	stm32_gpio_mode(USART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
}

void pin5_sel_usart1_tx(void)
{
	stm32_gpio_mode(USART2_TX, INPUT, 0);
	stm32_gpio_mode(USART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
}



/* USART1 and USART2 pins are connected together.
   Only one TX pin must be enable at any time */

/* Select USART2 TX */
void io_sel_usart2(void)
{
	stm32_gpio_mode(USART1_TX, INPUT, 0);
	stm32_gpio_mode(USART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
}

/* Select USART1 TX */
void io_sel_usart1(void)
{
	stm32_gpio_mode(USART2_TX, INPUT, 0);
	stm32_gpio_mode(USART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
}

void usb_vbus(bool on)
{
//	if (on)
//		stm32_gpio_set(USB_FS_VBUS);
//	else
//		stm32_gpio_clr(USB_FS_VBUS);

	if (on)
		stm32_gpio_mode(USB_FS_VBUS, OUTPUT, PUSH_PULL | SPEED_LOW);
	else
		stm32_gpio_mode(USB_FS_VBUS, INPUT, 0);
}

void io_init(void)
{
	struct stm32_rcc * rcc = STM32_RCC;

	stm32_gpio_clock_en(STM32_GPIOA);
	stm32_gpio_clock_en(STM32_GPIOB);

	/* Enable Alternate Functions IO clock */
	rcc->apb2enr |= RCC_AFIOEN;

	/* UART1 TX (disabled) */
	stm32_gpio_mode(USART1_TX, INPUT, 0);
	/* UART1 RX */
	stm32_gpio_mode(USART1_RX, INPUT, PULL_UP);

	/* Primary UART TX */
	stm32_gpio_mode(USART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	/* Primary UART RX */
	stm32_gpio_mode(USART2_RX, INPUT, PULL_UP);

	/* Secondary UART TX */
	stm32_gpio_mode(USART3_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	/* Secondary UART RX */
	stm32_gpio_mode(USART3_RX, INPUT, PULL_UP);

	/* Push button */
	stm32_gpio_mode(PUSHBTN_IO, INPUT, PULL_UP);

	/* External Reset pins */
	stm32_gpio_mode(EXTRST0_IO, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_set(EXTRST0_IO);

	stm32_gpio_mode(EXTRST1_IO, INPUT, 0);
	stm32_gpio_set(EXTRST1_IO);

	stm32_gpio_mode(USB_FS_VBUS, INPUT, 0);
	stm32_gpio_set(USB_FS_VBUS);
}

