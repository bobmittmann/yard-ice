/* 
 * crt0.S
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

	.align  0
	.thumb
	.section .init

	.thumb_func
_reset:
	.global _reset
	.type   _reset, %function

	b       0f
	.byte   '\r'
	.byte   '\n'
	.string "ThinkOS"
0:
	/* jump to hardware initialization code */
	/* disable interrupts */
    mov     r8, r0
    mov     r9, r1
	bl		_init

	/* copy initialized variables .data section  (Copy from ROM to RAM) */
	ldr     r2, .L_data_start
	ldr     r3, .L_data_end
	ldr     r4, .L_idata_start
1:
	/* while the .data pointer is less then the .bss
	   copy from .idata to .data */
	cmp     r2, r3
	ittt	lo
	ldrlo   r7, [r4], #4
	strlo   r7, [r2], #4
	blo     1b

	/* clear .bss  */
	ldr     r2, .L_bss_start
	ldr     r3, .L_bss_end
	mov     r7, #0
2:
	cmp     r2, r3
	itt		lo
	strlo   r7, [r2], #4
	blo     2b

	/* call constructors */
	ldr     r5, .L_ctors_start
	ldr     r6, .L_ctors_end
	.L_ctor_loop:
	cmp     r5, r6
	beq     .L_ctor_end
	ldr     r4, [r5], #4
	blx     r4
	b       .L_ctor_loop
	.L_ctor_end:

    mov     fp, #0
	mov     r7, #0
	/* argc  */
	mov     r0, r8
	/* argv = NULL */
	mov     r1, r9
	/* go to C main function */
	bl      main
	b       abort

/* section of initialized data at ROM */
.L_idata_start:
	.word	__text_end
/* section of initialized data at RAM */
.L_data_start:
	.word	__data_start
.L_data_end:
/* end of initialized data */
	.word	__data_end	
.L_bss_start:
	.word	__bss_start
.L_bss_end:
	.word	__bss_end
.L_heap_start:
	.word	__heap_start
.L_ctors_start:
	.word   __ctors_start
.L_ctors_end:
	.word   __ctors_end

	.size   _reset, . - _reset

	.text
	.align  2
	.global _halt
	.thumb
	.thumb_func
	.type   _halt, %function
_halt:
    mov     r0, #1
2:
	cmp     r0, #0
	bne     2b
	.size   _halt, . - _halt

	.align  2
	.global	_void
	.thumb
	.thumb_func
	.type   _void, %function
_void:
	bx      lr
	.size   _void, . - _void

	.weak       _init
	.thumb_set  _init,_void

	.weak       halt
	.thumb_set  halt,_halt

	.weak       abort
	.thumb_set  abort,_halt

	.end

