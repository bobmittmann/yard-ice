/*
 * File:    codelet_nand.S
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
	r0 = NAND data ptr
*/

codelet_nand_write:
	.global codelet_nand_write
	.word   .L_nand_write_end - .L_nand_write_begin
.L_nand_write_begin:
.L_write_again:
	/* wait for data to write */
1:  mrc     14, 0, r5, cr0, cr0
	tst     r5, #ARMICE_DCC_READ_FLAG
	beq     1b
	mrc     14, 0, r5, cr1, cr0

	strb    r5, [r0]
	lsr     r5, r5, #8
	strb    r5, [r0]
	lsr     r5, r5, #8
	strb    r5, [r0]
	lsr     r5, r5, #8
	strb    r5, [r0]

	b     .L_write_again

.L_nand_write_end:
	.size  codelet_nand_write, . - codelet_nand_write



codelet_nand_read:
	.global codelet_nand_read
	.word   .L_nand_read_end - .L_nand_read_begin
.L_nand_read_begin:

.L_read_again:
	ldrb    r5, [r0]
	ldrb    r6, [r0]
	ldrb    r7, [r0]
	ldrb    r8, [r0]
	orr     r5, r5, r6, lsl #8
	orr     r5, r5, r7, lsl #16
	orr     r5, r5, r8, lsl #24

2:  mrc     14, 0, r4, cr0, cr0
	tst     r4, #ARMICE_DCC_WRITE_FLAG
	bne     2b
	mcr     14, 0, r5, cr1, cr0

	subs    r1, r1, #4
	bgt     .L_read_again

	b       .

.L_nand_read_end:
	.size  codelet_nand_read, . - codelet_nand_read

	.end

