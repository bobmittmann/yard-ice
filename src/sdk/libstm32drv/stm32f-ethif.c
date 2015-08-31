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

#include <sys/stm32f.h>

#ifdef STM32F_ETH

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stm32f_ethif.h>
#include <sys/etharp.h>
#include <sys/param.h>

#include <tcpip/in.h>

#define __USE_SYS_IFNET__
#include "sys/ifnet.h"
#define __USE_SYS_ROUTE__
#include "sys/route.h"
#include "sys/net.h"
#include "sys/ip.h"
#include "sys/pktbuf.h"

#include <trace.h>

#undef TRACE_LEVEL
#define TRACE_LEVEL TRACE_LVL_NONE 

void stm32f_eth_isr(void);

void stm32f_eth_mii_wr(struct stm32f_eth * eth, unsigned int addr, 
					   unsigned int reg, uint16_t data)
{
	DCC_LOG1(LOG_MSG, "reg: %d", reg);

	while (eth->macmiiar & ETH_MII_MB) {
	}

	eth->macmiidr = data;
	eth->macmiiar = ETH_PA_SET(addr) | ETH_MR_SET(reg) | 
		ETH_CR_HCLK_16 | ETH_MII_MW | ETH_MII_MB;
}

void stm32f_eth_mii_rd(struct stm32f_eth * eth, unsigned int addr, 
					   unsigned int reg, uint16_t * data)
{
	DCC_LOG1(LOG_MSG, "reg: %d", reg);

	while (eth->macmiiar & ETH_MII_MB) {
	}

	eth->macmiiar = ETH_PA_SET(addr) | ETH_MR_SET(reg) | 
		ETH_CR_HCLK_16 | ETH_MII_MB;

	while (eth->macmiiar & ETH_MII_MB) {
	}

	*data = eth->macmiidr;
}


#if THINKOS_ENABLE_FLAG_ALLOC

void * stm32f_ethif_mmap(struct ifnet * __if, size_t __len)
{
	uint8_t * pktbuf;

	if (__len > __if->if_mtu) {
		NETIF_STAT_ADD(__if, tx_drop, 1);
		DCC_LOG(LOG_ERROR, "too big!");
		return NULL;
	}

	if ((pktbuf = pktbuf_alloc()) == NULL) {
		DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
		return NULL;
	}

	DCC_LOG1(LOG_INFO, "pktbuf=%p ++", pktbuf);

	return pktbuf + 14;
}

int stm32f_ethif_send(struct ifnet * __if, const uint8_t * __dst, 
					  int __proto, const void * __buf, int __len)
{
	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)__if->if_drv;
	struct stm32f_eth * eth = (struct stm32f_eth *)__if->if_io;
	struct txdma_enh_desc * txdesc;
	struct eth_hdr * hdr;
	uint32_t head;

	DCC_LOG2(LOG_INFO, "mem=%p len=%d", __buf, __len);

	/* wait for buffer availability */
	thinkos_sem_wait(drv->tx.sem);

	head = drv->tx.head;
	if ((head - drv->tx.tail) == STM32F_ETH_TX_NDESC) {
		DCC_LOG(LOG_PANIC, "queue full!..");
		abort();
	}

	txdesc = &drv->tx.desc[head % STM32F_ETH_TX_NDESC];
