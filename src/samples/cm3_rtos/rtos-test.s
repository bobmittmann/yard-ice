	.syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 18, 4
	.thumb
	.file	"rtos-test.c"
	.text
	.align	2
	.global	busy_task
	.thumb
	.thumb_func
	.type	busy_task, %function
busy_task:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r3, #0
	str	r3, [r0, #0]
.L2:
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	ldr	r3, [r0, #0]
	adds	r3, r3, #1
	str	r3, [r0, #0]
	b	.L2
	.size	busy_task, .-busy_task
	.align	2
	.global	yield_task
	.thumb
	.thumb_func
	.type	yield_task, %function
yield_task:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r3, #60672
	movt	r3, 57344
	mov	r2, #268435456
.L5:
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	str	r2, [r3, #4]
@ 67 "cm3rtos.h" 1
	dsb

@ 0 "" 2
	.thumb
	ldr	r1, [r0, #0]
	adds	r1, r1, #16
	str	r1, [r0, #0]
	b	.L5
	.size	yield_task, .-yield_task
	.align	2
	.global	speed_test
	.thumb
	.thumb_func
	.type	speed_test, %function
speed_test:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r9, sl, lr}
	ldr	r0, .L11
	sub	sp, sp, #32
	bl	puts
	ldr	r0, .L11+4
	ldr	r1, .L11+8
	ldr	r2, .L11+12
	mov	r3, #512
	mov	ip, #1
@ 60 "cm3rtos.h" 1
	svc 2

@ 0 "" 2
	.thumb
	str	r0, [sp, #16]
	mov	r6, r0
	adds	r1, r1, #4
	ldr	r0, .L11+4
	adds	r2, r2, r3
@ 60 "cm3rtos.h" 1
	svc 2

@ 0 "" 2
	.thumb
	mov	ip, #2
	str	r0, [sp, #20]
	mov	r5, r0
	adds	r1, r1, #4
	ldr	r0, .L11+4
	adds	r2, r2, r3
@ 60 "cm3rtos.h" 1
	svc 2

@ 0 "" 2
	.thumb
	str	r0, [sp, #24]
	mov	r4, r0
	adds	r1, r1, #4
	ldr	r0, .L11+4
	adds	r2, r2, r3
@ 60 "cm3rtos.h" 1
	svc 2

@ 0 "" 2
	.thumb
	mov	r7, r0
	mov	r2, r5
	mov	r3, r4
	mov	r1, r6
	ldr	r0, .L11+16
	str	r7, [sp, #28]
	str	r7, [sp, #0]
	bl	printf
	ldr	r4, .L11+8
	movs	r5, #5
.L7:
	ldr	r6, [r4, #0]
	mov	r0, #1000
	ldr	r1, [r4, #4]
	ldr	r2, [r4, #8]
	ldr	r3, [r4, #12]
@ 45 "cm3rtos.h" 1
	svc 4

@ 0 "" 2
	.thumb
	ldr	r8, [r4, #0]
	ldr	r7, [r4, #4]
	ldr	r0, [r4, #8]
	subs	r7, r7, r1
	ldr	r1, [r4, #12]
	rsb	r8, r6, r8
	rsb	r9, r3, r1
	subs	r6, r0, r2
	bl	cm3rtos_idle_val
	add	sl, r8, r7
	add	sl, r6, sl
	str	r0, [sp, #8]
	add	sl, r9, sl
	ldr	r0, .L11+20
	mov	r1, r8
	mov	r2, r7
	mov	r3, r6
	stmia	sp, {r9, sl}
	bl	printf
	subs	r5, r5, #1
	bne	.L7
	add	r5, sp, #16
	movs	r4, #30
.L8:
	ldr	r0, [r5, #0]
	mov	r1, r4
@ 53 "cm3rtos.h" 1
	svc 3

@ 0 "" 2
	.thumb
	ldr	r6, [r5], #4
	ldr	r0, .L11+24
	mov	r1, r6
	bl	printf
	mov	r0, r6
@ 49 "cm3rtos.h" 1
	svc 5

@ 0 "" 2
	.thumb
	mov	r1, r0
	adds	r4, r4, #1
	ldr	r0, .L11+28
	bl	printf
	cmp	r4, #34
	bne	.L8
	movs	r0, #10
	add	sp, sp, #32
	pop	{r4, r5, r6, r7, r8, r9, sl, lr}
	b	putchar
.L12:
	.align	2
.L11:
	.word	.LC0
	.word	yield_task
	.word	arg
	.word	stack
	.word	.LC1
	.word	.LC2
	.word	.LC3
	.word	.LC4
	.size	speed_test, .-speed_test
	.align	2
	.global	sleep_test
	.thumb
	.thumb_func
	.type	sleep_test, %function
sleep_test:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movw	r0, #:lower16:.LC5
	push	{r3, lr}
	movt	r0, #:upper16:.LC5
	bl	puts
	mov	r0, #1000
@ 45 "cm3rtos.h" 1
	svc 4

@ 0 "" 2
	.thumb
	movw	r0, #:lower16:.LC6
	movt	r0, #:upper16:.LC6
	pop	{r3, lr}
	b	puts
	.size	sleep_test, .-sleep_test
	.align	2
	.global	busy_test
	.thumb
	.thumb_func
	.type	busy_test, %function
busy_test:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
	ldr	r0, .L19
	sub	sp, sp, #36
	bl	puts
	ldr	r0, .L19+4
	ldr	r1, .L19+8
	ldr	r2, .L19+12
	mov	r3, #512
	mov	ip, #2
@ 60 "cm3rtos.h" 1
	svc 2

@ 0 "" 2
	.thumb
	mov	ip, #4
	str	r0, [sp, #16]
	mov	r6, r0
	adds	r1, r1, #4
	ldr	r0, .L19+4
	adds	r2, r2, r3
@ 60 "cm3rtos.h" 1
	svc 2

@ 0 "" 2
	.thumb
	str	r0, [sp, #20]
	mov	r5, r0
	add	r1, r1, ip
	ldr	r0, .L19+4
	adds	r2, r2, r3
	mov	ip, #6
@ 60 "cm3rtos.h" 1
	svc 2

@ 0 "" 2
	.thumb
	mov	ip, #8
	str	r0, [sp, #24]
	mov	r4, r0
	adds	r1, r1, #4
	ldr	r0, .L19+4
	adds	r2, r2, r3
@ 60 "cm3rtos.h" 1
	svc 2

@ 0 "" 2
	.thumb
	mov	r7, r0
	mov	r3, r4
	mov	r1, r6
	ldr	r0, .L19+16
	mov	r2, r5
	str	r7, [sp, #28]
	str	r7, [sp, #0]
	bl	printf
	ldr	r4, .L19+8
	mov	fp, #8
.L15:
	ldr	r6, [r4, #0]
	mov	r0, #1000
	ldr	r2, [r4, #4]
	ldr	r3, [r4, #8]
	ldr	r1, [r4, #12]
@ 45 "cm3rtos.h" 1
	svc 4

@ 0 "" 2
	.thumb
	ldr	r8, [r4, #0]
	ldr	r7, [r4, #4]
	ldr	r0, [r4, #8]
	ldr	r5, [r4, #12]
	rsb	r9, r2, r7
	subs	r5, r5, r1
	movs	r2, #100
	rsb	r8, r6, r8
	rsb	sl, r3, r0
	mul	r6, r2, r5
	bl	cm3rtos_idle_val
	add	r7, r8, r9
	add	r7, sl, r7
	mov	lr, r0
	adds	r7, r5, r7
	mov	r1, r8
	mov	r2, r9
	mov	r3, sl
	ldr	r0, .L19+20
	stmia	sp, {r5, r7, lr}
	bl	printf
	sdiv	r1, r6, r8
	sdiv	r2, r6, r9
	sdiv	r3, r6, sl
	sdiv	r5, r6, r5
	ldr	r0, .L19+24
	str	r5, [sp, #0]
	bl	printf
	subs	fp, fp, #1
	bne	.L15
	add	r4, sp, #16
.L16:
	ldr	r0, [r4, #0]
	add	r1, fp, #20
@ 53 "cm3rtos.h" 1
	svc 3

@ 0 "" 2
	.thumb
	ldr	r0, .L19+28
	mov	r1, fp
	bl	printf
	ldr	r0, [r4], #4
@ 49 "cm3rtos.h" 1
	svc 5

@ 0 "" 2
	.thumb
	mov	r1, r0
	add	fp, fp, #1
	ldr	r0, .L19+32
	bl	printf
	cmp	fp, #4
	bne	.L16
	movs	r0, #10
	add	sp, sp, #36
	pop	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
	b	putchar
.L20:
	.align	2
.L19:
	.word	.LC7
	.word	busy_task
	.word	arg
	.word	stack
	.word	.LC1
	.word	.LC2
	.word	.LC8
	.word	.LC3
	.word	.LC4
	.size	busy_test, .-busy_test
	.section	.text.startup,"ax",%progbits
	.align	2
	.global	main
	.thumb
	.thumb_func
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	bl	cm3_udelay_calibrate
	mov	r0, #20480
	mov	r1, #115200
	movs	r2, #2
	movt	r0, 16384
	bl	stm32f_usart_open
	movw	r3, #:lower16:stdout
	mov	r2, r0
	movt	r3, #:upper16:stdout
	movw	r4, #:lower16:.LC9
	str	r2, [r3, #0]
	movt	r4, #:upper16:.LC9
	movs	r0, #10
	bl	putchar
	mov	r0, r4
	bl	puts
	movw	r0, #:lower16:.LC10
	movt	r0, #:upper16:.LC10
	bl	puts
	mov	r0, r4
	bl	puts
	movs	r0, #10
	bl	putchar
	bl	cm3rtos_init
	bl	sleep_test
	bl	speed_test
	bl	busy_test
	movw	r0, #10000
@ 45 "cm3rtos.h" 1
	svc 4

@ 0 "" 2
	.thumb
	movs	r0, #0
	pop	{r4, pc}
	.size	main, .-main
	.comm	arg,16,4
	.comm	stack,2048,4
	.comm	stm32f_usart5_file,8,4
	.comm	stm32f_usart_ops,16,4
	.section	.rodata.str1.4,"aMS",%progbits,1
	.align	2
.LC0:
	.ascii	" - Scheduler speed test\000"
.LC1:
	.ascii	"  * Threads: %d, %d, %d, %d\012\000"
	.space	3
.LC2:
	.ascii	" %8d + %8d + %8d + %8d = %8d (%d idle)\012\000"
.LC3:
	.ascii	"  * join(%d) ...\000"
	.space	3
.LC4:
	.ascii	" %d\012\000"
	.space	3
.LC5:
	.ascii	" - sleep(1000)\000"
	.space	1
.LC6:
	.ascii	" - wakeup\000"
	.space	2
.LC7:
	.ascii	" - Scheduler test\000"
	.space	2
.LC8:
	.ascii	" %d%% %d%% %d%% %d%%\012\000"
	.space	2
.LC9:
	.ascii	"---------------------------------------------------"
	.ascii	"------\000"
	.space	2
.LC10:
	.ascii	" Cortex M3 RTOS Test\000"
	.ident	"GCC: (GNU) 4.6.2"
