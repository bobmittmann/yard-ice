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
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#include <thinkos.h>

#include <sys/dcclog.h>

void i2c_reset(void);

/* GPIO pin description */ 
struct stm32f_io {
	struct stm32f_gpio * gpio;
	uint8_t pin;
};

/* ----------------------------------------------------------------------
 * LEDs 
 * ----------------------------------------------------------------------
 */

#ifdef STM32F1X
const struct stm32f_io led_io[] = {
	{ STM32F_GPIOB, 14 },
	{ STM32F_GPIOB, 15 },
	{ STM32F_GPIOC, 13 },
	{ STM32F_GPIOC, 14 },
	{ STM32F_GPIOC, 15 }
};
#endif

#ifdef STM32F4X
const struct stm32f_io led_io[] = {
	{ STM32F_GPIOC, 1 },
	{ STM32F_GPIOC, 14 },
	{ STM32F_GPIOC, 7 },
	{ STM32F_GPIOC, 8 }
};
#endif

void led_on(int id)
{
	stm32f_gpio_set(led_io[id].gpio, led_io[id].pin);
}

void led_off(int id)
{
	stm32f_gpio_clr(led_io[id].gpio, led_io[id].pin);
}

void leds_init(void)
{
	int i;

	for (i = 0; i < sizeof(led_io) / sizeof(struct stm32f_io); ++i) {
		stm32f_gpio_mode(led_io[i].gpio, led_io[i].pin,
						 OUTPUT, PUSH_PULL | SPEED_LOW);

		stm32f_gpio_clr(led_io[i].gpio, led_io[i].pin);
	}
}


/* ----------------------------------------------------------------------
 * Console 
 * ----------------------------------------------------------------------
 */

#define USART1_TX STM32F_GPIOB, 6
#define USART1_RX STM32F_GPIOB, 7

struct file stm32f_uart1_file = {
	.data = STM32F_USART1, 
	.op = &stm32f_usart_fops 
};

void stdio_init(void)
{
	struct stm32f_usart * uart = STM32F_USART1;
#if defined(STM32F1X)
	struct stm32f_afio * afio = STM32F_AFIO;
#endif

	DCC_LOG(LOG_TRACE, "...");

	/* USART1_TX */
	stm32f_gpio_mode(USART1_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);

#if defined(STM32F1X)
	/* USART1_RX */
	stm32f_gpio_mode(USART1_RX, INPUT, PULL_UP);
	/* Use alternate pins for USART1 */
	afio->mapr |= AFIO_USART1_REMAP;
#elif defined(STM32F4X)
	stm32f_gpio_mode(USART1_RX, ALT_FUNC, PULL_UP);
	stm32f_gpio_af(USART1_RX, GPIO_AF7);
	stm32f_gpio_af(USART1_TX, GPIO_AF7);
#endif

	stm32f_usart_init(uart);
	stm32f_usart_baudrate_set(uart, 115200);
	stm32f_usart_mode_set(uart, SERIAL_8N1);
	stm32f_usart_enable(uart);

	stdin = &stm32f_uart1_file;
	stdout = &stm32f_uart1_file;
	stderr = &stm32f_uart1_file;
}

/* ----------------------------------------------------------------------
 * I/O 
 * ----------------------------------------------------------------------
 */

void stm32f_exti9_5_isr(void)
{
	struct stm32f_exti * exti = STM32F_EXTI;
	static uint32_t window_tmo;
	static bool enabled = true;
	uint32_t ticks;
	
	/* Clear pending flag */
	exti->pr = (1 << 9);

	ticks = __thinkos_ticks();
	if (enabled) {
		DCC_LOG(LOG_TRACE, "EVENT");
		/* 50 ms debouncing window */
		window_tmo = ticks + 100;
		enabled = false;
	} else {
		DCC_LOG(LOG_MSG, "GLITCH");
		if (((int32_t)(ticks - window_tmo)) > 0) {
			enabled = true;
		}
	}
}


#define PUSH_BTN STM32F_GPIOC, 9

void io_init(void)
{
#if defined(STM32F1X)
	struct stm32f_rcc * rcc = STM32F_RCC;

	/* Enable Alternate Functions IO clock */
	rcc->apb2enr |= RCC_AFIOEN;
#endif
#if defined(STM32F4X)
	struct stm32f_exti * exti = STM32F_EXTI;
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;
	struct stm32f_rcc * rcc = STM32F_RCC;
#endif

	DCC_LOG(LOG_MSG, "Configuring GPIO pins...");

	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOC);

