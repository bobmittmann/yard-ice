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

#if defined(STM32L1X)

#else

const struct stm32_gpio * const stm32_gpio_lut[] = {
	STM32_GPIOA,
	STM32_GPIOB,
	STM32_GPIOC,
	STM32_GPIOD,
	STM32_GPIOE,
	STM32_GPIOF,
#ifdef STM32F_GPIOG
	STM32_GPIOG,
#endif
#ifdef STM32F_GPIOH
	STM32_GPIOH,
#endif
#ifdef STM32F_GPIOI
	STM32_GPIOI
#endif
};

int stm32_gpio_id(struct stm32_gpio * gpio)
{
	uint32_t base = (uint32_t)gpio;

	return (base - STM32_BASE_GPIOA) / 0x400;
}

void stm32_gpio_clock_en(struct stm32_gpio * gpio)
{
	struct stm32_rcc * rcc = STM32_RCC;

#if defined(STM32F2X) || defined(STM32F4X)
	rcc->ahb1enr |= 1 << stm32_gpio_id(gpio);
#endif

#if defined(STM32F3X) 
	rcc->ahbenr |= 1 << (stm32_gpio_id(gpio) + 17);
#endif
	
#if defined(STM32F1X)
	rcc->apb2enr |= 1 << (stm32_gpio_id(gpio) + 2);
#endif

}
#endif

void stm32_gpio_mode(struct stm32_gpio * gpio, 
					  unsigned int pin, unsigned int mode, unsigned int opt)
{
#if defined(STM32F2X) || defined(STM32F3X) || defined(STM32F4X) || \
	defined(STM32L1X)

	uint32_t tmp;
	uint32_t moder; 

	/* set the pin mode */
	moder = gpio->moder;
	moder &= ~GPIO_MODE_MASK(pin);
	switch (mode & 0x0f) {
	case INPUT:
		moder |= GPIO_MODE_INPUT(pin);
		break;
	case OUTPUT:
		moder |= GPIO_MODE_OUTPUT(pin);
		break;
	case ALT_FUNC:
		moder |= GPIO_MODE_ALT_FUNC(pin);
		break;
	case ANALOG:
		moder |= GPIO_MODE_ANALOG(pin);
		break;
	}
	gpio->moder = moder;

	/* set the pin output speed */
	tmp = gpio->ospeedr;
	tmp &= ~GPIO_OSPEED_MASK(pin);
	tmp |= OPT_SPEED(opt) << (2 * pin);
	gpio->ospeedr = tmp;

	/* set the pin output type */
	tmp = gpio->otyper;
	tmp &= ~GPIO_OTYPE_MASK(pin);
	if (opt & OPEN_DRAIN)
		tmp |= GPIO_OPEN_DRAIN(pin);
	else
		tmp |= GPIO_PUSH_PULL(pin);
	gpio->otyper = tmp;

	/* set the pin pull up / pull down resistors */
	tmp = gpio->pupdr;
	tmp &= ~GPIO_PULL_MASK(pin);
	if (opt & PULL_UP)
		tmp |= GPIO_PULL_UP(pin);
	else if (opt & PULL_DOWN)
		tmp |= GPIO_PULL_DOWN(pin);
	gpio->pupdr = tmp;
#endif

#if defined(STM32F1X)
	uint32_t cnf = 0;
	uint32_t mod = 0; 

	switch (mode) {
	case ALT_FUNC:
		cnf |= 0x2;
		/* Fall through */
	case OUTPUT:
		if (opt & OPEN_DRAIN)
			cnf |= 0x1;

		switch (OPT_SPEED(opt)) {
		case SPEED_LOW:
			mod = 0x2;
			break;
		case SPEED_MED:
		case SPEED_FAST:
			mod = 0x1;
			break;
		case SPEED_HIGH:
			mod = 0x3;
			break;
		}
		break;
	case INPUT:
		if (opt & PULL_UP) {
			cnf = 0x2;
			gpio->odr |= 1 << pin;
		} else if (opt & PULL_DOWN) {
			cnf = 0x2;
			gpio->odr &= ~(1 << pin);
		} else
			cnf = 0x1;
		mod = 0x0;
		break;
	case ANALOG:
		cnf = 0x0;
		mod = 0x0;
		break;
	}

	if (pin < 8) {
		gpio->crl &= ~(0xf << (pin * 4));
		gpio->crl |= ((cnf << 2) | mod) << (pin * 4);
	} else {
		gpio->crh &= ~(0xf << ((pin - 8) * 4));
		gpio->crh |= ((cnf << 2) | mod) << ((pin - 8) * 4);
	}

#endif

}

#if defined(STM32F2X) || defined(STM32F3X) || defined(STM32F4X) || \
	defined(STM32L1X)
void stm32_gpio_af(struct stm32_gpio * gpio, int pin, int af)
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
}
#endif

