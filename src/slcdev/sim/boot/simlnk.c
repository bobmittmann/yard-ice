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

#ifndef ENABLE_SIMLNK_SANITY_CHECK
#define ENABLE_SIMLNK_SANITY_CHECK 0
#endif

#define RX_DMA_CHAN STM32_DMA_CHANNEL6
#define TX_DMA_CHAN STM32_DMA_CHANNEL7

struct simlnk {
	uint32_t tx_lock;
	uint32_t rx_pend;
	uint32_t tx_buf[(SIMLNK_MTU + 3) / 4];
	uint32_t rx_buf[(SIMLNK_MTU + 2 + 3) / 4];
} simlnk;

_Pragma ("GCC optimize (\"Ofast\")")

static int simlnk_dma_xmit(unsigned int len)
{
	struct stm32f_dma * dma = STM32_DMA1;
	struct stm32_usart * uart = STM32_USART2;
	uint32_t ccr;

	ccr = dma->ch[TX_DMA_CHAN].ccr;

#if ENABLE_SIMLNK_SANITY_CHECK
	/* Disable DMA */
	if (ccr & DMA_EN) {
        DCC_LOG(LOG_ERROR, "DMA enabled");
		/* disable DMA */
		dma->ch[TX_DMA_CHAN].ccr = ccr & ~DMA_EN;
		while ((ccr = dma->ch[TX_DMA_CHAN].ccr) & DMA_EN);
	} 
#endif
	/* Make sure the TC interrupt is clear */
	uart->sr &= ~USART_TC;

	DCC_LOG1(LOG_INFO, "DMA start %d bytes.", len);
	/* Program DMA transfer */
	dma->ch[TX_DMA_CHAN].cndtr = len;
	dma->ch[TX_DMA_CHAN].ccr = ccr | DMA_EN;	
	/* enable the transfer complete interrupt */
	uart->cr1 |= USART_TCIE;

	return 0;
}

int __simrpc_send(uint32_t opc, const void * data, unsigned int cnt)
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

