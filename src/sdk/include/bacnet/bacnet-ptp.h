/* 
 * File:	 bacnet_ptp.h
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

#ifndef __BACNET_PTP_H__
#define __BACNET_PTP_H__

#include <sys/serial.h>

#define BACNET_PTP_MTU 512

struct bacnet_ptp_lnk {
	struct serial_dev * dev;
	volatile uint8_t state; 
	uint8_t dln;
	uint32_t clk;
	struct {
		union {
			uint8_t buf[BACNET_PTP_MTU];
			struct {
				uint8_t hdr[6];
				uint8_t pdu[BACNET_PTP_MTU - 6];
			};
		};
		volatile unsigned int pdu_len;
		volatile bool xon;
		unsigned int off;
		unsigned int cnt;
		bool dle;
		uint32_t seq;
		uint32_t idle_tmr;
		int flag;
	} rx;
	struct {
		uint8_t buf[BACNET_PTP_MTU];
		volatile unsigned int len;
		volatile uint32_t seq;
		volatile bool xon;
		uint32_t idle_tmr;
		uint32_t rxmt_tmr;
		uint8_t rxmt_cnt;
		int flag;
	} tx;
};

#ifdef __cplusplus
extern "C" {
#endif

int __attribute__((noreturn)) bacnet_ptp_task(struct bacnet_ptp_lnk * lnk);

int bacnet_ptp_init(struct bacnet_ptp_lnk * lnk, 
					const char * name, 
					struct serial_dev * dev);

int bacnet_ptp_loop(struct bacnet_ptp_lnk * lnk);

int bacnet_ptp_inbound(struct bacnet_ptp_lnk * lnk);

int bacnet_ptp_outbound(struct bacnet_ptp_lnk * lnk);

int bacnet_ptp_recv(struct bacnet_ptp_lnk * lnk, uint8_t pdu[], 
					unsigned int max);

int bacnet_ptp_send(struct bacnet_ptp_lnk * lnk, const uint8_t pdu[], 
					unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* __BACNET_PTP_H__ */


