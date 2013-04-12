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

#include "i2c.h"
#include "console.h"
#include "io.h"
#include "i2s.h"
#include "tlv320.h"
#include "trace.h"
#include "vt100.h"

/* ----------------------------------------------------------------------
 * Supervisory task
 * ----------------------------------------------------------------------
 */
int supervisor_task(void)
{
	printf("%s() started...\n", __func__);

	for (;;) {
		led_on(2);
		thinkos_sleep(100);
		trace_print(stdout, 1);
		led_off(2);
		thinkos_sleep(400);
	}
}

void system_reset(void)
{
	DCC_LOG(LOG_TRACE, "...");

	thinkos_sleep(10);
    CM3_SCB->aircr =  SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
	for(;;);
}


int32_t i2c_mutex;
int32_t phif_addr = 0x55;
int32_t codec_addr = 64;

#define PHIF_ID_REG 0
#define PHIF_VER_REG 2
#define PHIF_ADC_REG 4
#define PHIF_LED_REG 14
#define PHIF_RLY_REG 15
#define PHIF_VR0_REG 16
#define PHIF_VR1_REG 17

#define LINE_TROUBLE_OPEN 0
#define LINE_TROUBLE_SHORT 1
#define LINE_ON_HOOK 2
#define LINE_OFF_HOOK 3

struct {
	int sup_st;
	bool connected;
} line[5];

uint16_t adc[5];

struct {
	uint8_t led;
	uint8_t relay;
	uint8_t vr[2];
} cache;

int line_set_connect(int line_idx, bool connect)
{
	uint8_t pkt[3];
	unsigned int led;
	unsigned int relay;
	int ret;

	thinkos_mutex_lock(i2c_mutex);

	led = cache.led;
	relay = cache.relay;

	if (connect) {
		led |= (1 << line_idx);
		relay |= (1 << line_idx);
	} else {
		led &= ~(1 << line_idx);
		relay &= ~(1 << line_idx);
	}

	pkt[0] = PHIF_LED_REG;
	pkt[1] = led;
	pkt[2] = relay;

	if ((ret = i2c_master_wr(phif_addr, pkt, 3)) > 0) {
		cache.led = led;
		cache.relay = relay;
		line[line_idx].connected = connect;
	} else {
		tracef("%s(): i2c_master_wr() failed!", __func__);
	}

	thinkos_mutex_unlock(i2c_mutex);

	return ret;
}

struct {
	uint8_t impedance;
	uint8_t gain;
} hybrid;

int hybrid_adjust(int impedance, int gain)
{

	uint8_t pkt[3];
	int ret;

	thinkos_mutex_lock(i2c_mutex);

	if (impedance < 0 )
		impedance = 0;
	else if (impedance > 63)
		impedance = 63;

	if (gain < 0)
		gain = 0;
	else if (gain > 63)
		gain = 63;

	pkt[0] = PHIF_VR0_REG;
	pkt[1] = impedance;
	pkt[2] = gain;

	if ((ret = i2c_master_wr(phif_addr, pkt, 3)) > 0) {
		hybrid.impedance = impedance;;
		hybrid.gain = gain;
	} else {
		tracef("%s(): i2c_master_wr() failed!", __func__);
	}

	thinkos_mutex_unlock(i2c_mutex);

	return ret;
}

int connect_off_hok(void)
{
	uint8_t pkt[3];
	int i;

	DCC_LOG(LOG_TRACE, ".");

	printf("\nConnect off-hook... ");

	thinkos_mutex_lock(i2c_mutex);

	pkt[0] = PHIF_LED_REG;
	pkt[1] = 0;
	pkt[2] = 0;

	for (i = 0; i < 5; ++i) {
		if (line[i].sup_st == LINE_OFF_HOOK) {
			pkt[1] |= (1 << i);
			pkt[2] |= (1 << i);
			printf("%d ", i);
		}
	}

	printf("\n");

	if (i2c_master_wr(phif_addr, pkt, 3) > 0) {
//		printf("i2c_master_wr() ok.\n");
		DCC_LOG(LOG_INFO, "ok.");
	} else {
//		printf("i2c_master_wr() failed!\n");
		DCC_LOG(LOG_WARNING, "i2c_master_wr() failed!");
	}

	thinkos_mutex_unlock(i2c_mutex);

	return 0;
}

int hangup_all(void)
{
	uint8_t pkt[3];

	printf("\nHangup all...\n");
	DCC_LOG(LOG_TRACE, ".");

	thinkos_mutex_lock(i2c_mutex);

	pkt[0] = PHIF_LED_REG;
	pkt[1] = 0;
	pkt[2] = 0;

	if (i2c_master_wr(phif_addr, pkt, 3) > 0) {
//		printf("i2c_master_wr() ok.\n");
		DCC_LOG(LOG_INFO, "ok.");
	} else {
//		printf("i2c_master_wr() failed!\n");
		DCC_LOG(LOG_WARNING, "i2c_master_wr() failed!");
	}

	thinkos_mutex_unlock(i2c_mutex);

	return 0;
}	

