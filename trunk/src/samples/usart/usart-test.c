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
#include <sys/delay.h>
#include <sys/serial.h>

FILE * stdout = (FILE *)&stm32f_usart5_file;

int main(int argc, char ** argv)
{
	struct stm32f_usart * us = STM32F_USART5;

	stm32f_usart_open(us, 115200, SERIAL_8N1);
	cm3_udelay_calibrate();
	cm3_systick_init();

	cm3_except_pri_set(CM3_EXCEPT_PENDSV, 0);
	cm3_except_pri_set(CM3_EXCEPT_SVC, 32);
	cm3_except_pri_set(CM3_EXCEPT_SYSTICK, 64);
	
	cm3_basepri_set(64 + 32);

	printf("\r\n");
	printf("PendSV Test\r\n");
	printf("\r\n");

	for (;;) {
		udelay(1000000);
		__SVC(1);
	}

	return 0;
}


int main(int argc, char ** argv)
{
	struct stm32f_usart * us = STM32F_USART5;

	stm32f_usart_open(us, 115200);
	cm3_udelay_calibrate();

	printf("\n");
	printf("PendSV Test\r\n");
	printf("\r\n");

	return 0;
}

