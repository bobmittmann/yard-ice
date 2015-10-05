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
#include "simrpc.h"
#include "simrpc_svc.h"

#define RX_DMA_CHAN STM32_DMA_CHANNEL6
#define TX_DMA_CHAN STM32_DMA_CHANNEL7

struct simlnk {
	uint32_t tx_lock;
	uint32_t tx_buf[(SIMLNK_MTU + 3) / 4];
	uint32_t rx_buf[(SIMLNK_MTU + 2 + 3) / 4];
} simlnk;

int simlnk_dma_xmit(unsigned int len)
{
	struct stm32f_dma * dma = STM32_DMA1;
	struct stm32_usart * uart = STM32_USART2;
	uint32_t ccr;

	/* Disable DMA */
//	if ((ccr = dma->ch[TX_DMA_CHAN].ccr) & DMA_EN) {
  //      DCC_LOG(LOG_ERROR, "DMA enabled");
 //       abort();
//	}

	while ((ccr = dma->ch[TX_DMA_CHAN].ccr) & DMA_EN)
		dma->ch[TX_DMA_CHAN].ccr = ccr & ~DMA_EN;

	/* Program DMA transfer */
	dma->ch[TX_DMA_CHAN].cndtr = len;
	dma->ch[TX_DMA_CHAN].ccr = ccr | DMA_EN;	
	/* enable the transfer complete interrupt */
	uart->cr1 |= USART_TCIE;

	return 0;
}

int __simrpc_send(uint32_t opc, void * data, unsigned int cnt)
{
	if (__ldrex(&simlnk.tx_lock))
		return -1;
	if (__strex(&simlnk.tx_lock, 1))
		return -1;

	simlnk.tx_buf[0] = opc;
	__thinkos_memcpy(&simlnk.tx_buf[1], data, cnt);

	return simlnk_dma_xmit(cnt + 4);
}

int __simrpc_send_int(uint32_t opc, int val)
{
	if (__ldrex(&simlnk.tx_lock))
		return -1;
	if (__strex(&simlnk.tx_lock, 1))
		return -1;

	simlnk.tx_buf[0] = opc;
	simlnk.tx_buf[1] = (uint32_t)val;

	return simlnk_dma_xmit(8);
}

int __simrpc_send_opc(uint32_t opc)
{
	if (__ldrex(&simlnk.tx_lock))
		return -1;
	if (__strex(&simlnk.tx_lock, 1))
		return -1;

	simlnk.tx_buf[0] = opc;

	return simlnk_dma_xmit(4);
}

static void simlnk_dma_recv(uint32_t * pkt, unsigned int cnt)
{
	uint32_t opc;
	void * data;
	int wq = THINKOS_WQ_COMM_RECV;
	int th;

	/* get the opcode */
	opc = pkt[0];

	DCC_LOG4(LOG_INFO, "dst=%02x src=%02x insn=%d seq=%d", 
			 SIMRPC_DST(opc), SIMRPC_SRC(opc),
			 SIMRPC_INSN(opc), SIMRPC_SEQ(opc));


	/* data portion */
	data = (void *)&pkt[1];
	/* adjust the data length */
	cnt -= 4;
	/* decode instruction */
	switch (SIMRPC_INSN(opc)) {
	case SIMRPC_SUSPEND:
		simrpc_suspend_svc(opc, data, cnt);
		break;
	case SIMRPC_RESUME:
		simrpc_resume_svc(opc, data, cnt);
		break;
	case SIMRPC_REBOOT:
		simrpc_reboot_svc(opc, data, cnt);
		break;
	case SIMRPC_EXEC:
		simrpc_exec_svc(opc, data, cnt);
		break;
	case SIMRPC_KERNELINFO:
		simrpc_kernelinfo_svc(opc, data, cnt);
		break;
	case SIMRPC_MEM_LOCK:
		simrpc_mem_lock_svc(opc, data, cnt);
		break;
	case SIMRPC_MEM_UNLOCK:
		simrpc_mem_unlock_svc(opc, data, cnt);
		break;
	case SIMRPC_MEM_ERASE:
		simrpc_mem_erase_svc(opc, data, cnt);
		break;
	case SIMRPC_MEM_READ:
		simrpc_mem_read_svc(opc, data, cnt);
		break;
	case SIMRPC_MEM_WRITE:
		simrpc_mem_write_svc(opc, data, cnt);
		break;
	case SIMRPC_MEM_SEEK:
		simrpc_mem_seek_svc(opc, data, cnt);
		break;
	case SIMRPC_MEM_CRC32:
		simrpc_mem_seek_svc(opc, data, cnt);
		break;
	default:
		/* forward to userland */
		if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			*((uint32_t *)thinkos_rt.ctx[th]->r1) = opc;
			__thinkos_memcpy32((void *)thinkos_rt.ctx[th]->r2, data, cnt);
			/* wakeup from the console wait queue */
			__thinkos_wakeup_return(wq, th, cnt);
			/* signal the scheduler ... */
			__thinkos_defer_sched();
		} else {
			/* no user waiting, return an error */
			DCC_LOG1(LOG_WARNING, "No thread on comm_recv wait queue, insn=%d",
					 SIMRPC_INSN(opc));
			__simrpc_send_int(SIMRPC_REPLY_ERR(opc), SIMRPC_ENOSYS);
		}
	}
}

