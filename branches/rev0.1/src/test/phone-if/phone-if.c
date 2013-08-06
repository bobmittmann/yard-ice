/* 
 * File:	 i2c-slave.c
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
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <thinkos.h>
#include <sys/dcclog.h>

#include "io.h"
#include "i2c.h"
#include "dac.h"
#include "dgpot.h"
#include "adc.h"
#include "console.h"

#ifndef VERSION_MAJOR
#define VERSION_MAJOR 0
#endif

#ifndef VERSION_MINOR
#define VERSION_MINOR 1
#endif

#ifndef ENABLE_WATCHDOG
#define ENABLE_WATCHDOG 0
#endif

#ifndef ENABLE_MENU
#define ENABLE_MENU 1
#endif

#ifndef ENABLE_STATUS
#define ENABLE_STATUS 1
#endif

#if ENABLE_WATCHDOG
int wdt_task(void)
{
	struct stm32f_iwdg * iwdg = STM32F_IWDG;

	for(;;) {
		DCC_LOG(LOG_TRACE, "WDT");
		/* WD reset */
		iwdg->kr = 0xaaaa;
		thinkos_sleep(250);
	}
}

uint32_t wdt_stack[32];

void wdt_init(void)
{
	struct stm32f_iwdg * iwdg = STM32F_IWDG;
	DCC_LOG(LOG_TRACE, "1. WDT config");

	/* WD unlock */
	iwdg->kr = 0x5555;

	while (iwdg->sr & IWDG_PVU);
	iwdg->pr = IWDG_PR_16;  /* 0.4 ms tick */

	while (iwdg->sr & IWDG_RVU);
	iwdg->rlr = 2500;  /* 500 ms / 0.4 ms */

	/* WD start */
	iwdg->kr = 0xcccc;

	DCC_LOG(LOG_TRACE, "2. thinkos_thread_create()");
	thinkos_thread_create((void *)wdt_task, (void *)NULL,
						  wdt_stack, sizeof(wdt_stack), 
						  THINKOS_OPT_PRIORITY(3) | THINKOS_OPT_ID(3));

	thinkos_sleep(10);
	DCC_LOG(LOG_TRACE, "3. done.");
}
#endif

void sys_init(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;

	/* Enable IO clocks */
	stm32f_gpio_clock_en(STM32F_GPIOA);
	stm32f_gpio_clock_en(STM32F_GPIOB);
	stm32f_gpio_clock_en(STM32F_GPIOC);

	/* Enable Alternate Functions IO clock */
	rcc->apb2enr |= RCC_AFIOEN;
}



#define OPT_SELF_TEST 1
#define OPT_RESET 2

void self_test(void)
{
	int i;

	DCC_LOG(LOG_TRACE, "...");

	for (i = 0; i < 5; ++i) {
		led_on(i);
		relay_on(i);
		thinkos_sleep(100);
		led_off(i);
		relay_off(i);
		thinkos_sleep(100);
	}
}

void codec_reset(void)
{
	DCC_LOG(LOG_TRACE, "...");
	codec_rst_lo();
	udelay(10);
	codec_rst_hi();
}

void system_reset(void)
{
	DCC_LOG(LOG_TRACE, "...");

	thinkos_sleep(10);
    CM3_SCB->aircr =  SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
	for(;;);
}

extern unsigned int wave_max;

int	tone_set(int chan, int mode)
{
	DCC_LOG2(LOG_TRACE, "DAC%d mode=%d", chan, mode);

	if (mode > wave_max)
		mode = 0;

	dac_wave_set(chan, mode);

	return mode;
}

struct io_block {
	uint8_t id[2];
	uint8_t ver[2];
	uint16_t adc[ADC_CHANS];
	uint8_t led;
	uint8_t relay;
	uint8_t dgpot[2];
	uint8_t tone[2];
	uint8_t codec_rst;
	uint8_t system_rst;
};

struct io_block rd_block = {
	.id = { 'P', 'H' },
	.ver = { VERSION_MAJOR, VERSION_MINOR }
};

struct io_block wr_block;

void chan_toggle(int chan)
{
	printf("Chan %d ", chan + 1);

	if (rd_block.relay & (1 << chan)) {
		relay_off(chan);
		led_off(chan);
		rd_block.relay &= ~(1 << chan);
		rd_block.led &= ~(1 << chan);
		printf("off.\n");
	} else {
		relay_on(chan);
		led_on(chan);
		rd_block.relay |= (1 << chan);
		rd_block.led |= (1 << chan);
		printf("on.\n");
	}
	wr_block.relay = rd_block.relay;
	wr_block.led = rd_block.led;
}

