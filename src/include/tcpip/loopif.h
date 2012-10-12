/* $Id: loopif.h,v 2.0 2006/03/28 15:10:08 bob Exp $ 
 *
 * File:	tcpip/loopif.h
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2005 CNX Technologies. All Rights Reserved.
 *
 */

#ifndef __TCPIP_LOOPIF_H__
#define __TCPIP_LOOPIF_H__

#include <tcpip/ifnet.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ifnet * loopif_init(in_addr_t ip_addr, in_addr_t netmask);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_LOOPIF_H__ */

