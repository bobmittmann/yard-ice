/*
 * File:	arch/cortex-m3.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:	cortex-m3
 * Comment:	Cortex M3 macros and definitions 
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


#ifndef __CORTEX_M3_H__
#define __CORTEX_M3_H__


/****************************************************************************
  CM3 SysTick 
 ****************************************************************************/

/* SysTick Control / Status Register */
#define SYSTICK_CTRL_COUNTFLAG  (0x01 << 16)
#define SYSTICK_CTRL_CLKSOURCE  (0x01 << 2)
#define SYSTICK_CTRL_TICKINT    (0x01 << 1)
#define SYSTICK_CTRL_ENABLE     (0x01 << 0)

/* SysTick Reload Register */
#define SYSTICK_LOAD_RELOAD (0x00ffffff)

/* SysTick Current Register */
#define SYSTICK_VAL_CURRENT (0x00ffffff)

/* SysTick Calibration Register */
#define SYSTICK_CALIB_NOREF (0x01 << 31)
#define SYSTICK_CALIB_SKEW (0x01 << 30)
#define SYSTICK_CALIB_TENMS(CALIB) ((CALIB) & 0x00ffffff)

/****************************************************************************
  CM3 SCB
 ****************************************************************************/

#define SCB_CPUID   0x00 /* SCB CPUID Register */
#define SCB_ICSR    0x04 /* SCB Interrupt Control State Register */
#define SCB_VTOR    0x08 /* SCB Vector Table Offset Register */
#define SCB_AIRCR   0x0c /* SCB Application Interrupt/Reset Control Register */
#define SCB_SCR     0x10 /* System Control Register */
#define SCB_CCR     0x14
#define SCB_SHPR1   0x18
#define SCB_SHPR2   0x1c
#define SCB_SHPR3   0x20
#define SCB_SHCSR   0x24
#define SCB_CFSR    0x28
#define SCB_HFSR    0x2c
#define SCB_DFSR    0x30
#define SCB_MMFAR   0x34
#define SCB_BFAR    0x38
#define SCB_AFSR    0x3c
#define SCB_CPACR   0x88

/* SCB CPUID Register */
#define SCB_CPUID_IMPLEMENTER (0xff << 24)
#define SCB_CPUID_VARIANT (0xf << 20)
#define SCB_CPUID_PARTNO (0xfff << 4)
#define SCB_CPUID_REVISION (0xf << 0)

/* SCB Interrupt Control State Register */
#define SCB_ICSR_NMIPENDSET (1 << 31)
#define SCB_ICSR_PENDSVSET (1 << 28)
#define SCB_ICSR_PENDSVCLR (1 << 27)
#define SCB_ICSR_PENDSTSET (1 << 26)
#define SCB_ICSR_PENDSTCLR (1 << 25)
#define SCB_ICSR_ISRPREEMPT (1 << 23)
#define SCB_ICSR_ISRPENDING (1 << 22)
#define SCB_ICSR_VECTPENDING (0x1ff << 12)
#define SCB_ICSR_RETTOBASE (1 << 11)
#define SCB_ICSR_VECTACTIVE (0x1ff << 0)

/* SCB Interrupt Control State Register Definitions */
#define SCB_VTOR_TBLBASE (0x7 << 29)
#define SCB_VTOR_TBLOFF (0x3fffff << 7)

/* SCB Application Interrupt and Reset Control Register */
#define SCB_AIRCR_VECTKEY (0xffff << 16)
#define SCB_AIRCR_VECTKEYSTAT (0xffff << 16)
#define SCB_AIRCR_ENDIANESS (1 << 15)
#define SCB_AIRCR_PRIGROUP (7 << 8)
#define SCB_AIRCR_SYSRESETREQ (1 << 2)
#define SCB_AIRCR_VECTCLRACTIVE (1 << 1)
#define SCB_AIRCR_VECTRESET (1 << 0)

/* SCB System Control Register */
#define SCB_SCR_SEVONPEND (1 << 4)
#define SCB_SCR_SLEEPDEEP (1 << 2)
#define SCB_SCR_SLEEPONEXIT (1 << 1)

