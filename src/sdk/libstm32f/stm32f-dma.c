/* 
 * File:	stm32f-dma.c
 * Author:  Robinson Mittmann (bobmittmann@gmail.com)
 * Target: 
 * Comment: 
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
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


#include <sys/stm32f.h>

#if defined(STM32F2X) || defined(STM32F4X)

const uint8_t stm32f_dma_isr_base_lut[] = {
	     0,      6,      16,      22,
	32 + 0, 32 + 6, 32 + 16, 32 + 22,
};

const uint8_t stm32f_dma1_irqnum_lut[] = {
	STM32F_IRQ_DMA1_STREAM0,
	STM32F_IRQ_DMA1_STREAM1,
	STM32F_IRQ_DMA1_STREAM2,
	STM32F_IRQ_DMA1_STREAM3,
	STM32F_IRQ_DMA1_STREAM4,
	STM32F_IRQ_DMA1_STREAM5,
	STM32F_IRQ_DMA1_STREAM6,
	STM32F_IRQ_DMA1_STREAM7
};

const uint8_t stm32f_dma2_irqnum_lut[] = {
	STM32F_IRQ_DMA2_STREAM0,
	STM32F_IRQ_DMA2_STREAM1,
	STM32F_IRQ_DMA2_STREAM2,
	STM32F_IRQ_DMA2_STREAM3,
	STM32F_IRQ_DMA2_STREAM4,
	STM32F_IRQ_DMA2_STREAM5,
	STM32F_IRQ_DMA2_STREAM6,
	STM32F_IRQ_DMA2_STREAM7
};

#if 0
void stm32f_dmactl_init(struct stm32f_dmactl * ctl, 
						struct stm32f_dma * dma,
						int strm_id)
{
	ctl->strm = &dma->s[strm_id];
	ctl->strm_id = strm_id;
	ctl->isr = dma_isr_bitband(dma, strm_id);
	ctl->ifcr = dma_ifcr_bitband(dma, strm_id);

	/* Disable DMA stream */
	ctl->strm->cr = 0;
	while (ctl->strm->cr & DMA_EN); 

	/* clear all interrupt flags */
	ctl->ifcr[FEIF_BIT] = 1;
	ctl->ifcr[DMEIF_BIT] = 1;
	ctl->ifcr[TEIF_BIT] = 1;
	ctl->ifcr[HTIF_BIT] = 1;
	ctl->ifcr[TCIF_BIT] = 1; 
}
#endif
#endif

#if defined(STM32F1X) || defined(STM32F3X) || defined(STM32L1X)

const uint8_t stm32f_dma1_irqnum_lut[] = {
	STM32F_IRQ_DMA1_STREAM0,
	STM32F_IRQ_DMA1_STREAM1,
	STM32F_IRQ_DMA1_STREAM2,
	STM32F_IRQ_DMA1_STREAM3,
	STM32F_IRQ_DMA1_STREAM4,
	STM32F_IRQ_DMA1_STREAM5,
	STM32F_IRQ_DMA1_STREAM6
};

const uint8_t stm32f_dma2_irqnum_lut[] = {
	STM32F_IRQ_DMA2_STREAM0,
	STM32F_IRQ_DMA2_STREAM1,
	STM32F_IRQ_DMA2_STREAM2,
	STM32F_IRQ_DMA2_STREAM3,
#ifdef STM32F_IRQ_DMA2_STREAM4
	STM32F_IRQ_DMA2_STREAM4,
#endif
#ifdef STM32F_IRQ_DMA2_STREAM5
	STM32F_IRQ_DMA2_STREAM5,
#endif
#ifdef STM32F_IRQ_DMA2_STREAM6
	STM32F_IRQ_DMA2_STREAM6
#endif
};

#endif

