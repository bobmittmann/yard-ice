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

#include "board.h"

#include <string.h>
#include <stdint.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/usb-cdc.h>

#include <sys/dcclog.h>

#include "sdu.h"
#include "mstp.h"
#include "trace.h"
#include "profclk.h"

#define FW_VERSION_MAJOR 1
#define FW_VERSION_MINOR 5

uint8_t fw_version[2] = { FW_VERSION_MAJOR, FW_VERSION_MINOR };

/* -------------------------------------------------------------------------
   USB-CDC ACM
   ------------------------------------------------------------------------- */

#define LANG_STR_SZ              4
static const uint8_t lang_str[LANG_STR_SZ] = {
	/* LangID = 0x0409: U.S. English */
	LANG_STR_SZ, USB_DESCRIPTOR_STRING, 0x09, 0x04
};

#define VENDOR_STR_SZ            26
static const uint8_t vendor_str[VENDOR_STR_SZ] = {
	VENDOR_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Manufacturer: "Mircom Group" */
	'M', 0, 'i', 0, 'r', 0, 'c', 0, 'o', 0, 'm', 0, 
	' ', 0, 'G', 0, 'r', 0, 'o', 0, 'u', 0, 'p', 0
};


#define PRODUCT_STR_SZ           52
static const uint8_t product_str[PRODUCT_STR_SZ] = {
	PRODUCT_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Product name: "Mircom U2S485 Adapter" */
	'M', 0, 'i', 0, 'r', 0, 'c', 0, 'o', 0, 'm', 0, ' ', 0, 'U', 0, 
	'2', 0, 'S', 0, '4', 0, '8', 0, '5', 0, ' ', 0, 'A', 0, 'd', 0, 
	'a', 0, 'p', 0, 't', 0, 'e', 0, 'r', 0, ' ', 0, 
	'0' + FW_VERSION_MAJOR, 0, '.', 0, '0' + FW_VERSION_MINOR, 0, 
};


#define SERIAL_STR_SZ            26
static const uint8_t serial_str[SERIAL_STR_SZ] = {
	SERIAL_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Serial number: "553273343835" */
	'5', 0, '5', 0, '3', 0, '2', 0, '7', 0, '3', 0, 
	'3', 0, '4', 0, '3', 0, '8', 0, '3', 0, '5', 0
};


#define INTERFACE_STR_SZ         16
static const uint8_t interface_str[INTERFACE_STR_SZ] = {
	INTERFACE_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Interface 0: "ST VCOM" */
	'S', 0, 'T', 0, ' ', 0, 'V', 0, 'C', 0, 'O', 0, 'M', 0
};

const uint8_t * const cdc_acm_str[] = {
	lang_str,
	vendor_str,
	product_str,
	serial_str,
	interface_str
};

const uint8_t cdc_acm_strcnt = sizeof(cdc_acm_str) / sizeof(uint8_t *);

/* -------------------------------------------------------------------------
   Virtual COM 
   ------------------------------------------------------------------------- */

struct serial_dev * serial2_open(void);

enum {
	VCOM_MODE_NONE = 0,
	VCOM_MODE_CONVERTER = 1,
	VCOM_MODE_SERVICE = 2,
	VCOM_MODE_INTERACTIVE = 3,
	VCOM_MODE_SDU_TRACE = 4,
	VCOM_MODE_MSTP_TRACE = 5,
	VCOM_MODE_RAW_TRACE = 6
};

struct vcom {
	volatile int mode;
	struct serial_config cfg;
	usb_cdc_class_t * cdc;
	uint32_t ticks_per_byte;
	struct serial_dev * serial;
	struct serial_status ser_stat;
	struct {
		const char * pat;
		int pos;
	} scan;
};

/* -------------------------------------------------------------------------
   Stream scan / pattern match
   ------------------------------------------------------------------------- */

static void vcom_scan_config(struct vcom * vcom, const char * pattern)
{
	vcom->scan.pat = pattern;
	vcom->scan.pos = 0;
}

