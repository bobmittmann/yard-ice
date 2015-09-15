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
	uint8_t * buf;
	volatile uint32_t cnt;
	volatile uint32_t txn;
	volatile uint32_t rxn;
	volatile int ret;
	uint32_t addr;
	int32_t flag;
	struct {
		volatile uint32_t irq_cnt;
		volatile uint32_t rx_cnt;
		volatile uint32_t tx_cnt;
		volatile uint32_t tc_cnt;
		volatile uint32_t nack_cnt;
	} stat;
};

struct i2c_xfer xfer;

#define I2C1_SCL STM32_GPIOB, 6
#define I2C1_SDA STM32_GPIOB, 7
#define I2C_IRQ_PRIORITY IRQ_PRIORITY_HIGH


uint32_t i2c_irq_cnt = 0;

/*
Master transmitter
In the case of a write transfer, the TXIS flag is set after each byte transmission, after the 9th
SCL pulse when an ACK is received.
A TXIS event generates an interrupt if the TXIE bit is set in the I2Cx_CR1 register. The flag
is cleared when the I2Cx_TXDR register is written with the next data byte to be transmitted.
The number of TXIS events during the transfer corresponds to the value programmed in
NBYTES[7:0]. If the total number of data bytes to be sent is greater than 255, reload mode
must be selected by setting the RELOAD bit in the I2Cx_CR2 register. In this case, when
NBYTES data have been transferred, the TCR flag is set and the SCL line is stretched low
until NBYTES[7:0] is written to a non-zero value.
The TXIS flag is not set when a NACK is received.
• When RELOAD=0 and NBYTES data have been transferred:
– In automatic end mode (AUTOEND=1), a STOP is automatically sent.
– In software end mode (AUTOEND=0), the TC flag is set and the SCL line is
stretched low in order to perform software actions:
A RESTART condition can be requested by setting the START bit in the I2Cx_CR2
register with the proper slave address configuration, and number of bytes to be
transferred. Setting the START bit clears the TC flag and the START condition is
sent on the bus.
A STOP condition can be requested by setting the STOP bit in the I2Cx_CR2
register. Setting the STOP bit clears the TC flag and the STOP condition is sent on
the bus.
• If a NACK is received: the TXIS flag is not set, and a STOP condition is automatically
sent after the NACK reception. the NACKF flag is set in the I2Cx_ISR register, and an
interrupt is generated if the NACKIE bit is set.


Master receiver
In the case of a read transfer, the RXNE flag is set after each byte reception, after the 8th
SCL pulse. An RXNE event generates an interrupt if the RXIE bit is set in the I2Cx_CR1
register. The flag is cleared when I2Cx_RXDR is read.
If the total number of data bytes to be received is greater than 255, reload mode must be
selected by setting the RELOAD bit in the I2Cx_CR2 register. In this case, when
NBYTES[7:0] data have been transferred, the TCR flag is set and the SCL line is stretched
low until NBYTES[7:0] is written to a non-zero value.
• When RELOAD=0 and NBYTES[7:0] data have been transferred:
– In automatic end mode (AUTOEND=1), a NACK and a STOP are automatically
sent after the last received byte.
– In software end mode (AUTOEND=0), a NACK is automatically sent after the last
received byte, the TC flag is set and the SCL line is stretched low in order to allow
software actions:
A RESTART condition can be requested by setting the START bit in the I2Cx_CR2
register with the proper slave address configuration, and number of bytes to be
transferred. Setting the START bit clears the TC flag and the START condition,
followed by slave address, are sent on the bus.
A STOP condition can be requested by setting the STOP bit in the I2Cx_CR2
register. Setting the STOP bit clears the TC flag and the STOP condition is sent on
the bus. */

