/* $Id: udp_close.c,v 2.16 2008/06/04 00:03:15 bob Exp $ 
 *
 * File:	udp_close.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#define __USE_SYS_UDP__
#include <sys/udp.h>

#include <stdlib.h>

int udp_close(struct udp_pcb * __up)
{
	int ret;

	if (__up == NULL) {
		DCC_LOG1(LOG_WARNING, "<%05x> invalid pcb", (int)__up);
		return -EFAULT;
	}

	tcpip_net_lock();

#if (ENABLE_NET_SANITY_CHECK)
	if (pcb_find((struct pcb *)__up, &__udp__.pcb) < 0) {
		DCC_LOG1(LOG_ERROR, "<%05x> pcb_find()", (int)__up);
		tcpip_net_unlock();
		/* TODO: errno */
		return -1;
	}
#endif

	if ((ret = udp_release(__up)) < 0) {
		DCC_LOG1(LOG_ERROR, "<%05x> udp_release()", (int)__up);
		/* TODO: errno */
		ret = -1;
	}

	tcpip_net_unlock();

	return ret;
}

