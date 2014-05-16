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

#ifdef STM32F405
#include <arch/stm32f405.h>
#endif

#ifdef STM32F207
#include <arch/stm32f207.h>
#endif

#if defined(STM32F303) || defined(STM32F303XB) || defined(STM32F303XC)
#include <arch/stm32f303.h>
#endif

#if defined(STM32F103) || defined(STM32F103X6)
#include <arch/stm32f103.h>
#endif

#ifdef STM32F100
#include <arch/stm32f100.h>
#endif

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <stdbool.h>
#include <arch/cortex-m3.h>

extern const uint32_t stm32f_apb1_hz;
extern const uint32_t stm32f_tim1_hz;
extern const uint32_t stm32f_apb2_hz;
extern const uint32_t stm32f_tim2_hz;
extern const uint32_t stm32f_ahb_hz;

/*---------------------------------------------------------------------
 * USB Device
 *---------------------------------------------------------------------*/
#include <sys/usb-dev.h>
extern const usb_dev_t stm32f_usb_fs_dev;
extern const usb_dev_t stm32f_otg_fs_dev;

enum {
	INPUT = 0,
	OUTPUT,
	ALT_FUNC,
	ANALOG
};

#define GPIOA STM32F_GPIOA_ID
#define GPIOB STM32F_GPIOB_ID
#define GPIOC STM32F_GPIOC_ID
#define GPIOD STM32F_GPIOD_ID
#define GPIOE STM32F_GPIOE_ID
#define GPIOF STM32F_GPIOF_ID
#define GPIOG STM32F_GPIOG_ID
#define GPIOH STM32F_GPIOH_ID
#define GPIOI STM32F_GPIOI_ID

extern const struct stm32f_gpio * const stm32f_gpio_lut[];

struct stm32f_gpio_io {
	uint8_t pin:4;
	uint8_t port:4;
} __attribute__((__packed__));

typedef struct stm32f_gpio_io gpio_io_t;

#define GPIO(PORT, PIN) (struct stm32f_gpio_io){ .port = PORT, .pin = PIN }

/* set output */
static inline void gpio_set(gpio_io_t __io) {
	struct stm32f_gpio * gpio = STM32F_GPIO(__io.port);
	gpio->bsr = GPIO_SET(__io.pin);
};

/* clear output */
static inline void gpio_clr(gpio_io_t __io) {
	struct stm32f_gpio * gpio = STM32F_GPIO(__io.port);
	gpio->brr = GPIO_RESET(__io.pin);
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

/*---------------------------------------------------------------------
 * DMA 
 *---------------------------------------------------------------------*/

extern const uint8_t stm32f_dma_isr_base_lut[];
extern const uint8_t stm32f_dma1_irqnum_lut[];
extern const uint8_t stm32f_dma2_irqnum_lut[];

#if defined(STM32F2X) || defined(STM32F4X)

static inline uint32_t * dma_isr_bitband(struct stm32f_dma * dma,
										 int stream) {
	return CM3_BITBAND_DEV(&dma->lisr, stm32f_dma_isr_base_lut[stream]);
}

static inline uint32_t * dma_ifcr_bitband(struct stm32f_dma * dma,
										 int stream) {
	return CM3_BITBAND_DEV(&dma->lifcr, stm32f_dma_isr_base_lut[stream]);
}

#endif

struct stm32f_dmactl {
	int strm_id;
	struct stm32f_dma_stream * strm;
	uint32_t * isr;
	uint32_t * ifcr;
};

void stm32f_dmactl_init(struct stm32f_dmactl * ctl, 
						struct stm32f_dma * dma,
						int strm_id);

#define FEIF_BIT 0
#define DMEIF_BIT 2
#define TEIF_BIT 3
#define HTIF_BIT 4
#define TCIF_BIT 5


#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------
 * GPIO
 *---------------------------------------------------------------------*/

static inline void gpio_mode_set(gpio_io_t __io, unsigned int __mode, 
								 unsigned int __opt) {
	struct stm32f_gpio * gpio = STM32F_GPIO(__io.port);

	stm32f_gpio_clock_en(gpio);
	stm32f_gpio_mode(gpio, __io.pin, __mode, __opt);
}

/*---------------------------------------------------------------------
 * FSMC
 *---------------------------------------------------------------------*/

void stm32f_fsmc_init(void);

void stm32f_fsmc_speed(int div);

/*---------------------------------------------------------------------
 * EXTI 
 *---------------------------------------------------------------------*/

#define EXTI_EDGE_RISING 1

void stm32f_exti_init(struct stm32f_gpio * gpio, unsigned int pin, 
					  unsigned int opt);

/*---------------------------------------------------------------------
 * MCO
 *---------------------------------------------------------------------*/

void stm32f_mco2_init(void);
void stm32f_mco2_disable(void);
void stm32f_mco2_enable(void);

/*---------------------------------------------------------------------
 * USB Device
 *---------------------------------------------------------------------*/

/*---------------------------------------------------------------------
 * USART 
 *---------------------------------------------------------------------*/

struct file * stm32f_usart_open(struct stm32f_usart * us,
								unsigned int baudrate, unsigned int flags);


/*---------------------------------------------------------------------
 * SPI
 *---------------------------------------------------------------------*/

int stm32f_spi_init(struct stm32f_spi * spi, 
					const struct stm32f_spi_io * spi_io, 
					unsigned int freq, unsigned int opt);
int stm32f_spi_putc(struct stm32f_spi * spi, int c);
int stm32f_spi_getc(struct stm32f_spi * spi);

#ifdef STM32F_ETH 
/*---------------------------------------------------------------------
 * Ethernet
 *---------------------------------------------------------------------*/
void stm32f_eth_init(struct stm32f_eth * eth);
void stm32f_eth_mac_get(struct stm32f_eth * eth, int idx, uint8_t * mac);
void stm32f_eth_mac_set(struct stm32f_eth * eth, int idx, const uint8_t * mac);
#endif

#include <sys/file.h>

extern const struct fileop stm32f_usart_fops;

extern const struct file stm32f_usart5_file;

#ifdef __cplusplus
}
#endif

#ifdef STM32F_USB
/*---------------------------------------------------------------------
 * USB Full Speed
 *---------------------------------------------------------------------*/

void stm32f_usb_io_init(void);

void stm32f_usb_vbus_connect(bool connect);

void stm32f_usb_power_on(struct stm32f_usb * usb);

void stm32f_usb_power_off(struct stm32f_usb * usb);

void stm32f_usb_ep0_init(struct stm32f_usb * usb, int mxpktsz);

void stm32f_usb_ep_init(struct stm32f_usb * usb, int ep_id,
		struct usb_descriptor_endpoint * desc);
#endif

/*---------------------------------------------------------------------
 * Flash Memory
 *---------------------------------------------------------------------*/

int stm32f_flash_erase(unsigned int offs, int len);

int stm32f_flash_write(uint32_t offs, const void * buf, int len);


#endif /* __ASSEMBLER__ */

#endif /* __SYS_STM32F_H__ */