void stm32f_i2c1_ev_isr(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint32_t sr;
	unsigned int c;

	xfer.stat.irq_cnt++;

	sr = i2c->isr;

	if (sr & I2C_TC) {
		udelay(30);
		xfer.cnt = 0;
		/* generate a Start condition */
		i2c->cr2 = I2C_SADD_SET(xfer.addr) | I2C_NBYTES_SET(xfer.rxn) | I2C_RD_WRN |
				I2C_START | I2C_AUTOEND;
		xfer.stat.tc_cnt++;
	}

	if (sr & I2C_RXNE) {
		c = i2c->rxdr;
		xfer.buf[xfer.cnt++] = c;
		if (xfer.cnt == xfer.rxn) {
			xfer.ret = xfer.cnt;
			thinkos_flag_give_i(xfer.flag);
		}
		xfer.stat.rx_cnt++;
	}

	if (sr & I2C_TXIS) {
		c = xfer.buf[xfer.cnt++];
		i2c->txdr = c;
		if (xfer.cnt == xfer.txn) {
			xfer.ret = xfer.cnt;
			thinkos_flag_give_i(xfer.flag);
		}
		xfer.stat.tx_cnt++;
	}

	if (sr & I2C_NACKF) {
		i2c->icr = I2C_NACKCF;
		xfer.ret = -1;
		thinkos_flag_give_i(xfer.flag);
		xfer.stat.nack_cnt++;
	}
}

void stm32f_i2c1_er_isr(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint32_t sr;

	sr = i2c->isr;

	i2c_irq_cnt++;

	if (sr & I2C_BERR) {
		i2c->icr = I2C_BERRCF;
		xfer.ret = -1;
		thinkos_flag_give_i(xfer.flag);
	}

	if (sr & I2C_ARLO) {
		i2c->icr = I2C_ARLOCF;
		xfer.ret = -1;
		thinkos_flag_give_i(xfer.flag);
	}

}

/*
I2C master initialization
Before enabling the peripheral, the I2C master clock must be configured by setting the
SCLH and SCLL bits in the I2Cx_TIMINGR register.
A clock synchronization mechanism is implemented in order to support multi-master
environment and slave clock stretching.
In order to allow clock synchronization:
• The low level of the clock is counted using the SCLL counter, starting from the SCL low
level internal detection.
• The high level of the clock is counted using the SCLH counter, starting from the SCL
high level internal detection.
The I2C detects its own SCL low level after a tSYNC1 delay depending on the SCL falling
edge, SCL input noise filters (analog + digital) and SCL synchronization to the I2CxCLK
clock. The I2C releases SCL to high level once the SCLL counter reaches the value
programmed in the SCLL[7:0] bits in the I2Cx_TIMINGR register.
The I2C detects its own SCL high level after a tSYNC2 delay depending on the SCL rising
edge, SCL input noise filters (analog + digital) and SCL synchronization to I2CxCLK clock.
The I2C ties SCL to low level once the SCLH counter is reached reaches the value
programmed in the SCLH[7:0] bits in the I2Cx_TIMINGR register.
Consequently the master clock period is:
tSCL = tSYNC1 + tSYNC2 + { [(SCLH+1) + (SCLL+1)] x (PRESC+1) x tI2CCLK }
The duration of tSYNC1 depends on these parameters:
– SCL falling slope
– When enabled, input delay induced by the analog filter.
– When enabled, input delay induced by the digital filter: DNF x tI2CCLK
– Delay due to SCL synchronization with I2CCLK clock (2 to 3 I2CCLK periods)
The duration of tSYNC2 depends on these parameters:
– SCL rising slope
– When enabled, input delay induced by the analog filter.
– When enabled, input delay induced by the digital filter: DNF x tI2CCLK
– Delay due to SCL synchronization with I2CCLK clock (2 to 3 I2CCLK periods)
*/

