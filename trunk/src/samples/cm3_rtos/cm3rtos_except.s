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
	.file	"cm3rtos_except.c"
	.text
	.align	2
	.global	cm3_hard_fault_isr
	.thumb
	.thumb_func
	.type	cm3_hard_fault_isr, %function
cm3_hard_fault_isr:
	@ Naked Function: prologue and epilogue provided by programmer.
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, #1
@ 588 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	msr FAULTMASK, r3

@ 0 "" 2
	.thumb
	ldr	r3, .L10
@ 598 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	msr MSP, r3

@ 0 "" 2
	.thumb
	ldr	r0, .L10+4
	bl	puts
	ldr	r0, .L10+8
	bl	printf
	mov	r3, #60672
	movt	r3, 57344
	ldr	r4, [r3, #44]
	cmp	r4, #0
	blt	.L7
.L2:
	lsls	r2, r4, #1
	bmi	.L8
.L3:
	lsls	r3, r4, #30
	bmi	.L9
.L4:
	movs	r0, #10
	bl	putchar
.L5:
	b	.L5
.L9:
	ldr	r0, .L10+12
	bl	printf
	b	.L4
.L8:
	ldr	r0, .L10+16
	bl	printf
	b	.L3
.L7:
	ldr	r0, .L10+20
	bl	printf
	b	.L2
.L11:
	.align	2
.L10:
	.word	cm3rtos_except_stack
	.word	.LC0
	.word	.LC1
	.word	.LC4
	.word	.LC3
	.word	.LC2
	.size	cm3_hard_fault_isr, .-cm3_hard_fault_isr
	.align	2
	.global	cm3_bus_fault_isr
	.thumb
	.thumb_func
	.type	cm3_bus_fault_isr, %function
cm3_bus_fault_isr:
	@ Naked Function: prologue and epilogue provided by programmer.
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, #1
@ 588 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	msr FAULTMASK, r3

@ 0 "" 2
	.thumb
	ldr	r3, .L30
@ 598 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	msr MSP, r3

@ 0 "" 2
	.thumb
	ldr	r0, .L30+4
	bl	puts
	ldr	r0, .L30+8
	bl	printf
	mov	r3, #60672
	movt	r3, 57344
	ldr	r4, [r3, #40]
	ubfx	r4, r4, #8, #8
	ands	r5, r4, #128
	bne	.L22
.L13:
	lsls	r0, r4, #26
	bmi	.L23
.L14:
	lsls	r1, r4, #27
	bmi	.L24
.L15:
	lsls	r2, r4, #28
	bmi	.L25
.L16:
	lsls	r3, r4, #29
	bmi	.L26
.L17:
	lsls	r0, r4, #30
	bmi	.L27
.L18:
	lsls	r1, r4, #31
	bmi	.L28
.L19:
	movs	r0, #10
	bl	putchar
	cbnz	r5, .L29
.L20:
.L21:
	b	.L21
.L29:
	mov	r3, #60672
	movt	r3, 57344
	ldr	r1, [r3, #56]
	ldr	r0, .L30+12
	bl	printf
	b	.L20
.L28:
	ldr	r0, .L30+16
	bl	printf
	b	.L19
.L27:
	ldr	r0, .L30+20
	bl	printf
	b	.L18
.L26:
	ldr	r0, .L30+24
	bl	printf
	b	.L17
.L25:
	ldr	r0, .L30+28
	bl	printf
	b	.L16
.L24:
	ldr	r0, .L30+32
	bl	printf
	b	.L15
.L23:
	ldr	r0, .L30+36
	bl	printf
	b	.L14
.L22:
	ldr	r0, .L30+40
	bl	printf
	b	.L13
.L31:
	.align	2
.L30:
	.word	cm3rtos_except_stack
	.word	.LC0
	.word	.LC5
	.word	.LC13
	.word	.LC12
	.word	.LC11
	.word	.LC10
	.word	.LC9
	.word	.LC8
	.word	.LC7
	.word	.LC6
	.size	cm3_bus_fault_isr, .-cm3_bus_fault_isr
	.align	2
	.global	cm3_usage_fault_isr
	.thumb
	.thumb_func
	.type	cm3_usage_fault_isr, %function
cm3_usage_fault_isr:
	@ Naked Function: prologue and epilogue provided by programmer.
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, #1
@ 588 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	msr FAULTMASK, r3

@ 0 "" 2
	.thumb
	ldr	r3, .L46
@ 598 "/home/bob/devel/stm32f-dk/include/arch/cortex-m3.h" 1
	msr MSP, r3

@ 0 "" 2
	.thumb
	ldr	r0, .L46+4
	bl	puts
	ldr	r0, .L46+8
	bl	printf
	mov	r3, #60672
	movt	r3, 57344
	ldr	r4, [r3, #40]
	lsrs	r4, r4, #16
	lsls	r3, r4, #22
	bmi	.L40
.L33:
	lsls	r0, r4, #23
	bmi	.L41
.L34:
	lsls	r1, r4, #28
	bmi	.L42
.L35:
	lsls	r2, r4, #29
	bmi	.L43
.L36:
	lsls	r3, r4, #30
	bmi	.L44
.L37:
	lsls	r4, r4, #31
	bmi	.L45
.L38:
	movs	r0, #10
	bl	putchar
.L39:
	b	.L39
.L45:
	ldr	r0, .L46+12
	bl	printf
	b	.L38
.L44:
	ldr	r0, .L46+16
	bl	printf
	b	.L37
.L43:
	ldr	r0, .L46+20
	bl	printf
	b	.L36
.L42:
	ldr	r0, .L46+24
	bl	printf
	b	.L35
.L41:
	ldr	r0, .L46+28
	bl	printf
	b	.L34
.L40:
	ldr	r0, .L46+32
	bl	printf
	b	.L33
.L47:
	.align	2
.L46:
	.word	cm3rtos_except_stack
	.word	.LC0
	.word	.LC14
	.word	.LC19
	.word	.LC18
	.word	.LC9
	.word	.LC17
	.word	.LC16
	.word	.LC15
	.size	cm3_usage_fault_isr, .-cm3_usage_fault_isr
	.comm	stm32f_usart5_file,8,4
	.comm	stm32f_usart_ops,16,4
	.section	.rodata.str1.4,"aMS",%progbits,1
	.align	2
.LC0:
	.ascii	"---\000"
.LC1:
	.ascii	"Hard fault:\000"
.LC2:
	.ascii	" DEBUGEVT\000"
	.space	2
.LC3:
	.ascii	" FORCED\000"
.LC4:
	.ascii	" VECTTBL\000"
	.space	3
.LC5:
	.ascii	"Bus fault:\000"
	.space	1
.LC6:
	.ascii	" BFARVALID\000"
	.space	1
.LC7:
	.ascii	" LSPERR\000"
.LC8:
	.ascii	" STKERR\000"
.LC9:
	.ascii	" INVPC\000"
	.space	1
.LC10:
	.ascii	" IMPRECISERR\000"
	.space	3
.LC11:
	.ascii	" PRECISERR\000"
	.space	1
.LC12:
	.ascii	" IBUSERR\000"
	.space	3
.LC13:
	.ascii	" * ADDR = 0x%08x\012\000"
	.space	2
.LC14:
	.ascii	"Usage fault:\000"
	.space	3
.LC15:
	.ascii	" DIVBYZERO\000"
	.space	1
.LC16:
	.ascii	" UNALIGNED\000"
	.space	1
.LC17:
	.ascii	" NOCP\000"
	.space	2
.LC18:
	.ascii	" INVSTATE\000"
	.space	2
.LC19:
	.ascii	" UNDEFINSTR\000"
	.ident	"GCC: (GNU) 4.6.2"
