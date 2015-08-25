/* 
 * Copyright(C) 2015 Robinson Mittmann. All Rights Reserved.
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
 * @file sflash.c
 * @brief Simple serial flash driver
 * @author Robinson Mittmann <bobmittmann@gmail.com>
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

#ifndef SFLASH_ENABLE_GPIO_INIT
#define SFLASH_ENABLE_GPIO_INIT 1
#endif

/* -------------------------------------------------------------------------
   Serial Flash Basic Commands
   -------------------------------------------------------------------------- */

/* RESET Operations */
#define SF_RESET_ENABLE         0x66
#define SF_RESET_MEMORY         0x99

/* IDENTIFICATION Operations */
#define SF_READ_ID              0x9f

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
#define SF_SECTOR_ERASE         0xd8
#define SF_DIE_ERASE            0xc4
#define SF_BULK_ERASE           0xc7

#define SF_PROGRAM_RESUME       0x7A
#define SF_PROGRAM_SUSPEND      0x75

/* 4-BYTE ADDRESS MODE Operations */
#define SF_4BYTE_ADDR_ENTER     0xb7
#define SF_4BYTE_ADDR_EXIT      0xe9

/* Status Register Bit Definitions
	7 Status register write enable/disable
	0 = Enabled
	1 = Disabled
	Nonvolatile bit: Used with the W# signal to enable or disable writing 
	to the status register.

	5 Top/bottom
	0 = Top
	1 = Bottom
	Nonvolatile bit: Determines whether the protected memory area defined 
	by the block protect bits starts from the top or bottom of the memory array.

	6, 4:2 Block protect 3–0
	See Protected Area Sizes – Upper Area and Lower Area tables in Device 
	Protection
	Nonvolatile bit: Defines memory to be software protected against PROGRAM 
	or ERASE operations. When one or more block protect bits is set to 1, a 
	designated memory area is protected from PROGRAM and ERASE operations.

	1 Write enable latch
	0 = Cleared (Default)
	1 = Set
	Volatile bit: The device always powers up with this bit cleared to 
	prevent inadvertent WRITE STATUS REGISTER, PROGRAM, or ERASE operations.
	To enable these operations, the WRITE ENABLE operation must be executed 
	first to set this bit.

	0 Write in progress
	0 = Ready
	1 = Busy
	Volatile bit: Indicates if one of the following command cycles is in 
	progress: WRITE STATUS REGISTER WRITE NONVOLATILE CONFIGURATION REGISTER 
	PROGRAM ERASE
*/

#define SF_STATUS_BUSY       (1 << 0)
#define SF_STATUS_WEN_LATCH  (1 << 1)

/*  Flag Status Register Bit Definitions

 	7 Program or erase controller
	0 = Busy
	1 = Ready
	Status bit: Indicates whether one of the following command cycles is in 
	progress: WRITE STATUS REGISTER, WRITE NONVOLATILE CONFIGURATION REGISTER, 
	PROGRAM, or ERASE.

	6 Erase suspend
	0 = Not in effect
	1 = In effect
	Status bit: Indicates whether an ERASE operation has been or is 
	going to be suspended.

	5 Erase
	0 = Clear
	1 = Failure or protection error
	Error bit: Indicates whether an ERASE operation has succeeded or failed.

	4 Program
	0 = Clear
	1 = Failure or protection error
	Error bit: Indicates whether a PROGRAM operation has succeeded or failed; 
	also an attempt to program a 0 to a 1 when VPP = VPPH and the data 
	pattern is a multiple of 64 bits.

	3 VPP 0 = Enabled
	1 = Disabled (Default)
	Error bit: Indicates an invalid voltage on VPP during a PROGRAM or 
	ERASE operation.

	2 Program suspend
	0 = Not in effect
	1 = In effect
	Status bit: Indicates whether a PROGRAM operation has been or is going 
	to be suspended.

	1 Protection
	0 = Clear
	1 = Failure or protection error
	Error bit: Indicates whether an ERASE or PROGRAM operation has attempted 
	to modify the protected array sector, or whether a PROGRAM operation has 
	attempted to access the locked OTP space.
 */

