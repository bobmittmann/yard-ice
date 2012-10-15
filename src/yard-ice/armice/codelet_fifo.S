/*
 * File:    codelet_fifo.S
 * Author:  Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
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


#include "armice.h" 

	.text
	.arm
	.align	0

/*
	r0 = FIFO address
	r1 = Data length
*/

codelet_fifo_write:
	.global codelet_fifo_write
	.word   .L_nand_write_end - .L_nand_write_begin
.L_nand_write_begin:
.L_write_again:
	cmp     r1, #0
	beq     .L_write_again
	/* wait for data to write */
1:  mrc     14, 0, r5, cr0, cr0
	tst     r5, #ARMICE_DCC_READ_FLAG
	beq     1b
	mrc     14, 0, r5, cr1, cr0

	strb    r5, [r0]
	subs    r1, r1, #1

	lsrne   r5, r5, #8
	strneb  r5, [r0]
	subnes  r1, r1, #1

	lsrne   r5, r5, #8
	strneb  r5, [r0]
	subnes  r1, r1, #1

	lsrne   r5, r5, #8
	strneb  r5, [r0]
	subnes  r1, r1, #1

	b     .L_write_again

.L_nand_write_end:
	.size  codelet_fifo_write, . - codelet_fifo_write


codelet_fifo_read:
	.global codelet_fifo_read
	.word   .L_nand_read_end - .L_nand_read_begin
.L_nand_read_begin:

.L_read_again:
	cmp     r1, #0
	beq     .L_read_again

	ldrb    r5, [r0]
	subs    r1, r1, #1

	ldrneb  r6, [r0]
	orrne   r5, r5, r6, lsl #8
	subnes  r1, r1, #1

	ldrneb  r7, [r0]
	orrne   r5, r5, r7, lsl #16
	subnes  r1, r1, #1

	ldrneb  r8, [r0]
	orrne   r5, r5, r8, lsl #24
	subnes  r1, r1, #1

2:  mrc     14, 0, r4, cr0, cr0
	tst     r4, #ARMICE_DCC_WRITE_FLAG
	bne     2b
	mcr     14, 0, r5, cr1, cr0

	b      .L_read_again

.L_nand_read_end:
	.size  codelet_fifo_read, . - codelet_fifo_read

	.end

