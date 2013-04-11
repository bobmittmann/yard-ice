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
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <thinkos.h>

#include <sys/dcclog.h>
#include <sys/usb-cdc.h>

struct vcom {
	struct serial_dev * serial;
	struct usb_cdc_class * cdc;
};

#define VCOM_BUF_SIZE 128

#if 0
int usb_recv_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_class * usb = vcom->usb;
	char buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = usb_cdc_read(usb, buf, VCOM_BUF_SIZE, 100);
		if (len > 0)
			serial_write(serial, buf, len);
	}

	return 0;
}

int usb_ctrl_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_class * usb = vcom->usb;
	struct usb_cdc_state prev_state;
	struct usb_cdc_state state;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	usb_cdc_state_get(usb, &prev_state);

	while (1) {
		usb_cdc_ctrl_event_wait(usb, 0);
		usb_cdc_state_get(usb, &state);
		if (state.cfg != prev_state.cfg) {
			DCC_LOG1(LOG_TRACE, "[%d] config changed.", thinkos_thread_self());
			prev_state.cfg = state.cfg;
		}
		if (state.ctrl != prev_state.ctrl) {
			DCC_LOG1(LOG_TRACE, "[%d] control changed.", thinkos_thread_self());
			prev_state.ctrl = state.ctrl;
		}
	}
	return 0;
}


int serial_recv_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_class * usb = vcom->usb;
	char buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = serial_read(serial, buf, VCOM_BUF_SIZE, 100);
		if (len > 0)
			usb_cdc_write(usb, buf, len);
	}

	return 0;
}

int serial_ctrl_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_class * usb = vcom->usb;
	struct usb_cdc_state prev_state;
	struct usb_cdc_state state;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	usb_cdc_state_get(usb, &prev_state);

	while (1) {
		usb_cdc_ctrl_event_wait(usb, 0);
		usb_cdc_state_get(usb, &state);
		if (state.cfg != prev_state.cfg) {
			DCC_LOG1(LOG_TRACE, "[%d] config changed.", thinkos_thread_self());
			prev_state.cfg = state.cfg;
		}
		if (state.ctrl != prev_state.ctrl) {
			DCC_LOG1(LOG_TRACE, "[%d] control changed.", thinkos_thread_self());
			prev_state.ctrl = state.ctrl;
		}
	}
	return 0;
}

#define STACK_SIZE 512
uint32_t usb_recv_stack[STACK_SIZE / 4];
uint32_t usb_ctrl_stack[STACK_SIZE / 4];
uint32_t serial_recv_stack[STACK_SIZE / 4];
uint32_t serial_ctrl_stack[STACK_SIZE / 4];

int main(int argc, char ** argv)
{
	struct vcom vcom;
	int i = 0;

	DCC_LOG_CONNECT();
	DCC_LOG_INIT();

	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	vcom.usb = usb_cdc_init();
	vcom.serial = serial_init(stm32f_uart5);

	thinkos_thread_create((void *)usb_recv_task, (void *)vcom,
						  usb_recv_stack, STACK_SIZE, 0);

	thinkos_thread_create((void *)usb_ctrl_task, (void *)vcom,
						  usb_ctrl_stack, STACK_SIZE, 0);

	thinkos_thread_create((void *)serial_recv_task, (void *)vcom,
						  serial_recv_stack, STACK_SIZE, 0);

	thinkos_thread_create((void *)serial_ctrl_task, (void *)vcom,
						  serial_ctrl_stack, STACK_SIZE, 0);

	for (i = 0; ;i++) {
		thinkos_sleep(10000);
	}

	return 0;
}
#endif

#define USB_FS_DP STM32F_GPIOA, 12
#define USB_FS_DM STM32F_GPIOA, 11
#define USB_FS_VBUS STM32F_GPIOA, 9

