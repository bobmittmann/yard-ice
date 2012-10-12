/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 *
 * This file is part of the libtcpip.
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
 * @file stm32f-eth.c
 * @brief STM32F Ethernet driver
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/mii.h>

#include <tcpip/stm32f_ethif.h>

#include <tcpip/etharp.h>
#include <tcpip/ip.h>

#define __USE_SYS_IFNET__
#include "sys/ifnet.h"
#include "sys/net.h"

struct rx_dma_desc desc;

void show_rx_desc_status(struct rx_dma_desc * desc)
{
	DCC_LOG1(LOG_TRACE, "status=0x%04x", desc->status);
	DCC_LOG8(LOG_TRACE, "%s%s%s%s%s%s%s%s",
			 desc->status & RXDMA_DSE  ?  " DSE" : "",
			 desc->status & RXDMA_OE   ? " OE" : "",
			 desc->status & RXDMA_VLAN ? " VLAN" : "",
			 desc->status & RXDMA_FS   ? " FS" : "",
			 desc->status & RXDMA_LS   ? " LS" : "",
			 desc->status & RXDMA_LCO  ? " LCO" : "",
			 desc->status & RXDMA_FT   ? " FT" : "",
			 desc->status & RXDMA_RWT  ? " RWT" : "");

	DCC_LOG1(LOG_TRACE, "frame len=%d", desc->fl);

	if (desc->own) {
		DCC_LOG(LOG_WARNING, "OWN bit set!!!");
	};
}

void show_dma_status(uint32_t dmasr)
{
	DCC_LOG1(LOG_TRACE, "DMA status: 0x%08x", dmasr);
	DCC_LOG7(LOG_TRACE, "%s%s%s%s%s%s%s%s",
			 dmasr & ETH_FBES ? " FBES" : "",
			 dmasr & ETH_RWTS ? " RWTS" : "",
			 dmasr & ETH_RBUS ? " RBUS" : "",
			 dmasr & ETH_RS   ? " RS" : "",
			 dmasr & ETH_RPSS ? " RPSS" : "",
			 dmasr & ETH_ROS  ? " ROS" : "",
			 dmasr & ETH_ROS  ? " ROS" : "");
}

int eth_recv(void * buf, unsigned int len)
{
	struct stm32f_eth * eth = STM32F_ETH;
	uint32_t dmasr;
	uint32_t rps;

	dmasr = eth->dmasr;
/*	show_dma_status(dmasr); */

	if (dmasr & ETH_RS) {
		/* clear RS bit */
		eth->dmasr = ETH_RS;
	}

	if (dmasr & ETH_RBUS) {
		/* clear RBUS bit */
		eth->dmasr = ETH_RBUS;
	}

	desc.rbs1 = len;
	desc.rer = 1;
	desc.rbap1 = (uint32_t *)buf;
	desc.status = 0;
	desc.fl = 0;
	desc.own = 1;

	/* DMA receive descriptor list address */
	/* This field contains the base address of the first descriptor in the 
	   receive descriptor list. The LSB bits [1/2/3:0] for 32/64/128-bit bus
	   width) are internally ignored and taken as all-zero by the DMA. Hence 
	   these LSB bits are read only. */
	eth->dmardlar = (uint32_t)&desc;

	rps = ETH_RPS_GET(dmasr);
	if (rps == 0) {
		/* DMA stopped */
		DCC_LOG(LOG_TRACE, " - DMA stopped\n");
		eth->dmaomr |= ETH_SR;
	} else if (rps == 4) {
		DCC_LOG(LOG_TRACE, " - DMA suspended\n");
		eth->dmarpdr = 1;
	/* DMA receive poll demand */
	/* When these bits are written with any value, the DMA reads the current 
	   descriptor pointed to by the ETH_DMACHRDR register. If that descriptor 
	   is not available (owned by Host), reception returns to the Suspended 
	   state and ETH_DMASR register bit 7 is not asserted. If the descriptor 
	   is available, the Receive DMA returns to active state. */
	}

	while (((dmasr = eth->dmasr) & ETH_RS) == 0) {
	}

	return desc.fl;
}

#if 0
#define ETH_P_IP	0x0800 /* Internet Protocol packet	*/
#define ETH_P_ARP	0x0806 /* Address Resolution packet	*/

void eth_input(uint8_t * frame, unsigned int len)
{
	unsigned int type;
	/* skip destination and source addresses */
	type = (frame[12] << 8) + frame[13];
	DCC_LOG2(LOG_TRACE, "type=%04x len=%d\n", type, len);
}
#endif

