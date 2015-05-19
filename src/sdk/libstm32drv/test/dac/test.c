/* 
 * File:	 dac-test.c
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
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <thinkos.h>

#include <sys/dcclog.h>

#include <yard-ice/drv.h>
#include <yard-ice/audio.h>


int main(int argc, char ** argv)
{
	int c;
//	uint32_t now;
//	uint32_t prev;
//	uint32_t dt;

	DCC_LOG_CONNECT();
	DCC_LOG_INIT();

	cm3_udelay_calibrate();
	thinkos_init(THINKOS_OPT_PRIORITY(0) | THINKOS_OPT_ID(0));

	stdout = uart_console_open(115200, SERIAL_8N1);
	stdin = stdout;

	printf("\n");
	printf("------------------------------------------------------\n");
	printf("- DAC test\n");
	printf("------------------------------------------------------\n");
	printf("\n");
	printf("\r\n");

	stm32f_dac_init();

	for (;;) {
		c = getchar();
		printf("%c", c);
		switch (c) {
		case '1':
			tone_play(TONE_A4, 125);
			break;
		case '2':
			tone_play(TONE_A4S, 125);
			break;
		case '3':
			tone_play(TONE_B4, 125);
			break;
		case '4':
			tone_play(TONE_C4, 125);
			break;
		case '5':
			tone_play(TONE_C4S, 125);
			break;
		case '6':
			tone_play(TONE_D4, 125);
			break;
		case '7':
			tone_play(TONE_D4S, 125);
			break;
		case '8':
			tone_play(TONE_E4, 125);
			break;
		case '9':
			tone_play(TONE_F4, 125);
			break;
		case '0':
			tone_play(TONE_F4S, 125);
			break;
		case '-':
			tone_play(TONE_G4, 125);
			break;
		case '=':
			tone_play(TONE_G4S, 125);
			break;
		case 'a': {
			int i;

			for (i = 0; i < 5; i++) {
				tone_play(TONE_E4, 125);
				tone_play(TONE_A4S, 125);
			}
			continue;
		}
		}
	}

	return 0;
}

