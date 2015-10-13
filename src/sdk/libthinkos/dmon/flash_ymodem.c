/* 
 * File:	 usb-cdc.c
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

#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <thinkos.h>
#include <sys/dcclog.h>
#include <sys/param.h>
#include <sys/stm32f.h>
#include <crc.h>

#if (THINKOS_ENABLE_MONITOR)

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18

#define XMODEM_RCV_TMOUT_MS 2000

struct ymodem_rcv {
	unsigned int pktno;
	unsigned int fsize;
	unsigned int count;

	unsigned char crc_mode;
	unsigned char xmodem;
	unsigned char sync;
	unsigned char retry;

	struct { 
		unsigned char hdr[3];
		unsigned char data[1024];
		unsigned char fcs[2];
	} pkt;
};

static int ymodem_rcv_init(struct ymodem_rcv * rx, bool crc_mode, bool xmodem)
{
	rx->pktno = 0;
	rx->crc_mode = crc_mode;
	rx->xmodem = xmodem;
	rx->sync = rx->crc_mode ? 'C' : NAK;
	rx->xmodem = 0;
	rx->retry = 30;
	rx->fsize = 1024 * 1024;
	rx->count = 0;

	return 0;
}

#if 0
static int ymodem_rcv_cancel(struct dmon_comm * comm, struct ymodem_rcv * rx)
{
	unsigned char * pkt = rx->pkt.hdr;

	pkt[0] = CAN;
	pkt[1] = CAN;
	pkt[2] = CAN;

	dmon_comm_send(comm, pkt, 3);

	return 0;
}
#endif

static int ymodem_rcv_pkt(struct dmon_comm * comm, struct ymodem_rcv * rx)
{
	unsigned char * pkt = rx->pkt.hdr;
	unsigned char * cp;
	int ret = 0;
	int cnt = 0;
	int nseq;
	int seq;
	int rem;

	for (;;) {

		dmon_alarm(XMODEM_RCV_TMOUT_MS);
		DCC_LOG1(LOG_INFO, "SYN=%02x", rx->sync);
		dmon_comm_send(comm, &rx->sync, 1);

		for (;;) {
			int c;

			ret = dmon_comm_recv(comm, pkt, 1);
			if (ret < 0)
				goto timeout;

			c = pkt[0];

			if (c == STX) {
				cnt = 1024;
				break;
			}

			if (c == SOH) {
				cnt = 128;
				break;
			}

			if (c == CAN) {
				return -1;
			}

			if (c == EOT) {
				DCC_LOG(LOG_INFO, "EOT!!");
				/* end of transmission */
				rx->sync = rx->crc_mode ? 'C' : NAK;
				rx->pktno = 0;
				pkt[0] = ACK;
				dmon_comm_send(comm, pkt, 1);
				return 0;
			}
		}

		rem = cnt + ((rx->crc_mode) ? 4 : 3);
		cp = pkt + 1;

		/* receive the packet */
		while (rem) {
			dmon_alarm(500);
			ret = dmon_comm_recv(comm, cp, rem);
			if (ret < 0)
				goto timeout;

			rem -= ret;
			cp += ret;
		}

		/* sequence */
		seq = pkt[1];
		/* inverse sequence */
		nseq = pkt[2];

		if (seq != ((~nseq) & 0xff)) {
			goto error;
		}

		cp = &pkt[3];

		if (rx->crc_mode) {
			unsigned short crc = 0;
			unsigned short cmp;
			int i;

			for (i = 0; i < cnt; ++i)
				crc = CRC16CCITT(crc, cp[i]);

			cmp = (unsigned short)cp[i] << 8 | cp[i + 1];

			if (cmp != crc) {
				DCC_LOG(LOG_WARNING, "CRC error");
				goto error;
			}

		} else {
			unsigned char cks = 0;
			int i;

			for (i = 0; i < cnt; ++i)
				cks += cp[i];

			if (cp[i] != cks) {
				DCC_LOG(LOG_WARNING, "wrong sum");
				goto error;
			}
		}

		if (seq == ((rx->pktno - 1) & 0xff)) {
			/* retransmission */
			DCC_LOG(LOG_INFO, "rxmit ...");
			continue;
		}

		if (seq != (rx->pktno & 0xff)) {
			if ((rx->pktno == 0) && (seq == 1)) {
				rx->pktno++;
				/* Fallback to XMODEM */
				rx->xmodem = 1;
				DCC_LOG(LOG_INFO, "XMODEM...");
			} else {
				DCC_LOG(LOG_WARNING, "wrong sequence");
				goto error;
			}
		}

		/* YModem first packet ... */
		if (rx->pktno == 0) {
			DCC_LOG(LOG_INFO, "ACK");
			pkt[0] = ACK;
			dmon_comm_send(comm, pkt, 1);
		} else {
			rx->retry = 10;
			rx->sync = ACK;
			if ((rx->count + cnt) > rx->fsize)
				cnt = rx->fsize - rx->count;
			rx->count += cnt;
		}

		DCC_LOG2(LOG_INFO, "pktno=%d count=%d", rx->pktno, rx->count);

		rx->pktno++;

		dmon_alarm_stop();

		return cnt;

