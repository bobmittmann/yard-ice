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
#include <sys/delay.h>

#ifdef CONFIG_H
#include "config.h"
#endif

#ifndef STM32F_ETH_ENABLE_RMII  
#define STM32F_ETH_ENABLE_RMII 0
#endif

#ifdef STM32F429

#define ETH_MII_TX_CLK STM32_GPIOC, 3
#define ETH_MII_TX_EN  STM32_GPIOG, 11
#define ETH_MII_TXD0   STM32_GPIOG, 13
#define ETH_MII_TXD1   STM32_GPIOG, 14
#define ETH_MII_TXD2   STM32_GPIOC, 2
#define ETH_MII_TXD3   STM32_GPIOB, 8
#define ETH_MII_RX_CLK STM32_GPIOA, 1
#define ETH_MII_RX_DV  STM32_GPIOA, 7
#define ETH_MII_RXD0   STM32_GPIOC, 4
#define ETH_MII_RXD1   STM32_GPIOC, 5
#define ETH_MII_RXD2   STM32_GPIOH, 6
#define ETH_MII_RXD3   STM32_GPIOH, 7
#define ETH_MII_RX_ER  STM32_GPIOI, 10
#define ETH_MII_CRS    STM32_GPIOA, 0
#define ETH_MII_COL    STM32_GPIOH, 3
#define ETH_MDC        STM32_GPIOC, 1 
#define ETH_MDIO       STM32_GPIOA, 2

#define ETH_RMII_TX_EN   STM32_GPIOG, 11
#define ETH_RMII_TXD0    STM32_GPIOG, 13
#define ETH_RMII_TXD1    STM32_GPIOG, 14
#define ETH_RMII_REF_CLK STM32_GPIOA, 1
#define ETH_RMII_CRS_DV  STM32_GPIOA, 7
#define ETH_RMII_RXD0    STM32_GPIOC, 4
#define ETH_RMII_RXD1    STM32_GPIOC, 5
#define ETH_RMII_RX_ER   STM32_GPIOI, 10

#ifndef ETH_PHY_IRQ_GPIO
#define ETH_PHY_IRQ_GPIO STM32_GPIOI, 4
#endif

#if STM32F_ETH_ENABLE_RMII

#endif

#else

#define ETH_MII_TX_CLK STM32_GPIOC, 3
#define ETH_MII_TX_EN  STM32_GPIOB, 11
#define ETH_MII_TXD0   STM32_GPIOB, 12
#define ETH_MII_TXD1   STM32_GPIOB, 13
#define ETH_MII_TXD2   STM32_GPIOC, 2
#define ETH_MII_TXD3   STM32_GPIOB, 8
#define ETH_MII_RX_CLK STM32_GPIOA, 1
#define ETH_MII_RX_DV  STM32_GPIOA, 7
#define ETH_MII_RXD0   STM32_GPIOC, 4
#define ETH_MII_RXD1   STM32_GPIOC, 5
#define ETH_MII_RXD2   STM32_GPIOB, 0
#define ETH_MII_RXD3   STM32_GPIOB, 1
#define ETH_MII_RX_ER  STM32_GPIOB, 10
#define ETH_MII_CRS    STM32_GPIOA, 0
#define ETH_MII_COL    STM32_GPIOA, 3
#define ETH_MDC        STM32_GPIOC, 1 
#define ETH_MDIO       STM32_GPIOA, 2

#ifndef ETH_PHY_RST_GPIO
#define ETH_PHY_RST_GPIO STM32_GPIOE, 2
#endif

#endif


#if defined(STM32F2X) || defined(STM32F4X)

void stm32f_eth_init(struct stm32f_eth * eth)
{
	struct stm32_rcc * rcc = STM32_RCC;
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;

#if STM32F_ETH_ENABLE_RMII
	DCC_LOG(LOG_TRACE, "Selecting RMII interface...");
	syscfg->pmc = SYSCFG_MII_RMII_SEL;
#else
	DCC_LOG(LOG_TRACE, "Selecting MII interface...");
	syscfg->pmc = 0;
#endif

	DCC_LOG(LOG_TRACE, "Enabling GPIO clocks...");
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);
#ifdef STM32F429
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOG);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOH);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOI);
#endif
	DCC_LOG(LOG_TRACE, "Configuring GPIO pins...");

