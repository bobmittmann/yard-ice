/* 
 * File:	tcpip/stat.h
 * Module:	
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2003-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 *
 */

#ifndef __TCPIP_STAT_H__
#define __TCPIP_STAT_H__

struct proto_stat {
	uint32_t rx_ok;
	uint32_t rx_err;
	uint32_t rx_drop;
	uint32_t tx_ok;
	uint32_t tx_err;
	uint32_t tx_drop;
};

#ifdef __cplusplus
extern "C" {
#endif

void proto_stat_copy(struct proto_stat * __dst, 
					 struct proto_stat * __src, int __rst);

#ifdef __cplusplus
}
#endif

#endif /* __TCPIP_STAT_H__ */