/* SCB Configuration Control Register */
#define SCB_CCR_STKALIGN (1 << 9)
#define SCB_CCR_BFHFNMIGN (1 << 8)
#define SCB_CCR_DIV_0_TRP (1 << 4)
#define SCB_CCR_UNALIGN_TRP (1 << 3)
#define SCB_CCR_USERSETMPEND (1 << 1)
#define SCB_CCR_NONBASETHRDENA (1 << 0)

/* SCB System Handler Control and State Register */
#define SCB_SHCSR_USGFAULTENA (1 << 18)
#define SCB_SHCSR_BUSFAULTENA (1 << 17)
#define SCB_SHCSR_MEMFAULTENA (1 << 16)
#define SCB_SHCSR_SVCALLPENDED (1 << 15)
#define SCB_SHCSR_BUSFAULTPENDED (1 << 14)
#define SCB_SHCSR_MEMFAULTPENDED (1 << 13)
#define SCB_SHCSR_USGFAULTPENDED (1 << 12)
#define SCB_SHCSR_SYSTICKACT (1 << 11)
#define SCB_SHCSR_PENDSVACT (1 << 10)
#define SCB_SHCSR_MONITORACT (1 << 8)
#define SCB_SHCSR_SVCALLACT (1 << 7)
#define SCB_SHCSR_USGFAULTACT (1 << 3)
#define SCB_SHCSR_BUSFAULTACT (1 << 1)
#define SCB_SHCSR_MEMFAULTACT (1 << 0)

/* SCB Configurable Fault Status Registers */
#define SCB_CFSR_USGFAULTSR (0xffff << 16)
#define SCB_CFSR_UFSR_GET(CFSR) (((CFSR) >> 16) & 0xffff)

#define UFSR_DIVBYZERO  (1 << 9)
#define UFSR_UNALIGNED  (1 << 8)
#define UFSR_NOCP       (1 << 3)
#define UFSR_INVPC      (1 << 2)
#define UFSR_INVSTATE   (1 << 1)
#define UFSR_UNDEFINSTR (1 << 0)

#define SCB_CFSR_BUSFAULTSR (0xff << 8)
#define SCB_CFSR_BFSR_GET(CFSR) (((CFSR) >> 8) & 0xff)

#define BFSR_BFARVALID   (1 << 7)
#define BFSR_LSPERR      (1 << 5)
#define BFSR_STKERR      (1 << 4)
#define BFSR_UNSTKERR    (1 << 3)
#define BFSR_IMPRECISERR (1 << 2)
#define BFSR_PRECISERR   (1 << 1)
#define BFSR_IBUSERR     (1 << 0)

#define SCB_CFSR_MEMFAULTSR (0xff << 0)

/* SCB Hard Fault Status Register */
#define SCB_HFSR_DEBUGEVT (1 << 31)
#define SCB_HFSR_FORCED (1 << 30)
#define SCB_HFSR_VECTTBL (1 << 1)

/* SCB Debug Fault Status Register */
#define SCB_DFSR_EXTERNAL (1 << 4)
#define SCB_DFSR_VCATCH (1 << 3)
#define SCB_DFSR_DWTTRAP (1 << 2)
#define SCB_DFSR_BKPT (1 << 1)
#define SCB_DFSR_HALTED (1 << 0)

/****************************************************************************
  CM3 DCB (Debug Control Block)
 ****************************************************************************/

#define DCB_DHCSR   0x00 /* Debug Halting Control and Status Register */
#define DCB_DCRSR   0x04 /* Debug Core Register Selector Register */
#define DCB_DCRDR   0x08 /* Debug Core Register Data Register */
#define DCB_DEMCR   0x0c /* Debug Exception and Monitor Control Register */

/* Debug Halting Control and Status Register */
#define DCB_DHCSR_DBGKEY         (0xffff << 16)
#define DCB_DHCSR_S_RESET_ST     (1 << 25)
#define DCB_DHCSR_S_RETIRE_ST    (1 << 24)
#define DCB_DHCSR_S_LOCKUP       (1 << 19)
#define DCB_DHCSR_S_SLEEP        (1 << 18)
#define DCB_DHCSR_S_HALT         (1 << 17)
#define DCB_DHCSR_S_REGRDY       (1 << 16)
#define DCB_DHCSR_C_SNAPSTALL    (1 << 5)
#define DCB_DHCSR_C_MASKINTS     (1 << 3)
#define DCB_DHCSR_C_STEP         (1 << 2)
#define DCB_DHCSR_C_HALT         (1 << 1)
#define DCB_DHCSR_C_DEBUGEN      (1 << 0)

