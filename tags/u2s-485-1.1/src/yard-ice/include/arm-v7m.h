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
 * @file rm-v7m.h
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */



#ifndef __ARM_V7M_H__
#define __ARM_V7M_H__

/*************************************************************************/
/* System Control Space (SCS)                                            */
/*************************************************************************/

/*
    The System Control Space (SCS) is a memory-mapped 4KB address space that provides 32-bit registers for configuration, status reporting and control. The SCS registers divide into the following groups:
	• system control and identification
	• the CPUID processor identification space
	• system configuration and status
	• fault reporting
	• a system timer, SysTick
	• a Nested Vectored Interrupt Controller (NVIC)
	• a Protected Memory System Architecture (PMSA)
	• system debug.

   +---------------------------------------+-----------------------+
   | System control and ID registers       | 0xE000E000-0xE000E00F |
   |   System control block (SCB)          | 0xE000ED00-0xE000ED8F |
   |   Debug registers in the SCS          | 0xE000EDF0-0xE000EEFF |
   |   SW Trigger Interrupt Register       | 0xE000EF00-0xE000EF8F |
   |   IMPLEMENTATION DEFINED              | 0xE000EF90-0xE000EFCF |
   |   Microcontroller-specific ID space   | 0xE000EFD0-0xE000EFFF |
   | SysTick                               | 0xE000E010-0xE000E0FF |
   | NVIC                                  | 0xE000E100-0xE000ECFF |
   | MPU                                   | 0xE000ED90-0xE000EDEF | 
   +---------------------------------------+-----------------------+
*/

/*************************************************************************/
/* System control and ID registers                                       */
/*************************************************************************/

/* System control block (SCB) registers */
#define ARMV7M_CPUID 0xe000ed00 /* CPUID Base Register */
#define ARMV7M_ICSR 0xe000ed04 /* Interrupt Control and State Register */
#define ARMV7M_VTOR 0xe000ed08 /* Vector Table Offset Register */
#define ARMV7M_AIRCR 0xe000ed0c /* Application Interrupt and Reset 
								   Control Register */
#define ARMV7M_SCR 0xe000ed10 /* System Control Register */
#define ARMV7M_CCR 0xe000ed14 /* Configuration and Control Register */
#define ARMV7M_SHPR1 0xe000ed18 /* System Handler Priority Register 1 */
#define ARMV7M_SHPR2 0xe000ed1c /* System Handler Priority Register 2 */
#define ARMV7M_SHPR3 0xe000ed20 /* System Handler Priority Register 3 */
#define ARMV7M_SHCSR 0xe000ed24 /* System Handler Control and State Register */
#define ARMV7M_CFSR 0xe000ed28 /* Configurable Fault Status Register */
#define ARMV7M_HFSR 0xe000ed2c /* HardFault Status Register */
#define ARMV7M_DFSR 0xe000ed30 /* Debug Fault Status Register */
#define ARMV7M_MMFAR 0xe000ed34 /* MemManage Fault Address Register */
#define ARMV7M_BFAR 0xe000ed38 /* BusFault Address Register */
#define ARMV7M_AFSR 0xe000ed3c /* Auxiliary Fault Status Register */
#define ARMV7M_CPACR 0xe000ed88 /* Coprocessor Access Control Register */

/* Additional SCB registers for the FP extension */
#define ARMV7M_FPCCR 0xe000ef34 /* Floating-Point Context Control Register */
#define ARMV7M_FPCAR 0xe000ef38 /* Floating-Point Context Address Register */
#define ARMV7M_FPDSCR 0xe000ef3c /* Floating-Point Default Status 
									Control Register */
#define ARMV7M_MVFR0 0xe000ef40 /* Media and FP Feature Register 0 */
#define ARMV7M_MVFR1 0xe000ef44 /* Media and FP Feature Register 1 */

#define ARMV7M_ICTR 0xe000e004 /* Interrupt Controller Type Register */
#define ARMV7M_ACTLR 0xe000e008 /* Auxiliary Control Register */
#define ARMV7M_STIR 0xe000ef00  /* Software Triggered Interrupt Register */

/* Peripheral Identification Registers */
#define ARMV7M_SCB_PID4 0xe000efd0 
#define ARMV7M_SCB_PID5 0xe000efd4 
#define ARMV7M_SCB_PID6 0xe000efd8
#define ARMV7M_SCB_PID7 0xe000efdc 
#define ARMV7M_SCB_PID0 0xe000efe0
#define ARMV7M_SCB_PID1 0xe000efe4
#define ARMV7M_SCB_PID2 0xe000efe8
#define ARMV7M_SCB_PID3 0xe000efec

/* Component Identification Registers */
#define ARMV7M_SCB_CID0 0xe000eff0 
#define ARMV7M_SCB_CID1 0xe000eff4
#define ARMV7M_SCB_CID2 0xe000eff8
#define ARMV7M_SCB_CID3 0xe000effc

/* Debug Halting Control and Status Register */
#define ARMV7M_DHCSR 0xe000edf0
/* Debug Core Register Selector Register */
#define ARMV7M_DCRSR 0xe000edf4
/* Debug Core Register Data Register */
#define ARMV7M_DCRDR 0xe000edf8
/* Debug Exception and Monitor Control Register */
#define ARMV7M_DEMCR 0xe000edfc

