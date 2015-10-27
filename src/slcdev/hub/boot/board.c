/* 
 * File:	 usb-test.c
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
#include <sys/delay.h>
#include <sys/dcclog.h>
#define __THINKOS_DMON__
#include <thinkos_dmon.h>
#include <gdb.h>

#include "board.h"
#include "lattice.h"
#include "version.h"

#define DAC1_DMA_STREAM  5
#define DAC2_DMA_STREAM  6
#define DAC_DMA_CHANNEL  7

#define AUDIO_DMA_STREAM DAC2_DMA_STREAM
#define AUDIO_SAMPLE_RATE 12000

/* over=15 err=0.001582 freq=1046.512 */
static const uint8_t wave_c4[] = /* 1046.502 Hz */
{
	   128,     194,     241,     255,     231,     178,     110,     47,
	     8,       4,      37,      96,     165,     223,     253,    247,
	   206,     142,      74,      22,       1,      17,      66,    133,
	   198,     243,     254,     229,     173,     105,      43,      6,
	     5,      40,     100,     169,     226,     254,     245,    202,
	   137,      70,      20,       1,      20,      70,     137,    202,
	   245,     254,     226,     169,     100,      40,       5,      6,
	    43,     105,     173,     229,     254,     243,     198,    133,
	    66,      17,       1,      22,      74,     142,     206,    247,
	   253,     223,     165,      96,      37,       4,       8,     47,
	   110,     178,     231,     255,     241,     194,     128,     62,
	    15,       1,      25,      78,     146,     209,     248,    252,
	   219,     160,      91,      33,       3,       9,      50,    114,
	   182,     234,     255,     239,     190,     123,      58,     13,
	     2,      27,      83,     151,     213,     250,     251,    216,
	   156,      87,      30,       2,      11,      54,     119,    186,
	   236,     255,     236,     186,     119,      54,      11,      2,
	    30,      87,     156,     216,     251,     250,     213,    151,
	    83,      27,       2,      13,      58,     123,     190,    239,
	   255,     234,     182,     114,      50,       9,       3,     33,
	    91,     160,     219,     252,     248,     209,     146,     78,
	    25,       1,      15,      62, 
};

void wave_set(const uint8_t * wave, unsigned int len)
{
	struct stm32f_dma * dma = STM32F_DMA1;

	/* disable DMA */
	dma->s[AUDIO_DMA_STREAM].cr &= ~DMA_EN;	
	/* Wait for the channel to be ready .. */
	while (dma->s[AUDIO_DMA_STREAM].cr & DMA_EN); 
	/* Memory address */
	dma->s[AUDIO_DMA_STREAM].m0ar = (void *)wave;
	/* Number of data items to transfer */
	dma->s[AUDIO_DMA_STREAM].ndtr = len;
}

void wave_play(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	/* enable DMA */
	dma->s[AUDIO_DMA_STREAM].cr |= DMA_EN;	
}

void wave_pause(void)
{
	struct stm32f_dma * dma = STM32F_DMA1;
	/* disable DMA */
	dma->s[AUDIO_DMA_STREAM].cr &= ~DMA_EN;	
}

void wave_play_init(const uint8_t * wave, unsigned int len)
{
	struct stm32f_dac * dac = STM32F_DAC;
	struct stm32f_tim * tim2 = STM32F_TIM2;
	struct stm32f_dma * dma = STM32F_DMA1;
	uint32_t freq = AUDIO_SAMPLE_RATE;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* DAC clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_DAC);
	/* DAC configure */
	dac->cr = DAC_EN2 | DAC_TSEL2_TIMER2 | DAC_TEN2 | DAC_DMAEN2;
	/* DAC channel 2 initial value */
	dac->dhr8r2 = 128;

	/* Timer clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_TIM2);
	/* get the total divisior */
	div = ((stm32f_tim1_hz) + (freq / 2)) / freq;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div + pre / 2) / pre;
	tim2->psc = pre - 1;
	tim2->arr = n - 1;
	tim2->cnt = 0;
	tim2->egr = 0; /* Update generation */
	tim2->cr2 = TIM_MMS_OC1REF;
	tim2->ccmr1 = TIM_OC1M_PWM_MODE1;
	tim2->ccr1 = tim2->arr - 2;
	tim2->cr1 = TIM_URS | TIM_CEN; /* Enable counter */

	/* DMA clock enable */
	stm32_clk_enable(STM32_RCC, STM32_CLK_DMA1);
	/* DMA Disable */
	dma->s[AUDIO_DMA_STREAM].cr = 0;
	/* Wait for the channel to be ready .. */
	while (dma->s[AUDIO_DMA_STREAM].cr & DMA_EN); 
	/*  DMA Configuration */
	/* Peripheral address */
	dma->s[AUDIO_DMA_STREAM].par = &dac->dhr8r2;
	/* Memory address */
	dma->s[AUDIO_DMA_STREAM].m0ar = (void *)wave;
	/* Number of data items to transfer */
	dma->s[AUDIO_DMA_STREAM].ndtr = len;
	/* Configuration single buffer circular */
	dma->s[AUDIO_DMA_STREAM].cr = DMA_CHSEL_SET(DAC_DMA_CHANNEL) | 
		DMA_MBURST_1 | DMA_PBURST_1 | 
		DMA_MSIZE_8 | DMA_PSIZE_8 | DMA_MINC | 
		DMA_CIRC | DMA_DIR_MTP;
}

