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

#include "board.h"
#include "sflash.h"

/* -------------------------------------------------------------------------
   Serial Flash Basic Commands
   -------------------------------------------------------------------------- */

/* RESET Operations */
#define SF_RESET_ENABLE         0x66
#define SF_RESET_MEMORY         0x99

/* IDENTIFICATION Operations */
#define SF_READ_ID              0x9e

/* READ Operations */
#define SF_READ                 0x03
#define SF_FAST_READ            0x0b

/* WRITE Operations */
#define SF_WRITE_ENABLE         0x06
#define SF_WRITE_DISABLE 	    0x04

/* REGISTER Operations */
#define SF_STATUS_REG_RD        0x05
#define SF_STATUS_REG_WR        0x01
#define SF_FLAG_STAT_REG_RD     0x70
#define SF_FLAG_STAT_REG_CLR    0x50

/* PROGRAM Operations */
#define SF_PAGE_PROGRAM         0x02

/* ERASE Operations */
#define SF_SUBSECTOR_ERASE      0x20

/* -------------------------------------------------------------------------
   Serial Flash Elementary Driver
   -------------------------------------------------------------------------- */

struct sflash_dev {
	struct stm32f_spi * spi;
	struct {
		struct stm32_gpio * gpio;
		unsigned int pin;
	} cs;
	unsigned int irqno;
	struct stm32f_dma * dma;
	struct stm32_dmactl tx_dma;
	struct stm32_dmactl rx_dma;
	uint8_t iobuf[4 + 256];
};

static inline void sflash_cs_set(struct sflash_dev * sf) {
	stm32_gpio_set(sf->cs.gpio, sf->cs.pin);
}

static inline void sflash_cs_clr(struct sflash_dev * sf) {
	stm32_gpio_clr(sf->cs.gpio, sf->cs.pin);
}

static void sflash_io_xfer(struct sflash_dev * sf,
					uint8_t txd[], uint8_t rxd[], 
					unsigned int len)
{
	struct stm32f_spi * spi = sf->spi;
	unsigned int dummy;
	unsigned int sr;
	unsigned int i;

	/* Disable SPI */
	spi->cr1 &= ~SPI_SPE;
	/* Enable RX interrupt */
	spi->cr2 = SPI_RXNEIE;

	while ((sr = spi->sr) & SPI_RXNE) {
		/* clear input buffer */
		dummy = spi->dr;
		(void)dummy;
		sr = spi->sr;
	}
	
	/* Enable SPI */
	spi->cr1 |= SPI_SPE;

	for (i = 0; i < len; ++i) {
		/* send dummy data */
		spi->dr = txd[i];
		/* wait for incoming data */
		while (!((sr = spi->sr) & SPI_RXNE))
			thinkos_irq_wait(sf->irqno);
		rxd[i] = spi->dr;
	}
}

