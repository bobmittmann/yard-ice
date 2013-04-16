/* 
 * File:	 audio-if.c
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
#include "audio.h"
#include "trace.h"
#include "fixpt.h"
#include "sndbuf.h"
#include "vt100.h"
#include "telctl.h"
#include "tonegen.h"
#include "net.h"

/* ----------------------------------------------------------------------
 * Supervisory task
 * ----------------------------------------------------------------------
 */
int supervisor_task(void)
{
	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

	for (;;) {
//		led_on(LED_S2);
		led_flash(LED_S2, 100);
		thinkos_sleep(100);
		trace_print(stdout, 1);
//		led_off(LED_S2);
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

const char sup_nm_ltu[][4] = {
	"UN",
	"OP",
	"ST",
	"ID",
	"1P",
	"2P",
	"3P",
	"4P",
	"5P"
};

void line_supv_status(void)
{
	char s[64];
	char * cp = s;
	int i;
	int g;
	int z;

	g = telctl.hybrid.g;
	z = telctl.hybrid.z;

	cp += sprintf(cp, VT100_CURSOR_SAVE VT100_GOTO, 1, 53); 
	cp += sprintf(cp, "%4d %d.%02d ", z, g / 100, g % 100);

	for (i = 0; i < 5; ++i) {
		cp += sprintf(cp, "%s ", sup_nm_ltu[telctl.line[i].sup_st]);
	}

	cp += sprintf(cp, VT100_CURSOR_UNSAVE); 

	printf(s);
}

int acq_task(void)
{
	int z;
	int vr;

	DCC_LOG(LOG_TRACE, "started...");
	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

	for (;;) {
		DCC_LOG(LOG_INFO, "Poll...");
		thinkos_sleep(500);
		if (telctl_adc_scan() >= 0) { 
			z = (1000 * telctl.load.cnt) - 500;
			vr = (z * 63) / 5000;
			hybrid_impedance_set(vr);
		};

		line_disconnect_on_hook();

		line_supv_status();
	}
}

int i2s_dac_tone = 0;
int i2s_dac_gain = 0;

void dac_gain_step(int d)
{
	char msg[32];
	char * cp = msg;
	int gain;

	gain = i2s_dac_gain + d;

	if (gain > 0) {
		gain = 0;
	} else if (gain < q15_db2amp_min) {
		gain = i2s_dac_gain;
	}

	audio_tone_set(i2s_dac_tone, gain);
	
	i2s_dac_gain = gain;

	cp += sprintf(cp, VT100_GOTO, 0, 0); 
	cp += sprintf(cp, "DAC gain: %ddB", i2s_dac_gain);
	cp += sprintf(cp, VT100_CLREOL); 
	printf(msg);

}

void dac_tone_cycle(void)
{
	unsigned int freq;
	char msg[32];
	char * cp = msg;

	i2s_dac_tone = (i2s_dac_tone == wave_max) ? 0 : i2s_dac_tone + 1;

	freq = audio_tone_set(i2s_dac_tone, i2s_dac_gain);

	cp += sprintf(cp, VT100_GOTO, 0, 0); 
	cp += sprintf(cp, "DAC tone: %dHz", freq);
	cp += sprintf(cp, VT100_CLREOL); 
	printf(msg);
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
	volatile bool enabled;
	int flag;
} spectrum_analyzer;

void spectrum_analyzer_task(void)
{
	while (1) {
		thinkos_flag_wait(spectrum_analyzer.flag);
		thinkos_flag_clr(spectrum_analyzer.flag);

		printf(VT100_CLRSCR);

		while (spectrum_analyzer.enabled) {

			if (spectrum_analyzer.chan == 1)
				audio_rx_analyze();
			else
				audio_tx_analyze();

			spectrum_analyzer.count--;
			if (spectrum_analyzer.count == 0)
				spectrum_analyzer.enabled = false;

//			thinkos_sleep(200);
		};
	}
}

void phone_test(void)
{
	int i;

	i2s_dac_gain = 0;

	for (i = 2; i < wave_max; ++i) {
		audio_tone_set(i2s_dac_tone = i, i2s_dac_gain);
		thinkos_sleep(1000);
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

		case 'r':
			spectrum_analyzer.chan = 1;
			spectrum_analyzer.count = 10000;
			spectrum_analyzer.enabled = true;
			thinkos_flag_set(spectrum_analyzer.flag);
			break;
		case 't':
			spectrum_analyzer.chan = 0;
			spectrum_analyzer.count = 10000;
			spectrum_analyzer.enabled = true;
			thinkos_flag_set(spectrum_analyzer.flag);
			break;
		case 'b':
			spectrum_analyzer.enabled = false;
			break;
		case 'd':
			dac_tone_cycle();
			break;
		case 'e':
			audio_enable();
			break;
		case 'o':
			audio_disable();
			break;
		case 's':
			system_reset();
			break;
		case '[':
			dac_gain_step(-6);
			break;
		case ']':
			dac_gain_step(6);
			break;

		case 'z':
			phone_test();
			break;

		case 'a':
			printf(" " VT100_CLRSCR);
			audio_reset();
			break;

		case '9':
			hybrid_impedance_step(-1);
			break;
		case '0':
			hybrid_impedance_step(1);
			break;

		case '-':
			hybrid_gain_step(-1);
			break;
		case '=':
			hybrid_gain_step(1);
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


	tracef("%s(): <%d> started...", __func__, thinkos_thread_self());

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
			line_connect_off_hook();
		}

		if (ev_release) {
			DCC_LOG(LOG_TRACE, "BTN Up");
			printf("[UP]");
			/* clear 'reset timer' */
			rst_tmr = 0;
		}

		if (ev_dbl_click) {
			printf("[DB CLK]");
			line_hangup_all();
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
		case 12:
		case 8:
		case 4:
			led_on(0);
			led_on(1);
			led_on(2);
			led_on(3);
			break;
		case 17:
		case 13:
		case 9:
		case 5:
			led_off(0);
			led_off(1);
			led_off(2);
			led_off(3);
			break;
		case 1:
			system_reset();
			break;
		}

		thinkos_sleep(50);
	}
}

uint32_t supervisor_stack[256];
uint32_t acq_stack[128];

int main(int argc, char ** argv)
{
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

	printf(" " VT100_CLRSCR);
	printf("\n\n");
	printf("\n\n");
	printf("-----------------------------------------\n");
	printf(" Audio interface test\n");
	printf("-----------------------------------------\n");
	printf("\n");

	DCC_LOG(LOG_TRACE, "5. leds_init()");
	leds_init();

	thinkos_thread_create((void *)supervisor_task, (void *)NULL,
						  supervisor_stack, sizeof(supervisor_stack), 
						  THINKOS_OPT_PRIORITY(4) | THINKOS_OPT_ID(8));

	DCC_LOG(LOG_TRACE, "6. sndbuf_pool_init()");
	sndbuf_pool_init();

	DCC_LOG(LOG_TRACE, "6. i2c_init()");
	i2c_init();

	net_init();

	/* give a little time for the slave board start-up */
	thinkos_sleep(100);

	thinkos_thread_create((void *)acq_task, (void *)NULL,
						  acq_stack, sizeof(acq_stack), 
						  THINKOS_OPT_PRIORITY(2) | THINKOS_OPT_ID(2));

	DCC_LOG(LOG_TRACE, "6. i2c_init()");
	audio_init();

	DCC_LOG(LOG_TRACE, "17. shell_init()");

	spectrum_analyzer_init();

	shell_init();

	audio_enable();

	return ui_task();
}