#ifdef DEBUG
	if (txdesc->es) {
		struct txdma_st st;
		st = txdesc->st;
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
#endif
	if (txdesc->own != 0) {
		DCC_LOG(LOG_PANIC, "DMA own flag is set!");
		abort();
	}

	hdr = (struct eth_hdr *)((uintptr_t)__buf - 14);
	DCC_LOG1(LOG_INFO, "pktbuf=%p", hdr);

	hdr->eth_type = __proto;

	/* ethernet source address */
	stm32f_eth_mac_get(eth, 0, (uint8_t *)hdr->eth_src);

	/* ethernet destination address */
	hdr->eth_dst[0] = __dst[0];
	hdr->eth_dst[1] = __dst[1];
	hdr->eth_dst[2] = __dst[2];
	hdr->eth_dst[3] = __dst[3];
	hdr->eth_dst[4] = __dst[4];
	hdr->eth_dst[5] = __dst[5];

#if 0
	DCC_LOG7(LOG_INFO, "from: %02x:%02x:%02x:%02x:%02x:%02x (prot=%d)",
			 hdr->eth_src[0], hdr->eth_src[1], 
			 hdr->eth_src[2], hdr->eth_src[3], 
			 hdr->eth_src[4], hdr->eth_src[5], hdr->eth_type); 

	DCC_LOG7(LOG_INFO, "to: %02x:%02x:%02x:%02x:%02x:%02x (len=%d)",
			 hdr->eth_dst[0], hdr->eth_dst[1], 
			 hdr->eth_dst[2], hdr->eth_dst[3], 
			 hdr->eth_dst[4], hdr->eth_dst[5], __len); 
#endif		 

	/* Transmit buffer 1 size */
	txdesc->tbs1 = 14 + __len;
	/* Transmit buffer 1 addr */
	txdesc->tbap1 = (void *)hdr;
	/* set the DMA descriptor ownership */
	txdesc->own = 1;
	/* update queue head */
	drv->tx.head = head + 1;

	DCC_LOG(LOG_INFO, " DMA start transmit...");
//	eth->dmaomr |= ETH_ST;

	/* Transmit Poll Demand command */
	eth->dmatpdr = 1;

	return 0;
}

int stm32f_ethif_pkt_free(struct ifnet * __if, uint8_t * __pkt)
{
	uint8_t * pktbuf = (uint8_t *)((uintptr_t)__pkt - 14);
	
	DCC_LOG1(LOG_INFO, "pktbuf=%p --", pktbuf);
	pktbuf_free(pktbuf);
	return 0;
}

int stm32f_ethif_munmap(struct ifnet * __if, void * __mem)
{
	uint8_t * pktbuf = (uint8_t *)((uintptr_t)__mem - 14);
	
	DCC_LOG1(LOG_INFO, "pktbuf=%p --", pktbuf);
	pktbuf_free(pktbuf);
	return 0;
}

int stm32f_ethif_pkt_recv(struct ifnet * __if, uint8_t ** __src, 
						  unsigned int * __proto, uint8_t ** __pkt)
{
	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)__if->if_drv;
	struct stm32f_eth * eth = (struct stm32f_eth *)__if->if_io;
	struct rxdma_ext_st ext_st;
	struct rxdma_st st;
	struct rxdma_enh_desc * rxdesc;
	struct eth_hdr * hdr;
	int len;
	
	rxdesc = &drv->rx.desc[drv->rx.tail & (STM32F_ETH_RX_NDESC - 1)];
	st = rxdesc->st;
	if (st.es) {
		DCC_LOG(LOG_ERROR, "error summary flag set!");
	}
	if (st.own) {
		DCC_LOG(LOG_INFO, "DMA own flag set!");
		return 0;
	}
	drv->rx.tail++; 

	len = st.fl;
	DCC_LOG1(LOG_INFO, "frame len=%d", len);
	DBG("STM32ETH: RX frame len=%d", len);

	ext_st = rxdesc->ext_st;

	/* IP payload type */
	switch (ext_st.ippt) {
	case ETH_IPPT_UNKOWN:
		DCC_LOG(LOG_INFO, "not IP!");
		DBG("STM32ETH: not IP!");
		break;
	case ETH_IPPT_UDP:
		DBG("STM32ETH: UDP");
		break;
	case ETH_IPPT_TCP:
		DBG("STM32ETH: TCP");
		break;
	case ETH_IPPT_ICMP:
		DBG("STM32ETH: ICMP");
		break;
	}

	if (ext_st.iphe) {
		DCC_LOG(LOG_WARNING, "IP header error!");
		DBG("IP header error!");
		goto error;
	}
	if (ext_st.ippe) {
		DCC_LOG(LOG_WARNING, "IP payload error!");
		DBG("IP payload error!");
		goto error;
	}
	if (ext_st.ipcb)
		DCC_LOG(LOG_INFO, "IP checksum bypass.");
	if (ext_st.ipv4pr)
		DCC_LOG(LOG_INFO, "IPv4 packet received.");
	if (ext_st.ipv6pr)
		DCC_LOG(LOG_INFO, "IPv6 packet received.");

	/* get the current buffer */
	hdr = (struct eth_hdr *)rxdesc->rbap1;
	DCC_LOG1(LOG_INFO, "pktbuf=%p", hdr);

	/* alloc a new buffer */
	if ((rxdesc->rbap1 = pktbuf_alloc()) == NULL) {
		DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
		abort();
	}
	DCC_LOG1(LOG_INFO, "pktbuf=%p ++", rxdesc->rbap1);

	/* set the DMA descriptor ownership */
	rxdesc->rdes0 = ETH_RXDMA_OWN;
	/* enable DMA receive interrupts */
	eth->dmaier |= ETH_RIE;
	/* Receive Poll Demand command */
	eth->dmarpdr = 1;

	*__proto = hdr->eth_type;
	*__pkt = (uint8_t *)hdr + 14;
	*__src = hdr->eth_src; 
	return len - 14;

