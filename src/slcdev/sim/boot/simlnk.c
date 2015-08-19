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

#include <thinkos.h>
#include <sys/serial.h>

#include "board.h"
#include "simlnk.h"

#define SIMLNK_BAUDRATE 10000

/* enable RX IDLE and errors interrupt */
#define USART_CR1 (USART_UE | USART_RE | USART_TE | USART_IDLEIE | \
				   USART_TCIE | USART_EIE)


#define RX_DMA_CHAN STM32_DMA_CHANNEL6
#define TX_DMA_CHAN STM32_DMA_CHANNEL7

struct simlnk {
	uint32_t tx_buf[520 / 4];
	uint32_t rx_buf[520 / 4];
} simlnk;

void simlnk_dma_recv(uint32_t opc, void * data, unsigned int cnt)
{

	switch (opc >> 24) {
	case SIMLNK_RPC_MEM_LOCK:
		DCC_LOG(LOG_TRACE, "MEM_LOCK");
		simrpc_mem_lock_svc(opc, data, cnt);
		break;
	case SIMLNK_RPC_MEM_UNLOCK:
		DCC_LOG(LOG_TRACE, "MEM_UNLOCK");
		simrpc_mem_unlock_svc(opc, data, cnt);
		break;
	case SIMLNK_RPC_MEM_ERASE:
		DCC_LOG(LOG_TRACE, "MEM_ERASE");
		simrpc_mem_erase_svc(opc, data, cnt);
		break;
	case SIMLNK_RPC_MEM_READ:
		DCC_LOG(LOG_TRACE, "MEM_READ");
		simrpc_mem_read_svc(opc, data, cnt);
		break;
	case SIMLNK_RPC_MEM_WRITE:
		DCC_LOG(LOG_TRACE, "MEM_WRITE");
		simrpc_mem_write_svc(opc, data, cnt);
		break;
	case SIMLNK_RPC_MEM_SEEK:
		DCC_LOG(LOG_TRACE, "MEM_SEEK");
		simrpc_mem_seek_svc(opc, data, cnt);
		break;
	default:
		DCC_LOG1(LOG_WARNING, "Invalid insn: 0x%02x", opc >> 24);
	}
}


void __attribute__((noinline)) simlnk_xmit(unsigned int len)
{
	struct stm32f_dma * dma = STM32_DMA1;
	uint32_t ccr;

	/* Disable DMA */
	while ((ccr = dma->ch[TX_DMA_CHAN].ccr) & DMA_EN)
		dma->ch[TX_DMA_CHAN].ccr = ccr & ~DMA_EN; 
	/* Program DMA transfer */
	dma->ch[TX_DMA_CHAN].cndtr = len;
	dma->ch[TX_DMA_CHAN].ccr = ccr | DMA_EN;	
}

int simrpc_send(uint32_t opc, void * data, unsigned int cnt)
{
	struct stm32f_dma * dma = STM32_DMA1;
	uint32_t ccr;

	simlnk.tx_buf[0] = opc;
	__thinkos_memcpy(&simlnk.tx_buf[1], data, cnt);

	/* Disable DMA */
	while ((ccr = dma->ch[TX_DMA_CHAN].ccr) & DMA_EN)
		dma->ch[TX_DMA_CHAN].ccr = ccr & ~DMA_EN; 
	/* Program DMA transfer */
	dma->ch[TX_DMA_CHAN].cndtr = cnt + 4;
	dma->ch[TX_DMA_CHAN].ccr = ccr | DMA_EN;	

	return 0;
}


