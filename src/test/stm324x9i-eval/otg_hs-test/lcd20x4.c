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
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/dcclog.h>
#include <thinkos.h>

#include "board.h"

#define LCD_SET_DDRAM_ADDR (1 << 7)

#define LCD_FUNCTION_SET   (1 << 5)
#define LCD_DL             (1 << 4)
#define LCD_N              (1 << 3)
#define LCD_F              (1 << 2)

#define	LCD_ON_OFF_CTRL    (1 << 3)
#define LCD_DISPLAY        (1 << 2)
#define LCD_CURSOR         (1 << 1)
#define LCD_BLINK          (1 << 0)

#define	LCD_ENTRY_MODE_SET (1 << 2)

#define LCD_ID             (1 << 1)
#define LCD_SH             (1 << 0)

#define	LCD_DISPLAY_CLEAR  (1 << 0)

struct lcd_dev {
	int mutex;
};

static struct lcd_dev lcd20x4_dev;

#define LCD_TMR_FREQ 1000000

static void lcd_timer_init(void)
{
	struct stm32f_tim * tim = STM32F_TIM10;
	unsigned int div;

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM10);
	/* get the total divisior */
	div = (stm32f_tim2_hz + (LCD_TMR_FREQ / 2)) / LCD_TMR_FREQ;
	/* Timer configuration */
	tim->psc = div - 1;
	tim->arr = 0;
	tim->cnt = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS; 
	cm3_irq_pri_set(STM32F_IRQ_TIM10, IRQ_PRIORITY_HIGH);
}

void lcd_usleep(unsigned int usec)
{
	struct stm32f_tim * tim = STM32F_TIM10;

	tim->arr = usec - 1; 
	tim->cr1 = TIM_CMS_EDGE | TIM_OPM | TIM_URS | TIM_CEN; 
	thinkos_irq_wait(STM32F_IRQ_TIM10);
	tim->sr = 0;
}

static void lcd_out(unsigned int data) 
{
	uint32_t mask = stm32_gpio_rd(LCD_GPIO) & ~0xff;

	stm32_gpio_wr(LCD_GPIO, mask | data); 
#if 0
	if (data & (1 << 0))
		stm32_gpio_set(LCD_D0);
	else
		stm32_gpio_clr(LCD_D0);

	if (data & (1 << 1))
		stm32_gpio_set(LCD_D1);
	else
		stm32_gpio_clr(LCD_D1);

	if (data & (1 << 2))
		stm32_gpio_set(LCD_D2);
	else
		stm32_gpio_clr(LCD_D2);

	if (data & (1 << 3))
		stm32_gpio_set(LCD_D3);
	else
		stm32_gpio_clr(LCD_D3);

	if (data & (1 << 4))
		stm32_gpio_set(LCD_D4);
	else
		stm32_gpio_clr(LCD_D4);

	if (data & (1 << 5))
		stm32_gpio_set(LCD_D5);
	else
		stm32_gpio_clr(LCD_D5);

	if (data & (1 << 6))
		stm32_gpio_set(LCD_D6);
	else
		stm32_gpio_clr(LCD_D6);

	if (data & (1 << 7))
		stm32_gpio_set(LCD_D7);
	else
		stm32_gpio_clr(LCD_D7);
#endif
	/* EN pulse */
	udelay(1);
	stm32_gpio_set(LCD_EN);
	udelay(1);
	stm32_gpio_clr(LCD_EN);
};

static void lcd_wr(unsigned int data)
{
	/* data out */
	stm32_gpio_set(LCD_RS);
	lcd_out(data);
}

static void lcd_ctrl(unsigned int data)
{
	/* data out */
	stm32_gpio_clr(LCD_RS);
	lcd_out(data);
}

void lcd_puts(struct lcd_dev * dev, char * s)
{
	int c;

	thinkos_mutex_lock(dev->mutex);
	while ((c = *s++) != '\0') {
		lcd_wr(c);
		lcd_usleep(37);
	}
	thinkos_mutex_unlock(dev->mutex);
}

void lcd_putc(struct lcd_dev * dev, int c)
{
	thinkos_mutex_lock(dev->mutex);
	lcd_wr(c);
	lcd_usleep(37);
	thinkos_mutex_unlock(dev->mutex);
}


void lcd_clear(struct lcd_dev * dev)
{
	thinkos_mutex_lock(dev->mutex);
	lcd_ctrl(LCD_DISPLAY_CLEAR);
	lcd_usleep(1600);
	thinkos_mutex_unlock(dev->mutex);
}