#if defined(STM32F4X)
	stm32f_gpio_mode(STM32F_GPIOB, 10, INPUT, 0);
	stm32f_gpio_mode(STM32F_GPIOB, 11, INPUT, 0);
	stm32f_gpio_mode(PUSH_BTN, INPUT, PULL_UP);

	/* System configuration controller clock enable */
	rcc->apb2enr |= RCC_SYSCFGEN;

	/* Select PC9 for EXTI9 */ 
	syscfg->exticr3 = SYSCFG_EXTI9_PC;
	/* Unmask interrupt */
	exti->imr |= (1 << 9);
	/* Select falling edge trigger */
	exti->ftsr |= (1 << 9);

//	cm3_irq_enable(STM32F_IRQ_EXTI9_5);
#endif

}

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
	int32_t event;
};

struct i2c_xfer xfer;

#define I2C1_SCL STM32F_GPIOB, 8
#define I2C1_SDA STM32F_GPIOB, 9

void i2c_master_init(unsigned int scl_freq)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	struct stm32f_rcc * rcc = STM32F_RCC;
	uint32_t pclk = stm32f_apb1_hz;
#if defined(STM32F1X)
	struct stm32f_afio * afio = STM32F_AFIO;
	/* Use alternate pins for I2C1 */
	afio->mapr |= AFIO_I2C1_REMAP;
#endif

#if 0
	stm32f_gpio_mode(I2C1_SCL, INPUT, 0);
	stm32f_gpio_mode(I2C1_SDA, INPUT, 0);

	printf("%s() SDA=%d SCL=%d\n", __func__,
		   stm32f_gpio_stat(I2C1_SDA) ? 1 : 0,
		   stm32f_gpio_stat(I2C1_SCL) ? 1 : 0);

#endif

	stm32f_gpio_mode(I2C1_SCL, ALT_FUNC, OPEN_DRAIN);
	stm32f_gpio_mode(I2C1_SDA, ALT_FUNC, OPEN_DRAIN);

#if defined(STM32F4X)
	stm32f_gpio_af(I2C1_SCL, GPIO_AF4);
	stm32f_gpio_af(I2C1_SDA, GPIO_AF4);
#endif

	/* Enable I2C clock */
	rcc->apb1enr |= RCC_I2C1EN;

	printf("%s() i2c=0x%08x\n", __func__, (uint32_t)i2c);

	/* Software reset */
	i2c->cr1 = I2C_SWRST; 
	udelay(100);


	printf("CR1=0x%04x CR2=0x%04x CCR=0x%04x\n", 
			 i2c->cr1, i2c->cr2, i2c->ccr);

	printf("OAR1=0x%04x OAR2=0x%04x TRISE=0x%04x\n", 
			 i2c->oar1, i2c->oar2, i2c->trise);

	printf("SR1=0x%04x SR2=0x%04x\n", i2c->sr1, i2c->sr2);


	DCC_LOG3(LOG_TRACE, "CR1=0x%04x CR2=0x%04x CCR=0x%04x", 
			 i2c->cr1, i2c->cr2, i2c->ccr);

	DCC_LOG3(LOG_TRACE, "OAR1=0x%04x OAR2=0x%04x TRISE=0x%04x", 
			 i2c->oar1, i2c->oar2, i2c->trise);

	DCC_LOG2(LOG_TRACE, "SR1=0x%04x SR2=0x%04x ", i2c->sr1, i2c->sr2);

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

	printf("%s() scl_freq=%d\n", __func__, scl_freq);
	printf("%s() SR1=0x%04x SR2=0x%04x\n", __func__, i2c->sr1, i2c->sr2);


	printf("CR1=0x%04x CR2=0x%04x CCR=0x%04x\n", 
			 i2c->cr1, i2c->cr2, i2c->ccr);

	printf("OAR1=0x%04x OAR2=0x%04x TRISE=0x%04x\n", 
			 i2c->oar1, i2c->oar2, i2c->trise);

	printf("SR1=0x%04x SR2=0x%04x\n", i2c->sr1, i2c->sr2);

	xfer.event = thinkos_ev_alloc();

	printf("event=%d\n", xfer.event);
}