void stm32_usart2_isr(void)
{
	struct stm32f_dma * dma = STM32_DMA1;
	struct stm32_usart * uart = STM32_USART2;
	uint32_t sr;
	uint32_t cr;
	
	sr = uart->sr;

	if (sr & USART_TC) {
		DCC_LOG(LOG_INFO, "TC");
		/* TC interrupt is cleared by writing 0 back to the SR register */
		uart->sr = sr & ~USART_TC;
		/* Pulse TE to generate an IDLE Frame */
		cr = uart->cr1;
		uart->cr1 = cr & ~USART_TE;
		uart->cr1 = cr;
		simlnk_xmit(4);
	}

	if (sr & (USART_IDLE | USART_ORE | USART_NF | USART_FE)) {
		uint32_t ccr;
		int c;
		
		/* Disable DMA, this will cause the DMA TCF (transfer complete
		 flag) to be set.*/
		ccr = dma->ch[RX_DMA_CHAN].ccr;
		dma->ch[RX_DMA_CHAN].ccr = ccr & ~DMA_EN;

		/* clear interrupt status flags */
		c = uart->rdr;
		(void)c;

		if (sr & (USART_ORE | USART_NF)) {
			DCC_LOG1(LOG_WARNING, "Error: %04b!", 
					sr & (USART_ORE | USART_NF | USART_FE));
		}

		/* break detection */
		if (sr & USART_FE) {
			unsigned int cnt;
			uint32_t opc;

			/* Get number of bytes received */
			cnt = sizeof(simlnk.rx_buf) - dma->ch[RX_DMA_CHAN].cndtr;
			/* Get the opcode form the buffer head */
			opc = simlnk.rx_buf[0];
			/* Prepare next DMA transfer */
			dma->ch[RX_DMA_CHAN].cndtr = sizeof(simlnk.rx_buf);
			dma->ch[RX_DMA_CHAN].ccr = ccr | DMA_EN;

			DCC_LOG2(LOG_TRACE, "BRK! opc=%08x cnt=%d", opc, cnt);

			if (cnt > 4) {
				/* process this request */
				simlnk_dma_recv(opc, &simlnk.rx_buf[1], cnt - 5);
			}
		}
#if 0
		/* idle detection */
		if (sr & USART_IDLE) {
			unsigned int cnt;
			uint32_t opc;

			/* Get number of bytes received */
			cnt = sizeof(simlnk.rx_buf) - dma->ch[RX_DMA_CHAN].cndtr;
			/* Get the opcode form the buffer head */
			opc = simlnk.rx_buf[0];
			/* Prepare next DMA transfer */
			dma->ch[RX_DMA_CHAN].cndtr = sizeof(simlnk.rx_buf);
			dma->ch[RX_DMA_CHAN].ccr = ccr | DMA_EN;

			DCC_LOG2(LOG_TRACE, "IDLE! opc=%08x cnt=%d", opc, cnt);

			if (cnt >= 4) {
				/* process this request */
				simlnk_dma_recv(opc, &simlnk.rx_buf[1], cnt - 4);
			}
		}
#endif
	}	

}

/* TX DMA ------------------------------------------------------------- */
void stm32_dma1_channel7_isr(void)
{
	struct stm32f_dma * dma = STM32_DMA1;

	if (dma->isr & DMA_TCIF7) {
		DCC_LOG(LOG_MSG, "TX TCIF");
		/* clear the DMA transfer complete flag */
		dma->ifcr = DMA_CTCIF7;
	}

	if (dma->isr & DMA_TEIF7) {
		DCC_LOG(LOG_TRACE, "TX TEIF");
		/* clear the DMA transfer complete flag */
		dma->ifcr = DMA_CTEIF7;
	}
}


/* RX DMA ------------------------------------------------------------- */
void stm32_dma1_channel6_isr(void)
{
	struct stm32f_dma * dma = STM32_DMA1;

	if (dma->isr & DMA_TCIF6) {
		DCC_LOG(LOG_TRACE, "RX TCIF");
		/* clear the DMA transfer complete flag */
		dma->ifcr = DMA_CTCIF6;
	}

	if (dma->isr & DMA_TEIF6) {
		DCC_LOG(LOG_TRACE, "RX TEIF");
		/* clear the DMA transfer error flag */
		dma->ifcr = DMA_CTEIF6;
	}
}

uint32_t __attribute__((noinline)) __crc32(void  * buf, unsigned int len)
{
	struct stm32_crc * crc = STM32_CRC;
	uint8_t * src = (uint8_t *)buf;
	uint8_t * mrk;

	crc->cr = CRC_RESET;

	mrk = src + (len & ~3);
	while (src != mrk) {
		crc->dr = src[0] + (src[1] << 8) + (src[2] << 16) + (src[3] << 24);
		src += 4;
	}
	
	switch (len & 3) {
	case 3:
		crc->dr = src[0] + (src[1] << 8) + (src[2] << 16);
		break;
	case 2:
		crc->dr = src[0] + (src[1] << 8);
		break;
	case 1:
		crc->dr = src[0];
		break;
	}

	return crc->dr;
}

