/* 
 * File:	mbuf_wait.c
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include "__mbuf.h"

int mbuf_wait(int mutex)
{
	int ret;

	DCC_LOG1(LOG_TRACE, "mutex=%d", mutex);

	thinkos_mutex_lock(__mbufs__.mutex);
	thinkos_mutex_unlock(mutex);

	DCC_LOG(LOG_TRACE, "cond");
	ret = thinkos_cond_wait(__mbufs__.cond, __mbufs__.mutex);

	DCC_LOG(LOG_TRACE, "unlock");
	thinkos_mutex_unlock(__mbufs__.mutex);

	DCC_LOG(LOG_TRACE, "lock");
	thinkos_mutex_lock(mutex);

	DCC_LOG(LOG_TRACE, "end");
	return ret;
}