/* Debug Core Register Selector Register */
#define DCB_DCRSR_REGWNR         (1 << 16)
#define DCB_DCRSR_REGSEL         (0x1f << 0)

/* Debug Exception and Monitor Control Register */
#define DCB_DEMCR_TRCENA         (1 << 24)
#define DCB_DEMCR_MON_REQ        (1 << 19)
#define DCB_DEMCR_MON_STEP       (1 << 18)
#define DCB_DEMCR_MON_PEND       (1 << 17)
#define DCB_DEMCR_MON_EN         (1 << 16)
#define DCB_DEMCR_VC_HARDERR     (1 << 10)
#define DCB_DEMCR_VC_INTERR      (1 << 9)
#define DCB_DEMCR_VC_BUSERR      (1 << 8)
#define DCB_DEMCR_VC_STATERR     (1 << 7)
#define DCB_DEMCR_VC_CHKERR      (1 << 6)
#define DCB_DEMCR_VC_NOCPERR     (1 << 5)
#define DCB_DEMCR_VC_MMERR       (1 << 4)
#define DCB_DEMCR_VC_CORERESET   (1 << 0)

/****************************************************************************
  CM3 NVIC
 ****************************************************************************/

#define NVIC_ISER0  0x100
#define NVIC_ISER1  0x104
#define NVIC_ISER2  0x108
#define NVIC_ISER3  0x10c
#define NVIC_ISER4  0x110
#define NVIC_ISER5  0x114
#define NVIC_ISER6  0x118
#define NVIC_ISER7  0x11c
#define NVIC_ISER8  0x120
#define NVIC_ISER9  0x124
#define NVIC_ISER10 0x128
#define NVIC_ISER11 0x12c
#define NVIC_ISER12 0x130
#define NVIC_ISER13 0x134
#define NVIC_ISER14 0x138
#define NVIC_ISER15 0x13c

#define NVIC_ICER0  0x180
#define NVIC_ICER1  0x184
#define NVIC_ICER2  0x188
#define NVIC_ICER3  0x18c
#define NVIC_ICER4  0x190
#define NVIC_ICER5  0x194
#define NVIC_ICER6  0x198
#define NVIC_ICER7  0x19c
#define NVIC_ICER8  0x1a0
#define NVIC_ICER9  0x1a4
#define NVIC_ICER10 0x1a8
#define NVIC_ICER11 0x1ac
#define NVIC_ICER12 0x1b0
#define NVIC_ICER13 0x1b4
#define NVIC_ICER14 0x1b8
#define NVIC_ICER15 0x1bc

/****************************************************************************
  CM3 ITM  (Instrumentation Trace Macrocell)
 ****************************************************************************/

/* ITM Trace Privilege Register */
#define ITM_TPR_PRIVMASK (0xF << 0)

/* ITM Trace Control Register */
#define ITM_TCR_BUSY (1 << 23)
#define ITM_TCR_ATBID (0x7f << 16)
#define ITM_TCR_TSPrescale (3 << 8)
#define ITM_TCR_SWOENA (1 << 4)
#define ITM_TCR_DWTENA (1 << 3)
#define ITM_TCR_SYNCENA (1 << 2)
#define ITM_TCR_TSENA (1 << 1)
#define ITM_TCR_ITMENA (1 << 0)

/* ITM Integration Write Register */
#define ITM_IWR_ATVALIDM (1 << 0)

/* ITM Integration Read Register */
#define ITM_IRR_ATREADYM (1 << 0)

/* ITM Integration Mode Control Register */
#define ITM_IMCR_INTEGRATION (1 << 0)

/* ITM Lock Status Register */
#define ITM_LSR_BYTEACC (1 << 2)
#define ITM_LSR_ACCESS (1 << 1)
#define ITM_LSR_PRESENT (1 << 0)


/****************************************************************************
  CM3 Base addresses
 ****************************************************************************/