void __attribute__((noinline)) simlnk_enqueue(uint32_t opc, void  * buf, 
												  unsigned int len)
{
	struct stm32_crc * crc = STM32_CRC;
	uint32_t * src = (uint32_t *)buf;
	uint32_t * dst = (uint32_t *)simlnk.tx_buf;
	uint32_t data;
	uint32_t * mrk;

	crc->cr = CRC_RESET;

	crc->dr = opc;
	*dst++ = opc;

	mrk = src + (len >> 2);
	while (src != mrk) {
		data = *src++;
		crc->dr = data;
		*dst++ = data;
	}

	data = *src;
	switch (len & 3) {
	case 3:
		data &= 0x00ffffff;
		crc->dr = data;
		*dst++ = data | ((crc->dr & 0xff) << 24);
		*dst = (crc->dr >> 8) & 0xff;
		break;
	case 2:
		data &= 0x0000ffff;
		crc->dr = data;
		*dst = data | ((crc->dr & 0xffff) << 16);
		break;
	case 1:
		data &= 0x000000ff;
		crc->dr = data;
		*dst = data | ((crc->dr & 0xffff) << 8);
		break;
	case 0:
		*dst = crc->dr & 0xffff;
	}
}

int simlnk_init(struct simlnk * lnk, const char * name, 
				unsigned int addr, struct serial_dev * dev)
{
	struct stm32f_dma * dma = STM32_DMA1;
	struct stm32_usart * uart = STM32_USART2;

	stm32_clk_enable(STM32_RCC, STM32_CLK_USART2);
	stm32_clk_enable(STM32_RCC, STM32_CLK_DMA1);

	/* UART --------------------------------------------------------------- */
	uart->gtpr = 1;
	uart->brr = stm32f_apb1_hz / SIMLNK_BAUDRATE;
	/* configure the UART for DMA transfer */
	uart->cr3 = USART_ONEBIT | USART_DMAT | USART_DMAR | USART_EIE;
	/* Configure 8N1 */
	uart->cr2 = USART_STOP_1;
	/* enable RX IDLE and errors interrupt */
	uart->cr1 = USART_UE | USART_RE | USART_TE | USART_TCIE;
//	uart->cr1 = USART_UE | USART_RE | USART_TE | USART_IDLEIE | USART_TCIE;


	/* TX DMA ------------------------------------------------------------- */
	dma->ch[TX_DMA_CHAN].ccr = 0;
	while (dma->ch[TX_DMA_CHAN].ccr & DMA_EN); 
	dma->ch[TX_DMA_CHAN].cpar = &uart->dr;
	dma->ch[TX_DMA_CHAN].cmar = simlnk.tx_buf;
	dma->ch[TX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
		DMA_MINC | DMA_DIR_MTP | DMA_TEIE | DMA_TCIE;

	simlnk.tx_buf[0] = 0x005555ff;

	/* RX DMA ------------------------------------------------------------- */
	dma->ch[RX_DMA_CHAN].ccr = 0;
	while (dma->ch[RX_DMA_CHAN].ccr & DMA_EN); 
	dma->ch[RX_DMA_CHAN].cpar = &uart->dr;
	dma->ch[RX_DMA_CHAN].cmar = simlnk.rx_buf;
	dma->ch[RX_DMA_CHAN].cndtr = sizeof(simlnk.rx_buf);
	dma->ch[RX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
		DMA_MINC | DMA_DIR_PTM | DMA_TEIE | DMA_TCIE | DMA_EN;
//		DMA_MINC | DMA_DIR_PTM | DMA_TEIE | DMA_TCIE;

#ifdef CM3_RAM_VECTORS
	thinkos_irq_register(STM32_IRQ_USART2, IRQ_PRIORITY_LOW, 
						 stm32_usart2_isr);
	thinkos_irq_register(STM32_IRQ_DMA1_CH7, IRQ_PRIORITY_LOW, 
						 stm32_dma1_channel7_isr);
	thinkos_irq_register(STM32_IRQ_DMA1_CH6, IRQ_PRIORITY_LOW, 
						 stm32_dma1_channel6_isr);
#else
	cm3_irq_pri_set(STM32_IRQ_USART2, IRQ_PRIORITY_LOW);
	cm3_irq_enable(STM32_IRQ_USART2);
	cm3_irq_pri_set(STM32_IRQ_DMA1_CH7, IRQ_PRIORITY_LOW);
	cm3_irq_enable(STM32_IRQ_DMA1_CH7);
	cm3_irq_pri_set(STM32_IRQ_DMA1_CH6, IRQ_PRIORITY_LOW);
	cm3_irq_enable(STM32_IRQ_DMA1_CH6);
#endif

	return 0;
}

