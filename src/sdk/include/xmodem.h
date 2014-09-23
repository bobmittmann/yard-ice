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


struct comm_ops {
	int  (* send)(void *, const void *, unsigned int);
	int  (* recv)(void *, void *, unsigned int, unsigned int);
};

struct comm_dev {
	void * arg;
	struct comm_ops op;
};

enum {
	XMODEM_RCV_CKS = 0,
	XMODEM_RCV_CRC = 1,
};

struct xmodem_rcv {
	const struct comm_dev * comm;

	signed char mode;
	unsigned char sync;
	unsigned char pktno;
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
	XMODEM_SND_STD = 0,
	XMODEM_SND_1K = 1
};

struct xmodem_snd {
	const struct comm_dev * comm;

	unsigned char seq;
	unsigned char state;
	unsigned char mode;
	unsigned short data_len;
	unsigned short data_max;

	struct { 
		unsigned char hdr[3];
		unsigned char data[1024];
	} pkt;
};

#ifdef __cplusplus
extern "C" {
#endif

int xmodem_rcv_init(struct xmodem_rcv * xp, const struct comm_dev * comm, 
					int mode);

int xmodem_rcv_loop(struct xmodem_rcv * rx, void * data, int len);

int xmodem_rcv_cancel(struct xmodem_rcv * rx);


int xmodem_rcv(struct xmodem_rcv * xp, int * cp);



int xmodem_snd_init(struct xmodem_snd * sx, const struct comm_dev * comm, 
					unsigned int mode);

int xmodem_snd_loop(struct xmodem_snd * sx, const void * data, int len);

int xmodem_snd_cancel(struct xmodem_snd * sx);

int xmodem_snd_eot(struct xmodem_snd * sx);


#ifdef __cplusplus
}
#endif

#endif /* __XMODEM_H__ */