void i2c_master_enable(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;

	cm3_irq_enable(STM32F_IRQ_I2C1_EV);
	cm3_irq_enable(STM32F_IRQ_I2C1_ER);

	DCC_LOG(LOG_TRACE, "Enabling interrupts....");
	/* events and errors */
	i2c->cr2 |= I2C_ITERREN | I2C_ITEVTEN | I2C_ITBUFEN;

	DCC_LOG(LOG_TRACE, "Enabling device ....");

	i2c->cr1 = I2C_PE;

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

	printf("CR1=0x%04x CR2=0x%04x CCR=0x%04x\n", cr1, cr2, ccr);
	printf("OAR1=0x%04x OAR2=0x%04x TRISE=0x%04x\n", oar1, oar2, trise);
	printf("SR1=0x%04x SR2=0x%04x\n", i2c->sr1, i2c->sr2);

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

		if (xfer.addr & 1) {
			DCC_LOG1(LOG_INFO, "%d SB (recv)", i2c_irq_cnt);
		} else {
			DCC_LOG1(LOG_INFO, "%d SB (xmit)", i2c_irq_cnt);
		}
	}

	if (sr1 & I2C_ADDR) {
		DCC_LOG1(LOG_INFO, "%d ADDR", i2c_irq_cnt);
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
		DCC_LOG1(LOG_TRACE, "%d BTF", i2c_irq_cnt);
		if (xfer.addr & 1) {
			goto do_recv;
		} else {
			goto do_xmit;
		}
	}

	if (sr1 & I2C_RXNE) {
		DCC_LOG1(LOG_INFO, "%d RXNE", i2c_irq_cnt);
do_recv:
		*xfer.ptr++ = i2c->dr;
		xfer.rem--;
		if (xfer.rem == 0) {
			xfer.ret = xfer.cnt;
			__thinkos_ev_timed_raise(xfer.event);
		} else if (xfer.rem == 1) {
			/* Clear ACK */
			i2c->cr1 = I2C_STOP | I2C_PE; 
		}
	}

	if (sr1 & I2C_TXE) {
do_xmit:
		if (xfer.rem > 0) {
			i2c->dr = *xfer.ptr++;
			DCC_LOG1(LOG_INFO, "%d TXE", i2c_irq_cnt);
		} else if (xfer.rem == 0) {
			/* Program STOP. */
			i2c->cr1 = I2C_STOP | I2C_PE; 
			/* Clear the TXE flag */
			i2c->dr = 0;
			DCC_LOG1(LOG_INFO, "%d TXE STOP", i2c_irq_cnt);
		} else {
			i2c->dr = 0;
			DCC_LOG1(LOG_INFO, "%d TXE ?", i2c_irq_cnt);
			xfer.ret = xfer.cnt;
			__thinkos_ev_timed_raise(xfer.event);
		} 
		xfer.rem--;
//		DCC_LOG1(LOG_TRACE, "%d TXE", i2c_irq_cnt);
	}
}

void stm32f_i2c1_er_isr(void)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint32_t sr1;

	sr1 = i2c->sr1;

	i2c_irq_cnt++;

	if (sr1 & I2C_BERR) {
		DCC_LOG(LOG_TRACE, "BERR");
		i2c->sr1 = 0;
		xfer.ret = -1;
		__thinkos_ev_timed_raise(xfer.event);
	}

	if (sr1 & I2C_ARLO) {
		DCC_LOG(LOG_TRACE, "ARLO");
		i2c->sr1 = 0;
		xfer.ret = -1;
		__thinkos_ev_timed_raise(xfer.event);
	}

	if (sr1 & I2C_AF) {
		DCC_LOG1(LOG_INFO, "%d AF", i2c_irq_cnt);
		/* clear AF */
		i2c->sr1 = 0;
		i2c->cr1 = I2C_STOP | I2C_PE; /* generate a Stop condition */
		xfer.ret = -1;
		__thinkos_ev_timed_raise(xfer.event);
	}

	if (sr1 & I2C_OVR) {
		DCC_LOG(LOG_TRACE, "OVR");
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

	DCC_LOG2(LOG_INFO, "addr=0x%02x len=%d", addr, len);

	i2c->cr1 = I2C_START | I2C_ACK | I2C_PE; /* generate a Start condition */

	while ((ret = xfer.ret) == -2) {
		if (thinkos_ev_timedwait(xfer.event, 100) == THINKOS_ETIMEDOUT) {
			DCC_LOG(LOG_TRACE, "Timeout...");
			i2c_reset();
			ret = -1;
			break;
		}
	}

	DCC_LOG1(LOG_INFO, "ret=%d", ret);

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

	DCC_LOG2(LOG_INFO, "addr=0x%02x len=%d", addr, len);

	i2c->cr1 = I2C_START | I2C_ACK | I2C_PE; /* generate a Start condition */

	while ((ret = xfer.ret) == -2) {
		if (thinkos_ev_timedwait(xfer.event, 100) == THINKOS_ETIMEDOUT) {
			DCC_LOG(LOG_TRACE, "Timeout...");
			i2c_reset();
			ret = -1;
			break;
		}
	}

	DCC_LOG1(LOG_INFO, "ret=%d", ret);

	return ret;
}