error:
	/* recycle the buffer */
	/* set the DMA descriptor ownership */
	rxdesc->rdes0 = ETH_RXDMA_OWN;
	/* enable DMA receive interrupts */
	eth->dmaier |= ETH_RIE;
	/* Receive Poll Demand command */
	eth->dmarpdr = 1;

	return -1;
}

int stm32f_ethif_getaddr(struct ifnet * __if, uint8_t * __buf)
{
	struct stm32f_eth * eth = (struct stm32f_eth *)__if->if_io;

	stm32f_eth_mac_get(eth, 0, __buf);
	return 0;
}

const char stm32f_ethif_desc[] = "STM32F-Ethernet";

int stm32f_ethif_getdesc(struct ifnet * __if, char * __s, int __len)
{
	int n = MIN(__len, sizeof(stm32f_ethif_desc));
	memcpy(__s, stm32f_ethif_desc, n);
	__s[n - 1] = '\0';
	return n;
}


int stm32f_ethif_init(struct ifnet * __if)
{
	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)__if->if_drv;
	struct stm32f_eth * eth = (struct stm32f_eth *)__if->if_io;
	struct rxdma_enh_desc * rxdesc;
	struct txdma_enh_desc * txdesc;
	unsigned int rx_buf_size;
	unsigned int tx_buf_size;
//	struct eth_hdr * hdr;
	unsigned int mtu;
	int i;

	DCC_LOG2(LOG_INFO, "if=0x%p drv=0x%p", __if, drv);
	drv->ifn = __if;
	drv->eth = eth;