#define CM3_SCS_BASE     (0xe000e000) /* System Control Space Base Address */
#define CM3_ITM_BASE     (0xe0000000) /* ITM Base Address */
#define CM3_DCB_BASE     (0xe000edf0) /* Core Debug Base Address */
#define CM3_SYSTICK_BASE (CM3_SCS_BASE + 0x0010)
#define CM3_NVIC_BASE    (CM3_SCS_BASE + 0x0100)
#define CM3_SCB_BASE     (CM3_SCS_BASE + 0x0d00)
#define CM3_MPU_BASE     (CM3_SCS_BASE + 0x0d90) 

/****************************************************************************
  CM3 Exception Numbers
 ****************************************************************************/

#define CM3_EXCEPT_NMI 2
#define CM3_EXCEPT_HARD_FAULT 3
#define CM3_EXCEPT_MEM_MANAGE 4
#define CM3_EXCEPT_BUS_FAULT 5
#define CM3_EXCEPT_USAGE_FAULT 6
#define CM3_EXCEPT_SVC 11
#define CM3_EXCEPT_DEBUG_MONITOR 12
#define CM3_EXCEPT_PENDSV 14
#define CM3_EXCEPT_SYSTICK 15

/****************************************************************************
  Get the bitband addresses 
 ****************************************************************************/
#define CM3_PERIPHERAL_BITBAND_ADDR(REG, BIT) \
	(0x42000000 + (((REG) - 0x40000000) * 32) + (4 * (BIT)))

/*
#define CM3_SRAM_BITBAND_ADDR(ADDR, BIT) \
	(0x22000000 + (((ADDR) - 0x20000000) * 32) + (4 * (BIT)))
*/

#define CM3_SRAM_BITBAND_ADDR(ADDR, BIT) \
	((((ADDR) - 0x20000000 + 0x01100000) * 32) + (4 * (BIT)))
/*
#define CM3_SRAM_BITBAND_ADDR(ADDR, BIT) \
	((((ADDR) - 0x1ef00000) * 32) + (4 * (BIT)))
*/

#ifndef __ASSEMBLER__

/****************************************************************************
  Bitband pointers
 ****************************************************************************/
#define CM3_BITBAND_DEV(REG, BIT) ((uint32_t *) \
								   CM3_PERIPHERAL_BITBAND_ADDR(REG, BIT))

#define CM3_BITBAND_MEM(ADDR, BIT) ((uint32_t *) \
								   CM3_SRAM_BITBAND_ADDR((uint32_t)ADDR, BIT))
#include <stdint.h>

struct cm3_context {
	uint32_t r[13];
	uint32_t sp;
	uint32_t lr;
	uint32_t pc;
	uint32_t xpsr;
};

#define __NOP() __asm__ __volatile__ ("\tnop\n" : : )

/****************************************************************************
  CM3 SCB (System Control Block)
 ****************************************************************************/

struct cm3_scb {
	volatile uint32_t cpuid; /* CPU ID Base */
	volatile uint32_t icsr; /* Interrupt Control State */
	volatile uint32_t vtor; /* Vector Table Offset */
	volatile uint32_t aircr; /* Application Interrupt / Reset Control */
	volatile uint32_t scr; /* System Control */
	volatile uint32_t ccr; /* Configuration Control */
	volatile uint8_t shp[12]; /* System Handlers Priority (4-7, 8-11, 12-15) */
	volatile uint32_t shcsr; /* System Handler Control and State */
	volatile uint32_t cfsr; /* Configurable Fault Status */
	volatile uint32_t hfsr; /* Hard Fault Status */
	volatile uint32_t dfsr; /* Debug Fault Status */
	volatile uint32_t mmfar; /* Mem Manage Address */
	volatile uint32_t bfar; /* Bus Fault Address */
	volatile uint32_t afsr; /* Auxiliary Fault Status */
	volatile uint32_t pfr[2]; /* Processor Feature */
	volatile uint32_t dfr; /* Debug Feature */
	volatile uint32_t adr; /* Auxiliary Feature */
	volatile uint32_t mmfr[4]; /* Memory Model Feature */
	volatile uint32_t isar[5]; /* ISA Feature */
}; 

/****************************************************************************
  CM3 DCB (Debug Control Block)
 ****************************************************************************/

