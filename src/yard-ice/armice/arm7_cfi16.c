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
 * @file .c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 
#include <stdlib.h>

#include "arm_opc.h"
#include "arm7ice.h"
#include "ice_drv.h"

#ifdef CFI_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif
#include <debug.h>

/**************************************************************
 * ARM Macros
 **************************************************************/

int arm7_cfi16_erase(const ice_drv_t * ice, uint32_t base, 
					uint32_t offs, int len)
{
	uint32_t data;
	uint32_t poll;
	int n;
	int i;

	DBG(DBG_TRACE, "base=0x%08x offs=0x%08x len=%d", base, offs, len);

	/* initialize some registers */
	arm7ice_step(1, ARM_LDM(((1 << ARM_R3) | (1 << ARM_R4) | 
								(1 << ARM_R5) | (1 << ARM_R6) | 
								(1 << ARM_R7) | (1 << ARM_R8) | 
								(1 << ARM_R9) ), ARM_SP), NULL);
	arm7ice_step(2, ARM_NOP, NULL);
	arm7ice_step(1, base + (0x555 << 1), NULL); /* r3 */
	arm7ice_step(1, base + (0x2aa << 1), NULL); /* r4 */
	arm7ice_step(1, 0xaa, NULL); /* r5 */
	arm7ice_step(1, 0x55, NULL); /* r6 */
	arm7ice_step(1, 0x80, NULL); /* r7 */
	arm7ice_step(1, 0x30, NULL); /* r8 */
	arm7ice_step(2, 0x0f, NULL); /* r9 */

	arm7ice_step(1, ARM_LDR(ARM_R2, ARM_SP, 0), NULL);
	arm7ice_step(2, ARM_NOP, NULL);

	offs &= ~0x000001;
	arm7ice_step(2, base + offs, NULL); /* r2 */
	arm7ice_step(1, ARM_NOP, NULL);

	for(n = len; n > 0; ) {
		/* read the memory content */
		if (((offs & 0x03) == 0) && (n >= 4)) {
			arm7ice_step(1, ARM_NOP, NULL);
			arm7ice_step_brk(1, ARM_NOP);
			arm7ice_step_sync(1, ARM_LDR(ARM_R1, ARM_R2, 4));
			arm7ice_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
			arm7ice_step(2, ARM_NOP, NULL);
			arm7ice_step(1, 0, &data);
			if (data == 0xffffffff) {
				offs += 4;
				n -= 4;
				continue;
			}
			arm7ice_step(1, ARM_SUB(ARM_R2, ARM_R2, 4), NULL);
		} else { 
			arm7ice_step(1, ARM_NOP, NULL);
			arm7ice_step_brk(1, ARM_NOP);
			arm7ice_step_sync(1, ARM_LDRH(ARM_R1, ARM_R2, 2));
			arm7ice_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
			arm7ice_step(2, ARM_NOP, NULL);
			arm7ice_step(1, 0, &data);
			if (data == 0xffff) {
				offs += 2;
				n -= 2;
				continue;
			}
			arm7ice_step(1, ARM_SUB(ARM_R2, ARM_R2, 2), NULL);
		}

		DBG(DBG_TRACE, "erasing: addr=0x%08x", base + offs);

		arm7ice_step(1, ARM_NOP, NULL);

		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R5, ARM_R3, 0));

		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R6, ARM_R4, 0));
		
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R7, ARM_R3, 0));

		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R5, ARM_R3, 0));

		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R6, ARM_R4, 0));

		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R8, ARM_R2, 0));

		for (i = 100; i > 0; i--) {
			arm7ice_step(1, ARM_NOP, NULL);
			arm7ice_step_brk(1, ARM_NOP);
			arm7ice_step_sync(1, ARM_LDRH(ARM_R1, ARM_R2, 0));

			arm7ice_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
			arm7ice_step(2, ARM_NOP, NULL);
			arm7ice_step(1, 0, &poll);
			if (poll & 0x0008)
				break;
		}

		if (i == 0) {
			DBG(DBG_WARNING, "erase fail: addr=0x%08x", base + offs);
			return -1;
		} else {
			DBG(DBG_TRACE, "erase command time left=%d!", i);
		}

		for (i = 40000; i > 0; i--) {
			if (poll & (0x0080 | 0x0020))
				break;
			arm7ice_step(1, ARM_NOP, NULL);

			arm7ice_step_brk(1, ARM_NOP);
			arm7ice_step_sync(1, ARM_LDRH(ARM_R1, ARM_R2, 0));

			arm7ice_step(1, ARM_STR(ARM_R1, ARM_SP, 0), NULL);
			arm7ice_step(2, ARM_NOP, NULL);
			arm7ice_step(1, 0, &poll);
		}

		if (!(poll & 0x0080) && (poll & 0x0020)) {
			DBG(DBG_WARNING, "erase algorithm timeout!");
			/* reset the chip */
			arm7ice_step_brk(1, ARM_NOP);
			arm7ice_step_sync(1, ARM_STRH(ARM_R9, ARM_R2, 0));
			return -2;
		}

		if (i == 0) {
			DBG(DBG_WARNING, "watchdog timeout!");
			/* reset the chip */
			arm7ice_step_brk(1, ARM_NOP);
			arm7ice_step_sync(1, ARM_STRH(ARM_R9, ARM_R2, 0));
			return -3;
		} 
		DBG(DBG_INFO, "poll time left=%d!", i);

		arm7ice_step(1, ARM_ADD(ARM_R2, ARM_R2, 2), NULL);
