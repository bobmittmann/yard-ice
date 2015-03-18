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
 * @file stm32f-ethif.c
 * @brief STM32F Ethernet driver
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/stm32f.h>

#ifdef STM32F_ETH

#include <sys/stm32f_ethif.h>
#include <sys/etharp.h>

#include <tcpip/in.h>

#define __USE_SYS_IFNET__
#include "sys/ifnet.h"
#define __USE_SYS_ROUTE__
#include "sys/route.h"
#include "sys/net.h"
#include "sys/ip.h"

#define __THINKOS_IRQ__
#include <thinkos_irq.h>


#if THINKOS_ENABLE_FLAG_ALLOC && ENABLE_NETWORK

void __attribute__((noreturn)) stm32f_ethif_input(struct ifnet * ifn)
{
	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)ifn->if_drv;
	struct stm32f_eth * eth = (struct stm32f_eth *)ifn->if_io;
	struct rxdma_ext_st ext_st;
	struct rxdma_st st;
	struct rxdma_enh_desc * desc;
	struct eth_hdr * hdr;
	uint32_t cnt = 0;
	uint8_t * pkt;
	int type;
	int len;

	DCC_LOG3(LOG_TRACE, "RX DMA: %d (desc) x %d = %d bytes", 
			 STM32F_ETH_RX_NDESC, STM32F_ETH_RX_BUF_SIZE + 
			 sizeof(struct rxdma_enh_desc),
			 STM32F_ETH_RX_NDESC * (STM32F_ETH_RX_BUF_SIZE + 
			 sizeof(struct rxdma_enh_desc))); 
	DCC_LOG3(LOG_TRACE, "TX DMA: %d (desc) x %d = %d bytes", 
			 STM32F_ETH_TX_NDESC, STM32F_ETH_TX_BUF_SIZE + 
			 sizeof(struct txdma_enh_desc) + sizeof(struct eth_hdr),
			 STM32F_ETH_TX_NDESC * (STM32F_ETH_TX_BUF_SIZE + 
			 sizeof(struct txdma_enh_desc) + sizeof(struct eth_hdr)));
	DCC_LOG1(LOG_TRACE, "Ethernet driver memory: %d bytes", 
			 sizeof(struct stm32f_eth_drv));
	DCC_LOG1(LOG_TRACE, "Ethernet MTU : %d bytes", STM32F_ETH_PAYLOAD_MAX);

	DCC_LOG1(LOG_TRACE, "<%d> DMA interrupts enabled ...", 
			 thinkos_thread_self());

	/* enable DMA RX interrupts */
	eth->dmaier |= ETH_RIE | ETH_NISE;

	DCC_LOG(LOG_INFO, "DMA start receive...");
	eth->dmaomr |= ETH_SR;

	for (;;) {
		desc = &drv->rx.desc[cnt++ & (STM32F_ETH_RX_NDESC - 1)];

		for (;;) {
			st = desc->st;
			if (st.es) {
				DCC_LOG(LOG_ERROR, "Error summary flag set!");
			}
			if (st.own == 0) {
				break;
			}
			DCC_LOG(LOG_MSG, "wait....");

			thinkos_sem_wait(drv->rx.sem);

			DCC_LOG(LOG_MSG, "wakeup....");
		}

		len = st.fl;
		DCC_LOG1(LOG_INFO, "frame len=%d", len);

		ext_st = desc->ext_st;

		/* IP payload type */
		switch (ext_st.ippt) {
		case ETH_IPPT_UNKOWN:
			DCC_LOG(LOG_INFO, "not IP!");
			break;
		case ETH_IPPT_UDP:
			DCC_LOG(LOG_INFO, "UDP");
			break;
		case ETH_IPPT_TCP:
			DCC_LOG(LOG_INFO, "TCP");
			break;
		case ETH_IPPT_ICMP:
			DCC_LOG(LOG_INFO, "ICMP");
			break;
		}

		if (ext_st.iphe)
			DCC_LOG(LOG_WARNING, "IP header error!");
		if (ext_st.ippe)
			DCC_LOG(LOG_WARNING, "IP payload error!");
		if (ext_st.ipcb)
			DCC_LOG(LOG_TRACE, "IP checksum bypass.");
		if (ext_st.ipv4pr)
			DCC_LOG(LOG_INFO, "IPv4 packet received.");
		if (ext_st.ipv6pr)
			DCC_LOG(LOG_TRACE, "IPv6 packet received.");

		hdr = (struct eth_hdr *)desc->rbap1;

#if LOG_LEVEL == LOG_INFO
		{
			uint8_t * da = hdr->eth_dst;
			uint8_t * sa = hdr->eth_src;
			DCC_LOG6(LOG_LEVEL, "Dst: %02x:%02x:%02x:%02x:%02x:%02x",
					 da[0], da[1], da[2], da[3], da[4], da[5]);

			DCC_LOG6(LOG_LEVEL, "Src: %02x:%02x:%02x:%02x:%02x:%02x",
					 sa[0], sa[1], sa[2], sa[3], sa[4], sa[5]);
		}
#endif


		type = hdr->eth_type;
		pkt = (uint8_t *)hdr + 14;
		len -= 14;

		DCC_LOG8(LOG_MSG, "Pkt: %02x %02x %02x %02x %02x %02x %02x %02x",
				 pkt[0], pkt[1], pkt[2], pkt[3], 
				 pkt[4], pkt[5], pkt[6], pkt[7]);

		tcpip_net_lock();

		NETIF_STAT_ADD(ifn, rx_pkt, 1);

		if (type == HTONS(ETH_P_IP)) {
			DCC_LOG(LOG_INFO, "IP");
			ip_input(ifn, (struct iphdr *)pkt, len);
		} else {
			if (type == HTONS(ETH_P_ARP)) {
				DCC_LOG(LOG_INFO, "ARP");
				etharp_input(ifn, (struct etharp*)pkt, len);
			} else {
				NETIF_STAT_ADD(ifn, rx_drop, 1);
				DCC_LOG(LOG_INFO, "unhandled protocol");
			}
		}

		tcpip_net_unlock();

		/* set the DMA descriptor ownership */
		desc->rdes0 = ETH_RXDMA_OWN;
		/* enable DMA receive interrupts */
		eth->dmaier |= ETH_RIE;
		/* Receive Poll Demand command */
		eth->dmarpdr = 1;
	}
}