/*************************************************************************/
/* The system timer, SysTick                                             */
/*************************************************************************/

#define ARMV7M_SYST_CSR 0xe000e010 /* SysTick Control and Status Register */
#define ARMV7M_SYST_RVR 0xe000e014 /* SysTick Reload Value Register */
#define ARMV7M_SYST_CVR 0xe000e018 /* SysTick Current Value Register */
#define ARMV7M_SYST_CALIB 0xe000e01c /* SysTick Calibration value Register */

/*************************************************************************/
/* Nested Vectored Interrupt Controller, NVIC                            */
/*************************************************************************/

/* Interrupt Set-Enable Registers */
#define ARMV7M_NVIC_ISER0 0xe000e100 
#define ARMV7M_NVIC_ISER15 0xe000e13c

/* Interrupt Clear-Enable Registers */
#define ARMV7M_NVIC_ICER0 0xe000e180 
#define ARMV7M_NVIC_ICER1 0xe000e184 
#define ARMV7M_NVIC_ICER2 0xe000e188 
#define ARMV7M_NVIC_ICER3 0xe000e18c 
#define ARMV7M_NVIC_ICER4 0xe000e190 
#define ARMV7M_NVIC_ICER5 0xe000e194 
#define ARMV7M_NVIC_ICER6 0xe000e198 
#define ARMV7M_NVIC_ICER7 0xe000e19c 
#define ARMV7M_NVIC_ICER8 0xe000e1a0 
#define ARMV7M_NVIC_ICER9 0xe000e1a4 
#define ARMV7M_NVIC_ICER10 0xe000e1a8 
#define ARMV7M_NVIC_ICER11 0xe000e1ac 
#define ARMV7M_NVIC_ICER12 0xe000e1b0 
#define ARMV7M_NVIC_ICER13 0xe000e1b4 
#define ARMV7M_NVIC_ICER14 0xe000e1b8 
#define ARMV7M_NVIC_ICER15 0xe000e1bc 

/* Interrupt Set-Pending Registers */
#define ARMV7M_NVIC_ISPR0 0xe000e200
#define ARMV7M_NVIC_ISPR1 0xe000e204
#define ARMV7M_NVIC_ISPR2 0xe000e208
#define ARMV7M_NVIC_ISPR3 0xe000e20c
#define ARMV7M_NVIC_ISPR4 0xe000e210
#define ARMV7M_NVIC_ISPR5 0xe000e214
#define ARMV7M_NVIC_ISPR6 0xe000e218
#define ARMV7M_NVIC_ISPR7 0xe000e21c
#define ARMV7M_NVIC_ISPR8 0xe000e220
#define ARMV7M_NVIC_ISPR9 0xe000e224
#define ARMV7M_NVIC_ISPR10 0xe000e228 
#define ARMV7M_NVIC_ISPR11 0xe000e22c 
#define ARMV7M_NVIC_ISPR12 0xe000e230 
#define ARMV7M_NVIC_ISPR13 0xe000e234 
#define ARMV7M_NVIC_ISPR14 0xe000e238 
#define ARMV7M_NVIC_ISPR15 0xe000e23c 

/* Interrupt Clear-Pending Registers */
#define ARMV7M_NVIC_ICPR0 0xe000e280 
#define ARMV7M_NVIC_ICPR1 0xe000e284 
#define ARMV7M_NVIC_ICPR2 0xe000e288 
#define ARMV7M_NVIC_ICPR3 0xe000e28c 
#define ARMV7M_NVIC_ICPR4 0xe000e290 
#define ARMV7M_NVIC_ICPR5 0xe000e294 
#define ARMV7M_NVIC_ICPR6 0xe000e298 
#define ARMV7M_NVIC_ICPR7 0xe000e29c 
#define ARMV7M_NVIC_ICPR8 0xe000e2a0 
#define ARMV7M_NVIC_ICPR9 0xe000e2a4 
#define ARMV7M_NVIC_ICPR10 0xe000e2a8 
#define ARMV7M_NVIC_ICPR11 0xe000e2ac 
#define ARMV7M_NVIC_ICPR12 0xe000e2b0 
#define ARMV7M_NVIC_ICPR13 0xe000e2b4 
#define ARMV7M_NVIC_ICPR14 0xe000e2b8 
#define ARMV7M_NVIC_ICPR15 0xe000e2bc 

/* Interrupt Active Bit Registers */
#define ARMV7M_NVIC_IABR0 0xe000e300 
#define ARMV7M_NVIC_IABR1 0xe000e304 
#define ARMV7M_NVIC_IABR2 0xe000e308 
#define ARMV7M_NVIC_IABR3 0xe000e30c 
#define ARMV7M_NVIC_IABR4 0xe000e310 
#define ARMV7M_NVIC_IABR5 0xe000e314 
#define ARMV7M_NVIC_IABR6 0xe000e318 
#define ARMV7M_NVIC_IABR7 0xe000e31c 
#define ARMV7M_NVIC_IABR8 0xe000e320 
#define ARMV7M_NVIC_IABR9 0xe000e324 
#define ARMV7M_NVIC_IABR10 0xe000e328 
#define ARMV7M_NVIC_IABR11 0xe000e32c 
#define ARMV7M_NVIC_IABR12 0xe000e330 
#define ARMV7M_NVIC_IABR13 0xe000e334 
#define ARMV7M_NVIC_IABR14 0xe000e338 
#define ARMV7M_NVIC_IABR15 0xe000e33c 

