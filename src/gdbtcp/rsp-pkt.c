/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
 * @file gdb-rsp.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#include "gdbtcp-i.h"

#include <sys/dcclog.h>


/*
 * Common response packets
 */

int rsp_send_ack(struct tcp_pcb * tp)
{
	return tcp_send(tp, "+", 1, 0);
}

int rsp_send_nack(struct tcp_pcb * tp)
{
	return tcp_send(tp, "-", 1, TCP_SEND_NOWAIT);
}

int rsp_send_ok(struct tcp_pcb * tp)
{
	return tcp_send(tp, "$OK#9a", 6, TCP_SEND_NOWAIT);
}

int rsp_send_empty(struct tcp_pcb * tp)
{
	return tcp_send(tp, "$#00", 4, TCP_SEND_NOWAIT);
}

int rsp_send_error(struct tcp_pcb * tp, int err)
{
	char pkt[16];
	int sum;

	pkt[0] = '$';
	pkt[1] = sum = 'E';
	sum += pkt[2] = __hextab[((err >> 4) & 0xf)];
	sum += pkt[3] = __hextab[(err & 0xf)];
	pkt[4] = '#';
	pkt[5] = __hextab[((sum >> 4) & 0xf)];
	pkt[6] = __hextab[sum & 0xf];

	return tcp_send(tp, pkt, 7, TCP_SEND_NOWAIT);
}

int rsp_send_msg(struct tcp_pcb * tp, const char * s)
{
	char pkt[256];
	int sum;
	int n;
	char c;

	pkt[0] = '$';
	pkt[1] = sum = 'O';
	n = 2;
	while (*s) {
		c = __hextab[((*s >> 4) & 0xf)];
		pkt[n++] = c;
		sum += c;
		c = __hextab[*s & 0xf];
		pkt[n++] = c;
		sum += c;
		s++;
	}
	pkt[n++] = '#';
	pkt[n++] = __hextab[((sum >> 4) & 0xf)];
	pkt[n++] = __hextab[sum & 0xf];

	return tcp_send(tp, pkt, n, TCP_SEND_NOWAIT);
}

int rsp_send_pkt(struct tcp_pcb * tp, char * pkt, unsigned int len)
{
	unsigned int sum = 0;
	unsigned int n;
	char c;

	for (n = 1; n < len; ++n) {
		c = pkt[n];
		sum += c;
	}
	pkt[n++] = '#';
	pkt[n++] = __hextab[((sum >> 4) & 0xf)];
	pkt[n++] = __hextab[sum & 0xf];

	pkt[n] = '\0';

#if GDB_DEBUG_PACKET
	DCC_LOGSTR(LOG_INFO, "--> '%s'", pkt);
#endif

	return tcp_send(tp, pkt, n, TCP_SEND_NOWAIT);
}

int rsp_send_stop_thread(struct tcp_pcb * tp, int signal, int thread)
{
	char pkt[32];
	char * cp;
	int n;

	cp = pkt;
	*cp++ = '$';
	*cp++ = 'T';
	cp += char2hex(cp, signal);
	cp += str2str(cp, "thread:");
	cp += uint2hex(cp, thread);
	*cp++ = ';';

	n = cp - pkt;
	return rsp_send_pkt(tp, pkt, n);
}

int rsp_send_stop_core(struct tcp_pcb * tp, int signal, int thread)
{
	char pkt[32];
	char * cp;
	int n;

	cp = pkt;
	*cp++ = '$';
	*cp++ = 'T';
	cp += char2hex(cp, signal);
	cp += str2str(cp, "core:");
	cp += uint2hex(cp, thread);
	*cp++ = ';';

	n = cp - pkt;

	return rsp_send_pkt(tp, pkt, n);
}

int rsp_send_signal(struct tcp_pcb * tp, int signal)
{
	char pkt[32];
	int sum;;

	pkt[0] = '$';
	pkt[1] = sum = 'S';
	sum += pkt[2] = __hextab[((signal >> 4) & 0xf)];
	sum += pkt[3] = __hextab[(signal & 0xf)];
	pkt[4] = '#';
	pkt[5] = __hextab[((sum >> 4) & 0xf)];
	pkt[6] = __hextab[sum & 0xf];

	return tcp_send(tp, pkt, 7, TCP_SEND_NOWAIT);
}