void sflash_dma_xfer(struct sflash_dev * sf,
					 uint8_t txd[], uint8_t rxd[], 
					 unsigned int len)
{
	struct stm32f_spi * spi = sf->spi;
//	struct stm32_dmactl * rx_dma = &sf->rx_dma;
//	struct stm32_dmactl * tx_dma = &sf->tx_dma;

	/* Disable SPI */
	spi->cr1 &= ~SPI_SPE;
	/* Enable DMA */
	spi->cr2 = SPI_TXDMAEN | SPI_RXDMAEN;

	/* Disable TX DMA */
	sf->tx_dma.strm->cr &= ~DMA_EN;
	while (sf->tx_dma.strm->cr & DMA_EN);
	/* Configure TX DMA transfer */
	sf->tx_dma.strm->m0ar = txd;
	sf->tx_dma.strm->ndtr = len;
	/* Enable TX DMA */
	sf->tx_dma.strm->cr |= DMA_EN;

	/* Disable RX DMA */
	sf->rx_dma.strm->cr &= ~DMA_EN;
	while (sf->rx_dma.strm->cr & DMA_EN);
	/* Configure RX DMA transfer */
	sf->rx_dma.strm->m0ar = rxd;
	sf->rx_dma.strm->ndtr = len;
	/* Enable RX DMA */
	sf->rx_dma.strm->cr |= DMA_EN;

	/* Enable SPI  */
	spi->cr1 |= SPI_SPE;

    /* wait for completion of DMA transfer */
     while (!sf->rx_dma.isr[TCIF_BIT]) {
#if 0
         if (sf->rx_dma.isr[TEIF_BIT]) {
             /* XXX: DMA transfer error... */
             sf->rx_dma.ifcr[TEIF_BIT] = 1;
         }

         if (sf->rx_dma.isr[FEIF_BIT]) {
             /* XXX: DMA fifo error... */
             sf->rx_dma.ifcr[FEIF_BIT] = 1;
         }

         if (sf->rx_dma.isr[HTIF_BIT]) {
        	 /* XXX: DMA half transfer... */
        	 sf->rx_dma.ifcr[HTIF_BIT] = 1;
         }
#endif
         /* wait DMA transfer interrupt */
         thinkos_irq_wait(sf->rx_dma.irqno);
     }

     /* clear the RX DMA transfer complete flag */
     sf->rx_dma.ifcr[TCIF_BIT] = 1;
     /* clear the TX DMA transfer complete flag */
     sf->tx_dma.ifcr[TCIF_BIT] = 1;
}

void sflash_dma_send(struct sflash_dev * sf, uint8_t txd[], unsigned int len)
{
	struct stm32f_spi * spi = sf->spi;
	struct stm32f_dma * dma = STM32F_DMA1;
	uint32_t isr;

    /* configure and Enable interrupt */
//    thinkos_irq_register(STM32F_IRQ_DMA1_STREAM7, IRQ_PRIORITY_REGULAR,
//    		stm32f_dma1_stream7_isr);

	/* Disable SPI */
	spi->cr1 &= ~SPI_SPE;
	/* Enable DMA */
	spi->cr2 = SPI_TXDMAEN;

	/* Disable TX DMA */
	sf->tx_dma.strm->cr &= ~DMA_EN;
	while (sf->tx_dma.strm->cr & DMA_EN);
	/* Configure TX DMA transfer */
	sf->tx_dma.strm->m0ar = txd;
	sf->tx_dma.strm->ndtr = len;
	/* Enable TX DMA */
	sf->tx_dma.strm->cr |= DMA_EN;

	/* Enable SPI  */
	spi->cr1 |= SPI_SPE;

    /* wait for completion of DMA transfer */
     while (!sf->tx_dma.isr[TCIF_BIT]) {
         if (sf->tx_dma.isr[TEIF_BIT]) {
             /* XXX: DMA transfer error... */
             sf->tx_dma.ifcr[TEIF_BIT] = 1;
         }
         /* wait DMA transfer interrupt */
         thinkos_irq_wait(sf->tx_dma.irqno);
     }

     /* clear the RX DMA transfer complete flag */
     sf->tx_dma.ifcr[TCIF_BIT] = 1;
}


