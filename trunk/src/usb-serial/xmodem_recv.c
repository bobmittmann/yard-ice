/*
 * File:	xmodem_rcv.c
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:	
 * Copyright(c) 2008-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifdef CONFIG_H
#include "config.h" 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <crc.h>
#include <limits.h>

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define BS   0x08

#include <sys/dcclog.h>

uint32_t pfdt(void);

int xmodem_recv(int (*__getc)(int), int (*__putc)(int), int max)
{
	uint8_t buf[1024];
	unsigned char sync;
	int err = 0;
	int retry;
	int pktno;
	int nseq;
	int seq;
	int crc;
	int tmp;
	int cnt;
	int n;
	int c;

	retry = 10;
	sync = 'C';
	pktno = 1;

	/* if the max size was not specified (0), do not check for limit... */
	if (max == 0)
		max = INT_MAX;

	for (;;) {
		DCC_LOG(LOG_INFO,"SYN");
		__putc(sync);

again:
		if ((c = __getc(2000)) < 0) {
			/* timeout */
			goto timeout;
		}

		switch (c) {
		case STX:
			DCC_LOG(LOG_INFO,"STX");
			cnt = 1024;
			break;

		case SOH:
			DCC_LOG(LOG_INFO,"SOH");
			cnt = 128;
			break;

		case EOT:
			DCC_LOG(LOG_INFO,"EOT");
			/* end of transmission */
			__putc(ACK);
			return 0;

		default:
			goto again;
		}
		
		/* sequence */
		if ((seq = __getc(500)) < 0) {
			/* timeout */
			goto timeout;
		}

		/* inverse sequence */
		if ((nseq = __getc(500)) < 0) {
			/* timeout */
			goto timeout;
		}

		DCC_LOG1(LOG_INFO, "%d SEQ", pfdt());

		if (seq != ((~nseq) & 0xff)) {
			DCC_LOG2(LOG_WARNING, "SEQ: %d != NSEQ: %d", seq, nseq);
			goto error;
		}

		crc = 0;

		for (n = 0; n < cnt; n++) {
			pfdt();
			if ((c = __getc(500)) < 0) {
				DCC_LOG1(LOG_WARNING, "TMO: n=%d!", n);
				/* timeout */
				goto timeout;
			}

			crc = __crc16ccitt(crc, c);

			buf[n] = c;
		}

		DCC_LOG(LOG_INFO, ">");

		if ((tmp = __getc(500)) < 0) {
			/* timeout */
			goto timeout;
		}

		if ((c = __getc(500)) < 0) {
			/* timeout */
			goto timeout;
		}

		tmp = c + (tmp << 8); 

		DCC_LOG(LOG_INFO, "&");

		if (tmp != crc) {
			goto error;
		}

		if (seq == ((pktno - 1) & 0xff)) {
			/* retransmission */
			DCC_LOG(LOG_TRACE, "RET");
			sync = ACK;
			continue;
		}

		if (seq != pktno) {
			DCC_LOG2(LOG_TRACE, "SEQ: %d != %d", seq, pktno);
			goto error;
		}

		max -= cnt;
		if (max < 0) {
			DCC_LOG(LOG_WARNING, "too large!");
			/* too large */
			err = -1;
			goto abort;
		}

#if 0
		if (uboot_mem_write(addr, buf, cnt) < 0) {
			DCC_LOG(LOG_WARNING, "uboot_mem_write()!");
			/* write fail */
			err = -2;
			goto abort;
		}
#else
		(void)buf;
		DCC_LOG(LOG_INFO, "WRITE");
#endif

		pktno = (pktno + 1) & 0xff;
		retry = 10;
		sync = ACK;
		DCC_LOG(LOG_INFO, "ACK");
		continue;

error:
		/* flush */
		while (__getc(500) > 0);
		sync = NAK;
		DCC_LOG(LOG_TRACE, "NACK");
timeout:
		if ((--retry) == 0) {
			/* too many errors */
			err = -3;
			goto abort;
		}
	}

abort:
	DCC_LOG(LOG_TRACE, "ABT!");

	/* flush */
	while (__getc(500) > 0);

	__putc(CAN);
	__putc(CAN);
	__putc(CAN);

	return err;
}
