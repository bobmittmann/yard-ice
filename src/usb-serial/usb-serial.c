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

#include <sys/serial.h>
#include <sys/dcclog.h>
#include <sys/usb-cdc.h>

#include <xmodem.h>

#include "board.h"
#include "led.h"

struct serial_dev * serial2_open(void);
struct serial_dev * serial3_open(void);

struct vcom {
	struct serial_dev * serial;
	usb_cdc_class_t * cdc;
	struct serial_status ser_stat;
};

#define VCOM_BUF_SIZE 128

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

void __attribute__((noreturn)) button_task(void)
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

//	return 0;
}


void __attribute__((noreturn)) usb_recv_task(struct vcom vcom[])
{
	struct serial_dev * serial1 = vcom[0].serial;
//	struct serial_dev * serial2 = vcom[1].serial;
	usb_cdc_class_t * cdc = vcom[0].cdc;
	char buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	for (;;) {
		len = usb_cdc_read(cdc, buf, VCOM_BUF_SIZE, 5000);
		if (len > 0) {
			led1_flash(1);
			DCC_LOG1(LOG_TRACE, "USB RX: %d bytes.", len);
			serial_write(serial1, buf, len);
//			serial_write(serial2, buf, len);
		}

	}
}

void __attribute__((noreturn)) serial_recv_task(struct vcom * vcom)
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
			led2_flash(1);
			usb_cdc_write(cdc, buf, len);
		}
	}
}

void __attribute__((noreturn)) serial_ctrl_task(struct vcom * vcom)
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
}


struct xmodem_snd sx;
struct xmodem_rcv rx;

int vcom_xmodem_recv(struct vcom * vcom)
{
	usb_cdc_class_t * cdc = vcom[0].cdc;
	struct comm_dev comm;
	uint8_t buf[8];
	int ret;
	int cnt;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	comm.arg = cdc;
	comm.op.send = (int (*)(void *, const void *, unsigned int))usb_cdc_write;
	comm.op.recv = (int (*)(void *, void *, 
						  unsigned int, unsigned int))usb_cdc_read;
	
	xmodem_rcv_init(&rx, &comm, XMODEM_RCV_CRC);

	cnt = 0;
	do {
		if ((ret = xmodem_rcv_loop(&rx, buf, 8)) < 0) {
			DCC_LOG1(LOG_ERROR, "ret=%d", ret);
			return ret;
		}
		cnt += ret;
	} while (ret > 0);

	return cnt;
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

int serial_xmodem_send(struct serial_dev * serial)
{
	struct comm_dev comm;
	char data[32] = "Hello world!";
	int ret;
	int i;

	comm.arg = serial;
	comm.op.send = (int (*)(void *, const void *, unsigned int))serial_write;
	comm.op.recv = (int (*)(void *, void *, 
						  unsigned int, unsigned int))serial_read;

	DCC_LOG(LOG_TRACE, "........................");

	xmodem_snd_init(&sx, &comm, XMODEM_SND_1K);

	for (i = 0; i < 32451; ++i) {
		if ((ret = xmodem_snd_loop(&sx, data, 32)) < 0) {
			DCC_LOG1(LOG_ERROR, "ret=%d", ret);
			return ret;
		}
	}

	return xmodem_snd_eot(&sx);
}

int serial_xmodem_recv(struct serial_dev * serial)
{
	struct comm_dev comm;
	uint8_t buf[8];
	int ret;
	int cnt;

	comm.arg = serial;
	comm.op.send = (int (*)(void *, const void *, unsigned int))serial_write;
	comm.op.recv = (int (*)(void *, void *, 
						  unsigned int, unsigned int))serial_read;

	DCC_LOG(LOG_TRACE, ".................................");

	xmodem_rcv_init(&rx, &comm, XMODEM_RCV_CRC);

	cnt = 0;
	do {
		if ((ret = xmodem_rcv_loop(&rx, buf, 8)) < 0) {
			DCC_LOG1(LOG_ERROR, "ret=%d", ret);
			return ret;
		}
		cnt += ret;
	} while (ret > 0);

	return cnt;
}

void show_menu(FILE * f)
{
	fprintf(f, "\n");
	fprintf(f, " Options:\n");
	fprintf(f, " --------\n");
	fprintf(f, "   r - xmodem receive\n");
	fprintf(f, "   s  - xmodem send\n");
	fprintf(f, "   1  - turn on TX2 PIN\n");
	fprintf(f, "   0  - turn off TX2 PIN\n");
	fprintf(f, "   d  - default pin assignment.\n");
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
			printf("XMODEM send...\n");
			serial_xmodem_send(serial1);
			printf("...\n");
			break;

		case 'r':
			printf("XMODEM receive...\n");
			serial_xmodem_recv(serial1);
			printf("...\n");
			break;

		case 'y':
			printf("PIN1 -> GND\n");
			pin1_sel_gnd();
			printf("PIN2 -> VCC\n");
			pin2_sel_vcc();
			printf("PIN4 -> UART2.RX\n");
			pin4_sel_usart2_rx();
			printf("PIN5 -> UART2.TX\n");
			pin5_sel_usart2_tx();
			break;

		case 'x':
			printf("PIN1 -> UART3.RX\n");
			pin1_sel_usart3_rx();
			printf("PIN2 -> UART3.TX\n");
			pin2_sel_usart3_tx();
			printf("PIN4 -> UART2.RX\n");
			pin4_sel_usart2_rx();
			printf("PIN5 -> UART2.TX\n");
			pin5_sel_usart2_tx();
			break;

		case '0':
			printf("PIN1 -> GND\n");
			pin1_sel_gnd();
			break;

		case '1':
			printf("PIN1 -> VCC\n");
			pin1_sel_vcc();
			break;

		case '2':
			printf("PIN1 <- input\n");
			pin1_sel_input();
			break;

		case '3':
			printf("PIN2 -> GND\n");
			pin2_sel_gnd();
			break;

		case '4':
			printf("PIN2 -> VCC\n");
			pin2_sel_vcc();
			break;

		case '5':
			printf("PIN2 <- input\n");
			pin2_sel_input();
			break;

		case '6':
			printf("PIN4 -> GND\n");
			pin4_sel_gnd();
			break;

		case '7':
			printf("PIN4 -> VCC\n");
			pin4_sel_vcc();
			break;

		case '8':
			printf("PIN4 <- input\n");
			pin4_sel_input();
			break;

		case 'a':
			printf("PIN5 -> GND\n");
			pin5_sel_gnd();
			break;

		case 'b':
			printf("PIN5 -> VCC\n");
			pin5_sel_vcc();
			break;

		case 'c':
			printf("PIN5 <- input\n");
			pin5_sel_input();
			break;

		case 'e':
			printf("Erasing memory\n");
			stm32f_flash_erase(32768, 32768);
			break;

		case 'p':
			printf("Programming memory\n");
			stm32f_flash_write(32768, "Hello world!", 12);
			break;


		default:
			printf("-------------\n");
		}
	}

	return 0;
}

