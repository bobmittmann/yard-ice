/* 
 * File:	 sndbuf.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
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

#include <arch/cortex-m3.h>
#include <stdlib.h>
#include <stdint.h>

#include "jitbuf.h"

void jitbuf_init(struct jitbuf *jb, unsigned int delay)
{
	jb->head_ts = 0;
	jb->head = 0;
	jb->tail = 0;
	jb->delay = delay;
}

int jitbuf_put(struct jitbuf * jb, sndbuf_t * buf[], 
				unsigned int cnt, uint32_t ts)
{
	unsigned int head;
	unsigned int tail;
	unsigned int free;
	int32_t dt;
	int n;
	int i;

	// Get the head frame pointer
	head = jb->head;
	tail = jb->tail;

	if (head == tail) {
		// Buffer empty. Fill with silence up to "delay" time
		dt = jb->delay;
//		trace_printf("%s: Warn: buffer empty, resync!", __func__);
	} else {
		// Compute the difference between the input time
		// and the time at the jitter buffer's head.
		dt = (int32_t)(ts - jb->head_ts);

		if (dt > 0) {
			unsigned int jit_tm;
			/* There is a dt gap between the received packet and the 
			   head of the buffer
			   How many samples (sound frames) are missing ???

			   Take this opportunity to resync the buffer
			   Compute the time of pending samples in the jitter buffer */
			jit_tm = (int32_t)(head - tail) * SNDBUF_LEN;
			if ((jit_tm + dt) > (int32_t)jb->delay) {
				//trace_printf("%s: (JimTm=%d + dt=%d) > Delay=%d!", __func__,
				//    jit_tm, dt, jb->Delay);
				dt = jb->delay - jit_tm;
				if (dt < 0) {
					// Ignore overlapping
					dt = 0;
				}
			}
		} else if (dt < 0) {
//			trace_printf("%s: Error: data overlap!!!!", __func__);
			// Ignore overlapping and append at the end
			dt = 0;
		}
	}

	if (0L != dt) {
		//trace_printf("%s: dt=%d head=%d tail=%d", __func__, dt, head, tail);
		n = dt / SNDBUF_LEN;

		// Get how many free frames we have
		free = JITBUF_FIFO_LEN - (unsigned int)(head - tail);
		if (n > free) {
			n = free;
//			trace_printf("%s: ERROR: buffer overflow!!!!", __func__);
		} else {
			// Fill in the gap
			while (n--)
				jb->fifo[head++ & (JITBUF_FIFO_LEN - 1)] = NULL;
		}
		// The buffer is in sync..., clear the compensation time
		dt = 0;
	}

	// Free entries in the buffer
	free = JITBUF_FIFO_LEN - (unsigned int)(head - tail);

	if (cnt > free) {
		n = free;
//		trace_printf("%s: Error: buffer overflow!!!!", __func__);
	} else {
		n = cnt;
	}

	//trace_printf("%s: n=%d head=%d free=%d", __func__, n, head, free);

	for (i = 0; i < n; ++i) {
		jb->fifo[head++ & (JITBUF_FIFO_LEN - 1)] = *buf++;
		dt += SNDBUF_LEN;
	}
	jb->head = head;
	jb->head_ts = ts + dt;

	for (; i < cnt; ++i)
		sndbuf_free(*buf++);

	return n;
}

sndbuf_t * jitbuf_get(struct jitbuf * jb)
{
	int tail = jb->tail;
	sndbuf_t * buf;

	if ((int)(jb->head - tail) > 0) {
		buf = jb->fifo[tail++ & (JITBUF_FIFO_LEN - 1)];
		jb->tail = tail;
	}
	else
		buf = NULL;

	return buf;
}

