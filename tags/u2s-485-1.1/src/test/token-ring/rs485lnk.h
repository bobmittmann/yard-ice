/* 
 * rs485lnk.h
 *
 * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the ThinkOS library.
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

#ifndef __RS485LNK_H__
#define __RS485LNK_H__

#include <sys/stm32f.h>
#include <stdint.h>

struct rs485_link {
	/* USART */
	struct stm32_usart * uart;
	/* DMA controller */
	struct stm32f_dma * dma;
	struct {
		/* DMA stream/channel id */
		int dma_id;
		struct stm32f_dma_stream * dma_strm;
		void * pend_pkt;
		/* Bitband pointer to interrupt status flags */
		uint32_t * isr;
		/* Bitband pointer to interrupt clear flags */
		uint32_t * ifcr;
		int dma_irq;
	} tx;

	struct {
		/* DMA stream/channel id */
		int dma_id;
		struct stm32f_dma_stream * dma_strm;
		void * pend_pkt;
		int max_len;
		/* Bitband pointer to DMA interrupt status flags */
		uint32_t * isr;
		/* Bitband pointer to DMA interrupt clear flags */
		uint32_t * ifcr;
		int dma_irq;
	} rx;

	uint32_t idle_tm;
};

#ifdef __cplusplus
extern "C" {
#endif

void rs485_init(struct rs485_link * lnk, 
				struct stm32_usart * uart,
				unsigned int speed,
				struct stm32f_dma * dma,
				int rx_dma_strm_id, int rx_dma_chan_id, 
				int tx_dma_strm_id, int tx_dma_chan_id);

void rs485_link_isr(struct rs485_link * lnk);

int rs485_pkt_receive(struct rs485_link * lnk, void ** ppkt, int max_len);


void * rs485_pkt_enqueue(struct rs485_link * lnk, void * pkt, int len);

#ifdef __cplusplus
}
#endif

#endif /* __RS485LNK_H__ */

