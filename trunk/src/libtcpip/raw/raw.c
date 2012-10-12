/* $Id: raw.c,v 2.1 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	raw.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2004-2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_RAW__
#include <sys/raw.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/os.h>
#include <sys/mbuf.h>

struct raw_pcb * ip_raw = NULL;

struct raw_pcb * raw_pcb_new(int __protocol)
{
	struct raw_pcb * raw;

	tcpip_net_lock();

	if (ip_raw != NULL) {
		DCC_LOG(LOG_WARNING, "raw pcb already allocated!");
		tcpip_net_unlock();
		return NULL;
	}

	/* get a new memory descriptor */
	if ((raw = (struct raw_pcb *)mbuf_alloc()) == NULL) {
		DCC_LOG(LOG_WARNING, "could not allocate a mbuf");
		tcpip_net_unlock();
		return NULL;
	}


	/* ensure the mem is clean */
	memset(raw, 0, sizeof(struct raw_pcb));

	raw->r_cond = __os_cond_alloc();
	raw->r_protocol = __protocol;
	ip_raw = raw;

	tcpip_net_unlock();

	return raw;
} 

int raw_pcb_free(struct raw_pcb * __raw)
{
/*	int id;

	if (!is_mbuf(__raw)) {
		DCC_LOG1(LOG_WARNING, "invalid PCB!");
		return -1;
	}
		
	id = mbuf_id(__raw);
	if (ip_raw != id) {
		DCC_LOG1(LOG_WARNING, "invalid RAW PCB!");
		return -1;
	} */

	__os_cond_free(__raw->r_cond);
	mbuf_free(__raw);

	ip_raw = NULL;

	return 0;
} 

int raw_input(struct ifnet * __if, struct iphdr * __ip, int __len)
{
	struct raw_pcb * raw;

	if (ip_raw == NULL) {
		return 0;
	}

	raw = (struct raw_pcb *)ip_raw;

	if (raw->r_protocol != __ip->proto) {
		DCC_LOG1(LOG_TRACE, "unknown protocol %d", __ip->proto); 
		return 0;
	}

	DCC_LOG3(LOG_TRACE, "%I > %I (%d)", __ip->saddr, __ip->daddr, __len); 

	raw->r_buf = __ip;
	raw->r_len = __len;
	raw->r_faddr = __ip->saddr;
    raw->r_laddr = __ip->daddr;

	__os_cond_signal(raw->r_cond);

	__os_cond_wait(raw->r_cond, net_mutex);

	return 1;
} 