int _i2c_master_rd(unsigned int addr, void * buf, int len)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint8_t * ptr = (uint8_t *)buf;
	uint32_t sr1;
	uint32_t sr2;
	int again;
	int rem;

//	printf("%s() addr=%d\n", __func__, addr);

//	printf("1. START");
	DCC_LOG(LOG_INFO, "1. START");

	i2c->cr1 = I2C_START | I2C_PE; /* generate a Start condition */

	/* Once the Start condition is sent:
	● The SB bit is set by hardware and an interrupt is generated if 
	the ITEVFEN bit is set.
	Then the master waits for a read of the SR1 register followed by 
	a write in the DR register with the Slave address (see Figure 237 
	& Figure 238 Transfer sequencing EV5). */

	again = 0;
	while (((sr1 = i2c->sr1) & I2C_SB) == 0) {
		/* If for some reason the I2C lines get stuck
		   at high level. The start bit will
		   never be reported. No other error will be reported also.
			TODO: Implement a timeout mechanism ... */

//		printf("%s() SDA=%d SCL=%d\n", __func__,
//			   stm32f_gpio_stat(I2C1_SDA) ? 1 : 0,
//			   stm32f_gpio_stat(I2C1_SCL) ? 1 : 0);

		(void)sr1;
//		sr2 = i2c->sr2;;
//		(void)sr2;
//		if ((sr1 != 0) || (sr2 != 0)) {
//			printf("CR1=0x%04x SR1=0x%04x SR2=0x%04x\n", i2c->cr1, sr1, sr2);
//			DCC_LOG2(LOG_INFO, "SR1=0x%04x SR2=0x%04x", sr1, sr2);
//		}
		if (++again == 100)
			goto abort;

		udelay(10);
	}


	/* – To enter Receiver mode, a master sends the slave 
	   address with LSB set. */