//		arm7ice_step(1, ARM_NOP, NULL);
		offs += 2;
		n -= 2;
	}

	return len;
}

int arm7_cfi16_write(const ice_drv_t * ice, uint32_t base, 
					uint32_t offs, const void * ptr, int len)
{
	uint32_t poll;
	uint16_t * wp;
	int n;
	uint32_t data;
	int m;
	int count;

	DBG(DBG_TRACE, "base=0x%08x offs=0x%08x len=%d", base, offs, len);
	
	/* truncate the address */
	wp = (uint16_t *)((uint32_t)ptr & ~0x000001);
	/* truncate the offset */
	offs &= ~0x000001;

	arm7ice_step(1, ARM_LDM(((1 << ARM_R2) | (1 << ARM_R3) | 
								(1 << ARM_R4) | (1 << ARM_R5) | 
								(1 << ARM_R6) | (1 << ARM_R7)), ARM_SP), NULL);
	arm7ice_step(2, ARM_NOP, NULL);
	arm7ice_step(1, base + offs, NULL); /* r2 */
	arm7ice_step(1, base + (0x555 << 1), NULL); /* r3 */
	arm7ice_step(1, base + (0x2aa << 1), NULL); /* r4 */
	arm7ice_step(1, 0xaa, NULL); /* r5 */
	arm7ice_step(1, 0x55, NULL); /* r6 */
	arm7ice_step(2, 0xa0, NULL); /* r7 */

	arm7ice_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
	arm7ice_step(2, ARM_NOP, NULL);

	m = (len + 1) >> 1;
	for (n = 0; n < m; n++) {
		data = wp[n]; 
		count = 4;

		/* data + ldr stuffing */
		arm7ice_step(2, data, NULL); /* r1 */
		arm7ice_step(4, ARM_NOP, NULL);
again:
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R5, ARM_R3, 0));

		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R6, ARM_R4, 0));

		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R7, ARM_R3, 0));

		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_STRH(ARM_R1, ARM_R2, 0));
		
		arm7ice_step_brk(1, ARM_NOP);
		arm7ice_step_sync(1, ARM_LDRH(ARM_R8, ARM_R2, 2));

		arm7ice_step(1, ARM_STR(ARM_R8, ARM_SP, 0), NULL);
		arm7ice_step(1, ARM_LDR(ARM_R1, ARM_SP, 0), NULL);
		arm7ice_step(1, ARM_MOV(ARM_PC, ARM_SP), NULL);
		arm7ice_step(1, 0, &poll);
		arm7ice_step(1, ARM_NOP, NULL);

		if (poll != data) {
			count--;
			if (count == 0)
				break;
			/* data + ldr stuffing */
			arm7ice_step(2, data, NULL); /* r1 */
			arm7ice_step(4, ARM_NOP, NULL);
			arm7ice_step(1, ARM_SUB(ARM_R2, ARM_R2, 2), NULL);
			
			goto again;
		}
	}
	arm7ice_step(5, ARM_NOP, NULL);

	if ((m << 1) >= len)
		return len;

	return n << 1;
}

