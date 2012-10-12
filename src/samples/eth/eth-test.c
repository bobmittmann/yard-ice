/* 
 * File:	 rtc-test.c
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
#include <sys/mii.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

FILE * stdout = (FILE *)&stm32f_usart5_file;

#define PA STM32F_GPIOA
#define PB STM32F_GPIOB
#define PC STM32F_GPIOC
#define PD STM32F_GPIOD
#define PE STM32F_GPIOE

#define ETH_MII_TX_CLK PC, 3
#define ETH_MII_TX_EN PB, 11
#define ETH_MII_TXD0 PB, 12
#define ETH_MII_TXD1 PB, 13
#define ETH_MII_TXD2 PC, 2
#define ETH_MII_TXD3 PB, 8
#define ETH_MII_RX_CLK PA, 1
#define ETH_MII_RX_DV PA, 7
#define ETH_MII_RXD0 PC, 4
#define ETH_MII_RXD1 PC, 5
#define ETH_MII_RXD2 PB, 0
#define ETH_MII_RXD3 PB, 1
#define ETH_MII_RX_ER PB, 10
#define ETH_MII_CRS PA, 0
#define ETH_MII_COL PA, 3
#define ETH_MDC PC, 1 
#define ETH_MDIO PA, 2

#define EXT_RST PE, 2

void eth_phy_wr(struct stm32f_eth * eth, unsigned int addr, 
			unsigned int reg, unsigned int data)
{
	while (eth->macmiiar & ETH_MII_MB) {
	};

	eth->macmiidr = data;

	eth->macmiiar = ETH_PA_SET(addr) | ETH_MR_SET(reg) | 
		ETH_CR_HCLK_62 | ETH_MII_MW | ETH_MII_MB;
}

int eth_phy_rd(struct stm32f_eth * eth, unsigned int addr, 
		   unsigned int reg)
{
	while (eth->macmiiar & ETH_MII_MB) {
	};

	eth->macmiiar = ETH_PA_SET(addr) | ETH_MR_SET(reg) | 
		ETH_CR_HCLK_62 | ETH_MII_MB;

	while (eth->macmiiar & ETH_MII_MB) {
	};

	return eth->macmiidr;
}

int eth_phy_init(struct stm32f_eth * eth, unsigned int addr)
{
	int id1;
	int id2;
	int n;
	int oui;
	int model;

//	printf(" - Detecting PHY @ 0x%02x...\n", addr);

	id1 = eth_phy_rd(eth, addr, MII_PHYID1);
	id2 = eth_phy_rd(eth, addr, MII_PHYID2);

	if ((id1 == 0) && (id2 ==0)) {
		printf(" # fail, invalid PHY ID\n");
		return -1;
	}

	printf(" - PHY ID: 0x%04x%04x\n", id2, id1);
	oui = id1 | (((id2 >> 10) & 0x3f) << 16);
	model = (id2 & 0x3ff);
	printf(" - PHY ID: Manuf=0x%06x Model=0x%04x\n", oui, model);

	/* Isolate the PHY */
	eth_phy_wr(eth, addr, MII_BMCR, BMCR_ISOLATE);

	/* MII mode
	 * automatic reduced power down 
	 * 100BASE-TX operation
	 * MF preamble suppression bit on
	 */
#if 0
	/* Do not bypass the scrambler/descrambler */
	eth_phy_wr(eth, addr, MII_DSCR, MODE_100TX | RPDCTR_EN | MFPSC);

	/* Clear 10BTCSR to default */
	eth_phy_wr(eth, addr, MII_10BTCSR, LP_EN | HBE | SQUELCH | JABEN);
#endif

	/* Reconnect the PHY, and enable Autonegotiation */
	eth_phy_wr(eth, addr, MII_BMCR, BMCR_ANENABLE | BMCR_ANRESTART);

	n = 500;
	while (eth_phy_rd(eth, addr, MII_BMCR) & BMCR_ANRESTART) {
		if (--n == 0) {
			printf(" # Autonegotiation start fail!\n");
			return -1;
		}
		udelay(10000);
	}

	return -1;
}

