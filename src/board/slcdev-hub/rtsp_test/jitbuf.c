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
#include <sys/param.h>
#include <stdlib.h>
#include <stdint.h>

#include "jitbuf.h"
#include "trace.h"

int jitbuf_init(struct jitbuf *jb, uint32_t tsclk_rate, 
				 uint32_t sample_rate, uint32_t delay_ms,
				 const struct sndbuf * silence)
{
	uint32_t delay;
	uint32_t tbuf;
	int cnt;

	/* ceiling delay time in TSCLK periods */
	delay = ((delay_ms * tsclk_rate) + 999) / 1000;

	/* ceiling buffer period in TSCLK periods */
	tbuf = (SNDBUF_LEN * tsclk_rate) / sample_rate;

	cnt = delay / tbuf;
	(void)cnt;

	INF("delay=%d[ms],%d[TSCLK],%d[bufs]", delay_ms, delay, cnt);
	DBG("tbuf=%d[TSCLK]", tbuf);
		
	jb->head_ts = 0;
	jb->head = 0;
	jb->tail = 0;
	jb->delay = delay;
	jb->tbuf = tbuf;
	jb->silence = (struct sndbuf *)silence;

	return 0;
}

int __jitbuf_enqueue(struct jitbuf * jb, struct sndbuf * buf, uint32_t ts)
{
	uint32_t head;
	int32_t lvl;
	int32_t dt;

	/* get the queue head pointer */
	head = jb->head;
	/* level of the current level of the queue */
	lvl = head - jb->tail;

	if (lvl > JITBUF_FIFO_LEN) {
		WARN("ts=%d, fifo overflow!", ts);
		/* Queue is full, discard */
		return -1;
	}

	if (lvl == JITBUF_FIFO_LEN) {
		WARN("ts=%d, fifo full!", ts);
		/* Queue is full, discard */
		return -1;
	}

	if (sndbuf_use(buf) == NULL) {
		WARN("ts=%d, sndbuf_use() failed!", ts);
		/* Queue is full, discard */
		return -1;
	}


	if (lvl == 0) {
		/* Queue empty. Fill with silence up to "delay" time. */
		dt = jb->delay;
		WARN("ts=%d, buffer empty, resync!", ts);
	} else { 
		/* Compute the difference between the input time
		   and the time at the jitter buffer's head. */
		dt = (int32_t)(ts - jb->head_ts);

		if (dt > 0) {
			uint32_t jit_tm;
			/* There is a dt gap between the received frame and the 
			   head of the queue.
			   How many samples (sound frames) are missing ??? */

			/* Take this opportunity to resync the buffer */

			/*  Compute the time span of pending samples in the jitter buffer */
			jit_tm = lvl * jb->tbuf;
			if ((jit_tm + dt) > jb->delay) {
				DBG("(jit_tm=%d + dt=%d) > Delay=%d!", jit_tm, dt, jb->delay);
				dt = jb->delay - jit_tm;
			}
		} 
			
		if (dt < 0) {
			WARN("data overlap, ts=%d dt=%d!", ts, dt);
			/*  Ignore overlapping and append at the end */
			dt = 0;
		}
	} 
	
	if (dt != 0) {
		int cnt;

		/* Get how many frames we have to insert in the gap */
		cnt = dt / jb->tbuf;
		/* The gap canont be higher than the available space
		   in the fifo, less 1 position (reserved for insertion) */
		cnt = MIN(cnt, JITBUF_FIFO_LEN - (lvl + 1));

		DBG("dt=%d head=%d cnt=%d", dt, head, cnt);

		/* Fill the gap with silence */
		while (cnt--)
			jb->fifo[head++ & (JITBUF_FIFO_LEN - 1)] = jb->silence;

		/* The buffer is in sync... */
	}

	jb->fifo[head++ & (JITBUF_FIFO_LEN - 1)] = buf;
	jb->head = head;
	jb->head_ts = ts + jb->tbuf;


	return 1;
}

int jitbuf_enqueue(struct jitbuf * jb, struct sndbuf * buf, uint32_t ts)
{
	uint32_t head;
	uint32_t lvl;
	int32_t dt;

	/* get the queue head pointer */
	head = jb->head;
	/* level of the current level of the queue */
	lvl = head - jb->tail;

	if (lvl > JITBUF_FIFO_LEN) {
		ERR("ERROR ts=%d, fifo overflow!", ts);
		/* Queue is full, discard */
		return -1;
	}

	if (lvl == JITBUF_FIFO_LEN) {
		WARN("ts=%d, fifo full!", ts);
		/* Queue is full, discard */
		return -1;
	}

	if (sndbuf_use(buf) == NULL) {
		WARN("ts=%d, sndbuf_use() failed!", ts);
		return -1;
	}

	if (lvl == 0) {
		int cnt;
		/* Queue empty. Fill with silence up to "delay" time. */
		dt = jb->delay;
		DBG("ts=%d, buffer empty, resync!", ts);

		/* Get how many frames we have to insert in the gap */
		cnt = dt / jb->tbuf;
		/* The gap canont be higher than the available space
		   in the fifo, less 1 position (reserved for insertion) */
		cnt = MIN(cnt, JITBUF_FIFO_LEN - (lvl + 1));

		DBG("dt=%d head=%d cnt=%d", dt, head, cnt);

		/* Fill the gap with silence */
		while (cnt--)
			jb->fifo[head++ & (JITBUF_FIFO_LEN - 1)] = jb->silence;

	} 

	jb->fifo[head++ & (JITBUF_FIFO_LEN - 1)] = buf;
	jb->head = head;
	jb->head_ts = ts + jb->tbuf;

	return 1;
}

sndbuf_t * jitbuf_dequeue(struct jitbuf * jb)
{
	int tail = jb->tail;
	sndbuf_t * buf;

	if ((int)(jb->head - tail) > 0) {
		buf = jb->fifo[tail++ & (JITBUF_FIFO_LEN - 1)];
		jb->tail = tail;
	} else
		buf = NULL;

	return buf;
}

