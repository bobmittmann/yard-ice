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

#include "board.h"
#include "sflash.h"

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
};

static inline void sflash_cs_set(struct sflash_dev * sf) {
	stm32_gpio_set(sf->cs.gpio, sf->cs.pin);
}

static inline void sflash_cs_clr(struct sflash_dev * sf) {
	stm32_gpio_clr(sf->cs.gpio, sf->cs.pin);
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
	spi->cr2 = 0;

	while ((sr = spi->sr) & SPI_RXNE) {
		/* clear input buffer */
		dummy = spi->dr;
		(void)dummy;
		sr = spi->sr;
	}
	
	/* Enable SPI */
	spi->cr1 |= SPI_SPE;

	for (i = 0; i < len; ++i) {
		while (!((sr = spi->sr) & SPI_TXE));
		/* send dummy data */
		spi->dr = txd[i];
		/* wait for incomming data */
		while (!((sr = spi->sr) & SPI_RXNE));
		rxd[i] = spi->dr;
	}

}


#define SPI_DMA_RX_STRM 2
#define SPI_DMA_RX_CHAN 0
#define SPI_DMA_RX_IRQ STM32F_IRQ_DMA1_STREAM3

#define SPI_DMA_TX_STRM 4
#define SPI_DMA_TX_CHAN 0
#define SPI_DMA_TX_IRQ STM32F_IRQ_DMA1_STREAM4

void sflash_dma_xfer(struct sflash_dev * sf, 
					 uint8_t txd[], uint8_t rxd[], 
					 unsigned int len)
{
	struct stm32f_spi * spi = sf->spi;
	struct stm32f_dma * dma = STM32F_DMA1;

	/* Disable SPI */
	spi->cr1 &= ~SPI_SPE;
	spi->cr2 = SPI_TXDMAEN | SPI_RXDMAEN;

	/* Disable DMA channel */
	dma->s[SPI_DMA_RX_STRM].cr = 0;
	while (dma->s[SPI_DMA_RX_STRM].cr & DMA_EN); 

	dma->s[SPI_DMA_TX_STRM].cr = 0;
	while (dma->s[SPI_DMA_TX_STRM].cr & DMA_EN); 

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
		DMA_MBURST_1 | DMA_PBURST_1 | DMA_MSIZE_8| DMA_PSIZE_8 | 
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
	
	thinkos_irq_wait(SPI_DMA_RX_IRQ);

	/* clear the DMA transfer complete flag */
	dma->hifcr = DMA_CTCIF4;
	/* clear the DMA transfer complete flag */
	dma->lifcr = DMA_CTCIF2;
}

/* ------------------------------------------------------------------------
   Serial Flash Public API 
   ------------------------------------------------------------------------- */

int sflash_reset(struct sflash_dev * sf)
{
	assert(sf != NULL);

	sflash_cs_clr(sf);
	stm32f_spi_putc(sf->spi, SF_RESET_ENABLE);
	sflash_cs_set(sf);

	udelay(10);

	sflash_cs_clr(sf);
	stm32f_spi_putc(sf->spi, SF_RESET_MEMORY);
	sflash_cs_set(sf);

	udelay(10);

	return 0;
}

