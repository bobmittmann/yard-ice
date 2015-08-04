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

/* Perform an SPI transfer (send and receive). */
static void spi_io_xfer(struct spi_dev * dev,
					uint8_t txd[], uint8_t rxd[], 
					unsigned int len)
{
	struct stm32f_spi * spi = dev->spi;
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

