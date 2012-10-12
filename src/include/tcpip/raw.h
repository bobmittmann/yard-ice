/* $Id: raw.h,v 2.1 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	tcpip/raw.h
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:	TCPIP
 * Copyright(c) 2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

/*! \addtogroup comm
 * @{
 */
/*! \defgroup tcpip_udp tcpip/raw.h
 * @{
 */	 
/*! \file
 *	Revision tag: $Revision: 2.1 $ $Date: 2008/06/04 00:03:14 $
 *	@author Robinson Mittman(bob@boreste.com)
 */

#ifndef __TCPIP_RAW_H__
#define __TCPIP_RAW_H__

#include <tcpip/pcb.h>

struct raw_pcb;

#ifdef __cplusplus
extern "C" {
#endif

struct raw_pcb * raw_pcb_new(int __protocol);

int raw_close(struct raw_pcb * __raw);

int raw_sendto(struct raw_pcb * __raw, void * __buf, int __len, 
			   const struct sockaddr_in * __sin);

int raw_recvfrom(struct raw_pcb * __raw, void * __buf, int __len, 
			 struct sockaddr_in * __sin);

int raw_recvfrom_tmo(struct raw_pcb * __raw, void * __buf, 
				 int __len, struct sockaddr_in * __sin, 
				 unsigned int msec);

int raw_ioctl(struct raw_pcb * __raw, int __cmd, void * __data);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_RAW_H__ */