/* Interrupt Priority Registers */
#define ARMV7M_NVIC_IPR0 0xe000e400
#define ARMV7M_NVIC_IPR1 0xe000e404
#define ARMV7M_NVIC_IPR2 0xe000e408
#define ARMV7M_NVIC_IPR3 0xe000e40c
#define ARMV7M_NVIC_IPR4 0xe000e410
#define ARMV7M_NVIC_IPR5 0xe000e414
#define ARMV7M_NVIC_IPR6 0xe000e418
#define ARMV7M_NVIC_IPR7 0xe000e41c
#define ARMV7M_NVIC_IPR8 0xe000e420
#define ARMV7M_NVIC_IPR9 0xe000e424
#define ARMV7M_NVIC_IPR10 0xe000e428
#define ARMV7M_NVIC_IPR11 0xe000e42c
#define ARMV7M_NVIC_IPR12 0xe000e430
#define ARMV7M_NVIC_IPR13 0xe000e434
#define ARMV7M_NVIC_IPR14 0xe000e438
#define ARMV7M_NVIC_IPR15 0xe000e43c

#define ARMV7M_NVIC_IPR16 0xe000e440
#define ARMV7M_NVIC_IPR17 0xe000e444
#define ARMV7M_NVIC_IPR18 0xe000e448
#define ARMV7M_NVIC_IPR19 0xe000e44c
#define ARMV7M_NVIC_IPR20 0xe000e450
#define ARMV7M_NVIC_IPR21 0xe000e454
#define ARMV7M_NVIC_IPR22 0xe000e458
#define ARMV7M_NVIC_IPR23 0xe000e45c
#define ARMV7M_NVIC_IPR24 0xe000e460
#define ARMV7M_NVIC_IPR25 0xe000e464
#define ARMV7M_NVIC_IPR26 0xe000e468
#define ARMV7M_NVIC_IPR27 0xe000e46c
#define ARMV7M_NVIC_IPR28 0xe000e470
#define ARMV7M_NVIC_IPR29 0xe000e474
#define ARMV7M_NVIC_IPR30 0xe000e478
#define ARMV7M_NVIC_IPR31 0xe000e47c

#define ARMV7M_NVIC_IPR32 0xe000e480
#define ARMV7M_NVIC_IPR33 0xe000e484
#define ARMV7M_NVIC_IPR34 0xe000e488
#define ARMV7M_NVIC_IPR35 0xe000e48c
#define ARMV7M_NVIC_IPR36 0xe000e490
#define ARMV7M_NVIC_IPR37 0xe000e494
#define ARMV7M_NVIC_IPR38 0xe000e498
#define ARMV7M_NVIC_IPR39 0xe000e49c
#define ARMV7M_NVIC_IPR40 0xe000e4a0
#define ARMV7M_NVIC_IPR41 0xe000e4a4
#define ARMV7M_NVIC_IPR42 0xe000e4a8
#define ARMV7M_NVIC_IPR43 0xe000e4ac
#define ARMV7M_NVIC_IPR44 0xe000e4b0
#define ARMV7M_NVIC_IPR45 0xe000e4b4
#define ARMV7M_NVIC_IPR46 0xe000e4b8
#define ARMV7M_NVIC_IPR47 0xe000e4bc

#define ARMV7M_NVIC_IPR48 0xe000e4c0
#define ARMV7M_NVIC_IPR49 0xe000e4c4
#define ARMV7M_NVIC_IPR50 0xe000e4c8
#define ARMV7M_NVIC_IPR51 0xe000e4cc
#define ARMV7M_NVIC_IPR52 0xe000e4d0
#define ARMV7M_NVIC_IPR53 0xe000e4d4
#define ARMV7M_NVIC_IPR54 0xe000e4d8
#define ARMV7M_NVIC_IPR55 0xe000e4dc
#define ARMV7M_NVIC_IPR56 0xe000e4e0
#define ARMV7M_NVIC_IPR57 0xe000e4e4
#define ARMV7M_NVIC_IPR58 0xe000e4e8
#define ARMV7M_NVIC_IPR59 0xe000e4ec
#define ARMV7M_NVIC_IPR60 0xe000e4f0
#define ARMV7M_NVIC_IPR61 0xe000e4f4
#define ARMV7M_NVIC_IPR62 0xe000e4f8
#define ARMV7M_NVIC_IPR63 0xe000e4fc

#define ARMV7M_NVIC_IPR64 0xe000e500
#define ARMV7M_NVIC_IPR65 0xe000e504
#define ARMV7M_NVIC_IPR66 0xe000e508
#define ARMV7M_NVIC_IPR67 0xe000e50c
#define ARMV7M_NVIC_IPR68 0xe000e510
#define ARMV7M_NVIC_IPR69 0xe000e514
#define ARMV7M_NVIC_IPR70 0xe000e518
#define ARMV7M_NVIC_IPR71 0xe000e51c
#define ARMV7M_NVIC_IPR72 0xe000e520
#define ARMV7M_NVIC_IPR73 0xe000e524
#define ARMV7M_NVIC_IPR74 0xe000e528
#define ARMV7M_NVIC_IPR75 0xe000e52c
#define ARMV7M_NVIC_IPR76 0xe000e530
#define ARMV7M_NVIC_IPR77 0xe000e534
#define ARMV7M_NVIC_IPR78 0xe000e538
#define ARMV7M_NVIC_IPR79 0xe000e53c

