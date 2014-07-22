/* 
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the usb-serial converter.
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
 * @file usb-serial.c
 * @brief
 * @author Robinson Mittmann <bobmittmann@gmail.com>
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
#include <xmodem.h>

#include "usb-serial.h"
#include "board.h"
#include "led.h"
#include "io.h"

struct {
	volatile int req;
	int flag;
} test_mod;

static void usb_test(usb_cdc_class_t * cdc)
{
	char buf[128];
	int i;
	int c;

	led2_flash(3);

	c = ' ';
	for (i = 0; i < 128; ++i) {
		buf[i] = ++c;
		if (c == 'z')
			c = ' ';
	}

	usb_cdc_write(cdc, buf, 64);
}

int usb_send(int ep_id, const void * buf, unsigned int len);
int usb_recv(int ep_id, void * buf, unsigned int len, unsigned int msec);
#define EP_OUT_ADDR 1
#define EP_IN_ADDR  2
int usb_xflash(uint32_t blk_offs, unsigned int blk_size);

extern uint32_t _stack;

void xflash_test(void)
{
//	uint32_t stack = (uint32_t)&_stack;
//	char buf[128];
//	int n;
//	DCC_LOG1(LOG_TRACE, "stack=%08x...", stack);

	led1_on();
	led2_on();

	cm3_cpsid_i();
//	cm3_sp_set(stack);

	usb_xflash(0x00000, 65536);
/*
	usb_send(EP_IN_ADDR, "--------------\r\n", 16);

	while ((n = usb_recv(EP_OUT_ADDR, buf, 128, 2000)) > 0) {
		usb_send(EP_IN_ADDR, buf, n);
	}

	usb_send(EP_IN_ADDR, "--------------\r\n", 16);
*/
	cm3_cpsie_i();
}

void test_sched(int test)
{
	test_mod.req = test;
	thinkos_flag_set(test_mod.flag);
}

void __attribute__((noreturn)) test_mod_task(struct vcom * vcom)
{
	struct usb_cdc_class * cdc = vcom->cdc;
	int test;

	test_mod.flag  = thinkos_flag_alloc();

	while (1) {
		thinkos_flag_wait(test_mod.flag);
		test = test_mod.req;
		thinkos_flag_clr(test_mod.flag);

		switch (test) {
		case TEST_USB:
			usb_test(cdc);
			break;
		case TEST_XFLASH:
			xflash_test();
			break;
		}
	}
}

void test_main(struct vcom * vcom)
{
	test_mod.flag  = thinkos_flag_alloc();
	test_mod.req = TEST_NONE;

	test_mod_task(vcom);
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
			stm32_flash_erase(32768, 32768);
			break;

		case 'p':
			printf("Programming memory\n");
			stm32_flash_write(32768, "Hello world!", 12);
			break;

		case 'q':
			printf("quit\n");
			return 0;

		default:
			printf("-------------\n");
		}
	}

	return 0;
}