void thinkos_comm_svc(int32_t * arg)
{
	int self = thinkos_rt.active;
	unsigned int req = arg[0];
#if 0
	unsigned int wq;
	unsigned int len;
	uint32_t queue;
	uint8_t * buf;
	int n;
#endif

	switch (req) {
	case COMM_RECV:
		DCC_LOG(LOG_INFO, "Comm recv wait...");
		/* wait for event */
		__thinkos_suspend(self);
		/* insert into the comm receive wait queue */
		__thinkos_wq_insert(THINKOS_WQ_COMM_RECV, self);
		/* signal the scheduler ... */
		__thinkos_defer_sched(); 
		break;

	case COMM_SEND:
		arg[0] = 0;
again:
		if (__ldrex(&simlnk.tx_lock)) {
			/* wait for event */
			__thinkos_suspend(self);
			/* insert into the comm send wait queue */
			__thinkos_wq_insert(THINKOS_WQ_COMM_SEND, self);
			/* signal the scheduler ... */
			__thinkos_defer_sched(); 
		} else {
			uint32_t opc;
			void * buf;
			int cnt;

			if (__strex(&simlnk.tx_lock, 1))
				goto again;

			opc = arg[1];
			buf = (void *)arg[2];
			cnt = arg[3];

			simlnk.tx_lock = 1;
			simlnk.tx_buf[0] = opc;
			__thinkos_memcpy(&simlnk.tx_buf[1], buf, cnt);
			simlnk_dma_xmit(cnt + 4);
		}
		break;

	default:
		DCC_LOG1(LOG_ERROR, "invalid comm request %d!", req);
		arg[0] = THINKOS_EINVAL;
		break;
	}
}