void tone_cycle(int chan)
{
	unsigned int freq;
	int mode;

	mode = rd_block.tone[chan] + 1;
	if (mode > wave_max)
		mode = 0;

	DCC_LOG2(LOG_TRACE, "DAC%d mode=%d", chan, mode);

	freq = dac_wave_set(chan, mode);

	rd_block.tone[chan] = mode;
	wr_block.tone[chan] = mode;

	printf("Tone%d: %dHz.\n", chan + 1, freq);
}

#if ENABLE_MENU
void show_menu(void)
{
	printf("\n");
	printf(" Options:\n");
	printf(" --------\n");
	printf("   1 - Toggle Chan 1\n");
	printf("   2 - Toggle Chan 2\n");
	printf("   3 - Toggle Chan 3\n");
	printf("   4 - Toggle Chan 4\n");
	printf("   5 - Toggle Chan 5\n");

	printf("   a - Tone 1 Select\n");
	printf("   b - Tone 2 Select\n");

	printf("   - - Gain -\n");
	printf("   = - Gain +\n");

	printf("   [ - Impedance -\n");
	printf("   ] - Impedance +\n");

	printf("   p - Print ADC\n");
 
	printf("   i - i2c reset\n");
	printf("   r - system reset\n");
	printf("   t - test\n");

	printf("\n");
}
#endif

void shell_task(void)
{
	int c;
	int val;
	int gain;
	int i;

	printf("\n\n");
	i = printf(" Firefighter Phone Interface %d.%d\n", 
			   VERSION_MAJOR, VERSION_MINOR);
	printf(" ");
	i -= 2;
	while (i-- > 0)
		printf("-");
	printf("\n\n");


	for(;;) {
		c = getchar();
		switch (c) {

#if ENABLE_MENU
		case '\n':
			show_menu();
			break;
#endif

		case '1':
			chan_toggle(0);
			break;
		case '2':
			chan_toggle(1);
			break;
		case '3':
			chan_toggle(2);
			break;
		case '4':
			chan_toggle(3);
			break;
		case '5':
			chan_toggle(4);
			break;

		case 'a':
			tone_cycle(0);
			break;
		case 'b':
			tone_cycle(1);
			break;

		case '[':
			val = dgpot_set(0, rd_block.dgpot[0] - 1);
			rd_block.dgpot[0] = val;
			wr_block.dgpot[0] = val;
			printf("Impedance: %d\n", (val * 5000) / 63);
			break;
		case ']':
			val = dgpot_set(0, rd_block.dgpot[0] + 1);
			rd_block.dgpot[0] = val;
			wr_block.dgpot[0] = val;
			printf("Impedance: %d\n", (val * 5000) / 63);
			break;

		case '-':
			val = dgpot_set(1, rd_block.dgpot[1] - 1);
			rd_block.dgpot[1] = val;
			wr_block.dgpot[1] = val;
			gain = 100 + (5000 * val) / (25 * 63);
			printf("Gain: %d.%02d\n", gain / 100, gain % 100);
			break;
		case '=':
			val = dgpot_set(1, rd_block.dgpot[1] + 1);
			rd_block.dgpot[1] = val;
			wr_block.dgpot[1] = val;
			gain = 100 + (5000 * val) / (25 * 63);
			printf("Gain: %d.%02d\n", gain / 100, gain % 100);
			break;

		case 'p':
			printf("ADC:");
			for (i = 0; i < 5; ++i)
				printf(" %5d", rd_block.adc[i]);
			printf("\n");
			break;

		case 'i':
			printf("I2C reset\n");
			i2c_reset();
			break;
		case 'r':
			printf("System reset\n");
			system_reset();
			break;
		case 't':
			printf("Self teset\n");
			self_test();
			break;
#if 0
		default:
			printf("%c", c);
#endif
			break;
		}
	}
}

uint32_t shell_stack[128];

void shell_init(void)
{
	DCC_LOG(LOG_TRACE, "thinkos_thread_create()");
	thinkos_thread_create((void *)shell_task, (void *)NULL,
						  shell_stack, sizeof(shell_stack), 
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));
}


