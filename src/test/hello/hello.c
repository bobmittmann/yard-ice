/* 
 * File:	 adc_test.c
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
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/delay.h>

#include <sys/dcclog.h>

void stdio_init(void);

int main(int argc, char ** argv)
{
	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");

	/* Calibrate the the uDelay loop */
	cm3_udelay_calibrate();

	DCC_LOG(LOG_TRACE, "2. stdio_init()");

	DCC_LOGSTR(LOG_TRACE, "\"%s\"", "Hello world!");

	/* Initialize the stdin, stdout and stderr */
	stdio_init();

	/* Print a useful information message */
	printf("\n");
	printf(" Hello world!\n");
	printf("\n");

	udelay(1000000);

	return 0;
}