//	mtu = STM32F_ETH_PAYLOAD_MAX;
	mtu = pktbuf_len - 16;
	rx_buf_size = pktbuf_len;
	tx_buf_size = pktbuf_len;
	(void)tx_buf_size;

	__if->if_mtu = mtu;
	__if->if_link_speed = 100000000;
	/* set the broadcast flag */
	__if->if_flags |= IFF_BROADCAST;

	drv->event = __if->if_idx;

	DCC_LOG1(LOG_INFO, "mtu=%d", mtu);
	INF("STM32ETH: MTU=%d", mtu);

	DCC_LOG(LOG_INFO, "MAC configuration ...");
	/* Bit 25 - CRC stripping for Type frames */
	/* Bit 14 - Fast Ethernet speed */
	/* Bit 13 - Receive own disable */
	/* Bit 11 - Duplex mode */
	/* Bit 10 - IPv4 checksum offload */
	/* Bit 3 - Transmitter enable */
	/* Bit 2 - Receiver enable */
	eth->maccr = ETH_CSTF | ETH_FES | ETH_ROD | ETH_DM | ETH_IPCO | 
		ETH_TE | ETH_RE;

	DCC_LOG(LOG_INFO, "DMA operation mode ...");
	/* Bit 21 - Transmit store and forward */
	/* Bit 25 - Receive store and forward */
	/* Bit 20 - Flush transmit FIFO */
	eth->dmaomr = ETH_RSF | ETH_TSF | ETH_FTF;

	DCC_LOG(LOG_INFO, "DMA bus mode ...");
	/* Bit 7 - Enhanced descriptor format enable */
	eth->dmabmr = ETH_EDFE;

	DCC_LOG(LOG_INFO, "DMA RX descriptors ...");
	for (i = 0; i < STM32F_ETH_RX_NDESC; ++i) {
		/* configure recevie descriptors */
		rxdesc = &drv->rx.desc[i];
		rxdesc->rdes0 = ETH_RXDMA_OWN;
		/* Receive buffer 1 size */
		rxdesc->rbs1 = rx_buf_size;
		/* Receive end of ring */
		rxdesc->rer = 0;
		/* Disable interrupt on completion */
		rxdesc->dic = 0;
		/* Second address chained */
		rxdesc->rch = 1;
//		rxdesc->rbap1 = drv->rx.buf[i];
		if ((rxdesc->rbap1 = pktbuf_alloc()) == NULL) {
			DCC_LOG(LOG_ERROR, "pktbuf_alloc() failed!");
			abort();
		}
		DCC_LOG1(LOG_INFO, "pktbuf=%p ++", rxdesc->rbap1);
		/* link to the next descriptor */
		rxdesc->rbap2 = (void *)&drv->rx.desc[i + 1];
	}
	/* link to the first */
	rxdesc->rbap2 = (void *)&drv->rx.desc[0];

	/* DMA receive descriptor list address */
	eth->dmardlar = (uint32_t)&drv->rx.desc[0];
	drv->rx.head = 0;
	drv->rx.tail = 0;

	DCC_LOG(LOG_INFO, "DMA TX descriptors ...");
	for (i = 0; i < STM32F_ETH_TX_NDESC; ++i) {
		/* configure transmit descriptors */
		txdesc = &drv->tx.desc[i];
		/* Transmit buffer 1 size */
		txdesc->tbs1 = 0;
		/* Transmit buffer 2 size */
		txdesc->tbs2 = 0;
//		txdesc->tdes0 = ETH_TXDMA_IC | ETH_TXDMA_LS | ETH_TXDMA_FS;
		txdesc->ic = 1;
		txdesc->ls = 1;
		txdesc->fs = 1;
		/* Second address chained */
		txdesc->tch = 1;
		/* Transmit end of ring */
		txdesc->ter = 0;
		/* Transmit buffer 1 addr */
		txdesc->tbap1 = NULL;
		/* Transmit buffer 2 addr */
		/* link to the next descriptor */
		txdesc->tbap2 = (void *)&drv->tx.desc[i + 1];
	}
	/* link the last to the first */
	txdesc->tbap2 = (void *)&drv->tx.desc[0];

	/* DMA transmit descriptor list address */
	eth->dmatdlar = (uint32_t)&drv->tx.desc[0];
	drv->tx.head = 0;
	drv->tx.tail = 0;

	drv->tx.sem = thinkos_sem_alloc(STM32F_ETH_TX_NDESC); 
	DCC_LOG1(LOG_INFO, "tx.sem=%d", drv->tx.sem);

#ifdef THINKAPP
	/* configure and Enable interrupt */
	thinkos_irq_register(STM32F_IRQ_ETH, IRQ_PRIORITY_HIGH, 
						 stm32f_eth_isr);
