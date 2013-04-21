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
 * @file jtag_adi.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */


#ifndef __JTAG_ADI_H__
#define __JTAG_ADI_H__

#include "jtag.h"

#define	JTAG_ADI_IR_EXTEST 0x00
#define JTAG_ADI_IR_INTEST 0x01
#define JTAG_ADI_IR_SAMPLE_PREALOAD 0x02
#define JTAG_ADI_IR_ABORT 0x08
#define JTAG_ADI_IR_DPACC 0x0a
#define JTAG_ADI_IR_APACC 0x0b
#define JTAG_ADI_IR_IDCODE 0x0e
#define JTAG_ADI_IR_BYPASS 0x0f

#define JTAG_ADI_ACK_OK_FAULT 0x02
#define JTAG_ADI_ACK_WAIT 0x01

/*****************************************************************************/
/* JTAG-DP (Debug Port)                                                      */
/*****************************************************************************/

/*****************************************************************/
/* DP Registers                                                  */
/*****************************************************************/
#define ADI_DP_CTRL_STAT 0x04
#define ADI_DP_SELECT 0x08
#define ADI_DP_RDBUFF 0x0c

/* DP SELECT fields */
#define DP_APBANKSEL(SELECT) ((SELECT) & 0x000000f0)
#define DP_APSEL(SELECT) ((SELECT) & 0xff000000)

/*****************************************************************/
/* DP Ctrl/Stat Register                                          */
/*****************************************************************/
#define DP_ORUNDETECT (1 << 0)
#define DP_STICKYORUN (1 << 1)
#define DP_TRNMODE (3 << 2)
#define DP_STICKYCMP (1 << 4)
#define DP_STICKYERR (1 << 5)
#define DP_READOK (1 << 6)
#define DP_WDATAERR (1 << 7)
#define DP_MASKLANE (0x00f << 8)
#define DP_TRNCNT (0x3ff << 12)

#define DP_CSYSPWRUPACK (1 << 31)
#define DP_CSYSPWRUPREQ (1 << 30)
#define DP_CDBGPWRUPACK (1 << 29)
#define DP_CDBGPWRUPREQ (1 << 28)
#define DP_CDBGRSTACK (1 << 27)
#define DP_CDBGRSTREQ (1 << 26)

/*****************************************************************************/
/* MEM-AP (Memory Access Port)                                               */
/*****************************************************************************/



/*****************************************************************/
/* MEM-AP Registers                                              */
/*****************************************************************/

#define ADI_AP_CSW  0x00 /* Control/Status Word */
#define ADI_AP_TAR  0x04 /* Transfer Address */
#define ADI_AP_DRW  0x0c /* Data Read/Write */

#define ADI_AP_BD0  0x10 /* Banked Data 0 */
#define ADI_AP_BD1  0x14 /* Banked Data 1 */
#define ADI_AP_BD2  0x18 /* Banked Data 2 */
#define ADI_AP_BD3  0x1c /* Banked Data 3 */

#define ADI_AP_CFG  0xf4 /* Configuration Register */
#define ADI_AP_BASE 0xf8 /* Debug Base Address */
#define ADI_AP_IDR  0xfc /* Identification Register */

/*****************************************************************/
/* MEM-AP Control/Status Word Register (CSW)                     */
/*****************************************************************/
#define CSW_DBGSW_ENABLE (1 << 31)
#define CSW_MASTER_TYPE (1 << 29) /* MasterType - 
									 0 = core
									 1 debug */
#define CSW_PROT (0x7f << 24)
#define CSW_SPID_EN (1 << 23)
#define CSW_MODE (0x0f << 8)
#define CSW_TR_IN_PROG (1 << 7)
#define CSW_DEVICE_EN (1 << 6)

/* Auto-increment of the TAR */
#define CSW_ADDR_INC (0x03 << 4)
#define CSW_INC_OFF (0x00 << 4)
#define CSW_INC_SINGLE (0x01 << 4)
#define CSW_INC_PACKED (0x02 << 4)

/* Access size for memory access */
#define CSW_SIZE (0x07 << 0)
#define CSW_BYTE (0x00 << 0)
#define CSW_HALFWORD (0x01 << 0)
#define CSW_WORD (0x02 << 0)

