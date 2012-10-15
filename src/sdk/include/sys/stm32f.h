/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
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
 * @file sys/stm32f.h
 * @brief YARD-ICE libstm32f
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __SYS_STM32F_H__
#define __SYS_STM32F_H__

#ifdef STM32F207
#include <arch/stm32f207.h>
#endif

#define STM32F

#include <arch/cortex-m3.h>

#define STM32F_HSI_HZ 16000000
#define STM32F_HSE_HZ 24000000
#define STM32F_PLL_HZ 120000000
#define STM32F_APB1_HZ 30000000
#define STM32F_APB2_HZ 60000000

#define STM32F_AHB_HZ 120000000

#ifndef __ASSEMBLER__

typedef enum {
	STM32F_UART1_ID = 0,
	STM32F_UART2_ID, 
	STM32F_UART3_ID,
	STM32F_UART4_ID,
	STM32F_UART5_ID,
	STM32F_UART6_ID
} stmf32_uart_id_t;


#if 0
enum {
	OUT_PUSH_PULL = 0,
	OUT_OPEN_DRAIN = 1
};

enum {
	PULL_UP = 1,
	PULL_DOWN = 2
};

enum {
	SPEED_LOW = 0,
	SPEED_MED = 1,
	SPEED_FAST = 2,
	SPEED_HIGH = 3
};
#endif

enum {
	INPUT = 0,
	OUTPUT,
	ALT_FUNC,
	ANALOG
};

#define	SPEED_LOW (0 << 0)
#define	SPEED_MED (1 << 0)
#define	SPEED_FAST (2 << 0)
#define	SPEED_HIGH (3 << 0)

#define OPT_SPEED(OPT) (((OPT) >> 0) & 0x3)

#define PUSH_PULL (0 << 4)
#define OPEN_DRAIN (1 << 4)

#define PULL_UP (1 << 5)
#define PULL_DOWN (1 << 6)

extern const struct stm32f_gpio * stm32f_gpio_lut[];

#define PA STM32F_GPIOA_ID
#define PB STM32F_GPIOB_ID
#define PC STM32F_GPIOC_ID
#define PD STM32F_GPIOD_ID
#define PE STM32F_GPIOE_ID
#define PF STM32F_GPIOF_ID
#define PG STM32F_GPIOG_ID
#define PH STM32F_GPIOH_ID
#define PI STM32F_GPIOI_ID

struct stm32f_gpio_io {
	uint8_t pin:4;
	uint8_t port:4;
} __attribute__((__packed__));

typedef struct stm32f_gpio_io gpio_io_t;

#define GPIO(PORT, PIN) (struct stm32f_gpio_io){ .port = PORT, .pin = PIN }

/* set output */
static inline void gpio_set(gpio_io_t __io) {
	struct stm32f_gpio * gpio = STM32F_GPIO(__io.port);
	gpio->bsrrl = GPIO_SET(__io.pin);
};

/* clear output */
static inline void gpio_clr(gpio_io_t __io) {
	struct stm32f_gpio * gpio = STM32F_GPIO(__io.port);
	gpio->bsrrh = GPIO_RESET(__io.pin);
};

/* pin status */
static inline int gpio_status(gpio_io_t __io) {
	struct stm32f_gpio * gpio = STM32F_GPIO(__io.port);
	return (gpio->idr & (1 << __io.pin));
};

struct stm32f_spi_io {
	struct stm32f_gpio_io miso;
	struct stm32f_gpio_io mosi;
	struct stm32f_gpio_io sck;
} __attribute__((__packed__));


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------
 * GPIO
 *---------------------------------------------------------------------*/
int stm32f_gpio_id(struct stm32f_gpio * gpio);

void stm32f_gpio_clock_en(struct stm32f_gpio * gpio);

/* Output mode */
void stm32f_gpio_output(struct stm32f_gpio * gpio, unsigned int pin, 
						int type, int speed);

/* mode */
void stm32f_gpio_mode(struct stm32f_gpio * gpio, unsigned int pin, 
					  unsigned int mode, unsigned int opt);

/* Alternate function selection */
void stm32f_gpio_af(struct stm32f_gpio * gpio, int port, int af);


/*---------------------------------------------------------------------
 * USART 
 *---------------------------------------------------------------------*/
int stm32f_usart_putc(struct stm32f_usart * usart, int c);

int stm32f_usart_getc(struct stm32f_usart * usart, unsigned int msec);

int stm32f_usart_read(struct stm32f_usart * usart, char * buf, 
					  unsigned int len, unsigned int msec);

int stm32f_usart_write(struct stm32f_usart * usart, const void * buf, 
					   unsigned int len);

int stm32f_usart_flush(struct stm32f_usart * usart);

int stm32f_usart_init(struct stm32f_usart * us,
					  unsigned int baudrate, unsigned int flags);

int stm32f_usart_release(struct stm32f_usart * usart);

struct file * stm32f_usart_open(struct stm32f_usart * us,
								unsigned int baudrate, unsigned int flags);

/*---------------------------------------------------------------------
 * SPI
 *---------------------------------------------------------------------*/

int stm32f_spi_init(struct stm32f_spi * spi, const struct stm32f_spi_io * io);
int stm32f_spi_putc(struct stm32f_spi * spi, int c);
int stm32f_spi_getc(struct stm32f_spi * spi);

/*---------------------------------------------------------------------
 * Ethernet
 *---------------------------------------------------------------------*/
void stm32f_eth_init(struct stm32f_eth * eth);
void stm32f_eth_mac_get(struct stm32f_eth * eth, int idx, uint8_t * mac);
void stm32f_eth_mac_set(struct stm32f_eth * eth, int idx, const uint8_t * mac);

#include <sys/file.h>

const struct fileop stm32f_usart_ops;

const struct file stm32f_usart5_file;


#ifdef __cplusplus
}
#endif

static inline void gpio_mode_set(gpio_io_t __io, unsigned int __mode, 
								 unsigned int __opt) {
	struct stm32f_gpio * gpio = STM32F_GPIO(__io.port);

	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, __io.pin, __mode, __opt);
}

#endif /* __ASSEMBLER__ */

#endif /* __SYS_STM32F_H__ */