void simlnk_simrpc_input(uint32_t opc, void * data, unsigned int cnt)
{
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
	case SIMRPC_EXCEPTINFO:
		simrpc_exceptinfo_svc(opc, data, cnt);
		break;
	case SIMRPC_THREADINFO:
		simrpc_threadinfo_svc(opc, data, cnt);
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
	}
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
	if (SIMRPC_INSN(opc) < 32) { 
		if (simlnk.tx_lock) {
			/* save for later */
			if (simlnk.rx_pend)
				DCC_LOG(LOG_WARNING, "pending data in the RX buffer!");
			simlnk.rx_pend = cnt + 4;
		} else {
			simlnk_simrpc_input(opc, data, cnt);
		}
	} else {
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

void stm32_usart2_isr(void)
{
	struct stm32f_dma * dma = STM32_DMA1;
	struct stm32_usart * uart = STM32_USART2;
	uint32_t sr;
	uint32_t cr;
	
	cr = uart->cr1;
	sr = uart->sr & (cr | USART_LBD);

	if (sr & USART_TC) {
		int wq = THINKOS_WQ_COMM_SEND;
		uint32_t opc;
		void * data;
		int cnt;
		int th;

		/* TC interrupt is cleared by writing 0 back to the SR register */
		uart->sr = sr & ~USART_TC;
        /* Disable the transfer complete interrupt */
		cr &= ~USART_TCIE;
		/* Generate a break condition */
		cr |= USART_SBK;
		uart->cr1 = cr;

		/* disable DMA */
		dma->ch[TX_DMA_CHAN].ccr &= ~DMA_EN;

		if (simlnk.rx_pend) {
			opc = simlnk.rx_buf[0];
			data = (void *)&simlnk.rx_buf[1];
			cnt = simlnk.rx_pend - 4;
			simlnk.tx_lock = 0;
			simlnk.rx_pend = 0;
			simlnk_simrpc_input(opc, data, cnt);
			DCC_LOG(LOG_TRACE, "TC RX pend.");
		} else if ((th = __thinkos_wq_head(wq)) != THINKOS_THREAD_NULL) {
			DCC_LOG(LOG_INFO, "TC xmit pending...");
			/* wakeup from the console wait queue */
			__thinkos_wakeup(wq, th);
			/* signal the scheduler ... */
			__thinkos_defer_sched();

			opc = thinkos_rt.ctx[th]->r1;
			data = (void *)thinkos_rt.ctx[th]->r2;
			cnt = thinkos_rt.ctx[th]->r3;
			simlnk.tx_buf[0] = opc;
			__thinkos_memcpy(&simlnk.tx_buf[1], data, cnt);

			simlnk_dma_xmit(cnt + 4);
		} else {
			simlnk.tx_lock = 0;
			DCC_LOG(LOG_INFO, "TC TX unlock.");
		}
	}

	/* break detection */
	if (sr & USART_LBD) {
		unsigned int cnt;
		uint32_t ccr;

		/* Disable DMA */
		dma->ch[RX_DMA_CHAN].ccr &= ~DMA_EN;
		/* Wait for DMA to stop */
		while ((ccr = dma->ch[RX_DMA_CHAN].ccr) & DMA_EN);

		/* clear the break detection interrupt flag */
		uart->sr = sr & ~USART_LBD;
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


#if ENABLE_SIMLNK_SANITY_CHECK
/* TX DMA ------------------------------------------------------------- */
void stm32_dma1_channel7_isr(void)
{
	struct stm32f_dma * dma = STM32_DMA1;

	if (dma->isr & DMA_TCIF7) {
		DCC_LOG(LOG_TRACE, "TX TCIF");
		/* clear the DMA transfer complete flag */
		dma->ifcr = DMA_CTCIF7;
	}

	if (dma->isr & DMA_TEIF7) {
		DCC_LOG(LOG_WARNING, "TX TEIF");
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
		DCC_LOG(LOG_WARNING, "RX TEIF");
		/* clear the DMA transfer error flag */
		dma->ifcr = DMA_CTEIF6;
	}
}
#endif

_Pragma ("GCC optimize (\"Os\")")

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
		/* Set the return value to ZERO. This system call never fails. */
		arg[0] = 0;
again:
		if (__ldrex(&simlnk.tx_lock)) {
			unsigned int wq = THINKOS_WQ_COMM_SEND;
			uint32_t queue;
			/* (1) suspend the thread by removing it from the
			   ready wait queue. The __thinkos_suspend() call cannot be nested
			   inside a LDREX/STREX pair as it may use the exclusive access 
			   itself,
			   in case we have anabled the time sharing option.
			   It is not a problem having a thread not contained in any 
			   waiting queue inside a system call. */ 
			__thinkos_suspend(self);
			/* update the thread status in preparation for event wait */
#if THINKOS_ENABLE_THREAD_STAT
			thinkos_rt.th_stat[self] = wq << 1;
#endif
			/* (2) Save the context pointer. In case an interrupt wakes up
			   this thread before the scheduler is called, this will allow
			   the interrupt handler to locate the return value (r0) address. */
			thinkos_rt.ctx[self] = (struct thinkos_context *)&arg[-8];
			/* Get the event wait queue... */
			queue = __ldrex(&thinkos_rt.wq_lst[wq]);

			/* The lock may have been released while suspending (1).
			   If this is the case roll back and restart. */
			if (!simlnk.tx_lock) {
				/* roll back */
#if THINKOS_ENABLE_THREAD_STAT
				thinkos_rt.th_stat[self] = 0;
#endif
				/* insert into the ready wait queue */
				__bit_mem_wr(&thinkos_rt.wq_ready, self, 1);  
				DCC_LOG2(LOG_WARNING, "<%d> rollback 1 %d...", self, wq);
				goto again;
			}

			/* Insert into the event wait queue */
			queue |= (1 << self);
			/* (3) Try to save back the queue state.
			   The queue may have changed by an interrup handler.
			   If this is the case roll back and restart. */
			if (__strex(&thinkos_rt.wq_lst[wq], queue)) {
				/* roll back */
#if THINKOS_ENABLE_THREAD_STAT
				thinkos_rt.th_stat[self] = 0;
#endif
				/* insert into the ready wait queue */
				__bit_mem_wr(&thinkos_rt.wq_ready, self, 1);  
				DCC_LOG2(LOG_WARNING, "<%d> rollback 2 %d...", self, wq);
				goto again;
			}
			/* -- wait for event ---------------------------------------- */
			DCC_LOG2(LOG_MSG, "<%d> waiting on console write %d...", 
					 self, wq);
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
	uart->cr3 = USART_ONEBIT | USART_DMAT | USART_DMAR;
	/* Configure 8N1 and line break detection */
	uart->cr2 = USART_STOP_1 |  USART_LINEN | USART_LBDIE;
	/* enable UART and errors interrupt */
	uart->cr1 = USART_UE | USART_RE | USART_TE;

	/* Clear pending TC and BRK interrupts */
	uart->sr = 0;

	/* TX DMA ------------------------------------------------------------- */
	dma->ch[TX_DMA_CHAN].ccr = 0;
	while (dma->ch[TX_DMA_CHAN].ccr & DMA_EN); 
	dma->ch[TX_DMA_CHAN].cpar = &uart->dr;
	dma->ch[TX_DMA_CHAN].cmar = simlnk.tx_buf;
#if ENABLE_SIMLNK_SANITY_CHECK
	dma->ch[TX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
		DMA_MINC | DMA_DIR_MTP | DMA_TEIE | DMA_TCIE;
#else
	dma->ch[TX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
		DMA_MINC | DMA_DIR_MTP;
#endif

	/* RX DMA ------------------------------------------------------------- */
	dma->ch[RX_DMA_CHAN].ccr = 0;
	while (dma->ch[RX_DMA_CHAN].ccr & DMA_EN); 
	dma->ch[RX_DMA_CHAN].cpar = &uart->dr;
	dma->ch[RX_DMA_CHAN].cmar = simlnk.rx_buf;
	dma->ch[RX_DMA_CHAN].cndtr = sizeof(simlnk.rx_buf);
#if ENABLE_SIMLNK_SANITY_CHECK
	dma->ch[RX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
		DMA_MINC | DMA_DIR_PTM | DMA_TEIE | DMA_TCIE | DMA_EN;
#else
	dma->ch[RX_DMA_CHAN].ccr = DMA_MSIZE_8 | DMA_PSIZE_8 | 
		DMA_MINC | DMA_DIR_PTM | DMA_EN;
#endif

#ifdef CM3_RAM_VECTORS
	thinkos_irq_register(STM32_IRQ_USART2, IRQ_PRIORITY_LOW, 
						 stm32_usart2_isr);
#if ENABLE_SIMLNK_SANITY_CHECK
	thinkos_irq_register(STM32_IRQ_DMA1_CH7, IRQ_PRIORITY_LOW, 
						 stm32_dma1_channel7_isr);
	thinkos_irq_register(STM32_IRQ_DMA1_CH6, IRQ_PRIORITY_LOW, 
						 stm32_dma1_channel6_isr);
#endif
#else
	cm3_irq_pri_set(STM32_IRQ_USART2, IRQ_PRIORITY_LOW);
	cm3_irq_enable(STM32_IRQ_USART2);
#if ENABLE_SIMLNK_SANITY_CHECK
	cm3_irq_pri_set(STM32_IRQ_DMA1_CH7, IRQ_PRIORITY_LOW);
	cm3_irq_pri_set(STM32_IRQ_DMA1_CH6, IRQ_PRIORITY_LOW);
	cm3_irq_enable(STM32_IRQ_DMA1_CH7);
	cm3_irq_enable(STM32_IRQ_DMA1_CH6);
	cm3_irq_enable(STM32_IRQ_DMA1_CH6);
#endif
#endif

	DCC_LOG1(LOG_TRACE, "SIMLKN started, baudrate=%d.", 
			 SIMLNK_BAUDRATE); 
	return 0;
}

