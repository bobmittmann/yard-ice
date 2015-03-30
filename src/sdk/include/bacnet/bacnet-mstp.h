/* 
 * File:	 bacnet_mstp.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __BACNET_MSTP_H__
#define __BACNET_MSTP_H__

#include <sys/serial.h>

#define BACNET_MSTP_MTU 512

struct bacnet_mstp_hdr {
	uint16_t preamble;
	uint8_t type;
	uint8_t daddr;
	uint8_t saddr;
	uint16_t len;
	uint8_t crc;
} __attribute__((packed));

struct bacnet_mstp_lnk {
	struct serial_dev * dev;
	volatile uint8_t state; 
	uint8_t dln;
	uint8_t ts; /* This station */
	bool sole_master;
	struct {
		union {
			uint8_t buf[BACNET_MSTP_MTU];
			struct {
				struct bacnet_mstp_hdr hdr;
				uint8_t pdu[BACNET_MSTP_MTU - 8];
			};
		};
		volatile uint16_t pdu_len;
		unsigned int off;
		unsigned int cnt;
		int flag;
	} rx;
	struct {
		uint8_t dst_addr;
		uint8_t frm_type;
		volatile uint16_t pdu_len;
		uint8_t pdu[BACNET_MSTP_MTU - 8];
		int flag;
	} tx;
};

#ifdef __cplusplus
extern "C" {
#endif

int __attribute__((noreturn)) bacnet_mstp_task(struct bacnet_mstp_lnk * lnk);

int bacnet_mstp_init(struct bacnet_mstp_lnk * lnk, 
					 const char * name, unsigned int addr,
					 struct serial_dev * dev);

int bacnet_mstp_loop(struct bacnet_mstp_lnk * lnk);

int bacnet_mstp_start(struct bacnet_mstp_lnk * lnk);

int bacnet_mstp_recv(struct bacnet_mstp_lnk * lnk, uint8_t pdu[], 
					unsigned int max);

int bacnet_mstp_send(struct bacnet_mstp_lnk * lnk, const uint8_t pdu[], 
					unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* __BACNET_MSTP_H__ */


