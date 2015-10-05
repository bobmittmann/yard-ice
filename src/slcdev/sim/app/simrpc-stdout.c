/* 
 * File:	 usb-test.c
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

#include <thinkos.h>
#include <sys/file.h>
#include <sys/null.h>
#include <sys/param.h>
#include <string.h>
#include <sys/dcclog.h>

#include "board.h"
#include "simlnk.h"
#include "simrpc.h"
#include "simrpc_svc.h"

struct simrpc_console {
	char buf[SIMRPC_DATA_MAX];
	unsigned int len;
	unsigned int seq;
};

int simrpc_stdout_write(struct simrpc_console * out, 
						 const char * buf, int cnt)
{
	int rem = cnt;
	int free;
	int n;

	DCC_LOG2(LOG_MSG, "<%d> cnt=%d.", cnt, thinkos_thread_self());

	thinkos_mutex_lock(CONSOLE_MUTEX);

	while (rem) {
		free = SIMRPC_DATA_MAX - out->len;
		n = rem;
		if (n > free)
			n = free;
		memcpy(&out->buf[out->len], buf, n);
		rem -= n;
		out->len += n;
		if (out->len == SIMRPC_DATA_MAX) {
			uint32_t opc = simrpc_mkopc(SIMRPC_ADDR_LHUB, SIMRPC_ADDR_ANY, 
										out->seq++, SIMRPC_STDOUT_DATA);
			simrpc_send(opc, (void *)out->buf, out->len);
			out->len = 0;
		}
	}

	thinkos_mutex_unlock(CONSOLE_MUTEX);

	return cnt;
}

const struct fileop simrpc_stdout_fops = {
	.write = (void *)simrpc_stdout_write,
	.read = (void *)null_read,
	.flush = (void *)null_flush,
	.close = (void *)null_close
};

struct simrpc_console console_stdout;

const struct file simrpc_stdout_file = {
	.data = &console_stdout,
	.op = &simrpc_stdout_fops
};

void simrpc_stdout_flush_svc(uint32_t hdr, uint32_t * data, unsigned int cnt)
{
	struct simrpc_console * out;

	out = (struct simrpc_console *)simrpc_stdout_file.data; 
	thinkos_mutex_lock(CONSOLE_MUTEX);

	if (out->len > 0) {
		uint32_t opc = simrpc_mkopc(SIMRPC_ADDR_LHUB, SIMRPC_ADDR_ANY, 
									out->seq++, SIMRPC_STDOUT_DATA);
		simrpc_send(opc, (void *)out->buf, out->len);
		DCC_LOG1(LOG_TRACE, "len=%d.", out->len);
		out->len = 0;
	}

	thinkos_mutex_unlock(CONSOLE_MUTEX);
}

FILE * simrpc_stdout_fopen(void)
{
	return (FILE *)&simrpc_stdout_file;
}

void simrpc_test(void)
{
	uint32_t opc;
	int seq = 0;

	for (;;) {
		if ((seq % 10) == 0) {
			DCC_LOG1(LOG_TRACE, "seq=%d.", seq);
		}
		opc = simrpc_mkopc(SIMRPC_ADDR_LHUB, SIMRPC_ADDR_ANY, 
						   seq++, SIMRPC_STDOUT_DATA);

		simrpc_send_opc(opc);
	}
}