struct cm3_dcb {
	volatile uint32_t dhcsr; /* Debug Halting Control and Status Register */
	volatile uint32_t dcrsr; /* Debug Core Register Selector Register */
	volatile uint32_t dcrdr; /* Debug Core Register Data Register */
	volatile uint32_t demcr; /* Debug Exception and Monitor Control Register */
};


/****************************************************************************
  CM3 NVIC (Nested Vectored Interrupt Controller)
 ****************************************************************************/

struct cm3_nvic {
	volatile uint32_t iser[8]; /* Interrupt Set Enable */
	uint32_t reserved0[24]; 
	volatile uint32_t icer[8]; /* Interrupt Clear Enable */
	uint32_t rserved1[24]; 
	volatile uint32_t ispr[8]; /* Interrupt Set Pending */
	uint32_t reserved2[24]; 
	volatile uint32_t icpr[8]; /* Interrupt Clear Pending */
	uint32_t reserved3[24]; 
	volatile uint32_t iabr[8]; /* Interrupt Active bit */
	uint32_t reserved4[56]; 
	volatile uint8_t ip[240]; /* Interrupt Priority */
	uint32_t reserved5[644]; 
	volatile uint32_t stir; /* Software Trigger Interrupt */
};


/****************************************************************************
  CM3 SysTick 
 ****************************************************************************/
struct cm3_systick {
	volatile uint32_t ctrl; /* Control and Status Register */
	volatile uint32_t load; /* Reload Value Register */
	volatile uint32_t val; /* Current Value Register */
	volatile uint32_t calib; /* Calibration Register */
};

/****************************************************************************
  CM3 MPU (Memory Protection Unit)
 ****************************************************************************/
struct cm3_mpu {
	volatile uint32_t type; /* Type Register */
	volatile uint32_t ctrl; /* Control Register */
	volatile uint32_t rnr; /* Region RNRber Register */
	volatile uint32_t rbar; /* Region Base Address Register */
	volatile uint32_t rasr; /* Region Attribute and Size Register */
	volatile uint32_t rbar_a1; /* Alias 1 Region Base Address Register */
	volatile uint32_t rasr_a1; /* Alias 1 Region Attribute and Size Register */
	volatile uint32_t rbar_a2; /* Alias 2 Region Base Address Register */
	volatile uint32_t rasr_a2; /* Alias 2 Region Attribute and Size Register */
	volatile uint32_t rbar_a3; /* Alias 3 Region Base Address Register */
	volatile uint32_t rasr_a3; /* Alias 3 Region Attribute and Size Register */
}; 

/****************************************************************************
  CM3 ITM  (Instrumentation Trace Macrocell)
 ****************************************************************************/

struct cm3_itm {
	union {
		volatile uint8_t u8; /* Stimulus Port 8-bit */
		volatile uint16_t u16; /* Stimulus Port 16-bit */
		volatile uint32_t u32; /* Stimulus Port 32-bit */
	} port[32]; /* Stimulus Port Registers */
	uint32_t reserved0[864]; 
	volatile uint32_t ter; /* Trace Enable Register */
	uint32_t reserved1[15]; 
	volatile uint32_t tpr; /* Trace Privilege Register */
	uint32_t reserved2[15]; 
	volatile uint32_t tcr; /* Trace Control Register */
	uint32_t reserved3[29]; 
	volatile uint32_t iwr; /* Integration Write Register */
	volatile uint32_t irr; /* Integration Read Register */
	volatile uint32_t imcr; /* Integration Mode Control Register */
	uint32_t reserved4[43]; 
	volatile uint32_t lar; /* Lock Access Register */
	volatile uint32_t lsr; /* Lock Status Register */
	uint32_t reserved5[6]; 
	volatile uint32_t pid4; /* Peripheral Identification Register #4 */
	volatile uint32_t pid5; /* Peripheral Identification Register #5 */
	volatile uint32_t pid6; /* Peripheral Identification Register #6 */
	volatile uint32_t pid7; /* Peripheral Identification Register #7 */
	volatile uint32_t pid0; /* Peripheral Identification Register #0 */
	volatile uint32_t pid1; /* Peripheral Identification Register #1 */
	volatile uint32_t pid2; /* Peripheral Identification Register #2 */
	volatile uint32_t pid3; /* Peripheral Identification Register #3 */
	volatile uint32_t cid0; /* Component Identification Register #0 */
	volatile uint32_t cid1; /* Component Identification Register #1 */
	volatile uint32_t cid2; /* Component Identification Register #2 */
	volatile uint32_t cid3; /* Component Identification Register #3 */
}; 



