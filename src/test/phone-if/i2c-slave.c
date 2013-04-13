/* 
 * File:	 i2c-slave.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
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
#include <arch/cortex-m3.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/delay.h>
#include <stdio.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

#include <sys/dcclog.h>

#include "i2c.h"

#define I2C1_SCL STM32F_GPIOB, 8
#define I2C1_SDA STM32F_GPIOB, 9
#define I2C_IRQ_PRIORITY IRQ_PRIORITY_VERY_HIGH 

struct i2c_io_blk {
	/* index to the memory position to be transfered next */
	uint32_t idx;
	/* top index for the I/O memory block */
	uint32_t max;
	/* transfer count */
	uint32_t cnt;
	/* transfer status/direction */
	int xfer;
	/* reference to the I/O memory block */
	void * rd_mem;
	void * wr_mem;
	int flag;
	uint32_t irq_cnt;
};

struct i2c_io_blk i2c_io;

struct i2c_io_blk * i2c_slave_init(unsigned int scl_freq, unsigned int addr,
								   void * rd_mem, void * wr_mem, 
								   unsigned int size)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	struct stm32f_rcc * rcc = STM32F_RCC;
	uint32_t pclk = stm32f_apb1_hz;
	struct i2c_io_blk * io = &i2c_io;
#if defined(STM32F1X)
	struct stm32f_afio * afio = STM32F_AFIO;
#endif

	io->rd_mem = rd_mem;
	io->wr_mem = wr_mem;
	io->max = size - 1;
	io->idx = 0;
	io->cnt = 0;

	stm32f_gpio_mode(I2C1_SCL, ALT_FUNC, OPEN_DRAIN);
	stm32f_gpio_mode(I2C1_SDA, ALT_FUNC, OPEN_DRAIN);

#if defined(STM32F1X)
	/* Use alternate pins for I2C1 */
	afio->mapr |= AFIO_I2C1_REMAP;
#endif
#if defined(STM32F4X)
	stm32f_gpio_af(I2C1_SCL, GPIO_AF4);
	stm32f_gpio_af(I2C1_SDA, GPIO_AF4);
#endif


	/* Enable I2C clock */
	rcc->apb1enr |= RCC_I2C1EN;

	i2c->cr1 = I2C_SWRST; 

	DCC_LOG3(LOG_TRACE, "CR1=0x%04x CR2=0x%04x CCR=0x%04x", 
			 i2c->cr1, i2c->cr2, i2c->ccr);

	DCC_LOG3(LOG_TRACE, "OAR1=0x%04x OAR2=0x%04x TRISE=0X%04X", 
			 i2c->oar1, i2c->oar2, i2c->trise);

	DCC_LOG2(LOG_TRACE, "SR1=0x%04x SR2=0x%04x ", i2c->sr1, i2c->sr2);

	i2c->cr1 = 0;

	/* I2C Control register 2 (I2C_CR2) */
	i2c->cr2 = I2C_FREQ_SET(pclk / 1000000);
	/*	I2C Own address register 1 (I2C_OAR1) */
	i2c->oar1 = addr << 1;
	/*	I2C Own address register 2 (I2C_OAR2) */
	i2c->oar2 = addr << 1;
	/* I2C Clock control register (I2C_CCR) */ 
	i2c->ccr = I2C_CCR_SET(pclk / scl_freq / 2);
	/* I2C TRISE register (I2C_TRISE) */
	i2c->trise = I2C_TRISE_SET((pclk / 1000000) + 1);

	i2c_io.flag = thinkos_flag_alloc();

	cm3_irq_enable(STM32F_IRQ_I2C1_EV);
	/* set event IRQ to very high priority */
	cm3_irq_pri_set(STM32F_IRQ_DMA1_STREAM0, I2C_IRQ_PRIORITY);
	cm3_irq_enable(STM32F_IRQ_I2C1_ER);
	/* set error IRQ to high priority */
	cm3_irq_pri_set(STM32F_IRQ_DMA1_STREAM0, I2C_IRQ_PRIORITY);

	DCC_LOG(LOG_TRACE, "Enabling interrupts....");
	/* enable ACK, events and errors */
	i2c->cr2 |= I2C_ITERREN | I2C_ITEVTEN | I2C_ITBUFEN;
	return io;
}



void i2c_slave_enable(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;

	DCC_LOG(LOG_TRACE, "Enabling device ....");
	/* enable ACK */
	/* I2C Control register 1 (I2C_CR1) */
	i2c->cr1 = I2C_STOP | I2C_ACK | I2C_PE; 
}

/* ----------------------------------------------------------------------
 * I2C I/O blocking (interrupts)
 * ----------------------------------------------------------------------
 */

