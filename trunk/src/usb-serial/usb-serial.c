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
#include <sys/null.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/tty.h>

#include <thinkos.h>
#define __THINKOS_SYS__
#include <thinkos_sys.h>

#include <sys/dcclog.h>
#include <sys/usb-cdc.h>

#include <xmodem.h>

void pfclock_init(void);
uint32_t pfclock(void);
uint32_t pfdt(void);

struct serial_dev * serial2_open(void);

struct serial_dev * serial3_open(void);

int serial_write(struct serial_dev * dev, const void * buf, 
				 unsigned int len);

int serial_read(struct serial_dev * dev, char * buf, 
				unsigned int len, unsigned int msec);

void led_lock(void);
void led_unlock(void);
void led1_flash(unsigned int cnt);
void led2_flash(unsigned int cnt);
void led_flash_all(unsigned int cnt);
void led1_on(void);
void led1_off(void);
void led2_on(void);
void led2_off(void);
void leds_init(void);

#define SERIAL_DEBUG 1
#define USB_DEBUG 0

#ifndef USB_DEBUG
#ifdef DEBUG
#define USB_DEBUG 1
#else
#define USB_DEBUG 0
#endif
#endif

#ifndef SERIAL_DEBUG
#ifdef DEBUG
#define SERIAL_DEBUG 1
#else
#define SERIAL_DEBUG 0
#endif
#endif

struct vcom {
	struct serial_dev * serial;
	usb_cdc_class_t * cdc;
	struct serial_status ser_stat;
};

#define VCOM_BUF_SIZE 128

#define USB_FS_DP STM32F_GPIOA, 12
#define USB_FS_DM STM32F_GPIOA, 11

#define USB_FS_VBUS STM32F_GPIOB, 6 /* PB6 */

#define PUSHBTN_IO STM32F_GPIOB, 8
#define EXTRST0_IO STM32F_GPIOB, 0 
#define EXTRST1_IO STM32F_GPIOA, 5 /* PA5 (connected to USART2_CK/ USART3_TX) */

#define USART1_TX STM32F_GPIOA, 9
#define USART1_RX STM32F_GPIOA, 10

#define USART2_TX STM32F_GPIOA, 2
#define USART2_RX STM32F_GPIOA, 3

#define USART3_TX STM32F_GPIOB, 10
#define USART3_RX STM32F_GPIOB, 11

void io2_sel_pa5(void)
{
	stm32f_gpio_mode(USART3_TX, INPUT, 0);
	stm32f_gpio_mode(EXTRST1_IO, OUTPUT, OPEN_DRAIN | PULL_UP);
}