#if 0
int stm32f_eth_isr(struct ifnet * ifn)
{
	struct stm32f_eth * emac = AT91_EMAC;
	struct stm32f_eth_drv * drv;
	int irq_no = ifn->if_irq_no;
	uint32_t isr;

	drv = (struct stm32f_eth_drv *)ifn->if_drv;

	for (;;) {
		uthread_int_wait(irq_no);

		/* get interrupt status */
		isr = emac->isr;

		DCC_LOG1(LOG_MSG, "ISR = 0x%04x", isr);
			
		if (isr & EMAC_MFD) {
			DCC_LOG(LOG_MSG, "mgm frm done");
			uthread_sem_post(drv->phy_sem);
		}
		
		if (isr & EMAC_TCOMP) {
			DCC_LOG(LOG_INFO, "tx completed");
			uthread_sem_post(drv->tx_sem);
		}

		if (isr & EMAC_RCOMP) {
			DCC_LOG(LOG_MSG, "rx completed");
			uthread_sem_post(drv->rx_sem);
		}

#ifdef DEBUG
		if (isr & EMAC_RXUBR) {
			DCC_LOG(LOG_INFO, "rx used bit read");
		}
				
		if (isr & EMAC_TXUBR) {
			DCC_LOG(LOG_INFO, "tx used bit read");
		}
					
		if (isr & EMAC_LINK) {
			DCC_LOG(LOG_INFO, "link");
		}

		if (isr & EMAC_HRESP) {
			DCC_LOG(LOG_INFO, "hresp not Ok");
		}
				
		if (isr & EMAC_PFRE) {
			DCC_LOG(LOG_INFO, "pause frame rcvd");
		}
					
		if (isr & EMAC_PTZ) {
			DCC_LOG(LOG_INFO, "pause time zero");
		}

		if (isr & EMAC_TUNDR) {
			DCC_LOG(LOG_INFO, "tx buffer underrun");
		}

		if (isr & EMAC_TXERR) {
			DCC_LOG(LOG_INFO, "tx error");
		}

		if (isr & EMAC_RLEX) {
			DCC_LOG(LOG_INFO, "retry limit exceed");
		}

		if (isr & EMAC_ROVR) {
			DCC_LOG(LOG_INFO, "rx overrun");
		}
#endif
	}
}

int __attribute__((noreturn)) stm32f_eth_input(struct ifnet * ifn, 
											  uthread_id_t thread)
{
	struct stm32f_eth_drv * drv;
	struct emac_buf_desc * desc;
	uint8_t * pkt;
	uint8_t * src;
	uint8_t * dst;
	int len;
	int type;
	int n;

	drv = (struct stm32f_eth_drv *)ifn->if_drv;
	/* skip the frame len */
	pkt = (uint8_t *)drv->rx.pkt;

	for (;;) {
		uthread_sem_wait(drv->rx_sem);

		DCC_LOG(LOG_INFO, "rx signal");

		len = 0;
		type = 0;
		dst = (uint8_t *)pkt;

		for (;;) {
			desc = &drv->rx.desc[drv->rx.dma % AT91_EMAC_RX_BUF_COUNT];

			if ((desc->ptr & EMAC_RX_OWNER) == 0)
				break;

			/* move the head pointer */
			drv->rx.dma++;
			//			if (drv->rx.head == AT91_EMAC_RX_BUF_COUNT) {
			//				drv->rx.head = 0;
			//			}

			if (desc->opt & EMAC_RX_SOF) {
				src = (uint8_t *)(desc->ptr & 0xfffffffc);
				/* skip destination and source addresses */
				src += 12;
				type = ((uint16_t *)src)[0];
				src += 2;
				dst = (uint8_t *)pkt;
				len = 0;
				/* end of frame */
				if (desc->opt & EMAC_RX_EOF) {
					DCC_LOG(LOG_INFO, "SOF EOF");
					n = EMAC_RX_LEN(desc->opt) - 14;
				} else {
					DCC_LOG(LOG_INFO, "SOF ---");
					n = AT91_EMAC_RX_BUF_SIZE - 14;
				}
			} else {
				if (len == 0) {
					DCC_LOG(LOG_PANIC, "missing SOF ???");
				}
				src = (uint8_t *)(desc->ptr & 0xfffffffc);
				if (desc->opt & EMAC_RX_EOF) {
					DCC_LOG(LOG_INFO, "--- EOF");
					n = (EMAC_RX_LEN(desc->opt) - 14) - len;
				} else {
					DCC_LOG(LOG_INFO, "--- --");
					n = AT91_EMAC_RX_BUF_SIZE;
				}
			}

			len += n;
			if (len <= AT91_EMAC_MAX_PKT_SIZE) {
				memcpy(dst, src, n);
				dst += n;
			}

			if (!(desc->opt & EMAC_RX_EOF)) {
				desc->ptr &= ~EMAC_RX_OWNER;
				continue;
			}

			desc->ptr &= ~EMAC_RX_OWNER;
			if (len > AT91_EMAC_MAX_PKT_SIZE) {
				DCC_LOG1(LOG_ERROR, "too big: %d", len);
				NETIF_STAT_ADD(ifn, rx_drop, 1);
				continue;
			}

			DCC_LOG1(LOG_INFO, "frame received: %d bytes", len);

			uthread_mutex_lock(net_mutex);

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

			uthread_mutex_unlock(net_mutex);
		}
	}
}

