/* 
 * File:	stm32f-gpio.c
 * Author:  Robinson Mittmann (bobmittmann@gmail.com)
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

const struct stm32f_gpio * stm32f_gpio_lut[] = {
	STM32F_GPIOA,
	STM32F_GPIOB,
	STM32F_GPIOC,
	STM32F_GPIOD,
	STM32F_GPIOE,
	STM32F_GPIOF,
	STM32F_GPIOG,
	STM32F_GPIOH,
	STM32F_GPIOI
};

int stm32f_gpio_id(struct stm32f_gpio * gpio)
{
	uint32_t base = (uint32_t)gpio;

	return (base - STM32F_BASE_GPIOA) / 0x400;
}

void stm32f_gpio_clock_en(struct stm32f_gpio * gpio)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

	rcc->ahb1enr |= (1 << stm32f_gpio_id(gpio));
}

void stm32f_gpio_mode(struct stm32f_gpio * gpio, 
					  unsigned int port, unsigned int mode, unsigned int opt)
{
	uint32_t tmp;
	uint32_t moder; 

	/* set the port mode */
	moder = gpio->moder;
	moder &= ~GPIO_MODE_MASK(port);
	switch (mode & 0x0f) {
	case INPUT:
		moder |= GPIO_MODE_INPUT(port);
		break;
	case OUTPUT:
		moder |= GPIO_MODE_OUTPUT(port);
		break;
	case ALT_FUNC:
		moder |= GPIO_MODE_ALT_FUNC(port);
		break;
	case ANALOG:
		moder |= GPIO_MODE_ANALOG(port);
		break;
	}
	gpio->moder = moder;

	/* set the port output speed */
	tmp = gpio->ospeedr;
	tmp &= ~GPIO_OSPEED_MASK(port);
	tmp |= OPT_SPEED(opt) << (2 * port);
	gpio->ospeedr = tmp;

	/* set the port output type */
	tmp = gpio->otyper;
	tmp &= ~GPIO_OTYPE_MASK(port);
	if (opt & OPEN_DRAIN)
		tmp |= GPIO_OPEN_DRAIN(port);
	else
		tmp |= GPIO_PUSH_PULL(port);
	gpio->otyper = tmp;

	/* set the port pull up / pull down resistors */
	tmp = gpio->pupdr;
	tmp &= ~GPIO_PULL_MASK(port);
	if (opt & PULL_UP)
		tmp |= GPIO_PULL_UP(port);
	else if (opt & PULL_DOWN)
		tmp |= GPIO_PULL_DOWN(port);
	gpio->pupdr = tmp;

}

void stm32f_gpio_af(struct stm32f_gpio * gpio, int port, int af)
{
	uint32_t tmp;

	if (port < 8) {
		tmp = gpio->afrl;
		tmp &= ~GPIO_AFRL_MASK(port);
		tmp |= GPIO_AFRL_SET(port, af);
		gpio->afrl = tmp;
	} else {
		tmp = gpio->afrh;
		tmp &= ~GPIO_AFRH_MASK(port);
		tmp |= GPIO_AFRH_SET(port, af);
		gpio->afrh = tmp;
	}
}