void _sflash_dma_xfer(struct sflash_dev * sf,
					 uint8_t txd[], uint8_t rxd[],
					 unsigned int len)
{
	struct stm32f_spi * spi = sf->spi;
//	struct stm32_dmactl * rx_dma = &sf->rx_dma;
//	struct stm32_dmactl * tx_dma = &sf->tx_dma;

	/* Disable SPI */
	spi->cr1 &= ~SPI_SPE;
	/* Enable DMA */
	spi->cr2 = SPI_TXDMAEN | SPI_RXDMAEN;

	/* Disable TX DMA */
	sf->tx_dma.strm->cr = 0;
	while (sf->tx_dma.strm->cr & DMA_EN);

	/* Disable RX DMA */
	sf->rx_dma.strm->cr = 0;
	while (sf->rx_dma.strm->cr & DMA_EN);

	/* Configure TX DMA stream */
	sf->tx_dma.strm->cr = DMA_CHSEL_SET(0) |
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_8| DMA_PSIZE_8 |
		DMA_CT_M0AR | DMA_MINC | DMA_DIR_MTP |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	/* Configure TX DMA transfer */
	sf->tx_dma.strm->m0ar = txd;
	sf->tx_dma.strm->ndtr = len;
	/* Enable TX DMA */
	sf->tx_dma.strm->cr |= DMA_EN;

	/* Configure RX DMA stream */
	sf->rx_dma.strm->cr = DMA_CHSEL_SET(0) |
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_8 | DMA_PSIZE_8 |
		DMA_CT_M0AR | DMA_MINC | DMA_DIR_PTM |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	/* Configure RX DMA transfer */
	sf->rx_dma.strm->m0ar = rxd;
	sf->rx_dma.strm->ndtr = len;
	/* Enable RX DMA */
	sf->rx_dma.strm->cr |= DMA_EN;

	/* Enable SPI  */
	spi->cr1 |= SPI_SPE;

    /* wait for completion of DMA transfer */
     while (!sf->rx_dma.isr[TCIF_BIT]) {
         /* wait DMA transfer interrupt */
//         thinkos_irq_wait(sf->rx_dma.irqno);
     }

     /* clear the RX DMA transfer complete flag */
     sf->rx_dma.ifcr[TCIF_BIT] = 1;
     /* clear the TX DMA transfer complete flag */
     sf->tx_dma.ifcr[TCIF_BIT] = 1;
}



#define SPI_DMA_RX_STRM 2
#define SPI_DMA_RX_CHAN 0
#define SPI_DMA_RX_IRQ STM32F_IRQ_DMA1_STREAM2

#define SPI_DMA_TX_STRM 7
#define SPI_DMA_TX_CHAN 0
#define SPI_DMA_TX_IRQ STM32F_IRQ_DMA1_STREAM7

void __sflash_dma_xfer(struct sflash_dev * sf,
					 uint8_t txd[], uint8_t rxd[],
					 unsigned int len)
{
	struct stm32f_spi * spi = sf->spi;
	struct stm32f_dma * dma = STM32F_DMA1;
	uint32_t isr;
	uint32_t sr;

	/* Disable SPI */
	spi->cr1 &= ~SPI_SPE;
	spi->cr2 = SPI_TXDMAEN | SPI_RXDMAEN;

	/* Disable DMA channel */
	dma->s[SPI_DMA_RX_STRM].cr = 0;
	while (dma->s[SPI_DMA_RX_STRM].cr & DMA_EN);

	dma->s[SPI_DMA_TX_STRM].cr = 0;
	while (dma->s[SPI_DMA_TX_STRM].cr & DMA_EN);

	dma->lifcr = DMA_TCIF2 | DMA_CHTIF2 | DMA_CTEIF2 | DMA_CDMEIF2 | DMA_CFEIF2;
	dma->hifcr = DMA_TCIF5 | DMA_CHTIF5 | DMA_CTEIF5 | DMA_CDMEIF5 | DMA_CFEIF5;

	/* Configure DMA channel */
	dma->s[SPI_DMA_RX_STRM].cr = DMA_CHSEL_SET(SPI_DMA_RX_CHAN) |
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_8 | DMA_PSIZE_8 |
		DMA_CT_M0AR | DMA_MINC | DMA_DIR_PTM |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	dma->s[SPI_DMA_RX_STRM].par = &spi->dr;
	dma->s[SPI_DMA_RX_STRM].m0ar = rxd;
	dma->s[SPI_DMA_RX_STRM].ndtr = len;
	dma->s[SPI_DMA_RX_STRM].fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;

	dma->s[SPI_DMA_TX_STRM].cr = DMA_CHSEL_SET(SPI_DMA_TX_CHAN) |
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_8 | DMA_PSIZE_8 |
		DMA_CT_M0AR | DMA_MINC | DMA_DIR_MTP |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	dma->s[SPI_DMA_TX_STRM].par = &spi->dr;
	dma->s[SPI_DMA_TX_STRM].m0ar = txd;
	dma->s[SPI_DMA_TX_STRM].ndtr = len;
	dma->s[SPI_DMA_TX_STRM].fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;

	/* enable DMA */
	dma->s[SPI_DMA_TX_STRM].cr |= DMA_EN;
	dma->s[SPI_DMA_RX_STRM].cr |= DMA_EN;

	/* Enable SPI transfer */
	spi->cr1 |= SPI_SPE;

	do {
		isr = dma->lisr;
		if (isr & DMA_TCIF2) {
			/* clear the DMA transfer complete flag */
			dma->lifcr = DMA_CTCIF2;
			break;
		}
		if (isr & (DMA_TEIF2 | DMA_DMEIF2 | DMA_FEIF2)) {
			dma->lifcr = DMA_CHTIF2 | DMA_CTEIF2 | DMA_CDMEIF2 | DMA_CFEIF2;
			fprintf(stderr, "RX DMA err:%s%s%s\n",
					isr & DMA_TEIF2 ? " TEIF" : "",
					isr & DMA_DMEIF2 ? " DMEIF" : "",
					isr & DMA_FEIF2 ? " FEIF" : "");
			break;
		}
		isr = dma->hisr;
		if (isr & DMA_TCIF7) {
			dma->hifcr = DMA_CTCIF7;
			break;
		}
		if (isr & (DMA_TEIF7 | DMA_DMEIF7 | DMA_FEIF7)) {
			fprintf(stderr, "TX DMA err:%s%s%s\n",
					isr & DMA_TEIF7 ? " TEIF" : "",
					isr & DMA_DMEIF7 ? " DMEIF" : "",
					isr & DMA_FEIF7 ? " FEIF" : "");
			dma->hifcr = DMA_CHTIF7 | DMA_CTEIF7 | DMA_CDMEIF7 | DMA_CFEIF7;
			break;
		}
	} while (1);

//	thinkos_irq_wait(SPI_DMA_RX_IRQ);

}


