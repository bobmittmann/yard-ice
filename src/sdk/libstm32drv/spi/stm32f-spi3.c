/* 
 * File:	 sflash.c
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

#include <sys/stm32f.h>
#include <sys/param.h> /* MIN() */
#include <sys/delay.h>
#include <string.h>
#include <assert.h>
#include <thinkos.h>
#include <stdio.h>


struct spi_dev {
	struct stm32f_spi * spi;
	struct {
		struct stm32_gpio * gpio;
		unsigned int pin;
	} cs; /* Chip select pin */
	unsigned int irqno;
	struct stm32f_dma * dma;
	struct stm32_dmactl tx_dma;
	struct stm32_dmactl rx_dma;
	uint8_t iobuf[4 + SF_PAGE_SIZE];
};

/* ------------------------------------------------------------------------
   SPI3 Serial Flash Instance 
   ------------------------------------------------------------------------- */

#define SPI_DMA_RX_STRM 2
#define SPI_DMA_RX_CHAN 0

#define SPI_DMA_TX_STRM 7
#define SPI_DMA_TX_CHAN 0

void spi_dma_init(struct stm32_dmactl * tx_dma, struct stm32_dmactl * rx_dma)
{
	/* Initialize DMA structures */
	stm32_dmactl_init(&sf->rx_dma, STM32F_DMA1, SPI_DMA_RX_STRM);
	/* Configure RX DMA stream */
	rx_dma->strm->cr = DMA_CHSEL_SET(SPI_DMA_RX_CHAN) |
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_8 | DMA_PSIZE_8 |
		DMA_CT_M0AR | DMA_MINC | DMA_DIR_PTM |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	rx_dma->strm->par = &spi->dr;
	rx_dma->strm->fcr = 0;

	stm32_dmactl_init(&sf->tx_dma, STM32F_DMA1, SPI_DMA_TX_STRM);
	/* Configure TX DMA stream */
	tx_dma->strm->cr = DMA_CHSEL_SET(SPI_DMA_TX_CHAN) |
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_8| DMA_PSIZE_8 |
		DMA_CT_M0AR | DMA_MINC | DMA_DIR_MTP |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	tx_dma->strm->par = &spi->dr;
	tx_dma->strm->fcr = 0;
}