int stm32f_eth_if_send(struct ifnet * __if, const uint8_t * __dst, 
					  int __proto, const void * __buf, int __len)
{
	struct stm32f_eth * emac = AT91_EMAC;
	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)__if->if_drv;

	if (__len > __if->if_mtu) {
		NETIF_STAT_ADD(__if, tx_drop, 1);
		DCC_LOG(LOG_WARNING, "too big!");
		return -1;
	}

	/* wait for buffer availability */
	uthread_sem_wait(drv->tx_sem);

	DCC_LOG7(LOG_INFO, "to: %02x:%02x:%02x:%02x:%02x:%02x (%d)",
		__dst[0], __dst[1], __dst[2], __dst[3], __dst[4], __dst[5], __len);

//	DBG_PUTC(DBG_TRACE, 'T');

/* To set up a frame for transmission: */
/* 1. Enable transmit in the network control register. */

/* 2. Allocate an area of system memory for transmit data. This does 
 * not have to be contiguous, varying byte lengths can be used as 
 * long as they conclude on byte borders. */

/* 3. Set-up the transmit buffer list. */

/*	4. Set the network control register to enable transmission and 
 *	enable interrupts. */

/*  5. Write data for transmission into these buffers. */
	drv->tx.hdr.eth_type = __proto;

#if 0
	/* source address */
	stm32f_eth_getaddr(__if, (uint8_t *)drv->tx.frame.src);
#endif

	/* ethernet destination address */
	drv->tx.hdr.eth_dst[0] = __dst[0];
	drv->tx.hdr.eth_dst[1] = __dst[1];
	drv->tx.hdr.eth_dst[2] = __dst[2];
	drv->tx.hdr.eth_dst[3] = __dst[3];
	drv->tx.hdr.eth_dst[4] = __dst[4];
	drv->tx.hdr.eth_dst[5] = __dst[5];

/*  6. Write the address to transmit buffer descriptor queue pointer. */

/*  7. Write control and length to word one of the transmit 
 *  buffer descriptor entry. */

	drv->tx.desc[0].ptr = (uint32_t)&drv->tx.hdr;
	drv->tx.desc[0].opt = 14;

	drv->tx.desc[1].ptr = (uint32_t)__buf;
	drv->tx.desc[1].opt = EMAC_TX_WRAP | EMAC_TX_LAST | __len;

/* 8. Write to the transmit start bit in the network control register. */

    emac->ncr |= EMAC_TSTART;

	NETIF_STAT_ADD(__if, tx_pkt, 1);

	return __len;
} 

int stm32f_eth_if_getaddr(struct ifnet * __if, uint8_t * __buf)
{
	struct stm32f_eth * emac = AT91_EMAC;
	uint32_t lo;
	uint32_t hi;

	lo = emac->sa1b;
	hi = emac->sa1t;
	
	__buf[0] = lo & 0xff;
	__buf[1] = (lo >> 8) & 0xff;
	__buf[2] = (lo >> 16) & 0xff;
	__buf[3] = (lo >> 24) & 0xff;
	__buf[4] = hi & 0xff;
	__buf[5] = (hi >> 8) & 0xff;

	return 6;
}

int stm32f_eth_if_getinfo(struct ifnet * __if, char * __s, int __len)
{
#if 0
	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)__if->if_drv;
	int addr = 0;
	int n;

	n = sprintf(__s, "AT91 emac ");

	if ((emac_phy_rd(drv->mgm_sem, addr, MII_BMSR) & BMSR_LSTATUS) == 0) {
		n += sprintf(&__s[n], "NO LINK");
	} else {
		if (emac_phy_rd(drv->mgm_sem, addr, MII_BMCR) & BMCR_SPEED100) {
			n += sprintf(&__s[n], "100 Mbps");
		} else {
			n += sprintf(&__s[n], "10 Mbps");
		}
	}

	return n;