void io_sel_usart3(void)
{
	stm32f_gpio_mode(EXTRST1_IO, INPUT, 0);
	stm32f_gpio_mode(USART3_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
}

void io_sel_i2c(void)
{
}

void io_sel_rts(void)
{
}

/* USART1 and USART2 pins are connected together.
   Only one TX pin must be enable at any time */

/* Select USART2 TX */
void io_sel_usart2(void)
{
	stm32f_gpio_mode(USART1_TX, INPUT, 0);
	stm32f_gpio_mode(USART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
}

/* Select USART1 TX */
void io_sel_usart1(void)
{
	stm32f_gpio_mode(USART2_TX, INPUT, 0);
	stm32f_gpio_mode(USART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
}

void usb_vbus(bool on)
{
//	if (on)
//		stm32f_gpio_set(USB_FS_VBUS);
//	else
//		stm32f_gpio_clr(USB_FS_VBUS);

	if (on)
		stm32f_gpio_mode(USB_FS_VBUS, OUTPUT, PUSH_PULL | SPEED_LOW);
	else
		stm32f_gpio_mode(USB_FS_VBUS, INPUT, 0);
}

void io_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);

	/* Enable Alternate Functions IO clock */
	rcc->apb2enr |= RCC_AFIOEN;

	/* UART1 TX (disabled) */
	stm32f_gpio_mode(USART1_TX, INPUT, 0);
	/* UART1 RX */
	stm32f_gpio_mode(USART1_RX, INPUT, PULL_UP);

	/* Primary UART TX */
	stm32f_gpio_mode(USART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	/* Primary UART RX */
	stm32f_gpio_mode(USART2_RX, INPUT, PULL_UP);

	/* Secondary UART TX */
	stm32f_gpio_mode(USART3_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	/* Secondary UART RX */
	stm32f_gpio_mode(USART3_RX, INPUT, PULL_UP);

	/* Push button */
	stm32f_gpio_mode(PUSHBTN_IO, INPUT, PULL_UP);

	/* External Reset pins */
	stm32f_gpio_mode(EXTRST0_IO, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_set(EXTRST0_IO);

	stm32f_gpio_mode(EXTRST1_IO, INPUT, 0);
	stm32f_gpio_set(EXTRST1_IO);

//	stm32f_gpio_mode(USB_FS_VBUS, OUTPUT, PUSH_PULL | SPEED_LOW);
//	stm32f_gpio_clr(USB_FS_VBUS);

	stm32f_gpio_mode(USB_FS_VBUS, INPUT, 0);
	stm32f_gpio_set(USB_FS_VBUS);
}

void system_reset(void)
{
	DCC_LOG(LOG_TRACE, "...");
    CM3_SCB->aircr =  SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
	for(;;);
}

#define LOOP_TIME 50 
#define BUSY_TIME (5000 / LOOP_TIME)

static int push_btn_stat(void)
{
	return stm32f_gpio_stat(PUSHBTN_IO) ? 1 : 0;
}

enum {
	EVENT_NONE,
	EVENT_BTN_PRESSED,
	EVENT_BTN_RELEASED,
	EVENT_EXT_RST_TIMEOUT,
	EVENT_SYS_RST_TIMEOUT
};

enum {
	EXT_RST_OFF,
	EXT_RST_ON,
	EXT_RST_WAIT
};


int button_task(void)
{
	int btn_st[2];
	int sys_rst_tmr = 0;
	int ext_rst_tmr = 0;
	int ext_rst_st = EXT_RST_OFF;
	int event;
	int i;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	btn_st[0] = push_btn_stat();

	while (1) {
		thinkos_sleep(LOOP_TIME);

		btn_st[1] = push_btn_stat();
		if (btn_st[1] != btn_st[0]) {
			/* process push button */
			event = btn_st[1] ? EVENT_BTN_PRESSED : EVENT_BTN_RELEASED;
			btn_st[0] = btn_st[1];
		} else if (ext_rst_tmr) {
			/* process external reset timer */
			event = (--ext_rst_tmr == 0) ? EVENT_EXT_RST_TIMEOUT: EVENT_NONE;
		} else if (sys_rst_tmr) {
			/* process system reset timer */
			event = (--sys_rst_tmr == 0) ? EVENT_SYS_RST_TIMEOUT: EVENT_NONE;
		} else {
			event = EVENT_NONE;
		}

		switch (event) {

		case EVENT_BTN_PRESSED:
			DCC_LOG(LOG_TRACE, "BTN_PRESSED");
			if (ext_rst_st == EXT_RST_OFF) {
				/* start external reset timer */
				ext_rst_tmr = 500 / LOOP_TIME;
				/* start system reset timer */
				sys_rst_tmr = 5000 / LOOP_TIME;
				stm32f_gpio_set(EXTRST1_IO);
				stm32f_gpio_set(EXTRST0_IO);
				led_lock();
				led2_on();
				ext_rst_st = EXT_RST_ON;
			}
			break;

		case EVENT_BTN_RELEASED:
			DCC_LOG(LOG_TRACE, "BTN_RELEASED");
//			if (ext_rst_st == EXT_RST_WAIT) {
//				ext_rst_st = EXT_RST_OFF;
//			}
			/* reset system reset timer */
			sys_rst_tmr = 0;
			break;

		case EVENT_EXT_RST_TIMEOUT:
			DCC_LOG(LOG_TRACE, "EXT_RST_TIMEOUT");
			stm32f_gpio_clr(EXTRST0_IO);
			stm32f_gpio_clr(EXTRST1_IO);
			led2_off();
			led_unlock();
			ext_rst_st = EXT_RST_OFF;
			break;

		case EVENT_SYS_RST_TIMEOUT:
			DCC_LOG(LOG_TRACE, "SYS_RST_TIMEOUT");
			led_lock();

			for (i = 0; i < 10; ++i) {
				led2_on();
				thinkos_sleep(100);
				led2_off();
				thinkos_sleep(200);
			}

			system_reset();
			break;
		}
	}
	return 0;
}

struct xmodem_rcv rx;

int vcom_xmodem_recv(struct vcom * vcom)
{
//	struct serial_dev * serial1 = vcom[0].serial;
	struct comm_dev comm;
//	struct serial_dev * serial2 = vcom[1].serial;
	usb_cdc_class_t * cdc = vcom[0].cdc;
	char buf[VCOM_BUF_SIZE];
	int len;
	int pos;
	int ret;
	int c;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());


	comm.arg = cdc;
	comm.op.send = (int (*)(void *, const void *, unsigned int))usb_cdc_write;
	comm.op.recv = (int (*)(void *, void *, 
						  unsigned int, unsigned int))usb_cdc_read;
	xmodem_rcv_init(&rx, &comm, XMODEM_1K);

	for (;;) {
		len = usb_cdc_read(cdc, buf, VCOM_BUF_SIZE, XMODEM_TMOUT_TICK_MS);
		if (len > 0) {
//			led1_flash(1);
			DCC_LOG1(LOG_INFO, "USB RX: %d bytes.", len);
//			serial_write(serial1, buf, len);
//			serial_write(serial2, buf, len);
			pos = 0;
			c = buf[0];
		} else {
			len = 0;
			pos = 0;
			c = -1;
		}

		do {
			ret = xmodem_rcv(&rx, &c);

			if (ret == XMODEM_RCV_EOT) {
				DCC_LOG(LOG_TRACE, "XModem EOT.");
				break;
			}

			DCC_LOG1(LOG_INFO, "XModem ret=%d", ret);
		
			c = buf[++pos];
		} while (pos < len);
	}

	return 0;
}

int usb_recv_task(struct vcom vcom[])
{
//	struct serial_dev * serial1 = vcom[0].serial;
	struct serial_dev * serial2 = vcom[1].serial;
	usb_cdc_class_t * cdc = vcom[0].cdc;
	char buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = usb_cdc_read(cdc, buf, VCOM_BUF_SIZE, 5000);
		if (len > 0) {
//			led1_flash(1);
			DCC_LOG1(LOG_TRACE, "USB RX: %d bytes.", len);
//			serial_write(serial1, buf, len);
			serial_write(serial2, buf, len);
		}

	}

	return 0;
}

#define ECHO_BUF_SIZE 128

int usb_echo(usb_cdc_class_t * cdc)
{
	char buf[ECHO_BUF_SIZE];
	int len;

	for (;;) {
		len = usb_cdc_read(cdc, buf, ECHO_BUF_SIZE, 10000);
		led1_flash(1);
		if (len > 0)
			usb_cdc_write(cdc, buf, len);
	}

	return 0;
}

struct serial_dev * serial1;
struct serial_dev * serial2;

int serial_recv_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_class * cdc = vcom->cdc;
	char buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = serial_read(serial, buf, VCOM_BUF_SIZE, 100);
		if (len > 0) {
			DCC_LOG5(LOG_INFO, "len=%d [%c%c%c%c]", 
					 len, buf[0], buf[1], buf[2], buf[3]);
//			led2_flash(1);
			usb_cdc_write(cdc, buf, len);
		}
	}

	return 0;
}


