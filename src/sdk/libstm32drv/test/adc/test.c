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

/* supervisory ADC initialization */
void stm32f_adc_init(void);

int32_t supv_temperature_get(void);
int32_t supv_vin_get(void);
int32_t supv_vbat_get(void);

int main(int argc, char ** argv)
{
	int32_t vin;
	int32_t vbat;
	int32_t temp;

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

	stm32f_adc_init();

	for (;;) {
		thinkos_sleep(1000);
		vin = supv_vin_get();
		vbat = supv_vbat_get();
		temp = supv_temperature_get();
		printf(" Vin  = %2d.%03d[V]", vin / 1000, vin % 1000);
		printf(" Vbat = %2d.%03d[V]", vbat / 1000, vbat % 1000);
		printf(" Temp = %2d[dg.C]", temp);
	}

	return 0;
}