#else
	/* configure interrupts */
	/* set the interrupt priority */
	cm3_irq_pri_set(STM32F_IRQ_ETH, IRQ_PRIORITY_HIGH);
	/* enable interrupts */
	cm3_irq_enable(STM32F_IRQ_ETH);
#endif


	/* enable Normal interrupt summary and Abnormal interrupt summary */
	eth->dmaier = ETH_NISE | ETH_AISE | ETH_FBEIE | 
		ETH_RWTIE | ETH_RBUIE | ETH_ROIE |
		ETH_TUIE | ETH_TJTIE | ETH_TIE | ETH_TBUIE;


	DCC_LOG3(LOG_INFO, "RX DMA: %d (desc) x %d = %d bytes", 
			 STM32F_ETH_RX_NDESC, rx_buf_size +
			 sizeof(struct rxdma_enh_desc),
			 STM32F_ETH_RX_NDESC * (rx_buf_size +
			 sizeof(struct rxdma_enh_desc))); 
	DCC_LOG3(LOG_INFO, "TX DMA: %d (desc) x %d = %d bytes", 
			 STM32F_ETH_TX_NDESC, tx_buf_size  +
			 sizeof(struct txdma_enh_desc) + sizeof(struct eth_hdr),
			 STM32F_ETH_TX_NDESC * (tx_buf_size +
			 sizeof(struct txdma_enh_desc) + sizeof(struct eth_hdr)));
	DCC_LOG1(LOG_INFO, "Ethernet driver memory: %d bytes", 
			 sizeof(struct stm32f_eth_drv));
	DCC_LOG1(LOG_INFO, "Ethernet MTU : %d bytes", mtu);

	DCC_LOG1(LOG_INFO, "<%d> DMA interrupts enabled ...", 
			 thinkos_thread_self());

	/* enable DMA RX interrupts */
	eth->dmaier |= ETH_RIE | ETH_NISE;

	DCC_LOG(LOG_INFO, "DMA start receive...");
	eth->dmaomr |= ETH_SR;

	DCC_LOG(LOG_INFO, "DMA start transmit...");
	eth->dmaomr |= ETH_ST;

	return 0;
}

int stm32f_ethif_cleanup(struct ifnet * __if)
{
	return 0;
}

const struct ifnet_operations stm32f_ethif_op = {
	.op_type = IFT_ETHER,
	.op_addrlen = ETH_ADDR_LEN,
	.op_init = stm32f_ethif_init,
	.op_cleanup = stm32f_ethif_cleanup, 
	.op_mmap = stm32f_ethif_mmap, 
	.op_send = stm32f_ethif_send,
	.op_pkt_recv = stm32f_ethif_pkt_recv,
	.op_pkt_free = stm32f_ethif_pkt_free,
	.op_arplookup = etharp_lookup, 
	.op_arpquery = etharp_query, 
	.op_getaddr = stm32f_ethif_getaddr,
	.op_getdesc = stm32f_ethif_getdesc, 
	.op_sleep = NULL,
	.op_wakeup = NULL,
	.op_munmap = stm32f_ethif_munmap
};

struct stm32f_eth_drv stm32f_eth_drv;

#if 0
int stm32f_ethif_rx_dma_stat(void)
{
	struct stm32f_eth_drv * drv = &stm32f_eth_drv;
	uint32_t tail = drv->rx.tail;
	struct rxdma_enh_desc * rxdesc;
	int cnt;

	for (cnt = 0; ; ++cnt) {
		rxdesc = &drv->rx.desc[tail & (STM32F_ETH_RX_NDESC - 1)];
		if (rxdesc->st.own)
			break;
		tail++; 
	}

	DCC_LOG1(LOG_TRACE, "%d pending DMA descriptors!", cnt);

	__thinkos_ev_info(__ifnet__.evset);
	__tdump();

	return cnt;
}
#endif

