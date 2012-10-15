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
 * @file .h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __TARGET_DAVINCI_H__
#define __TARGET_DAVINCI_H__

#include <stdint.h>
#include <target.h>
#include <target/arm.h>

#include "davinci/dm365.h"
#include "nand.h"

#define SYS_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_SYSTEM_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_SYSTEM_BASE + (OFFS), data | (VAL)); }

#define SYS_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_SYSTEM_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_SYSTEM_BASE + (OFFS), data & ~(VAL)); }

#define SYS_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_SYSTEM_BASE + (OFFS), &data); data; })

#define SYS_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_SYSTEM_BASE + (OFFS), (VAL));


#define PSC_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_PSC_BASE + (OFFS), &data); data; })

#define PSC_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_PSC_BASE + (OFFS), (VAL));



#define PLL1_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_PLL1_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_PLL1_BASE + (OFFS), data | (VAL)); }

#define PLL1_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_PLL1_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_PLL1_BASE + (OFFS), data & ~(VAL)); }

#define PLL1_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_PLL1_BASE + (OFFS), &data); data; })

#define PLL1_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_PLL1_BASE + (OFFS), (VAL));


#define PLL2_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_PLL2_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_PLL2_BASE + (OFFS), data | (VAL)); }

#define PLL2_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_PLL2_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_PLL2_BASE + (OFFS), data & ~(VAL)); }

#define PLL2_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_PLL2_BASE + (OFFS), &data); data; })

#define PLL2_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_PLL2_BASE + (OFFS), (VAL));


#define DDR_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_DDR_BASE + (OFFS), &data); data; })

#define DDR_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_DDR_BASE + (OFFS), (VAL));


#define AEMIF_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_AEMIF_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_AEMIF_BASE + (OFFS), data | (VAL)); }

#define AEMIF_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_AEMIF_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_AEMIF_BASE + (OFFS), data & ~(VAL)); }

#define AEMIF_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_AEMIF_BASE + (OFFS), &data); data; })

#define AEMIF_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_AEMIF_BASE + (OFFS), (VAL));


#define UART1_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_UART1_BASE + (OFFS), &data); data; })

#define UART1_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_UART1_BASE + (OFFS), (VAL));

#define UART0_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_UART0_BASE + (OFFS), &data); data; })

#define UART0_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_UART0_BASE + (OFFS), (VAL));

#define TIMER0_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_TIMER0_BASE + (OFFS), &data); data; })

#define TIMER0_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_TIMER0_BASE + (OFFS), (VAL));


#define I2C0_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_I2C0_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_I2C0_BASE + (OFFS), data | (VAL)); }

#define I2C0_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_I2C0_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_I2C0_BASE + (OFFS), data & ~(VAL)); }

#define I2C0_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_I2C0_BASE + (OFFS), &data); data; })

#define I2C0_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_I2C0_BASE + (OFFS), (VAL));


#define AINTC_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_AINTC_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_AINTC_BASE + (OFFS), data | (VAL)); }

#define AINTC_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_AINTC_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_AINTC_BASE + (OFFS), data & ~(VAL)); }

#define AINTC_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_AINTC_BASE + (OFFS), &data); data; })

#define AINTC_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_AINTC_BASE + (OFFS), (VAL));


#define GPIO_REG_SET(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_GPIO_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_GPIO_BASE + (OFFS), data | (VAL)); }

#define GPIO_REG_CLR(OFFS, VAL) { uint32_t data;\
	ice_rd32(ice, DM36X_GPIO_BASE + (OFFS), &data);\
	ice_wr32(ice, DM36X_GPIO_BASE + (OFFS), data & ~(VAL)); }

#define GPIO_REG_RD(OFFS) ({ uint32_t data;\
	ice_rd32(ice, DM36X_GPIO_BASE + (OFFS), &data); data; })

#define GPIO_REG_WR(OFFS, VAL) \
	ice_wr32(ice, DM36X_GPIO_BASE + (OFFS), (VAL));


extern const struct target_info ti_dm365;
extern const struct target_info ti_dm365hs;

/* 
 *  generic at91sam9 target 
 * This target is not constant to allow dynamic chages
 * when probing a device
 */
extern const struct target_info at91sam9;
extern const struct target_info at91sam9260;

extern const struct ice_mem_entry dm365_mem[]; 

extern const struct target_sym dm365_sym[];

extern const struct target_cpu davinci_cpu;

extern const struct armice_cfg davinci_cfg;

extern const struct nand_io_op dm365_nand_io8;
extern const struct nand_ecc_op dm365_nand_ecc;
extern const struct nand_bb_op dm365_nand_bb;

extern const struct ice_mem_oper dm365_nand_mem_oper;


#ifdef __cplusplus
extern "C" {
#endif

int davinci_on_init(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem);

int davinci_on_halt(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem);

int davinci_reset(FILE * f, const struct ice_drv * ice, 
				   ice_mem_entry_t * mem);

int davinci_probe(FILE * f, const struct ice_drv * ice, 
				  ice_mem_entry_t * mem);

/* write into PRTCSS register */
int davinci_prtcss_wr(const struct ice_drv * ice, int reg, 
					  uint8_t * data, int len);

/* Read from PRTCSS registers */
int davinci_prtcss_rd(const struct ice_drv * ice, int reg, 
					  uint8_t * data, int len);

int davinci_test(FILE * f, const struct ice_drv * ice, 
				 ice_mem_entry_t * mem, uint32_t val);

int ti_dm365_probe(FILE * f, const struct ice_drv * ice, 
					 ice_mem_entry_t * mem);

int dm365_on_init(FILE * f, const ice_drv_t * ice, ice_mem_entry_t * mem);


nand_dev_t * dm365_nand_mem_init(const ice_drv_t * ice);

#ifdef __cplusplus
	}
#endif

#endif /* __TARGET_DAVINCI_H__ */