/*------------------------------------*/
/* AHB-AP CSW bit assignments        */
/*------------------------------------*/
#define CSW_SPROT (1 << 30) /* (R/W) SProt Specifies that a secure transfer 
	is requested. SProt HIGH indicates a non-secure transfer. SProt LOW 
	indicates a secure transfer.
	• If this bit is LOW, and SPIDEN is HIGH, HPROT[6] is asserted LOW on an
	AHB transfer.
	• If this bit is LOW, and SPIDEN is LOW, HPROT[6] is asserted HIGH and the
	AHB transfer is not initiated.
	• If this bit is HIGH, the state of SPIDEN is ignored. HPROT[6] is HIGH.
*/

#define CSW_HPROT1 (1 << 25) /* Hprot1 - 
   User and Privilege control = HPROT[1] */

/* [28:24] R/W Prot Specifies the protection signal encoding to be output on HPROT[4:0]. Reset value: non-secure, non-exclusive, noncacheable, non-bufferable, data access, privileged = b00011. */

#define CSW_SPISTATUS (1 << 23) /* (RO) SPIStatus Indicates the status of the 
	SPIDEN port. If SPIStatus is LOW, no secure AHB transfers are carried out.
	*/

#define CSW_TRINPROG  (1 << 7) /* (RO) Transfer in progress. This field 
	indicates if a transfer is currently in progress on the AHB
   	master port. */

#define CSW_DBG_STATUS (1 << 6) /* (RO) DbgStatus - Indicates the status of the 
	DBGEN port. If DbgStatus is LOW, no AHB transfers are carried out.
	1 = AHB transfers permitted
	0 = AHB transfers not permitted */


/*****************************************************************/
/* MEM-AP Configuration Register (CFG)                           */
/*****************************************************************/
#define CFG_BIG_ENDIAN (1 << 0)

/*****************************************************************/
/* MEM-AP Debug Base Address Register (BASE)                     */
/*****************************************************************/
#define BASE_ADDR(BASE) ((BASE) & 0xfffff000)
#define BASE_FORMAT_ADIV5(BASE) (((BASE)>> 1) & 0x1)
#define BASE_ENTRY_PRESENT(BASE) (((BASE)>> 0) & 0x1)

/*****************************************************************/
/* MEM-AP Identification Register (IDR)                           */
/*****************************************************************/
#define IDR_REVISION(IDR) (((IDR) >> 28) & 0xf)
#define IDR_JEP106(IDR) (((IDR) >> 17) & 0x7ff)
#define IDR_CLASS(IDR) (((IDR) >> 16) & 0x1)
#define IDR_AP_VARIANT(IDR) (((IDR) >> 4) & 0xf)
#define IDR_AP_TYPE(IDR) (((IDR) >> 0) & 0xf)

/*****************************************************************/
/* Component and Peripheral ID Registers                         */
/*****************************************************************/

#define REG_PERIPHERAL_ID4 0xfd0
#define REG_PERIPHERAL_ID5 0xfd4
#define REG_PERIPHERAL_ID6 0xfd8
#define REG_PERIPHERAL_ID7 0xfdc
#define REG_PERIPHERAL_ID0 0xfe0
#define REG_PERIPHERAL_ID1 0xfe4
#define REG_PERIPHERAL_ID2 0xfe8
#define REG_PERIPHERAL_ID3 0xfec

#define REG_COMPONENT_ID0 0xff0
#define REG_COMPONENT_ID1 0xff4
#define REG_COMPONENT_ID2 0xff8
#define REG_COMPONENT_ID3 0xffc

/* Component ID register fields */
#define ID_SIGNATURE_OK 0xb105000d
#define ID_SIGNATURE_MASK 0xffff0fff
#define ID_CLASS(ID) (((ID) >> 12) & 0x0f)

/* Component Class values in the Component ID */
#define CLASS_ROM_TABLE 0x1
#define CLASS_DEBUG 0x9
#define CLASS_GENERIC_IP 0xe
#define CLASS_GENERIC_VERIFICATION 0x0

