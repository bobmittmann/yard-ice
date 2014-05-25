/* 
 * File:	 xmodem.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
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

