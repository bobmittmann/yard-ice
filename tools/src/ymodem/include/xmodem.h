/* 
 * Copyright(C) 2012-2014 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
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
 * @file xmodem.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __XMODEM_H__
#define __XMODEM_H__


#include "serial.h"

enum {
	FCS_CKS = 0,
	FCS_CRC = 1,
};

#define XMODEM_FNAME_MAX 117

struct xmodem_recv {
	struct serial_dev * dev;
	unsigned int pktno;
	unsigned int fsize;
	unsigned int count;
	char fname[XMODEM_FNAME_MAX + 1];

	signed char fcs_mode;
	signed char xfr_mode;
	unsigned char sync;
	unsigned char retry;

	unsigned short data_len;
	unsigned short data_pos;
	struct { 
		unsigned char hdr[3];
		unsigned char data[1024];
		unsigned char fcs[2];
	} pkt;
};

enum {
	MODE_XMODEM = 0,
	MODE_XMODEM_1K = 1,
	MODE_YMODEM = 2
};

struct xmodem_send {
	struct serial_dev * dev;

	unsigned char seq;
	unsigned char state;
	unsigned char mode;
	unsigned short data_len;
	unsigned short data_max;

	struct { 
		unsigned char hdr[3];
		unsigned char data[1024];
		unsigned char fcs[2];
	} pkt;
};

#ifdef __cplusplus
extern "C" {
#endif

int xmodem_recv_init(struct xmodem_recv * rx, const struct serial_dev * dev, 
					int fcs_mode, int xfr_mode);

int xmodem_recv_loop(struct xmodem_recv * rx, void * data, int len);

int xmodem_recv_cancel(struct xmodem_recv * rx);

struct xmodem_recv * xmodem_recv_alloc(void);

void xmodem_recv_free(struct xmodem_recv * rx);


int xmodem_send_open(struct xmodem_send * sx, 
					 const struct serial_dev * dev, unsigned int mode);

int xmodem_send_start(struct xmodem_send * sx, const char * fname, 
					  unsigned int fsize);

int xmodem_send_loop(struct xmodem_send * sx, const void * data, int len);

int xmodem_send_eot(struct xmodem_send * sx);

int xmodem_send_close(struct xmodem_send * sx);

int xmodem_send_cancel(struct xmodem_send * sx);

struct xmodem_send * xmodem_send_alloc(void);

void xmodem_send_free(struct xmodem_send * sx);


#ifdef __cplusplus
}
#endif

#endif /* __XMODEM_H__ */