uint32_t ethif_stack[128];

const struct thinkos_thread_inf stm32f_ethif_inf = {
	.stack_ptr = ethif_stack, 
	.stack_size = sizeof(ethif_stack), 
	.priority = 32,
	.thread_id = 32, 
	.paused = 0,
	.tag = "STM_ETH"
};

int stm32f_ethif_init(struct ifnet * __if)
{
	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)__if->if_drv;
	struct stm32f_eth * eth = (struct stm32f_eth *)__if->if_io;
	struct rxdma_enh_desc * rxdesc;
	struct txdma_enh_desc * txdesc;
	int mtu;
	int i;

	DCC_LOG2(LOG_TRACE, "if=0x%p drv=0x%p", __if, drv);
	drv->ifn = __if;
	drv->eth = eth;

	mtu = STM32F_ETH_PAYLOAD_MAX;
	__if->if_mtu = mtu;
	__if->if_link_speed = 100000000;
	/* set the broadcast flag */
	__if->if_flags |= IFF_BROADCAST;

	DCC_LOG1(LOG_TRACE, "mtu=%d", mtu);

	DCC_LOG(LOG_TRACE, "MAC configuration ...");
	/* Bit 25 - CRC stripping for Type frames */
	/* Bit 14 - Fast Ethernet speed */
	/* Bit 13 - Receive own disable */
	/* Bit 11 - Duplex mode */
	/* Bit 10 - IPv4 checksum offload */
	/* Bit 3 - Transmitter enable */
	/* Bit 2 - Receiver enable */
	eth->maccr = ETH_CSTF | ETH_FES | ETH_ROD | ETH_DM | ETH_IPCO | 
		ETH_TE | ETH_RE;

	DCC_LOG(LOG_TRACE, "DMA operation mode ...");
	/* Bit 21 - Transmit store and forward */
	/* Bit 25 - Receive store and forward */
	/* Bit 20 - Flush transmit FIFO */
	eth->dmaomr = ETH_RSF | ETH_TSF | ETH_FTF;

	DCC_LOG(LOG_TRACE, "DMA bus mode ...");
	/* Bit 7 - Enhanced descriptor format enable */
	eth->dmabmr = ETH_EDFE;

	DCC_LOG(LOG_TRACE, "DMA RX descriptors ...");
	for (i = 0; i < STM32F_ETH_RX_NDESC; ++i) {
		/* configure recevie descriptors */
		rxdesc = &drv->rx.desc[i];
		rxdesc->rdes0 = ETH_RXDMA_OWN;
		/* Receive buffer 1 size */
		rxdesc->rbs1 = STM32F_ETH_RX_BUF_SIZE;
		/* Receive end of ring */
		rxdesc->rer = 0;
		/* Disable interrupt on comletion */
		rxdesc->dic = 0;
		/* Second address chained */
		rxdesc->rch = 1;
		rxdesc->rbap1 = drv->rx.buf[i];
		/* link to the next descriptor */
		rxdesc->rbap2 = (void *)&drv->rx.desc[i + 1];
	}

	/* link to the first */
	rxdesc->rbap2 = (void *)&drv->rx.desc[0];

	/* DMA receive descriptor list address */
	eth->dmardlar = (uint32_t)&drv->rx.desc[0];

	/* alloc a new semaphore */
	drv->rx.sem = thinkos_sem_alloc(0); 
	DCC_LOG1(LOG_TRACE, "rx.sem=%d", drv->rx.sem);

	DCC_LOG(LOG_TRACE, "DMA TX descriptors ...");

	for (i = 0; i < STM32F_ETH_TX_NDESC; ++i) {
		/* setup the source address in the ethernet header 
		   of the  transmit buffer */
		stm32f_eth_mac_get(eth, 0, (uint8_t *)drv->tx.hdr[i].eth_src);

		/* configure transmit descriptors */
		txdesc = &drv->tx.desc[i];
		/* Transmit buffer 1 size */
		txdesc->tbs1 = 14;
		/* Transmit buffer 2 size */
		txdesc->tbs2 = 0;
		/* Transmit buffer 1 addr */
		txdesc->tbap1 = (void *)&drv->tx.hdr[i];
		/* Transmit buffer 2 addr */
		txdesc->tbap2 = drv->tx.buf[i];
		/* Transmit end of ring */
		txdesc->tdes0 = ETH_TXDMA_TER;
	}

	/* DMA transmit descriptor list address */
	eth->dmatdlar = (uint32_t)&drv->tx.desc[0];

	/* alloc a new event wait queue */
	drv->tx.flag = thinkos_flag_alloc(); 
	DCC_LOG1(LOG_TRACE, "tx.flag=%d", drv->tx.flag);


	DCC_LOG(LOG_TRACE, "__os_thread_create()");
	thinkos_thread_create_inf((void *)stm32f_ethif_input, (void *)__if, 
							  &stm32f_ethif_inf);

	/* set the interrupt priority */
	__thinkos_irq_pri_set(STM32F_IRQ_ETH, IRQ_PRIORITY_VERY_HIGH);
	/* enable interrupts */
	__thinkos_irq_enable(STM32F_IRQ_ETH);

	/* enable Normal interrupt summary and Abnormal interrupt summary */
	eth->dmaier = ETH_NISE | ETH_AISE | ETH_FBEIE | 
		ETH_RWTIE | ETH_RBUIE |ETH_ROIE |
		ETH_TUIE | ETH_TJTIE | ETH_TBUIE;

	return 0;
}

