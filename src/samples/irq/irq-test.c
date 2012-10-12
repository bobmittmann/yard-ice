/* 
 * File:	 irq-test.c
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
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <stdbool.h>
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>

#define UART_FIFO_BUF_LEN 64

struct uart_fifo {
	volatile uint32_t head;
	volatile uint32_t tail;
	uint8_t buf[UART_FIFO_BUF_LEN];
};

static inline uint8_t uart_fifo_get(struct uart_fifo * fifo)
{
	return fifo->buf[fifo->tail++ & (UART_FIFO_BUF_LEN - 1)];
}

static inline void uart_fifo_put(struct uart_fifo * fifo, uint8_t c)
{
	fifo->buf[fifo->head++ & (UART_FIFO_BUF_LEN - 1)] = c;
}

static inline bool uart_fifo_is_empty(struct uart_fifo * fifo)
{
	return (fifo->tail == fifo->head) ? true : false;
}

static inline bool uart_fifo_is_full(struct uart_fifo * fifo)
{
	return ((fifo->head - fifo->tail) == UART_FIFO_BUF_LEN) ? true : false;
}


struct serial_dev {
	struct stm32f_usart * us;
	uint32_t * txeie;
	struct uart_fifo tx_fifo;
	struct uart_fifo rx_fifo;
};

int serial_write(struct serial_dev * dev, const void * buf, 
					   unsigned int len)
{
	char * cp = (char *)buf;
	int n;

	for (n = 0; n < len; n++) {
		while (uart_fifo_is_full(&dev->tx_fifo)) {
			/* enable TX interrupt */
			*dev->txeie = 1; 
			udelay(1000);
		}
		uart_fifo_put(&dev->tx_fifo, cp[n]);
	}

	return n;
}

int serial_read(struct serial_dev * dev, void * buf, unsigned int len)
{
	char * cp = (char *)buf;
	int c;
	int n = 0;

	while (uart_fifo_is_empty(&dev->rx_fifo)) {
		udelay(1000);
	}

	do {
		if (n == len)
			return n;
		c = uart_fifo_get(&dev->rx_fifo);
		cp[n++] = c;
	} while (!uart_fifo_is_empty(&dev->rx_fifo));
	
	return n;
}

void serial_open(struct serial_dev * dev, 
				 unsigned int baudrate, unsigned int flags)
{
	struct stm32f_usart * us = dev->us;

	stm32f_usart_open(us, baudrate, flags);
	cm3_irq_enable(STM32F_IRQ_UART5);

	/* enable RX interrupt */
	us->cr1 |= USART_RXNEIE;
}

const struct fileop serial_ops = {
	.write = (void *)serial_write,
	.read = (void *)serial_read,
	.flush = (void *)NULL,
	.close = (void *)NULL
};

struct serial_dev dev_ttyS5 = {
	.us = STM32F_USART5,
	.txeie = (uint32_t *)CM3_PERIPHERAL_BITBAND_ADDR(STM32F_BASE_UART5 + 
													 USART_CR1, 7)
};

void UART5_IRQHandler(void)
{
	struct serial_dev * dev = &dev_ttyS5;
	struct stm32f_usart * us = dev->us;
	uint32_t sr = us->sr;
	int c;

	if (sr & USART_TXE) {
		if (uart_fifo_is_empty(&dev->tx_fifo)) {
			*dev->txeie = 0;
		} else {
			us->dr = uart_fifo_get(&dev->tx_fifo);
		}
	}

	if (sr & USART_RXNE) {
		c = us->dr;
		if (!uart_fifo_is_full(&dev->rx_fifo)) {
			uart_fifo_put(&dev->rx_fifo, c);
		}
	}	
}

const struct file dev_ttyS_file = {
	.data = &dev_ttyS5,
	.op = &serial_ops
};

FILE * stdout = (FILE *)&dev_ttyS_file;
FILE * stdin = (FILE *)&dev_ttyS_file;

char * readline(const char * prompt, char * buf, int len)
{
	char * s;

	printf("%s", prompt);

	buf[0] = '\0';

	while ((s = fgets(buf, len, stdin)) == NULL);

	return s;
}

int main(int argc, char ** argv)
{
	char buf[256];
	int i;

	cm3_udelay_calibrate();

	serial_open(&dev_ttyS5, 9600, SERIAL_8N1);

	printf("\r\n");
	printf("- IRQ test....\r\n");
	printf("\r\n");

	for (i = 0; i < 10; i++) {
		printf("Hello world! %d\r\n", i);
	}

	for (;;) {
		readline("[TEST]# ", buf, 256);
		printf(buf);
	}

	return 0;
}


