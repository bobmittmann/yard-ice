/* 
 * File:	 slcdev.h
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

#ifndef __STATS_H__
#define __STATS_H__

#include <stdint.h>

struct slcdev_stats {
	uint32_t proto_err; /* General protocol error */
	uint32_t clip_poll; /* Clip poll received */
	uint32_t clip_err;  /* Clip parity error */
	uint32_t clip_resp; /* Clip poll responses */
	uint32_t clip_pw5;  /* CLip PW5 requests */
	uint32_t ap_poll;   /* AP mode poll received */
	uint32_t ap_herr;   /* AP mode header checksum error */
	uint32_t ap_null;   /* AP Null */
	uint32_t ap_ralt;   /* AP Read Alarm Latch Tens" */
	uint32_t ap_rtlt;   /* AP Read Trouble Latch Tens */
	uint32_t ap_rtltsb; /* AP Read Trouble Latch Tens Sounder Base */
	uint32_t ap_rp;     /* AP Read Presence */
	uint32_t ap_ralu;   /* AP Read Alarm Latch Units */
	uint32_t ap_rtlu;   /* AP Read Trouble Latch Units */
	uint32_t ap_rpsb;   /* AP Read Presence Sounder Base */
	uint32_t ap_rtlusb; /* AP Read Trouble Latch Units Sounder Base"*/
	uint32_t ap_uerr;   /* AP unkown */
};

#endif /* __STATS_H__ */