int stm32f_ethif_cleanup(struct ifnet * __if)
{
	return 0;
}

void * stm32f_ethif_mmap(struct ifnet * __if, size_t __length)
{
	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)__if->if_drv;
	void * ptr;
#if 1
	struct txdma_enh_desc * txdesc;
	struct txdma_st st;

	txdesc = &drv->tx.desc[0];
	for (;;) {
		/* wait for buffer availability */
		st = txdesc->st;
		if (st.es) {
			DCC_LOG10(LOG_WARNING, "Error:%s%s%s%s%s%s%s%s%s%s",
					 (st.uf) ? " UF" : "",
					 (st.ed) ? " ED" : "",
					 (st.ec) ? " EC" : "",
					 (st.lco) ? " LCO" : "",
					 (st.nc) ? " NC" : "",
					 (st.lca) ? " LCA" : "",
					 (st.ipe) ? " IPE" : "",
					 (st.ff) ? " FF" : "",
					 (st.jt) ? " JT" : "",
					 (st.ihe) ? " IHE" : "");
		}
		if (st.own == 0) {
			break;
		}
		DCC_LOG(LOG_MSG, "wait....");
		thinkos_flag_take(drv->tx.flag);
		DCC_LOG(LOG_MSG, "wakeup....");
	}
#else
	/* chek for availability  */
	if (drv->tx.desc.st.own) {
		DCC_LOG(LOG_WARNING, "not available TX buffers!");
		return NULL;
	}
