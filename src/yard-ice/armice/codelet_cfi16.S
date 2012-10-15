/*
 * File:    codelet_cfi16.S
 * Module:  
 * Project: 
 * Author:	Robinson Mittmann (bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2005-2010 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include "armice.h" 

	.text
	.arm
	.align	0

/*
	r0 = base
	r1 = offs
	r2 = data
	r3 = scratch
	r4 = 0x555 << 1
	r5 = 0x2aa << 1
	r6 = 0xaa
	r7 = 0x55
	r8 = 0xa0
*/

codelet_cfi16_write:
	.global codelet_cfi16_write
	.word   .L_cfi16_write_end - .L_cfi16_write_begin
.L_cfi16_write_begin:
	/* cancel any pending request */
	mov     r3, #0xf0
	strh    r3, [r0, r1]

	mov     r4, #(0x550 << 1)
	add     r4, r4, #(0x005 << 1)
	mov     r5, #(0xaa0 << 1)
	add     r5, r5, #(0x00a << 1)
	mov 	r6, #0xaa
	mov 	r7, #0x55
	mov 	r8, #0xa0

.L_again:

	/* wait for data to program */
1:  mrc     14, 0, r11, cr0, cr0
	tst     r11, #ARMICE_DCC_READ_FLAG
	beq     1b
	mrc     14, 0, r9, cr1, cr0

	/* low word */
	mov     r2, r9, lsl #16
	mov     r2, r2, lsr #16
	bl      .L_word_program
	/* offset increment */
	add     r1, r1, #2

	/* high word */
	mov     r2, r9, lsr #16
	bl      .L_word_program
	/* offset increment */
	add     r1, r1, #2
	b       .L_again

	/* word program command */
.L_word_program:
	strh    r6, [r0, r4]
	strh    r7, [r0, r5]
	strh    r8, [r0, r4]

	strh    r2, [r0, r1]
.L_wp_toggle:
	ldrh    r3, [r0, r1]
	ldrh    r2, [r0, r1]
	eor     r3, r3, r2
	tst     r3, #(1 << 6)
	bne     .L_wp_toggle
	tst     r2, #(1 << 5) | (1 << 3)
	moveq   pc, lr
	ldrh    r3, [r0, r1]
	ldrh    r2, [r0, r1]
	eor     r3, r3, r2
	tst     r3, #(1 << 6)
	moveq   pc, lr

	mov     r3, #0xf0
	strh    r3, [r0, r1]

	mov     r0, #-1
	mcr     14, 0, r0, cr1, cr0
	b       .

.L_cfi16_write_end:
	.size  codelet_cfi16_write, . - codelet_cfi16_write



	.arm
	.align	0
codelet_cfi16_erase:
	.global codelet_cfi16_erase
	.word   .L_cfi16_erase_end - .L_cfi16_erase_begin
.L_cfi16_erase_begin:

	mov     r3, #0xf0
	strh    r3, [r0, r1]

	mov     r4, #(0x550 << 1)
	add     r4, r4, #(0x005 << 1)
	mov     r5, #(0xaa0 << 1)
	add     r5, r5, #(0x00a << 1)
	mov 	r6, #0xaa
	mov 	r7, #0x55
	mov 	r8, #0x80
	mov 	r9, #0x30

	subs    r10, r2, #0
	ble     .L_erase_done

.L_erase_loop:
	ldrsh   r3, [r0, r1]
	cmp     r3, #0xffffffff
	blne    .L_block_erase

	add     r1, r1, #2
	subs    r10, r10, #2
	bgt     .L_erase_loop

.L_erase_done:
	mov     r11, #'1'
	mcr     14, 0, r11, cr1, cr0
	b       .L_erase_done

	/* word program command */
.L_block_erase:
	strh    r6, [r0, r4]
	strh    r7, [r0, r5]
	strh    r8, [r0, r4]
	strh    r6, [r0, r4]
	strh    r7, [r0, r5]
	strh    r9, [r0, r1]

.L_erase_toggle:
	ldrh    r3, [r0, r1]
	ldrh    r2, [r0, r1]
	eor     r3, r2
	tst     r3, #(1 << 6)
	bne     .L_erase_toggle
	tst     r2, #(1 << 5) | (1 <<3)
	moveq   pc, lr
	ldrh    r3, [r0, r1]
	ldrh    r2, [r0, r1]
	eor     r3, r2
	tst     r3, #(1 << 6)
	moveq   pc, lr

	mov     r3, #0xf0
	strh    r3, [r0, r1]

2:
	mov     r11, #-1
	mcr     14, 0, r11, cr1, cr0
	b       2b

.L_cfi16_erase_end:
	.size  codelet_cfi16_erase, . - codelet_cfi16_erase

	.end

