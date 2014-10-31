/* 
 * File:	 telnet_svc.h
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

#ifndef __TELNET_SVC_H__
#define __TELNET_SVC_H__

struct telnet_svc;

#ifdef __cplusplus
extern "C" {
#endif

int telnet_svc_write(struct telnet_svc * tn, 
					 const void * buf, unsigned int len);

int telnet_svc_read(struct telnet_svc * tn, void * buf,
					unsigned int len, unsigned int msec);

int telnet_svc_flush(struct telnet_svc * tn);

int telnet_svc_release(struct telnet_svc * tn);

struct telnet_svc * telnet_svc_init(int port);

FILE * telnet_svc_fopen(struct telnet_svc * svc);

int telnet_shell(void * stack_buf, int stack_size);

#ifdef __cplusplus
}
#endif	

#endif /* __TELNET_SVC_H__ */
