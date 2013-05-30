/* 
 * stm32f-vec.S
 *
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

	.syntax unified
	.cpu cortex-m3

	.section .rom_vectors, "ax", %progbits

	.align  2
	.thumb
	.thumb_func
	.global _rom_vectors
//	.type _rom_vectors, %object
	.type _rom_vectors, %function
_rom_vectors:

	.word	_stack
	.word   _reset
	.word   _nmi_isr
	.word   _hard_fault_isr
	.word   _mem_manage_isr
	.word   _bus_fault_isr
	.word   _usage_fault_isr
	.word   0

	.word   0
	.word   0 
	.word   0
	.word   _svc_isr
	.word   _debug_mon_isr
	.word   0
	.word   _pendsv_isr
	.word   _systick_isr

/*
	.word   _irq0_isr
	.word   _irq1_isr
	.word   _irq2_isr
	.word   _irq3_isr
	.word   _irq4_isr
	.word   _irq5_isr
	.word   _irq6_isr
	.word   _irq7_isr

	.word   _irq8_isr
	.word   _irq9_isr
	.word   _irq10_isr
	.word   _irq11_isr
	.word   _irq12_isr
	.word   _irq13_isr
	.word   _irq14_isr
	.word   _irq15_isr

	.word   _irq16_isr
	.word   _irq17_isr
	.word   _irq18_isr
	.word   _irq19_isr
	.word   _irq20_isr
	.word   _irq21_isr
	.word   _irq22_isr
	.word   _irq23_isr

	.word   _irq24_isr
	.word   _irq25_isr
	.word   _irq26_isr
	.word   _irq27_isr
	.word   _irq28_isr
	.word   _irq29_isr
	.word   _irq30_isr
	.word   _irq31_isr
*/
	.size   _rom_vectors, . - _rom_vectors

	.end