int eth_init(uint8_t * addr)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_eth * eth = STM32F_ETH;
	struct stm32f_syscfg * syscfg = STM32F_SYSCFG;

	printf(" - Enabling ETH clocks...\n");
	rcc->ahb1enr |= RCC_ETHMACRXEN | RCC_ETHMACTXEN | RCC_ETHMACEN;

	printf(" - Enabling GPIO clocks...\n");
	stm32f_gpio_clock_en(PA);
	stm32f_gpio_clock_en(PB);
	stm32f_gpio_clock_en(PC);
	stm32f_gpio_clock_en(PE);

	printf(" - Selecting MII interface...\n");
	syscfg->pmc = 0;

	printf(" - Configuring GPIO pins...\n");
	stm32f_gpio_mode(EXT_RST, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_clr(EXT_RST);

	printf(" 1.\n");

	stm32f_gpio_af(ETH_MII_TX_CLK, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_TX_EN, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_TXD0, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_TXD1, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_TXD2, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_TXD3, GPIO_AF11);

	printf(" 2.\n");

	stm32f_gpio_af(ETH_MII_RX_CLK, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_RX_DV, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_RXD0, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_RXD1, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_RXD2, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_RXD3, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_RX_ER, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_CRS, GPIO_AF11);
	stm32f_gpio_af(ETH_MII_COL, GPIO_AF11);

	printf(" 3.\n");
	stm32f_gpio_af(ETH_MDC, GPIO_AF11);
	stm32f_gpio_af(ETH_MDIO, GPIO_AF11);

	printf(" 4.\n");
	stm32f_gpio_mode(ETH_MII_TX_CLK, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(ETH_MII_TX_EN, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(ETH_MII_TXD0, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(ETH_MII_TXD1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(ETH_MII_TXD2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32f_gpio_mode(ETH_MII_TXD3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);

	printf(" 5.\n");
	stm32f_gpio_mode(ETH_MII_RX_CLK, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_RX_DV, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_RXD0, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_RXD1, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_RXD2, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_RXD3, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_RX_ER, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_CRS, ALT_FUNC, 0);
	stm32f_gpio_mode(ETH_MII_COL, ALT_FUNC, 0);

	printf(" 6.\n");
	stm32f_gpio_mode(ETH_MDC, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32f_gpio_mode(ETH_MDIO, ALT_FUNC, OPEN_DRAIN | PULL_UP | SPEED_LOW);


	printf(" - Configuring ethernet...\n");
	eth->maccr = ETH_CSTF | ETH_FES | ETH_ROD | ETH_DM | ETH_TE | ETH_RE;

	/*  */
	printf(" - Setting MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
		   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	eth->maca0lr = addr[0] + (addr[1] << 8) + (addr[2] << 16) + (addr[3] << 24);
	eth->maca0hr = addr[4] + (addr[5] << 8);

	printf(" - Configuring DMA...\n");
	eth->dmaomr = ETH_RSF | ETH_TSF | ETH_FTF;

	printf(" - PHY reset...\n");
	udelay(1000);
	stm32f_gpio_set(EXT_RST);
	udelay(9000);



#if 0
	unsigned int phy;
	for (phy = 0; phy < 2; phy++) {
		if (eth_phy_init(eth, phy) == 0)
			break;
	}
#endif

	return 0;
};

struct rx_dma_desc {
	uint32_t status: 16;
	uint32_t fl: 14; /* Frame length */
	uint32_t afm: 1; /* Destination address filter fail */
	uint32_t own: 1; /* Own bit */

	uint32_t rbs1: 13; /* Receive buffer 1 size */
	uint32_t res1: 1;
	uint32_t rch: 1; /* Second address chained */
	uint32_t rer: 1; /* Receive end of ring */
	uint32_t rbs2: 15; /* Receive buffer 2 size */
	uint32_t dic: 1; /* Disable interrupt on completion */

	union {
		uint32_t * rbap1; /* Receive buffer 1 address pointer */
		uint32_t rtsl; /* Receive frame time stamp low */
	};
	union {
		uint32_t * rbap2; /* Receive buffer 2 address pointer */
		uint32_t rtsh; /* Receive frame time stamp high */
	};
};

struct rx_dma_desc desc;

void show_rx_desc_status(struct rx_dma_desc * desc)
{
	printf(" - status=0x%04x", desc->status);
	if (desc->status & RXDMA_DSE)
		printf(" DSE");
	if (desc->status & RXDMA_OE)
		printf(" OE");
	if (desc->status & RXDMA_VLAN)
		printf(" VLAN");
	if (desc->status & RXDMA_FS)
		printf(" FS");
	if (desc->status & RXDMA_LS)
		printf(" LS");
	if (desc->status & RXDMA_LCO)
		printf(" LCO");
	if (desc->status & RXDMA_FT)
		printf(" FT");
	if (desc->status & RXDMA_RWT)
		printf(" RWT");
	printf("\n");

	printf(" - frame len=%d", desc->fl);
	printf("\n");

	if (desc->own) {
		printf(" # OWN bit set!!!\n");
	};
}

void show_dma_status(uint32_t dmasr)
{
	printf(" - DMA status: 0x%08x", dmasr);
	if (dmasr & ETH_FBES) 
		printf(" FBES");
	if (dmasr & ETH_RWTS) 
		printf(" RWTS");
	if (dmasr & ETH_RBUS) 
		printf(" RBUS");
	if (dmasr & ETH_RS) 
		printf(" RS");
	if (dmasr & ETH_RPSS) 
		printf(" RPSS");
	if (dmasr & ETH_ROS) 
		printf(" ROS");
	if (dmasr & ETH_ROS) 
		printf(" ROS");
	printf("\n");

}

int eth_recv(void * buf, unsigned int len)
{
	struct stm32f_eth * eth = STM32F_ETH;
	uint32_t dmasr;
	uint32_t rps;

	dmasr = eth->dmasr;
/*	show_dma_status(dmasr); */

	if (dmasr & ETH_RS) {
		/* clear RS bit */
		eth->dmasr = ETH_RS;
	}

	if (dmasr & ETH_RBUS) {
		/* clear RBUS bit */
		eth->dmasr = ETH_RBUS;
	}

	desc.rbs1 = len;
	desc.rer = 1;
	desc.rbap1 = (uint32_t *)buf;
	desc.status = 0;
	desc.fl = 0;
	desc.own = 1;

	/* DMA receive descriptor list address */
	/* This field contains the base address of the first descriptor in the 
	   receive descriptor list. The LSB bits [1/2/3:0] for 32/64/128-bit bus
	   width) are internally ignored and taken as all-zero by the DMA. Hence 
	   these LSB bits are read only. */
	eth->dmardlar = (uint32_t)&desc;

	rps = ETH_RPS_GET(dmasr);
	if (rps == 0) {
		/* DMA stopped */
		printf(" - DMA stopped\n");
		eth->dmaomr |= ETH_SR;
	} else if (rps == 4) {
		printf(" - DMA suspended\n");
		eth->dmarpdr = 1;
	/* DMA receive poll demand */
	/* When these bits are written with any value, the DMA reads the current 
	   descriptor pointed to by the ETH_DMACHRDR register. If that descriptor 
	   is not available (owned by Host), reception returns to the Suspended 
	   state and ETH_DMASR register bit 7 is not asserted. If the descriptor 
	   is available, the Receive DMA returns to active state. */
	}

	while (((dmasr = eth->dmasr) & ETH_RS) == 0) {
	}

	return desc.fl;
}

#define ETH_P_IP	0x0800 /* Internet Protocol packet	*/
#define ETH_P_ARP	0x0806 /* Address Resolution packet	*/

void eth_input(uint8_t * frame, unsigned int len)
{
	unsigned int type;

	/* skip destination and source addresses */
	type = (frame[12] << 8) + frame[13];

	printf("type=%04x len=%d\n", type, len);
}

int main(int argc, char ** argv)
{
	uint8_t ethaddr[6] = { 0x1c, 0x95, 0x5d, 0x00, 0x00, 0x81};
	uint32_t frame[512];
	int len;

	stm32f_usart_open(STM32F_USART5, 115200, SERIAL_8N1);
	cm3_udelay_calibrate();

	printf("\n");
	printf("--------------------------------------------\n");
	printf("Ethernet test\n");
	printf("--------------------------------------------\n");
	printf("\n");

	eth_init(ethaddr);

	for (;;) {
		if ((len = eth_recv(frame, 2048)) > 0) {
			printf("+");
			eth_input((uint8_t *)frame, len);
		}
	}

	return 0;
}

