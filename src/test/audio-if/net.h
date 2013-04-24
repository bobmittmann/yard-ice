/* 
 * File:	 net.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
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

#ifndef __NET_H__
#define __NET_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------
 * NET subsystem
 * ----------------------------------------------------------------------
 */
void net_init(void);

int net_send(int sock, const void * buf, int len);

int net_recv(int sock, void * buf, int len);

int audio_send(int stream, sndbuf_t * buf, uint32_t ts);

int audio_recv(int stream, sndbuf_t * buf, uint32_t * ts);

int g711_alaw_send(int stream, sndbuf_t * buf, uint32_t ts);

int g711_alaw_recv(int stream, sndbuf_t * buf, uint32_t * ts);

#ifdef __cplusplus
}
#endif	

#endif /* __NET_H__ */