volatile uint32_t dma_tx_isr = 0;

/* TX DMA IRQ */
void stm32f_dma1_stream7_isr(void)
{
    struct stm32f_dma * dma = STM32F_DMA1;
    uint32_t isr;

    dma_tx_isr = isr = dma->hisr;

    if (isr & DMA_FEIF7) {
        dma->hifcr = DMA_CFEIF7;
    }

    if (isr & DMA_TEIF7) {
        dma->hifcr = DMA_CTEIF7;
    }

    if (isr & DMA_TCIF7) {
        /* clear the DMA transfer complete flag */
        dma->hifcr = DMA_CTCIF7;
    }
}

void __sflash_dma_send(struct sflash_dev * sf, uint8_t txd[], unsigned int len)
{
	struct stm32f_spi * spi = sf->spi;
	struct stm32f_dma * dma = STM32F_DMA1;
	uint32_t isr;

    /* configure and Enable interrupt */
//    thinkos_irq_register(STM32F_IRQ_DMA1_STREAM7, IRQ_PRIORITY_REGULAR,
//    		stm32f_dma1_stream7_isr);

	/* Disable SPI */
	spi->cr1 &= ~SPI_SPE;
	spi->cr2 = SPI_TXDMAEN;

	dma->s[SPI_DMA_TX_STRM].cr = 0;
	while (dma->s[SPI_DMA_TX_STRM].cr & DMA_EN);

	dma->hifcr = DMA_CHTIF7 | DMA_CTEIF7 | DMA_CDMEIF7 | DMA_CFEIF7;

	dma->s[SPI_DMA_TX_STRM].cr = DMA_CHSEL_SET(SPI_DMA_TX_CHAN) |
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_8 | DMA_PSIZE_8 |
		DMA_MINC | DMA_DIR_MTP |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	dma->s[SPI_DMA_TX_STRM].par = &spi->dr;
	dma->s[SPI_DMA_TX_STRM].m0ar = txd;
	dma->s[SPI_DMA_TX_STRM].ndtr = len;
	dma->s[SPI_DMA_TX_STRM].fcr = 0;

	/* enable DMA */
	dma->s[SPI_DMA_TX_STRM].cr |= DMA_EN;

	/* Enable SPI transfer */
	spi->cr1 |= SPI_SPE;

	do {
		isr = dma->hisr;
		if (isr & DMA_TCIF7) {
			/* clear the DMA transfer complete flag */
			dma->hifcr = DMA_CTCIF7;
			break;
		}
		if (isr & (DMA_TEIF7 | DMA_DMEIF7)) {
			fprintf(stderr, "TX DMA err:%s%s%s\n",
					isr & DMA_TEIF7 ? " TEIF" : "",
					isr & DMA_DMEIF7 ? " DMEIF" : "",
					isr & DMA_FEIF7 ? " FEIF" : "");
			dma->hifcr = DMA_CHTIF7 | DMA_CTEIF7 | DMA_CDMEIF7 | DMA_CFEIF7;
//			break;
		}

		thinkos_irq_wait(SPI_DMA_TX_IRQ);
	} while (1);

}

