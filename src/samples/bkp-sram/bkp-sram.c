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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

FILE * stdout = (FILE *)&stm32f_usart5_file;

int bkp_init(void)
{
	struct stm32f_pwr * pwr = STM32F_PWR;
	struct stm32f_rcc * rcc = STM32F_RCC;

	/* ● Access to the backup SRAM */
	/* 1. Enable the power interface clock by setting the PWREN bits in 
	   the RCC APB1 peripheral clock enable register (RCC_APB1ENR) */
	printf(" - Enabling power interface clock...\n");
	rcc->apb1enr |= RCC_PWREN;

	/* 2. Set the DBP bit in the PWR power control register (PWR_CR) 
	   to enable access to the backup domain */
	printf(" - Enabling access to the backup domain...\n");
	pwr->cr |= PWR_DBP;

	/* 3. Enable the backup SRAM clock by setting BKPSRAMEN bit in the RCC 
	   AHB1 peripheral clock register (RCC_AHB1ENR) */
	printf(" - Enabling backup SRAM clock...\n");
	rcc->ahb1enr |= RCC_BKPSRAMEN;

	/* Enable the backup regulator */
	printf(" - Enabling the backup regulator...\n");
	pwr->csr |= PWR_BRE;

	while (!(pwr->csr & PWR_BRR)) {
	}
	printf(" - Backup regulator ready.\n");

	return 0;
}

bool bkp_sanity_check(void)
{
	struct stm32f_pwr * pwr = STM32F_PWR;
	struct stm32f_rcc * rcc = STM32F_RCC;

	if (!(pwr->csr & PWR_BRE)) {
		printf(" ! Backup regulator is off!\n");
		return false;
	};

	if (!(pwr->csr & PWR_BRR)) {
		printf(" ! Backup regulator is not ready!\n");
		return false;
	}

	if (!(rcc->ahb1enr & RCC_BKPSRAMEN)) {
		printf(" ! Backup SRAM clock disabled\n");
		return false;
	}

	return true;
}

void set_magic(void)
{
	char * bkp = (char *)STM32F_BKPSRAM;

	printf(" - Writing magic word...\n");
	strcpy(bkp, "ABRACADABRA");
}

bool is_magic(void)
{
	char * bkp = (char *)STM32F_BKPSRAM;

	return (strcmp(bkp, "ABRACADABRA") == 0) ? true : false;
}


int main(int argc, char ** argv)
{
	struct stm32f_usart * us = STM32F_USART5;

	stm32f_usart_open(us, 115200, SERIAL_8N1);
	cm3_udelay_calibrate();

	printf("\n");
	printf("--------------------------------------------\n");
	printf("Backup SRAM test\n");
	printf("--------------------------------------------\n");
	printf("\n");

	if (bkp_sanity_check() == false) {
		bkp_init();
		if (bkp_sanity_check() == false) {
			printf(" - Backup SRAM init failed!\n");
		}
	} else {
		printf(" - Backup SRAM OK.\n");
	}

	if (is_magic()) {
		printf(" - Magic word is valid.\n");
	} else {
		printf(" - Invalid magic word!!!\n");
		set_magic();
	}

	for (;;) {
		udelay(500000);
	}

	return 0;
}

