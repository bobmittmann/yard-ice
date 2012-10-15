/*
 * File:    codelet_putchar.S
 * Module:  
 * Project: 
 * Author:	Robinson Mittmann (bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2005-2010 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

//#include <arch/at91x40.h>
#include <arch/at91sam9260.h>
#include "armice.h" 

	.text
	.arm
	.align	0


codelet_putchar:
	.global codelet_putchar
	.word   .L_putchar_end - .L_putchar_begin
.L_putchar_begin:
	add     r1, r1, #1
	add     r1, r1, #1
	add     r1, r1, #1
	add     r1, r1, #1
	add     r1, r1, #1

	ldr     r12, .L_at91_base_us
	mov     r7, #0

	mov     r0, #' '
	bl      .L_putc
	mov     r0, #'.'
	bl      .L_putc
	mov     r0, r1
	bl      .L_putc
	mov     r0, #' '
	bl      .L_putc
	mov     r0, #'H'
	bl      .L_putc
	mov     r0, #'e'
	bl      .L_putc
	mov     r0, #'l'
	bl      .L_putc
	mov     r0, #'l'
	bl      .L_putc
	mov     r0, #'o'
	bl      .L_putc
	mov     r0, #'!'
	bl      .L_putc
	mov     r0, #'\r'
	bl      .L_putc
	mov     r0, #'\n'
	bl      .L_putc


0:
	mov     r8, #1024 

1:  mrc     14, 0, r11, cr0, cr0
	tst     r11, #ARMICE_DCC_READ_FLAG
	beq     1b
	mrc     14, 0, r0, cr1, cr0

	subs    r8, r8, #1
	bne     1b

	add     r0, r7, #'0'
	bl      .L_putc

	add     r7, r7, #1
	cmp     r7, #10
	moveq   r7, #0

	b       0b

.L_putc:
2:
	ldr     r11, [r12, #US_CSR]
	tst     r11, #US_TXRDY
	beq     2b
	str     r0, [r12, #US_THR]
	mov     pc, lr

	.align  0
.L_at91_base_us:
	.word	AT91_BASE_DBGU

.L_putchar_end:
	.size  codelet_putchar, . - codelet_putchar

	.end