#define SF_FLAG_READY  (1 << 7)

/* -------------------------------------------------------------------------
   Serial Flash Elementary Driver
   -------------------------------------------------------------------------- */

#define SF_PAGE_SIZE        256
#define SF_SECTOR_SIZE    65536
#define SF_SUBSECTOR_SIZE  4096
#define SF_SECTOR_COUNT    1024

struct sflash_dev {
	int mutex;
	struct stm32f_spi * spi;
	struct {
		struct stm32_gpio * gpio;
		unsigned int pin;
	} cs; /* Chip select pin */
	unsigned int irqno;
	struct stm32f_dma * dma;
	struct stm32_dmactl tx_dma;
	struct stm32_dmactl rx_dma;
	struct {
		bool valid;
		bool sync;
		uint32_t addr;
		uint8_t cmd[4];
		uint8_t data[SF_PAGE_SIZE];
	} buf; /* DMA buffer */
	uint32_t io_addr; /* Read/Write address */
};

/* Chip select high */
static inline void sflash_cs_set(struct sflash_dev * sf) {
	stm32_gpio_set(sf->cs.gpio, sf->cs.pin);
}

/* Chip select low */
static inline void sflash_cs_clr(struct sflash_dev * sf) {
	stm32_gpio_clr(sf->cs.gpio, sf->cs.pin);
}

/* Perform an SPI transfer (send and receive) using DMA. */
static void sflash_dma_xfer(struct sflash_dev * sf,
							uint8_t txd[], uint8_t rxd[], 
							unsigned int len)
{
	struct stm32f_spi * spi = sf->spi;

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
		if (sf->rx_dma.isr[TEIF_BIT]) {
			/* FIXME: DMA transfer error handling... */
			sf->rx_dma.ifcr[TEIF_BIT] = 1;
		}
		/* wait DMA transfer interrupt */
		thinkos_irq_wait(sf->rx_dma.irqno);
	}

	/* clear the RX DMA transfer complete flag */
	sf->rx_dma.ifcr[TCIF_BIT] = 1;
	/* clear the TX DMA transfer complete flag */
	sf->tx_dma.ifcr[TCIF_BIT] = 1;
}

