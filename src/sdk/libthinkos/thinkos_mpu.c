/* 
 * thikos_core.c
 *
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the ThinkOS library.
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

_Pragma ("GCC optimize (\"Os\")")

#define __THINKOS_SYS__
#include <thinkos_sys.h>
#define __THINKOS_IRQ__
#include <thinkos_irq.h>
#define __THINKOS_DMON__
#include <thinkos_dmon.h>

#include <thinkos_except.h>

#include <stdio.h>
#include <string.h>

/* 

  */
#if THINKOS_ENABLE_MPU

#define STRONGLY_ORDERED MPU_RASR_TEX(0) 
#define SHARED_DEVICE    MPU_RASR_TEX(0) | MPU_RASR_B
#define WRITE_THROUGH    MPU_RASR_TEX(0) | MPU_RASR_C
#define WRITE_BACK       MPU_RASR_TEX(0) | MPU_RASR_C | MPU_RASR_B
#define NONCACHEABLE     MPU_RASR_TEX(0)
#define WRITE_ALLOCATE   MPU_RASR_TEX(1)
#define NONSHARED_DEVICE MPU_RASR_TEX(2)

#define SHAREABLE        MPU_RASR_S

#define DATA_ONLY        MPU_RASR_XN

#define NO_ACCESS        MPU_RASR_AP_NO_ACCESS
#define PRIV_RW          MPU_RASR_AP_PRIV_RW
#define USER_RO          MPU_RASR_AP_USER_RO
#define USER_RW          MPU_RASR_AP_USER_RW
#define PRIV_RO          MPU_RASR_AP_PRIV_RO
#define READ_ONLY        MPU_RASR_AP_READ_ONLY


/* Normal memory, Non-shareable, write-through */
#define M_FLASH         (WRITE_THROUGH)
/* Normal memory, Shareable, write-through */
#define M_SRAM          (WRITE_THROUGH | SHAREABLE)
#define M_CCM           (WRITE_THROUGH | SHAREABLE | DATA_ONLY)
/* Normal memory, Shareable, write-back, write-allocate */
#define M_EXTERN        (WRITE_BACK | SHAREABLE)
/* Device memory, Shareable */
#define M_PERIPHERAL    (SHARED_DEVICE | SHAREABLE | DATA_ONLY)  
/* System peripherals */
#define M_SYSTEM        (STRONGLY_ORDERED | SHAREABLE | DATA_ONLY)  

static void mpu_region_cfg(int region, uint32_t addr, uint32_t attr)
{
	struct cm3_mpu * mpu = CM3_MPU;
	/* Region Base Address Register */
	uint32_t rbar;
	/* Region Attribute and Size Register */
	uint32_t rasr;
	
	rbar = MPU_RBAR_ADDR(addr) | MPU_RBAR_VALID | MPU_RBAR_REGION(region);
	rasr = attr; 
	DCC_LOG3(LOG_TRACE, "region=%d rbar=%08x rasr=%08x", region, rbar, rasr);
	/* Region Base Address Register */
	mpu->rbar = rbar;
	/* Region Attribute and Size Register */
	mpu->rasr = rasr;
}

void thinkos_mpu_init(unsigned int size)
{
	struct cm3_mpu * mpu = CM3_MPU;
	uint32_t bmp;
	unsigned int n;

	DCC_LOG(LOG_TRACE, "configuring MPU ...");

	for (n = 0; (n * 1024) < size; ++n);

	bmp = 0xffff >> (16 - n);

	mpu_region_cfg(0, 0, 0); 

	/* SRAM */
	/* 8 * 1K blocks low */
	mpu_region_cfg(1, 0x20000000, 
				   M_SRAM | USER_RW | 
				   MPU_RASR_SIZE_8K | 
				   MPU_RASR_SRD(bmp & 0xff) |
				   MPU_RASR_ENABLE);

	/* 8 * 1K blocks high*/
	mpu_region_cfg(2, 0x20000000 + 8 * 1024, 
				   M_SRAM | USER_RW | 
				   MPU_RASR_SIZE_8K | 
				   MPU_RASR_SRD(bmp >> 8) |
				   MPU_RASR_ENABLE);

	/* SRAM */
	mpu_region_cfg(3, 0x20000000, 
				   M_SRAM | USER_RW | 
				   MPU_RASR_SIZE_128K | 
				   MPU_RASR_SRD(0x01) |
				   MPU_RASR_ENABLE);

#if 0
	/* SRAM and Bitbanding */
	mpu_region_cfg(3, 0x20000000, 
				   M_SRAM | USER_RW | 
				   MPU_RASR_SIZE_64M | 
				   MPU_RASR_SRD(0x0e) |
				   MPU_RASR_ENABLE);
#endif
	/* FLASH */
	mpu_region_cfg(4, 0x08000000, 
				   M_FLASH | USER_RW | 
				   MPU_RASR_SIZE_512K | 
				   MPU_RASR_ENABLE);

	/* CCM */
	mpu_region_cfg(5, 0x10000000, 
				   M_CCM | USER_RW | 
				   MPU_RASR_SIZE_64K | 
				   MPU_RASR_ENABLE);
	/* External memory */
	mpu_region_cfg(6, 0x60000000, 
				   M_EXTERN | USER_RW | 
				   MPU_RASR_SIZE_512M | 
				   MPU_RASR_ENABLE);
	/* Peripheral */
	mpu_region_cfg(7, 0x40000000, 
				   M_PERIPHERAL | USER_RW | 
				   MPU_RASR_SIZE_512M | 
				   MPU_RASR_ENABLE);
	/* System */
/*	mpu_region_cfg(7, 0xe0000000, 
				   M_SYSTEM | USER_RW | 
				   MPU_RASR_SIZE_1M | 
				   MPU_RASR_ENABLE);
*/

	/* Enable MPU with no background mapping */
//	mpu->ctrl = MPU_CTRL_ENABLE;
	/* Enable MPU with background mapping */
	mpu->ctrl = MPU_CTRL_PRIVDEFENA | MPU_CTRL_ENABLE;
	/* Control Register */
//	mpu->ctrl = MPU_CTRL_PRIVDEFENA | MPU_CTRL_HFNMIENA | MPU_CTRL_ENABLE;

	__mpudump();
}

void thinkos_userland(void)
{
	cm3_control_set(CONTROL_THREAD_PSP | CONTROL_THREAD_USER);
}

#endif /* THINKOS_ENABLE_MPU */

