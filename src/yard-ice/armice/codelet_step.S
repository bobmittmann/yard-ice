/*
 * File:    codelet_step.S
 * Module:  
 * Project: 
 * Author:	Robinson Mittmann (bobmittmann@gmail.com)
 * Target:	
 * Comment:
 * Copyright(c) 2010 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#include <arch/at91sam9260.h>
#include "armice.h" 

	.text
	.arm
	.align	0


codelet_step:
	.global codelet_step
	.type   codelet_step, %function
	.word   .L_step_end - .L_step_begin
.L_step_begin:
	mov     r0, #1
	mov     r0, #2
	mov     r0, #3
	mov     r0, #4
	mov     r0, #5
	mov     r0, #6
	mov     r0, #7
	mov     r0, #8
	mov     r0, #9
	mov     r0, #10
	mov     r0, #11
	mov     r0, #12
	mov     r0, #13
	mov     r0, #14
	mov     r0, #15
	mov     r0, #16
	mov     r0, #17
	mov     r0, #18
	mov     r0, #19
	mov     r0, #20
	mov     r0, #21
	mov     r0, #22
	mov     r0, #23
	mov     r0, #24
	mov     r0, #25
	mov     r0, #26
	mov     r0, #27
	mov     r0, #28
	mov     r0, #29
	mov     r0, #30
	mov     r0, #31
	mov     r0, #32
	mov     r0, #33
	mov     r0, #34
	mov     r0, #35
	mov     r0, #36
	mov     r0, #37
	mov     r0, #38
	mov     r0, #39
	mov     r0, #40
	mov     r0, #41
	mov     r0, #42
	mov     r0, #43
	mov     r0, #44
	mov     r0, #45
	mov     r0, #46
	mov     r0, #47
	mov     r0, #48
	mov     r0, #49
	mov     r0, #50
	b       .L_step_begin

.L_step_end:
	.size  codelet_step, . - codelet_step


codelet_step2:
	.global codelet_step2
	.type   codelet_step2, %function
	.word   .L_step2_end - .L_step2_begin
.L_step2_begin:
.L_reset:
	b    .L_start
.L_vec_undef:		
	mov  r8, #0x00011000
.L_vec_swi:		
	mov  r9, #0x00022000
.L_vec_pabt:		
	mov  r10, #0x00033000
.L_vec_dabt:
	mov  r11, #0x00044000
.L_step2_except:
	nop
.L_vec_irq: 
	nop
.L_vec_fiq:
	b   .L_step2_except

.L_start:
	mov     r2, #0x2
	mov     r3, #0x3
	mov     r4, #0x4
	mov     r5, #0x5
	mov     r6, #0x6

	mov     r0, #0
	mov     r1, #0
.L_step2_loop:
	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1

	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1

	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1

	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1

	add     r1, r1, r0
	mov     r0, #0
	b       .L_step2_loop

.L_step2_end:
	.size  codelet_step2, . - codelet_step2

	.arm

codelet_step_thumb:
	.global codelet_step_thumb
	.type   codelet_step_thumb, %function
	.word   .L_step_thumb_end - .L_step_thumb_begin
.L_step_thumb_begin:
	b    .L_step_thumb_start
	mov  r8, #0x00011000
	mov  r9, #0x00022000
	mov  r10, #0x00033000
	mov  r11, #0x00044000
.L_step_thumb_except:
	nop
	nop
	b   .L_step_thumb_except

.L_step_thumb_start:
	mov     r2, #0x2
	mov     r3, #0x3
	mov     r4, #0x4
	mov     r5, #0x5
	mov     r6, #0x6

	mov     r0, #0
	mov     r1, #0

	add     lr, pc, #1
	bx      lr

	.thumb

.L_step_thumb_loop:
	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1

	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1

	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1

	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1

	add     r1, r1, r0
	mov     r0, #0
	b       .L_step_thumb_loop

.L_step_thumb_end:
	.size  codelet_step_thumb, . - codelet_step_thumb


	.arm
codelet_step_thumb_arm:
	.global codelet_step_thumb_arm
	.type   codelet_step_thumb_arm, %function
	.word   .L_step_thumb_arm_end - .L_step_thumb_arm_begin
.L_step_thumb_arm_begin:
	b    .L_step_thumb_arm_start
	mov  r8, #0x00011000
	mov  r9, #0x00022000
	mov  r10, #0x00033000
	mov  r11, #0x00044000
.L_step_thumb_arm_except:
	nop
	nop
	b   .L_step_thumb_arm_except

.L_step_thumb_arm_start:
	mov     r2, #0x2
	mov     r3, #0x3
	mov     r4, #0x4
	mov     r5, #0x5
	mov     r6, #0x6

	mov     r0, #0
	mov     r1, #0

.L_step_thumb_arm_loop:
	add     r0, r0, #1
	add     r0, r0, #1

	add     lr, pc, #1
	bx      lr

	.thumb

	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1
	add     r0, r0, #1

	mov     lr, pc
	bx      lr

	.arm

	add     r0, r0, #1
	add     r0, r0, #1
	b       .L_step_thumb_arm_loop


.L_step_thumb_arm_end:
	.size  codelet_step_thumb_arm, . - codelet_step_thumb_arm