#endif

	ptr = &drv->tx.buf[14];

	DCC_LOG2(LOG_INFO, "mem=%p len=%d", ptr, __length);

	return ptr;
}

int stm32f_ethif_munmap(struct ifnet * __if, void * __mem)
{
//	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)__if->if_drv;
//
	DCC_LOG(LOG_INFO, "...");
	return 0;
}

int stm32f_ethif_send(struct ifnet * __if, const uint8_t * __dst, 
					  int __proto, const void * __buf, int __len)
{
	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)__if->if_drv;
	struct stm32f_eth * eth = (struct stm32f_eth *)__if->if_io;
	struct txdma_enh_desc * txdesc;
	struct txdma_st st;
	struct eth_hdr * hdr;

	DCC_LOG1(LOG_INFO, "len=%d", __len);

	if (__len > __if->if_mtu) {
		NETIF_STAT_ADD(__if, tx_drop, 1);
		DCC_LOG(LOG_WARNING, "too big!");
		return -1;
	}

	txdesc = &drv->tx.desc[0];
	hdr = &drv->tx.hdr[0];
	for (;;) {
		/* wait for buffer availability */
		st = txdesc->st;
		if (st.es) {
			DCC_LOG10(LOG_WARNING, "Error:%s%s%s%s%s%s%s%s%s%s",
					 (st.uf) ? " UF" : "",
					 (st.ed) ? " ED" : "",
					 (st.ec) ? " EC" : "",
					 (st.lco) ? " LCO" : "",
					 (st.nc) ? " NC" : "",
					 (st.lca) ? " LCA" : "",
					 (st.ipe) ? " IPE" : "",
					 (st.ff) ? " FF" : "",
					 (st.jt) ? " JT" : "",
					 (st.ihe) ? " IHE" : "");
		}
		if (st.own == 0) {
			break;
		}
		DCC_LOG(LOG_MSG, "wait....");
		thinkos_flag_take(drv->tx.flag);
		DCC_LOG(LOG_MSG, "wakeup....");
	}

	DCC_LOG7(LOG_MSG, "to: %02x:%02x:%02x:%02x:%02x:%02x (%d)",
			 __dst[0], __dst[1], __dst[2], __dst[3], __dst[4], __dst[5], __len);

	hdr->eth_type = __proto;

	/* ethernet destination address */
	hdr->eth_dst[0] = __dst[0];
	hdr->eth_dst[1] = __dst[1];
	hdr->eth_dst[2] = __dst[2];
	hdr->eth_dst[3] = __dst[3];
	hdr->eth_dst[4] = __dst[4];
	hdr->eth_dst[5] = __dst[5];

	/* Transmit buffer 1 size */
	txdesc->tbs1 = 14;
	/* Transmit buffer 2 size */
	txdesc->tbs2 = __len;
	/* Transmit buffer 1 addr */
	txdesc->tbap1 = (void *)hdr;
	/* Transmit buffer 2 addr */
	txdesc->tbap2 = (void *)__buf;
	/* set the DMA descriptor ownership and end of ring */
	txdesc->tdes0 = ETH_TXDMA_OWN | ETH_TXDMA_IC | 
		ETH_TXDMA_LS | ETH_TXDMA_FS | ETH_TXDMA_TER;
	/* enable DMA transmit interrupts */
//	eth->dmaier |= ETH_TIE;
//	/* Transmit Poll Demand command */
//	eth->dmatpdr = 1;
	DCC_LOG(LOG_MSG, " DMA start transmit...");
	eth->dmaomr |= ETH_ST;

	return 0;
}

int stm32f_ethif_getaddr(struct ifnet * __if, uint8_t * __buf)
{
	struct stm32f_eth * eth = (struct stm32f_eth *)__if->if_io;

	stm32f_eth_mac_get(eth, 0, __buf);
	return 0;
}