/* Peripheral ID register fields */
#define JEP106_CODE_USED(ID) (((ID) >> 19) & 0x1)
#define JEP106_ID_CODE(ID) (((ID) >> 12) & 0x7f)
#define JEP106_ID_CONTINUATION_CODE(ID) (((ID) >> 32) & 0x0f)
#define BLOCKS_4KB(ID) (1 << (((ID) >> 36) & 0x0f))
#define REV_AND(ID) (((ID) >> 28) & 0x0f)
#define CUSTOMER_MODIFIED(ID) (((ID) >> 24) & 0x0f)
#define REVISION(ID) (((ID) >> 20) & 0x0f)
#define PART_NUMBER(ID) (((ID) >> 0) & 0xfff)

/*****************************************************************/
/* ROM Table                                                     */
/*****************************************************************/

/* MEMTYPE register */
#define REG_MEMTYPE 0xfcc
#define SYSTEM_MEMORY_PRESENT(MEMTYPE) (((MEMTYPE) >> 0) & 1)

/* ROM Table entry fields */
#define ENTRY_PRESENT(ENTRY) (((ENTRY) >> 0) & 1)
#define FORMAT_32_BIT(ENTRY) (((ENTRY) >> 1) & 1)
#define ADDRESS_OFFSET(ENTRY) ((ENTRY) & 0xfffff000)

#ifdef __cplusplus
extern "C" {
#endif

/* low level JTAG operations */
int jtag_adi_tap_setup(jtag_tap_t * tap);

int jtag_adi_dpacc_rd(jtag_tap_t * tap, uint32_t addr, 
					  uint32_t din, uint32_t * dout);

int jtag_adi_dpacc_wr(jtag_tap_t * tap, uint32_t addr, 
					  uint32_t din, uint32_t * dout);

int jtag_adi_apacc_rd(jtag_tap_t * tap, uint32_t addr, 
					  uint32_t din, uint32_t * dout);

int jtag_adi_apacc_wr(jtag_tap_t * tap, uint32_t addr, 
					  uint32_t din, uint32_t * dout);

int jtag_adi_abort(jtag_tap_t * tap);

/* JTAG-DP operations */

int jtag_dp_ctrl_set(jtag_tap_t * tap, uint32_t bitmap);

int jtag_dp_ctrl_clr(jtag_tap_t * tap, uint32_t bitmap);

uint32_t jtag_dp_ctrl_stat_get(jtag_tap_t * tap);

int jtag_dp_dbg_pwr_up(jtag_tap_t * tap);

int jtag_dp_sys_pwr_up(jtag_tap_t * tap);

int jtag_dp_dbg_pwr_down(jtag_tap_t * tap);

int jtag_dp_sys_pwr_down(jtag_tap_t * tap);

int jtag_dp_dbg_reset(jtag_tap_t * tap);

int jtag_dp_init(jtag_tap_t * tap);


/* MEM-AP operations */

int jtag_mem_ap_init(jtag_tap_t * tap);

/* Memory Access Port Register Read */
int jtag_mem_ap_reg_rd(jtag_tap_t * tap, uint32_t addr, uint32_t * dout);

int jtag_mem_ap_reg_wr(jtag_tap_t * tap, uint32_t addr, uint32_t din);


int jtag_mem_ap_read(jtag_tap_t * tap, uint32_t addr, void * buf, int len);

int jtag_mem_ap_write(jtag_tap_t * tap, uint32_t addr, const void * buf, 
					  int len);

int jtag_mem_ap_set(jtag_tap_t * tap, uint32_t addr, uint8_t val, int len);

int jtag_mem_ap_rd8(jtag_tap_t * tap, uint32_t addr, uint8_t * dout);

int jtag_mem_ap_wr8(jtag_tap_t * tap, uint32_t addr, uint8_t data);

int jtag_mem_ap_rd16(jtag_tap_t * tap, uint32_t addr, uint16_t * dout);

int jtag_mem_ap_wr16(jtag_tap_t * tap, uint32_t addr, uint16_t data);

int jtag_mem_ap_rd32(jtag_tap_t * tap, uint32_t addr, uint32_t * dout);

int jtag_mem_ap_wr32(jtag_tap_t * tap, uint32_t addr, uint32_t data);

/* Memory Access Port high level functions */
int jtag_mem_ap_component_id_rd(jtag_tap_t * tap, uint32_t addr, uint32_t * id);

#ifdef __cplusplus
	}
#endif

#endif /* __JTAG_ADI_H__ */

