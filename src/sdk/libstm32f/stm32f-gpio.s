	.syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 1
	.eabi_attribute 18, 4
	.thumb
	.file	"stm32f-gpio.c"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.align	2
	.global	stm32f_gpio_clock_en
	.thumb
	.thumb_func
	.type	stm32f_gpio_clock_en, %function
stm32f_gpio_clock_en:
.LFB0:
	.file 1 "stm32f-gpio.c"
	.loc 1 25 0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
.LVL0:
	.loc 1 29 0
	mov	r3, #14336
	movt	r3, 16386
	ldr	r2, [r3, #48]
	mov	r1, #1
	lsl	r0, r1, r0
.LVL1:
	orrs	r0, r0, r2
	str	r0, [r3, #48]
	.loc 1 30 0
	bx	lr
	.cfi_endproc
.LFE0:
	.size	stm32f_gpio_clock_en, .-stm32f_gpio_clock_en
	.align	2
	.global	stm32f_gpio_mode
	.thumb
	.thumb_func
	.type	stm32f_gpio_mode, %function
stm32f_gpio_mode:
.LFB1:
	.loc 1 34 0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
.LVL2:
	push	{r4, r5, r6, r7}
.LCFI0:
	.cfi_def_cfa_offset 16
	.cfi_offset 7, -4
	.cfi_offset 6, -8
	.cfi_offset 5, -12
	.cfi_offset 4, -16
	.loc 1 39 0
	ldr	r4, [r0, #0]
.LVL3:
	.loc 1 40 0
	lsl	r3, r1, #1
.LVL4:
	mov	r7, #3
	lsl	r7, r7, r3
	mvn	r6, r7
	ands	r4, r4, r6
.LVL5:
	.loc 1 41 0
	and	r5, r2, #31
	cmp	r5, #2
	.loc 1 49 0
	ittt	eq
	moveq	r5, #2
	lsleq	r5, r5, r3
	orreq	r4, r4, r5
.LVL6:
	.loc 1 41 0
	beq	.L3
	cmp	r5, #3
	.loc 1 52 0
	it	eq
	orreq	r4, r4, r7
	.loc 1 41 0
	beq	.L3
	cmp	r5, #1
	.loc 1 46 0
	itt	eq
	lsleq	r5, r5, r3
	orreq	r4, r4, r5
.L3:
.LVL7:
	.loc 1 55 0
	str	r4, [r0, #0]
	.loc 1 58 0
	ldr	r4, [r0, #8]
.LVL8:
	.loc 1 59 0
	ands	r6, r6, r4
.LVL9:
	.loc 1 60 0
	and	r4, r2, #3
	lsl	r4, r4, r3
	orrs	r6, r6, r4
.LVL10:
	.loc 1 61 0
	str	r6, [r0, #8]
	.loc 1 64 0
	ldr	r5, [r0, #4]
.LVL11:
	.loc 1 65 0
	mov	r4, #1
	lsl	r1, r4, r1
.LVL12:
	mvn	r4, r1
	ands	r5, r5, r4
.LVL13:
	.loc 1 66 0
	tst	r2, #4
	.loc 1 67 0
	it	ne
	orrne	r5, r5, r1
.LVL14:
	.loc 1 70 0
	str	r5, [r0, #4]
	.loc 1 73 0
	ldr	r1, [r0, #12]
.LVL15:
	.loc 1 74 0
	ands	r1, r1, r4
.LVL16:
	.loc 1 75 0
	tst	r2, #8
	.loc 1 76 0
	ittt	ne
	movne	r2, #1
.LVL17:
	lslne	r3, r2, r3
	orrne	r1, r1, r3
.LVL18:
	.loc 1 75 0
	bne	.L9
	.loc 1 77 0
	tst	r2, #16
	.loc 1 78 0
	ittt	ne
	movne	r2, #2
	lslne	r3, r2, r3
	orrne	r1, r1, r3
.LVL19:
.L9:
	.loc 1 79 0
	str	r1, [r0, #12]
	.loc 1 81 0
	pop	{r4, r5, r6, r7}
	bx	lr
	.cfi_endproc
.LFE1:
	.size	stm32f_gpio_mode, .-stm32f_gpio_mode
	.align	2
	.global	stm32f_gpio_af
	.thumb
	.thumb_func
	.type	stm32f_gpio_af, %function
stm32f_gpio_af:
.LFB2:
	.loc 1 84 0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
.LVL20:
	push	{r4}
.LCFI1:
	.cfi_def_cfa_offset 4
	.cfi_offset 4, -4
	.loc 1 87 0
	cmp	r1, #7
	bgt	.L11
	.loc 1 88 0
	ldr	r4, [r0, #32]
.LVL21:
	.loc 1 89 0
	lsl	r1, r1, #2
.LVL22:
	mov	r3, #15
	lsl	r3, r3, r1
	bic	r3, r4, r3
.LVL23:
	.loc 1 90 0
	lsl	r1, r2, r1
	orrs	r1, r1, r3
.LVL24:
	.loc 1 91 0
	str	r1, [r0, #32]
	b	.L10
.LVL25:
.L11:
	.loc 1 93 0
	ldr	r4, [r0, #36]
.LVL26:
	.loc 1 94 0
	sub	r1, r1, #8
.LVL27:
	lsl	r1, r1, #2
.LVL28:
	mov	r3, #15
	lsl	r3, r3, r1
	bic	r3, r4, r3
.LVL29:
	.loc 1 95 0
	lsl	r2, r2, r1
.LVL30:
	orr	r1, r3, r2
.LVL31:
	.loc 1 96 0
	str	r1, [r0, #36]
.L10:
	.loc 1 98 0
	pop	{r4}
	bx	lr
	.cfi_endproc
.LFE2:
	.size	stm32f_gpio_af, .-stm32f_gpio_af
.Letext0:
	.file 2 "/home/bob/devel/uboot/include/stdint.h"
	.file 3 "/home/bob/devel/uboot/include/stm32f/stm32f-gpio.h"
	.file 4 "/home/bob/devel/uboot/include/stm32f/stm32f-rcc.h"
	.file 5 "/home/bob/devel/uboot/include/arch/stm32f207.h"
	.file 6 "/home/bob/devel/uboot/include/sys/stm32f.h"
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.4byte	0x471
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF67
	.byte	0x1
	.4byte	.LASF68
	.4byte	.LASF69
	.4byte	.Ltext0
	.4byte	.Letext0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF0
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF1
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF2
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF3
	.uleb128 0x4
	.4byte	.LASF5
	.byte	0x2
	.byte	0x19
	.4byte	0x53
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF4
	.uleb128 0x4
	.4byte	.LASF6
	.byte	0x2
	.byte	0x1a
	.4byte	0x65
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF7
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF8
	.uleb128 0x5
	.4byte	0x5a
	.uleb128 0x6
	.4byte	.LASF18
	.byte	0x28
	.byte	0x3
	.byte	0x53
	.4byte	0x11f
	.uleb128 0x7
	.4byte	.LASF9
	.byte	0x3
	.byte	0x54
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x7
	.4byte	.LASF10
	.byte	0x3
	.byte	0x55
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x7
	.4byte	.LASF11
	.byte	0x3
	.byte	0x56
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x7
	.4byte	.LASF12
	.byte	0x3
	.byte	0x57
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x8
	.ascii	"idr\000"
	.byte	0x3
	.byte	0x58
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x8
	.ascii	"odr\000"
	.byte	0x3
	.byte	0x59
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0x7
	.4byte	.LASF13
	.byte	0x3
	.byte	0x5a
	.4byte	0x11f
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x7
	.4byte	.LASF14
	.byte	0x3
	.byte	0x5b
	.4byte	0x11f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1a
	.uleb128 0x7
	.4byte	.LASF15
	.byte	0x3
	.byte	0x5c
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0x7
	.4byte	.LASF16
	.byte	0x3
	.byte	0x5d
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0x7
	.4byte	.LASF17
	.byte	0x3
	.byte	0x5e
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.byte	0
	.uleb128 0x5
	.4byte	0x48
	.uleb128 0x9
	.4byte	.LASF19
	.byte	0x88
	.byte	0x4
	.2byte	0x310
	.4byte	0x2f5
	.uleb128 0xa
	.ascii	"cr\000"
	.byte	0x4
	.2byte	0x311
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xb
	.4byte	.LASF20
	.byte	0x4
	.2byte	0x312
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0xb
	.4byte	.LASF21
	.byte	0x4
	.2byte	0x313
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0xa
	.ascii	"cir\000"
	.byte	0x4
	.2byte	0x314
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0xb
	.4byte	.LASF22
	.byte	0x4
	.2byte	0x315
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0xb
	.4byte	.LASF23
	.byte	0x4
	.2byte	0x316
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0xb
	.4byte	.LASF24
	.byte	0x4
	.2byte	0x317
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0xb
	.4byte	.LASF25
	.byte	0x4
	.2byte	0x318
	.4byte	0x5a
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0xb
	.4byte	.LASF26
	.byte	0x4
	.2byte	0x319
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0xb
	.4byte	.LASF27
	.byte	0x4
	.2byte	0x31a
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.uleb128 0xb
	.4byte	.LASF28
	.byte	0x4
	.2byte	0x31b
	.4byte	0x2f5
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0xb
	.4byte	.LASF29
	.byte	0x4
	.2byte	0x31c
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.uleb128 0xb
	.4byte	.LASF30
	.byte	0x4
	.2byte	0x31d
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x34
	.uleb128 0xb
	.4byte	.LASF31
	.byte	0x4
	.2byte	0x31e
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.uleb128 0xb
	.4byte	.LASF32
	.byte	0x4
	.2byte	0x31f
	.4byte	0x5a
	.byte	0x2
	.byte	0x23
	.uleb128 0x3c
	.uleb128 0xb
	.4byte	.LASF33
	.byte	0x4
	.2byte	0x320
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x40
	.uleb128 0xb
	.4byte	.LASF34
	.byte	0x4
	.2byte	0x321
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x44
	.uleb128 0xb
	.4byte	.LASF35
	.byte	0x4
	.2byte	0x322
	.4byte	0x2f5
	.byte	0x2
	.byte	0x23
	.uleb128 0x48
	.uleb128 0xb
	.4byte	.LASF36
	.byte	0x4
	.2byte	0x323
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x50
	.uleb128 0xb
	.4byte	.LASF37
	.byte	0x4
	.2byte	0x324
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x54
	.uleb128 0xb
	.4byte	.LASF38
	.byte	0x4
	.2byte	0x325
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x58
	.uleb128 0xb
	.4byte	.LASF39
	.byte	0x4
	.2byte	0x326
	.4byte	0x5a
	.byte	0x2
	.byte	0x23
	.uleb128 0x5c
	.uleb128 0xb
	.4byte	.LASF40
	.byte	0x4
	.2byte	0x327
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x60
	.uleb128 0xb
	.4byte	.LASF41
	.byte	0x4
	.2byte	0x328
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x64
	.uleb128 0xb
	.4byte	.LASF42
	.byte	0x4
	.2byte	0x329
	.4byte	0x2f5
	.byte	0x2
	.byte	0x23
	.uleb128 0x68
	.uleb128 0xb
	.4byte	.LASF43
	.byte	0x4
	.2byte	0x32a
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x70
	.uleb128 0xa
	.ascii	"csr\000"
	.byte	0x4
	.2byte	0x32b
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x74
	.uleb128 0xb
	.4byte	.LASF44
	.byte	0x4
	.2byte	0x32c
	.4byte	0x2f5
	.byte	0x2
	.byte	0x23
	.uleb128 0x78
	.uleb128 0xb
	.4byte	.LASF45
	.byte	0x4
	.2byte	0x32d
	.4byte	0x73
	.byte	0x3
	.byte	0x23
	.uleb128 0x80
	.uleb128 0xb
	.4byte	.LASF46
	.byte	0x4
	.2byte	0x32e
	.4byte	0x73
	.byte	0x3
	.byte	0x23
	.uleb128 0x84
	.byte	0
	.uleb128 0xc
	.4byte	0x5a
	.4byte	0x305
	.uleb128 0xd
	.4byte	0x65
	.byte	0x1
	.byte	0
	.uleb128 0xe
	.byte	0x1
	.byte	0x5
	.byte	0x6a
	.4byte	0x344
	.uleb128 0xf
	.4byte	.LASF47
	.sleb128 0
	.uleb128 0xf
	.4byte	.LASF48
	.sleb128 1
	.uleb128 0xf
	.4byte	.LASF49
	.sleb128 2
	.uleb128 0xf
	.4byte	.LASF50
	.sleb128 3
	.uleb128 0xf
	.4byte	.LASF51
	.sleb128 4
	.uleb128 0xf
	.4byte	.LASF52
	.sleb128 5
	.uleb128 0xf
	.4byte	.LASF53
	.sleb128 6
	.uleb128 0xf
	.4byte	.LASF54
	.sleb128 7
	.uleb128 0xf
	.4byte	.LASF55
	.sleb128 8
	.byte	0
	.uleb128 0x4
	.4byte	.LASF56
	.byte	0x5
	.byte	0x74
	.4byte	0x305
	.uleb128 0xe
	.byte	0x1
	.byte	0x6
	.byte	0x2b
	.4byte	0x370
	.uleb128 0xf
	.4byte	.LASF57
	.sleb128 0
	.uleb128 0xf
	.4byte	.LASF58
	.sleb128 1
	.uleb128 0xf
	.4byte	.LASF59
	.sleb128 2
	.uleb128 0xf
	.4byte	.LASF60
	.sleb128 3
	.byte	0
	.uleb128 0x10
	.byte	0x1
	.4byte	.LASF61
	.byte	0x1
	.byte	0x18
	.byte	0x1
	.4byte	.LFB0
	.4byte	.LFE0
	.byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	0x3a7
	.uleb128 0x11
	.4byte	.LASF63
	.byte	0x1
	.byte	0x18
	.4byte	0x344
	.4byte	.LLST0
	.uleb128 0x12
	.ascii	"rcc\000"
	.byte	0x1
	.byte	0x1a
	.4byte	0x3a7
	.4byte	0x40023800
	.byte	0
	.uleb128 0x13
	.byte	0x4
	.4byte	0x124
	.uleb128 0x14
	.byte	0x1
	.4byte	.LASF62
	.byte	0x1
	.byte	0x20
	.byte	0x1
	.4byte	.LFB1
	.4byte	.LFE1
	.4byte	.LLST1
	.4byte	0x41f
	.uleb128 0x15
	.4byte	.LASF64
	.byte	0x1
	.byte	0x20
	.4byte	0x41f
	.byte	0x1
	.byte	0x50
	.uleb128 0x11
	.4byte	.LASF65
	.byte	0x1
	.byte	0x21
	.4byte	0x65
	.4byte	.LLST2
	.uleb128 0x11
	.4byte	.LASF66
	.byte	0x1
	.byte	0x21
	.4byte	0x65
	.4byte	.LLST3
	.uleb128 0x16
	.ascii	"opt\000"
	.byte	0x1
	.byte	0x21
	.4byte	0x65
	.4byte	.LLST4
	.uleb128 0x17
	.ascii	"tmp\000"
	.byte	0x1
	.byte	0x23
	.4byte	0x5a
	.4byte	.LLST5
	.uleb128 0x18
	.4byte	.LASF9
	.byte	0x1
	.byte	0x24
	.4byte	0x5a
	.4byte	.LLST6
	.byte	0
	.uleb128 0x13
	.byte	0x4
	.4byte	0x78
	.uleb128 0x19
	.byte	0x1
	.4byte	.LASF70
	.byte	0x1
	.byte	0x53
	.byte	0x1
	.4byte	.LFB2
	.4byte	.LFE2
	.4byte	.LLST7
	.uleb128 0x15
	.4byte	.LASF64
	.byte	0x1
	.byte	0x53
	.4byte	0x41f
	.byte	0x1
	.byte	0x50
	.uleb128 0x11
	.4byte	.LASF65
	.byte	0x1
	.byte	0x53
	.4byte	0x33
	.4byte	.LLST8
	.uleb128 0x16
	.ascii	"af\000"
	.byte	0x1
	.byte	0x53
	.4byte	0x33
	.4byte	.LLST9
	.uleb128 0x17
	.ascii	"tmp\000"
	.byte	0x1
	.byte	0x55
	.4byte	0x5a
	.4byte	.LLST10
	.byte	0
	.byte	0
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x35
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x4
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1c
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",%progbits
.Ldebug_loc0:
.LLST0:
	.4byte	.LVL0-.Ltext0
	.4byte	.LVL1-.Ltext0
	.2byte	0x1
	.byte	0x50
	.4byte	0
	.4byte	0
.LLST1:
	.4byte	.LFB1-.Ltext0
	.4byte	.LCFI0-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI0-.Ltext0
	.4byte	.LFE1-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 16
	.4byte	0
	.4byte	0
.LLST2:
	.4byte	.LVL2-.Ltext0
	.4byte	.LVL12-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	0
	.4byte	0
.LLST3:
	.4byte	.LVL2-.Ltext0
	.4byte	.LVL17-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	0
	.4byte	0
.LLST4:
	.4byte	.LVL2-.Ltext0
	.4byte	.LVL4-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	0
	.4byte	0
.LLST5:
	.4byte	.LVL8-.Ltext0
	.4byte	.LVL9-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL9-.Ltext0
	.4byte	.LVL11-.Ltext0
	.2byte	0x1
	.byte	0x56
	.4byte	.LVL11-.Ltext0
	.4byte	.LVL15-.Ltext0
	.2byte	0x1
	.byte	0x55
	.4byte	.LVL15-.Ltext0
	.4byte	.LVL18-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL19-.Ltext0
	.4byte	.LFE1-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	0
	.4byte	0
.LLST6:
	.4byte	.LVL3-.Ltext0
	.4byte	.LVL6-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL7-.Ltext0
	.4byte	.LVL8-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL8-.Ltext0
	.4byte	.LFE1-.Ltext0
	.2byte	0x2
	.byte	0x70
	.sleb128 0
	.4byte	0
	.4byte	0
.LLST7:
	.4byte	.LFB2-.Ltext0
	.4byte	.LCFI1-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI1-.Ltext0
	.4byte	.LFE2-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 4
	.4byte	0
	.4byte	0
.LLST8:
	.4byte	.LVL20-.Ltext0
	.4byte	.LVL22-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL25-.Ltext0
	.4byte	.LVL27-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL27-.Ltext0
	.4byte	.LVL28-.Ltext0
	.2byte	0x3
	.byte	0x71
	.sleb128 8
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST9:
	.4byte	.LVL20-.Ltext0
	.4byte	.LVL30-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	0
	.4byte	0
.LLST10:
	.4byte	.LVL21-.Ltext0
	.4byte	.LVL23-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL23-.Ltext0
	.4byte	.LVL24-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL24-.Ltext0
	.4byte	.LVL25-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL26-.Ltext0
	.4byte	.LVL29-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL29-.Ltext0
	.4byte	.LVL31-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL31-.Ltext0
	.4byte	.LFE2-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	0
	.4byte	0
	.section	.debug_aranges,"",%progbits
	.4byte	0x1c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0
	.2byte	0
	.2byte	0
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	0
	.4byte	0
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.section	.debug_str,"MS",%progbits,1
.LASF46:
	.ascii	"plli2scfgr\000"
.LASF62:
	.ascii	"stm32f_gpio_mode\000"
.LASF1:
	.ascii	"short int\000"
.LASF50:
	.ascii	"STM32F_GPIOD_ID\000"
.LASF56:
	.ascii	"stmf32_gpio_id_t\000"
.LASF54:
	.ascii	"STM32F_GPIOH_ID\000"
.LASF31:
	.ascii	"ahb3enr\000"
.LASF57:
	.ascii	"INPUT\000"
.LASF45:
	.ascii	"sscgr\000"
.LASF38:
	.ascii	"ahb3lpenr\000"
.LASF15:
	.ascii	"lckr\000"
.LASF10:
	.ascii	"otyper\000"
.LASF37:
	.ascii	"hb2lpenr\000"
.LASF61:
	.ascii	"stm32f_gpio_clock_en\000"
.LASF24:
	.ascii	"ahb3rstr\000"
.LASF16:
	.ascii	"afrl\000"
.LASF2:
	.ascii	"long long int\000"
.LASF47:
	.ascii	"STM32F_GPIOA_ID\000"
.LASF20:
	.ascii	"pllcfgr\000"
.LASF11:
	.ascii	"ospeedr\000"
.LASF9:
	.ascii	"moder\000"
.LASF43:
	.ascii	"bdcr\000"
.LASF59:
	.ascii	"ALT_FUNC\000"
.LASF55:
	.ascii	"STM32F_GPIOI_ID\000"
.LASF26:
	.ascii	"apb1rstr\000"
.LASF21:
	.ascii	"cfgr\000"
.LASF3:
	.ascii	"unsigned char\000"
.LASF64:
	.ascii	"gpio\000"
.LASF18:
	.ascii	"stm32f_gpio\000"
.LASF0:
	.ascii	"signed char\000"
.LASF8:
	.ascii	"long long unsigned int\000"
.LASF6:
	.ascii	"uint32_t\000"
.LASF7:
	.ascii	"unsigned int\000"
.LASF41:
	.ascii	"apb2lpenr\000"
.LASF5:
	.ascii	"uint16_t\000"
.LASF33:
	.ascii	"apb1enr\000"
.LASF48:
	.ascii	"STM32F_GPIOB_ID\000"
.LASF22:
	.ascii	"ahb1rstr\000"
.LASF4:
	.ascii	"short unsigned int\000"
.LASF68:
	.ascii	"stm32f-gpio.c\000"
.LASF52:
	.ascii	"STM32F_GPIOF_ID\000"
.LASF29:
	.ascii	"ahb1enr\000"
.LASF60:
	.ascii	"ANALOG\000"
.LASF66:
	.ascii	"mode\000"
.LASF67:
	.ascii	"GNU C 4.6.2\000"
.LASF36:
	.ascii	"ahb1lpenr\000"
.LASF58:
	.ascii	"OUTPUT\000"
.LASF63:
	.ascii	"gpio_id\000"
.LASF65:
	.ascii	"port\000"
.LASF27:
	.ascii	"pb2rstr\000"
.LASF12:
	.ascii	"pupdr\000"
.LASF34:
	.ascii	"apb2enr\000"
.LASF17:
	.ascii	"afrh\000"
.LASF49:
	.ascii	"STM32F_GPIOC_ID\000"
.LASF14:
	.ascii	"bsrrh\000"
.LASF69:
	.ascii	"/home/bob/devel/uboot/src/stm32f\000"
.LASF13:
	.ascii	"bsrrl\000"
.LASF51:
	.ascii	"STM32F_GPIOE_ID\000"
.LASF53:
	.ascii	"STM32F_GPIOG_ID\000"
.LASF30:
	.ascii	"ahb2enr\000"
.LASF19:
	.ascii	"stm32f_rcc\000"
.LASF25:
	.ascii	"res0\000"
.LASF28:
	.ascii	"res1\000"
.LASF32:
	.ascii	"res2\000"
.LASF35:
	.ascii	"res3\000"
.LASF39:
	.ascii	"res4\000"
.LASF42:
	.ascii	"res5\000"
.LASF44:
	.ascii	"res6\000"
.LASF40:
	.ascii	"apb1lpenr\000"
.LASF23:
	.ascii	"ahb2rstr\000"
.LASF70:
	.ascii	"stm32f_gpio_af\000"
	.ident	"GCC: (GNU) 4.6.2"