static bool vcom_scan_match(struct vcom * vcom, uint8_t buf[], int len)
{
	uint8_t * pat = (uint8_t *)vcom->scan.pat;
	int j = vcom->scan.pos;
	int c = pat[j];
	int i;

	for (i = 0; i < len; ++i) {
		if (buf[i] == c) {
			j++;
			c = pat[j];
			if (c == '\0') {
				vcom->scan.pos = 0;
				return true;
			}
		} else {
			/* mismatch, restart */
			j = 0;
			c = pat[0];
		}
	}

	vcom->scan.pos = j;
	return false;
}

/* -------------------------------------------------------------------------
   Firmware update 
   ------------------------------------------------------------------------- */

extern const uint8_t usb_xflash_pic[];
extern const unsigned int sizeof_usb_xflash_pic;
extern uint32_t __data_start[]; 

void __attribute__((noreturn)) usb_xflash(uint32_t offs, uint32_t len)
{
	uint32_t * xflash_code = __data_start;
	int (* xflash_ram)(uint32_t, uint32_t) = ((void *)xflash_code) + 1;

	DCC_LOG3(LOG_TRACE, "sp=%08x offs=%08x len=%d", cm3_sp_get(), offs, len);

	cm3_cpsid_i();

	memcpy(xflash_code, usb_xflash_pic, sizeof_usb_xflash_pic);

	xflash_ram(offs, len);

	cm3_sysrst();
}

/* -------------------------------------------------------------------------
   Debug and trace 
   ------------------------------------------------------------------------- */
void show_menu(usb_cdc_class_t * cdc)
{
	usb_printf(cdc, "\r\n - Service options:\r\n");
	usb_printf(cdc, "    [1]   9600 8N1\r\n");
	usb_printf(cdc, "    [2]  19200 8N1\r\n");
	usb_printf(cdc, "    [3]  38400 8N1\r\n");
	usb_printf(cdc, "    [4]  57600 8N1\r\n");
	usb_printf(cdc, "    [5] 115200 8N1\r\n");
	usb_printf(cdc, "    [6] 500000 8N1\r\n");
	usb_printf(cdc, "    [q] quit service mode\r\n");
	usb_printf(cdc, "    [F] firmware update\r\n");
	usb_printf(cdc, "  [S/s] enable/disable SDU trace\r\n");
	usb_printf(cdc, "  [B/b] enable/disable BACnet MS/TP trace\r\n");
	usb_printf(cdc, "  [A/a] absolute/relative time\r\n");
	usb_printf(cdc, "  [U/u] enable/disable supervisory\r\n");
	usb_printf(cdc, "  [P/p] enable/disable packets\r\n");
	usb_printf(cdc, "  [R/r] enable/disable raw data trace\r\n");
	usb_printf(cdc, "  [X/x] enable/disable XON/XOFF flow control\r\n");
	usb_printf(cdc, "[U2S-485 %d.%d]: ", FW_VERSION_MAJOR, FW_VERSION_MINOR);
};

uint32_t protocol_buf[512];

