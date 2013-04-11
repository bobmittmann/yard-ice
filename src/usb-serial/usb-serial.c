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
#include <stdio.h>

#include <thinkos.h>

#include <sys/dcclog.h>
#include <sys/usb-cdc.h>

struct serial_dev * serial_open(struct stm32f_usart * uart);
int serial_write(struct serial_dev * dev, const void * buf, 
				 unsigned int len);
int serial_read(struct serial_dev * dev, char * buf, 
				unsigned int len, unsigned int msec);

struct vcom {
	struct serial_dev * serial;
	usb_cdc_class_t * cdc;
};

#define VCOM_BUF_SIZE 128

#if 0
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
#endif

#define USB_FS_DP STM32F_GPIOA, 12
#define USB_FS_DM STM32F_GPIOA, 11
#define USB_FS_VBUS STM32F_GPIOA, 9

#define LED0_IO STM32F_GPIOB, 9
#define PUSHBTN_IO STM32F_GPIOA, 6
#define EXTRST_IO STM32F_GPIOA, 5

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
	stm32f_gpio_mode(LED0_IO, OUTPUT, PUSH_PULL | SPEED_LOW);

	/* PA6: Push button */
	stm32f_gpio_mode(PUSHBTN_IO, INPUT, PULL_UP);

	/* PA5: External Reset */
	stm32f_gpio_mode(EXTRST_IO, OUTPUT, OPEN_DRAIN | PULL_UP);
//	stm32f_gpio_mode(EXTRST_IO, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_set(EXTRST_IO);
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

int8_t led_flag;
volatile int8_t led_locked;

void led_on(void)
{
	stm32f_gpio_set(LED0_IO);
}

void led_off(void)
{
	stm32f_gpio_clr(LED0_IO);
}

void led_lock(void)
{
	led_locked = 1;
}

void led_unlock(void)
{
	led_locked = 0;
}

void led_flash(void)
{
	thinkos_flag_set(led_flag);
}

int led_task(void)
{
	while (1) {
		thinkos_flag_wait(led_flag);
		thinkos_flag_clr(led_flag);
		if (!led_locked)
			led_on();
		thinkos_sleep(100);
		if (!led_locked)
			led_off();
		thinkos_sleep(100);
	}
}

uint32_t led_stack[32];

void led_init(void)
{
	led_flag = thinkos_flag_alloc();

	thinkos_thread_create((void *)led_task, (void *)NULL,
						  led_stack, sizeof(led_stack),
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));
}


#define LOOP_TIME 50 
#define BUSY_TIME (5000 / LOOP_TIME)

static int push_btn_stat(void)
{
	return stm32f_gpio_stat(PUSHBTN_IO) ? 0 : 1;
}

enum {
	EVENT_NONE,
	EVENT_BTN_PRESSED,
	EVENT_BTN_RELEASED,
	EVENT_RST_TIMEOUT
};

enum {
	RST_OFF,
	RST_ON,
	RST_WAIT
};


int button_task(void)
{
	int btn_st[2];
	int rst_tmr = 0;
	int rst_st = RST_OFF;
	int event;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	btn_st[0] = push_btn_stat();

	while (1) {
		thinkos_sleep(LOOP_TIME);

		btn_st[1] = push_btn_stat();
		if (btn_st[1] != btn_st[0]) {
			/* process push button */
			event = btn_st[1] ? EVENT_BTN_PRESSED : EVENT_BTN_RELEASED;
			btn_st[0] = btn_st[1];
		} else if (rst_tmr) {
			/* process timer */
			event = (--rst_tmr == 0) ? EVENT_RST_TIMEOUT: EVENT_NONE;
		} else {
			event = EVENT_NONE;
		}

		switch (event) {

		case EVENT_BTN_PRESSED:
			if (rst_st == RST_OFF) {
				rst_tmr = 500 / LOOP_TIME;
				stm32f_gpio_set(EXTRST_IO);
				led_lock();
				led_on();
				rst_st = RST_ON;
			}
			break;

		case EVENT_BTN_RELEASED:
			if (rst_st == RST_WAIT) {
				rst_st = RST_OFF;
			}
			break;

		case EVENT_RST_TIMEOUT:
			stm32f_gpio_clr(EXTRST_IO);
			led_off();
			led_unlock();
			rst_st = RST_WAIT;
			break;

		}
	}
	return 0;
}

uint32_t button_stack[32];

void button_init(void)
{
#ifdef STM32F10X
	thinkos_thread_create((void *)button_task, (void *)NULL,
						  button_stack, sizeof(button_stack),
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));
#endif
}

int usb_recv_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	usb_cdc_class_t * cdc = vcom->cdc;
	char buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = usb_cdc_read(cdc, buf, VCOM_BUF_SIZE, 100);
		led_flash();
		serial_write(serial, buf, len);
	}

	return 0;
}

int serial_recv_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_class * cdc = vcom->cdc;
	char buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = serial_read(serial, buf, VCOM_BUF_SIZE, 100);
		led_flash();
		usb_cdc_write(cdc, buf, len);
	}

	return 0;
}

int serial_ctrl_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_class * cdc = vcom->cdc;
	struct usb_cdc_state prev_state;
	struct usb_cdc_state state;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	memset(&prev_state, 0, sizeof(struct usb_cdc_state));

	while (1) {
		usb_cdc_state_get(cdc, &state);
		if ((state.cfg.baud_rate != prev_state.cfg.baud_rate) ||
			(state.cfg.data_bits != prev_state.cfg.data_bits) ||
			(state.cfg.parity != prev_state.cfg.parity) ||
			(state.cfg.stop_bits != prev_state.cfg.stop_bits)) {
			serial_config_set(serial, &state.cfg);
			prev_state.cfg = state.cfg;
		}
		usb_cdc_ctl_wait(cdc, 0);
	}
	return 0;
}

uint32_t serial_ctrl_stack[32];
uint32_t usb_recv_stack[128];
uint32_t serial_recv_stack[128];

int main(int argc, char ** argv)
{
	uint64_t esn;
	struct vcom vcom;
	unsigned int i;
#ifdef STM32F10X
	struct stm32f_usart * uart = STM32F_USART2;
#endif
#ifdef STM32F2X
	struct stm32f_usart * uart = STM32F_UART5;
#endif

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(4));

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 9600);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	vcom.serial = serial_open(uart);

	led_init();

	button_init();

	esn = *((uint64_t *)STM32F_UID);
	DCC_LOG2(LOG_TRACE, "ESN=0x%08x%08x", esn >> 32, esn);
	usb_cdc_sn_set(esn);

#ifdef STM32F10X
	vcom.cdc = usb_cdc_init(&stm32f_usb_fs_dev);
#endif

#ifdef STM32F2X
	vcom.cdc = usb_cdc_init(&stm32f_otg_fs_dev);
#endif

	thinkos_thread_create((void *)usb_recv_task, (void *)&vcom,
						  usb_recv_stack, sizeof(usb_recv_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	thinkos_thread_create((void *)serial_recv_task, (void *)&vcom,
						  serial_recv_stack, sizeof(serial_recv_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(2));

	thinkos_thread_create((void *)serial_ctrl_task, (void *)&vcom,
						  serial_ctrl_stack, sizeof(serial_ctrl_stack),
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(3));

	for (i = 0; ; ++i) {
		thinkos_sleep(5000);
		led_flash();
		DCC_LOG1(LOG_TRACE, "%d tick.", i);
	}

	return 0;
}