void stm32f_i2c1_ev_isr(void) 
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	struct i2c_io_blk * io = &i2c_io;
	uint8_t * rd_mem = (uint8_t *)io->rd_mem;
	uint8_t * wr_mem = (uint8_t *)io->wr_mem;
	uint32_t sr1;
	uint32_t sr2;

	io->irq_cnt++;

	sr1 = i2c->sr1;
	
	if (sr1 & I2C_ADDR) {
		/* Clear ADDR flag */
		sr2 = i2c->sr2;
		/* Check if the tranfer direction */
		if (sr2 & I2C_TRA) {
			io->xfer = I2C_XFER_OUT;
			DCC_LOG1(LOG_INFO, "%d ADDR: TRA=1 (xmit)", io->irq_cnt);
		} else {
			io->xfer = I2C_XFER_IDX;
			DCC_LOG1(LOG_INFO, "%d ADDR: TRA=0 (recv)", io->irq_cnt);
		} 
		io->cnt = 0;
	}

	if (sr1 & I2C_STOPF) {
		/* Clear STOPF */
		i2c->cr1 = I2C_ACK | I2C_PE; 
		/* Note: The STOPF bit is not set after a NACK reception.
		  It is recommended to perform the complete clearing sequence 
		  (READ SR1 then WRITE CR1) after the STOPF is set. */

		DCC_LOG3(LOG_INFO, "%d STOPF: cnt=%d %s", io->irq_cnt, io->cnt,
			(io->xfer == I2C_XFER_IN) ? "IN" : "OUT");

		if (io->xfer > I2C_XFER_IDX)
			__thinkos_flag_signal(io->flag);
	}

	if (sr1 & I2C_BTF) {
		DCC_LOG(LOG_TRACE, "BTF");
	}

	if (sr1 & I2C_RXNE) {
		if (io->cnt == 0) {
			/* First byte is the address (index) */
			io->idx = i2c->dr;
			if (io->idx > io->max)
				io->idx = 0;
			DCC_LOG2(LOG_INFO, "%d RXNE idx=%02x", io->irq_cnt, io->idx);
		} else {
			io->xfer = I2C_XFER_IN;
			wr_mem[io->idx] = i2c->dr;
			DCC_LOG2(LOG_INFO, "%d RXNE [%02x]", io->irq_cnt, wr_mem[io->idx]);
			io->idx = (io->idx == io->max) ? 0 : io->idx + 1;
		}
		io->cnt++;
	}

	if (sr1 & I2C_TXE) {
		i2c->dr = rd_mem[io->idx];
		io->idx = (io->idx == io->max) ? 0 : io->idx + 1;
		io->cnt++;
		DCC_LOG2(LOG_INFO, "%d TXE %02x", io->irq_cnt, rd_mem[io->idx]);
	}
}

void stm32f_i2c1_er_isr(void) 
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	struct i2c_io_blk * io = &i2c_io;
	uint32_t sr1;

	io->irq_cnt++;

	sr1 = i2c->sr1;

	if (sr1 & I2C_BERR) {
		DCC_LOG1(LOG_TRACE, "%d BERR", io->irq_cnt);
		io->xfer = I2C_XFER_ERR;
		__thinkos_flag_signal(io->flag);
	}

	if (sr1 & I2C_AF) {
		DCC_LOG3(LOG_INFO, "%d AF: cnt=%d %s", io->irq_cnt, io->cnt,
			(io->xfer == I2C_XFER_IN) ? "IN" : "OUT");
		__thinkos_flag_signal(io->flag);
	}

	if (sr1 & I2C_OVR) {
		DCC_LOG1(LOG_TRACE, "%d OVR", io->irq_cnt);
		io->xfer = I2C_XFER_ERR;
		__thinkos_flag_signal(io->flag);
	}

	/* clear AF */
	i2c->sr1 = 0;
}

int i2c_slave_io(void)
{
	struct i2c_io_blk * io = &i2c_io;

	DCC_LOG(LOG_INFO, "wait...");

	__thinkos_flag_clr(io->flag);
	thinkos_flag_wait(io->flag);

	DCC_LOG(LOG_INFO, "wake up.");

	return io->xfer;
}

void i2c_reset(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint32_t cr1;
	uint32_t cr2;
	uint32_t oar1;
	uint32_t oar2;
	uint32_t ccr;
	uint32_t trise;

	DCC_LOG(LOG_TRACE, "I2C reset...");

	cr1 = i2c->cr1;
	cr2 = i2c->cr2;
	oar1 = i2c->oar1;
	oar2 = i2c->oar2;
	ccr = i2c->ccr;
	trise = i2c->trise;


	/* Software reset */
	i2c->cr1 = I2C_SWRST; 
	udelay(10);
	i2c->cr1 = I2C_PE;

	i2c->cr2 = cr2;
	i2c->oar1 = oar1;
	i2c->oar2 = oar2;
	i2c->ccr = ccr;
	i2c->trise = trise;
	i2c->cr1 = cr1;
}


