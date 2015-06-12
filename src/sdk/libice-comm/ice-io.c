/* 
 * File:	ice-io.c
 * Author:  Robinson Mittmann (bobmittmann@gmail.com)
 * Target: 
 * Comment: 
 * Copyright(C) 2012 Bob Mittmann. All Rights Reserved.
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

#include <sys/param.h> /* MIN(), MAX() macros */
#include <stdlib.h>
#include <sys/ice-comm.h>
#include <arch/cortex-m3.h>

int ice_io_comm_write(struct ice_comm_blk * comm, const void * buf, 
					   unsigned int len)
{
	uint32_t fm = cm3_faultmask_get(); /* save fault mask */
	unsigned int head;
	char * cp = (char *)buf;
	int rem = len;
	int w;
	int n;
	int i;

	cm3_cpsid_f(); /* disable interrupts and faults */
	if (comm->dbg != DBG_CONNECTED) {
		if (comm->dbg == DBG_SYNC)
			comm->dev = DEV_CONNECTED;
		goto ret;
	}
	head = comm->tx_head;
	while (rem) {
		while ((w = 16 - ((head - comm->tx_tail) & 0xffff)) <= 0);
		i = 0;
		n = 0;
		/* Each word in the buffer can hold 4 characters.
		   Reserve 1 position for a possible CR->CR/LF conversion
		   in the last character. */
		while (i < MIN(w * 4 - 1, rem)) {
			if (*cp == '\n') {
				comm->tx_buf.u8[(head * 4 + i) & 0x3f] = '\r';
				i++;
			}
			comm->tx_buf.u8[(head * 4 + i) & 0x3f] = *cp++;
			i++;
			n++;
		}
		rem -= n;

		/* get the number of effectivelly used words. */
		w = (i + 3) / 4; 
		for (; i < (w * 4); i++)
			comm->tx_buf.u8[(head * 4 + i) & 0x3f] = '\0';

		head = (head + w) & 0xffff;
		/* write back */
		comm->tx_head = head;
	}
ret:
	cm3_faultmask_set(fm);  /* restore fault mask */
	return len;
}

int ice_io_comm_read(struct ice_comm_blk * comm, void * buf, unsigned int len)
{
	return 0;
}

const struct fileop ice_io_comm_ops = {
	.write = (void *)ice_io_comm_write,
	.read = (void *)ice_io_comm_read,
	.flush = (void *)NULL,
	.close = (void *)NULL
};

const struct file ice_comm_file = {
	.data = &ice_comm_blk,
	.op = &ice_io_comm_ops
};

struct file * ice_comm_open(void)
{
	ice_comm_sync();

	ice_comm_connect(); 

	return (struct file *)&ice_comm_file;
}

