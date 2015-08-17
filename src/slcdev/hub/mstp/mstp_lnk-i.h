/* 
 * Copyright(C) 2015 Robinson Mittmann. All Rights Reserved.
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
 * @file mstp_lnk-i.h
 * @brief MST/TP link layer private header
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __MSTP_LNK_I_H__
#define __MSTP_LNK_I_H__

#ifdef CONFIG_H
#include "config.h"
#endif

#include <bacnet/mstp_lnk.h>

#include <stdbool.h>
#include <thinkos.h>

#ifndef MSTP_LNK_POOL_SIZE
#define MSTP_LNK_POOL_SIZE 1
#endif

#ifndef MSTP_LNK_MAX_MASTERS
#define MSTP_LNK_MAX_MASTERS 31
#endif

struct mstp_hdr {
	uint16_t preamble;
	uint8_t type;
	uint8_t daddr;
	uint8_t saddr;
	uint16_t len;
	uint8_t crc;
} __attribute__((packed));

struct mstp_frm {
	struct mstp_hdr hdr;
	uint8_t data[];
};

#define MSTP_LNK_MTU 512
#define MSTP_LNK_PDU_MAX (MSTP_LNK_MTU - 11)

struct mstp_lnk {
	bool pool_alloc; /* indicate if the structure was 
						allocated from a resource pool */
	volatile uint8_t state; 
	bool sole_master;
	uint8_t addr; /* this station address */
	struct serial_dev * dev;
	struct {
		uint8_t buf[MSTP_LNK_MTU];
		struct mstp_frame_inf inf;
		volatile uint16_t pdu_len;
		uint8_t pdu[MSTP_LNK_PDU_MAX];
		unsigned int off;
		unsigned int cnt;
		int flag;
	} rx;
	struct {
		uint8_t buf[MSTP_LNK_MTU];
		uint8_t token[8];
		struct mstp_frame_inf inf;
		volatile uint16_t pdu_len;
		uint8_t pdu[MSTP_LNK_PDU_MAX];
		int flag;
	} tx;
	struct {
		void (* callback)(struct mstp_lnk * lnk, unsigned int ev);
		uint32_t netmap[(MSTP_LNK_MAX_MASTERS + 31) / 32];
	} mgmt;
	struct mstp_lnk_stat stat;
};

/* MSTP Connection State Machine states */
enum {
	MSTP_INITIALIZE = 0,
	MSTP_IDLE,
	MSTP_ANSWER_DATA_REQUEST,
	MSTP_NO_TOKEN,
	MSTP_POLL_FOR_MASTER,
	MSTP_PASS_TOKEN,
	MSTP_USE_TOKEN,
	MSTP_DONE_WITH_TOKEN,
	MSTP_WAIT_FOR_REPLY
};



extern const uint8_t mstp_crc8_lut[256];

static inline unsigned int __mstp_crc8(unsigned int crc, int c) {
	return mstp_crc8_lut[crc ^ c];
}

#ifdef __cplusplus
extern "C" {
#endif

unsigned int mstp_crc8(unsigned int crc, const void * buf, int len);

unsigned int mstp_crc16(unsigned int crc, const void * buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* __MSTP_LNK_I_H__ */

