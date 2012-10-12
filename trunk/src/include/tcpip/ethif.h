/* $Id: ethif.h,v 2.0 2006/03/28 15:10:08 bob Exp $ 
 *
 * File:	tcpip/ethif.h
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:	
 * Comment:	
 * Copyright(c) 2003-2005 CNX Technologies. All Rights Reserved.
 *
 */

#ifndef __TCPIP_ETHIF_H__
#define __TCPIP_ETHIF_H__

#include <tcpip/ifnet.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ifnet * ethif_init(in_addr_t ip_addr, in_addr_t netmask);

void ethif_stats_show(FILE * f, int clear);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_ETHIF_H__ */

