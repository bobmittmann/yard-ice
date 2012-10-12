/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 *
 * This file is part of the libstm32f.
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
 * @file stm32f-eth.c
 * @brief STM32F Ethernet 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <sys/mii.h>
#include <sys/dcclog.h>

#define ETH_MII_TX_CLK STM32F_GPIOC, 3
#define ETH_MII_TX_EN STM32F_GPIOB, 11
#define ETH_MII_TXD0 STM32F_GPIOB, 12
#define ETH_MII_TXD1 STM32F_GPIOB, 13
#define ETH_MII_TXD2 STM32F_GPIOC, 2
#define ETH_MII_TXD3 STM32F_GPIOB, 8
#define ETH_MII_RX_CLK STM32F_GPIOA, 1
#define ETH_MII_RX_DV STM32F_GPIOA, 7
#define ETH_MII_RXD0 STM32F_GPIOC, 4
#define ETH_MII_RXD1 STM32F_GPIOC, 5
#define ETH_MII_RXD2 STM32F_GPIOB, 0
#define ETH_MII_RXD3 STM32F_GPIOB, 1
#define ETH_MII_RX_ER STM32F_GPIOB, 10
#define ETH_MII_CRS STM32F_GPIOA, 0
#define ETH_MII_COL STM32F_GPIOA, 3
#define ETH_MDC STM32F_GPIOC, 1 
#define ETH_MDIO STM32F_GPIOA, 2

#define EXT_RST STM32F_GPIOE, 2

int stm32f_eth_init(uint8_t * addr)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_eth * eth = STM32F_ETH;
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;

	DCC_LOG(LOG_TRACE, "Enabling ETH clocks...");
	rcc->ahb1enr |= RCC_ETHMACRXEN | RCC_ETHMACTXEN | RCC_ETHMACEN;

	DCC_LOG(LOG_TRACE, "Enabling GPIO clocks...");
	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOC);
	stm32f_gpio_clock_en(STM32F_GPIOE);

	DCC_LOG(LOG_TRACE, "Selecting MII interface...");
	syscfg->pmc = 0;

	DCC_LOG(LOG_TRACE, "Configuring GPIO pins...");
	stm32f_gpio_mode(EXT_RST, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_clr(EXT_RST);

	DCC_LOG(LOG_TRACE, "1.");

	stm32f_gpio_af(ETH_MII_TX_CLK, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_TX_EN, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_TXD0, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_TXD1, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_TXD2, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_TXD3, GPIO_AF11);

	DCC_LOG(LOG_TRACE, "2.");

	stm32f_gpio_af(ETH_MII_RX_CLK, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_RX_DV, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_RXD0, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_RXD1, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_RXD2, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_RXD3, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_RX_ER, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_CRS, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_COL, GPIO_AF11);

	DCC_LOG(LOG_TRACE, "3.");
	stm32f_gpio_af(ETH_MDC, GPIO_AF11);
	stm32f_gpio_af(ETH_MDIO, GPIO_AF11);

	DCC_LOG(LOG_TRACE, "4.");
	stm32f_gpio_mode(ETH_MII_TX_CLK, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(ETH_MII_TX_EN, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(ETH_MII_TXD0, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(ETH_MII_TXD1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(ETH_MII_TXD2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(ETH_MII_TXD3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);

	DCC_LOG(LOG_TRACE, "5.");
	stm32f_gpio_mode(ETH_MII_RX_CLK, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_RX_DV, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_RXD0, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_RXD1, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_RXD2, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_RXD3, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_RX_ER, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_CRS, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_COL, ALT_FUNC, 0);

	DCC_LOG(LOG_TRACE, "6.");
	stm32f_gpio_mode(ETH_MDC, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(ETH_MDIO, ALT_FUNC, OPEN_DRAIN | PULL_UP | SPEED_LOW);


	DCC_LOG(LOG_TRACE, "Configuring ethernet...");
	eth->maccr = ETH_CSTF | ETH_FES | ETH_ROD | ETH_DM | ETH_TE | ETH_RE;

	/*  */
	DCC_LOG6(LOG_TRACE, "MAC: %02x:%02x:%02x:%02x:%02x:%02x",
		   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

	eth->maca0lr = addr[0] + (addr[1] << 8) + (addr[2] << 16) + (addr[3] << 24);
	eth->maca0hr = addr[4] + (addr[5] << 8);

	DCC_LOG(LOG_TRACE, "Configuring DMA...\n");
	eth->dmaomr = ETH_RSF | ETH_TSF | ETH_FTF;

	DCC_LOG(LOG_TRACE, "PHY reset...\n");
	udelay(1000);
	stm32f_gpio_set(EXT_RST);
	udelay(9000);

	return 0;
};