void lcd_on(struct lcd_dev * dev)
{
	thinkos_mutex_lock(dev->mutex);
	lcd_ctrl(LCD_ON_OFF_CTRL + LCD_DISPLAY);
	lcd_usleep(40);
	thinkos_mutex_unlock(dev->mutex);
}

void lcd_off(struct lcd_dev * dev)
{
	thinkos_mutex_lock(dev->mutex);
	lcd_ctrl(LCD_ON_OFF_CTRL);
	lcd_usleep(40);
	thinkos_mutex_unlock(dev->mutex);
}

void lcd_set_pos(struct lcd_dev * dev, unsigned int lin, unsigned int col)
{
	if (lin > 3)
		return;
	if (col > 19)
		return;

	thinkos_mutex_lock(dev->mutex);
	switch (lin) {
	case 0:
		lcd_ctrl(LCD_SET_DDRAM_ADDR + 0 + col);
		break;
	case 1:
		lcd_ctrl(LCD_SET_DDRAM_ADDR + 40 + col);
		break;
	case 2:
		lcd_ctrl(LCD_SET_DDRAM_ADDR + 20 + col);
		break;
	case 3:
		lcd_ctrl(LCD_SET_DDRAM_ADDR + 60 + col);
		break;
	}
	lcd_usleep(37);
	thinkos_mutex_unlock(dev->mutex);
}

void lcd_cursor_on(struct lcd_dev * dev, bool blink)
{
	thinkos_mutex_lock(dev->mutex);
	lcd_ctrl(LCD_ON_OFF_CTRL + LCD_DISPLAY + LCD_CURSOR +
			 ((blink) ? LCD_BLINK : 0));
	lcd_usleep(37);
	thinkos_mutex_unlock(dev->mutex);
}

void lcd_cursor_off(struct lcd_dev * dev)
{
	thinkos_mutex_lock(dev->mutex);
	lcd_ctrl(LCD_ON_OFF_CTRL + LCD_DISPLAY);
	lcd_usleep(37);
	thinkos_mutex_unlock(dev->mutex);
}

struct lcd_dev * lcd20x4_init(void)
{
	struct lcd_dev * dev = &lcd20x4_dev;

	dev->mutex = thinkos_mutex_alloc();
	lcd_timer_init();

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOJ);

	stm32_gpio_mode(LCD_POWER, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_clr(LCD_POWER);
	stm32_gpio_mode(LCD_BACKLIGHT, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_clr(LCD_BACKLIGHT);

	stm32_gpio_mode(LCD_EN, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_RNW, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_NRE, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_RS, OUTPUT, PUSH_PULL| SPEED_MED);

	stm32_gpio_mode(LCD_D0, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D1, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D2, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D3, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D4, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D5, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D6, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(LCD_D7, OUTPUT, PUSH_PULL| SPEED_MED);

	stm32_gpio_clr(LCD_EN);
	stm32_gpio_clr(LCD_RNW);
	stm32_gpio_clr(LCD_RS);
	stm32_gpio_set(LCD_NRE);
	stm32_gpio_clr(LCD_D0);
	stm32_gpio_clr(LCD_D1);
	stm32_gpio_clr(LCD_D2);
	stm32_gpio_clr(LCD_D3);
	stm32_gpio_clr(LCD_D4);
	stm32_gpio_clr(LCD_D5);
	stm32_gpio_clr(LCD_D6);
	stm32_gpio_clr(LCD_D7);

	lcd_usleep(10);
	stm32_gpio_set(LCD_POWER);
	stm32_gpio_set(LCD_BACKLIGHT);

	lcd_usleep(10);

	/* function set */
	lcd_ctrl(LCD_FUNCTION_SET + LCD_DL);
	lcd_usleep(4100);

	/* function set */
	lcd_ctrl(LCD_FUNCTION_SET + LCD_DL);
	lcd_usleep(200);

	/* function set */
	lcd_ctrl(LCD_FUNCTION_SET + LCD_DL);
	lcd_usleep(50);

	/* function set */
	lcd_ctrl(LCD_FUNCTION_SET + LCD_DL + LCD_N);
	lcd_usleep(50);

	/* ON/OFF control */
	lcd_ctrl(LCD_ON_OFF_CTRL);
	lcd_usleep(50);

	/* Display clear */
	lcd_ctrl(LCD_DISPLAY_CLEAR);
	lcd_usleep(1520);

	/* Entry mode set */
	lcd_ctrl(LCD_ENTRY_MODE_SET + LCD_ID);
	lcd_usleep(50);

	/* ON/OFF control */
	lcd_ctrl(LCD_ON_OFF_CTRL + LCD_DISPLAY);
	lcd_usleep(50);

	return dev;
}

