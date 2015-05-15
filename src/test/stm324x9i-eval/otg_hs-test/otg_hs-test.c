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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/param.h>

#include <sys/stm32f.h>
#include <sys/delay.h>
#include <sys/usb-cdc.h>
#include <sys/dcclog.h>

#include <thinkos.h>

#include "board.h"

const char msg[] = "1.\r\n\r\n"
	"When Zarathustra was thirty years old, he left his home and the lake of "
	"his home, and went into the mountains.  There he enjoyed his spirit and "
	"solitude, and for ten years did not weary of it.  But at last his heart "
	"changed,--and rising one morning with the rosy dawn, he went before the "
	"sun, and spake thus unto it:\r\n\r\n"
	"Thou great star!  What would be thy happiness if thou hadst not those for "
	"whom thou shinest!\r\n\r\n"
	"For ten years hast thou climbed hither unto my cave:  thou wouldst have "
	"wearied of thy light and of the journey, had it not been for me, mine "
	"eagle, and my serpent.\r\n\r\n"
	"But we awaited thee every morning, took from thee thine overflow "
	"and blessed thee for it.\r\n\r\n"
	"Lo!  I am weary of my wisdom, like the bee that hath gathered too much "
	"honey; I need hands outstretched to take it.\r\n\r\n"
	"I would fain bestow and distribute, until the wise have once more become "
	"joyous in their folly, and the poor happy in their riches.\r\n\r\n"
	"Therefore must I descend into the deep:  as thou doest in the evening, "
	"when thou goest behind the sea, and givest light "
	"also to the nether-world, "
	"thou exuberant star!\r\n\r\n"
	"Like thee must I GO DOWN, as men say, to whom I shall descend.\r\n\r\n"
	"Bless me, then, thou tranquil eye, that canst behold even the greatest "
	"happiness without envy!\r\n\r\n"
	"Bless the cup that is about to overflow, "
	"that the water may flow golden out "
	"of it, and carry everywhere the reflection of thy bliss!\r\n\r\n"
	"Lo!  This cup is again going to empty itself, and Zarathustra is again "
	"going to be a man.\r\n\r\n"
	"Thus began Zarathustra's down-going.\r\n\r\n\r\n";

#define BUF_SIZE 256

int echo_task(FILE * f)
{
	char buf[BUF_SIZE];
	unsigned int n;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());
	thinkos_sleep(100);

	while (1) {
		n = rand() & 0x1ff;
		n = MIN(n , BUF_SIZE);
		n = fread(buf, n, 1, f);
		fwrite(buf, n, 1, f);
	}

	return 0;
}

#ifndef FIRELINK
#define FIRELINK 0
#endif

#if FIRELINK
void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOJ);

	stm32_gpio_mode(LED1, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED2, OUTPUT, OPEN_DRAIN | SPEED_MED);

	stm32_gpio_set(LED1);
	stm32_gpio_set(LED2);

}
#endif

#if !FIRELINK
#undef LED1
#undef LED2
#define LED1      STM32_GPIOG, 6
#define LED2      STM32_GPIOG, 7
#define LED3      STM32_GPIOG, 10
#define LED4      STM32_GPIOG, 12
void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOG);
	stm32_gpio_mode(LED1, OUTPUT, OPEN_DRAIN | SPEED_MED);
	stm32_gpio_mode(LED2, OUTPUT, OPEN_DRAIN | SPEED_MED);

	stm32_gpio_set(LED1);
	stm32_gpio_set(LED2);
}
#endif


#define STACK_SIZE 512
uint32_t printer_stack1[STACK_SIZE / 4];
uint32_t printer_stack2[STACK_SIZE / 4];

uint32_t echo_stack1[STACK_SIZE / 4];
uint32_t echo_stack2[STACK_SIZE / 4];

int main(int argc, char ** argv)
{
#if FIRELINK
	struct lcd_dev * lcd;
#endif
	usb_cdc_class_t * cdc;
	uint8_t buf[128];
	int i;
	int n;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();


	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(7));


	DCC_LOG(LOG_TRACE, "3. io_init()");
	io_init();

#if FIRELINK
	DCC_LOG(LOG_TRACE, "4. lcd20x4_init()");
	lcd = lcd20x4_init();

	lcd_puts(lcd, "===== ThinkOS ======");
	lcd_puts(lcd, " Zigbee Coordinator ");
	lcd_puts(lcd, "      Firelink      ");
	lcd_puts(lcd, "====================");
#endif

	for (i = 0; i < 0; ++i) {
		DCC_LOG1(LOG_MSG, "%d", i);
		__led_on(LED1);
		thinkos_sleep(250);
		__led_on(LED2);
		thinkos_sleep(250);
		__led_off(LED1);
		thinkos_sleep(250);
		__led_off(LED2);
		thinkos_sleep(250);
	}

	DCC_LOG(LOG_TRACE, "4. usb_cdc_init()");
	usb_cdc_sn_set(*((uint64_t *)STM32F_UID));
	cdc = usb_cdc_init(&stm32f_otg_hs_dev, 
					   cdc_acm_def_str, 
					   cdc_acm_def_strcnt);

	thinkos_sleep(500);
//	mdelay(500);

//	__led_on(LED1);

	DCC_LOG(LOG_TRACE, "usb_cdc_acm_lc_wait()");
	usb_cdc_acm_lc_wait(cdc);

//	__led_on(LED2);

	while (0) {
		thinkos_sleep(5000);
		DCC_LOG(LOG_TRACE, "usb_cdc_write()");
		usb_cdc_write(cdc, "Hello world!", 12);
	};

	thinkos_sleep(3000);

	while (1) {
		DCC_LOG(LOG_TRACE, "usb_cdc_read()");
		n = usb_cdc_read(cdc, buf, 128, 5000);
		(void)n;
		DCC_LOG1(LOG_TRACE, "n=%d", n);
		if (n > 0)
			usb_cdc_write(cdc, buf, n);
	};

	thinkos_sleep(1000);

	usb_cdc_write(cdc, msg, sizeof(msg));


	thinkos_sleep(1000);

#if 0
	/* create some printer threads */
	thinkos_thread_create((void *)printer_task, (void *)f, 
						  printer_stack1, STACK_SIZE, 0);

	thinkos_thread_create((void *)printer_task, (void *)f, 
						  printer_stack2, STACK_SIZE, 0);


	/* create some echo threads */
	thinkos_thread_create((void *)echo_task, (void *)f, 
						  echo_stack1, STACK_SIZE, 0);

//	thinkos_thread_create((void *)echo_task, (void *)f, 
//						  echo_stack2, STACK_SIZE, 0);
#endif


	return 0;
}

