/* 
 * File:	 i2c-master.c
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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <sys/delay.h>

#include <thinkos.h>

#include "trace.h"

/* ----------------------------------------------------------------------
 * I2C
 * ----------------------------------------------------------------------
 */
struct i2c_xfer {
	volatile uint8_t * ptr;
	volatile int32_t cnt;
	volatile int32_t rem;
	volatile int ret;
	uint32_t addr;
	int32_t flag;
};

struct i2c_xfer xfer;

#define I2C1_SCL STM32_GPIOB, 6
#define I2C1_SDA STM32_GPIOB, 7
#define I2C_IRQ_PRIORITY IRQ_PRIORITY_VERY_HIGH 

void i2c_master_init(unsigned int scl_freq)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint32_t pclk = stm32f_apb1_hz;

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);

	stm32_gpio_mode(I2C1_SCL, ALT_FUNC, OPEN_DRAIN);
	stm32_gpio_mode(I2C1_SDA, ALT_FUNC, OPEN_DRAIN);

	stm32_gpio_af(I2C1_SCL, GPIO_AF4);
	stm32_gpio_af(I2C1_SDA, GPIO_AF4);

	stm32_clk_enable(STM32_RCC, STM32_CLK_I2C1);

	/* Software reset */
	i2c->cr1 = I2C_SWRST; 
	udelay(10);
	i2c->cr1 = 0;

	/* I2C Control register 2 (I2C_CR2) */
	i2c->cr2 = I2C_FREQ_SET(pclk / 1000000);
	/*	I2C Own address register 1 (I2C_OAR1) */
	i2c->oar1 = 0;
	/*	I2C Own address register 2 (I2C_OAR2) */
	i2c->oar2 = 0;
	/* I2C Clock control register (I2C_CCR) */ 
	i2c->ccr = I2C_CCR_SET(pclk / scl_freq / 2);
	/* I2C TRISE register (I2C_TRISE) */
	i2c->trise = I2C_TRISE_SET((pclk / 1000000) + 1);

	xfer.flag = thinkos_flag_alloc();

	cm3_irq_enable(STM32F_IRQ_I2C1_EV);
	/* set event IRQ to high priority */
	cm3_irq_pri_set(STM32F_IRQ_I2C1_EV, I2C_IRQ_PRIORITY);

	cm3_irq_enable(STM32F_IRQ_I2C1_ER);
	/* set error IRQ to high priority */
	cm3_irq_pri_set(STM32F_IRQ_I2C1_ER, I2C_IRQ_PRIORITY);

	/* enable ACK, events and errors */
	i2c->cr2 |= I2C_ITERREN | I2C_ITEVTEN | I2C_ITBUFEN;
}

void i2c_master_enable(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;

	i2c->cr1 = I2C_PE;
}

void i2c_master_reset(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint32_t cr1;
	uint32_t cr2;
	uint32_t oar1;
	uint32_t oar2;
	uint32_t ccr;
	uint32_t trise;

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


uint32_t i2c_irq_cnt = 0;

void stm32f_i2c1_ev_isr(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint32_t sr1;
	uint32_t sr2;

	i2c_irq_cnt++;

	sr1 = i2c->sr1;

	if (sr1 & I2C_SB) {
		/* – To enter Transmitter mode, a master sends the slave 
		   address with LSB reset. */
		i2c->dr = xfer.addr;
	}

	if (sr1 & I2C_ADDR) {
		/* Clear ADDR flag */
		sr2 = i2c->sr2;
		(void)sr2;

		if ((xfer.addr & 1) & (xfer.cnt == 1)) {
			/* ● Case of a single byte to be received:
			   – In the ADDR event, clear the ACK bit.
			   – Clear ADDR
			   – Program the STOP/START bit.
			   – Read the data after the RxNE flag is set. */
			/* Program the STOP/START bit. */
			i2c->cr1 = I2C_STOP | I2C_PE; 
		}
	}

	if (sr1 & I2C_BTF) {
		if (xfer.addr & 1) {
			goto do_recv;
		} else {
			goto do_xmit;
		}
	}

	if (sr1 & I2C_RXNE) {
do_recv:
		*xfer.ptr++ = i2c->dr;
		xfer.rem--;
		if (xfer.rem == 0) {
			xfer.ret = xfer.cnt;
			thinkos_flag_give_i(xfer.flag);
		} else if (xfer.rem == 1) {
			/* Clear ACK */
			i2c->cr1 = I2C_STOP | I2C_PE; 
		}
	}

	if (sr1 & I2C_TXE) {
do_xmit:
		if (xfer.rem > 0) {
			i2c->dr = *xfer.ptr++;
		} else if (xfer.rem == 0) {
			/* Program STOP. */
			i2c->cr1 = I2C_STOP | I2C_PE; 
			/* Clear the TXE flag */
			i2c->dr = 0;
		} else {
			i2c->dr = 0;
			xfer.ret = xfer.cnt;
			thinkos_flag_give_i(xfer.flag);
		} 
		xfer.rem--;
	}
}

void stm32f_i2c1_er_isr(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint32_t sr1;

	sr1 = i2c->sr1;

	i2c_irq_cnt++;

	if (sr1 & I2C_BERR) {
		i2c->sr1 = 0;
		xfer.ret = -1;
		thinkos_flag_give_i(xfer.flag);
	}

	if (sr1 & I2C_ARLO) {
		i2c->sr1 = 0;
		xfer.ret = -1;
		thinkos_flag_give_i(xfer.flag);
	}

	if (sr1 & I2C_AF) {
		/* clear AF */
		i2c->sr1 = 0;
		i2c->cr1 = I2C_STOP | I2C_PE; /* generate a Stop condition */
		xfer.ret = -1;
		thinkos_flag_give_i(xfer.flag);
	}

	if (sr1 & I2C_OVR) {
	}
}

int i2c_master_wr(unsigned int addr, const void * buf, int len)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	int ret;

	xfer.ptr = (uint8_t *)buf;
	xfer.rem = len;
	xfer.cnt = len;
	/* – To enter Transmitter mode, a master sends the slave 
	   address with LSB reset. */
	xfer.addr = addr << 1;
	xfer.ret = -2;

	i2c->cr1 = I2C_START | I2C_ACK | I2C_PE; /* generate a Start condition */

	if (thinkos_flag_timedtake(xfer.flag, 100) == THINKOS_ETIMEDOUT) {
		i2c_master_reset();
		ret = -1;
	} else
		ret = xfer.ret;

	return ret;

}