void io_init(void)
{
#ifdef STM32F10X
	struct stm32f_rcc * rcc = STM32F_RCC;
#endif

	DCC_LOG(LOG_MSG, "Configuring GPIO pins...");

#ifdef STM32F10X
	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);

	/* Enable Alternate Functions IO clock */
	rcc->apb2enr |= RCC_AFIOEN;

	/* PA3: USART2_TX */
	stm32f_gpio_mode(STM32F_GPIOA, 2, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	/* PA2: USART2_RX */
	stm32f_gpio_mode(STM32F_GPIOA, 3, INPUT, PULL_UP);

	/* PB9: LED */
	stm32f_gpio_mode(STM32F_GPIOB, 9, OUTPUT, PUSH_PULL | SPEED_LOW);

	/* PA6: Push button */
	stm32f_gpio_mode(STM32F_GPIOA, 6, INPUT, PULL_UP);

	/* PA5: External Reset */
//	stm32f_gpio_mode(STM32F_GPIOA, 5, OUTPUT, OPEN_DRAIN | PULL_UP);
	stm32f_gpio_mode(STM32F_GPIOA, 5, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_set(STM32F_GPIOA, 5);
#endif

#ifdef STM32F2X
	stm32f_gpio_clock_en(STM32F_GPIOC);
	stm32f_gpio_clock_en(STM32F_GPIOD);

	/* PC12: UART5_TX */
	stm32f_gpio_mode(STM32F_GPIOC, 12, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_af(STM32F_GPIOC, 12, GPIO_AF8);
	/* PD2: UART5_RX */
	stm32f_gpio_mode(STM32F_GPIOD, 2, INPUT, PULL_UP);
	stm32f_gpio_af(STM32F_GPIOD, 2, GPIO_AF8);
#endif
}

#define BUTTON_STACK_SIZE 128 
uint32_t button_stack[BUTTON_STACK_SIZE / 4];

enum {
	RST_OFF,
	RST_ON,
	RST_WAIT
};

enum {
	EV_NONE,
	EV_BTN_DOWN,
	EV_BTN_UP
};

#define LOOP_TIME 20 
#define BUSY_TIME (5000 / LOOP_TIME)

int button_task(void)
{
	int btn_state;
	int prev_state;
	bool btn_down;
	int rst_tmr = 0;
	int rst_st = RST_OFF;
	int busy_tmr = BUSY_TIME;
	int busy_cnt = 0;
	int led_tmr = 0;
	int i;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (i = 0; i < 4; i++) {
		thinkos_sleep(200);
		stm32f_gpio_set(STM32F_GPIOB, 9);
		thinkos_sleep(50);
		stm32f_gpio_clr(STM32F_GPIOB, 9);
	}

	prev_state = stm32f_gpio_stat(STM32F_GPIOA, 6);
	btn_down = (prev_state == 0) ? true : false;

	while (1) {
		thinkos_sleep(LOOP_TIME);
		continue;

		btn_state = stm32f_gpio_stat(STM32F_GPIOA, 6);
		if (btn_state != prev_state) {
			prev_state = btn_state;
			if (btn_state == 0) {
				btn_down = true;;
				busy_cnt++;
			} else {
				btn_down = false;
				busy_cnt--;
			}
		}

		/* Reset pulse */
		switch (rst_st) {
		case RST_OFF:
			if (btn_down) {
				rst_tmr = 500 / LOOP_TIME;
				stm32f_gpio_clr(STM32F_GPIOA, 5);
				stm32f_gpio_set(STM32F_GPIOB, 9);
				rst_st = RST_ON;
				busy_cnt++;
			}
			break;
		case RST_ON:
			if (--rst_tmr == 0) {
				stm32f_gpio_set(STM32F_GPIOA, 5);
				stm32f_gpio_clr(STM32F_GPIOB, 9);
				rst_st = RST_WAIT;
			}
			break;
		case RST_WAIT:
			if (!btn_down) {
				rst_st = RST_OFF;
				busy_cnt--;
			}
			break;
		}

		if ((busy_cnt == 0) && (busy_tmr == 0)) {
			if (led_tmr == 0) {
				/* Idle */
				led_tmr = (rand() & 0xf) + 2;
				DCC_LOG1(LOG_TRACE, "led_tmr=%d", led_tmr);
				if (led_tmr & 1)
					stm32f_gpio_set(STM32F_GPIOB, 9);
				else
					stm32f_gpio_clr(STM32F_GPIOB, 9);
			} else {
				led_tmr--;
			}
		} else {
			/* Reload the busy timer */
			busy_tmr = (busy_cnt) ? BUSY_TIME : busy_tmr - 1;;
		}
	}
	return 0;
}


int main(int argc, char ** argv)
{
	usb_cdc_class_t * cdc_acm;
	char buf[128];
	int n;
#ifdef STM32F10X
	struct stm32f_usart * us = STM32F_USART2;
#endif
#ifdef STM32F2X
	struct stm32f_usart * us = STM32F_UART5;
#endif

//	struct vcom vcom;
	int i;
	int j;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));

#ifdef STM32F10X
	thinkos_thread_create((void *)button_task, (void *)NULL,
						  button_stack, BUTTON_STACK_SIZE, 
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));
#endif

//	usb_cdc_init();
//	vcom.usb = usb_cdc_init();

//	vcom.serial = serial_init(stm32f_uart5);
	stm32f_usart_init(us);
	stm32f_usart_baudrate_set(us, 115200);
	stm32f_usart_mode_set(us, SERIAL_8N1);
	stm32f_usart_enable(us);

#ifdef STM32F10X
	cdc_acm = usb_cdc_init(&stm32f_usb_fs_dev);
#endif

#ifdef STM32F2X
	cdc_acm = usb_cdc_init(&stm32f_otg_fs_dev);
#endif

	for (i = 0; ; ++i) {
		n = usb_cdc_read(cdc_acm, buf, 128, 0);
		DCC_LOG2(LOG_TRACE, "%d usb_cdc_read()=%d", i, n);
		for (j = 0; j < n; ++j) {
			stm32f_usart_putc(us, buf[i]);
		}
		thinkos_sleep(1000);
	}

	return 0;
}