void vcom_service_input(struct vcom * vcom, uint8_t buf[], int len)
{
	usb_cdc_class_t * cdc = vcom->cdc;
	int i;
	int c;

	if (vcom->mode == VCOM_MODE_SERVICE) {
		usb_printf(cdc, "\r\n\r\n");
		usb_printf(cdc, "--- U2S-485 %d.%d -------------\r\n",
				   FW_VERSION_MAJOR, FW_VERSION_MINOR);
		usb_printf(cdc, " - Service mode ...\r\n");
		vcom->mode = VCOM_MODE_INTERACTIVE;
	}

	for (i = 0; i < len; ++i) {
		c = buf[i];
		(void)c;

		switch (c) {
		case 'q':
			usb_printf(cdc, " - Converter mode...\r\n");
			vcom->mode = VCOM_MODE_CONVERTER;
			break;
		case 'S':
			usb_printf(cdc, " - SDU trace mode...\r\n");
			vcom->mode = VCOM_MODE_SDU_TRACE;
			vcom->ticks_per_byte = ((PROFCLK_HZ) * 10) / vcom->cfg.baudrate;
			sdu_trace_init(cdc, (void *)protocol_buf);
			break;
		case 'B':
			usb_printf(cdc, " - BACnet MS/TP trace mode...\r\n");
			vcom->mode = VCOM_MODE_MSTP_TRACE;
			vcom->ticks_per_byte = ((PROFCLK_HZ) * 10) / vcom->cfg.baudrate;
			mstp_trace_init(cdc, (void *)protocol_buf);
			break;
		case 'R':
			usb_printf(cdc, " - Raw data trace mode...\r\n");
			vcom->mode = VCOM_MODE_RAW_TRACE;
			vcom->ticks_per_byte = ((PROFCLK_HZ) * 10) / vcom->cfg.baudrate;
			raw_trace_init(cdc, (void *)protocol_buf);
			break;
		case 'r':
		case 'b':
		case 's':
			usb_printf(cdc, " - Interactive mode...\r\n");
			vcom->mode = VCOM_MODE_INTERACTIVE;
			break;
		case 'X':
			usb_printf(cdc, " - XON/XOFF enabled...\r\n");
			serial_flowctrl_set(vcom->serial, SERIAL_FLOWCTRL_XONXOFF);
			break;
		case 'x':
			usb_printf(cdc, " - XON/XOFF disabled...\r\n");
			serial_flowctrl_set(vcom->serial, SERIAL_FLOWCTRL_NONE);
			break;

		case 'F':
			if (vcom->mode == VCOM_MODE_INTERACTIVE) {
				usb_printf(cdc, " - Firmware update...\r\n");
				usb_xflash(0, 32 * 1024);
			}
			break;
		case '1':
			vcom->cfg.baudrate = 9600;
			vcom->cfg.databits = 8;
			vcom->cfg.parity = 0;
			vcom->cfg.stopbits = 1;
			serial_rx_disable(vcom->serial);
			serial_config_set(vcom->serial, &vcom->cfg);
			serial_rx_enable(vcom->serial);
			usb_printf(cdc, " - 9600 8N1\r\n");
			break;
		case '2':
			vcom->cfg.baudrate = 19200;
			vcom->cfg.databits = 8;
			vcom->cfg.parity = 0;
			vcom->cfg.stopbits = 1;
			serial_rx_disable(vcom->serial);
			serial_config_set(vcom->serial, &vcom->cfg);
			serial_rx_enable(vcom->serial);
			usb_printf(cdc, " - 19200 8N1\r\n");
			break;
		case '3':
			vcom->cfg.baudrate = 38400;
			vcom->cfg.databits = 8;
			vcom->cfg.parity = 0;
			vcom->cfg.stopbits = 1;
			serial_rx_disable(vcom->serial);
			serial_config_set(vcom->serial, &vcom->cfg);
			serial_rx_enable(vcom->serial);
			usb_printf(cdc, " - 38400 8N1\r\n");
			break;
		case '4':
			vcom->cfg.baudrate = 57600;
			vcom->cfg.databits = 8;
			vcom->cfg.parity = 0;
			vcom->cfg.stopbits = 1;
			serial_rx_disable(vcom->serial);
			serial_config_set(vcom->serial, &vcom->cfg);
			serial_rx_enable(vcom->serial);
			usb_printf(cdc, " - 57600 8N1\r\n");
			break;
		case '5':
			vcom->cfg.baudrate = 115200;
			vcom->cfg.databits = 8;
			vcom->cfg.parity = 0;
			vcom->cfg.stopbits = 1;
			serial_rx_disable(vcom->serial);
			serial_config_set(vcom->serial, &vcom->cfg);
			serial_rx_enable(vcom->serial);
			usb_printf(cdc, " - 115200 8N1\r\n");
			break;

		case '6':
			vcom->cfg.baudrate = 500000;
			vcom->cfg.databits = 8;
			vcom->cfg.parity = 0;
			vcom->cfg.stopbits = 1;
			serial_rx_disable(vcom->serial);
			serial_config_set(vcom->serial, &vcom->cfg);
			serial_rx_enable(vcom->serial);
			usb_printf(cdc, " - 500000 8N1\r\n");
			break;

		case 'U':
			sdu_trace_show_supv(true);
			usb_printf(cdc, " - Show supervisory...\r\n");
			break;

		case 'u':
			sdu_trace_show_supv(false);
			usb_printf(cdc, " - Don't show supervisory...\r\n");
			break;

		case 'P':
			sdu_trace_show_pkt(true);
			usb_printf(cdc, " - Show packets...\r\n");
			break;

		case 'p':
			sdu_trace_show_pkt(false);
			usb_printf(cdc, " - Don't show packets...\r\n");
			break;

		case 'A':
			sdu_trace_time_abs(true);
			usb_printf(cdc, " - Absolute timestamps...\r\n");
			break;

		case 'a':
			sdu_trace_time_abs(false);
			usb_printf(cdc, " - Relative timestamps...\r\n");
			break;

		case 'v':
			usb_printf(cdc, "\r\n");
			usb_printf(cdc, 
	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF"
	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF");
			usb_printf(cdc, "\r\n");
			usb_printf(cdc, 
	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF"
	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF");
			usb_printf(cdc, "\r\n");
			usb_printf(cdc, 
	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF"
	"0123456789abcdef0123456789ABCDEF0123456789abcdef0123456789ABCDEF");

/*
			usb_printf(cdc, "\r\n"
					   "1. The qick brown fox jumps over the lazy dog.\r\n"
					   "2. THE QICK BROWN FOX JUMPS OVER THE LAZY DOG.\r\n");
			usb_printf(cdc, 
					   "3. The qick brown fox jumps over the lazy dog.\r\n"
					   "4. THE QICK BROWN FOX JUMPS OVER THE LAZY DOG.\r\n");
			usb_printf(cdc, 
					   "5. The qick brown fox jumps over the lazy dog.\r\n");
			usb_printf(cdc, 
					   "6. THE QICK BROWN FOX JUMPS OVER THE LAZY DOG.\r\n");
*/
			break;
		case 't':
			led_flash(LED_RED, 50);
			serial_send(vcom->serial, 
						"The qick brown fox jumps over the lazy dog.\r\n", 
						45);
			break;
		default:
			show_menu(cdc);
		}
	}
}

#define VCOM_BUF_SIZE 64

void __attribute__((noreturn)) usb_recv_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	usb_cdc_class_t * cdc = vcom->cdc;
	uint8_t buf[VCOM_BUF_SIZE];
	uint32_t usb_stat_rx = 0;
	struct serial_stat stat;
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());
	DCC_LOG2(LOG_TRACE, "vcom->%p, cdc->%p", vcom, cdc);

	stat.rx_cnt = 0;
	stat.tx_cnt = 0;

	for (;;) {
		len = usb_cdc_read(cdc, buf, VCOM_BUF_SIZE, 1000);
		if (len > 0) {
			usb_stat_rx += len;
		} else if (len == THINKOS_ETIMEDOUT) {
			struct serial_stat tmp;
			uint32_t rx_cnt;
			uint32_t tx_cnt;
			
			if (usb_stat_rx) {
				DCC_LOG1(LOG_TRACE, "USB rx=%d", usb_stat_rx);
				usb_stat_rx = 0;
			}

			serial_stat_get(serial, &tmp);
			rx_cnt = tmp.rx_cnt - stat.rx_cnt;
			tx_cnt = tmp.tx_cnt - stat.tx_cnt;
			stat.rx_cnt = tmp.rx_cnt;
			stat.tx_cnt = tmp.tx_cnt;

			if (rx_cnt || tx_cnt)
				DCC_LOG2(LOG_TRACE, "Serial rx=%d tx=%d", rx_cnt, tx_cnt);
		}
		if (vcom->mode == VCOM_MODE_CONVERTER) {
			if (len > 0) {
				led_flash(LED_RED, 50);
				serial_send(serial, buf, len);
			}
		} else {
			// forward to service input
			vcom_service_input(vcom, buf, len);
		}
	}
}


