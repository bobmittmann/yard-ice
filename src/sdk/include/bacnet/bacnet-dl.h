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

#ifndef __BACNET_DL_H__
#define __BACNET_DL_H__

struct bacnetdl_op {
	int ( * recv)(void * drv, uint8_t pdu[], unsigned int max);
	int ( * send)(void * drv, const uint8_t pdu[], unsigned int max);
};

#ifdef __cplusplus
extern "C" {
#endif

int __attribute__((noreturn)) bacnet_dl_task(void * arg);

int bacnet_dl_init(void);

int bacnet_dl_pdu_recv_notify(int link);

int bacnet_dl_register(const char * name, void * drv, 
					   const struct bacnetdl_op * op);

#ifdef __cplusplus
}
#endif

#endif /* __BACNET_DL_H__ */