#if GDB_DEBUG_PACKET
void log_pkt(char * pkt, int len)
{
	switch (len) {
	case 1:
		DCC_LOG1(LOG_TRACE, "[PKT] (1) '%c'", pkt[0]);
		break;
	case 2:
		DCC_LOG2(LOG_TRACE, "[PKT] (2) '%c%c'", pkt[0], pkt[1]);
		break;
	case 3:
		DCC_LOG3(LOG_TRACE, "[PKT] (3) '%c%c%c'", pkt[0], pkt[1], pkt[2]);
		break;
	case 4:
		DCC_LOG4(LOG_TRACE, "[PKT] (4) '%c%c%c%c'", pkt[0], pkt[1], 
				 pkt[2], pkt[3]);
		break;
	case 5:
		DCC_LOG5(LOG_TRACE, "[PKT] (5) '%c%c%c%c%c'", pkt[0], pkt[1], pkt[2], 
				 pkt[3], pkt[4]);
		break;
	case 6:
		DCC_LOG6(LOG_TRACE, "[PKT] (6) '%c%c%c%c%c%c'", pkt[0], pkt[1], 
				 pkt[2], pkt[3], pkt[4], pkt[5]);
		break;
	case 7:
		DCC_LOG7(LOG_TRACE, "[PKT] (7) '%c%c%c%c%c%c%c'", pkt[0], pkt[1], 
				 pkt[2], pkt[3], pkt[4], pkt[5], pkt[6]);
		break;
	case 8:
		DCC_LOG8(LOG_TRACE, "[PKT] (8) '%c%c%c%c%c%c%c%c'", pkt[0], pkt[1], 
				 pkt[2], pkt[3], pkt[4], pkt[5], pkt[6], pkt[7]);
		break;
	case 9:
		DCC_LOG9(LOG_TRACE, "[PKT] (9) '%c%c%c%c%c%c%c%c%c'", pkt[0], pkt[1], 
				 pkt[2], pkt[3], pkt[4], pkt[5], pkt[6], pkt[7], pkt[8]);
		break;
	case 10:
		DCC_LOG10(LOG_TRACE, "[PKT] (10) '%c%c%c%c%c%c%c%c%c%c'", len, pkt[0], 
				  pkt[1], pkt[2], pkt[3], pkt[4], pkt[5], pkt[6], 
				  pkt[7], pkt[8], pkt[9]);
	default:
		DCC_LOG10(LOG_TRACE, "[PKT] (%d) '%c%c%c%c%c%c%c%c%c'", len, pkt[0], 
				  pkt[1], pkt[2], pkt[3], pkt[4], pkt[5], pkt[6], 
				  pkt[7], pkt[8]);
	}
}
#endif


int rsp_write(struct tcp_pcb * tp, const void * buf, int len)
{
	char pkt[(len * 2) + 8];
	char * s = (char *)buf;
	int sum;
	int n;
	char c;
	int i;

	DCC_LOG1(LOG_MSG, "len=%d", len);

	pkt[0] = '$';
	pkt[1] = sum = 'O';
	n = 2;
	for (i = 0; i < len; i++) {
		c = __hextab[((s[i] >> 4) & 0xf)];
		pkt[n++] = c;
		sum += c;
		c = __hextab[s[i] & 0xf];
		pkt[n++] = c;
		sum += c;
	}
	pkt[n++] = '#';
	pkt[n++] = __hextab[((sum >> 4) & 0xf)];
	pkt[n++] = __hextab[sum & 0xf];

	return tcp_send(tp, pkt, n, 0);
}

int rsp_read(struct tcp_pcb * tp, const void * buf, int len)
{
	DCC_LOG1(LOG_TRACE, "len=%d", len);

	return 0;
}

static const struct fileop rsp_fileop = {
	.write = (void *)rsp_write,
	.read = (void *)rsp_read,
	.close = (void *)null_close,
	.flush = (void *)null_flush,
};

struct file * rsp_fopen(struct tcp_pcb * tp)
{
	return file_alloc(tp, &rsp_fileop);
}

int rsp_fclose(struct file * f)
{
	fclose(f);
	return file_free(f);
}

#if 0
static int rsp_thread_id(struct tcp_pcb * tp, int id)
{
	char pkt[32];
	int sum;
	int n;

	DCC_LOG1(LOG_TRACE, "id=%d", id);

	pkt[0] = '$';
	pkt[1] = 'Q';
	pkt[2] = 'C';
	n = 3;
	sum = 'Q' + 'C';
	if (id > 15) {
		sum += pkt[n++] = __hextab[((id >> 4) & 0xf)];
	}
	sum += pkt[n++] = __hextab[(id & 0xf)];
	pkt[n++] = '#';
	pkt[n++] = __hextab[((sum >> 4) & 0xf)];
	pkt[n++] = __hextab[sum & 0xf];

	return tcp_send(tp, pkt, n, TCP_SEND_NOWAIT);
}
#endif