//	printf(", 2. ADDR %d", addr);
//	printf("1.");
//	udelay(100);
	DCC_LOG1(LOG_INFO, "2. ADDR %d", addr);
	i2c->dr = (addr << 1) | 1;

	if (len == 1) {
		/* ● Case of a single byte to be received:
		   – In the ADDR event, clear the ACK bit.
		   – Clear ADDR
		   – Program the STOP/START bit.
		   – Read the data after the RxNE flag is set. */

//		printf("2.");
//		printf("~");
		again = 0;
		while (((sr1 = i2c->sr1) & I2C_ADDR) == 0) {
			/* Acknowledge failure */
			if (sr1 & I2C_AF )
				goto abort;
			if (++again == 100)
				goto abort;
			udelay(10);
		}

		/* Clear ADDR */
		sr2 = i2c->sr2;
		(void)sr2;
	
		/* Program the STOP/START bit. */
		i2c->cr1 = I2C_STOP | I2C_PE; 
	
//		printf("^");
		/* Read the data after the RxNE flag is set. */
		while ((i2c->sr1 & I2C_RXNE) == 0);

		DCC_LOG(LOG_TRACE, "3. STOP/DATA");
//		printf("3. STOP/DATA\n");
	} else if  (len == 2) {
		/* ● Case of two bytes to be received:
		   – Set POS and ACK
		   – Wait for the ADDR flag to be set
		   – Clear ADDR
		   – Clear ACK
		   – Wait for BTF to be set
		   – Program STOP
		   – Read DR twice */

		/* Set POS and ACK */
		i2c->cr1 = I2C_POS | I2C_ACK | I2C_PE; 

		/* Wait for the ADDR flag to be set */
		while (((sr1 = i2c->sr1) & I2C_ADDR) == 0) {
			/* Acknowledge failure */
			if (sr1 & I2C_AF )
				goto abort;
			if (++again == 100)
				goto abort;
			udelay(10);
		}

		/* Clear ADDR */
		sr2 = i2c->sr2; 
		(void)sr2;

		DCC_LOG1(LOG_TRACE, "CR1=0x%04x", i2c->cr1);
		/* Clear ACK ???? */
		i2c->cr1 = I2C_POS | I2C_PE; 

		/* Wait for BTF to be set */
		while (((sr1 = i2c->sr1) & I2C_BTF) == 0) {
			/* Acknowledge failure */
			if (sr1 & I2C_AF )
				goto abort;
			if (++again == 100)
				goto abort;
			udelay(10);
		}

		/* Program STOP */
		i2c->cr1 = I2C_STOP | I2C_PE; 

		/* Read DR twice */
		*ptr++ = i2c->dr;
	} else {
		rem = len;
		while (rem > 3) {
		}
		/* When 3 bytes remain to be read:
		   ● RxNE = 1 => Nothing (DataN-2 not read).
		   ● DataN-1 received
		   ● BTF = 1 because both shift and data registers are full: 
		   DataN-2 in DR and DataN-1 in the shift register => SCL tied low: 
		   no other data will be received on the bus.
		   ● Clear ACK bit
		   ● Read DataN-2 in DR => This will launch the DataN reception 
		   in the shift register
		   ● DataN received (with a NACK)
		   ● Program START/STOP
		   ● Read DataN-1
		   ● RxNE = 1
		   ● Read DataN
		 */
	}

	*ptr = i2c->dr;

	return len;

abort:
	DCC_LOG2(LOG_TRACE, "Abort: SR1=0x%04x SR2=0x%04x", i2c->sr1, i2c->sr2);
//	printf("?");
	sr1 = i2c->sr1 = 0;
	i2c->cr1 = I2C_STOP | I2C_PE; /* generate a Stop condition */

	sr1 = i2c->sr1;
	sr2 = i2c->sr2;
	
	if ((sr1 != 0) || (sr2 != 0)) {
//		printf("\nAbort: SR1=0x%04x SR2=0x%04x\n", i2c->sr1, i2c->sr2);
		DCC_LOG2(LOG_TRACE, "Reset: SR1=0x%04x SR2=0x%04x", i2c->sr1, i2c->sr2);
	}
//	printf("!");

	i2c->cr1 = I2C_STOP | I2C_PE; 
	again = 0;
	while (i2c->sr2 & I2C_BUSY) {
		if (++again == 1000)
			break;
		udelay(10);
	};

//	i2c->cr1 = 0; 
	i2c->cr1 = I2C_PE; 
#if 0
	udelay(100000);

	sr1 = i2c->sr1;
	sr2 = i2c->sr2;
	
	if ((sr1 != 0) || (sr2 != 0)) {
		printf("stick: SR1=0x%04x SR2=0x%04x\n", sr1, sr2);
	}
#endif
	return -1;
}