void process_data_in(void)
{
	uint8_t set;
	uint8_t clr;
	uint8_t val;
	int i;

	DCC_LOG(LOG_TRACE, "...");

	set = wr_block.led & (wr_block.led ^ rd_block.led);
	clr = rd_block.led & (wr_block.led ^ rd_block.led);

	for (i = 0; i < 5; ++i) {
		if (set & (1 << i)) {
#if ENABLE_STATUS
			printf("[LED%d ON]", i);
#endif
			led_on(i);
		} else if (clr & (1 << i)) {
#if ENABLE_STATUS
			printf("[LED%d OFF]", i);
#endif
			led_off(i);
		}
	}
	
	/* update read block */
	rd_block.led = wr_block.led;

	set = wr_block.relay & (wr_block.relay ^ rd_block.relay);
	clr = rd_block.relay & (wr_block.relay ^ rd_block.relay);

	for (i = 0; i < 5; ++i) {
		if (set & (1 << i)) {
#if ENABLE_STATUS
			printf("[RELAY%d ON]", i);
#endif
			relay_on(i);
		} else if (clr & (1 << i)) {
#if ENABLE_STATUS
			printf("[RELAY%d OFF]", i);
#endif
			relay_off(i);
		}
		rd_block.relay = wr_block.relay;
	}

	val = dgpot_set(0, wr_block.dgpot[0]);
	if (val != rd_block.dgpot[0]) {
		rd_block.dgpot[0] = val;
#if ENABLE_STATUS
		printf("[POT0 %d]", val);
#endif
	}

	val = dgpot_set(1, wr_block.dgpot[1]);
	if (val != rd_block.dgpot[1]) {
		rd_block.dgpot[1] = val;
#if ENABLE_STATUS
		printf("[POT1 %d]", val);
#endif
	}
	
	if (wr_block.tone[0] != rd_block.tone[0]) {
		val = tone_set(0, wr_block.tone[0]);
		rd_block.tone[0] = val;
#if ENABLE_STATUS
		printf("[TONE1 %d]", val);
#endif
	}

	if (wr_block.tone[1] != rd_block.tone[1]) {
		val = tone_set(1, wr_block.tone[1]);
		rd_block.tone[1] = val;
#if ENABLE_STATUS
		printf("[TONE1 %d]", val);
#endif
	}

	if (wr_block.codec_rst == 'R') {
#if ENABLE_STATUS
		printf("[TLV RST]");
#endif
		codec_reset();
	}

	if (wr_block.system_rst == 'S') {
#if ENABLE_STATUS
		printf("[RESET]");
#endif
		system_reset();
	}

	/* reset the write block */
	memcpy(&wr_block, &rd_block, sizeof(struct io_block));
}

#define DEVICE_ADDR 0x55

int main(int argc, char ** argv)
{
	int xfer;
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "2. sys_init()");
	sys_init();

	DCC_LOG(LOG_TRACE, "3. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "4. stdio_init()");
	stdio_init();

#if ENABLE_WATCHDOG
	DCC_LOG(LOG_TRACE, "5. wdt_init()");
	wdt_init();
#endif

	DCC_LOG(LOG_TRACE, "6. leds_init()");
	leds_init();
	leds_all_on();

	DCC_LOG(LOG_TRACE, "7. relays_init()");
	relays_init();

	DCC_LOG(LOG_TRACE, "8. codec_io_init()");
	codec_io_init();

	DCC_LOG(LOG_TRACE, "9. dac_init()");
	dac_init();

	DCC_LOG(LOG_TRACE, "10. adc_init()");
	adc_init(rd_block.adc);

	DCC_LOG(LOG_TRACE, "11. dgpot_init()");
	dgpot_init();

	DCC_LOG(LOG_TRACE, "12. i2c_slave_init()");
	i2c_slave_init(100000, DEVICE_ADDR, &rd_block, &wr_block, 
				   sizeof(struct io_block));

	DCC_LOG(LOG_TRACE, "13. adc_start()");
	adc_start();

	DCC_LOG(LOG_TRACE, "14. dac_start()");
	dac_start();

	DCC_LOG(LOG_TRACE, "15. i2c_slave_enable()");
	i2c_slave_enable();

	DCC_LOG(LOG_TRACE, "16. shell_init()");
	shell_init();

	DCC_LOG(LOG_TRACE, "17. codec_rst_hi()");
	codec_rst_hi();

	leds_all_off();

	for (i = 0; ; ++i) {
		xfer = i2c_slave_io();
		switch (xfer) {
		case I2C_XFER_IN:
			DCC_LOG(LOG_TRACE, "IN");
#if ENABLE_STATUS
			printf(".I");
#endif
			process_data_in();
			break;
		case I2C_XFER_OUT:
#if ENABLE_STATUS
			printf(".O");
#endif
			DCC_LOG5(LOG_TRACE, "OUT %5d %5d %5d %5d %5d ", 
					 rd_block.adc[0], rd_block.adc[1], 
					 rd_block.adc[2], rd_block.adc[3], 
					 rd_block.adc[4] );
			break;
		case I2C_XFER_ERR:
#if ENABLE_STATUS
			printf(".E");
#endif
			break;
		}

	}

	return 0;
}