void line_toggle(int line_idx)
{
	bool connect;

	printf("Line %d ", line_idx + 1);

	connect = line[line_idx].connected ? false : true;

	line_set_connect(line_idx, connect);

	printf("%s.\n", line[line_idx].connected ? "connected" : "disconnected");
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
//		printf("i2c_master_wr() ok.\n");
		DCC_LOG(LOG_INFO, "ok.");
	} else {
//		printf("i2c_master_wr() failed!\n");
		DCC_LOG(LOG_WARNING, "i2c_master_wr() failed!");
	}

	thinkos_mutex_unlock(i2c_mutex);
}

int acq_task(void)
{
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

//				tracef("ADC %5d %5d %5d %5d %5d", 
//					   adc[0], adc[1], adc[2], adc[3], adc[4]);

				printf("ADC: ");


				for (i = 0; i < 5; ++i) {
					printf("%5d", adc[i]);
					if (adc[i] < 3) {
						line[i].sup_st = LINE_TROUBLE_OPEN;
					} else if (adc[i] < 40) {
						line[i].sup_st = LINE_ON_HOOK;
					} else if (adc[i] < 8000) {
						line[i].sup_st = LINE_OFF_HOOK;
					} else {
						line[i].sup_st = LINE_TROUBLE_SHORT;
					}
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

void i2c_bus_scan(void)
{
	uint8_t buf[4];
	uint8_t addr = 0;

	thinkos_mutex_lock(i2c_mutex);

	printf("- I2C bus scan: ");

	/* 7 bit addresses range from 0 to 0x78 */
	for (addr = 1; addr < 0x78; ++addr) {

		DCC_LOG1(LOG_INFO, "Addr=0x%02x", addr);

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


int tlv320_wr(unsigned int reg, unsigned int val)
{
	uint8_t pkt[2];

	DCC_LOG2(LOG_TRACE, "reg=%d val=0x%02x", reg, val);

	thinkos_mutex_lock(i2c_mutex);

	pkt[0] = reg;
	pkt[1] = val;

	if (i2c_master_wr(codec_addr, pkt, 2) > 0) {
		printf("i2c_master_wr() ok.\n");
		DCC_LOG(LOG_INFO, "ok.");
	} else {
		printf("i2c_master_wr() failed!\n");
		DCC_LOG(LOG_WARNING, "i2c_master_wr() failed!");
	}

	thinkos_mutex_unlock(i2c_mutex);

	return 0;
}

/* Divider values of M, N, and P to be used in junction with the 
   FSDIV bit for calculation of FS frequency according to the 
   formula FS = MCLK / (16 x M x N x P) */
/* MCLK = 22579200 / 2 = 11289600 */ 
/* FS = 8000 */ 
void tlv320_init(void)
{
	printf("%s()... \n", __func__);

	/* reset the device */
	tlv320_wr(3, CR3_PWDN_NO | CR3_SWRS);
	tlv320_wr(0, 0);
	tlv320_wr(0, 0);
	tlv320_wr(2, CR2_DIFBP | CR2_I2CX_SET(4) | CR2_HPC_I2C);
	tlv320_wr(3, CR3_PWDN_NO | CR3_SWRS | CR3_OSR_256 | CR3_ASRF_1);
	tlv320_wr(4, CR4_M_SET(44));
	tlv320_wr(4, CR4_NP_SET(1, 2));
	tlv320_wr(5, CR5A_ADGAIN_DB(0));
	tlv320_wr(5, CR5B_DAGAIN_DB(0));
	tlv320_wr(5, CR5C_DGSTG_MUTE | CR5C_INBG_0DB);
	tlv320_wr(6, CR3_AINSEL_INP_M1);
	tlv320_wr(1, CR1_CX | CR1_IIR | CR1_BIASV_LO | CR1_DAC16);
};

uint32_t supervisor_stack[256];
uint32_t ui_stack[256];
uint32_t acq_stack[256];

/* ----------------------------------------------------------------------
 * User interface task
 * ----------------------------------------------------------------------
 */
int ui_task(void)
{
	int btn_st[2];
	int ev_press;
	int ev_release;
	int ev_dbl_click;
	int rst_tmr = 0;
	int click_cnt = 0;
	int click_tmr = 0;


	printf("%s() started...\n", __func__);

	btn_st[0] = push_btn_stat();
	for (;;) {

		/* process push button */
		btn_st[1] = push_btn_stat();

		ev_press = btn_st[1] & (btn_st[1] ^ btn_st[0]);
		ev_release = btn_st[0] & (btn_st[1] ^ btn_st[0]);
		ev_dbl_click = 0;
		btn_st[0] = btn_st[1];

		if (ev_press) {
			DCC_LOG(LOG_TRACE, "BTN Down");
			printf("[DWN]");
			/* set reset timer */
			rst_tmr = 50;
			/* reset click window timer */
			click_tmr = 10;
			/* update click counter */
			click_cnt++;
			if (click_cnt == 2) {
				/* generate a double click event */
				ev_dbl_click = 1;
			}
			connect_off_hok();
		}

		if (ev_release) {
			DCC_LOG(LOG_TRACE, "BTN Up");
			printf("[UP]");
			/* clear 'reset timer' */
			rst_tmr = 0;
		}

		if (ev_dbl_click) {
			printf("[DB CLK]");
//			i2c_reset();
			hangup_all();
		}

		if (rst_tmr)
			rst_tmr--;

		if (click_tmr) {
			if (--click_tmr == 0)
				click_cnt = 0;
		}

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

uint32_t tim_irq_cnt;

void stm32f_tim2_isr(void)
{
	struct stm32f_tim * tim = STM32F_TIM2;
	/* Clear interrupt flags */
	tim->sr = 0;

	tracef("%s(): irq_cnt=%d", __func__, ++tim_irq_cnt);
}

int i2s_dac_tone = 0;
int i2s_dac_gain = 0;

int tone_amp_ltu[] = {
	    0,
	  127,
	  255,
	  511,
 	 1023,
	 2047,
	 4095,
	 8191,
	16383,
	32767
};

void dac_gain_step(int d)
{
	i2s_dac_gain += d;

	if (i2s_dac_gain < 0) {
		i2s_dac_gain = 0;
	} else if (i2s_dac_gain > 9) {
		i2s_dac_gain = 9;
	}

	i2s_tone_set(i2s_dac_tone, tone_amp_ltu[i2s_dac_gain]);

	printf("DAC gain: %d\n", i2s_dac_gain);
}

void dac_tone_cycle(void)
{
	unsigned int freq;

	i2s_dac_tone = (i2s_dac_tone == 10) ? 0 : i2s_dac_tone + 1;

	freq = i2s_tone_set(i2s_dac_tone, tone_amp_ltu[i2s_dac_gain]);

	printf("DAC tone: %dHz.\n", freq);
}

void hybrid_step_impedance(int d)
{
	hybrid_adjust(hybrid.impedance + d, hybrid.gain);
	printf("Impedance: %d\n", (hybrid.impedance * 5000) / 63);
}

void hybrid_step_gain(int d)
{
	unsigned int gain;

	hybrid_adjust(hybrid.impedance, hybrid.gain + d);
	gain = 100 + (5000 * hybrid.gain) / (25 * 63);
	printf("Gain: %d.%02d\n", gain / 100, gain % 100);
}

void show_menu(void)
{
	printf("\n");
	printf(" Options:\n");
	printf(" --------\n");
	printf("   1..5 - Toggle channel\n");
	printf("   9 0  - Hybrid impedance\n");
	printf("   - =  - Hybrid gain\n");
	printf("   r    - dump I2S RX buffer\n");
	printf("   t    - dump I2S TX buffer\n");
	printf("   d    - DAC tone select\n");
	printf("   [ ]  - DAC tone gain\n");
	printf("   s    - system reset\n");

	printf("\n");
}

struct {
	volatile int chan;
	volatile int count;
	int flag;
} spectrum_analyzer;

void spectrum_analyzer_task(void)
{
	while (1) {
		thinkos_flag_wait(spectrum_analyzer.flag);
		thinkos_flag_clr(spectrum_analyzer.flag);

		printf(VT100_CLRSCR);

		while (spectrum_analyzer.count) {

			if (spectrum_analyzer.chan == 1)
				i2s_rx_analyze();
			else
				i2s_tx_analyze();

			spectrum_analyzer.count--;
		};
	}
}

void shell_task(void)
{
	int c;

	printf("-------------\n");
	printf(" Audio Shell \n");
	printf("-------------\n");
	printf("\n");

	for(;;) {
		c = getchar();
		switch (c) {

		case '\n':
			show_menu();
			break;

		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			line_toggle(c - '1');
			break;

		case 'R':
			i2s_rx_dump();
			break;
		case 'r':
			spectrum_analyzer.chan = 1;
			spectrum_analyzer.count = 10000;
			thinkos_flag_set(spectrum_analyzer.flag);
			break;
		case 'T':
			i2s_tx_dump();
			break;
		case 't':
			spectrum_analyzer.chan = 0;
			spectrum_analyzer.count = 10000;
			thinkos_flag_set(spectrum_analyzer.flag);
			break;
		case 'b':
			spectrum_analyzer.count = 0;
			break;
		case 'd':
			dac_tone_cycle();
			break;
		case 's':
			system_reset();
			break;
		case '[':
			dac_gain_step(-1);
			break;
		case ']':
			dac_gain_step(1);
			break;

		case '9':
			hybrid_step_impedance(-1);
			break;
		case '0':
			hybrid_step_impedance(1);
			break;

		case '-':
			hybrid_step_gain(-1);
			break;
		case '=':
			hybrid_step_gain(1);
			break;
		}
	}
}

uint32_t spectrum_analyzer_stack[1024];

void spectrum_analyzer_init(void)
{
	DCC_LOG(LOG_TRACE, "thinkos_thread_create()");

	spectrum_analyzer.flag = thinkos_flag_alloc();

	thinkos_thread_create((void *)spectrum_analyzer_task, (void *)NULL,
						  spectrum_analyzer_stack, 
						  sizeof(spectrum_analyzer_stack), 
						  THINKOS_OPT_PRIORITY(7) | THINKOS_OPT_ID(7));

	thinkos_sleep(10);
}

uint32_t shell_stack[128];

void shell_init(void)
{
	DCC_LOG(LOG_TRACE, "thinkos_thread_create()");
	thinkos_thread_create((void *)shell_task, (void *)NULL,
						  shell_stack, sizeof(shell_stack), 
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(4));

	thinkos_sleep(10);
}

void timer_init(uint32_t freq)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	struct stm32f_tim * tim = STM32F_TIM2;
	uint32_t div;
	uint32_t pre;
	uint32_t n;

	/* get the total divisior */
	div = ((2 * stm32f_apb1_hz) + (freq / 2)) / freq;
	/* get the minimum pre scaler */
	pre = (div / 65536) + 1;
	/* get the reload register value */
	n = (div * 2 + pre) / (2 * pre);

	DCC_LOG3(LOG_TRACE, "freq=%dHz pre=%d n=%d", freq, pre, n);
	DCC_LOG1(LOG_TRACE, "real freq=%dHz\n", (2 * stm32f_apb1_hz) / pre / n);

	/* Timer clock enable */
	rcc->apb1enr |= RCC_TIM2EN;
	
	/* Timer configuration */
	tim->psc = pre - 1;
	tim->arr = n - 1;
	tim->cnt = 0;
	tim->egr = 0;
	tim->dier = TIM_UIE; /* Update interrupt enable */
	tim->ccmr1 = TIM_OC1M_PWM_MODE1;
	tim->ccr1 = tim->arr - 2;

	/* Enable interrupt */
	cm3_irq_enable(STM32F_IRQ_TIM2);

	tim->cr1 = TIM_URS | TIM_CEN; /* Enable counter */
}

int main(int argc, char ** argv)
{
	int i;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	/* calibrate usecond delay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "1. trace_init()");
	trace_init();

	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(32));

	DCC_LOG(LOG_TRACE, "3. io_init()");
	io_init();

	DCC_LOG(LOG_TRACE, "4. stdio_init()");
	stdio_init();

	DCC_LOG(LOG_TRACE, "5. leds_init()");
	leds_init();


	DCC_LOG(LOG_TRACE, "7. i2s_slave_init()");
	i2s_slave_init();

	DCC_LOG(LOG_TRACE, "6. i2c_master_init()");
	i2c_master_init(100000);

	i2c_mutex = thinkos_mutex_alloc();
	tracef("I2C mutex=%d\n", i2c_mutex);

	printf("\n\n");
	printf("\n\n");
	printf("-----------------------------------------\n");
	printf(" Audio interface test\n");
	printf("-----------------------------------------\n");
	printf("\n");

	thinkos_thread_create((void *)supervisor_task, (void *)NULL,
						  supervisor_stack, sizeof(supervisor_stack), 
						  THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	DCC_LOG(LOG_TRACE, "8. i2c_master_enable()");
	i2c_master_enable();

	thinkos_sleep(100);

	tracef("i2c_bus_scan()");
	i2c_bus_scan();

	tracef("tlv320_init()");
	tlv320_init();

//	thinkos_thread_create((void *)ui_task, (void *)NULL,
//						  ui_stack, sizeof(ui_stack), 
//						  THINKOS_OPT_PRIORITY(1) | THINKOS_OPT_ID(1));
/*
	thinkos_thread_create((void *)acq_task, (void *)NULL,
						  acq_stack, sizeof(acq_stack), 
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));
*/

	DCC_LOG(LOG_TRACE, "8. i2s_enable()");
	tracef("i2s_enable()");
	i2s_enable();

	DCC_LOG(LOG_TRACE, "17. shell_init()");
	spectrum_analyzer_init();
	shell_init();

//	timer_init(500);

//	supervisor_task();

	for (i = 0; ; ++i) {
//		thinkos_sleep(1);
//		tracef("alive!");
//		printf("[alive!]");
//		vr_set(i, 2 * i);
	}


	return 0;
}