int _i2c_master_wr(unsigned int addr, const void * buf, int len)
{
	struct stm32f_i2c * i2c = STM32F_I2C1;
	uint8_t * ptr = (uint8_t *)buf;
	uint32_t sr1;
	uint32_t sr2;
	int again;
	int rem;

	DCC_LOG(LOG_INFO, "1. START");

	i2c->cr1 = I2C_START | I2C_PE; /* generate a Start condition */

	/* Once the Start condition is sent:
	● The SB bit is set by hardware and an interrupt is generated if 
	the ITEVFEN bit is set.
	Then the master waits for a read of the SR1 register followed by 
	a write in the DR register with the Slave address (see Figure 237 
	& Figure 238 Transfer sequencing EV5). */
	again = 0;
	while ((i2c->sr1 & I2C_SB) == 0) {
		if (++again == 100)
			goto abort;
		udelay(10);
	}

	/* – To enter Transmitter mode, a master sends the slave 
	   address with LSB reset. */
	DCC_LOG1(LOG_INFO, "2. ADDR %d", addr);
	i2c->dr = (addr << 1);

	again = 0;
	while (((sr1 = i2c->sr1) & I2C_ADDR) == 0) {
		/* Acknowledge failure */
		if (sr1 & I2C_AF )
			goto abort;
		if (++again == 100)
			goto abort;
		udelay(10);
	}

	/* Clear ADDR */
	sr2 = i2c->sr2;
	(void)sr2;

	rem = len;

	again = 0;
	while (rem > 0) {
		while ((sr1 = (i2c->sr1 & I2C_TXE)) == 0) {
			(void)sr1;
		}

		/* Acknowledge failure */
		if (sr1 & I2C_AF ) {
			goto abort;
		}

		i2c->dr = *ptr++;

		rem--;
	}

//	DCC_LOG(LOG_TRACE, "2.");
	while ((i2c->sr1 & I2C_BTF) == 0);

	/* Program STOP. */
	i2c->cr1 = I2C_STOP | I2C_PE; 

	return len;

abort:
	DCC_LOG2(LOG_TRACE, "Abort: SR1=0x%04x SR2=0x%04x", i2c->sr1, i2c->sr2);
	i2c->sr1 = 0;
	i2c->cr1 = I2C_STOP | I2C_PE; /* generate a Start condition */

	while (i2c->sr2 & I2C_BUSY) {
	};

	return -1;
}

/* ----------------------------------------------------------------------
 * Supervisory task
 * ----------------------------------------------------------------------
 */
int supervisor_task(void)
{
	printf("%s() started...\n", __func__);

	for (;;) {
		DCC_LOG(LOG_INFO, "...");
		thinkos_sleep(200);
		led_on(3);
		thinkos_sleep(100);
		led_off(3);
		thinkos_sleep(400);
	}
}

void system_reset(void)
{
	DCC_LOG(LOG_TRACE, "...");

	CM3_SCB->aircr =  SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;

	for(;;);

}

/* ----------------------------------------------------------------------
 * User interface task
 * ----------------------------------------------------------------------
 */
int ui_task(void)
{
	int btn_st[2];
	int ev_press;
	int ev_release;
	int rst_tmr = 0;


	printf("%s() started...\n", __func__);

	btn_st[0] = stm32f_gpio_stat(PUSH_BTN) ? 0 : 1;
	for (;;) {
		/* process push button */
		btn_st[1] = stm32f_gpio_stat(PUSH_BTN) ? 0 : 1;
		ev_press = btn_st[1] & (btn_st[1] ^ btn_st[0]);
		ev_release = btn_st[0] & (btn_st[1] ^ btn_st[0]);
		btn_st[0] = btn_st[1];

		if (ev_press) {
			DCC_LOG(LOG_TRACE, "BTN Down");
			printf("\nI2C reset...\n");
			i2c_reset();
			/* set reset timer */
			rst_tmr = 50;
		}

		if (ev_release) {
			DCC_LOG(LOG_TRACE, "BTN Up");
			/* clear 'reset timer' */
			rst_tmr = 0;
		}

		if (rst_tmr)
			rst_tmr--;

		switch (rst_tmr) {
		case 18:
		case 16:
		case 14:
		case 12:
		case 10:
		case 8:
		case 4:
		case 2:
			led_on(0);
			led_on(1);
			led_on(2);
			led_on(3);
			break;
		case 17:
		case 15:
		case 13:
		case 11:
		case 9:
		case 7:
		case 5:
		case 3:
			led_off(0);
			led_off(1);
			led_off(2);
			led_off(3);
			break;
		case 1:
			system_reset();
			break;
		}

		thinkos_sleep(100);
	}
}

int32_t i2c_mutex;
int32_t phif_addr = 0x55;
int32_t codec_addr = 64;

#define PHIF_ID_REG 0
#define PHIF_ADC_REG 2
#define PHIF_LED_REG 12
#define PHIF_RLY_REG 13
#define PHIF_VR0_REG 14
#define PHIF_VR1_REG 15