int stm32f_ethif_getdesc(struct ifnet * __if, char * __s, int __len)
{
	return 0;
}

struct stm32f_eth_drv stm32f_eth_drv;

void stm32f_eth_isr(void)
{
	struct stm32f_eth_drv * drv = &stm32f_eth_drv;
	struct stm32f_eth * eth = STM32F_ETH;
	uint32_t dmasr;

//	DCC_LOG1(LOG_TRACE, "if=0x%p", ifn);

	dmasr = eth->dmasr;
//	show_dma_status(dmasr);

	DCC_LOG1(LOG_INFO, "DMASR=0x%08x", dmasr);

	if (dmasr & ETH_RS) {
		DCC_LOG(LOG_INFO, "DMA RS");
		/* disable DMA receive interrupts */
		eth->dmaier &= ~ETH_RIE;
		thinkos_sem_post_i(drv->rx.sem);
	}

	if (dmasr & ETH_TS) {
		DCC_LOG(LOG_MSG, "DMA TS, stop transmission...");
		eth->dmaomr &= ~ETH_ST;
		thinkos_flag_give_i(drv->tx.flag);
	}

	if (dmasr & ETH_TBUS) {
		DCC_LOG(LOG_INFO, "TBUS");
	}

	if (dmasr & ETH_AIS) {
		if (dmasr & ETH_RBUS) {
			DCC_LOG(LOG_INFO, "RBUS");
			/* RBUS: Receive buffer unavailable status
			   This bit indicates that the next descriptor in the receive list 
			   is owned by the host and cannot be acquired by the DMA. Receive 
			   process is suspended. To resume processing receive descriptors, 
			   the host should change the ownership of the descriptor and 
			   issue a Receive Poll Demand command. If no Receive Poll Demand 
			   is issued, receive process resumes when the next recognized 
			   incoming frame is received. ETH_DMASR [7] is set only when the 
			   previous receive descriptor was owned by the DMA. */
		} else {
			DCC_LOG9(LOG_WARNING, "AIS:%s%s%s%s%s%s%s%s%s",
					 (dmasr & ETH_FBES) ? " FBES" : "",
					 (dmasr & ETH_ETS) ? " ETS" : "",
					 (dmasr & ETH_RPSS) ? " RPSS" : "",
					 (dmasr & ETH_RWTS) ? " RWTS" : "",
					 (dmasr & ETH_RBUS) ? " RBUS" : "",
					 (dmasr & ETH_TUS) ? " TUS" : "",
					 (dmasr & ETH_ROS) ? " ROS" : "",
					 (dmasr & ETH_TJTS) ? " TJTS" : "",
					 (dmasr & ETH_TPSS) ? " TPSS" : "");
		}
	}

	/* clear interrupt bits */
	eth->dmasr = dmasr;
}

const struct ifnet_operations stm32f_ethif_op = {
	.op_type = IFT_ETHER,
	.op_addrlen = ETH_ADDR_LEN,
	.op_init = stm32f_ethif_init,
	.op_cleanup = stm32f_ethif_cleanup, 
	.op_mmap = stm32f_ethif_mmap, 
	.op_send = stm32f_ethif_send,
	.op_arplookup = etharp_lookup, 
	.op_getaddr = stm32f_ethif_getaddr,
	.op_getdesc = stm32f_ethif_getdesc, 
	.op_sleep = NULL,
	.op_wakeup = NULL,
	.op_munmap = stm32f_ethif_munmap
};

struct ifnet * ethif_init(const uint8_t ethaddr[], in_addr_t ip_addr, 
						  in_addr_t netmask)
{
	struct stm32f_eth * eth = STM32F_ETH;
	struct ifnet * ifn;

	stm32f_eth_init(eth);
	stm32f_eth_mac_set(eth, 0, ethaddr);

	DCC_LOG2(LOG_TRACE, "ifn_register(%I, %I)", ip_addr, netmask);

	ifn = ifn_register((void *)&stm32f_eth_drv, &stm32f_ethif_op, 
					   eth, STM32F_IRQ_ETH);

	if (ifn != NULL) {
		ifn_ipv4_set(ifn, ip_addr, netmask);
	}

	DCC_LOG(LOG_TRACE, "done.");

	return ifn;
}

#endif

#endif /* STM32FX2 */

