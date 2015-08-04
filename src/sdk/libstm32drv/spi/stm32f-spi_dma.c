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
#include <thinkos.h>

/* Perform an SPI transfer (send and receive) using DMA. */
void spi_dma_xfer(struct stm32f_spi * spi,
					 struct stm32_dmactl * tx_dma,
					 struct stm32_dmactl * rx_dma,
					 uint8_t txd[], uint8_t rxd[], 
					 unsigned int len)
{
	/* Disable SPI */
	spi->cr1 &= ~SPI_SPE;
	/* Enable DMA */
	spi->cr2 = SPI_TXDMAEN | SPI_RXDMAEN;

	/* Disable TX DMA */
	tx_dma->strm->cr &= ~DMA_EN;
	while (tx_dma->strm->cr & DMA_EN);
	/* Configure TX DMA transfer */
	tx_dma->strm->m0ar = txd;
	tx_dma->strm->ndtr = len;
	/* Enable TX DMA */
	tx_dma->strm->cr |= DMA_EN;

	/* Disable RX DMA */
	rx_dma->strm->cr &= ~DMA_EN;
	while (rx_dma->strm->cr & DMA_EN);
	/* Configure RX DMA transfer */
	rx_dma->strm->m0ar = rxd;
	rx_dma->strm->ndtr = len;
	/* Enable RX DMA */
	rx_dma->strm->cr |= DMA_EN;

	/* Enable SPI  */
	spi->cr1 |= SPI_SPE;

    /* wait for completion of DMA transfer */
     while (!rx_dma->isr[TCIF_BIT]) {
         if (rx_dma->isr[TEIF_BIT]) {
             /* XXX: DMA transfer error... */
             rx_dma->ifcr[TEIF_BIT] = 1;
         }
         /* wait DMA transfer interrupt */
         thinkos_irq_wait(rx_dma->irqno);
     }

     /* clear the RX DMA transfer complete flag */
     rx_dma->ifcr[TCIF_BIT] = 1;
     /* clear the TX DMA transfer complete flag */
     tx_dma->ifcr[TCIF_BIT] = 1;
}

void spi_dma_send(struct stm32f_spi * spi, struct stm32_dmactl * tx_dma,
					 uint8_t txd[], unsigned int len)
{
	uint32_t isr;

	/* Disable SPI */
	spi->cr1 &= ~SPI_SPE;
	/* Enable DMA */
	spi->cr2 = SPI_TXDMAEN;

	/* Disable TX DMA */
	tx_dma->strm->cr &= ~DMA_EN;
	while (tx_dma->strm->cr & DMA_EN);
	/* Configure TX DMA transfer */
	tx_dma->strm->m0ar = txd;
	tx_dma->strm->ndtr = len;
	/* Enable TX DMA */
	tx_dma->strm->cr |= DMA_EN;

	/* Enable SPI  */
	spi->cr1 |= SPI_SPE;

    /* wait for completion of DMA transfer */
     while (!tx_dma->isr[TCIF_BIT]) {
         if (tx_dma->isr[TEIF_BIT]) {
             /* XXX: DMA transfer error... */
             tx_dma->ifcr[TEIF_BIT] = 1;
         }
         /* wait DMA transfer interrupt */
         thinkos_irq_wait(tx_dma->irqno);
     }

     /* clear the RX DMA transfer complete flag */
     tx_dma->ifcr[TCIF_BIT] = 1;
}

void spi_dma_recv(struct stm32f_spi * spi, struct stm32_dmactl * rx_dma,
				  struct spi_dev * sf, uint8_t rxd[], unsigned int len)
{
	/* Disable SPI */
	spi->cr1 &= ~SPI_SPE;
	/* Enable SPI transfer */
	spi->cr2 = SPI_RXDMAEN;

	/* Disable RX DMA */
	rx_dma->strm->cr &= ~DMA_EN;
	while (rx_dma->strm->cr & DMA_EN);
	/* Configure RX DMA transfer */
	rx_dma->strm->m0ar = rxd;
	rx_dma->strm->ndtr = len;
	/* Enable RX DMA */
	rx_dma->strm->cr |= DMA_EN;

	/* Enable SPI  */
	spi->cr1 |= SPI_SPE;

    /* wait for completion of DMA transfer */
     while (!rx_dma->isr[TCIF_BIT]) {
         if (rx_dma->isr[TEIF_BIT]) {
             /* XXX: DMA transfer error... */
             rx_dma->ifcr[TEIF_BIT] = 1;
         }
         /* wait DMA transfer interrupt */
         thinkos_irq_wait(rx_dma->irqno);
     }

     /* clear the RX DMA transfer complete flag */
     rx_dma->ifcr[TCIF_BIT] = 1;

     /* Disable SPI and clear RXONLY flag */
     spi->cr1 &= ~(SPI_SPE | SPI_RXONLY);
}