int acq_task(void)
{
	uint16_t adc[5];
	uint8_t reg;
	int i;

	DCC_LOG(LOG_TRACE, "started...");
	printf("%s() started...\n", __func__);

	for (;;) {
		DCC_LOG(LOG_INFO, "Poll...");
		thinkos_sleep(1000);
		thinkos_mutex_lock(i2c_mutex);
		reg = PHIF_ADC_REG;
		if (i2c_master_wr(phif_addr, &reg, 1) > 0) {
			DCC_LOG(LOG_INFO, "i2c_master_wr().");
			if (i2c_master_rd(phif_addr, adc, sizeof(adc)) > 0) {
				DCC_LOG5(LOG_TRACE, "ADC %5d %5d %5d %5d %5d",
						 adc[0], adc[1], adc[2], adc[3], adc[4]);
				printf("ADC: ");
				for (i = 0; i < 5; ++i) {
					printf("%5d", adc[i]);
				}
				printf("\n");
			} else {
				DCC_LOG(LOG_WARNING, "i2c_master_rd() failed!");
			}
		} else {
			DCC_LOG(LOG_WARNING, "i2c_master_wr() failed!");
		}
		thinkos_mutex_unlock(i2c_mutex);
	}
}

void vr_set(unsigned int val0, unsigned int val1)
{
	uint8_t pkt[3];

	DCC_LOG2(LOG_TRACE, "vr0=%d vr1=%d", val0, val1);

	thinkos_mutex_lock(i2c_mutex);

	pkt[0] = PHIF_VR0_REG;
	pkt[1] = val0;
	pkt[2] = val1;

	if (i2c_master_wr(phif_addr, pkt, 3) > 0) {
		DCC_LOG(LOG_INFO, "ok.");
	} else {
		DCC_LOG(LOG_WARNING, "i2c_master_wr() failed!");
	}

	thinkos_mutex_unlock(i2c_mutex);
}

void i2c_bus_scan(void)
{
	uint8_t buf[4];
	uint8_t addr = 0;

	thinkos_mutex_lock(i2c_mutex);

	printf("- I2C bus scan: ");

	/* 7 bit addresses range from 0 to 0x78 */
	for (addr = 1; addr < 0x78; ++addr) {

		DCC_LOG1(LOG_TRACE, "Addr=0x%02x", addr);

		buf[0] = 0;
		if (i2c_master_wr(addr, buf, 1) <= 0) {
			printf(".");
			continue;
		}

		printf("\nI2C device found @ %d", addr);

		if (i2c_master_rd(addr, buf, 2) != 2) {
			printf("\n");
			continue;
		}

		DCC_LOG3(LOG_TRACE, "Addr=0x%02x [0]--> 0x%02x%02x", 
				 addr, buf[1], buf[0]);
		printf(" 0x%02x%02x\n", buf[1], buf[0]);

		if ((buf[0] == 'P') && (buf[1] == 'H')) {
			printf(" Phone Interface.\n");
			DCC_LOG1(LOG_TRACE, "Phone Interface @ 0x%02x", addr);
			phif_addr = addr;
		}
	}

	DCC_LOG(LOG_TRACE, "done.");
	thinkos_mutex_unlock(i2c_mutex);

	printf("\n");
}

uint32_t supervisor_stack[256];
uint32_t ui_stack[256];
uint32_t acq_stack[256];

int main(int argc, char ** argv)
{
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "2. leds_init()");
	leds_init();

	DCC_LOG(LOG_TRACE, "3. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));

	DCC_LOG(LOG_TRACE, "4. stdio_init()");
	stdio_init();

	printf("\n\n");
	printf("-----------------------------------------\n");
	printf(" I2C master test\n");
	printf("-----------------------------------------\n");
	printf("\n");

	DCC_LOG(LOG_TRACE, "5. i2c_master_init()");
	i2c_master_init(100000);

	DCC_LOG(LOG_TRACE, "6. i2c_master_enable()");
	i2c_master_enable();

	thinkos_sleep(100);

	i2c_mutex = thinkos_mutex_alloc();
	printf("I2C mutex=%d\n", i2c_mutex);

	i2c_bus_scan();

	thinkos_thread_create((void *)supervisor_task, (void *)NULL,
						  supervisor_stack, sizeof(supervisor_stack), 
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	thinkos_thread_create((void *)ui_task, (void *)NULL,
						  ui_stack, sizeof(ui_stack), 
						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));

	thinkos_thread_create((void *)acq_task, (void *)NULL,
						  acq_stack, sizeof(acq_stack), 
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));

	for (i = 0; ; ++i) {
		thinkos_sleep(3000);
		vr_set(i, 2 * i);
	}


	return 0;
}