void i2c_master_init(unsigned int scl_freq)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint32_t pclk = stm32f_hsi_hz;
	uint32_t n;

	xfer.stat.irq_cnt = 0;
	xfer.stat.rx_cnt = 0;
	xfer.stat.tx_cnt = 0;
	xfer.stat.tc_cnt = 0;
	xfer.stat.nack_cnt = 0;

	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);

	stm32_gpio_mode(I2C1_SCL, ALT_FUNC, OPEN_DRAIN);
	stm32_gpio_mode(I2C1_SDA, ALT_FUNC, OPEN_DRAIN);

	stm32_gpio_af(I2C1_SCL, GPIO_AF4);
	stm32_gpio_af(I2C1_SDA, GPIO_AF4);

	stm32_clk_enable(STM32_RCC, STM32_CLK_I2C1);

	/* Software reset */
	i2c->cr1 = 0;

	/* I2C Timing configuration:
	  tPRESC = (PRESC+1) x tI2CCLK
	  tSCLDEL = (SCLDEL+1) x tPRESC
	  tSDADEL= SDADEL x tPRESC
	  tSCLH = (SCLH+1) x tPRESC
	  tSCLL = (SCLL+1) x tPRESC */

	n = pclk / scl_freq;
	(void)n;
	i2c->timingr = I2C_PRESC_SET(1) |
			I2C_SCLL_SET(0x13) | I2C_SCLHL_SET(0xf) |
			I2C_SDADEL_SET(2) | I2C_SCLDEL_SET(4);


	/* I2C Control register 2 (I2C_CR2) */
	i2c->cr2 = 0;
	/*	I2C Own address register 1 (I2C_OAR1) */
	i2c->oar1 = 0;
	/*	I2C Own address register 2 (I2C_OAR2) */
	i2c->oar2 = 0;

	xfer.flag = thinkos_flag_alloc();

#ifdef THINKAPP
	/* configure and Enable interrupts */
	thinkos_irq_register(STM32F_IRQ_I2C1_EV, I2C_IRQ_PRIORITY,
			stm32f_i2c1_ev_isr);
	thinkos_irq_register(STM32F_IRQ_I2C1_ER, I2C_IRQ_PRIORITY,
			stm32f_i2c1_er_isr);
#else
	cm3_irq_enable(STM32F_IRQ_I2C1_EV);
	/* set event IRQ to high priority */
	cm3_irq_pri_set(STM32F_IRQ_I2C1_EV, I2C_IRQ_PRIORITY);

	cm3_irq_enable(STM32F_IRQ_I2C1_ER);
	/* set error IRQ to high priority */
	cm3_irq_pri_set(STM32F_IRQ_I2C1_ER, I2C_IRQ_PRIORITY);
#endif

}

void i2c_master_enable(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;

	i2c->cr1 = I2C_TCIE | I2C_ERRIE | I2C_NACKIE | I2C_RXIE | I2C_TXIE | I2C_PE;
}

/*Software reset
A software reset can be performed by clearing the PE bit in the I2Cx_CR1 register. In that
case I2C lines SCL and SDA are released. Internal states machines are reset and
communication control bits, as well as status bits come back to their reset value. The
configuration registers are not impacted.
Here is the list of impacted register bits:
1. I2Cx_CR2 register: START, STOP, NACK
2. I2Cx_ISR register: BUSY, TXE, TXIS, RXNE, ADDR, NACKF, TCR, TC, STOPF,
BERR, ARLO, OVR
and in addition when the SMBus feature is supported:
1. I2Cx_CR2 register: PECBYTE
2. I2Cx_ISR register: PECERR, TIMEOUT, ALERT
PE must be kept low during at least 3 APB clock cycles in order to perform the software
reset. This is ensured by writing the following software sequence : - Write PE=0 - Check
PE=0 - Write PE=1
*/

void i2c_master_reset(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint32_t cr1;
	uint32_t cr2;
	uint32_t oar1;
	uint32_t oar2;

	cr1 = i2c->cr1;
	cr2 = i2c->cr2;
	oar1 = i2c->oar1;
	oar2 = i2c->oar2;

	/* Software reset */
	i2c->cr1 = 0;
	while (i2c->cr1 & I2C_PE);
	i2c->cr1 = I2C_PE;

	i2c->cr2 = cr2;
	i2c->oar1 = oar1;
	i2c->oar2 = oar2;
	i2c->cr1 = cr1;
}