#define ARMV7M_NVIC_IPR80 0xe000e540
#define ARMV7M_NVIC_IPR81 0xe000e544
#define ARMV7M_NVIC_IPR82 0xe000e548
#define ARMV7M_NVIC_IPR83 0xe000e54c
#define ARMV7M_NVIC_IPR84 0xe000e550
#define ARMV7M_NVIC_IPR85 0xe000e554
#define ARMV7M_NVIC_IPR86 0xe000e558
#define ARMV7M_NVIC_IPR87 0xe000e55c
#define ARMV7M_NVIC_IPR88 0xe000e560
#define ARMV7M_NVIC_IPR89 0xe000e564
#define ARMV7M_NVIC_IPR90 0xe000e568
#define ARMV7M_NVIC_IPR91 0xe000e56c
#define ARMV7M_NVIC_IPR92 0xe000e570
#define ARMV7M_NVIC_IPR93 0xe000e574
#define ARMV7M_NVIC_IPR94 0xe000e578
#define ARMV7M_NVIC_IPR95 0xe000e57c

#define ARMV7M_NVIC_IPR96 0xe000e580
#define ARMV7M_NVIC_IPR97 0xe000e584
#define ARMV7M_NVIC_IPR98 0xe000e588
#define ARMV7M_NVIC_IPR99 0xe000e58c
#define ARMV7M_NVIC_IPR100 0xe000e590 
#define ARMV7M_NVIC_IPR101 0xe000e594 
#define ARMV7M_NVIC_IPR102 0xe000e598 
#define ARMV7M_NVIC_IPR103 0xe000e59c 
#define ARMV7M_NVIC_IPR104 0xe000e5a0 
#define ARMV7M_NVIC_IPR105 0xe000e5a4 
#define ARMV7M_NVIC_IPR106 0xe000e5a8 
#define ARMV7M_NVIC_IPR107 0xe000e5ac 
#define ARMV7M_NVIC_IPR108 0xe000e5b0 
#define ARMV7M_NVIC_IPR109 0xe000e5b4 
#define ARMV7M_NVIC_IPR110 0xe000e5b8 
#define ARMV7M_NVIC_IPR111 0xe000e5bc 

#define ARMV7M_NVIC_IPR112 0xe000e5c0 
#define ARMV7M_NVIC_IPR113 0xe000e5c4 
#define ARMV7M_NVIC_IPR114 0xe000e5c8 
#define ARMV7M_NVIC_IPR115 0xe000e5cc 
#define ARMV7M_NVIC_IPR116 0xe000e5d0 
#define ARMV7M_NVIC_IPR117 0xe000e5d4 
#define ARMV7M_NVIC_IPR118 0xe000e5d8 
#define ARMV7M_NVIC_IPR119 0xe000e5dc 
#define ARMV7M_NVIC_IPR120 0xe000e5e0 
#define ARMV7M_NVIC_IPR121 0xe000e5e4 
#define ARMV7M_NVIC_IPR122 0xe000e5e8 
#define ARMV7M_NVIC_IPR123 0xe000e5ec 


/* ----------------------------------------------------------------------- */
/* CPUID Base Register */
/* ----------------------------------------------------------------------- */

#define CPUID_IMPLEMENTER (((CPUID) >> 24) & 0x0ff) /* Implementer code 
 assigned by ARM. Reads as 0x41 for a processor implemented by ARM. */

#define CPUID_VARIANT (((CPUID) >> 20) & 0x0f)
#define CPUID_ARCHITECTURE (((CPUID) >> 16) & 0x0f)
#define CPUID_PARTNO (((CPUID) >> 3) & 0x0fff) /* IMPLEMENTATION DEFINED 
												  part number. */
#define CPUID_REVISION (((CPUID) >> 0) & 0x0f) /* IMPLEMENTATION DEFINED 
												  revision number. */


/* ----------------------------------------------------------------------- */
/* Interrupt Control and State Register, ICSR                              */
/* ----------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/* Application Interrupt and Reset Control Register                        */
/* ----------------------------------------------------------------------- */

#define AIRCR_VECTKEYSTAT 0xfa05
#define AIRCR_VECTKEY_DEC(AIRCR) ((AIRCR) >> 16)
#define AIRCR_VECTKEY_MASK ((0xffff) >> 16)
#define AIRCR_VECTKEY (0x05fa << 16) /* (RW) Write: VECTKEY Vector Key.
	Read: VECTKEYSTAT Register writes must write 0x05FA to this field, 
	otherwise the write is ignored.
	On reads, returns 0xFA05. */
#define AIRCR_ENDIANNESS (1 << 15) /* (RO) Indicates the memory system 
  endianness:
  0 = Little endian
  1 = Big endian.
  This bit is static or configured by a hardware input on reset. */

