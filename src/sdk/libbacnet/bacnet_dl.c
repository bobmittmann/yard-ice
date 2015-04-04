/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/param.h>
#include <thinkos.h>
#include <crc.h>
#include <errno.h>

#include <sys/dcclog.h>

#include <bacnet/bacnet-dl.h>

#ifdef CONFIG_H
#include "config.h"
#endif

#include "dcc.h"
#include "npdu.h"
#include "address.h"
#include "device.h"
#include "bactext.h"
#include "bacerror.h"
#include "dlenv.h"
#include "tsm.h"
#include "lc.h"

/* some demo stuff needed */
#include "handlers.h"
#include "client.h"


/* -------------------------------------------------------------------------
 * Data Link Layer Control
 * ------------------------------------------------------------------------- */

struct bacnetdl_dev {
	const char * nm;
	void * drv;
	const struct bacnetdl_op * op;
};

#define BACNET_DL_DEV_MAX 12

struct {
	struct bacnetdl_dev dev[BACNET_DL_DEV_MAX];
	struct bacnetdl_dev * volatile reply_dev;
	struct {
		int ev;
	} rx;
	struct {
		int ev;
	} tx;
} __bacnet_dl;


int bacnet_dl_register(const char * name, void * drv, 
					   const struct bacnetdl_op * op)
{
	int i;

	for (i = 0; i < BACNET_DL_DEV_MAX; ++i) {
		if (__bacnet_dl.dev[i].drv == NULL) {
			__bacnet_dl.dev[i].drv = drv;
			__bacnet_dl.dev[i].nm = name;
			__bacnet_dl.dev[i].op = op;
			DCC_LOG2(LOG_TRACE, "\"%s\"-> %d", name, i);
			return i;
		}
	}
	return -1;
}

#define VIRTUAL_DNET 1

/** The list of DNETs that our router can reach.
 *  Only one entry since we don't support downstream routers.
 */
int DNET_list[8] = {
    VIRTUAL_DNET, -1    /* Need -1 terminator */
};

#define DCC_TIME_MS        1000

int __attribute__((noreturn)) bacnet_dl_task(void * arg)
{
    BACNET_ADDRESS src = {
        0
    };  /* address where message came from */
	struct bacnetdl_dev * dev;
	struct bacnetdl_addr addr;
	uint8_t pdu[512];
	int pdu_len;
	uint32_t clk;
	uint32_t dcc_clk;
	int ev;
	int netif;

	clk = thinkos_clock();
	dcc_clk = clk + DCC_TIME_MS; 

	for (;;) {
		/* set the production enable flag to start production */
		ev = thinkos_ev_timedwait(__bacnet_dl.rx.ev, 100);
		clk = thinkos_clock();
		if (ev > 0) {
			DCC_LOG3(LOG_TRACE, "%5d.%03d event=%d", 
					 clk / 1000, clk % 1000, ev);
			netif = ev;
			dev = &__bacnet_dl.dev[netif];
			addr.netif = netif;

			if ((pdu_len = dev->op->recv(dev->drv, &addr, pdu, 512)) > 0) {
				__bacnet_dl.reply_dev = dev;
	

				if (addr.mac_len == 0)
					DCC_LOG1(LOG_TRACE, "[%d]", addr.netif);
				else
					DCC_LOG2(LOG_TRACE, "[%d %d]", addr.netif, addr.mac[0]);

				//#if BAC_ROUTING
#if 0
				routing_npdu_handler(&src, DNET_list, pdu, pdu_len);
#else
				src.mac_len = addr.mac_len;
				src.mac[0] = addr.mac[0];
				src.mac[1] = addr.mac[1];
				src.mac[2] = addr.mac[2];
				src.mac[3] = addr.mac[3];
				src.mac[4] = addr.mac[4];
				src.mac[5] = addr.mac[5];
				npdu_handler(&src, pdu, pdu_len);
#endif
			}
		} else if (ev != THINKOS_ETIMEDOUT) {
			DCC_LOG(LOG_ERROR, "thinkos_ev_wait() failed!");
			abort();
		}

		if (((int32_t)(dcc_clk - clk)) < 0) {
			uint32_t dt = DCC_TIME_MS + (clk - dcc_clk);
			dcc_clk += DCC_TIME_MS; 
			DCC_LOG2(LOG_INFO, "%5d.%03d timeout...", clk / 1000, clk % 1000);
			dcc_timer_seconds(dt * 1000);
//            dlenv_maintenance_timer(dt * 1000);
 //           Load_Control_State_Machine_Handler();
            handler_cov_task();
            tsm_timer_milliseconds(dt);
		}
	}
}

