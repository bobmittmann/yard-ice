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
 * @file u2s-485.c
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
#include "trace.h"
#include "profclk.h"

void __attribute__((noreturn)) usb_xflash(uint32_t offs, uint32_t len);
struct serial_dev * serial2_open(void);


#define FW_VERSION_MAJOR 1
#define FW_VERSION_MINOR 11

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


#if FW_VERSION_MINOR > 9
#define PRODUCT_STR_SZ           54
#else
#define PRODUCT_STR_SZ           52
#endif
static const uint8_t product_str[PRODUCT_STR_SZ] = {
	PRODUCT_STR_SZ, USB_DESCRIPTOR_STRING,
	/* Product name: "Mircom U2S485 Adapter" */
	'M', 0, 'i', 0, 'r', 0, 'c', 0, 'o', 0, 'm', 0, ' ', 0, 'U', 0, 
	'2', 0, 'S', 0, '4', 0, '8', 0, '5', 0, ' ', 0, 'A', 0, 'd', 0, 
	'a', 0, 'p', 0, 't', 0, 'e', 0, 'r', 0, ' ', 0, 
	'0' + FW_VERSION_MAJOR, 0, '.', 0, 
#if FW_VERSION_MINOR > 9
	'0' + (FW_VERSION_MINOR / 10), 0, 
	'0' + (FW_VERSION_MINOR % 10), 0, 
#else 
	'0' + FW_VERSION_MINOR, 0, 
#endif
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

enum {
	VCOM_MODE_NONE        = 0,
	VCOM_MODE_CONVERTER   = 1,
	VCOM_MODE_SERVICE     = 2,
	VCOM_MODE_INTERACTIVE = 3,
	VCOM_MODE_SDU_TRACE   = 4,
	VCOM_MODE_RAW_TRACE   = 5
};

struct vcom {
	volatile int mode;
	struct serial_config cfg;
	bool cfg_lock;
	usb_cdc_class_t * cdc;
	uint32_t ticks_per_byte;
	struct serial_dev * serial;
	struct serial_status ser_stat;
	struct serial_stats stats;
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

#if 0
	buf[len] = '\0';
	c = buf[0];
	if ((len > 1) || ((c != '-') && (c != '/') && 
					 (c != '\\') && (c != '\r'))) {
		DCC_LOGSTR(LOG_TRACE, "\"%s\"", buf);
	}
#endif

	c = pat[j];
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
	usb_printf(cdc, "    [t] print serial statistics\r\n");
	usb_printf(cdc, "    [q] quit service mode\r\n");
	usb_printf(cdc, "    [F] firmware update\r\n");
	usb_printf(cdc, "  [A/a] absolute/relative time\r\n");
	usb_printf(cdc, "  [L/l] lock/unlock configuration\r\n");
	usb_printf(cdc, "  [P/p] enable/disable packets\r\n");
	usb_printf(cdc, "  [R/r] enable/disable raw data trace\r\n");
	usb_printf(cdc, "  [S/s] enable/disable SDU trace\r\n");
	usb_printf(cdc, "  [U/u] enable/disable SDU supervisory\r\n");
	usb_printf(cdc, "  [X/x] enable/disable XON/XOFF flow control\r\n");
	usb_printf(cdc, "[U2S-485 %d.%d]: ", FW_VERSION_MAJOR, FW_VERSION_MINOR);
};

static void vcom_serial_8n1_force_cfg(struct vcom * vcom, 
									  unsigned int baudrate)
{
	vcom->cfg_lock = true;
	vcom->cfg.baudrate = baudrate;
	vcom->cfg.databits = 8;
	vcom->cfg.parity = 0;
	vcom->cfg.stopbits = 1;
	serial_rx_disable(vcom->serial);
	serial_config_set(vcom->serial, &vcom->cfg);
	serial_rx_enable(vcom->serial);
	usb_printf(vcom->cdc, " - %d 8N1 (locked).\r\n", baudrate);
}

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
			sdu_trace_init();
			break;
		case 'R':
			usb_printf(cdc, " - Raw data trace mode...\r\n");
			vcom->mode = VCOM_MODE_RAW_TRACE;
			vcom->ticks_per_byte = ((PROFCLK_HZ) * 10) / vcom->cfg.baudrate;
			raw_trace_init();
			break;
		case 'r':
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

		case 'l':
			if (vcom->cfg_lock) {
				vcom->cfg_lock = false;
				serial_rx_disable(vcom->serial);
				serial_config_set(vcom->serial, &vcom->cfg);
				serial_rx_enable(vcom->serial);
				usb_printf(cdc, " - Config unlocked.\r\n");
			}
			break;

		case 'L':
			if (!vcom->cfg_lock) {
				vcom->cfg_lock = true;
				usb_printf(cdc, " - Config locked.\r\n");
			}
			break;

		case '1':
			vcom_serial_8n1_force_cfg(vcom, 9600);
			break;
		case '2':
			vcom_serial_8n1_force_cfg(vcom, 19200);
			break;
		case '3':
			vcom_serial_8n1_force_cfg(vcom, 38400);
			break;
		case '4':
			vcom_serial_8n1_force_cfg(vcom, 57600);
			break;
		case '5':
			vcom_serial_8n1_force_cfg(vcom, 115200);
			break;
		case '6':
			vcom_serial_8n1_force_cfg(vcom, 500000);
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

		case 't': {
			struct serial_stats stats;
			serial_stats_get(vcom->serial, &stats);
			usb_printf(cdc, "\r\n"
					   " -  RX: %10d\r\n"
					   " -  TX: %10d\r\n"
					   " - Err: %10d\r\n", 
					   stats.rx_cnt, stats.tx_cnt, stats.err_cnt);
			vcom->stats = stats;
			break;
		}
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
	uint32_t usb_rx_cnt = 0;
	struct serial_stats stats;
	int len;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());
	DCC_LOG2(LOG_TRACE, "vcom->%p, cdc->%p", vcom, cdc);

	stats.rx_cnt = 0;
	stats.tx_cnt = 0;

	for (;;) {
		len = usb_cdc_read(cdc, buf, VCOM_BUF_SIZE, 1000);

		DCC_LOG1(LOG_MSG, "rx len=%d", len);

		if (len > 0) {
			usb_rx_cnt += len;
			if (vcom->mode == VCOM_MODE_CONVERTER) {
				led_flash(LED_RED, 50);
				serial_send(serial, buf, len);
			} else {
				// forward to service input
				vcom_service_input(vcom, buf, len);
			}
		} else if (len == THINKOS_ETIMEDOUT) {
			struct serial_stats tmp;
			uint32_t rx_cnt;
			uint32_t tx_cnt;
			
			if (usb_rx_cnt) {
				DCC_LOG1(LOG_TRACE, "USB rx=%d", usb_rx_cnt);
				usb_rx_cnt = 0;
			}

			serial_stats_get(serial, &tmp);
			rx_cnt = tmp.rx_cnt - stats.rx_cnt;
			tx_cnt = tmp.tx_cnt - stats.tx_cnt;
			stats.rx_cnt = tmp.rx_cnt;
			stats.tx_cnt = tmp.tx_cnt;

			if (rx_cnt || tx_cnt)
				DCC_LOG2(LOG_TRACE, "Serial rx=%d tx=%d", rx_cnt, tx_cnt);
		} else {
			DCC_LOG1(LOG_ERROR, "len=%d", len);
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
	uint8_t buf[VCOM_BUF_SIZE + 1];
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
			led_flash(LED_AMBER, 50);
			ts = profclk_get();
			if (vcom->mode == VCOM_MODE_CONVERTER) {
				DCC_LOG(LOG_MSG, "CONVERTER");
				usb_cdc_write(cdc, buf, len);
				if (vcom_scan_match(vcom, buf, len)) {
					DCC_LOG(LOG_WARNING, "DSP5685 XON/XOFF magic!");
					if (!vcom->cfg_lock) {
						/* only update the configuration if we are not locked */
						vcom->cfg.flowctrl = SERIAL_FLOWCTRL_XONXOFF;
						serial_flowctrl_set(vcom->serial, 
											SERIAL_FLOWCTRL_XONXOFF);
					}
				}
			} else if (vcom->mode == VCOM_MODE_SDU_TRACE) {
				ts -= vcom->ticks_per_byte * len;
				sdu_decode(ts, buf, len);
			} else if (vcom->mode == VCOM_MODE_RAW_TRACE) {
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
		DCC_LOG1(LOG_MSG, "[%d] usb_cdc_ctl_wait() sleep!", 
				 thinkos_thread_self());
		usb_cdc_ctl_wait(cdc, 0);
		DCC_LOG1(LOG_MSG, "[%d] wakeup ---------- ", thinkos_thread_self());

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

			if (state.cfg.baudrate == 0) {
				state.cfg.baudrate = vcom->cfg.baudrate;
				DCC_LOG1(LOG_WARNING, "invalid baudrate, keep current: %d", 
						 vcom->cfg.baudrate);
				continue;
			}
			if (state.cfg.databits == 0) {
				state.cfg.databits = vcom->cfg.databits;
				DCC_LOG1(LOG_WARNING, "invalid databits, keep current: %d", 
						 vcom->cfg.databits);
				continue;
			}

			/* stop serial receiver */
			serial_rx_disable(serial);

			if (state.cfg.baudrate == 921600) {
				if (vcom->mode != VCOM_MODE_SERVICE) {
					vcom->mode = VCOM_MODE_SERVICE; 
					DCC_LOG(LOG_TRACE, "Service mode magic config ...");
				}
			} else {
				DCC_LOG1(LOG_TRACE, "baudrate=%d", state.cfg.baudrate);
				DCC_LOG1(LOG_TRACE, "databits=%d", state.cfg.databits);
				DCC_LOG1(LOG_TRACE, "parity=%d", state.cfg.parity);
				DCC_LOG1(LOG_TRACE, "stopbits=%d", state.cfg.stopbits);
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
				if (!vcom->cfg_lock) {
					/* only update the configuration if we are not locked */
					vcom->cfg = state.cfg;
					serial_config_set(serial, &vcom->cfg);
				}
			
				/* start serial receiver */
				serial_rx_enable(serial);
			}
		} else {
			DCC_LOG(LOG_INFO, "keeping serial config.");
			DCC_LOG1(LOG_MSG, "baudrate=%d", vcom->cfg.baudrate);
			DCC_LOG1(LOG_MSG, "databits=%d", vcom->cfg.databits);
			DCC_LOG1(LOG_MSG, "parity=%d", vcom->cfg.parity);
			DCC_LOG1(LOG_MSG, "stopbits=%d", vcom->cfg.stopbits);
		}
	}
}

uint32_t __attribute__((aligned(8))) led_stack[32];
uint32_t __attribute__((aligned(8))) serial_ctrl_stack[64];
uint32_t __attribute__((aligned(8))) serial_recv_stack[192];

int __attribute__((noreturn)) main(int argc, char ** argv)
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
	vcom.cfg_lock = false;

	usb_trace_init(cdc);

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

	usb_recv_task(&vcom);
}