/* XXX: If the input serial stream mathces this string it
   will assume that we are connected to a DSP5683 core board in
   boot loader monitor mode. This will trigger the XON/XOFF 
   flow control mode. 
   The reason for that being the fact that the USB CDC/ACM windows 
   driver (usbser.sys) do not implements XON/XOFF and the USB CDC
   standard do not provide any mechanism to configure flow cotrol.
 */

const char dsp5685_xonxoff_magic[] = "Motorola Inc. "
	"5685x Serial Data Flash Programmer. Version";

void __attribute__((noreturn)) serial_recv_task(struct vcom * vcom)
{
	struct serial_dev * serial = vcom->serial;
	struct usb_cdc_class * cdc = vcom->cdc;
	uint8_t buf[VCOM_BUF_SIZE];
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());

	vcom_scan_config(vcom, dsp5685_xonxoff_magic);

	/* wait for line configuration */
	usb_cdc_acm_lc_wait(cdc);

	/* enable serial */
	serial_enable(serial);

	for (;;) {
		len = serial_recv(serial, buf, VCOM_BUF_SIZE, 1000);
		if (len > 0) {
			uint32_t ts;
			ts = profclk_get();
			if (vcom->mode == VCOM_MODE_CONVERTER) {
				led_flash(LED_AMBER, 50);
				usb_cdc_write(cdc, buf, len);
				if (vcom_scan_match(vcom, buf, len)) {
					DCC_LOG(LOG_WARNING, "DSP5685 XON/XOFF magic!");
					vcom->cfg.flowctrl = SERIAL_FLOWCTRL_XONXOFF;
					serial_flowctrl_set(vcom->serial, SERIAL_FLOWCTRL_XONXOFF);
				}
			} else if (vcom->mode == VCOM_MODE_SDU_TRACE) {
				led_flash(LED_AMBER, 50);
				ts -= vcom->ticks_per_byte * len;
				sdu_decode(ts, buf, len);
			} else if (vcom->mode == VCOM_MODE_MSTP_TRACE) {
				led_flash(LED_AMBER, 50);
				ts -= vcom->ticks_per_byte * len;
				mstp_decode(ts, buf, len);
			} else if (vcom->mode == VCOM_MODE_RAW_TRACE) {
				led_flash(LED_AMBER, 50);
				ts -= vcom->ticks_per_byte * len;
				raw_trace(ts, buf, len);
			}
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

	usb_cdc_state_get(cdc, &prev_state);

	while (1) {
		DCC_LOG1(LOG_INFO, "[%d] usb_cdc_ctl_wait() sleep!", 
				 thinkos_thread_self());
		usb_cdc_ctl_wait(cdc, 0);
		DCC_LOG1(LOG_TRACE, "[%d] wakeup ---------- ", thinkos_thread_self());

		usb_cdc_state_get(cdc, &state);

		if (state.flags != prev_state.flags) {
			if (state.suspended) {
				DCC_LOG1(LOG_TRACE, "[%d] suspending...", 
						 thinkos_thread_self());
			} else {
				if (prev_state.suspended) {
					DCC_LOG1(LOG_TRACE, "[%d] wakeup from suspended!", 
							 thinkos_thread_self());
				} 
			}
			prev_state.flags = state.flags;
		}

		if (state.ctrl.dtr != prev_state.ctrl.dtr) {
			vcom->ser_stat.dsr = state.ctrl.dtr;
			usb_cdc_status_set(cdc, &vcom->ser_stat);
			prev_state.ctrl = state.ctrl;
		}

		if ((state.cfg.baudrate != vcom->cfg.baudrate) ||
			(state.cfg.databits != vcom->cfg.databits) ||
			(state.cfg.parity != vcom->cfg.parity) ||
			(state.cfg.stopbits != vcom->cfg.stopbits) ||
			(state.cfg.flowctrl != vcom->cfg.flowctrl)) {

			serial_rx_disable(serial);

			DCC_LOG1(LOG_TRACE, "baudrate=%d", state.cfg.baudrate);
			DCC_LOG1(LOG_TRACE, "databits=%d", state.cfg.databits);
			DCC_LOG1(LOG_TRACE, "parity=%d", state.cfg.parity);
			DCC_LOG1(LOG_TRACE, "stopbits=%d", state.cfg.stopbits);
			if (state.cfg.baudrate == 921600) {
				vcom->mode = VCOM_MODE_SERVICE; 
				DCC_LOG(LOG_TRACE, "Service mode magic config ...");
			} else {
				/* XXX: Big hack, enable XON/XOFF flow control by either enabling
				   it explicitly or by combining Parity=Mark with Stopbits=2.
				   This is to enable XON/XOFF in case this driver is used to 
				   implement a USB to serial converter. The CDC-ACM windows driver
				   usbser.sys does not handle the XON/XOFF codes and the USB-CDC-ACM
				   specification omits flow control altogether. */
				if (state.cfg.parity == SERIAL_PARITY_SPACE ||
					state.cfg.parity == SERIAL_PARITY_MARK) {
					DCC_LOG(LOG_TRACE, "XON/XOFF magic config ...");
					state.cfg.flowctrl = SERIAL_FLOWCTRL_XONXOFF;
				}
				vcom->mode = VCOM_MODE_CONVERTER;
				vcom->cfg = state.cfg;
				serial_config_set(serial, &vcom->cfg);
				serial_rx_enable(serial);
			}
		} else {
			DCC_LOG(LOG_TRACE, "keeping serial config.");
			DCC_LOG1(LOG_TRACE, "baudrate=%d", vcom->cfg.baudrate);
			DCC_LOG1(LOG_TRACE, "databits=%d", vcom->cfg.databits);
			DCC_LOG1(LOG_TRACE, "parity=%d", vcom->cfg.parity);
			DCC_LOG1(LOG_TRACE, "stopbits=%d", vcom->cfg.stopbits);
		}
	}
}

uint32_t __attribute__((aligned(8))) led_stack[32];
uint32_t __attribute__((aligned(8))) serial_ctrl_stack[64];
uint32_t __attribute__((aligned(8))) serial_recv_stack[192];

int main(int argc, char ** argv)
{
	struct usb_cdc_class * cdc;
	struct serial_dev * serial;
	struct vcom vcom;
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(3) | THINKOS_OPT_ID(3));

	leds_init();

	cdc = usb_cdc_init(&stm32f_usb_fs_dev, cdc_acm_str, 
					   cdc_acm_strcnt);

	serial = serial2_open();

	vcom.serial = serial;
	vcom.cdc = cdc;
	vcom.mode = VCOM_MODE_NONE;
	vcom.cfg.baudrate = 19200;
	vcom.cfg.databits = 8;
	vcom.cfg.parity = 0;
	vcom.cfg.stopbits = 1;

	thinkos_thread_create((void *)led_task, (void *)NULL,
						  led_stack, sizeof(led_stack) |
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	thinkos_thread_create((void *)serial_recv_task, 
						  (void *)&vcom,
						  serial_recv_stack, sizeof(serial_recv_stack) |
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	thinkos_thread_create((void *)serial_ctrl_task, 
						  (void *)&vcom,
						  serial_ctrl_stack, sizeof(serial_ctrl_stack) |
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));

	usb_vbus(true);

	for (i = 0; i < 5; ++i) {
		led_on(LED_AMBER);
		thinkos_sleep(100);
		led_off(LED_AMBER);
		led_on(LED_RED);
		thinkos_sleep(100);
		led_off(LED_RED);
	}

	for (;;) {
		usb_recv_task(&vcom);
	}
}

