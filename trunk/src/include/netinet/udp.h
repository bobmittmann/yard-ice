/* $Id: udp.h,v 2.0 2006/03/28 15:10:05 bob Exp $ 
 *
 * File:	netinet/udp.h
 * Module:	
 * Project:		
 * Author:	Robinson Mittmann (bob@cnxtech.com)
 * Target:	
 * Comment:	
 * Copyright(c) 2003,2004 CNX Technologies. All Rights Reserved.
 *
 */

#ifndef __NETINET_UDP_H__
#define __NETINET_UDP_H__

#include <stdint.h>

struct udphdr {
	uint16_t sport;
	uint16_t dport;
	uint16_t len;
	uint16_t chksum;
};

#define UDP_HEADER_LEN 8
#define UDP_MAX_DATA_LEN (576 - (20 + UDP_HEADER_LEN))

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __NETINET_UDP_H__ */

