/* 
 * File:	stm32f-bkp_sram.c
 * Module:  
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:	
 * Comment: 
 * Copyright(c) 2008-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#if CONFIG_H
#include "config.h"
#endif

#include <sys/stm32f.h>

void stm32f_bkp_sram_init(void)
{
	struct stm32f_pwr * pwr = STM32F_PWR;
	struct stm32f_rcc * rcc = STM32F_RCC;
	uint32_t apb1enr;

	/* ● Access to the backup SRAM */
	/* 1. Enable the power interface clock by setting the PWREN bits in 
	   the RCC APB1 peripheral clock enable register (RCC_APB1ENR) */
	apb1enr = rcc->apb1enr;
	rcc->apb1enr = apb1enr | RCC_PWREN;

	/* 2. Set the DBP bit in the PWR power control register (PWR_CR) 
	   to enable access to the backup domain */
	pwr->cr |= PWR_DBP;

	/* 3. Enable the backup SRAM clock by setting BKPSRAMEN bit in the RCC 
	   AHB1 peripheral clock register (RCC_AHB1ENR) */
	rcc->ahb1enr |= RCC_BKPSRAMEN;

	/* 4. Enable the backup regulator */
	pwr->csr |= PWR_BRE;

	/* 5. Wait until the regulator become ready */
	while (!(pwr->csr & PWR_BRR)) {
	}

	rcc->apb1enr = apb1enr;
}