#define RECV_STACK_SIZE (VCOM_BUF_SIZE + 256)

uint32_t __attribute__((aligned(8))) button_stack[32];
uint32_t __attribute__((aligned(8))) serial1_ctrl_stack[64];
uint32_t __attribute__((aligned(8))) serial2_ctrl_stack[64];
uint32_t __attribute__((aligned(8))) serial1_recv_stack[RECV_STACK_SIZE / 4];
uint32_t __attribute__((aligned(8))) serial2_recv_stack[RECV_STACK_SIZE / 4];
uint32_t __attribute__((aligned(8))) usb_recv_stack[RECV_STACK_SIZE / 4];

int main(int argc, char ** argv)
{
	uint64_t esn;
	struct vcom vcom[2];
	unsigned int i;

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

	vcom[0].serial = serial1;
	vcom[1].serial = serial2;

	esn = *((uint64_t *)STM32F_UID);
	DCC_LOG2(LOG_TRACE, "ESN=0x%08x%08x", esn >> 32, esn);

	vcom[0].cdc = usb_cdc_init(&stm32f_usb_fs_dev, esn);
	vcom[1].cdc = vcom[0].cdc;


	thinkos_thread_create((void *)button_task, (void *)NULL,
						  button_stack, sizeof(button_stack),
						  THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(5));

	thinkos_thread_create((void *)usb_recv_task, (void *)vcom,
						  usb_recv_stack, sizeof(usb_recv_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(0));

	thinkos_thread_create((void *)serial_recv_task, (void *)&vcom[0],
						  serial1_recv_stack, sizeof(serial1_recv_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	thinkos_thread_create((void *)serial_ctrl_task, (void *)&vcom[0],
						  serial1_ctrl_stack, sizeof(serial1_ctrl_stack),
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(3));
#if 0
	thinkos_thread_create((void *)serial_recv_task, (void *)&vcom[1],
						  serial2_recv_stack, sizeof(serial2_recv_stack),
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(2));

	thinkos_thread_create((void *)serial_ctrl_task, (void *)&vcom[1],
						  serial2_ctrl_stack, sizeof(serial2_ctrl_stack),
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(4));
#endif

	led_flash_all(3);

	usb_vbus(true);

	pin1_sel_gnd();
	pin2_sel_vcc();
//	usb_console(vcom[0].cdc);

	for (i = 0; ; ++i) {
		thinkos_sleep(5000);
//		vcom_xmodem_recv(vcom);

		DCC_LOG1(LOG_TRACE, "tick %d.", i);
	}

	return 0;
}