#define CM3_SYSTICK ((struct cm3_systick *) CM3_SYSTICK_BASE) 
#define CM3_SCB ((struct cm3_scb *) CM3_SCB_BASE) 
#define CM3_NVIC ((struct cm3_nvic *) CM3_NVIC_BASE) 
#define CM3_DCB ((struct cm3_dcb *) CM3_DCB_BASE) 
#define CM3_ITM ((struct cm3_itm *) CM3_ITM_BASE) 
#define CM3_MPU ((struct cm3_mpu *) CM3_MPU_BASE) 


#ifdef __cplusplus
extern "C" {
#endif

/* Enable Interrupt in NVIC Interrupt Controller */
static inline void cm3_irq_enable(int irq) {
	CM3_NVIC->iser[irq >> 5] = (1 << (irq & 0x1f));
}

/* Disable the interrupt line for external interrupt specified */
static inline void cm3_irq_disable(int irq) {
	CM3_NVIC->icer[irq >> 5] = (1 << (irq & 0x1f));
}

static inline int cm3_irq_is_enabled(int irq) {
	return (CM3_NVIC->icer[irq >> 5] & (1 << (irq & 0x1f))) ? 1 : 0;
}

/* Read the interrupt pending bit for a device specific interrupt source */
static inline int cm3_irq_pend_get(int irq) {
	return (CM3_NVIC->ispr[irq >> 5] & (1 << (irq & 0x1f))) ? 1 : 0;
}

/* Set the pending bit for an external interrupt */
static inline void cm3_irq_pend_set(int irq) {
	CM3_NVIC->ispr[irq >> 5] = (1 << (irq & 0x1f));
}

/* Clear the pending bit for an external interrupt */
static inline void cm3_irq_pend_clr(int irq) {
	CM3_NVIC->icpr[irq >> 5] = (1 << (irq & 0x1f));
}

/* Read the active bit for an external interrupt */
static inline int cm3_irq_act_get(int irq) {
	return (CM3_NVIC->iabr[irq >> 5] & (1 << (irq & 0x1f))) ? 1 : 0;
}

/* Set the priority for an interrupt */
static inline void cm3_irq_pri_set(int irq, int pri) {
	CM3_NVIC->ip[irq] = pri;
}

/* Get the priority for an interrupt */
static inline int cm3_irq_pri_get(int irq) {
	return CM3_NVIC->ip[irq];
}

/* Set Priority for Cortex-M3 System Interrupts */
static inline void cm3_except_pri_set(int xcpt, int pri) {
	CM3_SCB->shp[xcpt - 4] = pri;
}

/* Get Priority for Cortex-M3 System Interrupts */
static inline int cm3_except_pri_get(int xcpt) {
	return CM3_SCB->shp[xcpt - 4];
}

/* read a bit from a bitbang memory region */
static uint32_t inline __bit_mem_rd(void * ptr, int32_t bit) {
	/* get the bitband bit address */
	uint32_t * bit_addr = CM3_BITBAND_MEM(ptr, 0);
	return bit_addr[bit];
}

/* write a bit into a bitbang memory region */
static void inline __bit_mem_wr(void * ptr, int32_t bit, uint32_t val) {
	/* get the bitband bit address */
	uint32_t * bit_addr = CM3_BITBAND_MEM(ptr, 0);
	bit_addr[bit] = val;
}

#define CONTROL_THREAD_PRIV (0 << 0)
#define CONTROL_THREAD_USER (1 << 0)

#define CONTROL_THREAD_MSP (0 << 1)
#define CONTROL_THREAD_PSP (1 << 1)

static inline void cm3_control_set(uint32_t val) {
	asm volatile ("msr CONTROL, %0\n" : : "r" (val));
}

static inline void __attribute__((always_inline)) 
	cm3_basepri_set(uint32_t val) {
	asm volatile ("msr BASEPRI, %0\n" : : "r" (val));
}

static inline uint32_t __attribute__((always_inline)) cm3_basepri_get(void) {
	uint32_t val;
	asm volatile ("mrs %0, BASEPRI\n" : "=r" (val));
	return val;
}

static inline uint32_t cm3_ipsr_get(void) {
	uint32_t ipsr;
	asm volatile ("mrs %0, IPSR\n" : "=r" (ipsr));
	return ipsr;
}

static void inline __attribute__((always_inline)) cm3_cpsid_i(void) {
	asm volatile ("cpsid i\n");
}

static void inline __attribute__((always_inline)) cm3_cpsie_i(void) {
	asm volatile ("cpsie i\n");
}

/* disable interrupts and fault handlers (set fault mask) */
static void inline __attribute__((always_inline)) cm3_cpsid_f(void) {
	asm volatile ("cpsid f\n");
}

/* enable interrupts and fault handlers (set fault mask) */
static void inline __attribute__((always_inline)) cm3_cpsie_f(void) {
	asm volatile ("cpsie f\n");
}

static inline void cm3_primask_set(uint32_t val) {
	asm volatile ("msr PRIMASK, %0\n" : : "r" (val));
}

static inline uint32_t cm3_primask_get(void) {
	uint32_t val;
	asm volatile ("mrs %0, PRIMASK\n" : "=r" (val));
	return val;
}

static inline void  cm3_faultmask_set(uint32_t val) {
	asm volatile ("msr FAULTMASK, %0\n" : : "r" (val));
}

static inline uint32_t cm3_faultmask_get(void) {
	uint32_t val;
	asm volatile ("mrs %0, FAULTMASK\n" : "=r" (val));
	return val;
}

static inline void cm3_msp_set(uint32_t val) {
	asm volatile ("msr MSP, %0\n" : : "r" (val));
}

static inline uint32_t cm3_msp_get(void) {
	uint32_t msp;
	asm volatile ("mrs %0, MSP\n" : "=r" (msp));
	return msp;
}

static inline void cm3_psp_set(uint32_t val) {
	asm volatile ("msr PSP, %0\n" : : "r" (val));
}

static inline uint32_t cm3_psp_get(void) {
	uint32_t psp;
	asm volatile ("mrs %0, PSP\n" : "=r" (psp));
	return psp;
}

static inline uint32_t cm3_sp_get(void) {
	register uint32_t sp;
	asm volatile ("mov %0, sp\n" : "=r" (sp));
	return sp;
}

static inline uint32_t __clz(uint32_t val) {
	register uint32_t ret;
	asm volatile ("clz %0, %1\n" : "=r" (ret) : "r" (val));
	return ret;
}

static inline uint32_t __rbit(uint32_t val) {
	register uint32_t ret;
	asm volatile ("rbit %0, %1\n" : "=r" (ret) : "r" (val));
	return ret;
}

static inline uint8_t __ldrexb(uint8_t * addr) {
	register uint8_t ret;
	asm volatile ("ldrexb %0, [%1]" : "=r" (ret) : "r" (addr));
	return ret;
}

static inline uint32_t __ldrexh(uint16_t * addr) {
	register uint16_t ret;
	asm volatile ("ldrexh %0, [%1]" : "=r" (ret) : "r" (addr));
	return ret;
}

static inline uint32_t __ldrexw(uint32_t * addr) {
	register uint32_t ret;
	asm volatile ("ldrex %0, [%1]" : "=r" (ret) : "r" (addr));
	return ret;
}

static inline uint32_t __strexb(uint8_t * addr, uint8_t val) {
	register uint32_t ret;
	asm volatile ("strexb %0, %2, [%1]" : "=r" (ret) : "r" (addr), "r" (val));
	return ret;
}

static inline uint32_t __strexh(uint16_t * addr, uint16_t val) {
	register uint32_t ret;
	asm volatile ("strexh %0, %2, [%1]" : "=r" (ret) : "r" (addr), "r" (val));
	return ret;
}

static inline uint32_t __strexw(uint32_t * addr, uint32_t val) {
	register uint32_t ret;
	asm volatile ("strex %0, %2, [%1]" : "=r" (ret) : "r" (addr), "r" (val));
	return ret;
}

static inline void __clrx(void) {
	asm volatile ("clrex" : );
}

void cm3_udelay_calibrate(void);

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __CORTEX_M3_H__ */