int i2c_master_rd(unsigned int addr, void * buf, int len)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	int ret;

	if (len == 0)
		return 0;

	xfer.ptr = (uint8_t *)buf;
	xfer.rem = len;
	xfer.cnt = len;
	/* – To enter Receiver mode, a master sends the slave 
	   address with LSB set. */
	xfer.addr = (addr << 1) | 1;
	xfer.ret = -2;

	i2c->cr1 = I2C_START | I2C_ACK | I2C_PE; /* generate a Start condition */

	if (thinkos_flag_timedtake(xfer.flag, 100) == THINKOS_ETIMEDOUT) {
		i2c_master_reset();
		ret = -1;
	} else
		ret = xfer.ret;

	return ret;
}

int i2c_mutex = -1;

int i2c_read(unsigned int addr, void * buf, int n)
{
	int ret;

	thinkos_mutex_lock(i2c_mutex);

	ret = i2c_master_rd(addr, buf, n);

	thinkos_mutex_unlock(i2c_mutex);

	return ret;
}

int i2c_reg_read(unsigned int addr, unsigned int reg, void * buf, int n)
{
	uint8_t pkt[1];
	int ret;

	thinkos_mutex_lock(i2c_mutex);

	pkt[0] = reg;

	if ((ret = i2c_master_wr(addr, pkt, 1)) > 0) {
		ret = i2c_master_rd(addr, buf, n);
	}

	thinkos_mutex_unlock(i2c_mutex);

	return ret;
}

int i2c_reg_write(unsigned int addr, unsigned int reg, const void * buf, int n)
{
	uint8_t pkt[n + 1];
	int ret;

	thinkos_mutex_lock(i2c_mutex);

	pkt[0] = reg;
	memcpy(&pkt[1], buf, n);

	if ((ret = i2c_master_wr(addr, pkt, n + 1)) > 0) {
		ret--;
	}

	thinkos_mutex_unlock(i2c_mutex);

	return ret;
}

void i2c_reset(void)
{
	thinkos_mutex_lock(i2c_mutex);

	i2c_master_reset();

	thinkos_mutex_unlock(i2c_mutex);
}

void i2c_init(void)
{
	i2c_mutex = thinkos_mutex_alloc();
//	tracef("I2C mutex=%d\n", i2c_mutex);

	thinkos_mutex_lock(i2c_mutex);
	i2c_master_init(100000);

	i2c_master_enable();
	thinkos_mutex_unlock(i2c_mutex);
}

int i2c_bus_scan(unsigned int from, unsigned int to, uint8_t lst[], int len)
{
	uint8_t buf[4];
	uint8_t addr = 0;
	int n;

	thinkos_mutex_lock(i2c_mutex);

	/* 7 bit addresses range from 1 to 0x78 */
	if (from < 1)
		from = 1;
	if (to > 0x78)
		to = 0x78;

	n = 0;
	for (addr = from; addr <= to; ++addr) {

		buf[0] = 0;
		if (i2c_master_wr(addr, buf, 1) <= 0) {
			continue;
		}

		if (i2c_master_rd(addr, buf, 1) <= 0) {
			continue;
		}

		if (n < len)
			lst[n++] = addr;
	}

	thinkos_mutex_unlock(i2c_mutex);

	return n;
}