#define AIRCR_PRIGROUP (0x7 << 8) /* (RW) Priority grouping, indicates 
	the binary point position. */

#define AIRCR_SYSRESETREQ (1 << 2) /* (RW) System Reset Request:
	0 = Do not request a reset.
	1 = Request reset.
	Writing 1 to this bit asserts a signal to the external system to request
	a Local reset.
	A Local or Power-on reset clears this bit to 0. */

#define AIRCR_VECTCLRACTIVE (1 << 1) /* (WO) Writing 1 to this bit clears all
	active state information for fixed and configurable exceptions. 
	This includes clearing the IPSR to zero, see The IPSR on page B1-625.
	The effect of writing a 1 to this bit if the processor is not halted in
	Debug state is UNPREDICTABLE. */

#define AIRCR_VECTRESET (1 << 0) /* (WO) Writing 1 to this bit causes a 
	local system reset, see Reset management on page B1-679 for more 
	information. This bit self-clears.
	The effect of writing a 1 to this bit if the processor is not halted in
	Debug state is UNPREDICTABLE.
	When the processor is halted in Debug state, if a write to the register
	writes a 1 to both VECTRESET and SYSRESETREQ, the behavior 
	is UNPREDICTABLE. */


/* ----------------------------------------------------------------------- */
/* Debug Fault Status Register, DFSR                                       */
/* ----------------------------------------------------------------------- */

#define DFSR_EXTERNAL (1 << 4) /* Indicates a debug event generated because 
	of the assertion of EDBGRQ: 
	0 = No EDBGRQ debug event.
	1 = EDBGRQ debug event. */

#define DFSR_VCATCH (1 << 3) /* Indicates triggering of a Vector catch: 
	0 = No Vector catch triggered.
	1 = Vector catch triggered.
	The corresponding FSR shows the primary cause of the exception. */

#define DFSR_DWTTRAP (1 << 2) /* Indicates a debug event generated by the DWT:
	0 = No current debug events generated by the DWT.
	1 = At least one current debug event generated by the DWT. */
				
#define DFSR_BKPT (1 << 1) /* Indicates a debug event generated by BKPT 
	instruction execution or a breakpoint match in FPB:
	0 = No current breakpoint debug event.
	1 = At least one current breakpoint debug event. */

#define DFSR_HALTED  (1 << 0) /* Indicates a debug event generated by either:
	• a C_HALT or C_STEP request, triggered by a write to the DHCSR
	• a step request triggered by setting DEMCR.MON_STEP to 1.
	0 = No active halt request debug event.
	1 = Halt request debug event active. */

/* ----------------------------------------------------------------------- */
/* Debug Halting Control and Status Register, DHCSR                        */
/* ----------------------------------------------------------------------- */

#define DHCSR_DBGKEY ((0xa05f) << 16) /* (WO) Debug key: must write 0xA05F to 
	this field to enable write accesses to bits [15:0], otherwise the 
	processor ignores the write access. */

#define DHCSR_S_RESET_ST (1 << 25) /* (RO) Indicates whether the processor 
	has been reset since the last read of DHCSR:
	0 = No reset since last DHCSR read.
	1 = At least one reset since last DHCSR read.
	This is a sticky bit, that clears to 0 on a read of DHCSR. */

#define DHCSR_S_RETIRE_ST (1 << 24) /* (RO) Indicates whether the processor
	has completed the execution of an instruction since the last read of DHCSR:
	0 = No instruction retired since last DHCSR read.
	1 = At least one instruction retired since last DHCSR read.
	This is a sticky bit, that clears to 0 on a read of DHCSR.
	A debugger can check this bit to determine if the processor is stalled 
	on a load, store or fetch access.
	This bit is UNKNOWN after a Power-on or Local reset, but then is set to 1
	as soon as the processor executes and retires an instruction.  */

#define DHCSR_S_LOCKUP (1 << 19) /* (RO) Indicates whether the processor is 
	locked up because of an unrecoverable exception:
	0 = Not locked up
	1 = Locked up
	This bit can only be read as 1 by a remote debugger, using the DAP.
	The value of 1 indicates that the processor is running but locked up.
	The bit clears to 0 when the processor enters Debug state. */

#define DHCSR_S_SLEEP (1 << 18) /* (RO) Indicates whether the processor 
	is sleeping:
	0 = Not sleeping.
	1 = Sleeping.
	The debugger must set the C_HALT bit to 1 to gain control, or wait for 
	an interrupt or other wakeup event to wakeup the system. */

#define DHCSR_S_HALT (1 << 17) /* (RO) Indicates whether the processor is in 
	Debug state:
	0 = Not in Debug state.
	1 = In Debug state. */

#define DHCSR_S_REGRDY (1 << 16) /* (RO) A handshake flag for transfers 
	through the DCRDR: 
	• Writing to DCRSR clears the bit to 0. 
	• Completion of the DCRDR transfer then sets the bit to 1. 
	0 = There has been a write to the DCRDR, but the transfer is not complete
	1 = The transfer to or from the DCRDR is complete.
	This bit is valid only when the processor is in Debug state, otherwise 
	the bit is UNKNOWN. */