error:
		/* flush */
		while (dmon_comm_recv(comm, pkt, 1024) > 0);
		ret = -1;
		break;

timeout:
		DCC_LOG(LOG_WARNING, "timeout!!");
		if ((--rx->retry) == 0) {
			/* too many errors */
			ret = -1;
			break;
		}
	}


	pkt[0] = CAN;
	pkt[1] = CAN;

	dmon_comm_send(comm, pkt, 2);

	dmon_alarm_stop();
	return ret;
}

unsigned long dec2int(const char * __s)
{
	unsigned long val = 0;
	char * cp = (char *)__s;
	char c;

	while ((c = *cp) != '\0') {
		if ((c < '0') || (c > '9'))
			break;
		val = val * 10;
		val += c - '0';
		cp++;
	}

	return val;
}


extern uint32_t _stack;

/* Receive a file and write it into the flash using the YMODEM preotocol */
int dmon_ymodem_flash(struct dmon_comm * comm,
					  uint32_t addr, unsigned int size)
{
	/* FIXME: generalize the application load by removing the low
	   level flash calls dependency */
#ifdef STM32_FLASH_MEM
	/* The YMODEM state machine is allocated at the top of 
	   the stack, make sure there is no app running before 
	   calling the dmon_ymodem_flash()! */
	struct ymodem_rcv * ry = ((struct ymodem_rcv *)&_stack) - 1;
	uint32_t base = (uint32_t)STM32_FLASH_MEM;
	uint32_t offs = addr - base;
	int ret;

	DCC_LOG2(LOG_INFO, "sp=%p ry=%p", cm3_sp_get(), ry);
	DCC_LOG2(LOG_INFO, "offs=0x%08x size=%d", offs, size);
	ymodem_rcv_init(ry, true, false);
	ry->fsize = size;

	DCC_LOG(LOG_INFO, "Starting...");
	while ((ret = ymodem_rcv_pkt(comm, ry)) >= 0) {
		if ((ret == 0) && (ry->xmodem) )
			break;
		int len = ret;
		if (ry->pktno == 1) {
			char * cp;
			int fsize;

			cp = (char *)ry->pkt.data;
			DCC_LOGSTR(LOG_INFO, "fname='%s'", cp);
			while (*cp != '\0')
				cp++;
			/* skip null */
			cp++;
			fsize = dec2int(cp);
			if (fsize == 0) {
				ret = 0;
				break;
			}
			DCC_LOG1(LOG_INFO, "fsize='%d'", fsize);
			ry->fsize = fsize;
			DCC_LOG(LOG_INFO, "YMODEM first packet...");
		} else {
			if (ry->pktno == 2) {
				stm32_flash_erase(offs, ry->fsize);
			}	
			stm32_flash_write(offs, ry->pkt.data, len);
			offs += len;
		}
	}

	return ret;
#else
	return -1;
#endif
}

#endif /* THINKOS_ENABLE_MONITOR */

