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
 * @file yard-ice.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arch/cortex-m3.h>
#include <sys/serial.h>
#include <sys/delay.h>
#include <sys/dcclog.h>

#include <thinkos.h>

#include <tcpip/ethif.h>
#include <tcpip/route.h>
#include <tcpip/loopif.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void tcpip_init(void);

int network_config(void)
{
	struct ifnet * ifn;
	in_addr_t ip_addr;
	in_addr_t netmask = INADDR_ANY;
	in_addr_t gw_addr = INADDR_ANY;
	char s[64];
//	char * env;
	char * ip;
	int dhcp;

//	if ((env=getenv("IPCFG")) == NULL) {
		printf("IPCFG not set, using defaults!\n");
		/* default configuration */
		strcpy(s, "192.168.1.22 255.255.255.0 192.168.1.254 0");
		/* set the default configuration */
//		setenv("IPCFG", s, 1);
//	} else {
//		strcpy(s, env);
//	}

//	printf("IPCFG='%s'\n", s);

	printf("ipcfg=%s\n", s);
	ip = strtok(s, " ,");

	printf("ip=%s\n", ip);


	if (!inet_aton(strtok(s, " ,"), (struct in_addr *)&ip_addr)) {
		DCC_LOG(LOG_WARNING, "inet_aton() failed.");
		return -1;
	}

	if (inet_aton(strtok(NULL, " ,"), (struct in_addr *)&netmask)) {
		if (inet_aton(strtok(NULL, " ,"), (struct in_addr *)&gw_addr)) {
			dhcp = strtoul(strtok(NULL, ""), NULL, 0);
		}
	}

	/* initialize the Ethernet interface */
	/* configure the ip address */
	ifn = ethif_init(ip_addr, netmask);
//	ifn = loopif_init(ip_addr, netmask);

	printf("* netif ");
	ifn_getname(ifn, s);
	printf("%s: ", s);

	ifn_ipv4_get(ifn, &ip_addr, &netmask);
	printf("%s, ", inet_ntop(AF_INET, (void *)&ip_addr, s, 16));
	printf("%s\n", inet_ntop(AF_INET, (void *)&netmask, s, 16));

	if (gw_addr != INADDR_ANY) {
		/* add the default route (gateway) to ethif */
		route_add(INADDR_ANY, INADDR_ANY, gw_addr, ifn);
		printf("* default route gw: ");
		printf("%s\n", inet_ntop(AF_INET, (void *)&gw_addr, s, 16));
	}

	if (dhcp) {
#if 0
		/* configure the initial ip address */
		dhcp_start();
		/* schedule the interface to be configured through dhcp */
		dhcp_ifconfig(ethif, dhcp_callback);
		printf("DHCP started.\n");
#endif
	}

	return 0;
}


#define CONSOLE_RX_BUF_LEN 16

struct console_ctrl {
	struct stm32f_usart * uart;
	struct stm32f_dma * dma;
	struct {
		struct stm32f_dma_stream * s;
	} tx;
	struct {
		struct stm32f_dma_stream * s;
		uint8_t buf[CONSOLE_RX_BUF_LEN];
	} rx;
};

static int inline getc(struct stm32f_usart * usart, unsigned int msec)
{
	int tm;

	tm = msec * 20;

	for (;;) {		
		if (usart->sr & USART_RXNE) {
			return usart->dr;
		}
		if (tm == 0) {
			return -1;
		}
		udelay(50);
		tm--;
	}
}

int console_read(struct console_ctrl * ctrl, char * buf, 
				 unsigned int len, unsigned int msec)
{
	uint32_t st;

	if (ctrl->rx.s->cr & DMA_EN) {
		fprintf(stderr, "%s() DMA enabled\n", __func__);
	}

	/* Memory address */
	ctrl->rx.s->m0ar = (void *)buf;
	/* Number of data items to transfer */
	ctrl->rx.s->ndtr = len;

	/* enable DMA */
	ctrl->rx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_DIR_PTM | DMA_TCIE  | DMA_EN;

	/* wait for the DMA transfer to complete */
	while (((st = ctrl->dma->hisr) & DMA_TCIF7) == 0) {
		thinkos_irq_wait(STM32F_IRQ_DMA1_STREAM7);
	} 

	/* clear the the DMA stream trasfer complete flag */
	ctrl->dma->hifcr = DMA_CTCIF7;

	return len;
}

