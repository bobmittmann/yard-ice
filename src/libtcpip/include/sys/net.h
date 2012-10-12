/* $Id: net.h,v 1.1 2008/05/23 03:54:45 bob Exp $ 
 *
 * File:	sys/net.h
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:	Network Framework
 * Copyright(c) 2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifndef __SYS_NET_H__
#define __SYS_NET_H__

#include <sys/os.h>
#include <stdint.h>

/* Network Framework mutex - all functions accessing the network stack must
 lock this mutex to avoid race conditions...  */
extern int8_t net_mutex;

#ifdef __cplusplus
extern "C" {
#endif

static inline int tcpip_net_lock(void) {
	return __os_mutex_lock(net_mutex);
}

static inline int tcpip_net_unlock(void) {
	return __os_mutex_unlock(net_mutex);
}

#ifdef __cplusplus
}
#endif

#endif /* __SYS_NET_H__ */

