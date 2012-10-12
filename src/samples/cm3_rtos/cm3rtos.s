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
	.file	"cm3rtos.c"
	.text
	.align	2
	.global	cm3rtos_idle_task
	.thumb
	.thumb_func
	.type	cm3rtos_idle_task, %function
cm3rtos_idle_task:
	@ Naked Function: prologue and epilogue provided by programmer.
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
.L2:
@ 106 "cm3rtos.c" 1
	ldr  r12, [lr, #0]

@ 0 "" 2
	.thumb
	b	.L2
	.size	cm3rtos_idle_task, .-cm3rtos_idle_task
	.align	2
	.thumb
	.thumb_func
	.type	cm3rtos_thread_exit, %function
cm3rtos_thread_exit:
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r5, .L10
	push	{r3, r7, fp, lr}
	ldr	r4, [r5, #136]
	mov	r6, r0
	asrs	r4, r4, #1
@ 558 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	cpsid i

@ 0 "" 2
	.thumb
	ldr	r1, .L10+4
	mov	r2, r4
	mov	r3, r6
	ldr	r0, .L10+8
	bl	printf
	add	r7, r4, #84
	ldr	r2, .L10+12
	add	r7, r5, r7, lsl #2
	ldr	r9, .L10+16
	lsls	r2, r2, #5
	movs	r0, #0
	add	r7, r7, #17825792
	str	r0, [r2, r4, lsl #2]
	subs	r3, r2, #4
	mov	r1, r5
	add	sl, r5, #8
	rsb	r7, r2, r7, lsl #5
	rsb	r9, r2, r9, lsl #5
	mov	r8, r5
	mov	ip, #1
	rsb	lr, r2, r5
.L6:
	ldr	r2, [r3, #4]!
	cbz	r2, .L4
	ldrb	r2, [r1, #148]	@ zero_extendqisi2
	ldr	fp, [r5, #144]
	cmp	fp, r2
	it	cc
	strcc	r2, [r8, #144]
	ldr	r2, [r7, r3]
	cbz	r2, .L4
	str	ip, [r9, r3]
	str	r0, [r7, r3]
	ldr	r2, [lr, r3]
	str	r6, [r2, #32]
.L4:
	adds	r1, r1, #1
	cmp	r1, sl
	bne	.L6
	mov	r3, #60672
	lsls	r4, r4, #1
	movt	r3, 57344
	mov	r2, #268435456
	str	r4, [r5, #136]
	str	r2, [r3, #4]
@ 562 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	cpsie i

@ 0 "" 2
	.thumb
.L7:
	b	.L7
.L11:
	.align	2
.L10:
	.word	cm3rtos_rt
	.word	.LANCHOR0
	.word	.LC0
	.word	cm3rtos_rt+17826160
	.word	cm3rtos_rt+17825924
	.size	cm3rtos_thread_exit, .-cm3rtos_thread_exit
	.align	2
	.global	cm3rtos_idle_val
	.thumb
	.thumb_func
	.type	cm3rtos_idle_val, %function
cm3rtos_idle_val:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movw	r3, #:lower16:cm3rtos_idle
	movt	r3, #:upper16:cm3rtos_idle
	ldr	r0, [r3, #256]
	bx	lr
	.size	cm3rtos_idle_val, .-cm3rtos_idle_val
	.align	2
	.global	cm3rtos_wq_insert
	.thumb
	.thumb_func
	.type	cm3rtos_wq_insert, %function
cm3rtos_wq_insert:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L14
	adds	r0, r0, #52
	add	r0, r3, r0, lsl #2
	lsls	r0, r0, #5
	movs	r3, #1
	str	r3, [r0, r1, lsl #2]
	bx	lr
.L15:
	.align	2
.L14:
	.word	cm3rtos_rt+17825792
	.size	cm3rtos_wq_insert, .-cm3rtos_wq_insert
	.align	2
	.global	cm3rtos_wq_remove
	.thumb
	.thumb_func
	.type	cm3rtos_wq_remove, %function
cm3rtos_wq_remove:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L17
	adds	r0, r0, #52
	add	r0, r3, r0, lsl #2
	lsls	r0, r0, #5
	movs	r3, #0
	str	r3, [r0, r1, lsl #2]
	bx	lr
.L18:
	.align	2
.L17:
	.word	cm3rtos_rt+17825792
	.size	cm3rtos_wq_remove, .-cm3rtos_wq_remove
	.align	2
	.global	cm3rtos_wq_get_head
	.thumb
	.thumb_func
	.type	cm3rtos_wq_get_head, %function
cm3rtos_wq_get_head:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movw	r3, #:lower16:cm3rtos_rt
	movt	r3, #:upper16:cm3rtos_rt
	adds	r0, r0, #52
	ldr	r0, [r3, r0, lsl #2]
@ 625 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	clz r0, r0

@ 0 "" 2
	.thumb
	bx	lr
	.size	cm3rtos_wq_get_head, .-cm3rtos_wq_get_head
	.align	2
	.global	cm3rtos_signal
	.thumb
	.thumb_func
	.type	cm3rtos_signal, %function
cm3rtos_signal:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movw	r3, #:lower16:cm3rtos_rt
	movt	r3, #:upper16:cm3rtos_rt
	adds	r0, r0, #52
	ldr	r1, [r3, r0, lsl #2]
@ 631 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	rbit r1, r1

@ 0 "" 2
@ 625 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	clz r1, r1

@ 0 "" 2
	.thumb
	add	r3, r3, #17825792
	add	r0, r3, r0, lsl #2
	mov	r2, #60672
	movs	r3, #1
	lsls	r0, r0, #5
	str	r3, [r0, r1, lsl #2]
	movt	r2, 57344
	mov	r3, #268435456
	str	r3, [r2, #4]
	bx	lr
	.size	cm3rtos_signal, .-cm3rtos_signal
	.align	4
	.global	cm3_pendsv_isr
	.thumb
	.thumb_func
	.type	cm3_pendsv_isr, %function
cm3_pendsv_isr:
	@ Naked Function: prologue and epilogue provided by programmer.
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
@ 165 "cm3rtos.c" 1
	mrs   r0, PSP
stmdb r0!, {r4-r11}

@ 0 "" 2
	.thumb
	ldr	r3, .L23
	ldr	r4, .L23+4
	ldr	r2, [r3, #136]
	lsls	r4, r4, #5
	asrs	r1, r2, #1
	str	r0, [r3, r1, lsl #2]
	str	r2, [r4, r1, lsl #2]
	ldr	r0, [r3, #132]
@ 631 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	rbit r0, r0

@ 0 "" 2
@ 625 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	clz r0, r0

@ 0 "" 2
	.thumb
	lsls	r1, r0, #1
	adds	r2, r1, #1
	str	r1, [r4, r0, lsl #2]
	ldr	r0, [r3, r1, lsl #1]
	str	r2, [r3, #136]
@ 181 "cm3rtos.c" 1
	add    r2, r0, #(8 * 4)
msr    PSP, r2
ldmia  r0, {r4-r11}
bx     lr

@ 0 "" 2
	.thumb
.L22:
	b	.L22
.L24:
	.align	2
.L23:
	.word	cm3rtos_rt
	.word	cm3rtos_rt+17825924
	.size	cm3_pendsv_isr, .-cm3_pendsv_isr
	.align	4
	.global	cm3_systick_isr
	.thumb
	.thumb_func
	.type	cm3_systick_isr, %function
cm3_systick_isr:
	@ Naked Function: prologue and epilogue provided by programmer.
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
@ 203 "cm3rtos.c" 1
	mrs   r0, PSP
stmdb r0!, {r4-r11}

@ 0 "" 2
	.thumb
	ldr	r3, .L37
	ldr	r5, [r3, #200]
	ldr	r2, [r3, #136]
	adds	r5, r5, #1
	asrs	r2, r2, #1
	str	r0, [r3, r2, lsl #2]
	str	r5, [r3, #200]
	ldr	r4, [r3, #196]
@ 631 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	rbit r4, r4

@ 0 "" 2
	.thumb
	ldr	r0, .L37+4
	ldr	r1, .L37+8
	lsls	r0, r0, #5
	lsl	ip, r1, #5
	mov	lr, #-2147483648
	movs	r7, #0
	movs	r6, #1
	b	.L35
.L28:
	lsr	r9, lr, r1
	ldr	r8, [r8, #164]
	bic	r4, r4, r9
	rsb	r8, r5, r8
	cmp	r8, #0
	itt	le
	strle	r7, [ip, r1, lsl #2]
	strle	r6, [r0, r1, lsl #2]
.L35:
@ 625 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	clz r1, r4

@ 0 "" 2
	.thumb
	cmp	r1, #31
	add	r8, r3, r1, lsl #2
	ble	.L28
	cmp	r2, #32
	beq	.L29
	adds	r1, r3, r2
	ldrb	r5, [r1, #156]	@ zero_extendqisi2
	ldrb	r4, [r1, #148]	@ zero_extendqisi2
	subs	r4, r5, r4
	uxtb	r4, r4
	uxtb	r5, r4
	strb	r5, [r1, #156]
	lsls	r5, r5, #24
	bmi	.L36
	movs	r1, #1
	str	r1, [r0, r2, lsl #2]
.L29:
	ldr	r2, [r3, #132]
	ldr	r1, .L37
	cbnz	r2, .L31
	ldr	r2, [r1, #140]
	movs	r4, #0
	str	r2, [r1, #132]
	str	r4, [r1, #140]
.L31:
@ 631 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	rbit r2, r2

@ 0 "" 2
@ 625 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	clz r4, r2

@ 0 "" 2
	.thumb
	lsls	r1, r4, #1
	adds	r2, r1, #1
	str	r1, [r0, r4, lsl #2]
	ldr	r0, [r3, r1, lsl #1]
	str	r2, [r3, #136]
@ 255 "cm3rtos.c" 1
	add    r2, r0, #(8 * 4)
msr    PSP, r2
ldmia  r0, {r4-r11}
mov    r0, #0xfffffffd
bx     r0

@ 0 "" 2
	.thumb
.L32:
	b	.L32
.L36:
	ldr	r5, .L37
	ldr	r5, [r5, #144]
	adds	r4, r4, r5
	strb	r4, [r1, #156]
	ldr	r1, .L37+12
	movs	r4, #1
	lsls	r1, r1, #5
	str	r4, [r1, r2, lsl #2]
	b	.L29
.L38:
	.align	2
.L37:
	.word	cm3rtos_rt
	.word	cm3rtos_rt+17825924
	.word	cm3rtos_rt+17825988
	.word	cm3rtos_rt+17825932
	.size	cm3_systick_isr, .-cm3_systick_isr
	.align	2
	.global	cm3rtos_join_svc
	.thumb
	.thumb_func
	.type	cm3rtos_join_svc, %function
cm3rtos_join_svc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L43
	lsls	r2, r3, #5
	ldr	r2, [r2, r0, lsl #2]
	cbz	r2, .L40
	ldr	r2, .L43+4
	add	r3, r3, r0, lsl #2
	ldr	r1, [r2, #136]
	subs	r3, r3, #32
	asrs	r1, r1, #1
	lsls	r3, r3, #5
	movs	r2, #1
	str	r2, [r3, r1, lsl #2]
	ldr	r3, .L43+8
	lsls	r3, r3, #5
	ldr	r1, [r3, r0, lsl #2]
	cbnz	r1, .L42
.L41:
	ldr	r2, .L43+12
	mov	r3, #60672
	lsls	r2, r2, #5
	movs	r1, #0
	str	r1, [r2, #0]
	movt	r3, 57344
	mov	r2, #268435456
	str	r2, [r3, #4]
.L40:
	mov	r0, #-1
	bx	lr
.L42:
	movs	r1, #0
	str	r1, [r3, r0, lsl #2]
	ldr	r3, .L43+16
	lsls	r3, r3, #5
	str	r2, [r3, r0, lsl #2]
	b	.L41
.L44:
	.align	2
.L43:
	.word	cm3rtos_rt+17826160
	.word	cm3rtos_rt
	.word	cm3rtos_rt+17826168
	.word	cm3rtos_rt+17825928
	.word	cm3rtos_rt+17825924
	.size	cm3rtos_join_svc, .-cm3rtos_join_svc
	.align	2
	.global	cm3rtos_cancel_svc
	.thumb
	.thumb_func
	.type	cm3rtos_cancel_svc, %function
cm3rtos_cancel_svc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r2, .L49
	mov	r3, r0
	lsls	r0, r2, #5
	ldr	r0, [r0, r3, lsl #2]
	push	{r4, r5, r6, lr}
	cbz	r0, .L48
	ldr	r0, .L49+4
	sub	r6, r2, #172
	ldr	r5, [r0, r3, lsl #2]
	sub	r0, r2, #228
	subs	r2, r2, #236
	movs	r4, #0
	lsls	r0, r0, #5
	lsls	r2, r2, #5
	lsls	r6, r6, #5
	str	r4, [r0, r3, lsl #2]
	str	r4, [r6, r3, lsl #2]
	str	r4, [r2, r3, lsl #2]
	ldr	r2, .L49+8
	str	r1, [r5, #32]
	str	r2, [r5, #56]
	ldr	r2, .L49+12
	movs	r1, #1
	lsls	r2, r2, #5
	mov	r0, r4
	str	r1, [r2, r3, lsl #2]
	pop	{r4, r5, r6, pc}
.L48:
	ldr	r2, .L49+4
	ldr	r0, .L49+16
	ldr	r2, [r2, #136]
	ldr	r1, .L49+20
	asrs	r2, r2, #1
	bl	printf
	mov	r0, #-1
	pop	{r4, r5, r6, pc}
.L50:
	.align	2
.L49:
	.word	cm3rtos_rt+17826160
	.word	cm3rtos_rt
	.word	cm3rtos_thread_exit
	.word	cm3rtos_rt+17826168
	.word	.LC1
	.word	.LANCHOR0+20
	.size	cm3rtos_cancel_svc, .-cm3rtos_cancel_svc
	.align	2
	.global	cm3rtos_thread_create_svc
	.thumb
	.thumb_func
	.type	cm3rtos_thread_create_svc, %function
cm3rtos_thread_create_svc:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r4, r5, r6, r7, r8, r9, sl, fp}
	ldr	r2, [r0, #16]
	sub	sp, sp, #32
	mov	r3, r0
	cbnz	r2, .L52
	ldr	r2, .L58
	ldr	r0, [r2, #368]
	mvns	r0, r0
@ 631 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	rbit r0, r0

@ 0 "" 2
@ 625 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	clz r0, r0

@ 0 "" 2
	.thumb
	cmp	r0, #32
	beq	.L57
	ldr	r1, .L58+4
	movs	r4, #1
	lsls	r1, r1, #5
	cmp	r0, #0
	str	r4, [r1, r0, lsl #2]
	bge	.L54
	b	.L53
.L52:
	ldr	r2, .L58
	ldr	r0, [r2, #368]
	mvns	r0, r0
@ 625 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	clz r0, r0

@ 0 "" 2
	.thumb
	rsbs	r0, r0, #31
	bmi	.L57
	ldr	r1, .L58+4
	movs	r4, #1
	lsls	r1, r1, #5
	str	r4, [r1, r0, lsl #2]
.L54:
	ldr	r4, [r3, #8]
	ldr	r1, [r3, #12]
	ldr	r5, [r3, #4]
	adds	r1, r4, r1
	bic	r1, r1, #7
	ldr	r4, [r3, #0]
	ldrb	r3, [r3, #16]	@ zero_extendqisi2
	str	r1, [sp, #12]
	str	r4, [sp, #24]
	str	r3, [sp, #4]
	ldr	r4, [sp, #12]
	ldr	r3, [r2, #144]
	str	r5, [sp, #20]
	str	r3, [sp, #8]
	sub	r3, r4, #64
	movw	r4, #61451
	movt	r4, 57034
	str	r4, [sp, #0]
	movw	r4, #61452
	movt	r4, 57034
	movw	r5, #61450
	movw	r1, #61442
	str	r4, [sp, #16]
	movt	r5, 57034
	adds	r4, r2, r0
	movt	r1, 57034
	str	r5, [r3, #24]
	str	r1, [r3, #40]
	ldr	r5, [sp, #0]
	str	r4, [sp, #28]
	ldr	r1, .L58+8
	ldr	r4, [sp, #12]
	movw	sl, #61444
	movt	sl, 57034
	str	r5, [r3, #28]
	str	r1, [r3, #52]
	ldr	r5, [sp, #16]
	str	sl, [r4, #-64]
	ldr	r1, [sp, #24]
	ldr	r4, [sp, #20]
	movw	r9, #61445
	movw	r8, #61446
	movw	ip, #61447
	movw	r7, #61448
	movw	r6, #61449
	movw	fp, #61443
	movt	r9, 57034
	movt	r8, 57034
	movt	ip, 57034
	movt	r7, 57034
	movt	r6, 57034
	movt	fp, 57034
	str	r4, [r3, #32]
	str	r5, [r3, #48]
	str	r9, [r3, #4]
	str	r8, [r3, #8]
	str	ip, [r3, #12]
	str	r7, [r3, #16]
	str	r6, [r3, #20]
	str	r0, [r3, #36]
	str	fp, [r3, #44]
	str	r1, [r3, #56]
	mov	r1, #16777216
	str	r1, [r3, #60]
	ldr	r5, [sp, #8]
	str	r3, [r2, r0, lsl #2]
	ldr	r3, [sp, #4]
	ldr	r4, [sp, #28]
	cmp	r5, r3
	it	cc
	ldrcc	r1, [sp, #4]
	strb	r3, [r4, #148]
	itt	cc
	strcc	r1, [sp, #8]
	ldrcc	r3, .L58
	ldr	r4, [sp, #8]
	it	cc
	strcc	r1, [r3, #144]
	adds	r2, r2, r0
	lsrs	r3, r4, #1
	strb	r3, [r2, #156]
	ldr	r3, .L58+12
	movs	r2, #1
	lsls	r3, r3, #5
	str	r2, [r3, r0, lsl #2]
.L53:
	add	sp, sp, #32
	pop	{r4, r5, r6, r7, r8, r9, sl, fp}
	bx	lr
.L57:
	mov	r0, #-1
	b	.L53
.L59:
	.align	2
.L58:
	.word	cm3rtos_rt
	.word	cm3rtos_rt+17826160
	.word	cm3rtos_thread_exit
	.word	cm3rtos_rt+17825924
	.size	cm3rtos_thread_create_svc, .-cm3rtos_thread_create_svc
	.align	2
	.global	cm3rtos_thread_id
	.thumb
	.thumb_func
	.type	cm3rtos_thread_id, %function
cm3rtos_thread_id:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movw	r3, #:lower16:cm3rtos_rt
	movt	r3, #:upper16:cm3rtos_rt
	ldr	r0, [r3, #136]
	asrs	r0, r0, #1
	bx	lr
	.size	cm3rtos_thread_id, .-cm3rtos_thread_id
	.align	2
	.global	cm3rtos_sleep_svc
	.thumb
	.thumb_func
	.type	cm3rtos_sleep_svc, %function
cm3rtos_sleep_svc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movw	r3, #:lower16:cm3rtos_rt
	movt	r3, #:upper16:cm3rtos_rt
	ldr	r2, [r3, #136]
	ldr	r1, [r3, #200]
	asrs	r2, r2, #1
	push	{r4, r5, r6}
	adds	r1, r0, r1
	add	r6, r3, r2, lsl #2
	str	r1, [r6, #164]
	ldr	r1, .L62
	mov	r4, #60672
	lsls	r5, r2, #1
	movs	r0, #1
	lsls	r1, r1, #5
	str	r0, [r1, r2, lsl #2]
	movt	r4, 57344
	str	r5, [r3, #136]
	mov	r3, #268435456
	str	r3, [r4, #4]
	pop	{r4, r5, r6}
	bx	lr
.L63:
	.align	2
.L62:
	.word	cm3rtos_rt+17825988
	.size	cm3rtos_sleep_svc, .-cm3rtos_sleep_svc
	.align	2
	.global	cm3_svc_handler
	.thumb
	.thumb_func
	.type	cm3_svc_handler, %function
cm3_svc_handler:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
@ 613 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	mrs r4, PSP

@ 0 "" 2
	.thumb
	ldr	r3, [r4, #24]
	ldrb	r3, [r3, #-2]	@ zero_extendqisi2
	cmp	r3, #6
	bhi	.L64
	tbb	[pc, r3]
.L72:
	.byte	(.L66-.L72)/2
	.byte	(.L64-.L72)/2
	.byte	(.L67-.L72)/2
	.byte	(.L68-.L72)/2
	.byte	(.L69-.L72)/2
	.byte	(.L70-.L72)/2
	.byte	(.L71-.L72)/2
	.align	1
.L71:
	ldr	r3, .L73
	ldr	r2, [r4, #0]
	sub	r1, r3, #17825792
	ldr	r1, [r1, #136]
	adds	r2, r2, #52
	add	r2, r3, r2, lsl #2
	lsls	r2, r2, #5
	mov	r3, #60672
	asrs	r1, r1, #1
	movs	r0, #1
	str	r0, [r2, r1, lsl #2]
	movt	r3, 57344
	mov	r2, #268435456
	str	r2, [r3, #4]
.L64:
	pop	{r4, pc}
.L70:
	ldr	r0, [r4, #0]
	bl	cm3rtos_join_svc
	str	r0, [r4, #0]
	pop	{r4, pc}
.L66:
	ldr	r3, .L73+4
	ldr	r3, [r3, #136]
	asrs	r3, r3, #1
	str	r3, [r4, #0]
	pop	{r4, pc}
.L67:
	mov	r0, r4
	bl	cm3rtos_thread_create_svc
	str	r0, [r4, #0]
	pop	{r4, pc}
.L68:
	ldmia	r4, {r0, r1}
	bl	cm3rtos_cancel_svc
	str	r0, [r4, #0]
	pop	{r4, pc}
.L69:
	ldr	r0, [r4, #0]
	pop	{r4, lr}
	b	cm3rtos_sleep_svc
.L74:
	.align	2
.L73:
	.word	cm3rtos_rt+17825792
	.word	cm3rtos_rt
	.size	cm3_svc_handler, .-cm3_svc_handler
	.align	2
	.global	cm3rtos_init
	.thumb
	.thumb_func
	.type	cm3rtos_init, %function
cm3rtos_init:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r3, #60672
	movt	r3, 57344
	movs	r0, #128
	movs	r2, #0
	push	{r4, r5, r6, r7}
	mov	r1, r0
	strb	r2, [r3, #31]
	strb	r0, [r3, #35]
	strb	r0, [r3, #34]
.L76:
	mov	r3, #57600
	movt	r3, 57344
	adds	r3, r2, r3
	adds	r2, r2, #1
	cmp	r2, #80
	strb	r1, [r3, #768]
	bne	.L76
@ 619 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	mov r3, sp

@ 0 "" 2
@ 608 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	msr PSP, r3

@ 0 "" 2
	.thumb
	ldr	r3, .L80
@ 598 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	msr MSP, r3

@ 0 "" 2
	.thumb
	movs	r3, #2
@ 544 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	msr CONTROL, r3

@ 0 "" 2
	.thumb
	ldr	r3, .L80+4
	mvn	r2, #255
	str	r2, [r3, #368]
	movs	r4, #255
@ 631 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	rbit r4, r4

@ 0 "" 2
@ 625 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	clz r4, r4

@ 0 "" 2
	.thumb
	cmp	r4, #32
	itt	eq
	moveq	r7, #-1
	moveq	r4, r7
	beq	.L77
	ldr	r2, .L80+8
	movs	r1, #1
	lsls	r7, r4, #1
	lsls	r2, r2, #5
	str	r1, [r2, r4, lsl #2]
	adds	r7, r7, r1
.L77:
	movs	r0, #0
	adds	r4, r3, r4
	movs	r6, #1
	movw	r1, #57360
	ldr	r5, .L80+12
	strb	r6, [r4, #148]
	strb	r0, [r4, #156]
	movt	r1, 57344
	movw	r4, #14999
	str	r7, [r3, #136]
	str	r6, [r3, #144]
	str	r4, [r1, #4]
	movs	r4, #3
	str	r0, [r1, #8]
	str	r4, [r1, #0]
	str	r5, [r3, #128]
	ldr	r3, .L80+16
	str	r0, [r5, #48]
	str	r3, [r5, #56]
	mov	r3, #16777216
	str	r3, [r5, #60]
	ldr	r3, .L80+20
	str	r3, [r5, #52]
	pop	{r4, r5, r6, r7}
	bx	lr
.L81:
	.align	2
.L80:
	.word	cm3rtos_idle+256
	.word	cm3rtos_rt
	.word	cm3rtos_rt+17826160
	.word	cm3rtos_idle+208
	.word	cm3rtos_idle_task
	.word	cm3rtos_rt+200
	.size	cm3rtos_init, .-cm3rtos_init
	.comm	cm3rtos_idle,272,8
	.comm	cm3rtos_rt,384,8
	.comm	stm32f_usart5_file,8,4
	.comm	stm32f_usart_ops,16,4
	.section	.rodata
	.align	2
	.set	.LANCHOR0,. + 0
	.type	__func__.2350, %object
	.size	__func__.2350, 20
__func__.2350:
	.ascii	"cm3rtos_thread_exit\000"
	.type	__func__.2365, %object
	.size	__func__.2365, 19
__func__.2365:
	.ascii	"cm3rtos_cancel_svc\000"
	.section	.rodata.str1.4,"aMS",%progbits,1
	.align	2
.LC0:
	.ascii	"%s(): <%d> code=%d\012\000"
.LC1:
	.ascii	"%s(): <%d> cancel %d failed, not alloc!\012\000"
	.ident	"GCC: (GNU) 4.6.2"