struct xmodem_snd sx;
struct xmodem_rcv rx;

int serial_xmodem_send(struct serial_dev * serial)
{
	struct comm_dev comm;
	char data[8] = "Hello!";
	int ret;
	int i;

	comm.arg = serial;
	comm.op.send = (int (*)(void *, const void *, unsigned int))serial_write;
	comm.op.recv = (int (*)(void *, void *, 
						  unsigned int, unsigned int))serial_read;

	DCC_LOG(LOG_TRACE, "........................");
	printf("XMODEM send\n");

	printf("xmodem_snd_init()\n");
	xmodem_snd_init(&sx, &comm, XMODEM_1K);

	for (i = 0; i < 8192; ++i) {
		if ((ret = xmodem_snd_loop(&sx, data, 8)) < 0) {
			printf("xmodem_snd_loop() error: %d\n", ret);
			return ret;
		}
	}

	printf("xmodem_snd_eot().\n");
	return xmodem_snd_eot(&sx);
}

int serial_xmodem_recv(struct serial_dev * serial)
{
	struct comm_dev comm;
	int ret;
	int cnt;

	comm.arg = serial;
	comm.op.send = (int (*)(void *, const void *, unsigned int))serial_write;
	comm.op.recv = (int (*)(void *, void *, 
						  unsigned int, unsigned int))serial_read;

	DCC_LOG(LOG_TRACE, "........................");
	printf("XMODEM send\n");

	printf("xmodem_snd_init()\n");
	xmodem_rcv_init(&rx, &comm, XMODEM_1K);

	cnt = 0;
	do {
		if ((ret = xmodem_rcv_loop(&rx, NULL, 1)) < 0) {
			printf("xmodem_snd_loop() error: %d\n", ret);
			return ret;
		}

		cnt += ret;
	} while (ret > 0);

	printf("EOT\n");
	printf("xfer size = %d\n", cnt);
	printf("xmodem_snd_eot().\n");
	return cnt;
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
		if ((state.cfg.baudrate != prev_state.cfg.baudrate) ||
			(state.cfg.databits != prev_state.cfg.databits) ||
			(state.cfg.parity != prev_state.cfg.parity) ||
			(state.cfg.stopbits != prev_state.cfg.stopbits)) {
			serial_config_set(serial, &state.cfg);
			prev_state.cfg = state.cfg;
		}

		if (state.ctrl.dtr != prev_state.ctrl.dtr) {
			vcom->ser_stat.dsr = state.ctrl.dtr;
			usb_cdc_status_set(cdc, &vcom->ser_stat);
			prev_state.ctrl = state.ctrl;
		}

		DCC_LOG1(LOG_TRACE, "[%d] sleep!", thinkos_thread_self());
		usb_cdc_ctl_wait(cdc, 0);
		DCC_LOG1(LOG_TRACE, "[%d] wakeup!", thinkos_thread_self());
	}
	return 0;
}

