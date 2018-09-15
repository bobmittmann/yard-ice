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
 * @file armice-dcc.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/at91.h>

#include "jtag_arm.h"
#include "armice.h"
#include "arm_pn.h"
#include "dbglog.h"
#include "uthreads.h"

int armice_dcc_open(jtag_arm_dcc_t * arm_dcc)
{
	DCC_LOG(LOG_TRACE, ".");

	/* Reset the queue */
	arm_dcc->.head = arm_dcc->.tail;

	return 0;
}

int armice_dcc_close(jtag_arm_dcc_t * arm_dcc)
{
	DCC_LOG(LOG_TRACE, ".");

	arm_dcc->.head = 0;
	arm_dcc->.tail = 0;

	return 0;
}

/*
 * The DCC read function waits on the timer (TC2) interrupt that
 * is either a 500ms timeout or a block of data ready in the
 * dcc buffer (dcc_buf).
 * The poll routine in the FIQ polls the target, read data from
 * the DCC and store in the dcc buffer.
 */
int armice_dcc_read(jtag_arm_dcc_t * arm_dcc, void * buf, int len)
{
	int head;
	int tail;
	int max;
	int n;
	int m;

#if 0	
	if (max_jtag.error) {
		DCC_LOG1(LOG_WARNING, "jtagtap->arm.error %s", 
				 jtagtap->arm_errtab[max_jtag.error]);
		return -1;
	}
#endif
	DCC_LOG1(LOG_MSG, "len=%d", len);

	head = arm_dcc->.head;

	if ((tail = arm_dcc->.tail) == head) {
		DCC_LOG(LOG_MSG, "block");

		/* wait the end of operation */
		uthread_sem_timedwait(arm_dcc->.sem, 500);

		if ((tail = arm_dcc->.tail) == head) {
			DCC_LOG(LOG_MSG, "timeout.");
			return 0;
		}
	}
	
	DCC_LOG2(LOG_MSG, "head=%d tail=%d", head, tail);

	/* round wrap the pointers */
	tail &= JTAG_ARM_DCC_BUF_LEN - 1;
	head &= JTAG_ARM_DCC_BUF_LEN - 1;
	/* we are copying 4 bytes at time, so the we can copy up to 
	   floor(length / 4) words */

	max = len / 4;

	if (tail > head) {
		n = tail - head;
		m = 0;
	} else {
		n = (JTAG_ARM_DCC_BUF_LEN - head);
		m = tail;
	}
	if (n > max) {
		n = max;
		m = 0;
	} else {
		if ((n + m) > max) {
			m = (max - n);
		}
	}

	DCC_LOG4(LOG_MSG, "head=%d tail=%d n=%d m=%d", head, tail, n, m);

	memcpy(buf, &arm_dcc->.buf[head], n * 4);
	memcpy(((uint8_t *)buf) + n * 4, arm_dcc->.buf, m * 4);

	arm_dcc->.head += n + m;

	return (n + m) * 4;
}

int armice_dcc_flush(jtag_arm_dcc_t * arm_dcc)
{
	DCC_LOG(LOG_TRACE, ".");

	/* Reset the queue */
	arm_dcc->.head = arm_dcc->.tail;

	return ICE_OK;
}

int armice_dcc_init(jtag_arm_dcc_t * arm_dcc)
{
	arm_dcc->.head = 0;
	arm_dcc->.tail = 0;
	arm_dcc->.sem = uthread_sem_alloc(0);

	DCC_LOG(LOG_TRACE, "[INIT]");

	return ICE_OK;
}

int armice_dcc_done(jtag_arm_dcc_t * arm_dcc)
{
	uthread_sem_free(arm_dcc->.sem);
	arm_dcc->.sem = -1;

	DCC_LOG(LOG_TRACE, "[DONE]");

	return ICE_OK;
}


