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

#ifndef XMODEM_TMOUT_TICK_MS 
#define XMODEM_TMOUT_TICK_MS 250
#endif

enum {
	XMODEM_MODE_CKS = 0,
	XMODEM_MODE_CRC = 1
};

#define XMODEM_CKS_BUF_SIZE 128
#define XMODEM_CRC_BUF_SIZE 1024

enum {
	XMODEM_RCV_IDLE = 0,
	XMODEM_RCV_DATA,
	XMODEM_RCV_EOT,
	XMODEM_RCV_SYNC,
	XMODEM_RCV_STX,
	XMODEM_RCV_SEQ,
	XMODEM_RCV_NSEQ,
	XMODEM_RCV_CKS,
	XMODEM_RCV_CRC1,
	XMODEM_RCV_CRC2,
	XMODEM_RCV_ABORT
};

struct xmodem_rcv {
	char * buf;

	unsigned char mode;
	unsigned char state;
	unsigned char pktno;
	unsigned char cks;
	
	unsigned char seq;
	unsigned char nseq;
	signed char retry;
	signed char again;

	unsigned short crc;
	unsigned short count;

	signed short tmout;
	unsigned short pos;	
};

#define XMODEM_INIT_ERR -5
#define XMODEM_RETRANS_ERR -4
#define XMODEM_RETRY_ERR -3
#define XMODEM_SYNC_ERR -2
#define XMODEM_CANCEL -1
#define XMODEM_OK 0


struct xmodem_snd {
	char * buf;

	char mode;
	char state;
	unsigned char pktno;
	unsigned char cks;
	
	unsigned char seq;
	unsigned char nseq;
	signed char retry;
	signed char again;

	unsigned short crc;
	unsigned short count;

	unsigned short tmout;
	unsigned short pos;	
};

#define XMODEM_SND_IDLE 0
#define XMODEM_SND_DATA 1
#define XMODEM_SND_EOT 2 
#define XMODEM_SND_START 3
#define XMODEM_SND_STX 4
#define XMODEM_SND_SEQ 5
#define XMODEM_SND_NSEQ 6
#define XMODEM_SND_CKS 7
#define XMODEM_SND_CRC1 8
#define XMODEM_SND_CRC2 9
#define XMODEM_SND_ABORT 10

#ifdef __cplusplus
extern "C" {
#endif

int xmodem_rcv_init(struct xmodem_rcv * xp, char * buf, int mode);

int xmodem_rcv(struct xmodem_rcv * xp, int * cp);

int xmodem_rcv_cancel(struct xmodem_rcv * xp);


int xmodem_snd_init(struct xmodem_snd * xp, char * buf, int mode);

int xmodem_snd(struct xmodem_snd * xp, int * cp);

int xmodem_snd_cancel(struct xmodem_snd * xp);

#ifdef __cplusplus
}
#endif

#endif /* __XMODEM_H__ */

