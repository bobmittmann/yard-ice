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
 * @file stm32f-bkp_sram.c
 * @brief Baterry Backed Up SRAM
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 


#if CONFIG_H
#include "config.h"
#endif

#include <sys/stm32f.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <crc.h>

#include <sys/dcclog.h>

#if defined(STM32F2X) || defined(STM32F4X)

void stm32f_bkp_sram_init(void)
{
	struct stm32_pwr * pwr = STM32_PWR;
	struct stm32_rcc * rcc = STM32_RCC;
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

#define ENV_BLK_ADDR ((uint32_t)STM32F_BKPSRAM + 0)
#define ENV_BLK_SIZE (4096 - 4)

struct stm32f_env_blk {
	uint16_t crc;
	uint16_t res;
	uint32_t data[ENV_BLK_SIZE / sizeof(uint32_t)];
};

int stm32f_bkp_sram_env_clean(void)
{
	struct stm32f_env_blk * blk = (struct stm32f_env_blk *)ENV_BLK_ADDR;
	int i;

	DCC_LOG(LOG_TRACE, "zeroing...");

	/* clear the memory block */
	for (i = 0; i < (ENV_BLK_SIZE / sizeof(uint32_t)); ++i) {
		blk->data[i] = 0;
	}

	blk->crc = 0;

	DCC_LOG(LOG_TRACE, "done.");

	return 0;
}

bool stm32f_bkp_sram_env_check(void)
{
	struct stm32f_env_blk * blk = (struct stm32f_env_blk *)ENV_BLK_ADDR;
	uint16_t crc;

	crc = crc16ccitt(0, blk->data, ENV_BLK_SIZE);

	DCC_LOG1(LOG_TRACE, "crc=%04x.", crc);

	return (crc == blk->crc) ? true : false;
}

int stm32f_bkp_sram_env_scan(char * env[], int max)
{
	struct stm32f_env_blk * blk = (struct stm32f_env_blk *)ENV_BLK_ADDR;
	char * cp;
	char * end;
	int c;
	int n;

	DCC_LOG(LOG_TRACE, "1.");

	cp = (char *)&blk->data;
	end = cp + ENV_BLK_SIZE;

	n = 0;

	do {
		/* Check for double nulls. If the first block's char is null
		   it will return from here as well */
		if ((c = *cp) == '\0')
			break;

		env[n++] = cp++;

		while (*cp != '\0') {
			if (cp == end) {
				env[n] = NULL;
				return n;
			}
			cp++;
		}
		cp++;
	} while ((cp != end) && (n < max));

	DCC_LOG1(LOG_TRACE, "n=%d", n);

	env[n] = NULL;
	return n;
}

int stm32f_bkp_sram_env_remove(char * var)
{
	struct stm32f_env_blk * blk = (struct stm32f_env_blk *)ENV_BLK_ADDR;
	int offs;
	int len;
	char * dst = var;
	char * src;
	char * end;

	end = (char *)&blk->data + ENV_BLK_SIZE;
	offs = strlen(var) + 1; 
	len = end - dst - offs;
	src = dst + offs;
	/* move all strings to the top */
	memcpy(dst, src, len);
	memset(dst + len, '\0', offs);

	/* update CRC */
	blk->crc = crc16ccitt(0, blk->data, ENV_BLK_SIZE);

	return 0;
}

char * stm32f_bkp_sram_env_insert(char * var, const char * name, 
								  const char * val)
{
	struct stm32f_env_blk * blk = (struct stm32f_env_blk *)ENV_BLK_ADDR;
	char * cp = var;
	char * end;
	int len;

	if (cp == NULL)
		cp = (char *)blk->data;

	/* search for the last variable */
	while (*cp != '\0') {
		DCC_LOG1(LOG_TRACE, "cp=%0p", cp);
		cp += strlen(cp) + 1;
	};

	/* get the end of the block pointer */
	end = (char *)&blk->data + ENV_BLK_SIZE;
	/* lenght of the insertion including the null termination */
	len = strlen(name) + 1 + strlen(val) + 1;	

	if ((cp + len) > end) {
		DCC_LOG(LOG_WARNING, "No room left on device!");
		return NULL;
	}

	var = cp;

	cp += sprintf(var, "%s=%s", name, val);

//	printf("'%s' %s %s\n", var, name, val);

	/* make suer we end with double zeroes */
	cp[1] = '\0';

	/* update CRC */
	blk->crc = crc16ccitt(0, blk->data, ENV_BLK_SIZE);

	return var;
}


#ifndef ENVIRON_MAX 
#define ENVIRON_MAX 32
#endif

extern char ** environ;
char * env_tab[ENVIRON_MAX + 1];

void stm32f_nvram_env_init(void)
{
	DCC_LOG(LOG_TRACE, "...");

	stm32f_bkp_sram_init();
	if (!stm32f_bkp_sram_env_check())
		stm32f_bkp_sram_env_clean();
	stm32f_bkp_sram_env_scan(env_tab, ENVIRON_MAX);

	environ = env_tab;
}

int setenv(const char *name, const char * value, int overwrite)
{
	int nm_len;
	char * var;
	char * cp;
	int i;

	if ((name == NULL) || (value == NULL) || ((nm_len = strlen(name)) == 0)) {
		DCC_LOG(LOG_WARNING, "Invalid argument!");
		return -1;
	}

	/* check whether it is already defined or not */
	i = 0;
	var = NULL;
	while ((cp = environ[i]) != NULL) {
		var = cp;
		if ((strncmp(var, name, nm_len) == 0) && 
			(var[nm_len] == '=')) {
			if (!overwrite) {
				DCC_LOG(LOG_WARNING, "Variable already exists!");
				return -1;
			}
			DCC_LOG2(LOG_TRACE, "removing @ %0p, i=%d", var, i);
			stm32f_bkp_sram_env_remove(var);
			break;
		}
		i++;
	}

	if (i == ENVIRON_MAX) {
		DCC_LOG(LOG_WARNING, "Too many variables!");
		return -1;
	}

	/* FIXME: */
	var = NULL;
	DCC_LOG2(LOG_TRACE, "inserting @ %0p, i=%d", var, i);
	
	var = stm32f_bkp_sram_env_insert(var, name, value);

	stm32f_bkp_sram_env_scan(environ, ENVIRON_MAX);

	if (var == NULL)
		return -1;

	return 0;
}

char * getenv(const char * name)
{
	int nm_len;
	char * var;
	int i;

	if ((name == NULL) || ((nm_len = strlen(name)) == 0)) {
		return NULL;
	}

	/* check whether it is already defined or not */
	i = 0;
	while ((var = environ[i]) != NULL) {
		if ((strncmp(var, name, nm_len) == 0) && 
			(var[nm_len] == '=')) {
			return &var[nm_len + 1];
		}
		i++;
	}

	return NULL;
}

int unsetenv(const char *name)
{
	int nm_len;
	char * var;
	char * cp;
	int i;

	if ((name == NULL) || ((nm_len = strlen(name)) == 0)) {
		DCC_LOG(LOG_WARNING, "Invalid argument!");
		return -1;
	}

	/* check whether it is defined or not */
	i = 0;
	var = NULL;
	while ((cp = environ[i]) != NULL) {
		var = cp;
		if ((strncmp(var, name, nm_len) == 0) && 
			(var[nm_len] == '=')) {
			DCC_LOG2(LOG_TRACE, "removing @ %0p, i=%d", var, i);
			stm32f_bkp_sram_env_remove(var);
			break;
		}
		i++;
	}

	stm32f_bkp_sram_env_scan(environ, ENVIRON_MAX);
	
	return 0;
}

int clearenv(void)
{
	stm32f_bkp_sram_env_clean();
	environ[0] = NULL;

	return 0;
}

#endif /* defined(STM32F2X) || defined(STM32F4X) */