void sflash_dma_recv(struct sflash_dev * sf, uint8_t rxd[], unsigned int len)
{
	struct stm32f_spi * spi = sf->spi;
	struct stm32f_dma * dma = STM32F_DMA1;
	uint32_t isr;

	/* Disable SPI */
	spi->cr1 &= ~SPI_SPE;
	/* Enable SPI transfer */
	spi->cr2 = SPI_RXDMAEN;

	while (spi->sr & SPI_RXNE) {
		/* clear input buffer */
		uint32_t dummy = spi->dr;
		(void)dummy;
	}

	/* Disable DMA channel */
	dma->s[SPI_DMA_RX_STRM].cr = 0;
	while (dma->s[SPI_DMA_RX_STRM].cr & DMA_EN);

	/* Configure DMA channel */
	dma->s[SPI_DMA_RX_STRM].cr = DMA_CHSEL_SET(SPI_DMA_RX_CHAN) |
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_8 | DMA_PSIZE_8 |
		DMA_MINC | DMA_DIR_PTM |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	dma->s[SPI_DMA_RX_STRM].par = &spi->dr;
	dma->s[SPI_DMA_RX_STRM].m0ar = rxd;
	dma->s[SPI_DMA_RX_STRM].ndtr = len;
//	dma->s[SPI_DMA_RX_STRM].fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;
	dma->s[SPI_DMA_RX_STRM].fcr = 0;
	/* enable DMA */
	dma->s[SPI_DMA_RX_STRM].cr |= DMA_EN;

	spi->cr1 |= SPI_SPE | SPI_RXONLY;

	do {
		isr = dma->lisr;
		if (isr & DMA_TCIF2) {
			/* clear the DMA transfer complete flag */
			dma->lifcr = DMA_CTCIF2;
			break;
		}
		if (isr & (DMA_TEIF2 | DMA_DMEIF2 | DMA_FEIF2)) {
			dma->lifcr = DMA_CHTIF2 | DMA_CTEIF2 | DMA_CDMEIF2 | DMA_CFEIF2;
			fprintf(stderr, "RX DMA err:%s%s%s\n",
					isr & DMA_TEIF2 ? " TEIF" : "",
					isr & DMA_DMEIF2 ? " DMEIF" : "",
					isr & DMA_FEIF2 ? " FEIF" : "");
//			break;
		}

		thinkos_irq_wait(SPI_DMA_RX_IRQ);
	} while (1);

	spi->cr1 &= ~(SPI_SPE | SPI_RXONLY);
}

/* ------------------------------------------------------------------------
   Serial Flash Public API 
   ------------------------------------------------------------------------- */

int sflash_reset(struct sflash_dev * sf)
{
	uint8_t data[1];
	assert(sf != NULL);

	sflash_cs_clr(sf);
	data[0] = SF_RESET_ENABLE;
	sflash_io_xfer(sf, data, data, 1);
	sflash_cs_set(sf);

	udelay(1);

	sflash_cs_clr(sf);
	data[0] = SF_RESET_MEMORY;
	sflash_io_xfer(sf, data, data, 1);
	sflash_cs_set(sf);

	udelay(1);

	return 0;
}

