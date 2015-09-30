/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file simlnk.h
 * @brief YARD-ICE libhexdump
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SIMLNK_H__
#define __SIMLNK_H__

#include <stdlib.h> 
#include <stdbool.h> 
#include <stdint.h> 
#include <sys/serial.h> 

#include "simrpc_clnt.h"

#define SIMLNK_MTU (512 + 4)
#define SIMLNK_BAUDRATE 125000

struct simlnk_stat {
	uint32_t rx_err;
	uint32_t rx_token;
	uint32_t rx_mgmt;
	uint32_t rx_unicast;
	uint32_t rx_bcast;
	uint32_t tx_token;
	uint32_t tx_mgmt;
	uint32_t tx_unicast;
	uint32_t tx_bcast;
	uint32_t token_lost;
};

#ifdef __cplusplus
extern "C" {
#endif

struct simlnk * simlnk_alloc(void);

int simlnk_init(struct simlnk * lnk, const char * name, 
				unsigned int addr, struct serial_dev * dev);

int simlnk_send(struct simlnk * lnk, const void * buf, unsigned int cnt); 

int simlnk_recv(struct simlnk * lnk, void * buf, unsigned int max,
				unsigned int tmo);

int simlnk_rpc(struct simrpc_pcb * sp, uint32_t insn,
			   const void * req, unsigned int cnt,
			   void * rsp, unsigned int max);

int simlnk_rpc_async(struct simrpc_pcb * sp, uint32_t insn,
			   const void * buf, unsigned int cnt);

#ifdef __cplusplus
}
#endif

#endif /* __SIMLNK_H__ */