void sflash_io_xfer(struct sflash_dev * sf,
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
/* ------------------------------------------------------------------------
   Serial Flash Command Level private API
   ------------------------------------------------------------------------- */

static int sf_reset(struct sflash_dev * sf)
{
	uint8_t * cmd = sf->buf.cmd;

	sflash_cs_clr(sf);
	cmd[0] = SF_RESET_ENABLE;
	sflash_dma_xfer(sf, cmd, cmd, 1);
	sflash_cs_set(sf);

	udelay(10);

	sflash_cs_clr(sf);
	cmd[0] = SF_RESET_MEMORY;
	sflash_dma_xfer(sf, cmd, cmd, 1);
	sflash_cs_set(sf);

	udelay(10);

	return 0;
}

static int sf_device_id(struct sflash_dev * sf)
{
	uint8_t * cmd = &sf->buf.cmd[3];

	/* chip select low */
	sflash_cs_clr(sf);
	cmd[0] = SF_READ_ID;
	sflash_dma_xfer(sf, cmd, cmd, 21);
	/* chip select high */
	sflash_cs_set(sf);

	return 0;
}

static int sf_write_enable(struct sflash_dev * sf)
{
	uint8_t * cmd = sf->buf.cmd;

	/* chip select low */
	sflash_cs_clr(sf);
	/* send the command */
	cmd[0] = SF_WRITE_ENABLE;
	sflash_dma_xfer(sf, cmd, cmd, 1);
	/* chip select high */
	sflash_cs_set(sf);

	return 0;
}

static int sf_read_status(struct sflash_dev * sf)
{
	uint8_t * data = sf->buf.cmd;

	/* chip select low */
	sflash_cs_clr(sf);
	/* send the command */
	data[0] = SF_STATUS_REG_RD;
	sflash_dma_xfer(sf, data, data, 2);
	/* chip select high */
	sflash_cs_set(sf);

	return data[1];
}

static int sf_flag_status_read(struct sflash_dev * sf)
{
	uint8_t * data = sf->buf.cmd;

	/* chip select low */
	sflash_cs_clr(sf);
	/* send the command */
	data[0] = SF_FLAG_STAT_REG_RD;
	sflash_dma_xfer(sf, data, data, 2);
	/* chip select high */
	sflash_cs_set(sf);

	return data[1];
}

static void sf_flag_status_clear(struct sflash_dev * sf)
{
	uint8_t * cmd = sf->buf.cmd;

	/* chip select low */
	sflash_cs_clr(sf);
	/* send the command */
	cmd[0] = SF_FLAG_STAT_REG_CLR;
	sflash_dma_xfer(sf, cmd, cmd, 1);
	/* chip select high */
	sflash_cs_set(sf);
}

static int sf_subsector_erase(struct sflash_dev * sf, uint32_t addr)
{
	uint8_t * cmd = sf->buf.cmd;

	/* Address should be subsector aligned */
	assert((addr & (SF_SUBSECTOR_SIZE - 1)) == 0);

	cmd[0] = SF_SUBSECTOR_ERASE;
	cmd[1] = (addr >> 16) & 0xff;
	cmd[2] = (addr >> 8) & 0xff;
	cmd[3] = addr & 0xff;

	/* chip select low */
	sflash_cs_clr(sf);
	/* send the command */
	sflash_dma_xfer(sf, cmd, cmd, 4);
	/* chip select high */
	sflash_cs_set(sf);

	return 0;
}

static int sf_sector_erase(struct sflash_dev * sf, uint32_t addr)
{
	uint8_t * cmd = sf->buf.cmd;

	/* Address should be sector aligned */
	assert((addr & (SF_SECTOR_SIZE - 1)) == 0);

	cmd[0] = SF_SECTOR_ERASE;
	cmd[1] = (addr >> 16) & 0xff;
	cmd[2] = (addr >> 8) & 0xff;
	cmd[3] = addr & 0xff;

	/* chip select low */
	sflash_cs_clr(sf);
	/* send the command */
	sflash_dma_xfer(sf, cmd, cmd, 4);
	/* chip select high */
	sflash_cs_set(sf);

	return 0;
}

static int sf_page_program(struct sflash_dev * sf, uint32_t addr, 
						   unsigned int count)
{
	uint8_t * cmd= sf->buf.cmd;

	/* prepare the command */
	cmd[0] = SF_PAGE_PROGRAM;
	cmd[1] = (addr >> 16) & 0xff;
	cmd[2] = (addr >> 8) & 0xff;
	cmd[3] = addr & 0xff;

	/* chip select low */
	sflash_cs_clr(sf);
	/* send command and data */
	sflash_dma_xfer(sf, cmd, cmd, 4 + count);

	/* chip select high */
	sflash_cs_set(sf);

	return 0;
}

static int sf_page_read(struct sflash_dev * sf, uint32_t addr, unsigned int count)
{
	uint8_t * cmd = sf->buf.cmd;

	/* prepare the command */
	cmd[0] = SF_READ;
	cmd[1] = (addr >> 16) & 0xff;
	cmd[2] = (addr >> 8) & 0xff;
	cmd[3] = addr & 0xff;

	/* chip select low */
	sflash_cs_clr(sf);
	/* send the command and receive the data */
	sflash_dma_xfer(sf, cmd, cmd, 4 + count);
	/* chip select high */
	sflash_cs_set(sf);

	return 0;
}

/* ------------------------------------------------------------------------
   Serial Flash Public API (Low Level)
   ------------------------------------------------------------------------- */

int sflash_sector_erase(struct sflash_dev * sf, uint32_t addr)
{
	/* Sanity check */
	assert(sf != NULL);

	thinkos_mutex_lock(sf->mutex);

	sf_write_enable(sf);
	sf_sector_erase(sf, addr);
	while ((sf_flag_status_read(sf) & SF_FLAG_READY) == 0);

	sf->buf.sync = false;
	sf->buf.valid = false;

	thinkos_mutex_unlock(sf->mutex);

	return SF_SECTOR_SIZE;
}

int sflash_subsector_erase(struct sflash_dev * sf, uint32_t addr)
{
	unsigned int sr;
	unsigned int n;

	/* Sanity check */
	assert(sf != NULL);

	thinkos_mutex_lock(sf->mutex);

	sr = sf_read_status(sf);
	if (sr & SF_STATUS_BUSY) {
		return -1;
	}

	sf_write_enable(sf);
	sr = sf_read_status(sf);
	if ((sr & SF_STATUS_WEN_LATCH) == 0) {
		return -2;
	}
	sf_write_enable(sf);
	sf_subsector_erase(sf, addr);
	n = 0;
	do {
		n++;
		sr = sf_flag_status_read(sf);
	} while ((sr & SF_FLAG_READY) == 0);
	(void)n;

	sr = sf_read_status(sf);
	if (sr & SF_STATUS_BUSY) {
		return -1;
	}

	sf->buf.sync = false;
	sf->buf.valid = false;

	thinkos_mutex_unlock(sf->mutex);

	return SF_SUBSECTOR_SIZE;
}

int sflash_page_write(struct sflash_dev * sf, uint32_t addr,
					  const void * buf, size_t count)
{
	unsigned int sr;
	unsigned int n;

	/* Sanity check */
	assert(sf != NULL);
	assert(buf != NULL);

	if (count > SF_PAGE_SIZE)
		count = SF_PAGE_SIZE;

	thinkos_mutex_lock(sf->mutex);

	memcpy(sf->buf.data, buf, count);

	sr = sf_read_status(sf);
	if (sr & SF_STATUS_BUSY) {
		return -1;
	}

	sf_write_enable(sf);
	sr = sf_read_status(sf);
	if ((sr & SF_STATUS_WEN_LATCH) == 0) {
		return -2;
	}

	sf_page_program(sf, addr, count);

	n = 0;
	do {
		n++;
		sr = sf_flag_status_read(sf);
	} while ((sr & SF_FLAG_READY) == 0);
	(void)n;

	sf->buf.sync = false;
	sf->buf.valid = false;

	thinkos_mutex_unlock(sf->mutex);

	return count;
}

int sflash_page_read(struct sflash_dev * sf, uint32_t addr,
					 void * buf, size_t count)
{
	/* Sanity check */
	assert(sf != NULL);
	assert(buf != NULL);

	if (count > SF_PAGE_SIZE)
		count = SF_PAGE_SIZE;

	thinkos_mutex_lock(sf->mutex);

	sf_page_read(sf, addr, count);

	memcpy(buf, sf->buf.data, count);

	sf->buf.sync = false;
	sf->buf.valid = false;

	thinkos_mutex_unlock(sf->mutex);

	return count;
}

/* ------------------------------------------------------------------------
   Serial Flash Public API (High Level)
   ------------------------------------------------------------------------- */

int sflash_probe(struct sflash_dev * sf, struct sflash_inf * inf)
{
	unsigned int sr;

	/* Sanity check */
	assert(sf != NULL);
	assert(inf != NULL);

	/* Lock the driver */
	thinkos_mutex_lock(sf->mutex);

	/* reset the chip */
	sf_reset(sf);

	/* read the status flag register */
	sr = sf_flag_status_read(sf);
	if ((sr & SF_FLAG_READY) == 0) {
		return -1;
	}

	/* read the status register */
	sr = sf_read_status(sf);
	if (sr & SF_STATUS_BUSY) {
		return -1;
	}

	/* clear the status flag register */
	sf_flag_status_clear(sf);

	/* request the device ID */
	sf_device_id(sf);

	inf->manufacturer = sf->buf.data[0];
	inf->device_type = sf->buf.data[1];
	inf->capacity = sf->buf.data[2];

	/* FIXME: configure the device geometry and capacity dynamically */
	inf->sector_size = SF_SECTOR_SIZE;
	inf->sector_count = SF_SECTOR_COUNT;
	inf->subsector_size = SF_SUBSECTOR_SIZE;
	inf->page_size = SF_PAGE_SIZE;

	/* Set the read/write pointer to 0 */
	sf->io_addr = 0;
	/* Initialize the DMA buffer */
	sf->buf.addr = 0;
	sf->buf.sync = false;
	sf->buf.valid = false;

	/* Unlock the driver */
	thinkos_mutex_unlock(sf->mutex);

	return 0;
}

int sflash_seek(struct sflash_dev * sf, unsigned int pos)
{
	uint32_t addr = pos;

	/* Sanity check */
	assert(sf != NULL);

	thinkos_mutex_lock(sf->mutex);

	/* Check whether the address lies within the buffer limits or not */
	if ((addr < sf->buf.addr) || (addr >= sf->buf.addr + SF_PAGE_SIZE)) {
		/* Flush the DMA buffer */
		if (sf->buf.valid && !sf->buf.sync) {
			sf_write_enable(sf);
			sf_page_program(sf, sf->buf.addr, SF_PAGE_SIZE);
			while ((sf_flag_status_read(sf) & SF_FLAG_READY) == 0);
		}
		sf->buf.sync = false;
		sf->buf.valid = false;
	}

	sf->io_addr = addr;

	thinkos_mutex_unlock(sf->mutex);

	return 0;
}

int sflash_sync(struct sflash_dev * sf)
{
	/* Sanity check */
	assert(sf != NULL);

	thinkos_mutex_lock(sf->mutex);

	if (sf->buf.valid && !sf->buf.sync) {
		/* Address should be page aligned */
		assert((sf->buf.addr & (SF_PAGE_SIZE - 1)) == 0);
		sf_write_enable(sf);
		sf_page_program(sf, sf->buf.addr, SF_PAGE_SIZE);
		while ((sf_flag_status_read(sf) & SF_FLAG_READY) == 0);
	}

	sf->buf.sync = true;

	thinkos_mutex_unlock(sf->mutex);

	return 0;
}

int sflash_write(struct sflash_dev * sf, const void * buf, size_t count)
{
	uint8_t * src = (uint8_t *)buf;
	unsigned int rem;
	uint32_t addr;

	/* Sanity check */
	assert(sf != NULL);
	assert(buf != NULL);

	thinkos_mutex_lock(sf->mutex);

	/* Get read/write pointer */
	addr = sf->io_addr;

	rem = count;
	while (rem) {
		unsigned int offs;
		unsigned int n;

		/* Check whether the data in the DMA buffer is valid or not and
		 * if the address lies within the buffer limits  */
		if (!sf->buf.valid || (addr < sf->buf.addr) ||
			(addr >= sf->buf.addr + SF_PAGE_SIZE)) {

			/* Flush the DMA buffer */
			if (sf->buf.valid && !sf->buf.sync) {
				sf_write_enable(sf);
				sf_page_program(sf, sf->buf.addr, SF_PAGE_SIZE);
				while ((sf_flag_status_read(sf) & SF_FLAG_READY) == 0);
				/* XXX: the DMA transfer on sf_page_program will override the
				 * DMA buffer, mark the buffer as invalid then.
				 */
				sf->buf.valid = false;
				sf->buf.sync = false;
			}

			sf->buf.addr = addr & ~(SF_PAGE_SIZE - 1);
			if ((sf->buf.addr != addr) || (rem < SF_PAGE_SIZE)) {
				/* If the address do not points to the page's first byte or
				 * we are not writing the full buffer then read from serial 
				 * flash to fill in the gaps. */
				sf_page_read(sf, sf->buf.addr, SF_PAGE_SIZE);
				sf->buf.valid = true;
				sf->buf.sync = true;
			}
		}

		/* get the write offset within the page */
		offs = addr - sf->buf.addr;
		/* make sure we don't cross the page boundary */
		n = MIN(rem, SF_PAGE_SIZE - offs);

		/* copy the user data into the DMA buffer */
		memcpy(&sf->buf.data[offs], src, n);
		/* clear the sync flag */
		sf->buf.sync = false;
		/* mark the DMA buffer data as valid */
		sf->buf.valid = true;

		rem -= n;
		addr += n;
		src += n;
	}

	/* Update read/write pointer */
	sf->io_addr = addr;

	thinkos_mutex_unlock(sf->mutex);

	return count;
}

int sflash_read(struct sflash_dev * sf, void * buf, size_t count)
{
	uint8_t * dst = (uint8_t *)buf;
	unsigned int rem;
	uint32_t addr;

	/* Sanity check */
	assert(sf != NULL);
	assert(buf != NULL);

	thinkos_mutex_lock(sf->mutex);

	/* Get read/write pointer */
	addr = sf->io_addr;

	rem = count;
	while (rem) {
		unsigned int offs;
		unsigned int n;

		/* Check whether the data in the DMA buffer is valid or not and
		 * if the address lies within the buffer limits  */
		if ((sf->buf.valid) || (addr < sf->buf.addr) ||
			(addr >= sf->buf.addr + SF_PAGE_SIZE)) {

			/* Flush the DMA buffer */
			if (sf->buf.valid && !sf->buf.sync) {
				sf_write_enable(sf);
				sf_page_program(sf, sf->buf.addr, SF_PAGE_SIZE);
				while ((sf_flag_status_read(sf) & SF_FLAG_READY) == 0);
			}

			/* Read from serial flash */
			sf->buf.addr = addr & ~(SF_PAGE_SIZE - 1);
			sf_page_read(sf, sf->buf.addr, SF_PAGE_SIZE);
			sf->buf.valid = true;
			sf->buf.sync = true;
		}

		offs = addr - sf->buf.addr;
		n = MIN(count, SF_PAGE_SIZE - offs);

		/* copy the data from the DMA buffer */
		memcpy(dst, &sf->buf.data[offs], n);
		rem -= n;
		addr += n;
		dst += n;
	}

	/* Update read/write pointer */
	sf->io_addr = addr;

	thinkos_mutex_unlock(sf->mutex);

	return count;
}

int sflash_erase(struct sflash_dev * sf, uint32_t addr, size_t count)
{
	unsigned int rem;
	uint32_t offs;
	uint32_t start;
	uint32_t end;

	/* Sanity check */
	assert(sf != NULL);

	thinkos_mutex_lock(sf->mutex);

	/* find the start of the subsector */
	start = addr & ~(SF_SUBSECTOR_SIZE - 1);
	offs = addr - start;
	/* find the end of the flash region */
	end = (start + count + offs + SF_SUBSECTOR_SIZE - 1) & ~(SF_SUBSECTOR_SIZE - 1);

	rem = end - start;
	addr = start;
	while (rem > 0) {
		unsigned int n;

		if (((addr & (SF_SECTOR_SIZE - 1)) == 0) && (rem >= SF_SECTOR_SIZE)) {
			sf_write_enable(sf);
			sf_sector_erase(sf, addr);
			while ((sf_flag_status_read(sf) & SF_FLAG_READY) == 0);
			n = SF_SECTOR_SIZE;
		} else {
			sf_write_enable(sf);
			sf_subsector_erase(sf, addr);
			while ((sf_flag_status_read(sf) & SF_FLAG_READY) == 0);
			n = SF_SUBSECTOR_SIZE;
		}

		rem -= n;
		addr += n;
	}

	sf->buf.sync = false;
	sf->buf.valid = false;

	thinkos_mutex_unlock(sf->mutex);

	/* FIXME: the return value is not consistent ... */
	return count + offs;
}

#if 0
/* ------------------------------------------------------------------------
   SPI3 Serial Flash Instance 
   ------------------------------------------------------------------------- */

#define SPI_DMA STM32F_DMA1

#define SPI_DMA_RX_STRM 2
#define SPI_DMA_RX_CHAN 0

#define SPI_DMA_TX_STRM 7
#define SPI_DMA_TX_CHAN 0

#define SPI_IRQ STM32F_IRQ_SPI3

#endif

struct sflash_dev * sflash_init(void)
{
	static struct sflash_dev sflash_dev_singleton;
	struct sflash_dev * sf = &sflash_dev_singleton;
	struct stm32f_spi * spi = SPI_SFLASH;
	unsigned int freq = 25000000; /* 25 MHz */
	unsigned int div;
	unsigned int br;

#ifdef SFLASH_ENABLE_GPIO_INIT
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);

	/* Configure GPIO */
	stm32_gpio_mode(IO_SFLASH_CS, OUTPUT, SPEED_HIGH);
	stm32_gpio_set(IO_SFLASH_CS);

	stm32_gpio_mode(IO_SFLASH_SCK, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_SFLASH_SCK, AF_SPI_SFLASH);

	stm32_gpio_mode(IO_SFLASH_MISO, ALT_FUNC, PULL_UP);
	stm32_gpio_af(IO_SFLASH_MISO, AF_SPI_SFLASH);

	stm32_gpio_mode(IO_SFLASH_MOSI, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(IO_SFLASH_MOSI, AF_SPI_SFLASH);
#endif

	/* SPI clock enable */
	stm32_clk_enable(STM32_RCC, CLK_SPI_SFLASH);

	/* Compute the baudrate field */
	div = stm32_clk_hz(CLK_SPI_SFLASH) / freq / 2;
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
	sf->irqno = IRQ_SPI_SFLASH;

	/* Initialize DMA structures */

	stm32_dmactl_init(&sf->rx_dma, DMA_SPI_SFLASH, DMA_SPI_RX_STREAM_SFLASH);
	/* Configure RX DMA stream */
	sf->rx_dma.strm->cr = DMA_CHSEL_SET(DMA_SPI_RX_CHAN_SFLASH) |
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_8 | DMA_PSIZE_8 |
		DMA_CT_M0AR | DMA_MINC | DMA_DIR_PTM |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	sf->rx_dma.strm->par = &spi->dr;
	sf->rx_dma.strm->fcr = 0;

	stm32_dmactl_init(&sf->tx_dma, DMA_SPI_SFLASH, DMA_SPI_TX_STREAM_SFLASH);
	/* Configure TX DMA stream */
	sf->tx_dma.strm->cr = DMA_CHSEL_SET(DMA_SPI_TX_CHAN_SFLASH) |
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_8| DMA_PSIZE_8 |
		DMA_CT_M0AR | DMA_MINC | DMA_DIR_MTP |
		DMA_TCIE | DMA_TEIE | DMA_DMEIE;
	sf->tx_dma.strm->par = &spi->dr;
	sf->tx_dma.strm->fcr = 0;

	/* Initialize DMA buffer */
	sf->buf.valid = false;
	sf->buf.sync = false;
	sf->buf.addr = 0;

	/* Allocate a mutex for high level operations */
	sf->mutex = thinkos_mutex_alloc();

	/* Return this driver's instance */
	return &sflash_dev_singleton;
}
