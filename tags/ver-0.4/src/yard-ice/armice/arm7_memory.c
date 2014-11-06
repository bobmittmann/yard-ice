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
 * @file arm7_memory.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdlib.h>
#include <string.h>

#include "arm_opc.h"
#include "arm7ice.h"
#include "ice_drv.h"

#ifdef MEM_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

#include <sys/dcclog.h>

int arm7_mem_rd(const ice_drv_t * ice, uint32_t base, 
			   uint32_t offs, void * p, int len)
{
	uint32_t addr;
	uint32_t data;
	unsigned char * cp;
	int n;

	DCC_LOG3(LOG_INFO, "base=0x%08x offs=0x%08x len=%d,", base, offs, len);

	if (len == 0)
		return 0;

	addr = base + offs;

	arm7ice_step(1, ARM_LDR(ARM_R12, ARM_SP, 0), NULL);
	arm7ice_step(2, ARM_NOP, NULL);
	arm7ice_step(2, addr, NULL);

	cp = (unsigned char *)p;
	n = 0;

	/* get the first bytes, if the address is not half-word (16bits) aligned */
	if(addr & 0x01) {
		/* run at system speed and increment the base register by one */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_LDRB(ARM_R1, ARM_R12, 1));

		arm7ice_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
		arm7ice_step(2, ARM_NOP, NULL);
		arm7ice_step(1, 0, &data);
		*cp++ = data;
		len--;
		addr++;
		n++;
	}
	/* get the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		/* run at system speed and increment the base register by two */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_LDRH(ARM_R1, ARM_R12, 2));
		arm7ice_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
		arm7ice_step(2, ARM_NOP, NULL);
		arm7ice_step(1, 0, &data);
		*cp++ = data;
		*cp++ = data >> 8;
		len -= 2;
		n += 2;
	}

	for (; len >= 4; len -= 4) {
		/* run at system speed and increment the base register by four */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_LDR(ARM_R1, ARM_R12, 4));
		arm7ice_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
		arm7ice_step(2, ARM_NOP, NULL);
		arm7ice_step(1, 0, &data);
		*cp++ = data;
		*cp++ = data >> 8;
		*cp++ = data >> 16;
		*cp++ = data >> 24;
		n += 4;
	}
	/* get the last word, if the address is not word (32bits) aligned */
	if (len & 0x02) {
		/* run at system speed and increment the base register by two */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_LDRH(ARM_R1, ARM_R12, 2));
		arm7ice_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
		arm7ice_step(2, ARM_NOP, NULL);
		arm7ice_step(1, 0, &data);
		*cp++ = data;
		*cp++ = data >> 8;
		n += 2;
	}
	/* get the last bytes, if the address is not half-word (16bits) aligned */
	if(len & 0x01) {
		/* run at system speed and increment the base register by one */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_LDRB(ARM_R1, ARM_R12, 1));
		arm7ice_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
		arm7ice_step(2, ARM_NOP, NULL);
		arm7ice_step(1, 0, &data);
		*cp = data;
		n ++;
	}

	return n;
}

int arm7_mem_wr(const ice_drv_t * ice, uint32_t base, 
			   uint32_t offs, const void * p, int len)
{
	uint32_t addr;
	uint32_t data;
	unsigned char * cp;
	int n;

	DCC_LOG3(LOG_INFO, "base=0x%08x offs=0x%08x len=%d,", base, offs, len);

	if (len == 0)
		return 0;

	addr = base + offs;
	n = 0;
	cp = (unsigned char *)p;

	arm7ice_step(1, ARM_LDR(ARM_R12, ARM_SP, 0), NULL);
	arm7ice_step(2, ARM_NOP, NULL);
	arm7ice_step(2, addr, NULL);

	/* get the first bytes, if the address is not half-word (16bits) aligned */
	if (addr & 0x01) {
		data = *cp++;
		arm7ice_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
		arm7ice_step(2, ARM_NOP, NULL);
		arm7ice_step(2, data, NULL);	
		/* run at system speed and increment the base register by one */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRB(ARM_R1, ARM_R12, 1));
		len--;
		addr++;
		n++;
	}
	/* get the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		data = *cp++;
		data += *cp++ << 8;
		arm7ice_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
		arm7ice_step(2, ARM_NOP, NULL);
		arm7ice_step(2, data, NULL);	
		/* run at system speed and increment the base register by two */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R1, ARM_R12, 2));
		len -= 2;
		n += 2;
	}
	while (len >= 4) {
		data = *cp++;
		data += *cp++ << 8;
		data += *cp++ << 16;
		data += *cp++ << 24;
		arm7ice_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
		arm7ice_step(2, ARM_NOP, NULL);
		arm7ice_step(2, data, NULL);
		/* run at system speed and increment the base register by four */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STR(ARM_R1, ARM_R12, 4));
		len -= 4;
		n += 4;
	}
	/* get the last word, if the address is not word (32bits) aligned */
	if (len & 0x02) {
		data = *cp++;
		data += *cp++ << 8;
		arm7ice_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
		arm7ice_step(2, ARM_NOP, NULL);
		arm7ice_step(2, data, NULL);	
		/* run at system speed and increment the base register by two */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R1, ARM_R12, 2));
		len -= 2;
		n += 2;
	}
	/* get the last bytes, if the address is not half-word (16bits) aligned */
	if (len) {
		data = *cp;
		arm7ice_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
		arm7ice_step(2, ARM_NOP, NULL);
		arm7ice_step(2, data, NULL);	
		/* run at system speed and increment the base register by one */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRB(ARM_R1, ARM_R12, 1));
		n++;
	}

	return n;
}

int arm7_mem_zero(const ice_drv_t * ice, uint32_t base, 
				 uint32_t offs, int len)
{
	uint32_t addr;
	uint32_t data;
	unsigned int val = 0;
	int n;

	DCC_LOG3(LOG_TRACE, "base=0x%08x offs=0x%08x len=%d,", base, offs, len);

	if (len == 0)
		return 0;

	addr = base + offs;
	val &= 0xff;
	data = val | (val << 8) | (val << 16) | (val << 24);
	n = 0;

	arm7ice_step(1, ARM_LDR(ARM_R12, ARM_SP, 0), NULL);
	arm7ice_step(2, ARM_NOP, NULL);
	arm7ice_step(2, addr, NULL);

	arm7ice_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
	arm7ice_step(2, ARM_NOP, NULL);
	arm7ice_step(2, data, NULL);	

	/* write the first bytes, if the address is not half-word (16bits) aligned */
	if (addr & 0x01) {
		/* run at system speed and increment the base register by one */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRB(ARM_R1, ARM_R12, 1));
		len--;
		addr++;
		n++;
	}
	/* write the next word, if the address is not word (32bits) aligned */
	if ((len >= 2) && (addr & 0x02)) {
		/* run at system speed and increment the base register by two */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R1, ARM_R12, 2));
		len -= 2;
		n += 2;
	}
	while (len >= 4) {
		/* run at system speed and increment the base register by four */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STR(ARM_R1, ARM_R12, 4));
		len -= 4;
		n += 2;
	}
	/* write the last word, if the address is not word (32bits) aligned */
	if (len & 0x02) {
		/* run at system speed and increment the base register by two */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R1, ARM_R12, 2));
		len -= 2;
		n += 2;
	}
	/* write the last bytes, if the address is not half-word (16bits) aligned */
	if (len) {
		/* run at system speed and increment the base register by one */
		arm7ice_step(1, ARM_NOP, NULL);
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRB(ARM_R1, ARM_R12, 1));
		n++;
	}

	return n;
}