int sflash_device_id(struct sflash_dev * sf, void * buf, size_t len)
{
	unsigned int cnt;
	uint8_t * data = sf->iobuf;

	assert(sf != NULL);
	assert(buf != NULL);
	assert(len > 0);

	cnt = MIN(20, len);	

	/* chip select low */
	sflash_cs_clr(sf);
	data[0] = SF_READ_ID;
	sflash_dma_xfer(sf, data, data, cnt + 1);
//	sflash_dma_send(sf, data, 1);
//	sflash_dma_recv(sf, &data[1], cnt);
	/* chip select high */
	sflash_cs_set(sf);

	memcpy(buf, &data[1], cnt);

	return cnt;
}

int sflash_write_enable(struct sflash_dev * sf)
{
	uint8_t cmd[1];

	assert(sf != NULL);

	/* chip select low */
	sflash_cs_clr(sf);
	/* send the command */
	cmd[0] = SF_WRITE_ENABLE;
	sflash_io_xfer(sf, cmd, cmd, 1);
	/* chip select high */
	sflash_cs_set(sf);

	return 0;
}

int sflash_read_status(struct sflash_dev * sf)
{
	uint8_t buf[2];

	assert(sf != NULL);

	/* chip select low */
	sflash_cs_clr(sf);
	/* send the command */
	buf[0] = SF_STATUS_REG_RD;
	sflash_io_xfer(sf, buf, buf, 2);
	/* chip select high */
	sflash_cs_set(sf);

	return buf[1];
}

int sflash_flag_status_read(struct sflash_dev * sf)
{
	uint8_t buf[2];

	assert(sf != NULL);

	/* chip select low */
	sflash_cs_clr(sf);
	/* send the command */
	buf[0] = SF_FLAG_STAT_REG_RD;
	sflash_io_xfer(sf, buf, buf, 2);
	/* chip select high */
	sflash_cs_set(sf);

	return buf[1];
}

int sflash_sector_erase(struct sflash_dev * sf, uint32_t addr)
{
	uint8_t buf[4];

	assert(sf != NULL);

	buf[0] = SF_SUBSECTOR_ERASE;
	buf[1] = (addr >> 16) & 0xff;
	buf[2] = (addr >> 8) & 0xff;
	buf[3] = addr & 0xff;

	/* chip select low */
	sflash_cs_clr(sf);

	/* send the command */
	sflash_io_xfer(sf, buf, buf, 4);

	/* chip select high */
	sflash_cs_set(sf);

	return 0;
}

int sflash_page_write(struct sflash_dev * sf, uint32_t addr,
					  const void * buf, size_t count)
{
	uint8_t * data = (uint8_t *)buf;
	uint8_t cmd[4];

	assert(sf != NULL);
	assert(buf != NULL);
	assert(count > 0);

	/* prepare the command */
	cmd[0] = SF_PAGE_PROGRAM;
	cmd[1] = (addr >> 16) & 0xff;
	cmd[2] = (addr >> 8) & 0xff;
	cmd[3] = addr & 0xff;

	/* chip select low */
	sflash_cs_clr(sf);
	/* send command */
	sflash_io_xfer(sf, cmd, cmd, 4);
	/* send data */
	sflash_io_xfer(sf, data, data, count);
	/* chip select high */
	sflash_cs_set(sf);

	return count;
}

int sflash_page_read(struct sflash_dev * sf, uint32_t addr,
					 void * buf, size_t count)
{
	uint8_t * data = (uint8_t *)buf;
	uint8_t cmd[4];

	assert(sf != NULL);
	assert(buf != NULL);
	assert(count > 0);

	/* prepare the command */
	cmd[0] = SF_READ;
	cmd[1] = (addr >> 16) & 0xff;
	cmd[2] = (addr >> 8) & 0xff;
	cmd[3] = addr & 0xff;

	/* chip select low */
	sflash_cs_clr(sf);
	/* send the command */
	sflash_io_xfer(sf, cmd, cmd, 4);
	/* receive the data */
	sflash_io_xfer(sf, data, data, count);
	/* chip select high */
	sflash_cs_set(sf);

	return count;
}

