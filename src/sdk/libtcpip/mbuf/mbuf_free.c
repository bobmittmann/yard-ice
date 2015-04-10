/* 
 *
 * File:	mbuf_free.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2005-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include "__mbuf.h"

int mbuf_free(void * __p)
{
	struct mbuf * p = (struct mbuf *)__p; 

#ifdef DEBUG
//	if (!is_mbuf(__p)) {
//		DCC_LOG1(LOG_WARNING, "invalid mbuf: 0x%08x", (int)p);
//		return -1;
//	}
#endif

	DCC_LOG1(LOG_INFO, "p=0x%05x", (int)p);

	thinkos_mutex_lock(__mbufs__.mutex);

	__mbuf_release(p);

	DCC_LOG3(LOG_INFO, "mbuf=%05x used=%d free=%d", (int)p, 
		__mbufs__.used, __mbufs__.max - __mbufs__.used);

	thinkos_cond_signal(__mbufs__.cond);

	thinkos_mutex_unlock(__mbufs__.mutex);

	return 0;
}