#ifdef ETH_PHY_RST_GPIO
	stm32_gpio_mode(ETH_PHY_RST_GPIO, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_clr(ETH_PHY_RST_GPIO);
#endif

#ifdef ETH_PHY_IRQ_GPIO
	stm32_gpio_mode(ETH_PHY_IRQ_GPIO, INPUT, PULL_UP);
#endif

#if STM32F_ETH_ENABLE_RMII
	stm32_gpio_af(ETH_RMII_TX_EN, GPIO_AF11);
	stm32_gpio_af(ETH_RMII_TXD0, GPIO_AF11);
	stm32_gpio_af(ETH_RMII_TXD1, GPIO_AF11);
	stm32_gpio_af(ETH_RMII_REF_CLK, GPIO_AF11);
	stm32_gpio_af(ETH_RMII_CRS_DV, GPIO_AF11);
	stm32_gpio_af(ETH_RMII_RXD0, GPIO_AF11);
	stm32_gpio_af(ETH_RMII_RXD1, GPIO_AF11);
	stm32_gpio_af(ETH_RMII_RX_ER, GPIO_AF11);

	stm32_gpio_mode(ETH_RMII_TX_EN, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(ETH_RMII_TXD0, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(ETH_RMII_TXD1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(ETH_RMII_REF_CLK, ALT_FUNC, 0);
	stm32_gpio_mode(ETH_RMII_CRS_DV, ALT_FUNC, 0);
	stm32_gpio_mode(ETH_RMII_RXD0, ALT_FUNC, 0);
	stm32_gpio_mode(ETH_RMII_RXD1, ALT_FUNC, 0);
	stm32_gpio_mode(ETH_RMII_RX_ER, ALT_FUNC, 0);
#else
	stm32_gpio_af(ETH_MII_TX_CLK, GPIO_AF11);
	stm32_gpio_af(ETH_MII_TX_EN, GPIO_AF11);
	stm32_gpio_af(ETH_MII_TXD0, GPIO_AF11);
	stm32_gpio_af(ETH_MII_TXD1, GPIO_AF11);
	stm32_gpio_af(ETH_MII_TXD2, GPIO_AF11);
	stm32_gpio_af(ETH_MII_TXD3, GPIO_AF11);
	stm32_gpio_af(ETH_MII_RX_CLK, GPIO_AF11);
	stm32_gpio_af(ETH_MII_RX_DV, GPIO_AF11);
	stm32_gpio_af(ETH_MII_RXD0, GPIO_AF11);
	stm32_gpio_af(ETH_MII_RXD1, GPIO_AF11);
	stm32_gpio_af(ETH_MII_RXD2, GPIO_AF11);
	stm32_gpio_af(ETH_MII_RXD3, GPIO_AF11);
	stm32_gpio_af(ETH_MII_RX_ER, GPIO_AF11);
	stm32_gpio_af(ETH_MII_CRS, GPIO_AF11);
	stm32_gpio_af(ETH_MII_COL, GPIO_AF11);

	stm32_gpio_mode(ETH_MII_TX_CLK, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(ETH_MII_TX_EN, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(ETH_MII_TXD0, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(ETH_MII_TXD1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(ETH_MII_TXD2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(ETH_MII_TXD3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(ETH_MII_RX_CLK, ALT_FUNC, 0);
	stm32_gpio_mode(ETH_MII_RX_DV, ALT_FUNC, 0);
	stm32_gpio_mode(ETH_MII_RXD0, ALT_FUNC, 0);
	stm32_gpio_mode(ETH_MII_RXD1, ALT_FUNC, 0);
	stm32_gpio_mode(ETH_MII_RXD2, ALT_FUNC, 0);
	stm32_gpio_mode(ETH_MII_RXD3, ALT_FUNC, 0);
	stm32_gpio_mode(ETH_MII_RX_ER, ALT_FUNC, 0);
	stm32_gpio_mode(ETH_MII_CRS, ALT_FUNC, 0);
	stm32_gpio_mode(ETH_MII_COL, ALT_FUNC, 0);
#endif
	stm32_gpio_af(ETH_MDC, GPIO_AF11);
	stm32_gpio_af(ETH_MDIO, GPIO_AF11);

	stm32_gpio_mode(ETH_MDC, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(ETH_MDIO, ALT_FUNC, OPEN_DRAIN | PULL_UP | SPEED_MED);

	DCC_LOG(LOG_TRACE, "Enabling ETH clocks...");
	rcc->ahb1enr |= RCC_ETHMACRXEN | RCC_ETHMACTXEN | RCC_ETHMACEN;

	/* disable MAC interrupts */
	eth->macimr = 0;
	/* disable MMC recive interrupts */
	eth->mmcrimr = 0;
	/* disable MMC transmit interrupts */
	eth->mmctimr = 0;
	/* disable DMA interrupts */
	eth->dmaier = 0;

	/* clear all DMA status bits */
	eth->dmasr = 0xffffffff;

	/* Reset the MMC counters to zero after read */
	eth->mmccr = ETH_ROR;

#ifdef ETH_PHY_RST_GPIO
	DCC_LOG(LOG_TRACE, "PHY reset...");
	udelay(1000);
	stm32_gpio_set(ETH_PHY_RST_GPIO);
	udelay(9000);
#endif
};

void stm32f_eth_mac_get(struct stm32f_eth * eth, int idx, uint8_t * mac)
{
	uint32_t lo;
	uint32_t hi;

	lo = eth->maca[idx].lr;
	hi = eth->maca[idx].hr;
	
	mac[0] = lo & 0xff;
	mac[1] = (lo >> 8) & 0xff;
	mac[2] = (lo >> 16) & 0xff;
	mac[3] = (lo >> 24) & 0xff;
	mac[4] = hi & 0xff;
	mac[5] = (hi >> 8) & 0xff;
}

void stm32f_eth_mac_set(struct stm32f_eth * eth, int idx, const uint8_t * mac)
{
	/*  */
	DCC_LOG6(LOG_TRACE, "MAC: %02x:%02x:%02x:%02x:%02x:%02x",
		   mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	eth->maca[idx].lr = mac[0] + (mac[1] << 8) + 
		(mac[2] << 16) + (mac[3] << 24);
	eth->maca[idx].hr = mac[4] + (mac[5] << 8);
}

#if 0
void stm32eth_st(struct stm32f_eth * eth)
{
	DCC_LOG1(LOG_TRACE, " TGFSCC: %9u", eth->mmctgfsccr);
	DCC_LOG1(LOG_TRACE, "TGFMSCC: %9u", eth->mmctgfmsccr);
	DCC_LOG1(LOG_TRACE, "   TGFC: %9u", eth->mmctgfcr);
	DCC_LOG1(LOG_TRACE, "  RFCEC: %9u", eth->mmcrfcecr);
	DCC_LOG1(LOG_TRACE, "  RFAEC: %9u", eth->mmcrfaecr);
	DCC_LOG1(LOG_TRACE, "  RGUFC: %9u", eth->mmcrgufcr);
}
#endif

#endif