int sflash_spi_fush(struct sflash_dev * sf)
{
	struct stm32f_spi * spi;
	unsigned int dummy;
	unsigned int sr;

	assert(sf != NULL);
	spi = sf->spi;

	while (!((sr = spi->sr) & SPI_TXE))
		thinkos_irq_wait(sf->irqno);

	if (sr & SPI_MODF)
		return -2;

	if (sr & SPI_RXNE) {
		/* clear input buffer */
		dummy = sf->spi->dr;
		(void)dummy;
	}

	return 0;
}

/* ------------------------------------------------------------------------
   SPI3 Serial Flash Instance 
   ------------------------------------------------------------------------- */

#define SPI_DMA_RX_STRM 2
#define SPI_DMA_RX_CHAN 0

//#define SPI_DMA_TX_STRM 5
#define SPI_DMA_TX_CHAN 0

struct sflash_dev * sflash_init(void)
{
	static struct sflash_dev sflash_dev_singleton;
	struct sflash_dev * sf = &sflash_dev_singleton;
	struct stm32f_spi * spi = STM32F_SPI3;
	unsigned int freq = 100000;
	unsigned int div;
	unsigned int br;

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);

	/* Configure GPIO */
	stm32_gpio_mode(IO_SFLASH_CS, OUTPUT, SPEED_HIGH);
	stm32_gpio_set(IO_SFLASH_CS);

	stm32_gpio_mode(IO_SFLASH_SCK, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_SFLASH_SCK, GPIO_AF6);

	stm32_gpio_mode(IO_SFLASH_MISO, ALT_FUNC, PULL_UP);
	stm32_gpio_af(IO_SFLASH_MISO, GPIO_AF6);

	stm32_gpio_mode(IO_SFLASH_MOSI, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(IO_SFLASH_MOSI, GPIO_AF6);

	/* SPI clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_SPI3);

	/* Compute the baudrate field */
	div = stm32_clk_hz(STM32_CLK_SPI3) / freq / 2;
	br = 31 - __clz(div);
	if (div > (1 << br))
		br++;

	/* Configure SPI: Master mode, MSB first */
	spi->cr1 = SPI_BR_SET(br) | SPI_MSTR | SPI_SSM | SPI_SSI;
	spi->cr2 = 0;
	spi->i2scfgr = 0;
	spi->i2spr = 0;

	/* Set driver's SPI related fields */
	sf->spi = spi;
	sf->cs.gpio = stm32_io_gpio(IO_SFLASH_CS);
	sf->cs.pin = stm32_io_pin(IO_SFLASH_CS);
	sf->irqno = STM32F_IRQ_SPI3;

	/* Initialize DMA structures */

	stm32_dmactl_init(&sf->rx_dma, STM32F_DMA1, SPI_DMA_RX_STRM);
	/* Configure RX DMA stream */
	sf->rx_dma.strm->cr = DMA_CHSEL_SET(SPI_DMA_RX_CHAN) |
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_8 | DMA_PSIZE_8 |
		DMA_CT_M0AR | DMA_MINC | DMA_DIR_PTM |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	sf->rx_dma.strm->par = &spi->dr;
//	sf->rx_dma.strm->fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;
	sf->rx_dma.strm->fcr = 0;

	stm32_dmactl_init(&sf->tx_dma, STM32F_DMA1, SPI_DMA_TX_STRM);
	/* Configure TX DMA stream */
	sf->tx_dma.strm->cr = DMA_CHSEL_SET(SPI_DMA_TX_CHAN) |
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_8| DMA_PSIZE_8 |
		DMA_CT_M0AR | DMA_MINC | DMA_DIR_MTP |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	sf->tx_dma.strm->par = &spi->dr;
//	sf->tx_dma.strm->fcr = DMA_FEIE | DMA_DMDIS | DMA_FTH_FULL;
	sf->tx_dma.strm->fcr = 0;

	/* Return this driver's instance */
	return &sflash_dev_singleton;
}