void stm32f_eth_isr(void)
{
	struct stm32f_eth_drv * drv = &stm32f_eth_drv;
	struct stm32f_eth * eth = STM32F_ETH;
	uint32_t dmasr;

//	DCC_LOG1(LOG_INFO, "if=0x%p", ifn);

	dmasr = eth->dmasr;
//	show_dma_status(dmasr);

	DCC_LOG1(LOG_INFO, "DMASR=0x%08x", dmasr);

	if (dmasr & ETH_RS) {
		DCC_LOG(LOG_INFO, "DMA RS");
		/* disable DMA receive interrupts */
		eth->dmaier &= ~ETH_RIE;
		ifn_signal_i(drv->event);
	}

	if (dmasr & ETH_TS) {
		DCC_LOG(LOG_INFO, "TS");
		struct txdma_enh_desc * txdesc;
		void * pktbuf;
		uint32_t tail;

	//	eth->dmaomr &= ~ETH_ST;
		tail = drv->tx.tail;
		while (tail != drv->tx.head) {
			txdesc = &drv->tx.desc[tail % STM32F_ETH_TX_NDESC];
			if (txdesc->own) {
				DCC_LOG(LOG_INFO, "DMA own flag set!");
				break;
			}
			pktbuf = txdesc->tbap1;
			DCC_LOG1(LOG_INFO, "pktbuf=%p --", pktbuf);
			pktbuf_free(pktbuf);
			tail++;
			thinkos_sem_post_i(drv->tx.sem);
		}
		drv->tx.tail = tail;
//		thinkos_flag_give_i(drv->tx.flag);
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

#define PHY_RESET (1 << 15)
#define PHY_AUTO (1 << 12)

#define PHY_LINK_UP (1 << 2)
#define PHY_AUTO_DONE (1 << 5)

struct ifnet * ethif_init(const uint8_t ethaddr[], in_addr_t ip_addr, 
						  in_addr_t netmask)
{
	struct stm32f_eth * eth = STM32F_ETH;
	struct ifnet * ifn;

	stm32f_eth_init(eth);
	stm32f_eth_mac_set(eth, 0, ethaddr);

	unsigned int addr;
	uint16_t data;
	uint32_t phy_id;
	DCC_LOG(LOG_TRACE, "probing PHY...");
	for (addr = 0; addr < 32; ++addr) {
		stm32f_eth_mii_rd(eth, addr, 0, &data);
		DCC_LOG2(LOG_INFO, "addr=%d r0=%04x", addr, data);
		if (data != 0xff)
			break;
	}

	if (addr < 32) {
		stm32f_eth_mii_rd(eth, addr, 2, &data);
		phy_id = data;
		stm32f_eth_mii_rd(eth, addr, 3, &data);
		phy_id |= data << 16;
		DCC_LOG2(LOG_TRACE, "PHY addr=%d id=%08x", addr, phy_id);
		(void)phy_id;	
		DCC_LOG(LOG_TRACE, "PHY reset...");
		stm32f_eth_mii_wr(eth, addr, 0, PHY_RESET);
/*
		stm32f_eth_mii_wr(eth, addr, 0, PHY_AUTO);
		do {
			stm32f_eth_mii_rd(eth, addr, 1, &data);
		} while (!(data & PHY_AUTO_DONE)); 

		DCC_LOG1(LOG_TRACE, "Link is %s.", 
				 data & PHY_LINK_UP ? "up" : " down");
*/
	}

	DCC_LOG2(LOG_INFO, "ifn_register(%I, %I)", ip_addr, netmask);

	ifn = ifn_register((void *)&stm32f_eth_drv, &stm32f_ethif_op, 
					   eth, STM32F_IRQ_ETH);

	if (ifn != NULL) {
		ifn_ipv4_set(ifn, ip_addr, netmask);
	}

	DCC_LOG(LOG_INFO, "done.");

	return ifn;
}

#endif

#endif /* STM32F_ETH */