/*
Master communication initialization (address phase)
In order to initiate the communication, you must program the following parameters for the
addressed slave in the I2Cx_CR2 register:
• Addressing mode (7-bit or 10-bit): ADD10
• Slave address to be sent: SADD[9:0]
• Transfer direction: RD_WRN
• In case of 10-bit address read: HEAD10R bit. HEAD10R must be configure to indicate
if the complete address sequence must be sent, or only the header in case of a
direction change.
• The number of bytes to be transferred: NBYTES[7:0]. If the number of bytes is equal to
or greater than 255 bytes, NBYTES[7:0] must initially be filled with 0xFF.
You must then set the START bit in I2Cx_CR2 register. Changing all the above bits is not
allowed when START bit is set.
Then the master automatically sends the START condition followed by the slave address as
soon as it detects that the bus is free (BUSY = 0) and after a delay of tBUF.
In case of an arbitration loss, the master automatically switches back to slave mode and can
acknowledge its own address if it is addressed as a slave.
*/

int i2c_master_wr(unsigned int addr, const void * buf, int len)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	int ret;

	xfer.stat.irq_cnt = 0;
	xfer.stat.rx_cnt = 0;
	xfer.stat.tx_cnt = 0;
	xfer.stat.tc_cnt = 0;
	xfer.stat.nack_cnt = 0;

	xfer.buf = (uint8_t *)buf;
	xfer.txn = len;
	xfer.cnt = 0;
	xfer.addr = addr;
	xfer.ret = 0;

	/* generate a Start condition */
	i2c->cr2 = I2C_SADD_SET(addr) | I2C_NBYTES_SET(len) |
		I2C_START | I2C_AUTOEND;

	if (thinkos_flag_timedtake(xfer.flag, 10) == THINKOS_ETIMEDOUT) {
		i2c_master_reset();
		ret = -1;
	} else
		ret = xfer.ret;

	return ret;
}

int i2c_master_xfer(unsigned int addr, const void * buf,
		unsigned int txn, unsigned int rxn)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	int ret;

	xfer.stat.irq_cnt = 0;
	xfer.stat.rx_cnt = 0;
	xfer.stat.tx_cnt = 0;
	xfer.stat.tc_cnt = 0;
	xfer.stat.nack_cnt = 0;

	xfer.buf = (uint8_t *)buf;
	xfer.txn = txn + 1;
	xfer.rxn = rxn;
	xfer.cnt = 0;
	xfer.addr = addr;
	xfer.ret = 0;

	/* generate a Start condition */
	i2c->cr2 = I2C_SADD_SET(addr) | I2C_NBYTES_SET(txn) | I2C_START;

	if (thinkos_flag_timedtake(xfer.flag, 10) == THINKOS_ETIMEDOUT) {
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

	xfer.buf = (uint8_t *)buf;
	xfer.rxn = len;
	xfer.cnt = 0;
	xfer.ret = 0;

	/* generate a Start condition */
	i2c->cr2 = I2C_SADD_SET(addr) | I2C_NBYTES_SET(len) | I2C_RD_WRN |
			I2C_START | I2C_AUTOEND;

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
	uint8_t * pkt = (uint8_t *)buf;
	int ret;

	thinkos_mutex_lock(i2c_mutex);

	pkt[0] = reg | ((n > 1) ? 0x80 : 0);


	ret = i2c_master_xfer(addr, buf, 1, n);

	thinkos_mutex_unlock(i2c_mutex);

	return ret;
}

int i2c_reg_write(unsigned int addr, unsigned int reg, const void * buf, int n)
{
	uint8_t pkt[n + 1];
	int ret;

	thinkos_mutex_lock(i2c_mutex);

	pkt[0] = reg | ((n > 1) ? 0x80 : 0);
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