#endif
	return 0;
}

int stm32f_eth_if_init(struct ifnet * __if)
{
	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)__if->if_drv;
	struct at91_pmc * pmc = AT91_PMC;	
	struct stm32f_eth * emac = AT91_EMAC;
	int mtu;
	int i;

	DCC_LOG2(LOG_TRACE, "if=0x%p drv=0x%p", __if, drv);

	/* Initialize the hardware */

    /* disable interrupts */
	emac->idr = EMAC_MFD | EMAC_RCOMP | EMAC_RXUBR | EMAC_TXUBR | \
				EMAC_TUNDR | EMAC_RLE | EMAC_TXERR | EMAC_TCOMP | \
				EMAC_LINK | EMAC_ROVR | EMAC_HRESP | EMAC_PFRE | EMAC_PTZ;
    /* Power ON */
    pmc->pcer = 1 << AT91_ID_EMAC;

    /* Disable transmit and receive circuits */
    emac->ncr = 0;


	/* Initialize the Receive Buffer List */
	for (i = 0; i < AT91_EMAC_RX_BUF_COUNT; i++) {
		drv->rx.desc[i].ptr = (uint32_t)drv->rx.buf[i];
		drv->rx.desc[i].opt = 0;
	}
	drv->rx.desc[AT91_EMAC_RX_BUF_COUNT - 1].ptr |= EMAC_RX_WRAP;
	drv->rx.dma = 0;

    /* Enable the copy of data into the buffers,
	 - Discard Receive FCS
	 - Pause enable
	 - MDC clock is MCLK/32
	 - Enable Full Duplex
	 - 100Mbps operation
	*/
    emac->ncfgr = EMAC_DRFCS | EMAC_PAE | EMAC_CLK_HCLK_32 | EMAC_FD |
		EMAC_SPD;

	/* Clear statistics registers. */
    emac->ncr = EMAC_CLRSTAT;

    /* Clear all status bits in the receive status register.*/
    emac->rsr = EMAC_OVR | EMAC_REC | EMAC_BNA;

    /* Clear all status bits in the transmit status register*/
    emac->tsr = EMAC_UBR | EMAC_COL | EMAC_RLES | EMAC_BEX | \
				EMAC_COMP | EMAC_UND;

	emac->rbqp = drv->rx.desc;

	/* Set the Transmit Buffer Queue Pointer */
	emac->tbqp = drv->tx.desc;

    emac->ncfgr = EMAC_DRFCS | EMAC_PAE | EMAC_CLK_HCLK_32 | \
				  EMAC_CAF | EMAC_SPD | EMAC_FD;

	/* select MII mode and enable clock */
	emac->usrio = EMAC_CLKEN;
    emac->ncr = EMAC_MPE;

    /* Enable Rx, Tx and Management Port */
    emac->ncr = EMAC_TE | EMAC_RE | EMAC_MPE;

	emac->ier = EMAC_MFD | EMAC_RCOMP | EMAC_TCOMP;

   	DCC_LOG(LOG_INFO, "EMAC initialized");

	/* setup the source address in the ethernet header 
	   of the  transmit buffer */
	stm32f_eth_if_getaddr(__if, (uint8_t *)drv->tx.hdr.eth_src);
	drv->tx.dma = 0;

	mtu = AT91_EMAC_MAX_PKT_SIZE;
	__if->if_mtu = mtu;
	__if->if_link_speed = 100000000;
	/* set the broadcast flag */
	__if->if_flags |= IFF_BROADCAST;

	DCC_LOG1(LOG_TRACE, "mtu=%d", mtu);

	/* alloc a semaphore initialized with 1, corresponding to the single
	 * output buffer */ 
	drv->tx_sem = uthread_sem_alloc(1);
	DCC_LOG1(LOG_TRACE, "tx_sem=%d", drv->tx_sem);

	/* alloc a semaphore for packet reception */ 
	drv->rx_sem = uthread_sem_alloc(0);
	DCC_LOG1(LOG_TRACE, "rx_sem=%d", drv->rx_sem);

	/* alloc a semaphore for phy flow control */ 
	drv->phy_sem = uthread_sem_alloc(0);
	DCC_LOG1(LOG_TRACE, "phy_sem=%d", drv->phy_sem);
	/* set the phy address */
	drv->phy_addr = 0;

	/* create the interrupt service routine task */
	drv->isr_thread = uthread_create(drv->isr_stack, 
									AT91_EMAC_ISR_STACK_SIZE, 
									(uthread_task_t)stm32f_eth_isr, 
									(void *)__if, -1, NULL); 

	DCC_LOG1(LOG_TRACE, "isr_thread=%d", drv->isr_thread);

	/* initialize the phy - this must be performed after 
	 the isr thread is running */
	if (stm32f_eth_phy_init(drv) < 0) {
		DCC_LOG(LOG_WARNING, "at91_phy_init() fail!");
		return -1;
	}

	drv->input_thread = uthread_create(drv->input_stack, 
									  AT91_EMAC_INPUT_STACK_SIZE, 
									  (uthread_task_t)stm32f_eth_input, 
									  (void *)__if, 0, NULL); 

	DCC_LOG1(LOG_TRACE, "input_thread=%d", drv->input_thread);

	return 0;
}