int sflash_device_id(struct sflash_dev * sf, void * buf, size_t len)
{
	unsigned int cnt;
	uint8_t data[22];

	assert(sf != NULL);
	assert(buf != NULL);
	assert(len > 0);

	cnt = MIN(20, len);	

	/* chip select low */
	sflash_cs_clr(sf);

#if 0
	/* send the command */
	cmd[0] = SF_READ_ID;
	sflash_send(sf, cmd, 1);

	/* receive the data */
	sflash_recv(sf, data, cnt);
#endif

	data[0] = SF_READ_ID;

	sflash_io_xfer(sf, data, data, cnt + 1);

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

	/* send the command */
	cmd[0] = SF_PAGE_PROGRAM;
	cmd[1] = (addr >> 16) & 0xff;
	cmd[2] = (addr >> 8) & 0xff;
	cmd[3] = addr & 0xff;

	/* chip select low */
	sflash_cs_clr(sf);

	/* send command */
	sflash_io_xfer(sf, cmd, cmd, 4);

	/* receive the data */
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

	/* chip select low */
	sflash_cs_clr(sf);

	/* send the command */
	cmd[0] = SF_READ;
	cmd[1] = (addr >> 16) & 0xff;
	cmd[2] = (addr >> 8) & 0xff;
	cmd[3] = addr & 0xff;
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

	stm32_gpio_mode(IO_LCD_EN, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(IO_LCD_RNW, OUTPUT, PUSH_PULL| SPEED_MED);
	stm32_gpio_mode(IO_LCD_NRE, OUTPUT, PUSH_PULL| SPEED_MED);

	stm32_gpio_set(IO_SFLASH_CS);
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

	/* Configure SPI */
	div = stm32_clk_hz(STM32_CLK_SPI3) / freq / 2;
	br = 31 - __clz(div);
	if (div > (1 << br))
		br++;

	spi->cr1 = 0;
	spi->cr2 = 0;
	spi->i2scfgr = 0;
	spi->i2spr = 0;

	/* Master mode, MSB first */
	spi->cr1 = SPI_SPE | SPI_BR_SET(br) | SPI_MSTR | SPI_SSM | SPI_SSI;
	/*enable interrupts */
	spi->cr2 = SPI_TXEIE | SPI_RXNEIE;

	sf->spi = spi;
	sf->cs.gpio = stm32_io_gpio(IO_SFLASH_CS); 
	sf->cs.pin = stm32_io_pin(IO_SFLASH_CS); 
	sf->irqno = STM32F_IRQ_SPI3;

	/* DMA clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_DMA1);

	return &sflash_dev_singleton;
}




#if 0
int l3gd20_wr(struct stm32f_spi * spi, unsigned int reg, 
			  void * buf, unsigned int len)
{
	uint8_t * data = (uint8_t *)buf;
	unsigned int addr;
	unsigned int sr;
	unsigned int dummy;
	int i;

	if (len == 0)
		return 0;

	addr = (reg & 0x3f) | ((len > 1) ? 0x40 : 0x00);

	gpio_clr(l3gd20_cs);

	udelay(1);

	sr = spi->sr;
	
	if (!(sr & SPI_TXE))
		return -1;

	if (sr & SPI_MODF)
		return -2;

	if (sr & SPI_RXNE) {
		/* clear input buffer */
		dummy = spi->dr;
		(void)dummy;
	}

	/* send the address */
	spi->dr = addr;

	for (i = 0; i < len; ++i) {

		while (!((sr = spi->sr) & SPI_TXE)) {
			thinkos_irq_wait(STM32F_IRQ_SPI1);
		} 

		/* send the data */
		spi->dr = data[i];

		/* wait for incomming data */
		while (!((sr = spi->sr) & SPI_RXNE)) {
			thinkos_irq_wait(STM32F_IRQ_SPI1);
		} 

		/* discard */
		dummy = spi->dr;
		(void)dummy;
	}

	while (!((sr = spi->sr) & SPI_RXNE)) {
		thinkos_irq_wait(STM32F_IRQ_SPI1);
	}

	dummy = spi->dr;
	(void)dummy;

	udelay(1);

	gpio_set(l3gd20_cs);

	return len;
}

int l3gd20_rd(struct stm32f_spi * spi, int reg, 
			  void * buf, unsigned int len)
{
	uint8_t * data = (uint8_t *)buf;
	unsigned int addr;
	unsigned int sr;
	unsigned int dummy;
	int i;

	if (len == 0)
		return 0;

	addr = (reg & 0x3f) | 0x80 | ((len > 1) ? 0x40 : 0x00);

	gpio_clr(l3gd20_cs);

	udelay(1);

	sr = spi->sr;
	
	if (!(sr & SPI_TXE))
		return -1;

	if (sr & SPI_MODF)
		return -2;

	if (sr & SPI_RXNE) {
		/* clear input buffer */
		dummy = spi->dr;
		(void)dummy;
	}

	/* send the address */
	spi->dr = addr;

	while (!((sr = spi->sr) & SPI_TXE)) {
		thinkos_irq_wait(STM32F_IRQ_SPI1);
	} 

	/* send first dummy data */
	spi->dr = 0;

	/* wait for incomming data */
	while (!((sr = spi->sr) & SPI_RXNE)) {
		thinkos_irq_wait(STM32F_IRQ_SPI1);
	} 

	/* discard */
	dummy = spi->dr;
	(void)dummy;

	for (i = 0; i < (len - 1); ++i) {
		while (!((sr = spi->sr) & SPI_TXE)) {
			thinkos_irq_wait(STM32F_IRQ_SPI1);
		} 

		/* send dummy data */
		spi->dr = 0;

		/* wait for incomming data */
		while (!((sr = spi->sr) & SPI_RXNE)) {
			thinkos_irq_wait(STM32F_IRQ_SPI1);
		} 

		data[i] = spi->dr;
	}

	while (!((sr = spi->sr) & SPI_RXNE)) {
		thinkos_irq_wait(STM32F_IRQ_SPI1);
	}

	data[i] = spi->dr;

	udelay(1);

	gpio_set(l3gd20_cs);

	return len;
}

int l3gd20_init(void)
{
	struct stm32f_spi * spi = STM32F_SPI1;

	gpio_io_t io;

	io = l3gd20_cs ;
	stm32_gpio_clock_en(STM32_GPIO(io.port));
	stm32_gpio_mode(STM32_GPIO(io.port), io.pin, OUTPUT, SPEED_MED);
	gpio_set(io);

	stm32f_spi_init(spi, &spi1_io, 500000, SPI_MSTR | SPI_CPOL | SPI_CPHA);

	spi->cr2 = SPI_TXEIE | SPI_RXNEIE;

	return 0;
}

#endif