#define DHCSR_C_SNAPSTALL (1 << 5) /* (RW) If the processor is stalled on a 
	load or store operation, a debugger can set this bit to 1 to attempt to 
	break the stall. The effect of this bit is:
	0 = No action 
	1 = Attempt to force any stalled load or store instruction to complete. 
	The effect of setting this bit to 1 is UNPREDICTABLE unless the DHCSR 
	write also sets C_DEBUGEN and C_HALT to 1. This means that if the 
	processor is not already in Debug state a it enters Debug state when 
	the stalled instruction completes. Writing 1 to this bit makes the state 
	of the memory system UNPREDICTABLE. Therefore, if a debugger writes 1 to 
	this bit it must reset the processor before leaving Debug state.
	--- Note ---
	A debugger can write to the DHCSR to clear this bit to 0.
	However, this does not remove the UNPREDICTABLE state of the
	memory system caused by setting C_SNAPSTALL to 1.
	The architecture does not guarantee that setting this bit to 1 will
	force a stalled load or store operation to complete. */

#define DHCSR_C_MASKINTS (1 << 3) /* (RW) When debug is enabled, the debugger 
	can write to this bit to mask PendSV, SysTick and external configurable 
	interrupts: 
	0 = Do not mask.
	1 = Mask PendSV, SysTick and external configurable interrupts.
	The effect of any attempt to change the value of this bit is 
	UNPREDICTABLE unless both:
	• before the write to DHCSR, the value of the C_HALT bit is 1 
	• the write to the DHCSR that changes the C_MASKINTS bit also writes 1 
	to the C_HALT bit.
	This means that a single write to DHCSR cannot set the C_HALT to 0 and 
	change the value of the C_MASKINTS bit.
	The bit does not affect NMI. When DHCSR.C_DEBUGEN is set to 0, the value 
	of this bit is UNKNOWN. */

#define DHCSR_C_STEP (1 << 2) /* (RW) Processor step bit. The effects of writes 
	to this bit are:
	0 = No effect.
	1 = Step the processor. */

#define DHCSR_C_HALT (1 << 1) /* (RW) Processor halt bit. The effects of writes 
	to this bit are:
	0 = No effect.
	1 = Halt the processor. */

#define DHCSR_C_DEBUGEN (1 << 0) /* (RW) Halting debug enable bit: 
	0 = Disabled.
	1 = Enabled.
	If a debugger writes to DHCSR to change the value of this bit from 0 to 
	1, it must also write 0 to the C_MASKINTS bit, otherwise behavior is 
	UNPREDICTABLE. This bit can only be set to 1 from the DAP, it cannot 
	be set to 1 under software control. */

/* ----------------------------------------------------------------------- */
/* Debug Core Register Selector Register, DCRSR                            */
/* ----------------------------------------------------------------------- */


#define DCRSR_REGWNR (1 << 16) /* Specifies the access type for the transfer:
	0 = Read
	1 = Write */

#define DCRSR_REGSEL (0x7f << 0) /* Specifies the ARM core register, 
	special-purpose register, or Floating-point extension 
	register, to transfer: 
	0b0000000-0b0001100 ARM core registers R0-R12. 
	0b0001101 The current SP. See also values 0b0010001 and 0b0010010.
	0b0001110 LR.
	0b0001111 DebugReturnAddress
	0b0010000 xPSR.
	0b0010001 Main stack pointer, MSP.
	0b0010010 Process stack pointer, PSP.
	0b0010100 Bits [31:24] CONTROL
		Bits [23:16] FAULTMASK
		Bits [15:8] BASEPRI
		Bits [7:0] PRIMASK.
   0b0100001 Floating-point Status and Control Register, FPSCR
   0b1000000-0b1011111 FP registers S0-S31. */