int bacnet_dl_pdu_recv_notify(int link)
{
	return thinkos_ev_raise(__bacnet_dl.rx.ev, link);
}

uint32_t bacnetdl_stack[512];

const struct thinkos_thread_inf bacnetdl_inf = {
	.stack_ptr = bacnetdl_stack, 
	.stack_size = sizeof(bacnetdl_stack), 
	.priority = 32,
	.thread_id = 4, 
	.paused = 0,
	.tag = "BACnet"
};

int bacnet_dl_init(void)
{
	DCC_LOG(LOG_TRACE, "...");
	__bacnet_dl.rx.ev = thinkos_ev_alloc();
	thinkos_thread_create_inf((void *)bacnet_dl_task, 
							  (void *)NULL, &bacnetdl_inf);
	return 0;
}

int bacnet_dl_send(struct bacnetdl_dev * dev, struct bacnetdl_addr * addr,
				   const uint8_t pdu[], unsigned int pdu_len)
{
	return dev->op->send(dev->drv, addr, pdu, pdu_len);
}

struct bacnetdl_addr * bacnet_dl_getaddr(struct bacnetdl_dev * dev)
{
	return dev->op->getaddr(dev->drv);
}

struct bacnetdl_addr * bacnet_dl_getbcast(struct bacnetdl_dev * dev)
{
	return dev->op->getbcast(dev->drv);
}

/* -------------------------------------------------------------------------
   Open source bacnet stack 
   ------------------------------------------------------------------------- */

int datalink_send_pdu(BACNET_ADDRESS * dest, BACNET_NPDU_DATA * npdu_data,
					  uint8_t * pdu, unsigned pdu_len)
{
	struct bacnetdl_addr * addr = (struct bacnetdl_addr *)dest;

	DCC_LOG(LOG_TRACE, "...");

	return bacnet_dl_send(__bacnet_dl.reply_dev, addr, pdu, pdu_len);
}


#if 0
uint16_t datalink_receive(BACNET_ADDRESS * src, uint8_t * pdu,
						  uint16_t max_pdu, unsigned timeout)
{
	return 0;
}
#endif

void datalink_cleanup(void)
{
	DCC_LOG(LOG_TRACE, "...");
	return;
}

void datalink_get_broadcast_address(BACNET_ADDRESS * dest)
{
	struct bacnetdl_addr * bcast;

	DCC_LOG(LOG_TRACE, "...");

	bcast = bacnet_dl_getbcast(__bacnet_dl.reply_dev);
    dest->mac_len = bcast->mac_len;
    dest->mac[0] = bcast->mac[0];
    dest->mac[1] = bcast->mac[1];
    dest->mac[2] = bcast->mac[2];
    dest->mac[3] = bcast->mac[3];
    dest->mac[4] = bcast->mac[4];
    dest->mac[5] = bcast->mac[5];

    dest->net = 0;
    dest->len = 0;
}

void datalink_get_my_address(BACNET_ADDRESS * my_address)
{
	struct bacnetdl_addr * addr;

	DCC_LOG(LOG_TRACE, "...");

	addr = bacnet_dl_getaddr(__bacnet_dl.reply_dev);
    my_address->mac_len = addr->mac_len;
    my_address->mac[0] = addr->mac[0];
    my_address->mac[1] = addr->mac[1];
    my_address->mac[2] = addr->mac[2];
    my_address->mac[3] = addr->mac[3];
    my_address->mac[4] = addr->mac[4];
    my_address->mac[5] = addr->mac[5];

    my_address->net = 0;
    my_address->len = 0;        
}

void datalink_set_interface(char *ifname)
{
	DCC_LOG(LOG_TRACE, "...");
	return;
}

void datalink_set(char *datalink_string)
{
	DCC_LOG(LOG_TRACE, "...");
	return;
}