struct ifnet * ethif_init(in_addr_t ip_addr, in_addr_t netmask)
{
	struct ifnet * ifn;

	/* configure the interrupt */
	interrupt_config(AT91_ID_EMAC, INT_HIGH_LEVEL, 0);

	ifn = ifn_register((void *)&emac_drv, &stm32f_eth_ifnet_op, 
					   AT91_EMAC, AT91_ID_EMAC);

	if (ifn != NULL) {
		ifn_ipconfig(ifn, ip_addr, netmask);
	}

	return ifn;
}
#endif

int stm32f_eth_if_init(struct ifnet * __if)
{
	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)__if->if_drv;
	struct stm32f_eth * eth = (struct stm32f_eth *)__if->if_io;

	drv->ifn = __if;
	drv->eth = eth;

	DCC_LOG2(LOG_TRACE, "if=0x%p drv=0x%p", __if, drv);

	/* Initialize the hardware */
	return 0;
}


int stm32f_eth_if_cleanup(struct ifnet * __if)
{
	return 0;
}

void * stm32f_eth_if_mmap(struct ifnet * __if, size_t __length)
{
	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)__if->if_drv;

	/* FIXME: chek for availability  */
//	return drv->tx.buf[drv->tx.dma++ % AT91_EMAC_TX_BUF_COUNT];

	drv->tx.dma++;
	return NULL;
}

int stm32f_eth_if_munmap(struct ifnet * __if, void * __mem)
{
	struct stm32f_eth_drv * drv = (struct stm32f_eth_drv *)__if->if_drv;

	/* FIXME: check the memory region */
	drv->tx.dma--;
	return 0;
}

int stm32f_eth_if_send(struct ifnet * __if, const uint8_t * __dst, 
					  int __proto, const void * __buf, int __len)
{
	return 0;
}

int stm32f_eth_if_getaddr(struct ifnet * __if, uint8_t * __buf)
{
	return 0;
}

int stm32f_eth_if_getinfo(struct ifnet * __if, char * __s, int __len)
{
	return 0;
}


struct stm32f_eth_drv stm32f_eth_drv;

void stm32f_eth_isr(void)
{
	struct stm32f_eth_drv * drv = &stm32f_eth_drv;
	struct ifnet * ifn = drv->ifn;
	struct stm32f_eth * eth = drv->eth;

	DCC_LOG1(LOG_TRACE, "if=0x%p", ifn);
}

const struct ifnet_operations stm32f_eth_ifnet_op = {
	.op_type = IFT_ETHER,
	.op_addrlen = ETH_ADDR_LEN,
	.op_init = stm32f_eth_if_init,
	.op_cleanup = stm32f_eth_if_cleanup, 
	.op_mmap = stm32f_eth_if_mmap, 
	.op_send = stm32f_eth_if_send,
	.op_arplookup = etharp_lookup, 
	.op_getaddr = stm32f_eth_if_getaddr,
	.op_getinfo = stm32f_eth_if_getinfo, 
	.op_sleep = NULL,
	.op_wakeup = NULL,
	.op_munmap = stm32f_eth_if_munmap
};

struct ifnet * ethif_init(in_addr_t ip_addr, in_addr_t netmask)
{
	struct ifnet * ifn;

	ifn = ifn_register((void *)&stm32f_eth_drv, &stm32f_eth_ifnet_op, 
					   STM32F_ETH, STM32F_IRQ_ETH);

	if (ifn != NULL) {
		ifn_ipv4_set(ifn, ip_addr, netmask);
	}

	return ifn;
}