void show_menu(FILE * f)
{
	fprintf(f, "\n");
	fprintf(f, " Options:\n");
	fprintf(f, " --------\n");
	fprintf(f, "   r    - xmodem receive\n");
	fprintf(f, "   s    - xmodem send\n");
	fprintf(f, "\n");
}

int usb_console(struct usb_cdc_class * cdc)
{
	struct tty_dev * tty;
	FILE * f_raw;
	unsigned int n;
	int c;

	f_raw = usb_cdc_fopen(cdc);
	tty = tty_attach(f_raw);
	stdout = tty_fopen(tty);
	stdin = f_raw;

	for (n = 0; ; ++n) {
		thinkos_sleep(100);
		c = fgetc(stdin);
		switch (c) {
		case '\r':
			show_menu(stdout);
			break;

		case 's':
			printf("\nXMODEM send...\n");
			serial_xmodem_send(serial2);
			printf(".\n");
			break;

		case 'r':
			printf("\nXMODEM receive...\n");
			printf(".\n");
			break;
		}
	}

	return 0;
}

uint32_t button_stack[40];
uint32_t serial1_ctrl_stack[64];
uint32_t serial2_ctrl_stack[64];
uint32_t serial1_recv_stack[(VCOM_BUF_SIZE / 4) + 64];
uint32_t serial2_recv_stack[(VCOM_BUF_SIZE / 4) + 64];
uint32_t usb_recv_stack[(VCOM_BUF_SIZE / 4) + 64];

int main(int argc, char ** argv)
{
	uint64_t esn;
	struct vcom vcom[2];
	unsigned int i;
//	uint8_t buf[1024];

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	stdout = null_fopen("");
	stderr = stdout;
	stdin = stdout;

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(7));

	leds_init();

	serial1 = serial2_open();
	serial2 = serial3_open();

	vcom[0].serial = serial2_open();
	vcom[1].serial = serial3_open();

	esn = *((uint64_t *)STM32F_UID);
	DCC_LOG2(LOG_TRACE, "ESN=0x%08x%08x", esn >> 32, esn);

	vcom[0].cdc = usb_cdc_init(&stm32f_usb_fs_dev, esn);
	vcom[1].cdc = vcom[0].cdc;


	thinkos_thread_create((void *)button_task, (void *)NULL,
						  button_stack, sizeof(button_stack),
						  THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(5));

#if 0
	thinkos_thread_create((void *)usb_recv_task, (void *)vcom,
						  usb_recv_stack, sizeof(usb_recv_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(0));

#endif

#if 0
	thinkos_thread_create((void *)serial_recv_task, (void *)&vcom[0],
						  serial1_recv_stack, sizeof(serial1_recv_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	thinkos_thread_create((void *)serial_ctrl_task, (void *)&vcom[0],
						  serial1_ctrl_stack, sizeof(serial1_ctrl_stack),
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(3));

	thinkos_thread_create((void *)serial_recv_task, (void *)&vcom[1],
						  serial2_recv_stack, sizeof(serial2_recv_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(2));

#endif
	thinkos_thread_create((void *)serial_ctrl_task, (void *)&vcom[1],
						  serial2_ctrl_stack, sizeof(serial2_ctrl_stack),
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(4));

#if 1
	led_flash_all(3);
#endif

	usb_vbus(true);

	usb_console(vcom[0].cdc);

	for (i = 0; ; ++i) {
		thinkos_sleep(5000);
//		vcom_xmodem_recv(vcom);

		DCC_LOG1(LOG_TRACE, "tick %d.", i);
	}

	return 0;
}

