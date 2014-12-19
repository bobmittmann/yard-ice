/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 *
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file stm32f-rtc.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/dcclog.h>

int stm32f_rtc_init(void)
{
	struct stm32_rcc * rcc = STM32_RCC;
	struct stm32_pwr * pwr = STM32_PWR;
	struct stm32f_rtc * rtc = STM32F_RTC;
	uint32_t bdcr;

/* ● Access to the RTC and RTC backup registers */
/* 1. Enable the power interface clock by setting the PWREN bits in 
   the RCC APB1 peripheral clock enable register (RCC_APB1ENR) */
	DCC_LOG(LOG_INFO, " - Enabling power interface clock...");
	rcc->apb1enr |= RCC_PWREN;

/* 2. Set the DBP bit in the PWR power control register (PWR_CR) 
   to enable access to the backup domain */
	DCC_LOG(LOG_TRACE, " - Enabling access to the backup domain...");
	pwr->cr |= PWR_DBP;

	DCC_LOG(LOG_INFO, " - Reseting the backup domain!");
	rcc->bdcr = RCC_BDRST;
	bdcr = 0;

/* 3. Select the RTC clock source: see Section 5.2.8: RTC/AWU clock */
	DCC_LOG(LOG_INFO, " - Selecting RTC clock source...");
	bdcr |= RCC_RTCSEL_LSE;

/* 4. Enable the RTC clock by programming the RTCEN [15] bit in the RCC 
   Backup domain control register (RCC_BDCR) */
	DCC_LOG(LOG_INFO, " - Enabling RTC clock...");
	bdcr |= RCC_RTCEN;

	DCC_LOG(LOG_INFO, " - Enabling LSE...");
	bdcr |= RCC_LSEON;

	/* Updating BDCR */
	rcc->bdcr = bdcr;

	while (!((bdcr = rcc->bdcr) & RCC_LSERDY)) {
	}
	DCC_LOG(LOG_INFO, " - LSE clock ready.");

/* After power-on reset, all the RTC registers are write-protected. 
   Writing to the RTC registers is enabled by writing a key into the 
   Write Protection register, RTC_WPR.
   The following steps are required to unlock the write protection on all 
   the RTC registers except for RTC_ISR[13:8], RTC_TAFCR, and RTC_BKPxR.
   1. Write ‘0xCA’ into the RTC_WPR register.
   2. Write ‘0x53’ into the RTC_WPR register.
   Writing a wrong key reactivates the write protection.
   The protection mechanism is not affected by system reset. */
	DCC_LOG(LOG_INFO, " - Unlocking the write protection cell...");
	rtc->wpr = 0xca;
	rtc->wpr = 0x53;

	return 0;
}

void rtc_time_get(struct stm32f_rtc * rtc)
{
	uint32_t tr;
	int pm;
	int hour;
	int min;
	int sec;

	tr = rtc->tr;

	pm = tr & RTC_PM ? 1 : 0;
	hour = RTC_HT_GET(tr) * 10 + RTC_HU_GET(tr);
	min = RTC_MNT_GET(tr) * 10 + RTC_MNU_GET(tr);
	sec = RTC_ST_GET(tr) * 10 + RTC_SU_GET(tr);

	printf("%02d:%02d:%02d %s\n", hour, min, sec, pm ? "PM" : "AM");
}

#if 0
bool rtc_sanity_check(void)
{
	struct stm32f_rcc * rcc = STM32F_RCC;
	uint32_t bdcr;

	bdcr = rcc->bdcr;

	if (!(bdcr & RCC_LSERDY)) {
		printf(" - LSE not ready!\n");
		return false;
	}

	if (!(bdcr & RCC_LSEON)) {
		printf(" - LSE disabled!\n");
		return false;
	}

	if (!(bdcr & RCC_RTCEN)) {
		printf(" - RTC disabled!\n");
		return false;
	}

	if ((bdcr & RCC_RTCSEL_MSK) != RCC_RTCSEL_LSE) {
		printf(" - RTC oscillator != LSE!\n");
		return false;
	}

	return true;
}
int main(int argc, char ** argv)
{
	struct stm32f_usart * us = STM32F_USART5;
	struct stm32f_rtc * rtc = STM32F_RTC;


	stm32f_usart_open(us, 115200, SERIAL_8N1);
	cm3_udelay_calibrate();

	printf("\n");
	printf("--------------------------------------------\n");
	printf("RTC test\n");
	printf("--------------------------------------------\n");
	printf("\n");

	if (rtc_sanity_check() == false) {
		rtc_init();
		if (rtc_sanity_check() == false) {
			printf(" - RTC init failed!\n");
		}
	} else {
		printf(" - RTC OK.\n");
	}

	for (;;) {
		rtc_time_get(rtc);
		udelay(500000);
	}

	return 0;
}
#endif
