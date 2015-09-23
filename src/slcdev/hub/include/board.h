/* 
 * File:	 board.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(c) 2003-2006 BORESTE (www.boreste.com). All Rights Reserved.
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

#ifndef __BOARD_H__
#define __BOARD_H__

#include <sys/stm32f.h>

/* -------------------------------------------------------------------------
 * Lattice FPGA (iCE40LP384) connections
 * ------------------------------------------------------------------------- */

#define IO_ICE40_SPI_SCK  STM32_GPIOD, 3
#define IO_ICE40_SPI_SDO  STM32_GPIOB, 14
#define IO_ICE40_SPI_SDI  STM32_GPIOB, 15
#define IO_ICE40_SPI_SS   STM32_GPIOD, 4
#define ICE40_SPI         STM32F_SPI2
#define ICE40_CLK_SPI     STM32_CLK_SPI2
#define ICE4_SPI_AF       GPIO_AF5

#define IO_ICE40_CRESET   STM32_GPIOE, 0
#define IO_ICE40_CDONE    STM32_GPIOB, 9

#define IO_RS485_TX       STM32_GPIOB, 6
#define IO_RS485_RX       STM32_GPIOB, 7
#define IO_RS485_DBG      STM32_GPIOB, 5
#define IO_RS485_MODE     STM32_GPIOE, 1
#define IO_RS485_USART    STM32_USART1
#define RS485_USART_AF    GPIO_AF7

/* -------------------------------------------------------------------------
 * Serial Ports
 * ------------------------------------------------------------------------- */

#define IO_UART2_TX       STM32_GPIOD, 5
#define IO_UART2_RX       STM32_GPIOD, 6
#define UART2_AF          GPIO_AF7

#define IO_UART3_TX       STM32_GPIOD, 8
#define IO_UART3_RX       STM32_GPIOD, 9
#define UART3_AF          GPIO_AF7

#define IO_UART4_TX       STM32_GPIOC, 10
#define IO_UART4_RX       STM32_GPIOC, 11
#define UART4_AF          GPIO_AF8

#define IO_UART5_TX       STM32_GPIOC, 12
#define IO_UART5_RX       STM32_GPIOD, 2
#define UART5_AF          GPIO_AF8

#define IO_UART6_TX       STM32_GPIOC, 6
#define IO_UART6_RX       STM32_GPIOC, 7
#define UART6_AF          GPIO_AF8

/* -------------------------------------------------------------------------
 * LEDs 
 * ------------------------------------------------------------------------- */

#define IO_LED1           STM32_GPIOD, 14
#define IO_LED2           STM32_GPIOD, 15

/* -------------------------------------------------------------------------
 * Address switch
 * ------------------------------------------------------------------------- */

#define IO_ADDR0          STM32_GPIOE, 7
#define IO_ADDR1          STM32_GPIOE, 8
#define IO_ADDR2          STM32_GPIOE, 9
#define IO_ADDR3          STM32_GPIOE, 10

/* -------------------------------------------------------------------------
 * Mode switch
 * ------------------------------------------------------------------------- */

#define IO_MODE0          STM32_GPIOD, 10
#define IO_MODE1          STM32_GPIOD, 11
#define IO_MODE2          STM32_GPIOD, 12
#define IO_MODE3          STM32_GPIOD, 13

/* -------------------------------------------------------------------------
 * Serial Flash  connections
 * ------------------------------------------------------------------------- */

/* IO Pins */
#define IO_SFLASH_SCK     STM32_GPIOE, 12
#define IO_SFLASH_MISO    STM32_GPIOE, 13
#define IO_SFLASH_MOSI    STM32_GPIOE, 14
#define IO_SFLASH_CS      STM32_GPIOE, 11
#define AF_SPI_SFLASH     GPIO_AF5
/* SPI */
#define SPI_SFLASH        STM32F_SPI4
#define CLK_SPI_SFLASH    STM32_CLK_SPI4
#define IRQ_SPI_SFLASH    STM32F_IRQ_SPI4
/* DMA */
#define DMA_SPI_SFLASH              STM32F_DMA2
#define DMA_SPI_RX_CHAN_SFLASH      5
#define DMA_SPI_RX_STREAM_SFLASH    3
#define DMA_SPI_TX_CHAN_SFLASH      5
#define DMA_SPI_TX_STREAM_SFLASH    4

#if 0
#define DMA_SPI_RX_CHAN_SFLASH      4
#define DMA_SPI_RX_STREAM_SFLASH    0
#define DMA_SPI_TX_CHAN_SFLASH      4
#define DMA_SPI_TX_STREAM_SFLASH    2
#endif

/* -------------------------------------------------------------------------
 * Speaker
 * ------------------------------------------------------------------------- */
#define IO_DAC2           STM32_GPIOA, 5

/* -------------------------------------------------------------------------
 * Ethernet 
 * ------------------------------------------------------------------------- */
#define IO_ETH_MII_TX_CLK  STM32_GPIOC, 3
#define IO_ETH_MII_TX_EN   STM32_GPIOB, 11
#define IO_ETH_MII_TXD0    STM32_GPIOB, 12
#define IO_ETH_MII_TXD1    STM32_GPIOB, 13
#define IO_ETH_MII_TXD2    STM32_GPIOC, 2
#define IO_ETH_MII_TXD3    STM32_GPIOB, 8
#define IO_ETH_MII_RX_CLK  STM32_GPIOA, 1
#define IO_ETH_MII_RX_DV   STM32_GPIOA, 7
#define IO_ETH_MII_RXD0    STM32_GPIOC, 4
#define IO_ETH_MII_RXD1    STM32_GPIOC, 5
#define IO_ETH_MII_RXD2    STM32_GPIOB, 0
#define IO_ETH_MII_RXD3    STM32_GPIOB, 1
#define IO_ETH_MII_RX_ER   STM32_GPIOB, 10
#define IO_ETH_MII_CRS     STM32_GPIOA, 0
#define IO_ETH_MII_COL     STM32_GPIOA, 3
#define IO_ETH_MDC         STM32_GPIOC, 1 
#define IO_ETH_MDIO        STM32_GPIOA, 2
#define IO_ETH_PHY_RST     STM32_GPIOE, 5

/* low level led on/off functions */
static inline void __led_on(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_clr(__gpio, __pin);
}

static inline void __led_off(struct stm32_gpio *__gpio, int __pin) {
	stm32_gpio_set(__gpio, __pin);
}
#define FLASH_BLK_FS_OFFS     0x00020000
#define FLASH_BLK_FS_SIZE     (256 * 1024)


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