#define DCRSR_REGSEL_R0 (0x00 << 0)
#define DCRSR_REGSEL_R1 (0x01 << 0)
#define DCRSR_REGSEL_R2 (0x02 << 0)
#define DCRSR_REGSEL_R3 (0x03 << 0)
#define DCRSR_REGSEL_R4 (0x04 << 0)
#define DCRSR_REGSEL_R5 (0x05 << 0)
#define DCRSR_REGSEL_R6 (0x06 << 0)
#define DCRSR_REGSEL_R7 (0x07 << 0)
#define DCRSR_REGSEL_R8 (0x08 << 0)
#define DCRSR_REGSEL_R9 (0x09 << 0)
#define DCRSR_REGSEL_R10 (0x0a << 0)
#define DCRSR_REGSEL_R11 (0x0b << 0)
#define DCRSR_REGSEL_R12 (0x0c << 0)
#define DCRSR_REGSEL_SP (0x0d << 0)
#define DCRSR_REGSEL_LR (0x0e << 0)
#define DCRSR_REGSEL_DBG_RET_ADDR (0x0f << 0)
#define DCRSR_REGSEL_XPSR (0x10 << 0)
#define DCRSR_REGSEL_MSP (0x11 << 0)
#define DCRSR_REGSEL_PSP (0x12 << 0)
#define DCRSR_REGSEL_CTRL (0x14 << 0)
#define DCRSR_REGSEL_FPSCR (0x21 << 0)
#define DCRSR_REGSEL_S0 (0x40 << 0)
#define DCRSR_REGSEL_S1 (0x41 << 0)
#define DCRSR_REGSEL_S2 (0x42 << 0)
#define DCRSR_REGSEL_S3 (0x43 << 0)
#define DCRSR_REGSEL_S4 (0x44 << 0)
#define DCRSR_REGSEL_S5 (0x45 << 0)
#define DCRSR_REGSEL_S6 (0x46 << 0)
#define DCRSR_REGSEL_S7 (0x47 << 0)
#define DCRSR_REGSEL_S8 (0x48 << 0)
#define DCRSR_REGSEL_S9 (0x49 << 0)
#define DCRSR_REGSEL_S10 (0x4a << 0)
#define DCRSR_REGSEL_S11 (0x4b << 0)
#define DCRSR_REGSEL_S12 (0x4c << 0)
#define DCRSR_REGSEL_S13 (0x4d << 0)
#define DCRSR_REGSEL_S14 (0x4e << 0)
#define DCRSR_REGSEL_S15 (0x4f << 0)
#define DCRSR_REGSEL_S16 (0x50 << 0)
#define DCRSR_REGSEL_S17 (0x51 << 0)
#define DCRSR_REGSEL_S18 (0x52 << 0)
#define DCRSR_REGSEL_S19 (0x53 << 0)
#define DCRSR_REGSEL_S20 (0x54 << 0)
#define DCRSR_REGSEL_S21 (0x55 << 0)
#define DCRSR_REGSEL_S22 (0x56 << 0)
#define DCRSR_REGSEL_S23 (0x57 << 0)
#define DCRSR_REGSEL_S24 (0x58 << 0)
#define DCRSR_REGSEL_S25 (0x59 << 0)
#define DCRSR_REGSEL_S26 (0x5a << 0)
#define DCRSR_REGSEL_S27 (0x5b << 0)
#define DCRSR_REGSEL_S28 (0x5c << 0)
#define DCRSR_REGSEL_S29 (0x5d << 0)
#define DCRSR_REGSEL_S30 (0x5e << 0)
#define DCRSR_REGSEL_S31 (0x5f << 0)


/* ----------------------------------------------------------------------- */
/* Debug Exception and Monitor Control Register, DEMCR                     */ 
/* ----------------------------------------------------------------------- */

#define DEMCR_TRCENA (1 << 24) /* Global enable for all DWT and ITM features:
	0 = DWT and ITM blocks disabled.
	1 = DWT and ITM blocks enabled.
	If the DWT and ITM blocks are not implemented, this bit is UNK/SBZP.
	When TRCENA is set to 0:
	• DWT registers return UNKNOWN values on reads. Whether the processor
	ignores writes to the DWT unit is IMPLEMENTATION DEFINED.
	• ITM registers return UNKNOWN values on reads. Whether the processor
	ignores writes to the ITM unit is IMPLEMENTATION DEFINED.
	Setting this bit to 0 might not stop all events. To ensure all events 
	are stopped, software must set all DWT and ITM feature enable bits to 
	0, and then set this bit to 0.

	The effect of this bit on the TPIU, ETM, and other system trace 
	components is IMPLEMENTATION DEFINED. */


#define DEMCR_MON_REQ (1 << 19) /* DebugMonitor semaphore bit. The processor 
	does not use this bit. The monitor software defines the meaning and use 
	of this bit. */

#define DEMCR_MON_STEP (1 << 18) /* When MON_EN is set to 0, this feature is 
	disabled and the processor ignores MON_STEP. When MON_EN is set to 1, 
	the meaning of MON_STEP is:
	0 = Do not step the processor.
	1 = Step the processor.
	Setting this bit to 1 makes the step request pending. The request 
	becomes active when the processor returns from the DebugMonitor handler 
	to the code being debugged. The effect of setting this bit to 1 is 
	UNPREDICTABLE if the code being debugged is executing at an execution 
	priority that is lower than the priority of the DebugMonitor exception. 
	--- Note ---
	This is the debug monitor equivalent of DHCSR.C_STEP in Debug state. */

#define DEMCR_MON_PEND (1 << 17) /* Sets or clears the pending state of the 
	DebugMonitor exception:
	0 = Clear the status of the DebugMonitor exception to not pending.
	1 = Set the status of the DebugMonitor exception to pending.
	When the DebugMonitor exception is pending it becomes active subject to 
	the exception priority rules. A debugger can use this bit to wakeup the 
	monitor using the DAP.
	The effect of setting this bit to 1 is not affected by the value of the 
	MON_EN bit.
	A debugger can set MON_PEND to 1, and force the processor to take a
	DebugMonitor exception, even when MON_EN is set to 0. */

#define DEMCR_MON_EN (1 << 16) /* Enable the DebugMonitor exception:
	0 = DebugMonitor exception disabled.
	1 = DebugMonitor exception enabled,
	If DHCSR.C_DEBUGEN is set to 1, the processor ignores the value of 
	this bit. */

#define DEMCR_VC_HARDERR (1 << 10) /* Enable halting debug trap on a HardFault 
	exception.
	0 = Halting debug trap disabled.
	1 = Halting debug trap enabled.
	If DHCSR.C_DEBUGEN is set to 0, the processor ignores the value of 
	this bit. */