extern const uint8_t ice40lp384_bin[];
extern const unsigned int sizeof_ice40lp384_bin;

static void io_init(void)
{
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOA);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOB);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOC);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOD);
	stm32_clk_enable(STM32_RCC, STM32_CLK_GPIOE);

	/* - LEDs ---------------------------------------------------------*/
	stm32_gpio_set(IO_LED1);
	stm32_gpio_mode(IO_LED1, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_set(IO_LED2);
	stm32_gpio_mode(IO_LED2, OUTPUT, PUSH_PULL | SPEED_LOW);

	/* - Address switch -------------------------------------------------*/
	stm32_gpio_mode(IO_ADDR0, INPUT, PULL_UP);
	stm32_gpio_mode(IO_ADDR1, INPUT, PULL_UP);
	stm32_gpio_mode(IO_ADDR2, INPUT, PULL_UP);
	stm32_gpio_mode(IO_ADDR3, INPUT, PULL_UP);

	/* - Mode switch ---------------------------------------------------*/
	stm32_gpio_mode(IO_MODE0, INPUT, PULL_UP);
	stm32_gpio_mode(IO_MODE1, INPUT, PULL_UP);
	stm32_gpio_mode(IO_MODE2, INPUT, PULL_UP);
	stm32_gpio_mode(IO_MODE3, INPUT, PULL_UP);

	/* - RS485 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_RS485_RX, ALT_FUNC, PULL_UP);
	stm32_gpio_af(IO_RS485_RX, RS485_USART_AF);

	stm32_gpio_mode(IO_RS485_TX, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_af(IO_RS485_TX, RS485_USART_AF);

	stm32_gpio_mode(IO_RS485_MODE, OUTPUT, PUSH_PULL | SPEED_LOW);
	stm32_gpio_set(IO_RS485_MODE);

	stm32_gpio_mode(IO_RS485_DBG, INPUT, PULL_UP);

	/* - UART 2 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART2_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART2_TX, UART2_AF);
	stm32_gpio_mode(IO_UART2_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART2_RX, UART2_AF);

	/* - UART 3 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART3_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART3_TX, UART3_AF);
	stm32_gpio_mode(IO_UART3_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART3_RX, UART3_AF);

	/* - UART 4 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART4_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART4_TX, UART4_AF);
	stm32_gpio_mode(IO_UART4_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART4_RX, UART4_AF);

	/* - UART 5 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART5_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART5_TX, UART5_AF);
	stm32_gpio_mode(IO_UART5_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART5_RX, UART5_AF);

	/* - UART 6 ---------------------------------------------------------*/
	stm32_gpio_mode(IO_UART6_TX, ALT_FUNC, PUSH_PULL | SPEED_LOW);
	stm32_gpio_af(IO_UART6_TX, UART6_AF);
	stm32_gpio_mode(IO_UART6_RX, ALT_FUNC, 0);
	stm32_gpio_af(IO_UART6_RX, UART6_AF);

	/* - Serial Flash ----------------------------------------------------*/
	stm32_gpio_mode(IO_SFLASH_CS, OUTPUT, SPEED_HIGH);
	stm32_gpio_set(IO_SFLASH_CS);

	stm32_gpio_mode(IO_SFLASH_SCK, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_af(IO_SFLASH_SCK, AF_SPI_SFLASH);

	stm32_gpio_mode(IO_SFLASH_MISO, ALT_FUNC, SPEED_HIGH);
	stm32_gpio_af(IO_SFLASH_MISO, AF_SPI_SFLASH);

	stm32_gpio_mode(IO_SFLASH_MOSI, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_af(IO_SFLASH_MOSI, AF_SPI_SFLASH);

	/* - Speaker ----------------------------------------------------*/
	stm32_gpio_mode(IO_DAC2, ANALOG, 0);

	/* - Ethernet ----------------------------------------------------*/
	stm32_gpio_af(IO_ETH_MII_TX_CLK, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_TX_EN, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_TXD0, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_TXD1, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_TXD2, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_TXD3, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_RX_CLK, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_RX_DV, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_RXD0, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_RXD1, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_RXD2, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_RXD3, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_RX_ER, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_CRS, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MII_COL, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MDC, GPIO_AF11);
	stm32_gpio_af(IO_ETH_MDIO, GPIO_AF11);

	stm32_gpio_mode(IO_ETH_MII_TX_CLK, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(IO_ETH_MII_TX_EN, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(IO_ETH_MII_TXD0, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(IO_ETH_MII_TXD1, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(IO_ETH_MII_TXD2, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(IO_ETH_MII_TXD3, ALT_FUNC, PUSH_PULL | SPEED_HIGH);
	stm32_gpio_mode(IO_ETH_MII_RX_CLK, ALT_FUNC, 0);
	stm32_gpio_mode(IO_ETH_MII_RX_DV, ALT_FUNC, 0);
	stm32_gpio_mode(IO_ETH_MII_RXD0, ALT_FUNC, 0);
	stm32_gpio_mode(IO_ETH_MII_RXD1, ALT_FUNC, 0);
	stm32_gpio_mode(IO_ETH_MII_RXD2, ALT_FUNC, 0);
	stm32_gpio_mode(IO_ETH_MII_RXD3, ALT_FUNC, 0);
	stm32_gpio_mode(IO_ETH_MII_RX_ER, ALT_FUNC, 0);
	stm32_gpio_mode(IO_ETH_MII_CRS, ALT_FUNC, 0);
	stm32_gpio_mode(IO_ETH_MII_COL, ALT_FUNC, 0);
	stm32_gpio_mode(IO_ETH_MDC, ALT_FUNC, PUSH_PULL | SPEED_MED);
	stm32_gpio_mode(IO_ETH_MDIO, ALT_FUNC, OPEN_DRAIN | PULL_UP | SPEED_MED);

}

bool board_init(void)
{
	DCC_LOG1(LOG_TRACE, "clk[AHB]=%d", stm32f_ahb_hz);
	DCC_LOG1(LOG_TRACE, "clk[APB1]=%d", stm32f_apb1_hz);
	DCC_LOG1(LOG_TRACE, "clk[TIM1]=%d", stm32f_tim1_hz);
	DCC_LOG1(LOG_TRACE, "clk[APB2]=%d", stm32f_apb2_hz);
	DCC_LOG1(LOG_TRACE, "clk[TIM2]=%d", stm32f_tim2_hz);

	DCC_LOG(LOG_TRACE, "io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "wave_play_init()");
//	wave_play_init(wave_c4, sizeof(wave_c4));

//	lattice_ice40_configure(ice40lp384_bin, sizeof_ice40lp384_bin);

	return true;
}

void board_comm_irqen(void)
{
	/* Enable USB OTG FS interrupts */
	cm3_irq_enable(STM32F_IRQ_OTG_FS);
}
	
void board_softreset(void)
{
	struct stm32_rcc * rcc = STM32_RCC;
#if 0
	struct stm32f_dma * dma;
	int i;

	/* disable DMA */
	dma = STM32F_DMA1;
	for(i = 0; i < 8; ++i) {
		dma->s[i].cr = 0;
		while (dma->s[i].cr & DMA_EN); 
	}

	dma = STM32F_DMA2;
	for(i = 0; i < 8; ++i) {
		dma->s[i].cr = 0;
		while (dma->s[i].cr & DMA_EN); 
	}
#endif

	DCC_LOG1(LOG_TRACE, "AHB1ENR=0x%08x", rcc->ahb1enr);
	DCC_LOG1(LOG_TRACE, "AHB2ENR=0x%08x", rcc->ahb2enr);
	DCC_LOG1(LOG_TRACE, "AHB3ENR=0x%08x", rcc->ahb3enr);
	DCC_LOG1(LOG_TRACE, "APB1ENR=0x%08x", rcc->apb1enr);
	DCC_LOG1(LOG_TRACE, "APB2ENR=0x%08x", rcc->apb2enr);

	/* Reset all peripherals except USB_OTG and GPIOA */
	rcc->ahb1rstr = ~(1 << RCC_GPIOA); 
	rcc->ahb2rstr = ~(1 << RCC_OTGFS);
	rcc->ahb3rstr = ~(0);
	rcc->apb1rstr = ~(0);
	rcc->apb2rstr = ~(0);


	rcc->ahb1rstr = 0;
	rcc->ahb2rstr = 0;
	rcc->ahb3rstr = 0;
	rcc->apb1rstr = 0;
	rcc->apb2rstr = 0;

	/* disable all peripherals clock sources except USB_OTG and GPIOA */
	rcc->ahb1enr = (1 << RCC_CCMDATARAM) | (1 << RCC_GPIOA); 
	rcc->ahb2enr = (1 << RCC_OTGFS);
	rcc->ahb3enr = 0;
	rcc->apb1enr = 0;
	rcc->apb2enr = 0;

	/* reinitialize IO's */
	io_init();

	board_comm_irqen();
}

void app_default(void * arg);

bool board_autoboot(uint32_t tick)
{
	int mode = 0;

	/* get the mode selection from the rotary switch (SW4) */
	mode |= stm32_gpio_stat(IO_MODE0) ? 0 : (1 << 0);
	mode |= stm32_gpio_stat(IO_MODE2) ? 0 : (1 << 1);
	mode |= stm32_gpio_stat(IO_MODE1) ? 0 : (1 << 2);
	mode |= stm32_gpio_stat(IO_MODE3) ? 0 : (1 << 3);

	if (mode == 8) { /* test mode */
		if (tick == 1)
			__thinkos_exec(0, app_default, NULL, false);
		return false;
	}

	if (tick & 1) {
		__led_on(IO_LED1);
		__led_on(IO_LED2);
	} else {
		__led_off(IO_LED1);
		__led_off(IO_LED2);
	}

	if (mode == 0) /* Production mode */
		return true; 

//	if (tick == 0)
//		wave_play();

	if (tick == 1)
		wave_pause();

	if (mode == 9) /* No autoboot mode */
		return false;

	/* Time window autoboot */
	return (tick < 40) ? false : true;
}

void board_on_appload(void)
{
	wave_pause();
	__led_off(IO_LED1);
	__led_off(IO_LED2);
}

struct board_cfg {
	uint32_t magic;
	uint32_t mstp_addr;
	uint32_t ip_addr;
};

#define CFG_MAGIC 0x01020304

#define IP4_ADDR1(A) ((int)(((uint32_t)(A)) & 0xff))
#define IP4_ADDR2(A) ((int)((((uint32_t)(A)) >> 8) & 0xff))
#define IP4_ADDR3(A) ((int)((((uint32_t)(A)) >> 16) & 0xff))
#define IP4_ADDR4(A) ((int)((((uint32_t)(A)) >> 24) & 0xff))
#define IP4_ADDR(A, B, C, D) ((((A) & 0xff) << 0) | \
		(((B) & 0xff) << 8) | (((C) & 0xff) << 16) | (((D) & 0xff) << 24))

bool board_configure(struct dmon_comm * comm)
{
	uint32_t offs = 0x10000 - 0x0100;
	struct board_cfg * cfg = (struct board_cfg *)(0x08000000 + offs);
	struct board_cfg buf;
	int n;
	int ip[4];

	if (cfg->magic == CFG_MAGIC) {
		dmprintf(comm, "MS/TP address: %d\r\n", cfg->mstp_addr);
		dmprintf(comm, "IP address: %d.%d.%d.%d\r\n", 
				 IP4_ADDR1(cfg->ip_addr),
				 IP4_ADDR2(cfg->ip_addr),
				 IP4_ADDR3(cfg->ip_addr),
				 IP4_ADDR4(cfg->ip_addr));
		return false;
	}
	do {
		dmprintf(comm, "MS/TP address > ");
		n = dmscanf(comm, "%d", &buf.mstp_addr);
		if (n < 0)
			return false;
	} while (n != 1);
	do {
		dmprintf(comm, "IP address > ");
		n = dmscanf(comm, "%3d.%3d.%3d.%3d", &ip[0], &ip[1], &ip[2], &ip[3]);
		if (n < 0)
			return false;
	} while (n != 4);

	buf.ip_addr = IP4_ADDR(ip[0], ip[1], ip[2], ip[3]);
	buf.magic = CFG_MAGIC;

	stm32_flash_write(offs, &buf, sizeof(buf));

	dmprintf(comm, "Configuation saved.\r\n");

	return true;
}

extern int __heap_base;
const int * heap_base_ = &__heap_base;

extern const uint8_t otg_xflash_pic[];
extern const unsigned int sizeof_otg_xflash_pic;

struct magic {
	uint32_t cnt;
	struct {
		uint32_t addr;
		uint32_t mask;
		uint32_t comp;
	} rec[];
};

const struct magic bootloader_magic = {
	.cnt = 4,
	.rec = {
		{  0x08000000, 0xffffffff, 0x10010000 },
		{  0x08000004, 0xffff0000, 0x08000000 },
		{  0x08000008, 0xffff0000, 0x08000000 },
		{  0x0800000c, 0xffff0000, 0x08000000 }
	}
};

void board_upgrade(struct dmon_comm * comm)
{
	uint32_t * xflash_code = (uint32_t *)(0x20001000);
	int (* xflash_ram)(uint32_t, uint32_t, const struct magic *) = 
		((void *)xflash_code) + 1;

	cm3_cpsid_f();
	__thinkos_memcpy(xflash_code, otg_xflash_pic, sizeof_otg_xflash_pic);
	xflash_ram(0, 65536, &bootloader_magic);
}

#ifndef ENABLE_PRIPHERAL_MEM
#define  ENABLE_PRIPHERAL_MEM 1
#endif

const struct mem_desc sram_desc = {
	.name = "RAM",
	.blk = {
		{ 0x10000000, BLK_RW, SZ_64K,  1 }, /*  CCM - Main Stack */

		{ 0x20000000, BLK_RO, SZ_4K,   1 }, /* Bootloader: 4KiB */
		{ 0x20001000, BLK_RW, SZ_4K,  27 }, /* Application: 108KiB */
		{ 0x2001c000, BLK_RW, SZ_16K,  1 }, /* SRAM 2: 16KiB */
		{ 0x20020000, BLK_RW, SZ_64K,  1 }, /* SRAM 3: 64KiB */

		{ 0x22000000, BLK_RO, SZ_512K, 1 }, /* Bootloader - bitband */
		{ 0x22080000, BLK_RW, SZ_512K, 27}, /* Application - bitband */
		{ 0x22e00000, BLK_RW, SZ_2M,   1 }, /* SRAM 2 - bitband */
		{ 0x23000000, BLK_RW, SZ_8M,   1 }, /* SRAM 3 - bitband */

#ifdef ENABLE_PRIPHERAL_MEM
		{ 0x40000000, BLK_RO, SZ_1M,   1 },  /* Peripheral */
		{ 0x42000000, BLK_RO, SZ_128M, 1 },  /* Peripheral - bitband */
#endif

		{ 0x00000000, 0, 0, 0 }
	}
}; 

const struct mem_desc flash_desc = {
	.name = "FLASH",
	.blk = {
		{ 0x08000000, BLK_RO, SZ_16K,  2 }, /* Bootloader */
		{ 0x08008000, BLK_RW, SZ_16K,  2 }, /* Application */
		{ 0x08010000, BLK_RW, SZ_64K,  1 }, /* Application */
		{ 0x08020000, BLK_RW, SZ_128K, 7 }, /* Application */
		{ 0x00000000, 0, 0, 0 }
	}
}; 

const struct thinkos_board this_board = {
	.name = "SLCDEV-HUB",
	.hw_ver = {
		.major = 0,
		.minor = 1,
	},
	.sw_ver = {
		.major = VERSION_MAJOR,
		.minor = VERSION_MINOR
	},
	.memory = {
		.ram = &sram_desc,
		.flash = &flash_desc
	},
	.application = {
		.start_addr = 0x08020000,
		.block_size = (7 * 128) * 1024
	},
	.init = board_init,
	.softreset = board_softreset,
	.autoboot = board_autoboot,
	.configure = board_configure,
	.upgrade = board_upgrade,
	.on_appload = board_on_appload,
	.comm_irqen = board_comm_irqen
};


void board_test(void * arg)
{
	DCC_LOG1(LOG_TRACE, "arg=%p", arg);

	for (;;) {
		__led_on(IO_LED1);

		thinkos_sleep(100);

		__led_off(IO_LED1);

		__led_on(IO_LED2);

		thinkos_sleep(100);

		__led_off(IO_LED2);
	}	
}

void app_default(void * arg) __attribute__((weak, alias("board_test")));