void stm32_usart2_isr(void)
{
	struct stm32f_dma * dma = STM32_DMA1;
	struct stm32_usart * uart = STM32_USART2;
	uint32_t sr;
	uint32_t cr;
	
	cr = uart->cr1;
	sr = uart->sr & (cr | USART_FE);

	if (sr & USART_TC) {
		int wq = THINKOS_WQ_COMM_SEND;
		uint32_t opc;
		void * buf;
		int cnt;
		int th;

		DCC_LOG(LOG_MSG, "TC");
		/* TC interrupt is cleared by writing 0 back to the SR register */
		uart->sr = sr & ~USART_TC;
        /* Disable the transfer complete interrupt */
		cr &= ~USART_TCIE;
		/* Generate a break condition */
		cr |= USART_SBK;
		uart->cr1 = cr;

		if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			/* wakeup from the console wait queue */
			__thinkos_wakeup(wq, th);
			/* signal the scheduler ... */
			__thinkos_defer_sched();

			opc = thinkos_rt.ctx[th]->r1;
			buf = (void *)thinkos_rt.ctx[th]->r2;
			cnt = thinkos_rt.ctx[th]->r3;
			simlnk.tx_buf[0] = opc;
			__thinkos_memcpy(&simlnk.tx_buf[1], buf, cnt);
			simlnk_dma_xmit(cnt + 4);
		} else {
			simlnk.tx_lock = 0;
		}
	}

	/* break detection */
	if (sr & USART_FE) {
		unsigned int cnt;
		uint32_t ccr;
		int c;
		
		/* Disable DMA */
		while ((ccr = dma->ch[RX_DMA_CHAN].ccr) & DMA_EN)
			dma->ch[RX_DMA_CHAN].ccr = ccr & ~DMA_EN;

		/* clear interrupt status flags */
		c = uart->rdr;
		(void)c;

		/* Get number of bytes received */
		cnt = sizeof(simlnk.rx_buf) - dma->ch[RX_DMA_CHAN].cndtr;
		/* Prepare next DMA transfer */
		dma->ch[RX_DMA_CHAN].cndtr = sizeof(simlnk.rx_buf);
		dma->ch[RX_DMA_CHAN].ccr = ccr | DMA_EN;

		DCC_LOG1(LOG_INFO, "BRK! cnt=%d", cnt);
	
		if (cnt > 4) {
			/* process this request,
			 remove the break character from the packet length. */
			simlnk_dma_recv(simlnk.rx_buf, cnt - 1);
		} else {
			DCC_LOG1(LOG_WARNING, "short pkt! cnt=%d", cnt);
		}
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

int simlnk_init(struct simlnk * lnk, const char * name, 
				unsigned int addr, struct serial_dev * dev)
{
	struct stm32f_dma * dma = STM32_DMA1;
	struct stm32_usart * uart = STM32_USART2;

	simlnk.tx_lock = 0;

	stm32_clk_enable(STM32_RCC, STM32_CLK_USART2);
	stm32_clk_enable(STM32_RCC, STM32_CLK_DMA1);

	/* UART --------------------------------------------------------------- */
	uart->gtpr = 1;
	uart->brr = stm32f_apb1_hz / SIMLNK_BAUDRATE;
	/* configure the UART for DMA transfer */
	uart->cr3 = USART_ONEBIT | USART_DMAT | USART_DMAR | USART_EIE;
	/* Configure 8N1 */
	uart->cr2 = USART_STOP_1;
	/* enable UART and errors interrupt */
	uart->cr1 = USART_UE | USART_RE | USART_TE | USART_TCIE;

	/* TX DMA ------------------------------------------------------------- */
	dma->ch[TX_DMA_CHAN].ccr = 0;
	while (dma->ch[TX_DMA_CHAN].ccr & DMA_EN); 
	dma->ch[TX_DMA_CHAN].cpar = &uart->dr;
	dma->ch[TX_DMA_CHAN].cmar = simlnk.tx_buf;
	dma->ch[TX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
		DMA_MINC | DMA_DIR_MTP | DMA_TEIE | DMA_TCIE;

	/* RX DMA ------------------------------------------------------------- */
	dma->ch[RX_DMA_CHAN].ccr = 0;
	while (dma->ch[RX_DMA_CHAN].ccr & DMA_EN); 
	dma->ch[RX_DMA_CHAN].cpar = &uart->dr;
	dma->ch[RX_DMA_CHAN].cmar = simlnk.rx_buf;
	dma->ch[RX_DMA_CHAN].cndtr = sizeof(simlnk.rx_buf);
	dma->ch[RX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
		DMA_MINC | DMA_DIR_PTM | DMA_TEIE | DMA_TCIE | DMA_EN;

#ifdef CM3_RAM_VECTORS
	thinkos_irq_register(STM32_IRQ_USART2, IRQ_PRIORITY_LOW, 
						 stm32_usart2_isr);
	thinkos_irq_register(STM32_IRQ_DMA1_CH7, IRQ_PRIORITY_LOW, 
						 stm32_dma1_channel7_isr);
	thinkos_irq_register(STM32_IRQ_DMA1_CH6, IRQ_PRIORITY_LOW, 
						 stm32_dma1_channel6_isr);
#else
	cm3_irq_pri_set(STM32_IRQ_USART2, IRQ_PRIORITY_LOW);
	cm3_irq_pri_set(STM32_IRQ_DMA1_CH7, IRQ_PRIORITY_LOW);
	cm3_irq_pri_set(STM32_IRQ_DMA1_CH6, IRQ_PRIORITY_LOW);
	cm3_irq_enable(STM32_IRQ_USART2);
	cm3_irq_enable(STM32_IRQ_DMA1_CH7);
	cm3_irq_enable(STM32_IRQ_DMA1_CH6);
#endif

	return 0;
}
	