#define DEMCR_VC_INTERR (1 << 9) /* Enable halting debug trap on a fault 
	occurring during exception entry or exception return.
	0 = Halting debug trap disabled.
	1 = Halting debug trap enabled.
	If DHCSR.C_DEBUGEN is set to 0, the processor ignores the value of 
	this bit. */

#define DEMCR_VC_BUSERR (1 << 8) /* Enable halting debug trap on a BusFault 
	exception.
	0 = Halting debug trap disabled.
	1 = Halting debug trap enabled.
	If DHCSR.C_DEBUGEN is set to 0, the processor ignores the 
	value of this bit. */

#define DEMCR_VC_STATERR (1 << 7) /* Enable halting debug trap on a UsageFault 
	exception caused by a state information error, for example an 
	Undefined Instruction exception.
	0 = Halting debug trap disabled.
	1 = Halting debug trap enabled.
	If DHCSR.C_DEBUGEN is set to 0, the processor ignores the 
	value of this bit. */

#define DEMCR_VC_CHKERR (1 << 6) /* Enable halting debug trap on a UsageFault 
	exception caused by a checking error, for example an alignment check error.
	0 = Halting debug trap disabled.
	1 = Halting debug trap enabled.
	If DHCSR.C_DEBUGEN is set to 0, the processor ignores the 
	value of this bit. */

#define DEMCR_VC_NOCPERR (1 << 5) /* Enable halting debug trap on a UsageFault 
	caused by an access to a Coprocessor.
	0 = Halting debug trap disabled.
	1 = Halting debug trap enabled.
	If DHCSR.C_DEBUGEN is set to 0, the processor ignores the 
	value of this bit. */

#define DEMCR_VC_MMERR (1 << 4) /* Enable halting debug trap on a MemManage
	exception.
	0 = Halting debug trap disabled.
	1 = Halting debug trap enabled.
	If DHCSR.C_DEBUGEN is set to 0, the processor ignores the 
	value of this bit. */

#define DEMCR_VC_CORERESET (1 << 0) /* Enable Reset Vector Catch.
	This causes a Local reset to halt a running system.
	0 = Reset Vector Catch disabled.
	1 = Reset Vector Catch enabled.
	If DHCSR.C_DEBUGEN is set to 0, the processor ignores the 
	value of this bit. */


/* ----------------------------------------------------------------------- */
/* Configurable Fault Status Register, CFSR                                */ 
/* ----------------------------------------------------------------------- */

#define CFSR_USGFAULTSR (0xffff << 16)
#define CFSR_UFSR_GET(CFSR) (((CFSR) >> 16) & 0xffff)

#define UFSR_DIVBYZERO  (1 << 9)
#define UFSR_UNALIGNED  (1 << 8)
#define UFSR_NOCP       (1 << 3)
#define UFSR_INVPC      (1 << 2)
#define UFSR_INVSTATE   (1 << 1)
#define UFSR_UNDEFINSTR (1 << 0)

#define CFSR_BUSFAULTSR (0xff << 8)
#define CFSR_BFSR_GET(CFSR) (((CFSR) >> 8) & 0xff)

#define BFSR_BFARVALID   (1 << 7)
#define BFSR_LSPERR      (1 << 5)
#define BFSR_STKERR      (1 << 4)
#define BFSR_UNSTKERR    (1 << 3)
#define BFSR_IMPRECISERR (1 << 2)
#define BFSR_PRECISERR   (1 << 1)
#define BFSR_IBUSERR     (1 << 0)

#define CFSR_MMFSR (0xff << 0)
#define CFSR_MMFSR_GET(CFSR) (((CFSR) >> 0) & 0xff)

/* MemManage Status Register, MMFSR */

#define MMFSR_MMARVALID (1 << 7) 
/*	0 = MMAR does not have valid contents.
	1 = MMAR has valid contents. */

#define MMFSR_MLSPERR (1 << 5) 
/*	0 = No MemManage fault occurred during FP lazy state preservation
	1 = A MemManage fault occurred during FP lazy state preservation */

#define MMFSR_MSTKERR (1 << 4) 
/*	0 = No derived MemManage fault has occurred.
	1 = A derived MemManage fault occurred on exception entry. */

#define MMFSR_MUNSTKERR (1 << 3) 
/*	0 = No derived MemManage fault has occurred.
	1 = A derived MemManage fault occurred on exception return. */

#define MMFSR_DACCVIOL (1 << 1)
/*	0 = No data access violation has occurred.
	1 = Data access violation. The MMAR shows the data address that the 
	load or store tried to access. */

#define MMFSR_IACCVIOL (1 << 0)
/*	0 = No MPU or Execute Never (XN) default memory map access violation 
	has occurred.
	1 = MPU or Execute Never (XN) default memory map access violation on 
	an instruction fetch has occurred. The fault is signalled only if 
	the instruction is issued. */

#ifdef __ASSEMBLER__

#else /* __ASSEMBLER__ */

#include "jtag_adi.h"

struct armv7m_map {
	uint32_t itm;
	uint32_t dwt;
	uint32_t fpb;
	uint32_t scb;
	uint32_t dcb;
	uint32_t tpiu;
	uint32_t etm;
	uint32_t rom;
};

#ifdef __cplusplus
extern "C" {
#endif

int armv7m_probe(jtag_tap_t * tap, struct armv7m_map * map);

#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* __ARM_V7M_H__ */