int console_write(struct console_ctrl * ctrl, const void * buf, 
					   unsigned int len)
{
	uint32_t st;
	uint32_t sr;

	if (ctrl->tx.s->cr & DMA_EN) {
		fprintf(stderr, "%s() DMA enabled\n", __func__);
	}

	/* Memory address */
	ctrl->tx.s->m0ar = (void *)buf;
	/* Number of data items to transfer */
	ctrl->tx.s->ndtr = len;

	/* clear the TC bit */
	if ((sr = ctrl->uart->sr) & USART_TC) {
		fprintf(stderr, "%s() TC=1\n", __func__);
	}

	/* enable DMA */
	ctrl->tx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_DIR_MTP | DMA_TCIE  | DMA_EN;

	/* wait for the DMA transfer to complete */
	while (((st = ctrl->dma->lisr) & DMA_TCIF0) == 0) {
		thinkos_irq_wait(STM32F_IRQ_DMA1_STREAM0);
	} 

	/* clear the the DMA stream trasfer complete flag */
	ctrl->dma->lifcr = DMA_CTCIF0;

	return len;
}

int console_flush(struct console_ctrl * ctrl)
{
	return 0;
}

const struct fileop console_ops = {
	.write = (void *)console_write,
	.read = (void *)console_read,
	.flush = (void *)console_flush,
	.close = (void *)NULL
};

const struct console_ctrl uart5_ctrl = {
	.uart = STM32F_UART5, 
	.dma = STM32F_DMA1,
	.rx.s = &STM32F_DMA1->s[0],
	.tx.s = &STM32F_DMA1->s[7]
};

const struct file console_file = {
	.data = (void *)&uart5_ctrl, 
	.op = &console_ops
};

struct file * console_open(unsigned int baudrate, unsigned int flags)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct console_ctrl * ctrl = (struct console_ctrl *)console_file.data;

	stm32f_usart_init(ctrl->uart, baudrate, flags);

	/* Enable DMA for transmission and reception */
	ctrl->uart->cr3 |= USART_DMAT | USART_DMAR;

	/* DMA clock enable */
	rcc->ahb1enr |= (ctrl->dma == STM32F_DMA1) ? RCC_DMA1EN : RCC_DMA2EN;

	/* Disable DMA stream */
	ctrl->tx.s->cr = 0;
	while (ctrl->tx.s->cr & DMA_EN); /* Wait for the channel to be ready .. */
	ctrl->tx.s->par = &ctrl->uart->dr;
//	ctrl->tx.s->fcr = 0;
	ctrl->tx.s->fcr = DMA_DMDIS | DMA_FTH_FULL;

	/* Disable DMA stream */
	ctrl->rx.s->cr = 0;
	while (ctrl->rx.s->cr & DMA_EN); /* Wait for the channel to be ready .. */
	ctrl->rx.s->par = &ctrl->uart->dr;
	/* Memory address */
	ctrl->tx.s->m0ar = (void *)ctrl->rx.buf;
//	ctrl->tx.s->m0ar = (void *)ctrl->rx.buf[0];
//	ctrl->tx.s->m1ar = (void *)ctrl->rx.buf[1];
	/* Number of data items to transfer */
	ctrl->tx.s->ndtr = CONSOLE_RX_BUF_LEN;
	ctrl->rx.s->cr = DMA_CHSEL_SET(4) | DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_DIR_PTM;

	return (struct file *)&console_file;
}

int main(int argc, char ** argv)
{
	int i;

	DCC_LOG(LOG_TRACE, "cm3_udelay_calibrate()");
	cm3_udelay_calibrate();
	udelay(100000);
	DCC_LOG(LOG_TRACE, "stm32f_usart_open().");
	stdout = stm32f_usart_open(STM32F_UART5, 115200, SERIAL_8N1);
	udelay(100000);

	DCC_LOG(LOG_TRACE, "thinkos_init().");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "tcpip_init().");
	tcpip_init();

	DCC_LOG(LOG_TRACE, "network_config().");
	network_config();

	printf("\n");
	printf("---------------------------------------------------------\n");
	printf(" YAR-ICE\n");
	printf("---------------------------------------------------------\n");
	printf("\n");


	for (i = 0; i < 1000; i++) {
		DCC_LOG1(LOG_TRACE, "%d.", i);
		thinkos_sleep(2000);
	}

	printf("---------------------------------------------------------\n");
	delay(10);

	return 0;
}

